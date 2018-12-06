// Copyright (c) 2018 The Bitcoin Post-Quantum developers

#include "crypt_xmss.h"

#include <hash.h>
//#include <serialize.h>
#include <random.h>

#include <secp256k1.h>
#include <secp256k1_recovery.h>


namespace
{
	
    static const unsigned int ECDSA_PRIVATE_KEY_SIZE            = 279;
    static const unsigned int ECDSA_COMPRESSED_PRIVATE_KEY_SIZE = 214;
	
    static const unsigned int ECDSA_INTERNAL_KEY_SIZE			= 32;
	
    static const unsigned int ECDSA_PUBLIC_KEY_SIZE             = 65;
    static const unsigned int ECDSA_COMPRESSED_PUBLIC_KEY_SIZE  = 33;

    static const unsigned int ECDSA_SIGNATURE_SIZE              = 72;
	static const unsigned int ECDSA_COMPACT_SIGNATURE_SIZE      = 65;
	
	
	/* Global secp256k1_context object used for verification. */
	secp256k1_context* secp256k1_context_verify = nullptr;
	
	secp256k1_context* secp256k1_context_sign = nullptr;
	
} // namespace

namespace bpqcrypto {
	
    static const unsigned int ECDSA_PUBLIC_KEY_SIZE             = 65;
    static const unsigned int ECDSA_COMPRESSED_PUBLIC_KEY_SIZE  = 33;
	
	bool is_ecdsa_pubkey(uint8_t const * pubkey, size_t pubkey_size) noexcept
	{
		if (!pubkey || pubkey_size < ECDSA_COMPRESSED_PUBLIC_KEY_SIZE)
		{
			//  Non-canonical public key: too short
			return false;
		}
		
		if (pubkey[0] == 0x04) {
			if (pubkey_size != ECDSA_PUBLIC_KEY_SIZE) {
				//  Non-canonical public key: invalid length for uncompressed key
				return false;
			}
		} else if (pubkey[0] == 0x02 || pubkey[0] == 0x03) {
			if (pubkey_size != ECDSA_COMPRESSED_PUBLIC_KEY_SIZE) {
				//  Non-canonical public key: invalid length for compressed key
				return false;
			}
		} else {
			//  Non-canonical public key: neither compressed nor uncompressed
			return false;
		}
		return true;
	}
	
	bool is_ecdsa_compressed_pubkey(uint8_t const * pubkey, size_t pubkey_size) noexcept
	{
		if (!pubkey || pubkey_size != ECDSA_COMPRESSED_PUBLIC_KEY_SIZE) {
			//  Non-canonical public key: invalid length for compressed key
			return false;
		}
		if (pubkey[0] != 0x02 && pubkey[0] != 0x03) {
			//  Non-canonical public key: invalid prefix for compressed key
			return false;
		}
		return true;
	}
	
	

} // namespace bpqcrypto

/** These functions are taken from the libsecp256k1 distribution and are very ugly. */

/**
 * This parses a format loosely based on a DER encoding of the ECPrivateKey type from
 * section C.4 of SEC 1 <http://www.secg.org/sec1-v2.pdf>, with the following caveats:
 *
 * * The octet-length of the SEQUENCE must be encoded as 1 or 2 octets. It is not
 *   required to be encoded as one octet if it is less than 256, as DER would require.
 * * The octet-length of the SEQUENCE must not be greater than the remaining
 *   length of the key encoding, but need not match it (i.e. the encoding may contain
 *   junk after the encoded SEQUENCE).
 * * The privateKey OCTET STRING is zero-filled on the left to 32 octets.
 * * Anything after the encoding of the privateKey OCTET STRING is ignored, whether
 *   or not it is validly encoded DER.
 *
 * out32 must point to an output buffer of length at least 32 bytes.
 */
