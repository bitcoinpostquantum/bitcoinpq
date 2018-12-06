// Copyright (c) 2018 The Bitcoin Post-Quantum developers

#include <set>
#include <stdint.h>
#include <utility>
#include <vector>

#include <consensus/validation.h>
#include <policy/policy.h>
#include <chainparams.h>

#include <validation.h>
#include <consensus/tx_verify.h>

#include <miner.h>

#include <wallet/wallet.h>
#include <wallet/coincontrol.h>
#include <wallet/test/wallet_test_fixture.h>

#include <test/test_bitcoin.h>
#include <boost/test/unit_test.hpp>


class BpqWalletTestingSetup : public TestingSetup
{
public:
	
	BpqWalletTestingSetup() : TestingSetup(CBaseChainParams::REGTEST)
	{
		// CreateAndProcessBlock() does not support building SegWit blocks, so don't activate in these tests.
		// TODO: fix the code to support SegWit blocks.
		//UpdateVersionBitsParameters(Consensus::DEPLOYMENT_SEGWIT, 0, Consensus::BIP9Deployment::NO_TIMEOUT);
		UpdateVersionBitsParameters(Consensus::DEPLOYMENT_SEGWIT, 0, 0);
		
		// legacy keys
		legacyCoinbaseKey.MakeNewKey(CKeyType::ECDSA_COMPRESSED);
		legacyKey1.MakeNewKey(CKeyType::ECDSA_COMPRESSED);
		legacyKey2.MakeNewKey(CKeyType::ECDSA_COMPRESSED);

		// premine keys
		
		auto key00 = ParseHex("10010000017E9AC3780FD65FEE94E9FCC572644F49D00E047D136570BF3BC85D57D7E05E996F225C419237D42DC30B1D887F42E806CFFE8324D09332A6DAAC8D736B7438FC00000000000000008523A051B3F19C114F2483FFA322B6271415D7F3AC21960425357E7AC92C304071F8EDE182C95E56789DC19C68B1F79E394E4158C54A8C14620DA91488875541");
		auto key01 = ParseHex("1001000001DEB3BE633B2DAC0F4E142F5427F5CFAFB3D3DC7A8A522593D7A0C8A6314BB35C80CB9F3900BC22C406500B5A8E1587BB9C77CD18BBF7BAFDAA4B4F0BEC57C9E70000000000000000C45881D45382A7168350F33F2B03C141EDF4CB443AF9E9D99AC6DCF2783FAFBFF7FE6B3D5A79CB0BE0563AF1C6B339D19629DD161F2E9001DB7CE6B018CA1DCF");
		auto key02 = ParseHex("1001000001AE96AE6013ED9EB832029EC50B97EC55392E54546DC3AF4BCAF3280C136FD91319ABD9FC8F87E09AB7025E3E0EA16780682B2A44D07D9D58CDF698718A4F13B900000000000000009B8F35C0830C1F317DFE6D978F986233260985ACE576B273545C726F0C5CD0B225D2ACA6F9A4394F3368945A6865D9DAF44954F00F9E86A8618C58715C9AE7E1");
		auto key10 = ParseHex("1001000001C42EEFAB25F3455DA8D87D0795E41D79DFFAF77A2DC1BD44C7E8D70493E91BF64887DD8D20A9D15B98B085575E62BE492B03AB3E5FEC9F0FBE2CE38476953C570000000000000000EABEBF08E2F75E5B8BBCC0001EEDE97A9893D057421C3C4938152BF3881E27C25BF15EF265824868AB57E82DFFA5BC7887D0941227BB119C5E7F645B313A6649");
		auto key11 = ParseHex("10010000015D563467133142C3C0EEB19383BBEF2F8F4C55311D45BE474E9A74E5299847024CAAA3A00C603DB2AD67DA9F583C7D5BCF94241BC4EDFC8B49549EAC5D61FFC500000000000000001579A651FF55C489BBCC546CF0F67356F585547BE76E9CFD7620CB142AD5197E0172CEF15177C3CD2AD2733D806DDECBFD3BAE1C02F02B1CFA43FEBF04782FDC");
		auto key12 = ParseHex("1001000001658B52B84135626EABCA7E926333274BDFFECA08052E2C99D227E4EFFA2CADE4FE653498983A2B9498776229E53A7A126F4F3D398E3C29A049A1B46098C98F100000000000000000A8680362D957C8640B3A6F03CEAA66F74D946D9B4D4AC41465F87C12C9872A2FA355C99717FC68AB34920DB3C3BFB33F57A96C33C6520B7DE8075FD38DC43D10");
		
		premineKey[0][0].Set(key00.data(), key00.size());
		premineKey[0][1].Set(key01.data(), key01.size());
		premineKey[0][2].Set(key02.data(), key02.size());
		premineKey[1][0].Set(key10.data(), key10.size());
		premineKey[1][1].Set(key11.data(), key11.size());
		premineKey[1][2].Set(key12.data(), key12.size());

		std::vector<std::vector<std::string>> pubkeys(2);
		for (int i = 0; i < 2; ++i)
		{
			pubkeys[i].resize(3);
			for(int j = 0; j < 3; ++j)
			{
				premineKey[i][j].MakeNewKey(Params().ChainKeyType());
				pubkeys[i][j] = HexStr(ToByteVector(premineKey[i][j].GetPubKey())); 
			}
		}

		const_cast<CChainParams&>(Params()).UpdatePremineKeys(pubkeys);
		
		// mine keys
		coinbaseKey.MakeNewKey(Params().ChainKeyType());
		
		// init wallet
		g_address_type = OUTPUT_TYPE_BECH32_V1;
		g_change_type = OUTPUT_TYPE_BECH32_V1;

		addressKey1.MakeNewKey(Params().ChainKeyType());
		addressKey2.MakeNewKey(Params().ChainKeyType());
		
        destination1 = WitnessV1ScriptHash(GetScriptForRawPubKey(addressKey1.GetPubKey()));
        destination2 = WitnessV1ScriptHash(GetScriptForRawPubKey(addressKey2.GetPubKey()));
        
		//GeneratePremineChain();
		//CreateAndProcessBlock({}, GetScriptForRawPubKey(coinbaseKey.GetPubKey()));

		::bitdb.MakeMock();

		wallet.reset(new CWallet(std::unique_ptr<CWalletDBWrapper>(new CWalletDBWrapper(&bitdb, "wallet_test.dat"))));

		bool firstRun;
		wallet->LoadWallet(firstRun);
		AddKey(*wallet, coinbaseKey);

		WalletRescanReserver reserver(wallet.get());
		reserver.reserve();
		wallet->ScanForWalletTransactions(chainActive.Genesis(), nullptr, reserver);
	}

