// Copyright (c) 2009-2017 The Bitcoin Core developers
// Copyright (c) 2017 The Zcash developers
// Copyright (c) 2018 The Bitcoin Post-Quantum developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <pubkey.h>

#include <secp256k1.h>
#include <secp256k1_recovery.h>

//#include <botan/auto_rng.h>
//#include <botan/xmss.h>

#include "key.h"
#include "script/standard.h"
#include <util.h>

std::string KeyTypeToString(CKeyType keytype)
{
	switch (keytype)
	{
	case CKeyType::ECDSA_UNCOMPRESSED: return "ECDSA_UNCOMPRESSED";
	case CKeyType::ECDSA_COMPRESSED: return "ECDSA_COMPRESSED";
	case CKeyType::XMSS_256_H10: return "XMSS_256_H10";
	case CKeyType::XMSS_256_H16: return "XMSS_256_H16";
	case CKeyType::XMSS_256_H20: return "XMSS_256_H20";
	default:
		throw std::invalid_argument(__func__);
	}
}

CKeyType CPubKey::GetKeyType() const
{
	if (vch.empty())
		throw std::runtime_error(__func__);
	
	switch (vch[0])
	{
	case 2:
	case 3:
		return CKeyType::ECDSA_COMPRESSED;

	case 4:
	case 6:
	case 7:
		return CKeyType::ECDSA_UNCOMPRESSED;

	case HDR_XMSS_V1:
		{
			auto info = bpqcrypto::get_key_info(vch.data(), vch.size());
			
			if (info.keytype == CKeyType::UNDEFINED)
				throw std::runtime_error(__func__);
			
			return info.keytype;
		}
	
	default:
		throw std::runtime_error(__func__);
	}
}

size_t CPubKey::GetMaxUseCount() const
{
    auto info = bpqcrypto::get_key_info(vch.data(), vch.size());

    if (!info.is_xmss)
        return 0;

    size_t max_use_count = 1u << info.tree_height;
    return max_use_count;
}

void CPubKey::Set(uint8_t const * data, size_t size)
{
	if ( bpqcrypto::check_pubkey(data, size) )
	{
		vch.assign(data, data + size);
	}
	else
	{
		Invalidate();
	}
}

bool CPubKey::Verify(const uint8_t * msg, size_t msg_size, const std::vector<unsigned char>& vchSig) const 
{
	if (!IsValid())
		return false;

	if ( IsXMSS() )
	{
		return bpqcrypto::xmss_verify(msg, msg_size, vchSig, vch);
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
		
		return ecdsa_verify(vchSig, hash, vch);
	}
}

bool CPubKey::VerifyHash(const uint256 &hash, const std::vector<unsigned char>& vchSig) const 
{
	if (!IsValid())
		return false;

	if ( IsXMSS() )
	{
		throw std::invalid_argument("xmss_sign does not accept hash as input");
		//return xmss_verify(vchSig, vch, hash.begin(), hash.size());
	}
	else
	{
		return ecdsa_verify(vchSig, hash, vch);
	}
}

bool CPubKey::RecoverCompact(std::string const & msg, const std::vector<unsigned char>& vchSig) 
{
	std::vector<unsigned char> pubkey;
	pubkey = xmss_recover_compact(vchSig, (uint8_t const*)msg.data(), msg.size());

	if (pubkey.empty())
	{
		// ECDSA
		if (!ecdsa_recover_compact(vchSig, Hash(msg.begin(), msg.end()), pubkey))
			return false;
	}
	
	Set(pubkey.begin(), pubkey.end());
	return IsValid();
}

bool CPubKey::RecoverCompact(const uint256 &hash, const std::vector<unsigned char>& vchSig) 
{
	std::vector<unsigned char> pubkey;
	pubkey = xmss_recover_compact(vchSig, (uint8_t const*)hash.begin(), hash.size());

	if (pubkey.empty())
	{
		// ECDSA
		if (!ecdsa_recover_compact(vchSig, hash, pubkey))
			return false;
	}
	
	Set(pubkey.begin(), pubkey.end());
	return IsValid();
}

bool CPubKey::IsFullyValid() const 
{
	if (!IsValid())
		return false;
	
	if (IsXMSS())
	{
		return bpqcrypto::is_xmss_pubkey(vch.data(), size());
	}
	else
	{
		return ecdsa_check_pubkey(vch.data(), vch.size());
	}
}

bool CPubKey::Decompress() 
{
	if (!IsValid())
        return false;
	
	if ( IsXMSS() )
	{
        // xmss: not implemented
        return false;
	}
	else
	{
		std::vector<unsigned char> pubkey;
		if (!ecdsa_decompress(vch.data(), size(), pubkey))
			return false;
		
		Set(pubkey.begin(), pubkey.end());
		return IsValid();
	}
}

bool CPubKey::Derive(CPubKey& pubkeyChild, 
	ChainCode &ccChild, unsigned int nChild, const ChainCode& cc) const 
{
    assert(IsValid());

    if ( !IsXMSS() )
    {
		std::vector<unsigned char> pubkey;
		if (!ecdsa_derive(vch.data(), vch.size(), ccChild, nChild, cc, pubkey))
			return false;
		
		pubkeyChild.Set(pubkey.begin(), pubkey.end());
		return pubkeyChild.IsValid();
    }
    else
    {
        // xmss: not implemented
        return false;
    }	
}

void CExtPubKey::Encode(unsigned char code[BIP32_EXTKEY_SIZE]) const {
    code[0] = nDepth;
    memcpy(code+1, vchFingerprint, 4);
    code[5] = (nChild >> 24) & 0xFF; code[6] = (nChild >> 16) & 0xFF;
    code[7] = (nChild >>  8) & 0xFF; code[8] = (nChild >>  0) & 0xFF;
    memcpy(code+9, chaincode.begin(), 32);
    assert(pubkey.size() == CPubKey::ECDSA_COMPRESSED_PUBLIC_KEY_SIZE);
    memcpy(code+41, pubkey.begin(), CPubKey::ECDSA_COMPRESSED_PUBLIC_KEY_SIZE);
}

void CExtPubKey::Decode(const unsigned char code[BIP32_EXTKEY_SIZE]) {
    nDepth = code[0];
    memcpy(vchFingerprint, code+1, 4);
    nChild = (code[5] << 24) | (code[6] << 16) | (code[7] << 8) | code[8];
    memcpy(chaincode.begin(), code+9, 32);
    pubkey.Set(code+41, code+BIP32_EXTKEY_SIZE);
}

bool CExtPubKey::Derive(CExtPubKey &out, unsigned int _nChild) const {
    out.nDepth = nDepth + 1;
    CKeyID id = pubkey.GetID();
    memcpy(&out.vchFingerprint[0], &id, 4);
    out.nChild = _nChild;
    return pubkey.Derive(out.pubkey, out.chaincode, _nChild, chaincode);
}

