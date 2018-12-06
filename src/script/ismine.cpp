// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Copyright (c) 2018 The Bitcoin Post-Quantum developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <script/ismine.h>

#include <key.h>
#include <keystore.h>
#include <script/script.h>
#include <script/sign.h>


typedef std::vector<unsigned char> valtype;

unsigned int HaveKeys(const std::vector<valtype>& pubkeys, const CKeyStore& keystore)
{
    unsigned int nResult = 0;
    for (const valtype& pubkey : pubkeys)
    {
        CKeyID keyID = CPubKey(pubkey).GetID();
        if (keystore.HaveKey(keyID))
            ++nResult;
    }
    return nResult;
}

isminetype IsMine(const CKeyStore& keystore, const CScript& scriptPubKey, SigVersion sigversion)
{
    bool isInvalid = false;
	bool isBitcoin = false;
    return IsMine(keystore, scriptPubKey, isInvalid, isBitcoin, sigversion);
}

isminetype IsMine(const CKeyStore& keystore, const CTxDestination& dest, SigVersion sigversion)
{
    bool isInvalid = false;
	bool isBitcoin = false;
    return IsMine(keystore, dest, isInvalid, isBitcoin, sigversion);
}

isminetype IsMine(const CKeyStore &keystore, const CTxDestination& dest, bool& isInvalid, bool & isBitcoin, SigVersion sigversion)
{
    CScript script = GetScriptForDestination(dest);
    return IsMine(keystore, script, isInvalid, isBitcoin, sigversion);
}

isminetype IsMine(const CKeyStore &keystore, const CTxDestination& dest, bool& isInvalid, SigVersion sigversion)
{
	bool isBitcoin = false;
    CScript script = GetScriptForDestination(dest);
    return IsMine(keystore, script, isInvalid, isBitcoin, sigversion);
}

isminetype IsMine(const CKeyStore &keystore, const CScript& scriptPubKey, bool& isInvalid, SigVersion sigversion)
{
	bool isBitcoin = false;
    return IsMine(keystore, scriptPubKey, isInvalid, isBitcoin, sigversion);
}