    ~BpqWalletTestingSetup()
	{
		wallet.reset();
		::bitdb.Flush(true);
		::bitdb.Reset();
	}

    // Create a new block with just given transactions, coinbase paying to
    // scriptPubKey, and try to add it to the current chain.
    bool CreateAndProcessBlock(const std::vector<CMutableTransaction>& txns,
                                 CScript scriptPubKey)
	{
		const CChainParams& chainparams = Params();
		
		int nNextHeight = chainActive.Height() + 1;
		
		if (Params().IsPremineBlock(nNextHeight))
			scriptPubKey = GetScriptForDestination(
                    Params().GetPremineCoinbaseDestination(nNextHeight));
		
		std::unique_ptr<CBlockTemplate> pblocktemplate = BlockAssembler(chainparams).CreateNewBlock(scriptPubKey);
		CBlock& block = pblocktemplate->block;
		
		auto ptx0 = MakeTransactionRef(*block.vtx[0]);
//		ptx0->vout.resize(1);

		// Replace mempool-selected txns with just coinbase plus passed-in txns:
		block.vtx.resize(1);
		for (const CMutableTransaction& tx : txns)
		{
			auto ptx = MakeTransactionRef(tx);
			block.vtx.push_back(ptx);
		}
		// IncrementExtraNonce creates a valid coinbase and merkleRoot
		unsigned int extraNonce = 0;
		{
			LOCK(cs_main);
			IncrementExtraNonce(&block, chainActive.Tip(), extraNonce);
		}

		while (!CheckProofOfWork(block.GetHash(), block.nBits, false, chainparams.GetConsensus())) 
			block.nNonce = ArithToUint256(UintToArith256(block.nNonce) + 1);

		std::shared_ptr<const CBlock> shared_pblock = std::make_shared<const CBlock>(block);

		if (!ProcessNewBlock(chainparams, shared_pblock, true, nullptr))
		{
			BOOST_CHECK_MESSAGE(false, "block not accepted: " << nNextHeight);
			return false;
		}
		
		BOOST_CHECK_MESSAGE(chainActive.Height() == nNextHeight, "current height: " << chainActive.Height() << ", expected height: " << nNextHeight);
		
		coinbaseTxns.push_back(block.vtx[0]->GetHash());
		
		for (auto & ptx : block.vtx)
		{
			mapTx[ptx->GetHash()] = ptx;
		}
	
		return true;
	}

