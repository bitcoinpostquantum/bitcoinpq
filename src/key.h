// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Copyright (c) 2017 The Zcash developers
// Copyright (c) 2018 The Bitcoin Post-Quantum developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_KEY_H
#define BITCOIN_KEY_H

#include <pubkey.h>
#include <serialize.h>
#include <uint256.h>

#include <stdexcept>
#include <vector>


/**
 * secure_allocator is defined in allocators.h
 * CPrivKey is a serialized private key, with all parameters included
 * (PRIVATE_KEY_SIZE bytes)
 */
typedef bpqcrypto::secure_vector<uint8_t> CPrivKey;

class CKeyStore;

/** An encapsulated private key. */
class CKey
{
public:
    
    /**
     * disable: CKey::Sign throws exception if xmss use_count exceeds limits
     * default: false
     */
    
    static bool force_signing;
    
    /**
     * secp256k1:
     */
    static const unsigned int ECDSA_PRIVATE_KEY_SIZE            = 279;
    static const unsigned int ECDSA_COMPRESSED_PRIVATE_KEY_SIZE = 214;
	
    static const unsigned int ECDSA_INTERNAL_KEY_SIZE	= 32;

    /**
     * see www.keylength.com
     * script supports up to 75 for single byte push
     */
    static_assert(
        ECDSA_PRIVATE_KEY_SIZE >= ECDSA_COMPRESSED_PRIVATE_KEY_SIZE,
        "ECDSA_COMPRESSED_PRIVATE_KEY_SIZE is larger than ECDSA_PRIVATE_KEY_SIZE");
	
private:

	CKeyStore * m_pKeyStore; // keystore to support use_count increment

    //! Whether this private key is valid. We check for correctness when modifying the key
    //! data, so fValid should always correspond to the actual state.
    bool fValid;

	//! Whethe the private key is ECDSA legacy key
	bool fLegacy;

    //! Whether the public key corresponding to this private key is (to be) compressed.
    bool fCompressed;

    //! The actual byte data
    Botan::secure_vector<uint8_t> keydata;
	
    //! Check whether the 32-byte array pointed to by vch is valid keydata.
    bool static ECDSA_Check(const unsigned char* vch);
	
public:
	
    //! Construct an invalid private key.
    CKey(CKeyStore * keystore = nullptr) 
		: m_pKeyStore(keystore)
		, fValid(false)
		, fLegacy(true)
		, fCompressed(false)
    {
        // Important: vch must be 32 bytes in length to not break serialization
        keydata.resize(ECDSA_INTERNAL_KEY_SIZE);
    }

	//! Construct from vector
	CKey( bpqcrypto::secure_vector<uint8_t> const & raw_key, CKeyStore * keystore = nullptr) 
		: m_pKeyStore(keystore)
		, fValid(false)
	{ 
		Set( raw_key, keystore ); 
	}

	void AssignKeyStore(CKeyStore * keystore)
	{
		m_pKeyStore = keystore;
	}
	
    friend bool operator==(const CKey& a, const CKey& b)
    {
		if (a.fLegacy)
			return a.fCompressed == b.fCompressed && a.keydata == b.keydata;
		else
			return xmss_equal(a.keydata, b.keydata);
    }

    friend bool operator!=(const CKey& a, const CKey& b)
	{
		return !(a == b);
	}

	// data can be:
	// ecdsa uncompressed key:   32 bytes
	// ecdsa compressed key:     32 bytes + 1
	// xmss any key:             140+ bytes
	void Set( uint8_t const * data, size_t size, CKeyStore * keystore = nullptr);

	void Set( bpqcrypto::secure_vector<uint8_t> const & raw_key, CKeyStore * keystore = nullptr) 
    {
		Set(raw_key.data(), raw_key.size(), keystore ); 
    }
	
	void Set( std::vector<uint8_t> const & raw_key, CKeyStore * keystore = nullptr) 
    {
		Set(raw_key.data(), raw_key.size(), keystore ); 
    }
    
    template <typename T>
    void Set(T pbegin, T pend, CKeyStore * keystore = nullptr)
	{
		int len = pend - pbegin;
		Set((uint8_t const*)&pbegin[0], len, nullptr);
	}
	
    //! Initialize using begin and end iterators to byte data.
    template <typename T>
    void SetLegacy(const T pbegin, const T pend, bool fCompressedIn = false)
    {
		size_t len = size_t(pend - pbegin);
        if (len != ECDSA_INTERNAL_KEY_SIZE) {
            fValid = false;
        } else if (ECDSA_Check(&pbegin[0])) {
			keydata.assign((unsigned char*)&pbegin[0], (unsigned char*)&pbegin[0] + ECDSA_INTERNAL_KEY_SIZE);
            fValid = true;
			fLegacy = true;
            fCompressed = fCompressedIn;
        } else {
            fValid = false;
        }
    }
	
    void SetLegacy(bpqcrypto::secure_vector<uint8_t> const & raw, bool fCompressedIn = false)
	{
		SetLegacy(raw.begin(), raw.end(), fCompressedIn);
	}
	
