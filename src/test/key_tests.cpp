// Copyright (c) 2012-2017 The Bitcoin Core developers
// Copyright (c) 2018 The Bitcoin Post-Quantum developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <key.h>

#include <base58.h>
#include <script/script.h>
#include <uint256.h>
#include <util.h>
#include <utilstrencodings.h>
#include <test/test_bitcoin.h>

#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

static const std::string strSecret1 = "5HxWvvfubhXpYYpS3tJkw6fq9jE9j18THftkZjHHfmFiWtmAbrj";
static const std::string strSecret2 = "5KC4ejrDjv152FGwP386VD1i2NYc5KkfSMyv1nGy1VGDxGHqVY3";
static const std::string strSecret1C = "Kwr371tjA9u2rFSMZjTNun2PXXP3WPZu2afRHTcta6KxEUdm1vEw";
static const std::string strSecret2C = "L3Hq7a8FEQwJkW1M2GNKDW28546Vp5miewcCzSqUD9kCAXrJdS3g";
static const std::string addr1 = "1QFqqMUD55ZV3PJEJZtaKCsQmjLT6JkjvJ";
static const std::string addr2 = "1F5y5E5FMc5YzdJtB9hLaUe43GDxEKXENJ";
static const std::string addr1C = "1NoJrossxPBKfCHuJXT4HadJrXRE9Fxiqs";
static const std::string addr2C = "1CRj2HyM1CXWzHAXLQtiGLyggNT9WQqsDs";

static const std::string strAddressBad = "1HV9Lc3sNHZxwj4Zk6fB38tEmBryq2cBiF";

static const std::string strKey11 = "10010000016d294078745a5daa3d3d9a2e658d903b05eff19743397e06ef0ee419c9c9bb0b3bb2583c49212e185243f7e9415bd3bc44113b4ee1b66e65dd2b28b299a74c3000000000000000006fbda6eec50d7b11c942f99b0c83767ab0e68fa5a747bc683b95d26d4b206d2a8ed7f1a9b776129bd51f1e260d51de439ab5f8e79b0ccd93852d5f2bdfffdb01";
static const std::string strKey12 = "1001000001475cc0e246dabca106fcc7de92c89b67f0c600f5e8ff82fb46f26d8a4c86502184f2725b87de31334dc94c0a76344dcc2fc9118a63e1eab259cbd8c36011f2fd0000000000000000d95f1c7bb7c35d4cb2b5170c52c524b5d68b227224eeb2e693ea9f01dcffceb5440c2537ca9cc3abc43451739227742f6f777f0e84e752156173c39a80ba3871";
static const std::string strKey21 = "10040000041fbc5c165084cf9bd6cee5e8016ca89e5c27a49474ffde09f5bc45eaa392a3ab230f806e5e55a25bd41167a6dc9d7a6be3c201e6aca1d4d6953636249066477f51164514680c0ee228a6f17306368fe1a3fc1014445c7079fb4dfe9e0c2a70327e2c3fc177732dbc1509418d43579f5580ca6a88417adacf0067f5ac08f538f900000000000000007d434105028d15a9a327832726fe1a0d384c44bcc44e17e707bb87d0fb04285b31d415f1837f1a7ae50c5e0647bfc2430a71fbfaaeb5f67c7a9ae6c25939fc225678cb07d8ee084e1c6afa99570ea846390f50b7400d3649552257b12f9a3dfec8ce352a4fdfd1e550cf0d5a71f0ba563e0e574c3fc9aceb7d507bc7a41ec7e8";
static const std::string strKey22 = "10040000042fadf2f71ae09ed07125751bd90bba049cdbb082056fd0bb2504417f30a1ce7e75e242a2ee50af22c2c813cbcf8fe34e7da9cf906c334e6cb6e2ef59ad2f2611f1bfc72d47df2dbef54c9093606b810c5a8626811a358e6f79ac98100ead27c0bfcfedb81f534c8fec50577b9b754804599c9e221d0d37c74dd99acea3d8414d0000000000000000907740b099f5d21a45bc800ac4eaf24199fe8e5b59e301cbe626808f8a087f39f398d1cab199f7ea1e2262d24b73c7e38b03103f18e765e5cfd578c2f7049a8409dd295d3d060355bac7dd9c2a6f2d11b3743cc7094e8cee09b0e61b23c2d0bcbe95a1940f1bdf2bee96ce9e8619b14ae4ec85ce016c9c8d27c990f537dc1bd4";

