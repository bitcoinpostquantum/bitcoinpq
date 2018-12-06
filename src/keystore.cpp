// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Copyright (c) 2018 The Bitcoin Post-Quantum developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <keystore.h>

#include <util.h>

#include "base58.h"

bool CKeyStore::AddKey(const CKey &key) {
    return AddKeyPubKey(key, key.GetPubKey());
}

void CBasicKeyStore::ImplicitlyLearnRelatedKeyScripts(const CPubKey& pubkey)
{
    AssertLockHeld(cs_KeyStore);
    CKeyID key_id = pubkey.GetID();
    // We must actually know about this key already.
    assert(HaveKey(key_id) || mapWatchKeys.count(key_id));
    // This adds the redeemscripts necessary to detect P2WPKH and P2SH-P2WPKH
    // outputs. Technically P2WPKH outputs don't have a redeemscript to be
    // spent. However, our current IsMine logic requires the corresponding
    // P2SH-P2WPKH redeemscript to be present in the wallet in order to accept
    // payment even to P2WPKH outputs.
    // Also note that having superfluous scripts in the keystore never hurts.
    // They're only used to guide recursion in signing and IsMine logic - if
    // a script is present but we can't do anything with it, it has no effect.
    // "Implicitly" refers to fact that scripts are derived automatically from
    // existing keys, and are present in memory, even without being explicitly
    // loaded (e.g. from a file).
    
    if (pubkey.IsXMSS())
    {
        CScript reedemscript = GetScriptForRawPubKey(pubkey);
        CScript script = GetScriptForDestination(WitnessV1ScriptHash(reedemscript));
        
        mapScripts[CScriptID(reedemscript, 1)] = std::move(reedemscript);
        mapScripts[CScriptID(script, 1)] = std::move(script);
        
    } else
    {
        if (is_key_segwit_useable(pubkey)) {
            CScript script = GetScriptForDestination(WitnessV0KeyHash(key_id));
            // This does not use AddCScript, as it may be overridden.
            mapScripts[CScriptID(script, 0)] = std::move(script);
        }
    }
}

bool CBasicKeyStore::GetPubKey(const CKeyID &address, CPubKey &vchPubKeyOut) const
{
    CKey key;
    if (!GetKey(address, key)) {
        LOCK(cs_KeyStore);
        WatchKeyMap::const_iterator it = mapWatchKeys.find(address);
        if (it != mapWatchKeys.end()) {
            vchPubKeyOut = it->second;
            return true;
        }
        return false;
    }
    vchPubKeyOut = key.GetPubKey();
    return true;
}

bool CBasicKeyStore::AddKeyPubKey(const CKey& key, const CPubKey &pubkey)
{
    LOCK(cs_KeyStore);
    mapKeys[pubkey.GetID()] = key;
    ImplicitlyLearnRelatedKeyScripts(pubkey);
    return true;
}

bool CBasicKeyStore::HaveKey(const CKeyID &address) const
{
    LOCK(cs_KeyStore);
    return mapKeys.count(address) > 0;
}

std::set<CKeyID> CBasicKeyStore::GetKeys() const
{
    LOCK(cs_KeyStore);
    std::set<CKeyID> set_address;
    for (const auto& mi : mapKeys) {
        set_address.insert(mi.first);
    }
    return set_address;
}

bool CBasicKeyStore::GetKey(const CKeyID &address, CKey &keyOut) const
{
    LOCK(cs_KeyStore);
    KeyMap::const_iterator mi = mapKeys.find(address);
    if (mi != mapKeys.end()) {
        keyOut = mi->second;
		keyOut.AssignKeyStore(const_cast<CBasicKeyStore*>(this));
        return true;
    }
    return false;
}

std::pair<size_t,size_t> CBasicKeyStore::GetKeyUseCount(const CTxDestination &dest) const
{
    LOCK(cs_KeyStore);
	
	CKeyID address = GetKeyForDestination(*this, dest);
	if (address.IsNull())
		return {0,0};
	
	auto it = mapKeyUseCount.find(address);
	if ( it == mapKeyUseCount.end())
		return {0,0};
	
	CPubKey pubkey;
	if (!GetPubKey(address, pubkey))
	{
	    LogPrintf("%s: !!! pubkey for keyid not found, db storage corrupted\n", __func__);
		return std::make_pair(it->second,0);
	}
	
	if (pubkey.IsXMSS())
	{
		return std::make_pair(it->second, pubkey.GetMaxUseCount());
	}

	return std::make_pair(it->second,0);
}

uint64_t CBasicKeyStore::GetKeyUseCount(const CKeyID &address) const
{
    LOCK(cs_KeyStore);

	auto it = mapKeyUseCount.find(address);
	if ( it != mapKeyUseCount.end())
		return it->second;

	return 0;
}

