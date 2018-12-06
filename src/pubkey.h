// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Copyright (c) 2017 The Zcash developers
// Copyright (c) 2018 The Bitcoin Post-Quantum developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_PUBKEY_H
#define BITCOIN_PUBKEY_H

#include <hash.h>
#include <serialize.h>
#include <uint256.h>
#include <streams.h>

#include "crypt_xmss.h"
#include <script/standard.h>

#include <stdexcept>
#include <vector>
#include <cstdint>

const unsigned int BIP32_EXTKEY_SIZE = 74;

/** A reference to a CKey: the Hash160 of its serialized public key: 
 * KeyID = RIPEMD160(SHA256(pubkey))
 */
class CKeyID : public uint160
{
public:
    CKeyID() : uint160() {}
    explicit CKeyID(const uint160& in) : uint160(in) {}
};

typedef uint256 ChainCode;

using CKeyType = bpqcrypto::KeyType;

extern std::string KeyTypeToString(CKeyType keytype);

/** An encapsulated public key. */
class CPubKey
{
public:
    /**
     * secp256k1:
     */
    static const unsigned int ECDSA_PUBLIC_KEY_SIZE             = 65;
    static const unsigned int ECDSA_COMPRESSED_PUBLIC_KEY_SIZE  = 33;
    static const unsigned int ECDSA_SIGNATURE_SIZE              = 72;
    static const unsigned int ECDSA_COMPACT_SIGNATURE_SIZE      = 65;
    /**
     * see www.keylength.com
     * script supports up to 75 for single byte push
     */
    static_assert(
        ECDSA_PUBLIC_KEY_SIZE >= ECDSA_COMPRESSED_PUBLIC_KEY_SIZE,
        "COMPRESSED_PUBLIC_KEY_SIZE is larger than PUBLIC_KEY_SIZE");
	
	
    /**
     * xmss
     */
	
	// public key length including prefix
    static const unsigned int XMSS_256_PUBLIC_KEY_SIZE   = 69;
    static const unsigned int XMSS_512_PUBLIC_KEY_SIZE   = 133;

	// prefix for XMSS public key
	// values 16+ reserved for XMSS public keys versions
    static const uint8_t HDR_XMSS_V1 = 16;

private:

    /**
     * Just store the serialized data.
     * Its length can very cheaply be computed from the first byte.
     */
	
	std::vector<uint8_t> vch;
	//uint8_t vch[80];

    //! Compute the length of a pubkey with a given first byte.
    unsigned int static GetLen1(unsigned char chHeader)
    {
        if (chHeader == 2 || chHeader == 3)
            return ECDSA_COMPRESSED_PUBLIC_KEY_SIZE;
        if (chHeader == 4 || chHeader == 6 || chHeader == 7)
            return ECDSA_PUBLIC_KEY_SIZE;
		
		if (chHeader == HDR_XMSS_V1)
            return XMSS_256_PUBLIC_KEY_SIZE;

        return 0;
    }

    //! Set this key data to be invalid
    void Invalidate()
    {
		vch.clear();
		//vch.resize(1);
        //vch[0] = 0xFF;
    }

public:
    //! Construct an invalid public key.
    CPubKey()
    {
		//vch.resize(80);
        Invalidate();
    }

    //! Initialize a public key
    void Set(uint8_t const * data, size_t size);
	
    template <typename T>
    void Set(const T pbegin, const T pend)
    {
        int len = pend - pbegin;
		Set((uint8_t const*)&pbegin[0], len);
    }

    void Set(std::vector<uint8_t> const & raw_key)
	{
		Set(raw_key.data(), raw_key.size());
	}
	
    //! Construct a public key using begin/end iterators to byte data.
    template <typename T>
    CPubKey(const T pbegin, const T pend)
    {
        Set(pbegin, pend);
    }

    explicit CPubKey(std::vector<uint8_t> const & raw_key)
    {
        Set(raw_key);
    }

    CPubKey(uint8_t const * pbegin, uint8_t const * pend)
    {
        Set(pbegin, pend);
    }

	CKeyType GetKeyType() const;
	
    //! Simple read-only vector-like interface to the pubkey data.
    unsigned int size() const { return vch.size(); } //GetLen(vch[0]); }
    const unsigned char* begin() const { return vch.data(); }
    const unsigned char* end() const { return vch.data() + size(); }
    const unsigned char& operator[](unsigned int pos) const { return vch[pos]; }
    
    std::vector<uint8_t> const & raw_pubkey() const { return vch; }

    //! Comparator implementation.
    friend bool operator==(const CPubKey& a, const CPubKey& b)
    {
        //return a.vch[0] == b.vch[0] &&
        //       memcmp(a.vch, b.vch, a.size()) == 0;
        return a.vch == b.vch;
    }
    friend bool operator!=(const CPubKey& a, const CPubKey& b)
    {
        return !(a == b);
    }
    friend bool operator<(const CPubKey& a, const CPubKey& b)
    {
        //return a.vch[0] < b.vch[0] ||
        //       (a.vch[0] == b.vch[0] && memcmp(a.vch, b.vch, a.size()) < 0);
        return a.vch < b.vch;
    }