static const std::string strSecret11 = "3Q9Xy2HFk3f7HgdbbN6jCScdQQeSfYsJsj2AZiujVCCZFACvKVTu5LgHVreVGJkGPfbv4WHJpjtQwTMpYr26HfBpUZXjGfZqhvHBe9uRZW6CD8nPfJXu1coKkvTw8hi6DUBvtoN6AZAu4KCZqZcs1YRMUpaMQiJEoQeXR5BHmrmYqwLfupWVxceXTC3KiuNdeDLcvGqz";
static const std::string strSecret12 = "3Q9Xy2HFjiyKpxMQDbKXvYMZspsBf7Hvfe2FniZqxGp3aANCLziucycXchLMowB8qEWXHvc5pQRWkD21dGAfvVvUxxW58Kg7bBccYxd6JWEX7TeoEBMDBktCE6bYACRS62ut44GkPR3j2eQU3BfGb3V5KEkfV3tQpYjuNFBAEGQW2ZwAnzRX51ydLjP492yw1gCuREEP";
static const std::string strSecret21 = "25rBW9uY3A69129eJqWPxnaYU7UxMXgr6FUqAxcvinXVLcKbD1XKnx7kDCPkfVRzY3HdFhGqfAXh8c7KFiQHpA5kEMkfvwWozcoE5ZmSiumwy3Twd8mLRqwmR9aX3ZQHsYDnoAWmLgwjdzRwc8LKRD2Rka9QhiSPopHqJJS8aVyK4eLtPyEacQLwMwvwVSy7Rwd1rUFRvLvC5pTJ1z87snhCb9ymXy1HjBucSdQhEE8VALBnB7jeaQdz8z2gU3yx4NCSUnpmsnBJMpREZfM8yFdhwWyuRHwsUcxbbbJUvrDKgwp2gc8h5RGtMXLCXhw5wQzJxuoMoubR1NZHDzjdDqAkyWMVjPq5STQQntGLoD29gukWbTv5v5E";
static const std::string strSecret22 = "25rBW9uY3Decde7CWUj5PMeHnezYgVYJjJ7NoD82ozJEWJ2ZzZbpJGhzbpQAYucjVALVmZcUEvghxi6oUU5x2nmotfCtfhmD4mbWVmMqojpdBrAMKBz9eA4AgK1Gwi7dvJmPTTgctS1en2qujTtF7EH1FU1nXnxmM3UL82Ut4BdTb8q63cPXUqEFDZov8gVEvYwEpJsV1PzQh9QzHcxC2FNBTu9XWRkWtRxupRxANeZRChCUxYeB6kkJkfaMhFuyvZeaVYpZvoFUtpe67Z6kBbrKVeVWhjGPF6BX5fDN1Z96cyvV4nkP94EoupwZzyPEQ2yRqCNgQ7Mi5mDS9gUF54En6SKxFA7p6uhCv27h823GrHB3m5uZyPm";

BOOST_FIXTURE_TEST_SUITE(key_tests, BasicTestingSetup)

