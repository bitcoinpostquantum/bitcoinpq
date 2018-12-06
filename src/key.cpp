// Copyright (c) 2009-2017 The Bitcoin Core developers
// Copyright (c) 2017 The Zcash developers
// Copyright (c) 2018 The Bitcoin Post-Quantum developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <key.h>

#include <arith_uint256.h>
#include <crypto/common.h>
#include <crypto/hmac_sha512.h>
#include <random.h>

#include <secp256k1.h>
#include <secp256k1_recovery.h>

#include <botan/auto_rng.h>
#include <botan/xmss.h>

#include <stdexcept>

#include <util.h>
#include <utilstrencodings.h>

#include "keystore.h"

bool CKey::force_signing = false;


void check_use_count(uint8_t const * key, size_t key_size, size_t use_count, size_t utxo_count)
{
    if (CKey::force_signing)
        return;
    
    auto info = bpqcrypto::get_key_info(key, key_size);
    
    if (info.is_xmss)
    {
        size_t max_use_count = 1u << info.tree_height;
        
        if (use_count >= max_use_count/2)
        {
            throw std::runtime_error("key use_count exeedes half limit");
        }

        if (utxo_count >= (max_use_count-use_count)/2)
        {
            throw std::runtime_error("key UTXO count exeedes half limit");
        }
    }
}

bool CKey::ECDSA_Check(const unsigned char *vch) 
{
	return ecdsa_check_private_key(vch);
}

void CKey::Set( uint8_t const * data, size_t size, CKeyStore * keystore )
{
	if ( bpqcrypto::is_xmss_short_key(data, size) )
	{
		keydata = bpqcrypto::xmss_get_long_key(secure_vector(data, data + size));
		fValid = true;
		fLegacy = false;
	}
	else if ( bpqcrypto::is_xmss_key(data, size) )
	{
		keydata.assign(data, data + size);
		fValid = true;
		fLegacy = false;
	}
	else if (bpqcrypto::is_ecdsa_key(data, size))
	{
		keydata.assign(data, data + 32);
		fCompressed = size == 33 && data[32] == 1;
		fValid = true;
		fLegacy = true;
	}
	else
	{
		//throw std::invalid_argument( strprintf("invalid key") );
		fValid = false;
	}
	
	if (keystore)
	{
		m_pKeyStore = keystore;
	}
}

void CKey::MakeNewKey(CKeyType ktype) 
{
	if (ktype == CKeyType::ECDSA_UNCOMPRESSED || ktype == CKeyType::ECDSA_COMPRESSED)
	{
		LogPrintf("%s: ECDSA\n", __func__);

		keydata.resize(ECDSA_INTERNAL_KEY_SIZE);
		do {
			GetStrongRandBytes(keydata.data(), keydata.size());
		} while (!ECDSA_Check(keydata.data()));
		fValid = true;
		fLegacy = true;
		fCompressed = ktype == CKeyType::ECDSA_COMPRESSED;
	}
	else
	{
		LogPrintf("%s: %s\n", __func__, KeyTypeToString(ktype));

        try
        {
			keydata = bpqcrypto::xmss_generate(ktype);
			
			fValid = true;
			fLegacy = false;
        }
        catch (std::exception &e)
        {
			LogPrintf("%s XMSS failed: %s\n", __func__, e.what());
            fValid = false;
        }
	}
}

unsigned int CKey::size() const
{
	return keydata.size();
}

bpqcrypto::secure_vector<uint8_t> CKey::raw_private_key_legacy() const
{
	if ( IsXMSS() )
		return {};
		
	bpqcrypto::secure_vector<uint8_t> k;

	k.reserve(33);
	k.assign( begin(), end());
	
	if (IsCompressed())
		k.push_back(1);
	
	return k;
}

size_t CKey::short_size() const
{
	if ( IsXMSS() )
	{
		return bpqcrypto::xmss_get_short_key_size(keydata);
	} else
	{
		return size();
	}
}

