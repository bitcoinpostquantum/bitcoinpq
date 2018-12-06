// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Copyright (c) 2018 The Bitcoin Post-Quantum developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "primitives/block.h"

#include "hash.h"
#include "tinyformat.h"
#include "utilstrencodings.h"
#include "chainparams.h"
#include "consensus/params.h"
#include "crypto/common.h"


uint256 CBlockHeader::GetHash() const
{
    CHashWriter writer(SER_GETHASH, PROTOCOL_VERSION);

    if (nMajorVersion >= BPQ_MAJOR_VERSION) {
        ::Serialize(writer, *this);
    } else {
        SerializeLegacy(writer);
    }

    return writer.GetHash();
}

std::string CBlock::ToString() const
{
    std::stringstream s;
    s << strprintf("CBlock(hash=%s, ver=%u.%08x, hashPrevBlock=%s, hashMerkleRoot=%s, nTime=%u, nBits=%08x, nNonce=%s, vtx=%u)\n",
        GetHash().ToString(),
                   nMajorVersion,
        nMinorVersion,
        hashPrevBlock.ToString(),
        hashMerkleRoot.ToString(),
        nTime, nBits, nNonce.GetHex(),
        vtx.size());
    for (const auto& tx : vtx) {
        s << "  " << tx->ToString() << "\n";
    }
    return s.str();
}