    //! Simple read-only vector-like interface.
    unsigned int size() const;
    const unsigned char* begin() const { return keydata.data(); }
    const unsigned char* end() const { return keydata.data() + size(); }

    //! Check whether this private key is valid.
    bool IsValid() const { return fValid; }

    //! Check whether the public key corresponding to this private key is (to be) compressed.
    bool IsCompressed() const { return fCompressed; }
	
	bool IsXMSS() const { return !fLegacy; }
	
	//! returns key in legacy form: 32 bytes payload + 1 optional byte(if compressed)
	bpqcrypto::secure_vector<uint8_t> raw_private_key_legacy() const;

	//! returns key in new form: 1 byte prefix + payload of any size
	bpqcrypto::secure_vector<uint8_t> raw_private_key() const;
	
    size_t short_size() const;
	bpqcrypto::secure_vector<uint8_t> raw_short_key() const;
	
	//unsigned long get_leaf_index() const;
	//void set_leaf_index(unsigned long index);

    //! Generate a new private key using a cryptographic PRNG.
    void MakeNewKey(CKeyType ktype);

    /**
     * Convert the private key to a CPrivKey (serialized OpenSSL private key data).
     * This is expensive.
     */
    CPrivKey GetPrivKey() const;

    /**
     * Compute the public key from a private key.
     * This is expensive.
     */
    CPubKey GetPubKey() const;

    /**
     * Create a DER-serialized signature.
     * The test_case parameter tweaks the deterministic nonce.
     */
	bool Sign(const uint8_t * msg, size_t msg_size, std::vector<unsigned char>& vchSig, uint32_t test_case = 0) const;

	template <typename T>
	bool Sign(T const & msg, std::vector<unsigned char>& vchSig, uint32_t test_case = 0) const
	{
		auto && beg = msg.begin();
		auto && end = msg.end();
		size_t len = end - beg;
		return len > 0 && Sign((uint8_t const *)&beg[0], len, vchSig, test_case);
	}
	
    bool SignHash(const uint256& hash, std::vector<unsigned char>& vchSig, uint32_t test_case = 0) const;

    /**
     * Create a compact signature (65 bytes), which allows reconstructing the used public key.
     * The format is one header byte, followed by two times 32 bytes for the serialized r and s values.
     * The header byte: 0x1B = first key with even y, 0x1C = first key with odd y,
     *                  0x1D = second key with even y, 0x1E = second key with odd y,
     *                  add 0x04 for compressed keys.
     */
	bool SignCompact(std::string const & msg, std::vector<unsigned char>& vchSig) const;
    bool SignCompact(const uint256& hash, std::vector<unsigned char>& vchSig) const;

    //! Derive BIP32 child key.
    bool Derive(CKey& keyChild, ChainCode &ccChild, unsigned int nChild, const ChainCode& cc) const;

    /**
     * Verify thoroughly whether a private key and a public key match.
     * This is done using a different mechanism than just regenerating it.
     */
    bool VerifyPubKey(const CPubKey& vchPubKey) const;

    //! Load private key and check that public key matches.
    bool Load(const CPrivKey& privkey, const CPubKey& vchPubKey, bool fSkipCheck);
};

struct CExtKey {
    unsigned char nDepth;
    unsigned char vchFingerprint[4];
    unsigned int nChild;
    ChainCode chaincode;
    CKey key;

    friend bool operator==(const CExtKey& a, const CExtKey& b)
    {
        return a.nDepth == b.nDepth &&
            memcmp(&a.vchFingerprint[0], &b.vchFingerprint[0], sizeof(vchFingerprint)) == 0 &&
            a.nChild == b.nChild &&
            a.chaincode == b.chaincode &&
            a.key == b.key;
    }

    void Encode(unsigned char code[BIP32_EXTKEY_SIZE]) const;
    void Decode(const unsigned char code[BIP32_EXTKEY_SIZE]);

    bool Derive(CExtKey& out, unsigned int nChild) const;

    CExtPubKey Neuter() const;

    void SetMaster(const unsigned char* seed, unsigned int nSeedLen);
    template <typename Stream>

    void Serialize(Stream& s) const
    {
        if ( key.IsXMSS() )
        {
            unsigned int len = key.size();
            ::WriteCompactSize(s, len);
            s.write((const char *)&key.begin()[0], len);
        }
        else
        {
            unsigned int len = BIP32_EXTKEY_SIZE;
            ::WriteCompactSize(s, len);
            unsigned char code[BIP32_EXTKEY_SIZE];
            Encode(code);
            s.write((const char *)&code[0], len);
        }
    }
    template <typename Stream>
    void Unserialize(Stream& s)
    {
        unsigned int len = ::ReadCompactSize(s);

        unsigned char code[BIP32_EXTKEY_SIZE];
        if (len != BIP32_EXTKEY_SIZE)
            throw std::runtime_error("Invalid extended key size\n");
        s.read((char *)&code[0], len);
        Decode(code);
    }
};

inline bool is_key_segwit_useable(CKey const & key )
{
	return key.IsXMSS() || key.IsCompressed();
}


/** Check that required EC support is available at runtime. */
bool ECC_InitSanityCheck(void);

#endif // BITCOIN_KEY_H