	bool SignPremineTx(CMutableTransaction & tx, int height)
	{
		unsigned int nIn = 0;
		if (nIn >= tx.vin.size())
			throw std::invalid_argument(std::string(__func__) + ": nIn");
		
		const auto & input = tx.vin[nIn];

		auto mi = mapTx.find(input.prevout.hash);
		if(mi == mapTx.end() || input.prevout.n >= mi->second->vout.size())
		{
			BOOST_TEST_MESSAGE(__func__ << ": prev tx not found");
			return false;
		}
		
		CTxOut const & output = mi->second->vout[input.prevout.n];

		const CAmount& amount = output.nValue;

		auto & keys = premineKey[(height-Params().GetConsensus().BPQHeight) % 2];

		CScript scriptRedeem = Params().GetPremineCoinbaseRedeemScript(height);
		CScript scriptPubKey = GetScriptForDestination(WitnessV1ScriptHash(scriptRedeem));

		auto sig = Signature(scriptPubKey, tx, nIn, SIGHASH_ALL, amount, SIGVERSION_WITNESS_V1);

	    CScript         scriptSig;
        CScriptWitness  scriptWitness;

        scriptWitness.stack.push_back({});
        
   		for (int i = 0; i < 2; ++i)
		{
			const CKey &key = keys[i];
			std::vector<unsigned char> vchSig;
			if (!key.Sign(sig, vchSig))
				return false;

			vchSig.push_back((unsigned char)SIGHASH_ALL);
            
            scriptWitness.stack.push_back(vchSig);
		}
        
        scriptWitness.stack.push_back(ToByteVector(scriptRedeem));

        tx.vin[nIn].scriptSig = scriptSig;
		tx.vin[nIn].scriptWitness = scriptWitness;
		
		BOOST_TEST_MESSAGE("scriptPubKey: " << HexStr(output.scriptPubKey));
		BOOST_TEST_MESSAGE("scriptRedeem: " << HexStr(scriptRedeem));
		BOOST_TEST_MESSAGE("scriptSig: " << HexStr(scriptSig));
        
		MutableTransactionSignatureChecker checker(&tx, 0, amount);
		ScriptError serror;
		if (!VerifyScript(scriptSig, output.scriptPubKey, &scriptWitness, STANDARD_SCRIPT_VERIFY_FLAGS, checker, &serror))
		{
			BOOST_TEST_MESSAGE(__func__ << ": verify failed: " << serror << ", " << ScriptErrorString(serror));
			return false;
		}
		
		return true;
	}

	bool SignTx(CMutableTransaction & tx)
	{
		// sign the new tx
		CTransaction txNewConst(tx);

		for (unsigned int nIn = 0; nIn < tx.vin.size(); ++nIn) 
		{
			const auto & input = tx.vin[nIn];
			
			auto mi = mapTx.find(input.prevout.hash);
			if(mi == mapTx.end() || input.prevout.n >= mi->second->vout.size())
			{
				BOOST_TEST_MESSAGE(__func__ << ": prev tx not found");
				return false;
			}
			
			CTxOut const & output = mi->second->vout[input.prevout.n];

			const CAmount& amount = output.nValue;

			SignatureData sigdata;
			MutableTransactionSignatureCreator creator(wallet.get(), &tx, nIn, amount, SIGHASH_ALL);
			if (!ProduceSignature(creator, output.scriptPubKey, sigdata)) 
			{
				BOOST_TEST_MESSAGE(__func__ << ": signing failed");
				return false;
			}
			
			tx.vin[nIn].scriptSig = sigdata.scriptSig;
			tx.vin[nIn].scriptWitness = sigdata.scriptWitness;
		}
		return true;		
	}
	