uint64_t CBasicKeyStore::GetKeyUseCountInc(const CKeyID &address)
{
    LOCK(cs_KeyStore);

	auto it = mapKeyUseCount.find(address);
	if ( it != mapKeyUseCount.end())
	{
		return it->second++;
	}

	return 0;
}

bool CBasicKeyStore::SetKeyUseCount(const CKeyID &address, uint64_t use_count)
{
    LOCK(cs_KeyStore);

	auto & iref = mapKeyUseCount[address];
	
	if (use_count <= iref)
	{
	    LogPrintf("%s: %u -> %u, skip\n", __func__, iref, use_count);
		return false;
	}
	
    LogPrintf("%s: %u -> %u\n", __func__, iref, use_count);
	
	iref = use_count;
	
	// TODO: set dirty to update database, 
	
	return true;
}

bool CBasicKeyStore::AddCScript(const CScript& redeemScript, int version)
{
    if (redeemScript.size() > MAX_SCRIPT_ELEMENT_SIZE)
        return error("CBasicKeyStore::AddCScript(): redeemScripts > %i bytes are invalid", MAX_SCRIPT_ELEMENT_SIZE);

    LOCK(cs_KeyStore);
    mapScripts[CScriptID(redeemScript, version)] = redeemScript;
    return true;
}

bool CBasicKeyStore::HaveCScript(const CScriptID& hash) const
{
    LOCK(cs_KeyStore);
    return mapScripts.count(hash) > 0;
}

std::set<CScriptID> CBasicKeyStore::GetCScripts() const
{
    LOCK(cs_KeyStore);
    std::set<CScriptID> set_script;
    for (const auto& mi : mapScripts) {
        set_script.insert(mi.first);
    }
    return set_script;
}

bool CBasicKeyStore::GetCScript(const CScriptID &hash, CScript& redeemScriptOut) const
{
    LOCK(cs_KeyStore);
    ScriptMap::const_iterator mi = mapScripts.find(hash);
    if (mi != mapScripts.end())
    {
        redeemScriptOut = (*mi).second;
        return true;
    }
    return false;
}

bool CBasicKeyStore::AddWatchOnly(const CScript &dest, int version)
{
    LOCK(cs_KeyStore);
    setWatchOnly.insert(dest);
    CPubKey pubKey;
    if (ExtractPubKey(dest, pubKey)) {
        mapWatchKeys[pubKey.GetID()] = pubKey;
        ImplicitlyLearnRelatedKeyScripts(pubKey);
    }
    return true;
}

bool CBasicKeyStore::RemoveWatchOnly(const CScript &dest)
{
    LOCK(cs_KeyStore);
    setWatchOnly.erase(dest);
    CPubKey pubKey;
    if (ExtractPubKey(dest, pubKey)) {
        mapWatchKeys.erase(pubKey.GetID());
    }
    // Related CScripts are not removed; having superfluous scripts around is
    // harmless (see comment in ImplicitlyLearnRelatedKeyScripts).
    return true;
}

bool CBasicKeyStore::HaveWatchOnly(const CScript &dest) const
{
    LOCK(cs_KeyStore);
    return setWatchOnly.count(dest) > 0;
}

bool CBasicKeyStore::HaveWatchOnly() const
{
    LOCK(cs_KeyStore);
    return (!setWatchOnly.empty());
}

CKeyID GetKeyForDestination(const CKeyStore& store, const CTxDestination& dest)
{
    // Only supports destinations which map to single public keys, i.e. P2PKH,
    // P2WPKH, and P2SH-P2WPKH.
    if (auto id = boost::get<CKeyID>(&dest)) {
        return *id;
    }
    if (auto witness_id = boost::get<WitnessV0KeyHash>(&dest)) {
        return CKeyID(*witness_id);
    }
    if (auto script_id = boost::get<CScriptID>(&dest)) {
        CScript script;
        CTxDestination inner_dest;
        if (store.GetCScript(*script_id, script) && ExtractDestination(script, inner_dest)) {
            if (auto inner_witness_id = boost::get<WitnessV0KeyHash>(&inner_dest)) {
                return CKeyID(*inner_witness_id);
            }
        }
    }
    
    if (auto witness_id = boost::get<WitnessV0ScriptHash>(&dest)) {
        CScriptID script_id(*witness_id);
        CScript script;
        CPubKey pubKey;
        if (store.GetCScript(script_id, script) && ExtractPubKey(script, pubKey))
        {
            return pubKey.GetID();
        }
    }
    
    if (auto witness_id = boost::get<WitnessV1ScriptHash>(&dest)) {
        CScriptID script_id(*witness_id);
        CScript script;
        CPubKey pubKey;
        if (store.GetCScript(script_id, script) && ExtractPubKey(script, pubKey))
        {
            return pubKey.GetID();
        }
    }
    
    return CKeyID();
}