bpqcrypto::secure_vector<uint8_t> CKey::raw_short_key() const
{
	if ( IsXMSS() )
	{
		return bpqcrypto::xmss_get_short_key(keydata);
	} else
	{
		bpqcrypto::secure_vector<uint8_t> k;
		k.reserve(keydata.size() + 1);
		k.assign( keydata.begin(), keydata.end() );
		if (IsCompressed())
			k.push_back(1);
		return k;
	}
}

bpqcrypto::secure_vector<uint8_t> CKey::raw_private_key() const
{
	if ( IsXMSS() )
	{
		return keydata;
	} else
	{
		bpqcrypto::secure_vector<uint8_t> k;
		k.reserve(keydata.size() + 1);
		k.assign( keydata.begin(), keydata.end() );
		if (IsCompressed())
			k.push_back(1);
		return k;
	}
}

CPrivKey CKey::GetPrivKey() const 
{
    assert(fValid);

    if ( IsXMSS() )
	{
	    CPrivKey privkey;
		LogPrintf("%s: xmss key size: %d\n", __func__, keydata.size());

        // TODO: make in DER format
        //auto && privkey = raw_private_key();
        auto && rawkey = raw_private_key();
        privkey.assign(rawkey.begin(), rawkey.end());

		LogPrintf("%s: xmss privkey size: %d\n", __func__, privkey.size());

        assert(privkey.size() > 0);
		
	    return privkey;	
	}
	else
    {
		// ECDSA
		return ecdsa_private_export_der(keydata.data(), keydata.size(), fCompressed);
    }
}

CPubKey CKey::GetPubKey() const 
{
	if (!IsValid())
		return {};
		
	if ( IsXMSS() )
	{
		return CPubKey(bpqcrypto::xmss_get_pubkey(keydata));
	}
	else
	{
		CPubKey result(ecdsa_extract_public_key_from_private(
			keydata.data(), keydata.size(), fCompressed));
		return result;
	}
}

bool CKey::Sign(const uint8_t * msg, size_t msg_size, std::vector<unsigned char>& vchSig, uint32_t test_case) const 
{
    if (!fValid)
        return false;

    if ( IsXMSS() )
	{
        try
        {
			size_t use_count = 0;
			CKeyID keyid = GetPubKey().GetID();
			
			if (m_pKeyStore)
            {
				use_count = m_pKeyStore->GetKeyUseCountInc(keyid);
                
                // TODO: get key TXO count
                size_t txo_count = 0;
                check_use_count(keydata.data(), keydata.size(), use_count, txo_count);
            }

			vchSig = bpqcrypto::xmss_sign(msg, msg_size, keydata.data(), keydata.size(), use_count);
			
			if (m_pKeyStore)
				m_pKeyStore->SetKeyUseCount(keyid, use_count);

			LogPrintf("%s: xmss: msg size: %d, key size: %d, signature size: %d, use_count: %d\n", 
				__func__, msg_size, keydata.size(), vchSig.size(), use_count );
			
            return true;
        }
        catch (std::exception &e)
        {
			LogPrintf("%s: [EX]: %s\n", __func__, e.what());
			throw;
            return false;
        }
	}
	else
    {
		uint256 hash;
		if (msg_size == 1 && msg[0] == 1 )
		{
			// special case
		    hash = uint256S("0000000000000000000000000000000000000000000000000000000000000001");
		} else
		{
			hash = Hash(msg, msg + msg_size);
		}
		vchSig = ecdsa_sign(keydata.data(), keydata.size(), hash, test_case);
		return true;
    }

    return false;
}

bool CKey::SignHash(const uint256 &hash, std::vector<unsigned char>& vchSig, uint32_t test_case) const 
{
    if (!fValid)
        return false;

    if ( IsXMSS() )
	{
		//throw std::invalid_argument("xmss_sign does not accept hash as input");
        
        size_t use_count = 0;
        CKeyID keyid = GetPubKey().GetID();

        if (m_pKeyStore)
        {
            use_count = m_pKeyStore->GetKeyUseCountInc(keyid);
            
            // TODO: get key TXO count
            size_t txo_count = 0;
            check_use_count(keydata.data(), keydata.size(), use_count, txo_count);
        }

        vchSig = bpqcrypto::xmss_sign(&hash.begin()[0], hash.size(), 
                keydata.data(), keydata.size(), use_count);

        if (m_pKeyStore)
            m_pKeyStore->SetKeyUseCount(keyid, use_count);
     
        return true;
	}
	else
    {
		vchSig = ecdsa_sign(keydata.data(), keydata.size(), hash, test_case);
		return true;
    }

    return false;
}