    //! Implement serialization, as if this was a byte vector.
    template <typename Stream>
    void Serialize(Stream& s) const
    {
        unsigned int len = size();
        ::WriteCompactSize(s, len);
        s.write((char const*)begin(), len);
    }
    template <typename Stream>
    void Unserialize(Stream& s)
    {
        unsigned int len = ::ReadCompactSize(s);
		vch.resize(len);
		s.read((char*)vch.data(), len);
    }

    //! Get the KeyID of this public key (hash of its serialization)
    CKeyID GetID() const
    {
        return CKeyID(Hash160(vch.data(), vch.data() + size()));
    }

    //! Get the 256-bit hash of this public key.
    //! SHA256(SHA256(pubkey))
    uint256 GetHash() const
    {
        return Hash(vch.data(), vch.data() + size());
    }

    /*
     * Check syntactic correctness.
     *
     * Note that this is consensus critical as CheckSig() calls it!
     */
    bool IsValid() const
    {
        return size() > 0;
    }

    //! fully validate whether this is a valid public key (more expensive than IsValid())
    bool IsFullyValid() const;

    //! Check whether this is a compressed public key.
    bool IsCompressed() const
    {
        //return size() > 0 && (vch[0] == 2 || vch[0] == 3);
		return size() == ECDSA_COMPRESSED_PUBLIC_KEY_SIZE;
    }

    bool IsXMSS() const
    {
        return vch.size() > 0 && vch[0] == HDR_XMSS_V1;
    }
	
	size_t GetMaxUseCount() const;
	
    /**
     * Verify a DER signature (~72 bytes).
     * If this public key is not fully valid, the return value will be false.
     */
    bool Verify(const uint8_t * msg, size_t msg_size, std::vector<unsigned char> const & vchSig) const;

	template <typename T>
	bool Verify(T const & msg, std::vector<unsigned char> const & vchSig) const
	{
		auto && beg = msg.begin();
		auto && end = msg.end();
		size_t len = end - beg;
		return len > 0 && Verify((uint8_t const *)&beg[0], len, vchSig);
	}
	
    bool VerifyHash(const uint256& hash, const std::vector<unsigned char>& vchSig) const;

    //! Recover a public key from a compact signature.
    bool RecoverCompact(std::string const & msg, const std::vector<unsigned char>& vchSig);
    bool RecoverCompact(const uint256& hash, const std::vector<unsigned char>& vchSig);

    //! Turn this public key into an uncompressed public key.
    bool Decompress();

    //! Derive BIP32 child pubkey.
    bool Derive(CPubKey& pubkeyChild, ChainCode &ccChild, unsigned int nChild, const ChainCode& cc) const;
};

struct CExtPubKey {
    unsigned char nDepth;
    unsigned char vchFingerprint[4];
    unsigned int nChild;
    ChainCode chaincode;
    CPubKey pubkey;

    friend bool operator==(const CExtPubKey &a, const CExtPubKey &b)
    {
        return a.nDepth == b.nDepth &&
            memcmp(&a.vchFingerprint[0], &b.vchFingerprint[0], sizeof(vchFingerprint)) == 0 &&
            a.nChild == b.nChild &&
            a.chaincode == b.chaincode &&
            a.pubkey == b.pubkey;
    }

    void Encode(unsigned char code[BIP32_EXTKEY_SIZE]) const;
    void Decode(const unsigned char code[BIP32_EXTKEY_SIZE]);
    bool Derive(CExtPubKey& out, unsigned int nChild) const;

    void Serialize(CSizeComputer& s) const
    {
        // Optimized implementation for ::GetSerializeSize that avoids copying.
        s.seek(BIP32_EXTKEY_SIZE + 1); // add one byte for the size (compact int)
    }
    template <typename Stream>
    void Serialize(Stream& s) const
    {
		if ( pubkey.IsXMSS() )
		{
            // TODO: encode
            unsigned int len = pubkey.size();
            ::WriteCompactSize(s, len);
            s.write((const char *)&pubkey.begin()[0], len);
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
		if ( len != BIP32_EXTKEY_SIZE )
        {
            std::vector<uint8_t> code(len);
            s.read((char *)code.data(), len);
            // TODO: decode
            pubkey.Set(code.begin(), code.end());            
        }		
		else
		{
			unsigned char code[BIP32_EXTKEY_SIZE];
			if (len != BIP32_EXTKEY_SIZE)
				throw std::runtime_error("Invalid extended key size\n");
			s.read((char *)&code[0], len);
			Decode(code);
		}
    }
};

inline bool is_key_segwit_useable(CPubKey const & pubkey )
{
	return pubkey.IsXMSS() || pubkey.IsCompressed();
}

#endif // BITCOIN_PUBKEY_H
