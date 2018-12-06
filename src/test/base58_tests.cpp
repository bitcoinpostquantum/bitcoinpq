// Copyright (c) 2011-2017 The Bitcoin Core developers
// Copyright (c) 2018 The Bitcoin Post-Quantum developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <base58.h>

#include <test/data/base58_encode_decode.json.h>
#include <test/data/base58_keys_invalid.json.h>
#include <test/data/base58_keys_valid.json.h>

#include <key.h>
#include <script/script.h>
#include <test/test_bitcoin.h>
#include <uint256.h>
#include <util.h>
#include <utilstrencodings.h>

#include <univalue.h>

#include <boost/test/unit_test.hpp>


extern UniValue read_json(const std::string& jsondata);

BOOST_FIXTURE_TEST_SUITE(base58_tests, BasicTestingSetup)

// Goal: test low-level base58 encoding functionality
BOOST_AUTO_TEST_CASE(base58_EncodeBase58)
{
    UniValue tests = read_json(std::string(json_tests::base58_encode_decode, json_tests::base58_encode_decode + sizeof(json_tests::base58_encode_decode)));
    for (unsigned int idx = 0; idx < tests.size(); idx++) {
        UniValue test = tests[idx];
        std::string strTest = test.write();
        if (test.size() < 2) // Allow for extra stuff (useful for comments)
        {
            BOOST_ERROR("Bad test: " << strTest);
            continue;
        }
        std::vector<unsigned char> sourcedata = ParseHex(test[0].get_str());
        std::string base58string = test[1].get_str();
        BOOST_CHECK_MESSAGE(
                    EncodeBase58(sourcedata.data(), sourcedata.data() + sourcedata.size()) == base58string,
                    strTest);
    }
}

// Goal: test low-level base58 decoding functionality
BOOST_AUTO_TEST_CASE(base58_DecodeBase58)
{
    UniValue tests = read_json(std::string(json_tests::base58_encode_decode, json_tests::base58_encode_decode + sizeof(json_tests::base58_encode_decode)));
    std::vector<unsigned char> result;

    for (unsigned int idx = 0; idx < tests.size(); idx++) {
        UniValue test = tests[idx];
        std::string strTest = test.write();
        if (test.size() < 2) // Allow for extra stuff (useful for comments)
        {
            BOOST_ERROR("Bad test: " << strTest);
            continue;
        }
        std::vector<unsigned char> expected = ParseHex(test[0].get_str());
        std::string base58string = test[1].get_str();
        BOOST_CHECK_MESSAGE(DecodeBase58(base58string, result), strTest);
        BOOST_CHECK_MESSAGE(result.size() == expected.size() && std::equal(result.begin(), result.end(), expected.begin()), strTest);
    }

    BOOST_CHECK(!DecodeBase58("invalid", result));

    // check that DecodeBase58 skips whitespace, but still fails with unexpected non-whitespace at the end.
    BOOST_CHECK(!DecodeBase58(" \t\n\v\f\r skip \r\f\v\n\t a", result));
    BOOST_CHECK( DecodeBase58(" \t\n\v\f\r skip \r\f\v\n\t ", result));
    std::vector<unsigned char> expected = ParseHex("971a55");
    BOOST_CHECK_EQUAL_COLLECTIONS(result.begin(), result.end(), expected.begin(), expected.end());
}

BOOST_AUTO_TEST_CASE(is_addess_bpq_test)
{
    BOOST_CHECK(!IsDestinationBPQ(CTxDestination(CNoDestination())));
    BOOST_CHECK(!IsDestinationBPQ(CTxDestination(CKeyID())));
    BOOST_CHECK(!IsDestinationBPQ(CTxDestination(CScriptID())));
    BOOST_CHECK(!IsDestinationBPQ(CTxDestination(WitnessV0KeyHash())));
    BOOST_CHECK(!IsDestinationBPQ(CTxDestination(WitnessV0ScriptHash())));
    BOOST_CHECK(IsDestinationBPQ(CTxDestination(WitnessV1ScriptHash())));
    BOOST_CHECK(!IsDestinationBPQ(CTxDestination(WitnessUnknown())));
}