bool CKey::VerifyPubKey(const CPubKey& pubkey) const 
{
	std::string str = "BPQ key verification\n";

	if ( IsXMSS() )
	{
        return GetPubKey() == pubkey;
        
		if ( !pubkey.IsXMSS() )
			return false;
		
		// TODO: make special implementation
		
		unsigned char rnd[8];
		GetRandBytes(rnd, sizeof(rnd));
		uint256 hash;
		CHash256()
			.Write((unsigned char*)str.data(), str.size())
			.Write(rnd, sizeof(rnd)).Finalize(hash.begin());

		std::vector<unsigned char> vchSig;
		Sign(hash, vchSig);

		return pubkey.Verify(hash, vchSig);
	}
	else
	{
		if (pubkey.IsCompressed() != fCompressed) {
			return false;
		}
		unsigned char rnd[8];
		GetRandBytes(rnd, sizeof(rnd));
		uint256 hash;
		CHash256().Write((unsigned char*)str.data(), str.size()).Write(rnd, sizeof(rnd)).Finalize(hash.begin());
		std::vector<unsigned char> vchSig;
		SignHash(hash, vchSig);
		return pubkey.VerifyHash(hash, vchSig);
	}
}

bool CKey::SignCompact(std::string const & msg, std::vector<unsigned char>& vchSig) const 
{
    if (!fValid)
        return false;
	
    if ( IsXMSS() )
    {
		size_t use_count = 0;
		CKeyID keyid = GetPubKey().GetID();
		
		if (m_pKeyStore)
			use_count = m_pKeyStore->GetKeyUseCountInc(keyid);

		vchSig = xmss_sign_compact(keydata, use_count, (uint8_t const*)msg.data(), msg.size());

		if (m_pKeyStore)
			m_pKeyStore->SetKeyUseCount(keyid, use_count);

		return true;
    }
    else
    {
		
		vchSig = ecdsa_sign_compact(keydata.data(), keydata.size(), fCompressed, 
				Hash(msg.begin(), msg.end()));
		
        return true;
    }	
}

bool CKey::SignCompact(const uint256 &hash, std::vector<unsigned char>& vchSig) const 
{
    if (!fValid)
        return false;

    if ( IsXMSS() )
    {
        return SignHash(hash, vchSig);
    }
    else
    {
		vchSig = ecdsa_sign_compact(keydata.data(), keydata.size(), fCompressed, hash);
        return true;
    }	
    if (!fValid)
        return false;
}

bool CKey::Load(const CPrivKey &privkey, const CPubKey &vchPubKey, bool fSkipCheck=false) 
{
    if ( vchPubKey.IsXMSS() )
	{
        // XMSS
		LogPrintf("%s: xmss pkey: %d bytes, pubkey: %s\n", __func__,
				privkey.size(), HexStr(vchPubKey).c_str());

        Set(privkey.data(), privkey.size());
	}
	else
    {
		LogPrintf("%s: legacy pkey: %d bytes, pubkey: %s\n", __func__, 
				privkey.size(), HexStr(vchPubKey).c_str());
		
        // ECDSA
		
		auto key = ecdsa_import_der_privkey(privkey);
        if (key.empty())
		{
			LogPrintf("CKey::Load: ec_privkey_import_der failed\n");
            return false;
		}
		keydata.assign(key.begin(), key.end());

        fCompressed = vchPubKey.IsCompressed();
        fValid = true;
		fLegacy = true;
    }

    if (fSkipCheck)
        return true;

    return VerifyPubKey(vchPubKey);
}