static int ec_privkey_import_der(const secp256k1_context* ctx, unsigned char *out32, const unsigned char *privkey, size_t privkeylen) {
    const unsigned char *end = privkey + privkeylen;
    memset(out32, 0, 32);
    /* sequence header */
    if (end - privkey < 1 || *privkey != 0x30u) {
        return 0;
    }
    privkey++;
    /* sequence length constructor */
    if (end - privkey < 1 || !(*privkey & 0x80u)) {
        return 0;
    }
    size_t lenb = *privkey & ~0x80u; privkey++;
    if (lenb < 1 || lenb > 2) {
        return 0;
    }
    if (end < privkey + lenb) {
        return 0;
    }
    /* sequence length */
    size_t len = privkey[lenb-1] | (lenb > 1 ? privkey[lenb-2] << 8 : 0u);
    privkey += lenb;
    if (end < privkey + len) {
        return 0;
    }
    /* sequence element 0: version number (=1) */
    if (end - privkey < 3 || privkey[0] != 0x02u || privkey[1] != 0x01u || privkey[2] != 0x01u) {
        return 0;
    }
    privkey += 3;
    /* sequence element 1: octet string, up to 32 bytes */
    if (end - privkey < 2 || privkey[0] != 0x04u) {
        return 0;
    }
    size_t oslen = privkey[1];
    privkey += 2;
    if (oslen > 32 || end < privkey + oslen) {
        return 0;
    }
    memcpy(out32 + (32 - oslen), privkey, oslen);
    if (!secp256k1_ec_seckey_verify(ctx, out32)) {
        memset(out32, 0, 32);
        return 0;
    }
    return 1;
}

/**
 * This serializes to a DER encoding of the ECPrivateKey type from section C.4 of SEC 1
 * <http://www.secg.org/sec1-v2.pdf>. The optional parameters and publicKey fields are
 * included.
 *
 * privkey must point to an output buffer of length at least CKey::PRIVATE_KEY_SIZE bytes.
 * privkeylen must initially be set to the size of the privkey buffer. Upon return it
 * will be set to the number of bytes used in the buffer.
 * key32 must point to a 32-byte raw private key.
 */