BOOST_AUTO_TEST_CASE(key_test_base1)
{
	std::string exp_pub1 = "0497e922cac2c9065a0cac998c0735d9995ff42fb6641d29300e8c0071277eb5b4e770fcc086f322339bdefef4d5b51a23d88755969d28e965dacaaa5d0d2a0e09";
	std::string exp_pub2 = "0339a36013301597daef41fbe593a02cc513d0b55527ec2df1050e2e8ff49c85c2";
	std::string exp_pub3 = "026557fdda1d5d43d79611f784780471f086d58e8126b8c40acb82272a7712e7f2";

    CPubKey pubkey1(ParseHex(exp_pub1));
	CPubKey pubkey2(ParseHex(exp_pub1));
	CPubKey pubkey3(ParseHex(exp_pub2));
	
    BOOST_CHECK( HexStr(pubkey1) == exp_pub1 );
    BOOST_CHECK( pubkey1 == pubkey2 );
    BOOST_CHECK( pubkey1 != pubkey3 );

	// serialisation
	{
		CDataStream ss1(SER_DISK, CLIENT_VERSION);
		ss1 << pubkey1;
		CPubKey pubkey1x;
		ss1 >> pubkey1x;
	    BOOST_CHECK( pubkey1 == pubkey1x );
	}

	{
		CDataStream ss1(SER_DISK, CLIENT_VERSION);
		ss1 << pubkey2;
		CPubKey pubkey2x;
		ss1 >> pubkey2x;
	    BOOST_CHECK( pubkey2 == pubkey2x );
	}
	
	auto exp_key1 = ParseHex("129ee831061c31be7d636fc0402fd2299855f40015a2c60b2e5755c57270460d");
	auto exp_key2 = ParseHex("f1c7c871a54a804afe328b4c83a1c33b8e5ff48f5087273f04efa83b247d6a2d");
	auto exp_key3 = ParseHex("877c779ad9687164e9c2f4f0f4ff0340814392330693ce95a58fe18fd52e6e93");
	
    CKey key1; key1.SetLegacy(exp_key1.data(), exp_key1.data() + exp_key1.size());
	CKey key2; key2.SetLegacy(exp_key2.data(), exp_key2.data() + exp_key2.size());
	CKey key3; key3.SetLegacy(exp_key3.data(), exp_key3.data() + exp_key3.size());

    BOOST_CHECK( key1 != key2 );
    BOOST_CHECK( key1 != key3 );
}

BOOST_AUTO_TEST_CASE(key_test_base2)
{
	auto seed = ParseHex("000102030405060708090a0b0c0d0e0f");

    CExtKey key;
    key.SetMaster(seed.data(), seed.size());

	CDataStream ssPriv(SER_DISK, CLIENT_VERSION);
	ssPriv << key;
	BOOST_CHECK(ssPriv.size() == 75);

    CExtKey keyc;
	ssPriv >> keyc;
	
	BOOST_CHECK_MESSAGE(key == keyc, HexStr(key.key) << " != " << HexStr(keyc.key));
}

BOOST_AUTO_TEST_CASE(xmss_newkey1)
{
	{
		CKey key1;
		key1.MakeNewKey(CKeyType::XMSS_256_H10);
		BOOST_CHECK(key1.IsValid());

		CPubKey pubkey1 = key1.GetPubKey();
		BOOST_CHECK(pubkey1.IsValid());

		BOOST_CHECK_MESSAGE(key1.VerifyPubKey(pubkey1), 
				"key1: " << HexStr(key1) <<
				"\npubkey1: " << HexStr(pubkey1) << 
				"\nfailed");
	}
}

BOOST_AUTO_TEST_CASE(xmss_newkey2)
{
	{
		CKey key1;
		key1.MakeNewKey(CKeyType::XMSS_256_H16);
		BOOST_CHECK(key1.IsValid());

		CPubKey pubkey1 = key1.GetPubKey();
		BOOST_CHECK(pubkey1.IsValid());

		BOOST_CHECK_MESSAGE(key1.VerifyPubKey(pubkey1), 
				"key1: " << HexStr(key1) << 
				"\npubkey1: " << HexStr(pubkey1) << 
				"\nfailed");
	}
}

