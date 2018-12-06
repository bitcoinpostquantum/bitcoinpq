// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Copyright (c) 2018 The Bitcoin Post-Quantum developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_PRIMITIVES_BLOCK_H
#define BITCOIN_PRIMITIVES_BLOCK_H

#include "arith_uint256.h"
#include "primitives/transaction.h"
#include "serialize.h"
#include "uint256.h"
#include "version.h"
#include <string.h>

/** Nodes collect new transactions into a block, hash them into a hash tree,
 * and scan through nonce values to make the block's hash satisfy proof-of-work
 * requirements.  When they solve the proof-of-work, they broadcast the block
 * to everyone and the block is added to the block chain.  The first transaction
 * in the block is a special one that creates a new coin owned by the creator
 * of the block.
 */
class CBlockHeader
{
public:
    static const size_t LEGACY_HEADER_SIZE = 4+32+32+4+4+4;
    static const size_t BPQ_HEADER_SIZE = 1+4+32+32+32+4+4+32;  // Excluding Equihash solution

    static const uint8_t BITCOIN_MAJOR_VERSION = 0;
    static const uint8_t BPQ_MAJOR_VERSION = 1;

    uint8_t nMajorVersion;  // used for hard forks
    int32_t nMinorVersion;  // used for soft forks

    uint256 hashPrevBlock;
    uint256 hashMerkleRoot;
    uint256 hashWitnessMerkleRoot;  // reserved

    uint32_t nTime;
    uint32_t nBits;

    uint256 nNonce;
    std::vector<unsigned char> nSolution;  // Equihash solution.

    CBlockHeader()
    {
        SetNull();
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action)
    {
        READWRITE(nMajorVersion);
        READWRITE(nMinorVersion);

        READWRITE(hashPrevBlock);
        READWRITE(hashMerkleRoot);
        READWRITE(hashWitnessMerkleRoot);

        READWRITE(nTime);
        READWRITE(nBits);

        READWRITE(nNonce);
        READWRITE(nSolution);  // var_int
    }

    template <typename Stream>
    void SerializeLegacy(Stream &s) const
    {
        // serialize as bitcoin header

        s << nMinorVersion;

        s << hashPrevBlock;
        s << hashMerkleRoot;

        s << nTime;
        s << nBits;

        uint32_t legacy_nonce = (uint32_t)nNonce.GetUint64(0);
        s << legacy_nonce;
    }

    template <typename Stream>
    void UnserializeLegacy(Stream &s)
    {
        nMajorVersion = BITCOIN_MAJOR_VERSION;

        s >> nMinorVersion;

        s >> hashPrevBlock;
        s >> hashMerkleRoot;
        hashWitnessMerkleRoot.SetNull();

        s >> nTime;
        s >> nBits;

        uint32_t legacy_nonce;
        s >> legacy_nonce;
        nNonce = ArithToUint256(arith_uint256(legacy_nonce));

        nSolution.clear();
    }

    void SetNull()
    {
        nMajorVersion = 0;
        nMinorVersion = 0;

        hashPrevBlock.SetNull();
        hashMerkleRoot.SetNull();
        hashWitnessMerkleRoot.SetNull();
        nTime = 0;
        nBits = 0;
        nNonce.SetNull();
        nSolution.clear();
    }

    bool IsNull() const
    {
        return (nBits == 0);
    }

    uint256 GetHash() const;

    int64_t GetBlockTime() const
    {
        return (int64_t)nTime;
    }
};


class CBlock : public CBlockHeader
{
public:
    // network and disk
    std::vector<CTransactionRef> vtx;

    // memory only
    mutable bool fChecked;

    CBlock()
    {
        SetNull();
    }

    CBlock(const CBlockHeader &header)
    {
        SetNull();
        *((CBlockHeader*)this) = header;
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(*(CBlockHeader*)this);
        READWRITE(vtx);
    }

    void SetNull()
    {
        CBlockHeader::SetNull();
        vtx.clear();
        fChecked = false;
    }

    CBlockHeader GetBlockHeader() const
    {
        CBlockHeader block;
        block.nMajorVersion  = nMajorVersion;
        block.nMinorVersion       = nMinorVersion;
        block.hashPrevBlock  = hashPrevBlock;
        block.hashMerkleRoot = hashMerkleRoot;
        block.hashWitnessMerkleRoot = hashWitnessMerkleRoot;
        block.nTime          = nTime;
        block.nBits          = nBits;
        block.nNonce         = nNonce;
        block.nSolution      = nSolution;
        return block;
    }

    std::string ToString() const;
};

/**
 * Custom serializer for CBlockHeader that omits the nonce and solution, for use
 * as input to Equihash.
 */
class CEquihashInput : private CBlockHeader
{
public:
    CEquihashInput(const CBlockHeader &header)
    {
        CBlockHeader::SetNull();
        *((CBlockHeader*)this) = header;
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(nMajorVersion);
        READWRITE(nMinorVersion);
        READWRITE(hashPrevBlock);
        READWRITE(hashMerkleRoot);
        READWRITE(hashWitnessMerkleRoot);
        READWRITE(nTime);
        READWRITE(nBits);
    }
};

/** Describes a place in the block chain to another node such that if the
 * other node doesn't have the same branch, it can find a recent common trunk.
 * The further back it is, the further before the fork it may be.
 */
struct CBlockLocator
{
    std::vector<uint256> vHave;

    CBlockLocator() {}

    CBlockLocator(const std::vector<uint256>& vHaveIn) : vHave(vHaveIn) {}

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        int nVersion = s.GetVersion();
        if (!(s.GetType() & SER_GETHASH))
            READWRITE(nVersion);
        READWRITE(vHave);
    }

    void SetNull()
    {
        vHave.clear();
    }

    bool IsNull() const
    {
        return vHave.empty();
    }
};

#endif // BITCOIN_PRIMITIVES_BLOCK_H