	bool SpendCoins(uint256 const & prev_tx_hash, unsigned int nOut, CScript const & scriptPubKey, CMutableTransaction & tx)
	{
		auto mi = mapTx.find(prev_tx_hash);
		if(mi == mapTx.end() || nOut >= mi->second->vout.size())
		{
			BOOST_TEST_MESSAGE(__func__ << ": prev tx not found");
			return false;
		}
		
		CTxOut const & txo = mi->second->vout[nOut];
		
		tx.vin.resize(1);
		tx.vin[0].prevout.hash = prev_tx_hash;
		tx.vin[0].prevout.n = nOut;
		tx.vout.resize(1);
		tx.vout[0].scriptPubKey = scriptPubKey;
		tx.vout[0].nValue = txo.nValue;

		if (!SignTx(tx))
		{
			BOOST_TEST_MESSAGE(__func__ << ": SignTx failed");
			return false;
		}
		
		//BOOST_TEST_MESSAGE("tx: value: " << txo.nValue << ", scriptPubKey: " << HexStr(txo.scriptPubKey)
		//	<< ", in scriptSig: " << HexStr(tx.vin[0].scriptSig)
		//	<< ", in scriptWitness: " << tx.vin[0].scriptWitness.ToString()
		//	<< ", out scriptPubKey: " << HexStr(tx.vout[0].scriptPubKey));
		
		return true;
	}

	bool SpendPremineCoins(int height, CScript const & scriptPubKey, CMutableTransaction & tx)
	{
		if (!Params().IsPremineBlock(height))
			return false;
		
		auto & prev_tx_hash = coinbaseTxns[height-1];
		unsigned int nOut = 0;
		
		auto mi = mapTx.find(prev_tx_hash);
		if(mi == mapTx.end() || nOut >= mi->second->vout.size())
		{
			BOOST_TEST_MESSAGE(__func__ << ": prev tx not found");
			return false;
		}
		
		CTxOut const & txo = mi->second->vout[nOut];
		
		tx.vin.resize(1);
		tx.vin[0].prevout.hash = prev_tx_hash;
		tx.vin[0].prevout.n = nOut;
		tx.vout.resize(1);
		tx.vout[0].scriptPubKey = scriptPubKey;
		tx.vout[0].nValue = txo.nValue;

		if (!SignPremineTx(tx, height))
		{
			BOOST_TEST_MESSAGE(__func__ << ": SignPremineTx failed");
			return false;
		}
		
		//BOOST_TEST_MESSAGE("tx: value: " << txo.nValue << ", scriptPubKey: " << HexStr(txo.scriptPubKey)
		//	<< ", in scriptSig: " << HexStr(tx.vin[0].scriptSig)
		//	<< ", in scriptWitness: " << tx.vin[0].scriptWitness.ToString()
		//	<< ", out scriptPubKey: " << HexStr(tx.vout[0].scriptPubKey));
		
		return true;
	}

	bool CheckTx(CMutableTransaction const & mtx)
	{
		CTransactionRef ptx = MakeTransactionRef(mtx);
	    const CTransaction& tx = *ptx;
				
		bool fRequireStandard = true;
		bool fWitnessEnabled = true; // IsWitnessEnabled(chainActive.Tip(), Params().GetConsensus());
		
		CValidationState state;

		if (!CheckTransaction(tx, state))
			return false; // state filled in by CheckTransaction
		
		// Reject transactions with witness before segregated witness activates (override with -prematurewitness)
		if (tx.HasWitness() && !fWitnessEnabled) {
			return state.DoS(0, false, REJECT_NONSTANDARD, "no-witness-yet", true);
		}
		
		// Rather not work on nonstandard transactions (unless -testnet/-regtest)
		std::string reason;
		if (fRequireStandard && !IsStandardTx(tx, reason, fWitnessEnabled))
			return state.DoS(0, false, REJECT_NONSTANDARD, reason);
		
		return true;
	}