bool CKey::Derive(CKey& keyChild, ChainCode &ccChild, unsigned int nChild, const ChainCode& cc) const 
{
    if ( !IsXMSS() )
    {
        assert(IsValid());
        assert(IsCompressed());
        std::vector<unsigned char, secure_allocator<unsigned char>> vout(64);
        if ((nChild >> 31) == 0) {
            CPubKey pubkey = GetPubKey();
            assert(pubkey.size() == CPubKey::ECDSA_COMPRESSED_PUBLIC_KEY_SIZE);
            BIP32Hash(cc, nChild, *pubkey.begin(), pubkey.begin()+1, vout.data());
        } else {
            assert(size() == 32);
            BIP32Hash(cc, nChild, 0, begin(), vout.data());
        }
        memcpy(ccChild.begin(), vout.data()+32, 32);
        memcpy((unsigned char*)keyChild.begin(), begin(), 32);
		
		bool ret = ecdsa_privkey_tweak_add((unsigned char*)keyChild.begin(), vout);
		
        keyChild.fLegacy = true;
        keyChild.fCompressed = true;
        keyChild.fValid = ret;
        return ret;
    }
    else
    {
        // XMSS: not implemented
        return false;
    }	
}

bool CExtKey::Derive(CExtKey &out, unsigned int _nChild) const {
    out.nDepth = nDepth + 1;
    CKeyID id = key.GetPubKey().GetID();
    memcpy(&out.vchFingerprint[0], &id, 4);
    out.nChild = _nChild;
    return key.Derive(out.key, out.chaincode, _nChild, chaincode);
}

void CExtKey::SetMaster(const unsigned char *seed, unsigned int nSeedLen) 
{
//    static const unsigned char hashkey[] = {'B','p','q','_','b','p','q',' ','s','e','e','d'};
    static const unsigned char hashkey[] = {'B','i','t','c','o','i','n',' ','s','e','e','d'};
    std::vector<unsigned char, secure_allocator<unsigned char>> vout(64);
    CHMAC_SHA512(hashkey, sizeof(hashkey)).Write(seed, nSeedLen).Finalize(vout.data());
    key.SetLegacy(vout.data(), vout.data() + 32, true);
    memcpy(chaincode.begin(), vout.data() + 32, 32);
    nDepth = 0;
    nChild = 0;
    memset(vchFingerprint, 0, sizeof(vchFingerprint));
}

CExtPubKey CExtKey::Neuter() const 
{
    CExtPubKey ret;
    ret.nDepth = nDepth;
    memcpy(&ret.vchFingerprint[0], &vchFingerprint[0], 4);
    ret.nChild = nChild;
    ret.pubkey = key.GetPubKey();
    ret.chaincode = chaincode;
    return ret;
}

void CExtKey::Encode(unsigned char code[BIP32_EXTKEY_SIZE]) const {
    code[0] = nDepth;
    memcpy(code+1, vchFingerprint, 4);
    code[5] = (nChild >> 24) & 0xFF; code[6] = (nChild >> 16) & 0xFF;
    code[7] = (nChild >>  8) & 0xFF; code[8] = (nChild >>  0) & 0xFF;
    memcpy(code+9, chaincode.begin(), 32);
    code[41] = 0;
    assert(key.size() == 32);
    memcpy(code+42, key.begin(), 32);
}

void CExtKey::Decode(const unsigned char code[BIP32_EXTKEY_SIZE]) {
    nDepth = code[0];
    memcpy(vchFingerprint, code+1, 4);
    nChild = (code[5] << 24) | (code[6] << 16) | (code[7] << 8) | code[8];
    memcpy(chaincode.begin(), code+9, 32);
    key.SetLegacy(code+42, code+BIP32_EXTKEY_SIZE, true);
}

bool ECC_InitSanityCheck() {
    CKey key;
    key.MakeNewKey(CKeyType::ECDSA_COMPRESSED);
    CPubKey pubkey = key.GetPubKey();
    return key.VerifyPubKey(pubkey);
}