static int ec_privkey_export_der(const secp256k1_context *ctx, unsigned char *privkey, size_t *privkeylen, const unsigned char *key32, int compressed) {
    assert(*privkeylen >= ECDSA_PRIVATE_KEY_SIZE);
    secp256k1_pubkey pubkey;
    size_t pubkeylen = 0;
    if (!secp256k1_ec_pubkey_create(ctx, &pubkey, key32)) {
        *privkeylen = 0;
        return 0;
    }
    if (compressed) {
        static const unsigned char begin[] = {
            0x30,0x81,0xD3,0x02,0x01,0x01,0x04,0x20
        };
        static const unsigned char middle[] = {
            0xA0,0x81,0x85,0x30,0x81,0x82,0x02,0x01,0x01,0x30,0x2C,0x06,0x07,0x2A,0x86,0x48,
            0xCE,0x3D,0x01,0x01,0x02,0x21,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
            0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
            0xFF,0xFF,0xFE,0xFF,0xFF,0xFC,0x2F,0x30,0x06,0x04,0x01,0x00,0x04,0x01,0x07,0x04,
            0x21,0x02,0x79,0xBE,0x66,0x7E,0xF9,0xDC,0xBB,0xAC,0x55,0xA0,0x62,0x95,0xCE,0x87,
            0x0B,0x07,0x02,0x9B,0xFC,0xDB,0x2D,0xCE,0x28,0xD9,0x59,0xF2,0x81,0x5B,0x16,0xF8,
            0x17,0x98,0x02,0x21,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
            0xFF,0xFF,0xFF,0xFF,0xFE,0xBA,0xAE,0xDC,0xE6,0xAF,0x48,0xA0,0x3B,0xBF,0xD2,0x5E,
            0x8C,0xD0,0x36,0x41,0x41,0x02,0x01,0x01,0xA1,0x24,0x03,0x22,0x00
        };
        unsigned char *ptr = privkey;
        memcpy(ptr, begin, sizeof(begin)); ptr += sizeof(begin);
        memcpy(ptr, key32, 32); ptr += 32;
        memcpy(ptr, middle, sizeof(middle)); ptr += sizeof(middle);
        pubkeylen = ECDSA_COMPRESSED_PUBLIC_KEY_SIZE;
        secp256k1_ec_pubkey_serialize(ctx, ptr, &pubkeylen, &pubkey, SECP256K1_EC_COMPRESSED);
        ptr += pubkeylen;
        *privkeylen = ptr - privkey;
        assert(*privkeylen == ECDSA_COMPRESSED_PRIVATE_KEY_SIZE);
    } else {
        static const unsigned char begin[] = {
            0x30,0x82,0x01,0x13,0x02,0x01,0x01,0x04,0x20
        };
        static const unsigned char middle[] = {
            0xA0,0x81,0xA5,0x30,0x81,0xA2,0x02,0x01,0x01,0x30,0x2C,0x06,0x07,0x2A,0x86,0x48,
            0xCE,0x3D,0x01,0x01,0x02,0x21,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
            0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
            0xFF,0xFF,0xFE,0xFF,0xFF,0xFC,0x2F,0x30,0x06,0x04,0x01,0x00,0x04,0x01,0x07,0x04,
            0x41,0x04,0x79,0xBE,0x66,0x7E,0xF9,0xDC,0xBB,0xAC,0x55,0xA0,0x62,0x95,0xCE,0x87,
            0x0B,0x07,0x02,0x9B,0xFC,0xDB,0x2D,0xCE,0x28,0xD9,0x59,0xF2,0x81,0x5B,0x16,0xF8,
            0x17,0x98,0x48,0x3A,0xDA,0x77,0x26,0xA3,0xC4,0x65,0x5D,0xA4,0xFB,0xFC,0x0E,0x11,
            0x08,0xA8,0xFD,0x17,0xB4,0x48,0xA6,0x85,0x54,0x19,0x9C,0x47,0xD0,0x8F,0xFB,0x10,
            0xD4,0xB8,0x02,0x21,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
            0xFF,0xFF,0xFF,0xFF,0xFE,0xBA,0xAE,0xDC,0xE6,0xAF,0x48,0xA0,0x3B,0xBF,0xD2,0x5E,
            0x8C,0xD0,0x36,0x41,0x41,0x02,0x01,0x01,0xA1,0x44,0x03,0x42,0x00
        };
        unsigned char *ptr = privkey;
        memcpy(ptr, begin, sizeof(begin)); ptr += sizeof(begin);
        memcpy(ptr, key32, 32); ptr += 32;
        memcpy(ptr, middle, sizeof(middle)); ptr += sizeof(middle);
        pubkeylen = ECDSA_PUBLIC_KEY_SIZE;
        secp256k1_ec_pubkey_serialize(ctx, ptr, &pubkeylen, &pubkey, SECP256K1_EC_UNCOMPRESSED);
        ptr += pubkeylen;
        *privkeylen = ptr - privkey;
        assert(*privkeylen == ECDSA_PRIVATE_KEY_SIZE);
    }
    return 1;
}

/** This function is taken from the libsecp256k1 distribution and implements
 *  DER parsing for ECDSA signatures, while supporting an arbitrary subset of
 *  format violations.
 *
 *  Supported violations include negative integers, excessive padding, garbage
 *  at the end, and overly long length descriptors. This is safe to use in
 *  Bitcoin because since the activation of BIP66, signatures are verified to be
 *  strict DER before being passed to this module, and we know it supports all
 *  violations present in the blockchain before that point.
 */