	bool ToMemPool(CMutableTransaction const & tx)
	{
		LOCK(cs_main);

		CValidationState state;
		if (!AcceptToMemoryPool(mempool, state, MakeTransactionRef(tx), nullptr, nullptr, false, 0))
		{
			BOOST_TEST_MESSAGE("AcceptToMemoryPool failed: " << state.GetRejectCode() << ", reason: " << state.GetRejectReason()<< ", message: " << state.GetDebugMessage() );
			return false;
		}

		return true;
	}
	
	static void AddKey(CWallet& wallet, const CKey& key)
	{
		LOCK(wallet.cs_wallet);
		wallet.AddKeyPubKey(key, key.GetPubKey());
	}

	CWalletTx& AddTx(CRecipient recipient)
	{
		CWalletTx wtx;
		CReserveKey reservekey(wallet.get());
		CAmount fee;
		int changePos = -1;
		std::string error;
		CCoinControl dummy;
		BOOST_CHECK(wallet->CreateTransaction({recipient}, wtx, reservekey, fee, changePos, error, dummy));
		CValidationState state;
		BOOST_CHECK(wallet->CommitTransaction(wtx, reservekey, nullptr, state));
		CMutableTransaction blocktx;
		{
			LOCK(wallet->cs_wallet);
			blocktx = CMutableTransaction(*wallet->mapWallet.at(wtx.GetHash()).tx);
		}
		CreateAndProcessBlock({CMutableTransaction(blocktx)}, GetScriptForRawPubKey(coinbaseKey.GetPubKey()));
		LOCK(wallet->cs_wallet);
		auto it = wallet->mapWallet.find(wtx.GetHash());
		BOOST_CHECK(it != wallet->mapWallet.end());
		it->second.SetMerkleBranch(chainActive.Tip(), 1);
		return it->second;
	}

	void GenerateLegacyChain()
	{
		BOOST_CHECK(legacyCoinbaseKey.IsValid());

		CScript scriptPubKey = CScript() << ToByteVector(legacyCoinbaseKey.GetPubKey()) << OP_CHECKSIG;

		for (int i = 0; i < COINBASE_MATURITY+2; i++)
		{
			BOOST_CHECK(CreateAndProcessBlock({}, scriptPubKey));
		}
		
		AddKey(*wallet, legacyCoinbaseKey);

		CMutableTransaction tx1, tx2;
		BOOST_CHECK(SpendCoins(coinbaseTxns[0], 0, GetScriptForDestination(legacyKey1.GetPubKey().GetID()), tx1));
		BOOST_CHECK(SpendCoins(coinbaseTxns[1], 0, GetScriptForDestination(legacyKey1.GetPubKey().GetID()), tx2));
		
		BOOST_CHECK(CreateAndProcessBlock( { tx1, tx2 }, scriptPubKey));

		AddKey(*wallet, legacyKey1);

		CMutableTransaction tx3;
		BOOST_CHECK(SpendCoins(tx1.GetHash(), 0, GetScriptForDestination(legacyKey2.GetPubKey().GetID()), tx3));
		
		BOOST_CHECK(CreateAndProcessBlock( { tx3 }, scriptPubKey));
		
		BOOST_REQUIRE(Params().GetConsensus().BPQHeight <= 400);

		while (chainActive.Height()+1 < Params().GetConsensus().BPQHeight)
		{
			BOOST_CHECK(CreateAndProcessBlock({}, scriptPubKey));
		}
		
		BOOST_TEST_MESSAGE("legacy chain last block: " << chainActive.Height());
		BOOST_TEST_MESSAGE("legacy chain coinbaseTxns size: " << coinbaseTxns.size());
	}