BOOST_AUTO_TEST_CASE(base58_keys_valid_parse_0)
{
	BOOST_TEST_MESSAGE( "===== base58_keys_valid_parse_0 =====" );
	
    SelectParams(CBaseChainParams::MAIN);

	if (1)
	{
		// public key
		// chain: main
		std::string exp_base58 = "1AGNa15ZQXAZUgFiqJ2i7Z2DPU2J6hW62i";
		auto exp_payload = ParseHex("76a91465a16059864a2fdbc7c99a4723a8395bc6f188eb88ac");
		BOOST_TEST_MESSAGE( "base58: " + exp_base58 );
		BOOST_TEST_MESSAGE( "exp_payload: " + HexStr(exp_payload));
		
        SelectParams(CBaseChainParams::MAIN);

		std::vector<unsigned char> v;
		//BOOST_CHECK_MESSAGE(DecodeBase58Check(exp_base58,v), "base58 inv: " + exp_base58);
		//BOOST_TEST_MESSAGE( "base58: " + HexStr(v) );
		CBitcoinSecret secret;
		secret.SetString(exp_base58);
		BOOST_TEST_MESSAGE( "base58: decoded " + secret.ToString() );
		
	    CTxDestination destination;
		
		destination = DecodeDestination(exp_base58);
		BOOST_TEST_MESSAGE( "destination: " + boost::get<CKeyID>(destination).ToString());

		CScript script = GetScriptForDestination(destination);
		BOOST_CHECK_MESSAGE(IsValidDestination(destination), "!IsValid:");
		BOOST_CHECK_EQUAL(HexStr(script), HexStr(exp_payload));
	}
	
	if (1)
	{
		// public script
		// chain: main
		std::string exp_base58 = "3CMNFxN1oHBc4R1EpboAL5yzHGgE611Xou";
		auto exp_payload = ParseHex("a91474f209f6ea907e2ea48f74fae05782ae8a66525787");
		BOOST_TEST_MESSAGE( "base58: " + exp_base58 );
		BOOST_TEST_MESSAGE( "exp_payload: " + HexStr(exp_payload));
		
        SelectParams(CBaseChainParams::MAIN);

		std::vector<unsigned char> v;
		//BOOST_CHECK_MESSAGE(DecodeBase58Check(exp_base58,v), "base58 inv: " + exp_base58);
		//BOOST_TEST_MESSAGE( "base58: " + HexStr(v) );
		CBitcoinSecret secret;
		secret.SetString(exp_base58);
		BOOST_TEST_MESSAGE( "base58: decoded " + secret.ToString() );
		
	    CTxDestination destination;
		
		destination = DecodeDestination(exp_base58);
		BOOST_TEST_MESSAGE( "destination: " + boost::get<CScriptID>(destination).ToString());

		CScript script = GetScriptForDestination(destination);
		BOOST_CHECK_MESSAGE(IsValidDestination(destination), "!IsValid:");
		BOOST_CHECK_EQUAL(HexStr(script), HexStr(exp_payload));
	}

	if (1)
	{
		// public key
		// chain: regtest
		std::string exp_base58 = "mo9ncXisMeAoXwqcV5EWuyncbmCcQN4rVs";
		auto exp_payload = ParseHex("76a91453c0307d6851aa0ce7825ba883c6bd9ad242b48688ac");
		BOOST_TEST_MESSAGE( "base58: " + exp_base58 );
		BOOST_TEST_MESSAGE( "exp_payload: " + HexStr(exp_payload));
		
        SelectParams(CBaseChainParams::REGTEST);

		std::vector<unsigned char> v;
		//BOOST_CHECK_MESSAGE(DecodeBase58Check(exp_base58,v), "base58 inv: " + exp_base58);
		//BOOST_TEST_MESSAGE( "base58: " + HexStr(v) );
		CBitcoinSecret secret;
		secret.SetString(exp_base58);
		BOOST_TEST_MESSAGE( "base58: decoded " + secret.ToString() );
		
	    CTxDestination destination;
		
		destination = DecodeDestination(exp_base58);
		BOOST_TEST_MESSAGE( "destination: " + boost::get<CKeyID>(destination).ToString());

		CScript script = GetScriptForDestination(destination);
		BOOST_CHECK_MESSAGE(IsValidDestination(destination), "!IsValid:");
		BOOST_CHECK_EQUAL(HexStr(script), HexStr(exp_payload));
	}

	if (1)
	{
		// public script
		// chain: test
		std::string exp_base58 = "2N2JD6wb56AfK4tfmM6PwdVmoYk2dCKf4Br";
		auto exp_payload = ParseHex("a9146349a418fc4578d10a372b54b45c280cc8c4382f87");
		BOOST_TEST_MESSAGE( "base58: " + exp_base58 );
		BOOST_TEST_MESSAGE( "exp_payload: " + HexStr(exp_payload));
		
        SelectParams(CBaseChainParams::TESTNET);

		std::vector<unsigned char> v;
		//BOOST_CHECK_MESSAGE(DecodeBase58Check(exp_base58,v), "base58 inv: " + exp_base58);
		//BOOST_TEST_MESSAGE( "base58: " + HexStr(v) );
		CBitcoinSecret secret;
		secret.SetString(exp_base58);
		BOOST_TEST_MESSAGE( "base58: decoded " + secret.ToString() );
		
	    CTxDestination destination;
		
		destination = DecodeDestination(exp_base58);
		BOOST_TEST_MESSAGE( "destination: " + boost::get<CScriptID>(destination).ToString());

		CScript script = GetScriptForDestination(destination);
		BOOST_CHECK_MESSAGE(IsValidDestination(destination), "!IsValid:");
		BOOST_CHECK_EQUAL(HexStr(script), HexStr(exp_payload));
	}

	if (1)
	{
		// priv, !compressed
		std::string exp_base58 = "5Kd3NBUAdUnhyzenEwVLy9pBKxSwXvE9FMPyR4UKZvpe6E3AgLr";
		auto exp_payload = ParseHex("eddbdc1168f1daeadbd3e44c1e3f8f5a284c2029f78ad26af98583a499de5b19");
		BOOST_TEST_MESSAGE( "base58: " + exp_base58 );
		
        SelectParams(CBaseChainParams::MAIN);
		
		CBitcoinSecret secret;
		secret.SetString(exp_base58);
		BOOST_TEST_MESSAGE( "secret: " + secret.ToString() );
		BOOST_TEST_MESSAGE( HexStr(exp_payload) );
		CKey key = secret.GetKey();
		BOOST_TEST_MESSAGE( HexStr(key) );
	}
}