static int ecdsa_signature_parse_der_lax(const secp256k1_context* ctx, secp256k1_ecdsa_signature* sig, const unsigned char *input, size_t inputlen) {
    size_t rpos, rlen, spos, slen;
    size_t pos = 0;
    size_t lenbyte;
    unsigned char tmpsig[64] = {0};
    int overflow = 0;

    /* Hack to initialize sig with a correctly-parsed but invalid signature. */
    secp256k1_ecdsa_signature_parse_compact(ctx, sig, tmpsig);

    /* Sequence tag byte */
    if (pos == inputlen || input[pos] != 0x30) {
        return 0;
    }
    pos++;

    /* Sequence length bytes */
    if (pos == inputlen) {
        return 0;
    }
    lenbyte = input[pos++];
    if (lenbyte & 0x80) {
        lenbyte -= 0x80;
        if (lenbyte > inputlen - pos) {
            return 0;
        }
        pos += lenbyte;
    }

    /* Integer tag byte for R */
    if (pos == inputlen || input[pos] != 0x02) {
        return 0;
    }
    pos++;

    /* Integer length for R */
    if (pos == inputlen) {
        return 0;
    }
    lenbyte = input[pos++];
    if (lenbyte & 0x80) {
        lenbyte -= 0x80;
        if (lenbyte > inputlen - pos) {
            return 0;
        }
        while (lenbyte > 0 && input[pos] == 0) {
            pos++;
            lenbyte--;
        }
        static_assert(sizeof(size_t) >= 4, "size_t too small");
        if (lenbyte >= 4) {
            return 0;
        }
        rlen = 0;
        while (lenbyte > 0) {
            rlen = (rlen << 8) + input[pos];
            pos++;
            lenbyte--;
        }
    } else {
        rlen = lenbyte;
    }
    if (rlen > inputlen - pos) {
        return 0;
    }
    rpos = pos;
    pos += rlen;

    /* Integer tag byte for S */
    if (pos == inputlen || input[pos] != 0x02) {
        return 0;
    }
    pos++;

    /* Integer length for S */
    if (pos == inputlen) {
        return 0;
    }
    lenbyte = input[pos++];
    if (lenbyte & 0x80) {
        lenbyte -= 0x80;
        if (lenbyte > inputlen - pos) {
            return 0;
        }
        while (lenbyte > 0 && input[pos] == 0) {
            pos++;
            lenbyte--;
        }
        static_assert(sizeof(size_t) >= 4, "size_t too small");
        if (lenbyte >= 4) {
            return 0;
        }
        slen = 0;
        while (lenbyte > 0) {
            slen = (slen << 8) + input[pos];
            pos++;
            lenbyte--;
        }
    } else {
        slen = lenbyte;
    }
    if (slen > inputlen - pos) {
        return 0;
    }
    spos = pos;

    /* Ignore leading zeroes in R */
    while (rlen > 0 && input[rpos] == 0) {
        rlen--;
        rpos++;
    }
    /* Copy R value */
    if (rlen > 32) {
        overflow = 1;
    } else {
        memcpy(tmpsig + 32 - rlen, input + rpos, rlen);
    }

    /* Ignore leading zeroes in S */
    while (slen > 0 && input[spos] == 0) {
        slen--;
        spos++;
    }
    /* Copy S value */
    if (slen > 32) {
        overflow = 1;
    } else {
        memcpy(tmpsig + 64 - slen, input + spos, slen);
    }

    if (!overflow) {
        overflow = !secp256k1_ecdsa_signature_parse_compact(ctx, sig, tmpsig);
    }
    if (overflow) {
        /* Overwrite the result again with a correctly-parsed but invalid
           signature if parsing failed. */
        memset(tmpsig, 0, 64);
        secp256k1_ecdsa_signature_parse_compact(ctx, sig, tmpsig);
    }
    return 1;
}

bool ecdsa_check_lower_S(const std::vector<unsigned char>& vchSig) 
{
    secp256k1_ecdsa_signature sig;
    if (!ecdsa_signature_parse_der_lax(secp256k1_context_verify, &sig, vchSig.data(), vchSig.size())) {
        return false;
    }
    return (!secp256k1_ecdsa_signature_normalize(secp256k1_context_verify, nullptr, &sig));
}