	void GeneratePremineChain()
	{
		BOOST_REQUIRE(chainActive.Height()+1 == Params().GetConsensus().BPQHeight);
		BOOST_REQUIRE((int)coinbaseTxns.size()+1 == Params().GetConsensus().BPQHeight);
		
		while (true)
		{
			int nNextHeight = chainActive.Height() + 1;
			if (nNextHeight >= Params().GetConsensus().BPQHeight + Params().GetConsensus().BPQPremineWindow)
				break;
			
            CScript reedemScript = Params().GetPremineCoinbaseRedeemScript(nNextHeight);
			CScript scriptPubKey = GetScriptForDestination(WitnessV1ScriptHash(reedemScript));
			CreateAndProcessBlock({}, scriptPubKey);
		}

		BOOST_TEST_MESSAGE("premine chain last block: " << chainActive.Tip()->nHeight);
		BOOST_TEST_MESSAGE("premine chain coinbaseTxns size: " << coinbaseTxns.size());
	}

    CKey legacyCoinbaseKey; // private/public key needed to spend coinbase transactions
    std::vector<uint256> coinbaseTxns; // For convenience, coinbase transactions
	std::map<uint256, CTransactionRef>	mapTx;
    CKey legacyKey1;
    CKey legacyKey2;
	
	CKey premineKey[2][3];
	
    CKey coinbaseKey; // private/public key needed to spend coinbase transactions

    CKey addressKey1;
    CKey addressKey2;
    
    CTxDestination destination1; // destination for addressKey1
    CTxDestination destination2; // destination for addressKey2
	
    std::unique_ptr<CWallet> wallet;
};

BOOST_FIXTURE_TEST_SUITE(bpq_tests, BpqWalletTestingSetup)

BOOST_AUTO_TEST_CASE(test_1)
{
    return;
    
	GenerateLegacyChain();
	
	GeneratePremineChain();

	CScript scriptCoinbase = GetScriptForDestination(coinbaseKey.GetPubKey().GetID());

	CScript scriptAddress1 = GetScriptForDestination(destination1);
	CScript scriptAddress2 = GetScriptForDestination(destination2);
    
	// first BPQ block
	BOOST_CHECK(CreateAndProcessBlock( {}, scriptCoinbase));
	
	// try spend premine coins
	
    for (int i = 0; i < 2; ++i)
    {
        CScript redeemScript = Params().GetPremineCoinbaseRedeemScript(Params().GetConsensus().BPQHeight+i);
        auto dest = WitnessV1ScriptHash(redeemScript);
        CScript witnessScript = GetScriptForDestination(dest);
    	wallet->AddCScript(redeemScript, 1);
    	wallet->AddCScript(witnessScript, 1);
    }

	//AddKey(*wallet, premineKey[0][0]);
	//AddKey(*wallet, premineKey[0][1]);
	//AddKey(*wallet, premineKey[0][2]);
	//AddKey(*wallet, premineKey[1][0]);
	//AddKey(*wallet, premineKey[1][1]);
	//AddKey(*wallet, premineKey[1][2]);
	
	CMutableTransaction tx1;
	CMutableTransaction tx2;

	//BOOST_CHECK(SpendCoins(coinbaseTxns[Params().GetConsensus().BPQHeight-1], 0, scriptAddress1, tx1));
	//BOOST_CHECK(SpendCoins(coinbaseTxns[Params().GetConsensus().BPQHeight], 0, scriptAddress1, tx2));
	BOOST_CHECK(SpendPremineCoins(Params().GetConsensus().BPQHeight+0, scriptAddress1, tx1));
	BOOST_CHECK(SpendPremineCoins(Params().GetConsensus().BPQHeight+1, scriptAddress1, tx2));

	//AddKey(*wallet, premineKey[1][1]);
	//AddKey(*wallet, premineKey[1][2]);
	
	// first BPQ block

	BOOST_CHECK(CreateAndProcessBlock( { tx1, tx2 }, scriptCoinbase));
	
	AddKey(*wallet, addressKey1);
	
	CMutableTransaction tx3;
	BOOST_CHECK(SpendCoins(tx1.GetHash(), 0, scriptAddress2, tx3));

	BOOST_CHECK(CreateAndProcessBlock( { tx3 }, scriptCoinbase));
	
	//AddTx(CRecipient{scriptAddress1, 1 * COIN, false /* subtract fee */});
}

BOOST_AUTO_TEST_SUITE_END()