BOOST_AUTO_TEST_CASE(xmss_base)
{
    CBitcoinSecret bsecret1, bsecret2, bsecret3, bsecret4;
    BOOST_REQUIRE( bsecret1.SetString (strSecret11));
    BOOST_CHECK( bsecret2.SetString (strSecret12));
    BOOST_CHECK( bsecret3.SetString (strSecret21));
    BOOST_CHECK( bsecret4.SetString (strSecret22));
	
    CKey key1  = bsecret1.GetKey();
    BOOST_CHECK(key1.IsXMSS());
	BOOST_CHECK_EQUAL( HexStr(key1.raw_short_key()), strKey11 );
    CKey key2  = bsecret2.GetKey();
    BOOST_CHECK(key2.IsXMSS());
	BOOST_CHECK_EQUAL( HexStr(key2.raw_short_key()), strKey12 );

    CKey key3  = bsecret3.GetKey();
    BOOST_CHECK(key3.IsXMSS());
	BOOST_CHECK_EQUAL( HexStr(key3.raw_short_key()), strKey21 );
    CKey key4  = bsecret4.GetKey();
    BOOST_CHECK(key4.IsXMSS());
	BOOST_CHECK_EQUAL( HexStr(key4.raw_short_key()), strKey22 );

	CPubKey pubkey1 = key1.GetPubKey();
	BOOST_CHECK(pubkey1.IsXMSS());
	
	CPubKey pubkey2 = key2.GetPubKey();
	BOOST_CHECK(pubkey2.IsXMSS());

	CPubKey pubkey3 = key3.GetPubKey();
	BOOST_CHECK(pubkey3.IsXMSS());
	
	CPubKey pubkey4 = key4.GetPubKey();
	BOOST_CHECK(pubkey4.IsXMSS());

	BOOST_CHECK(key1.VerifyPubKey(pubkey1));
	BOOST_CHECK(key2.VerifyPubKey(pubkey2));
	BOOST_CHECK(key3.VerifyPubKey(pubkey3));
	BOOST_CHECK(key4.VerifyPubKey(pubkey4));
	
	BOOST_CHECK(!key1.VerifyPubKey(pubkey2));
	BOOST_CHECK(!key2.VerifyPubKey(pubkey1));
	BOOST_CHECK(!key1.VerifyPubKey(pubkey3));
	
	BOOST_CHECK(!key3.VerifyPubKey(pubkey4));
	BOOST_CHECK(!key4.VerifyPubKey(pubkey3));
	BOOST_CHECK(!key3.VerifyPubKey(pubkey1));
	
    for (int n=0; n<2; n++)
	{
		std::string strMsg = strprintf("Very secret message %i: 22", n);
		//uint256 hashMsg = Hash(strMsg.begin(), strMsg.end());

		std::vector<unsigned char> sign1;
		std::vector<unsigned char> sign2;
		std::vector<unsigned char> sign3;
		std::vector<unsigned char> sign4;

		BOOST_CHECK( key1.Sign (strMsg, sign1) );
		BOOST_CHECK( key2.Sign (strMsg, sign2) );
		BOOST_CHECK( key3.Sign (strMsg, sign3) );
		BOOST_CHECK( key4.Sign (strMsg, sign4) );
		
		{
			auto sign1c = sign1; sign1c.push_back(SIGHASH_ALL);
			auto sign2c = sign2; sign2c.push_back(SIGHASH_ALL);
			auto sign3c = sign3; sign3c.push_back(SIGHASH_ALL);
			auto sign4c = sign4; sign4c.push_back(SIGHASH_ALL);
			
			BOOST_CHECK( xmss_is_der_signature(sign1c) );
			BOOST_CHECK( xmss_is_der_signature(sign2c) );
			BOOST_CHECK( xmss_is_der_signature(sign3c) );
			BOOST_CHECK( xmss_is_der_signature(sign4c) );
		}

		
		BOOST_CHECK( pubkey1.Verify(strMsg, sign1) );
		BOOST_CHECK( pubkey2.Verify(strMsg, sign2) );
		BOOST_CHECK( pubkey3.Verify(strMsg, sign3) );
		BOOST_CHECK( pubkey4.Verify(strMsg, sign4) );

		BOOST_CHECK( !pubkey1.Verify(strMsg, sign2) );
		BOOST_CHECK( !pubkey2.Verify(strMsg, sign1) );
		BOOST_CHECK( !pubkey1.Verify(strMsg, sign3) );

		BOOST_CHECK( !pubkey3.Verify(strMsg, sign4) );
		BOOST_CHECK( !pubkey4.Verify(strMsg, sign3) );
		BOOST_CHECK( !pubkey3.Verify(strMsg, sign1) );
		
		std::vector<unsigned char> csign1, csign2, csign3, csign4;

		BOOST_CHECK(key1.SignCompact (strMsg, csign1));
		BOOST_CHECK(key2.SignCompact (strMsg, csign2));
		BOOST_CHECK(key3.SignCompact (strMsg, csign3));
		BOOST_CHECK(key4.SignCompact (strMsg, csign4));

		CPubKey rkey1, rkey2;
		CPubKey rkey3, rkey4;

		BOOST_CHECK(rkey1.RecoverCompact (strMsg, csign1));
		BOOST_CHECK(rkey2.RecoverCompact (strMsg, csign2));
		BOOST_CHECK(rkey3.RecoverCompact (strMsg, csign3));
		BOOST_CHECK(rkey4.RecoverCompact (strMsg, csign4));

		BOOST_CHECK(rkey1 == pubkey1);
		BOOST_CHECK(rkey2 == pubkey2);
		BOOST_CHECK(rkey3 == pubkey3);
		BOOST_CHECK(rkey4 == pubkey4);
	}
	
	//std::string strMsg = "Very deterministic message";
	//uint256 hashMsg = Hash(strMsg.begin(), strMsg.end());

    //std::vector<unsigned char> detsig1, detsig2;
	
    //BOOST_CHECK(key1.Sign(hashMsg, detsig1));
    //BOOST_CHECK(key2.Sign(hashMsg, detsig2));
	
	//BOOST_TEST_MESSAGE("detsig1: " << HexStr(detsig1));
	//BOOST_TEST_MESSAGE("detsig2: " << HexStr(detsig2));
}