bool ecdsa_check_der_signature(uint8_t const * sig, size_t sig_size)
{
    // Format: 0x30 [total-length] 0x02 [R-length] [R] 0x02 [S-length] [S] [sighash]
    // * total-length: 1-byte length descriptor of everything that follows,
    //   excluding the sighash byte.
    // * R-length: 1-byte length descriptor of the R value that follows.
    // * R: arbitrary-length big-endian encoded R value. It must use the shortest
    //   possible encoding for a positive integers (which means no null bytes at
    //   the start, except a single one when the next byte has its highest bit set).
    // * S-length: 1-byte length descriptor of the S value that follows.
    // * S: arbitrary-length big-endian encoded S value. The same rules apply.
    // * sighash: 1-byte value indicating what data is hashed (not part of the DER
    //   signature)

    // Minimum and maximum size constraints.
    if (sig_size < 9) return false;
    if (sig_size > 73) return false;

    // A signature is of type 0x30 (compound).
    if (sig[0] != 0x30) return false;

    // Make sure the length covers the entire signature.
    if (sig[1] != sig_size - 3) return false;

    // Extract the length of the R element.
    unsigned int lenR = sig[3];

    // Make sure the length of the S element is still inside the signature.
    if (5 + lenR >= sig_size) return false;

    // Extract the length of the S element.
    unsigned int lenS = sig[5 + lenR];

    // Verify that the length of the signature matches the sum of the length
    // of the elements.
    if ((size_t)(lenR + lenS + 7) != sig_size) return false;
 
    // Check whether the R element is an integer.
    if (sig[2] != 0x02) return false;

    // Zero-length integers are not allowed for R.
    if (lenR == 0) return false;

    // Negative numbers are not allowed for R.
    if (sig[4] & 0x80) return false;

    // Null bytes at the start of R are not allowed, unless R would
    // otherwise be interpreted as a negative number.
    if (lenR > 1 && (sig[4] == 0x00) && !(sig[5] & 0x80)) return false;

    // Check whether the S element is an integer.
    if (sig[lenR + 4] != 0x02) return false;

    // Zero-length integers are not allowed for S.
    if (lenS == 0) return false;

    // Negative numbers are not allowed for S.
    if (sig[lenR + 6] & 0x80) return false;

    // Null bytes at the start of S are not allowed, unless S would otherwise be
    // interpreted as a negative number.
    if (lenS > 1 && (sig[lenR + 6] == 0x00) && !(sig[lenR + 7] & 0x80)) return false;

    return true;
}

bool ecdsa_verify(const std::vector<unsigned char>& vchSig, const uint256 &hash, 
	const std::vector<unsigned char>& vchPubKey)
{
	secp256k1_pubkey pubkey;
	secp256k1_ecdsa_signature sig;
	if (!secp256k1_ec_pubkey_parse(secp256k1_context_verify, &pubkey, vchPubKey.data(), vchPubKey.size())) {
		return false;
	}
	if (!ecdsa_signature_parse_der_lax(secp256k1_context_verify, &sig, vchSig.data(), vchSig.size())) {
		return false;
	}
	/* libsecp256k1's ECDSA verification requires lower-S signatures, which have
	 * not historically been enforced in Bitcoin, so normalize them first. */
	secp256k1_ecdsa_signature_normalize(secp256k1_context_verify, &sig, &sig);
	return secp256k1_ecdsa_verify(secp256k1_context_verify, &sig, hash.begin(), &pubkey);
}

bool ecdsa_recover_compact(const std::vector<unsigned char>& vchSig, const uint256 &hash, std::vector<unsigned char> & vchPubKey)
{
	if (vchSig.size() != ECDSA_COMPACT_SIGNATURE_SIZE)
		return false;
	
	int recid = (vchSig[0] - 27) & 3;
	bool fComp = ((vchSig[0] - 27) & 4) != 0;
	secp256k1_pubkey pub;
	secp256k1_ecdsa_recoverable_signature sig;
	if (!secp256k1_ecdsa_recoverable_signature_parse_compact(secp256k1_context_verify, &sig, &vchSig[1], recid)) {
		return false;
	}
	if (!secp256k1_ecdsa_recover(secp256k1_context_verify, &pub, &sig, hash.begin())) {
		return false;
	}

	size_t publen = ECDSA_PUBLIC_KEY_SIZE;
	vchPubKey.resize(publen);

	if (!secp256k1_ec_pubkey_serialize(secp256k1_context_verify, vchPubKey.data(), &publen, &pub, fComp ? SECP256K1_EC_COMPRESSED : SECP256K1_EC_UNCOMPRESSED))
		return false;
	
	vchPubKey.resize(publen);
	return true;
}