isminetype IsMine(const CKeyStore &keystore, const CScript& scriptPubKey, bool& isInvalid, bool & isBitcoin, SigVersion sigversion)
{
    isInvalid = false;

    std::vector<valtype> vSolutions;
    txnouttype whichType;
    if (!Solver(scriptPubKey, whichType, vSolutions)) {
        if (keystore.HaveWatchOnly(scriptPubKey))
            return ISMINE_WATCH_UNSOLVABLE;
        return ISMINE_NO;
    }

    CKeyID keyID;
    switch (whichType)
    {
    case TX_NONSTANDARD:
    case TX_NULL_DATA:
    case TX_WITNESS_UNKNOWN:
        break;
    case TX_PUBKEY:

        if (sigversion != SIGVERSION_BASE && !bpqcrypto::check_pubkey(vSolutions[0]))
        {
            isInvalid = true;
            return ISMINE_NO;
        }

        keyID = CPubKey(vSolutions[0]).GetID();

        if (keystore.HaveKey(keyID))
		{
			CPubKey pubkey;
			if (isBitcoin)
				isBitcoin = keystore.GetPubKey(keyID, pubkey) && pubkey.IsValid() && !pubkey.IsXMSS();
			
            return ISMINE_SPENDABLE;
		}
        break;
    case TX_WITNESS_V0_KEYHASH:
    {
        if (!keystore.HaveCScript(CScriptID(CScript() << OP_0 << vSolutions[0], 0))) {
            // We do not support bare witness outputs unless the P2SH version of it would be
            // acceptable as well. This protects against matching before segwit activates.
            // This also applies to the P2WSH case.
            break;
        }
        isminetype ret = ::IsMine(keystore, GetScriptForDestination(CKeyID(uint160(vSolutions[0]))), 
                isInvalid, SIGVERSION_WITNESS_V0);
		
        if (ret == ISMINE_SPENDABLE || ret == ISMINE_WATCH_SOLVABLE || (ret == ISMINE_NO && isInvalid))
		{
			CPubKey pubkey;
			if (isBitcoin)
				isBitcoin = keystore.GetPubKey(keyID, pubkey) && pubkey.IsValid() && !pubkey.IsXMSS();
			
            return ret;
		}
        break;
    }
    case TX_PUBKEYHASH:
        keyID = CKeyID(uint160(vSolutions[0]));
        if (sigversion != SIGVERSION_BASE) {
            CPubKey pubkey;
            if (keystore.GetPubKey(keyID, pubkey) && !is_key_segwit_useable(pubkey)) {
                isInvalid = true;
                return ISMINE_NO;
            }
        }
        if (keystore.HaveKey(keyID))
            return ISMINE_SPENDABLE;
        break;
    case TX_SCRIPTHASH:
    {
        CScriptID scriptID = CScriptID(uint160(vSolutions[0]));
        CScript subscript;
        if (keystore.GetCScript(scriptID, subscript)) {
            isminetype ret = IsMine(keystore, subscript, isInvalid);
			
            if (ret == ISMINE_SPENDABLE || ret == ISMINE_WATCH_SOLVABLE || (ret == ISMINE_NO && isInvalid))
			{
				CPubKey pubkey;
				if (isBitcoin)
					isBitcoin = keystore.GetPubKey(keyID, pubkey) && pubkey.IsValid() && !pubkey.IsXMSS();
				
                return ret;
			}
        }
        break;
    }
    case TX_WITNESS_V0_SCRIPTHASH:
    {
        WitnessV0ScriptHash dest(vSolutions[0]);
        CScript script = GetScriptForDestination(dest);
        if (!keystore.HaveCScript(CScriptID(script, 0))) {
            break;
        }
        
        CScriptID scriptID = CScriptID(dest);
        CScript subscript;
        
        if (keystore.GetCScript(scriptID, subscript)) 
        {
            isminetype ret = IsMine(keystore, subscript, isInvalid, SIGVERSION_WITNESS_V0);
			
            if (ret == ISMINE_SPENDABLE || ret == ISMINE_WATCH_SOLVABLE || (ret == ISMINE_NO && isInvalid))
			{
				CPubKey pubkey;
				if (isBitcoin)
					isBitcoin = keystore.GetPubKey(keyID, pubkey) && pubkey.IsValid() && !pubkey.IsXMSS();
				
                return ret;
			}
        }
        break;
    }
    
    case TX_WITNESS_V1_SCRIPTHASH:
    {
        WitnessV1ScriptHash dest(vSolutions[0]);
        // TODO: check
        CScript script = GetScriptForDestination(dest);
        if (!keystore.HaveCScript(CScriptID(script, 1))) {
            break;
        }

        CScriptID scriptID = CScriptID(dest);
        CScript subscript;
        
        if (keystore.GetCScript(scriptID, subscript)) {
            isminetype ret = IsMine(keystore, subscript, isInvalid, SIGVERSION_WITNESS_V1);
			
            if (ret == ISMINE_SPENDABLE || ret == ISMINE_WATCH_SOLVABLE || (ret == ISMINE_NO && isInvalid))
			{
				CPubKey pubkey;
				if (isBitcoin)
					isBitcoin = keystore.GetPubKey(keyID, pubkey) && pubkey.IsValid() && !pubkey.IsXMSS();
				
                return ret;
			}
        }
        break;
    }

    case TX_MULTISIG:
    {
        // Only consider transactions "mine" if we own ALL the
        // keys involved. Multi-signature transactions that are
        // partially owned (somebody else has a key that can spend
        // them) enable spend-out-from-under-you attacks, especially
        // in shared-wallet situations.
        std::vector<valtype> keys(vSolutions.begin()+1, vSolutions.begin()+vSolutions.size()-1);
        if (sigversion != SIGVERSION_BASE) {
            for (size_t i = 0; i < keys.size(); i++) {
                if (keys[i].size() != 33) {
                    isInvalid = true;
                    return ISMINE_NO;
                }
            }
        }
        if (HaveKeys(keys, keystore) == keys.size())
            return ISMINE_SPENDABLE;
        break;
    }
    }

    if (keystore.HaveWatchOnly(scriptPubKey)) {
        // TODO: This could be optimized some by doing some work after the above solver
        SignatureData sigs;
        return ProduceSignature(DummySignatureCreator(&keystore), scriptPubKey, sigs) ? ISMINE_WATCH_SOLVABLE : ISMINE_WATCH_UNSOLVABLE;
    }
    return ISMINE_NO;
}