BOOST_AUTO_TEST_CASE(use_count)
{
	CKey key0;
	key0.MakeNewKey(CKeyType::XMSS_256_H10);
	BOOST_CHECK(key0.IsValid());

	CPubKey pubkey = key0.GetPubKey();
	BOOST_CHECK(pubkey.IsValid());

	auto secret = key0.raw_private_key();

	//BOOST_TEST_MESSAGE("secret: " << HexStr(secret));
	
	for (int i = 0; i < 4; ++i)
	{
		CKey key1(secret);
		BOOST_REQUIRE(key1.IsValid());

		std::string strMsg = strprintf("Very secret message %i: 11", i);
		//uint256 hashMsg = Hash(strMsg.begin(), strMsg.end());

        std::vector<unsigned char> sign1;
		
		BOOST_CHECK( key1.Sign (strMsg, sign1) );
		BOOST_CHECK( pubkey.Verify(strMsg, sign1) );
	}

	for (int i = 0; i < 4; ++i)
	{
		CKey key1(secret);
		BOOST_REQUIRE(key1.IsValid());

		std::string strMsg = strprintf("Very secret message %i: 22", i);
		//uint256 hashMsg = Hash(strMsg.begin(), strMsg.end());

        std::vector<unsigned char> sign1;
		
		BOOST_CHECK( key1.Sign (strMsg, sign1) );
		BOOST_CHECK( pubkey.Verify(strMsg, sign1) );
	}
}