bool ecdsa_check_pubkey(uint8_t const * pubkey, size_t pubkey_size)
{
	secp256k1_pubkey pub;
	return secp256k1_ec_pubkey_parse(secp256k1_context_verify, &pub, pubkey, pubkey_size);
}

bool ecdsa_decompress(uint8_t const * pubkey, size_t pubkey_size, 
	/* out */ std::vector<unsigned char> & vchPubKey)
{
	secp256k1_pubkey pub;
	if (!secp256k1_ec_pubkey_parse(secp256k1_context_verify, &pub, pubkey, pubkey_size)) {
		return false;
	}

	size_t publen = ECDSA_PUBLIC_KEY_SIZE;
	vchPubKey.resize(publen);

	if (!secp256k1_ec_pubkey_serialize(secp256k1_context_verify, vchPubKey.data(), &publen, &pub, SECP256K1_EC_UNCOMPRESSED))
		return false;

	vchPubKey.resize(publen);
	return true;
}

bool ecdsa_derive(uint8_t const * pubkey, size_t pubkey_size, 
	uint256 &ccChild, unsigned int nChild, const uint256& cc,
	/* out */ std::vector<unsigned char> & vchPubKey)
{
	assert((nChild >> 31) == 0);
	assert(pubkey_size == ECDSA_COMPRESSED_PUBLIC_KEY_SIZE);

	unsigned char out[64];
	BIP32Hash(cc, nChild, pubkey[0], pubkey + 1, out);
	memcpy(ccChild.begin(), out+32, 32);

	secp256k1_pubkey pub;
	if (!secp256k1_ec_pubkey_parse(secp256k1_context_verify, &pub, pubkey, pubkey_size)) {
		return false;
	}
	if (!secp256k1_ec_pubkey_tweak_add(secp256k1_context_verify, &pub, out)) {
		return false;
	}
	
	size_t publen = ECDSA_COMPRESSED_PUBLIC_KEY_SIZE;
	vchPubKey.resize(publen);

	secp256k1_ec_pubkey_serialize(secp256k1_context_verify, vchPubKey.data(), &publen, &pub, SECP256K1_EC_COMPRESSED);
	
	vchPubKey.resize(publen);
	return true;
}

bool ecdsa_check_private_key(const unsigned char *vch)
{
	return secp256k1_ec_seckey_verify(secp256k1_context_sign, vch);
}

bpqcrypto::secure_vector<uint8_t> ecdsa_private_export_der(
	uint8_t const * key, size_t key_size, bool fCompressed)
{
	bpqcrypto::secure_vector<uint8_t> result;
	
    size_t privkeylen = ECDSA_PRIVATE_KEY_SIZE;
    result.resize(privkeylen);
		
    bool ret = ec_privkey_export_der(secp256k1_context_sign, 
		result.data(), &privkeylen, 
		key, fCompressed ? SECP256K1_EC_COMPRESSED : SECP256K1_EC_UNCOMPRESSED);
	assert(ret);
        
	result.resize(privkeylen);
	return result;
}

std::vector<unsigned char> ecdsa_extract_public_key_from_private(uint8_t const * key, size_t key_size, bool fCompressed)
{
	assert(key && key_size > 0);
	
	secp256k1_pubkey pubkey;
	int ret = secp256k1_ec_pubkey_create(secp256k1_context_sign, &pubkey, key);
	assert(ret);
	
	std::vector<unsigned char> result;
	result.resize(ECDSA_PUBLIC_KEY_SIZE);
	size_t clen = result.size();
	secp256k1_ec_pubkey_serialize(secp256k1_context_sign, 
		result.data(), &clen, &pubkey, fCompressed ? SECP256K1_EC_COMPRESSED : SECP256K1_EC_UNCOMPRESSED);

	result.resize(clen);
	return result;
}

