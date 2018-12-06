// Copyright (c) 2018 The Bitcoin Post-Quantum developers

#include "crypt_xmss.h"

//#include <botan/xmss_parameters.h>
//#include <botan/xmss_publickey.h>
//#include <botan/xmss_privatekey.h>
//#include <botan/auto_rng.h>
//#include <botan/der_enc.h>
//#include <botan/ber_dec.h>

#include <util.h>
#include <utilstrencodings.h>

using namespace bpqcrypto;

namespace bpqcrypto {

	size_t xmss_get_max_use_count(uint8_t const * pubkey, size_t pubkey_size) noexcept
	{
		KeyInfo ki = get_key_info(pubkey, pubkey_size);
		return size_t(1u) << ki.tree_height;
	}
	
} // namespace bpqcrypto

bool xmss_equal(Botan::secure_vector<uint8_t> const & a, Botan::secure_vector<uint8_t> const & b)
{
	size_t len = xmss_get_short_key_size(a);
	return len <= b.size() && std::equal(a.data(), a.data() + len, b.data());
}

std::vector<unsigned char> xmss_sign_compact(
		Botan::secure_vector<uint8_t> const & key, 
		size_t & leaf_index,
		uint8_t const * message, size_t message_size)
{
	auto sig = xmss_sign(message, message_size, key, leaf_index);
	
	auto && pubkey = xmss_get_pubkey(key);
	
	std::vector<unsigned char> sign_compact;
	sign_compact.reserve(pubkey.size() + sig.size());
	
	sign_compact.insert(sign_compact.end(), pubkey.begin(), pubkey.end());
	sign_compact.insert(sign_compact.end(), sig.begin(), sig.end());
	
	return sign_compact;
}

std::vector<uint8_t> 
xmss_recover_compact(std::vector<unsigned char> const & compact_sig, 
	uint8_t const * message, size_t message_size)
{
	try
	{
		auto && pubkey = xmss_get_pubkey(compact_sig.data(), compact_sig.size());
		
		std::vector<uint8_t> sig(compact_sig.begin() + pubkey.size(), compact_sig.end());

		if (!xmss_verify(message, message_size, sig, pubkey))
			return {};

		return pubkey;
	}
	catch (...)
	{
		return {};
	}
}

/*
uint64_t xmss_get_leaf_index( uint8_t const * privkey, size_t size)
{
	auto params = xmss_extract_params_from_key(privkey, size);
	
	size_t pub_size = xmss_get_public_key_size(params);
	size_t priv_size = xmss_get_private_key_size(params);
	
	if (size < priv_size)
		throw std::invalid_argument("xmss_get_leaf_index: invalid private key");
	
	uint64_t leaf_index = 0;
	for(int i = 0; i < 8; i--)
	{
		leaf_index = (leaf_index << 8) | privkey[pub_size + i];
	}
		
	return leaf_index;
}

void xmss_set_leaf_index( uint8_t * privkey, size_t size, uint64_t leaf_index)
{
	auto params = xmss_extract_params_from_key(privkey, size);
	size_t pub_size = xmss_get_public_key_size(params);
	size_t priv_size = xmss_get_private_key_size(params);
	
	if (size < priv_size)
		throw std::invalid_argument("xmss_set_leaf_index: invalid private key");
	
	for(int i = 7; i >= 0; i--)
	{
		privkey[pub_size + i] = static_cast<uint8_t>(leaf_index >> 8 * i);
	}
}

*/

bool xmss_create_dummy_der_signature(std::vector<unsigned char>& vchSig)
{
	vchSig = xmss_create_dummy_signature(DEFAULT_KEYTYPE);
	vchSig.push_back(1);  // SIGHASH_ALL
	return true;
}

int64_t xmss_get_use_count_from_der_sig(std::vector<uint8_t> const & der_sig) noexcept
{
	auto info = get_sig_info(der_sig.data(), der_sig.size());
    
    if (!info.is_xmss)
    {
        return -1;
    }
    
	return info.key_index;
}

bool bpqcrypto::check_pubkey( uint8_t const * data, size_t size ) noexcept
{
	if (size == 0)
		return false;

	switch (data[0])
	{
	case 2:
	case 3:
	case 4:
		// ECDSA key
		return is_ecdsa_pubkey(data, size);
	case 6:
	case 7:
		// TODO: ECDSA key ?
		return size >= 33 && size <= 65;
	
	default:
		// XMSS key
		return is_xmss_pubkey(data, size);
	}

	return false;
}