BOOST_AUTO_TEST_CASE(base58_keys_bpq)
{
	CKey key1;
	key1.MakeNewKey(CKeyType::XMSS_256_H10);
	BOOST_CHECK(key1.IsValid() && key1.IsXMSS());
	
    // test private keys
    CBitcoinSecret secret1;
	secret1.SetKey(key1);
	
	auto str1 = secret1.ToString();
	BOOST_TEST_MESSAGE("secret1:  size: " << str1.size() <<  ", " << str1 );
	BOOST_TEST_MESSAGE("key1: " << HexStr(key1.raw_short_key()));

	BOOST_CHECK_MESSAGE( str1.size() == 200, "invalid key encoding size: " << str1.size());
	
    CBitcoinSecret secret2;
	secret2.SetString(str1);
	BOOST_CHECK(secret2.ToString() == str1);

	BOOST_TEST_MESSAGE("secret2: " << secret2.ToString());
	
	CKey key2(secret2.GetKey());
	BOOST_CHECK(key2.IsValid());
	BOOST_CHECK_MESSAGE(key2.IsXMSS(), "key2: " << HexStr(key2.raw_short_key()));
	BOOST_CHECK(key1 == key2);
    
    // test public keys
    
    CPubKey pubkey1 = key1.GetPubKey();
	BOOST_CHECK(pubkey1.IsXMSS());

    CScript reedemScript = GetScriptForRawPubKey(pubkey1);
	BOOST_TEST_MESSAGE("pubkey1 reedemScript: " << HexStr(reedemScript));

    CTxDestination dest = WitnessV1ScriptHash(reedemScript);
  	BOOST_CHECK(IsValidDestination(dest));
  	BOOST_CHECK(IsDestinationBPQ(dest));
    auto destid1 = HexStr(boost::get<WitnessV1ScriptHash>(dest));
	BOOST_TEST_MESSAGE("dest id: " << destid1);
    
    auto destScript = GetScriptForDestination(dest);
	BOOST_TEST_MESSAGE("pubkey1 destScript: " << HexStr(destScript));
    
    // test ExtractDestination
    CTxDestination dest2;
    BOOST_CHECK(ExtractDestination(destScript, dest2));
  	BOOST_CHECK(IsValidDestination(dest2));
  	BOOST_CHECK(IsDestinationBPQ(dest2));

    std::string destid2 = HexStr(boost::get<WitnessV1ScriptHash>(dest2));
    BOOST_CHECK_MESSAGE(destid1 == destid2, destid1 << " != " << destid2);
    
    // test EncodeDestination
    std::string address = EncodeDestination(dest);
	BOOST_TEST_MESSAGE("pubkey1 address: " << address);

    // test DecodeDestination
    dest2 = DecodeDestination(address);
  	BOOST_CHECK(IsValidDestination(dest2));
  	BOOST_CHECK(IsDestinationBPQ(dest2));
    
    destid2 = HexStr(boost::get<WitnessV1ScriptHash>(dest2));
    
    BOOST_CHECK_MESSAGE(destid1 == destid2, destid1 << " != " << destid2);
}

