// Copyright (c) 2018 The Bitcoin Post-Quantum developers

#ifndef CRYPT_XMSS_H
#define CRYPT_XMSS_H

#include <cstdint>
#include <vector>

#include <support/allocators/secure.h>
#include <uint256.h>

#include "bpqcrypto/include/bpqcrypto.hpp"

namespace bpqcrypto {
	
	// TODO: move to bpq_crypto
	size_t xmss_get_max_use_count(uint8_t const * pubkey, size_t pubkey_size) noexcept;
	
	bool is_ecdsa_key(uint8_t const * pKey, size_t nKeySize) noexcept;
	bool is_ecdsa_pubkey(uint8_t const * pKey, size_t nKeySize) noexcept;

	inline bool is_ecdsa_pubkey(std::vector<uint8_t> const & key) noexcept
	{
		return is_ecdsa_pubkey(key.data(), key.size());
	}

	bool is_ecdsa_compressed_pubkey(uint8_t const * pubkey, size_t pubkey_size) noexcept;

	inline bool is_ecdsa_compressed_pubkey(std::vector<uint8_t> const & key) noexcept
	{
		return is_ecdsa_compressed_pubkey(key.data(), key.size());
	}
	
	bool check_pubkey( uint8_t const * data, size_t size ) noexcept;
	
	inline bool check_pubkey( std::vector<uint8_t> const & pubkey ) noexcept
	{
		return check_pubkey(pubkey.data(), pubkey.size());
	}
}

static const bpqcrypto::KeyType DEFAULT_KEYTYPE = bpqcrypto::KeyType::XMSS_256_H10;

static const unsigned int XMSS_DER_SIGNATURE_SIZE_MIN = 2504u;
static const unsigned int XMSS_DER_SIGNATURE_SIZE_MAX = 10000u;


/*
 * public key structure:
 * i8					prefix '16'
 * i32					key parameters id
 * 2 * element_size		key data
 * 
 * private key structure:
 * <public key>			public key
 * i64					unuzed leaf index
 * 2 * element_size		key data
 * 
 * raw key structure:
 * <private_key>		private key
 * <hash tree>			cached hash tree
 * 
 */

//size_t xmss_get_private_long_key_size(Botan::XMSS_Parameters const & params);

//std::vector<uint8_t> xmss_extract_public_key_from_private(uint8_t const * data, size_t size);

//uint8_t xmss_get_TreeH_from_public_key(std::vector<uint8_t> const & pubkey);
	
bool xmss_equal(Botan::secure_vector<uint8_t> const & a, Botan::secure_vector<uint8_t> const & b);

// returns signature with public key
std::vector<unsigned char> 
xmss_sign_compact(
	Botan::secure_vector<uint8_t> const & priv_key, size_t & leaf_index,
	uint8_t const * message, size_t message_size);

// returns der publick key extracted from signature & verifys it
// on error returns empty vector
std::vector<uint8_t> 
xmss_recover_compact(
	std::vector<unsigned char> const & compact_sig, 
	uint8_t const * message, size_t message_size);

//uint64_t xmss_get_leaf_index( uint8_t const * privkey, size_t size);

//void xmss_set_leaf_index( uint8_t * privkey, size_t size, uint64_t leaf_index);

// checks valid der signature + 1 byte sighash
inline bool xmss_is_der_signature(std::vector<uint8_t> const & der_sig) noexcept
{
	return bpqcrypto::is_xmss_signature(der_sig.data(), der_sig.size(), false);
}

inline bool xmss_is_der_pubkey(uint8_t const * pubkey, size_t pubkey_size) noexcept
{
	return bpqcrypto::is_xmss_pubkey(pubkey, pubkey_size);
}

inline bool xmss_is_der_pubkey(std::vector<uint8_t> const & pubkey) noexcept
{
	return bpqcrypto::is_xmss_pubkey(pubkey.data(), pubkey.size());
}

// Create a dummy signature that is a valid DER-encoding, +1 byte sighash
bool xmss_create_dummy_der_signature(std::vector<unsigned char>& vchSig);

// der signature
int64_t xmss_get_use_count_from_der_sig(std::vector<uint8_t> const & der_sig) noexcept;

//
// ECDSA 
//


/**
 * Check whether a signature is normalized (lower-S).
 */
bool ecdsa_check_lower_S(const std::vector<unsigned char>& vchSig);

/**
 * A canonical signature exists of: <30> <total len> <02> <len R> <R> <02> <len S> <S> <hashtype>
 * Where R and S are not negative (their first byte has its highest bit not set), and not
 * excessively padded (do not start with a 0 byte, unless an otherwise negative number follows,
 * in which case a single 0 byte is necessary and even required).
 * 
 * See https://bitcointalk.org/index.php?topic=8392.msg127623#msg127623
 *
 * This function is consensus-critical since BIP66.
 */
bool ecdsa_check_der_signature(uint8_t const * sig, size_t sig_size);

bool ecdsa_verify(const std::vector<unsigned char>& vchSig, const uint256 &hash, 
	const std::vector<unsigned char>& vchPubKey);

bool ecdsa_recover_compact(const std::vector<unsigned char>& vchSig, const uint256 &hash, 
	/* out */ std::vector<unsigned char> & vchPubKey);

bool ecdsa_check_pubkey(uint8_t const * pubkey, size_t pubkey_size);

bool ecdsa_decompress(uint8_t const * pubkey, size_t pubkey_size, 
	/* out */ std::vector<unsigned char> & vchPubKey);

bool ecdsa_derive(uint8_t const * pubkey, size_t pubkey_size, 
	uint256 &ccChild, unsigned int nChild, const uint256& cc,
	/* out */ std::vector<unsigned char> & vchPubKey);

bool ecdsa_check_private_key(const unsigned char *vch);
bpqcrypto::secure_vector<uint8_t> ecdsa_private_export_der(uint8_t const * key, size_t key_size, bool fCompressed);

std::vector<unsigned char> ecdsa_extract_public_key_from_private(uint8_t const * key, size_t key_size, bool fCompressed);

std::vector<unsigned char> ecdsa_sign(uint8_t const * key, size_t key_size, const uint256 &hash, uint32_t test_case);
std::vector<unsigned char> ecdsa_sign_compact(uint8_t const * key, size_t key_size, bool fCompressed, const uint256 &hash);

bpqcrypto::secure_vector<uint8_t> ecdsa_import_der_privkey(
	bpqcrypto::secure_vector<uint8_t> const & der_privkey);

bool ecdsa_privkey_tweak_add(unsigned char * keyChild, 
	bpqcrypto::secure_vector<uint8_t> const & tweak);


// Create a dummy signature that is a valid DER-encoding, +1 byte sighash
bool ecdsa_create_dummy_der_signature(std::vector<unsigned char>& vchSig);


/** Users of this module must hold an ECCVerifyHandle. The constructor and
 *  destructor of these are not allowed to run in parallel, though. */
class ECCVerifyHandle
{
    static int refcount;

public:
    ECCVerifyHandle();
    ~ECCVerifyHandle();
};

/** Initialize the elliptic curve support. May not be called twice without calling ECC_Stop first. */
extern void ECC_Start(void);

/** Deinitialize the elliptic curve support. No-op if ECC_Start wasn't called first. */
extern void ECC_Stop(void);

#endif /* CRYPT_XMSS_H */