BOOST_AUTO_TEST_CASE(key_test1)
{
    CBitcoinSecret bsecret1, bsecret2, bsecret1C, bsecret2C, baddress1;
    BOOST_CHECK( bsecret1.SetString (strSecret1));
    BOOST_CHECK( bsecret2.SetString (strSecret2));
    BOOST_CHECK( bsecret1C.SetString(strSecret1C));
    BOOST_CHECK( bsecret2C.SetString(strSecret2C));
    BOOST_CHECK(!baddress1.SetString(strAddressBad));

    CKey key1  = bsecret1.GetKey();
    BOOST_CHECK(key1.IsCompressed() == false);
    CKey key2  = bsecret2.GetKey();
    BOOST_CHECK(key2.IsCompressed() == false);
    CKey key1C = bsecret1C.GetKey();
    BOOST_CHECK(key1C.IsCompressed() == true);
    CKey key2C = bsecret2C.GetKey();
    BOOST_CHECK(key2C.IsCompressed() == true);

    CPubKey pubkey1  = key1. GetPubKey();
    CPubKey pubkey2  = key2. GetPubKey();
    CPubKey pubkey1C = key1C.GetPubKey();
    CPubKey pubkey2C = key2C.GetPubKey();

    BOOST_CHECK(key1.VerifyPubKey(pubkey1));
    BOOST_CHECK(!key1.VerifyPubKey(pubkey1C));
    BOOST_CHECK(!key1.VerifyPubKey(pubkey2));
    BOOST_CHECK(!key1.VerifyPubKey(pubkey2C));

    BOOST_CHECK(!key1C.VerifyPubKey(pubkey1));
    BOOST_CHECK(key1C.VerifyPubKey(pubkey1C));
    BOOST_CHECK(!key1C.VerifyPubKey(pubkey2));
    BOOST_CHECK(!key1C.VerifyPubKey(pubkey2C));

    BOOST_CHECK(!key2.VerifyPubKey(pubkey1));
    BOOST_CHECK(!key2.VerifyPubKey(pubkey1C));
    BOOST_CHECK(key2.VerifyPubKey(pubkey2));
    BOOST_CHECK(!key2.VerifyPubKey(pubkey2C));

    BOOST_CHECK(!key2C.VerifyPubKey(pubkey1));
    BOOST_CHECK(!key2C.VerifyPubKey(pubkey1C));
    BOOST_CHECK(!key2C.VerifyPubKey(pubkey2));
    BOOST_CHECK(key2C.VerifyPubKey(pubkey2C));

    BOOST_CHECK(DecodeDestination(addr1)  == CTxDestination(pubkey1.GetID()));
    BOOST_CHECK(DecodeDestination(addr2)  == CTxDestination(pubkey2.GetID()));
    BOOST_CHECK(DecodeDestination(addr1C) == CTxDestination(pubkey1C.GetID()));
    BOOST_CHECK(DecodeDestination(addr2C) == CTxDestination(pubkey2C.GetID()));

    for (int n=0; n<16; n++)
    {
        std::string strMsg = strprintf("Very secret message %i: 11", n);
        //uint256 hashMsg = Hash(strMsg.begin(), strMsg.end());

        // normal signatures

        std::vector<unsigned char> sign1, sign2, sign1C, sign2C;

        BOOST_CHECK(key1.Sign (strMsg, sign1));
        BOOST_CHECK(key2.Sign (strMsg, sign2));
        BOOST_CHECK(key1C.Sign(strMsg, sign1C));
        BOOST_CHECK(key2C.Sign(strMsg, sign2C));

        BOOST_CHECK( pubkey1.Verify(strMsg, sign1));
        BOOST_CHECK(!pubkey1.Verify(strMsg, sign2));
        BOOST_CHECK( pubkey1.Verify(strMsg, sign1C));
        BOOST_CHECK(!pubkey1.Verify(strMsg, sign2C));

        BOOST_CHECK(!pubkey2.Verify(strMsg, sign1));
        BOOST_CHECK( pubkey2.Verify(strMsg, sign2));
        BOOST_CHECK(!pubkey2.Verify(strMsg, sign1C));
        BOOST_CHECK( pubkey2.Verify(strMsg, sign2C));

        BOOST_CHECK( pubkey1C.Verify(strMsg, sign1));
        BOOST_CHECK(!pubkey1C.Verify(strMsg, sign2));
        BOOST_CHECK( pubkey1C.Verify(strMsg, sign1C));
        BOOST_CHECK(!pubkey1C.Verify(strMsg, sign2C));

        BOOST_CHECK(!pubkey2C.Verify(strMsg, sign1));
        BOOST_CHECK( pubkey2C.Verify(strMsg, sign2));
        BOOST_CHECK(!pubkey2C.Verify(strMsg, sign1C));
        BOOST_CHECK( pubkey2C.Verify(strMsg, sign2C));

        // compact signatures (with key recovery)

        std::vector<unsigned char> csign1, csign2, csign1C, csign2C;

        BOOST_CHECK(key1.SignCompact (strMsg, csign1));
        BOOST_CHECK(key2.SignCompact (strMsg, csign2));
        BOOST_CHECK(key1C.SignCompact(strMsg, csign1C));
        BOOST_CHECK(key2C.SignCompact(strMsg, csign2C));

        CPubKey rkey1, rkey2, rkey1C, rkey2C;

        BOOST_CHECK(rkey1.RecoverCompact (strMsg, csign1));
        BOOST_CHECK(rkey2.RecoverCompact (strMsg, csign2));
        BOOST_CHECK(rkey1C.RecoverCompact(strMsg, csign1C));
        BOOST_CHECK(rkey2C.RecoverCompact(strMsg, csign2C));

        BOOST_CHECK(rkey1  == pubkey1);
        BOOST_CHECK(rkey2  == pubkey2);
        BOOST_CHECK(rkey1C == pubkey1C);
        BOOST_CHECK(rkey2C == pubkey2C);
    }

    // test deterministic signing

	BOOST_CHECK(!key1.IsXMSS());
	BOOST_CHECK(!key1C.IsXMSS());
	BOOST_CHECK(!key2.IsXMSS());
	BOOST_CHECK(!key2C.IsXMSS());

    std::vector<unsigned char> detsig, detsigc;
    std::string strMsg = "Very deterministic message";
    //uint256 hashMsg = Hash(strMsg.begin(), strMsg.end());
	
    BOOST_CHECK(key1.Sign(strMsg, detsig));
    BOOST_CHECK(key1C.Sign(strMsg, detsigc));
	
    BOOST_CHECK(detsig == detsigc);
    BOOST_CHECK(detsig == ParseHex("304402205dbbddda71772d95ce91cd2d14b592cfbc1dd0aabd6a394b6c2d377bbe59d31d022014ddda21494a4e221f0824f0b8b924c43fa43c0ad57dccdaa11f81a6bd4582f6"));

    BOOST_CHECK(key2.Sign(strMsg, detsig));
    BOOST_CHECK(key2C.Sign(strMsg, detsigc));
    BOOST_CHECK(detsig == detsigc);
    BOOST_CHECK(detsig == ParseHex("3044022052d8a32079c11e79db95af63bb9600c5b04f21a9ca33dc129c2bfa8ac9dc1cd5022061d8ae5e0f6c1a16bde3719c64c2fd70e404b6428ab9a69566962e8771b5944d"));

    BOOST_CHECK(key1.SignCompact(strMsg, detsig));
    BOOST_CHECK(key1C.SignCompact(strMsg, detsigc));
    BOOST_CHECK(detsig == ParseHex("1c5dbbddda71772d95ce91cd2d14b592cfbc1dd0aabd6a394b6c2d377bbe59d31d14ddda21494a4e221f0824f0b8b924c43fa43c0ad57dccdaa11f81a6bd4582f6"));
    BOOST_CHECK(detsigc == ParseHex("205dbbddda71772d95ce91cd2d14b592cfbc1dd0aabd6a394b6c2d377bbe59d31d14ddda21494a4e221f0824f0b8b924c43fa43c0ad57dccdaa11f81a6bd4582f6"));

    BOOST_CHECK(key2.SignCompact(strMsg, detsig));
    BOOST_CHECK(key2C.SignCompact(strMsg, detsigc));
    BOOST_CHECK(detsig == ParseHex("1c52d8a32079c11e79db95af63bb9600c5b04f21a9ca33dc129c2bfa8ac9dc1cd561d8ae5e0f6c1a16bde3719c64c2fd70e404b6428ab9a69566962e8771b5944d"));
    BOOST_CHECK(detsigc == ParseHex("2052d8a32079c11e79db95af63bb9600c5b04f21a9ca33dc129c2bfa8ac9dc1cd561d8ae5e0f6c1a16bde3719c64c2fd70e404b6428ab9a69566962e8771b5944d"));
}