std::vector<unsigned char> ecdsa_sign(uint8_t const * key, size_t key_size, 
	const uint256 &hash, uint32_t test_case)
{
	assert(key && key_size > 0);
	std::vector<unsigned char> vchSig;
	
	vchSig.resize(ECDSA_SIGNATURE_SIZE);
	size_t nSigLen = vchSig.size();

	unsigned char extra_entropy[32] = {0};
	WriteLE32(extra_entropy, test_case);

	secp256k1_ecdsa_signature sig;
	int ret = secp256k1_ecdsa_sign(secp256k1_context_sign, &sig, hash.begin(), 
		key, secp256k1_nonce_function_rfc6979, test_case ? extra_entropy : nullptr);
	assert(ret);

	secp256k1_ecdsa_signature_serialize_der(secp256k1_context_sign, 
		(unsigned char*)vchSig.data(), &nSigLen, &sig);

	vchSig.resize(nSigLen);
	return vchSig;
}

std::vector<unsigned char> ecdsa_sign_compact(
	uint8_t const * key, size_t key_size, bool fCompressed, const uint256 &hash)
{
	assert(key && key_size > 0);
	std::vector<unsigned char> vchSig;
	vchSig.resize(ECDSA_COMPACT_SIGNATURE_SIZE);
	
	int rec = -1;
	secp256k1_ecdsa_recoverable_signature sig;
	
	int ret = secp256k1_ecdsa_sign_recoverable(secp256k1_context_sign, &sig, 
		hash.begin(), key, secp256k1_nonce_function_rfc6979, nullptr);
	assert(ret);
	
	ret = secp256k1_ecdsa_recoverable_signature_serialize_compact(
		secp256k1_context_sign, vchSig.data() + 1, &rec, &sig);

	assert(ret);
	assert(rec != -1);
	vchSig[0] = 27 + rec + (fCompressed ? 4 : 0);
	
	return vchSig;
}

secure_vector ecdsa_import_der_privkey(secure_vector const & der_key)
{
	secure_vector key;
	key.resize(ECDSA_INTERNAL_KEY_SIZE);
	if (!ec_privkey_import_der(secp256k1_context_sign, (unsigned char*)key.data(), 
		der_key.data(), der_key.size()))
	{
		return {};
	}
	
	return key;
}

bool ecdsa_privkey_tweak_add(unsigned char * keyChild, secure_vector const & tweak)
{
    return secp256k1_ec_privkey_tweak_add(secp256k1_context_sign, 
		keyChild, tweak.data());
}

bool ecdsa_create_dummy_der_signature(std::vector<unsigned char>& vchSig)
{
	vchSig.assign(72, '\000');
	vchSig[0] = 0x30;
	vchSig[1] = 69;
	vchSig[2] = 0x02;
	vchSig[3] = 33;
	vchSig[4] = 0x01;
	vchSig[4 + 33] = 0x02;
	vchSig[5 + 33] = 32;
	vchSig[6 + 33] = 0x01;
	vchSig[6 + 33 + 32] = 1; //SIGHASH_ALL;
	return true;
}


/* static */ int ECCVerifyHandle::refcount = 0;

ECCVerifyHandle::ECCVerifyHandle()
{
    if (refcount == 0) {
        assert(secp256k1_context_verify == nullptr);
        secp256k1_context_verify = secp256k1_context_create(SECP256K1_CONTEXT_VERIFY);
        assert(secp256k1_context_verify != nullptr);
    }
    refcount++;
}

ECCVerifyHandle::~ECCVerifyHandle()
{
    refcount--;
    if (refcount == 0) {
        assert(secp256k1_context_verify != nullptr);
        secp256k1_context_destroy(secp256k1_context_verify);
        secp256k1_context_verify = nullptr;
    }
}

void ECC_Start() {
    assert(secp256k1_context_sign == nullptr);

    secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
    assert(ctx != nullptr);

    {
        // Pass in a random blinding seed to the secp256k1 context.
        bpqcrypto::secure_vector<uint8_t> vseed(32);
        GetRandBytes(vseed.data(), 32);
        bool ret = secp256k1_context_randomize(ctx, vseed.data());
        assert(ret);
    }

    secp256k1_context_sign = ctx;
}

void ECC_Stop() {
    secp256k1_context *ctx = secp256k1_context_sign;
    secp256k1_context_sign = nullptr;

    if (ctx) {
        secp256k1_context_destroy(ctx);
    }
}