// Goal: check that generated keys match test vectors
BOOST_AUTO_TEST_CASE(base58_keys_valid_gen)
{
    UniValue tests = read_json(std::string(json_tests::base58_keys_valid, json_tests::base58_keys_valid + sizeof(json_tests::base58_keys_valid)));

    for (unsigned int idx = 0; idx < tests.size(); idx++) {
        UniValue test = tests[idx];
        std::string strTest = test.write();
        if (test.size() < 3) // Allow for extra stuff (useful for comments)
        {
            BOOST_ERROR("Bad test: " << strTest);
            continue;
        }
        std::string exp_base58string = test[0].get_str();
        std::vector<unsigned char> exp_payload = ParseHex(test[1].get_str());
        const UniValue &metadata = test[2].get_obj();
        bool isPrivkey = find_value(metadata, "isPrivkey").get_bool();
        SelectParams(find_value(metadata, "chain").get_str());
		bool isXMSS = !find_value(metadata, "isXMSS").isNull() && find_value(metadata, "isXMSS").get_bool();
        if (isPrivkey) 
		{
			CKey key;
			
			if (isXMSS)
			{
				key.Set(exp_payload.begin(), exp_payload.end());
				BOOST_REQUIRE(key.IsValid());
			} else
			{
				bool isCompressed = find_value(metadata, "isCompressed").get_bool();
				key.SetLegacy(exp_payload.begin(), exp_payload.end(), isCompressed);
				BOOST_REQUIRE(key.IsValid());
				
				BOOST_CHECK( key.IsCompressed() == isCompressed );
				//auto && raw = key.raw_private_key_lagacy();
				//BOOST_CHECK_MESSAGE( raw.size() == exp_payload.size() && 
				//		std::equal(raw.begin(), raw.end(), exp_payload.begin()), 
				//		"result mismatch: " + HexStr(raw) + " != " + HexStr(exp_payload) );
			}

			BOOST_CHECK( key.IsXMSS() == isXMSS );
			
			CBitcoinSecret secret;
			secret.SetKey(key);
			BOOST_CHECK_MESSAGE(secret.ToString() == exp_base58string, 
				"result mismatch: " << Params().Base58Prefix(CChainParams::SECRET_KEY)[0] << ", " << secret.ToString() << " != " << exp_base58string);
			
        } else {
            CTxDestination dest;
            CScript exp_script(exp_payload.begin(), exp_payload.end());
            ExtractDestination(exp_script, dest);
            std::string address = EncodeDestination(dest);

            BOOST_CHECK_EQUAL(address, exp_base58string);
        }
    }

    SelectParams(CBaseChainParams::MAIN);
}