#if 0
BOOST_AUTO_TEST_CASE(witness_v1_meta)
{
    //CPubKey key1(ParseHex("5128751e76e8199196d454941c45d1b3a323f1433bd6751e76e8199196d454941c45d1b3a323f1433bd6"));
    //CPubKey key2(ParseHex("1007000007590e09943577069c18c3c2b88f98800c094067bbf60b6b0c49bef3dff02c52506f81694b7c411dc20ea661a4b1f77558a5a37572746a5f791c5852fb525be6a2"));
    //CPubKey key3(ParseHex("1008000008974ce0ba1f5393e59ec0a83eb089abfb41fdce1b7f35aa264cae699f58a5f9216f7837006abb4ce7c9fa81f0408607a1d83fbc1787cd7c4e2b9ec08d976cadaf"));
    
    //BOOST_CHECK_EQUAL(key1.GetMeta(), WitnessV1Meta::None);
    //BOOST_CHECK_EQUAL(key2.GetMeta(), WitnessV1Meta::XMSS_SHAKE128_10_PUBKEYHASH);
    //BOOST_CHECK_EQUAL(key3.GetMeta(), WitnessV1Meta::XMSS_SHAKE128_16_PUBKEYHASH);
}
#endif

BOOST_AUTO_TEST_SUITE_END()