// Goal: check that parsed keys match test payload
BOOST_AUTO_TEST_CASE(base58_keys_valid_parse)
{
	BOOST_TEST_MESSAGE( "===== base58_keys_valid_parse =====" );

    UniValue tests = read_json(std::string(json_tests::base58_keys_valid, json_tests::base58_keys_valid + sizeof(json_tests::base58_keys_valid)));
    CBitcoinSecret secret;
    CTxDestination destination;
    SelectParams(CBaseChainParams::MAIN);

    for (unsigned int idx = 0; idx < tests.size(); idx++) {
        UniValue test = tests[idx];
        std::string strTest = test.write();
        if (test.size() < 3) { // Allow for extra stuff (useful for comments)
            BOOST_ERROR("Bad test: " << strTest);
            continue;
        }
        std::string exp_base58string = test[0].get_str();
        std::vector<unsigned char> exp_payload = ParseHex(test[1].get_str());
        const UniValue &metadata = test[2].get_obj();
        bool isPrivkey = find_value(metadata, "isPrivkey").get_bool();
        SelectParams(find_value(metadata, "chain").get_str());
		BOOST_TEST_MESSAGE( "chain: " + find_value(metadata, "chain").get_str() );
		
        bool try_case_flip = find_value(metadata, "tryCaseFlip").isNull() ? false : find_value(metadata, "tryCaseFlip").get_bool();
		bool isXMSS = !find_value(metadata, "isXMSS").isNull() && find_value(metadata, "isXMSS").get_bool();
        if (isPrivkey) 
		{
			BOOST_CHECK_MESSAGE(secret.SetString(exp_base58string), "!SetString:"+ strTest);
			BOOST_CHECK_MESSAGE(secret.IsValid(), "!IsValid:" + strTest);

			// Must be valid private key
			CKey privkey = secret.GetKey();

			auto short_key = privkey.raw_short_key();
			
			if (!isXMSS)
			{
				bool isCompressed = find_value(metadata, "isCompressed").get_bool();
				BOOST_CHECK_MESSAGE(privkey.IsCompressed() == isCompressed, "compressed mismatch:" + strTest);
				
				BOOST_CHECK_MESSAGE(privkey.size() == exp_payload.size() && 
					std::equal(privkey.begin(), privkey.end(), exp_payload.begin()), 
					"key mismatch:" << HexStr(privkey) << " != " << HexStr(exp_payload));

			} else
			{
				BOOST_CHECK_MESSAGE(short_key.size() == exp_payload.size() && 
					std::equal(short_key.begin(), short_key.end(), exp_payload.begin()), 
					"key mismatch:" << HexStr(short_key) << " != " << HexStr(exp_payload));
			}
			
			
			// Private key must be invalid public key
			destination = DecodeDestination(exp_base58string);
			BOOST_CHECK_MESSAGE(!IsValidDestination(destination), "IsValid privkey as pubkey:" + strTest);
        } else {
			
			BOOST_TEST_MESSAGE( "exp_base58string: " + exp_base58string );
			BOOST_TEST_MESSAGE( "exp_payload: " + HexStr(exp_payload));

            // Must be valid public key
            destination = DecodeDestination(exp_base58string);
            BOOST_CHECK(IsValidDestination(destination));
            
            CScript script = GetScriptForDestination(destination);
            BOOST_CHECK_MESSAGE(IsValidDestination(destination), "!IsValid:" + strTest);
            BOOST_CHECK_EQUAL(HexStr(script), HexStr(exp_payload));

            // Try flipped case version
            for (char& c : exp_base58string) {
                if (c >= 'a' && c <= 'z') {
                    c = (c - 'a') + 'A';
                } else if (c >= 'A' && c <= 'Z') {
                    c = (c - 'A') + 'a';
                }
            }
            destination = DecodeDestination(exp_base58string);
            BOOST_CHECK_MESSAGE(IsValidDestination(destination) == try_case_flip, "!IsValid case flipped:" + strTest);
            if (IsValidDestination(destination)) {
                script = GetScriptForDestination(destination);
                BOOST_CHECK_EQUAL(HexStr(script), HexStr(exp_payload));
            }

            // Public key must be invalid private key
            secret.SetString(exp_base58string);

            BOOST_CHECK_MESSAGE(!secret.IsValid(), "IsValid pubkey as privkey:" + strTest);
        }
    }
}

// Goal: check that base58 parsing code is robust against a variety of corrupted data
BOOST_AUTO_TEST_CASE(base58_keys_invalid)
{
    UniValue tests = read_json(std::string(json_tests::base58_keys_invalid, json_tests::base58_keys_invalid + sizeof(json_tests::base58_keys_invalid))); // Negative testcases
    CBitcoinSecret secret;
    CTxDestination destination;

    for (unsigned int idx = 0; idx < tests.size(); idx++) {
        UniValue test = tests[idx];
        std::string strTest = test.write();
        if (test.size() < 1) // Allow for extra stuff (useful for comments)
        {
            BOOST_ERROR("Bad test: " << strTest);
            continue;
        }
        std::string exp_base58string = test[0].get_str();

        // must be invalid as public and as private key
        for (auto chain : { CBaseChainParams::MAIN, CBaseChainParams::TESTNET, CBaseChainParams::REGTEST }) {
            SelectParams(chain);
            destination = DecodeDestination(exp_base58string);
            BOOST_CHECK_MESSAGE(!IsValidDestination(destination), "IsValid pubkey in mainnet:" + strTest);
            secret.SetString(exp_base58string);
            BOOST_CHECK_MESSAGE(!secret.IsValid(), "IsValid privkey in mainnet:" + strTest);
        }
    }
}


BOOST_AUTO_TEST_SUITE_END()
