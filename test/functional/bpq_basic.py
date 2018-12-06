#!/usr/bin/env python3

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import *
from decimal import Decimal

REGTEST_BIT_PREMINE = 2000
REGTEST_BPQ_PREMINE = 100

PREMINE_PUBKEYS = [
    [
        "100700000796a8f86cec8db8be05671641473f0464f0e142d177cfbbddf35464a14eefb4577310ac8bc2a4a3b2d01de0f57f4f4441e8d17c9081e9e6288dbd45942e8a231e",
        "10070000072f2908f2cd6a09103e2f905c6d71c258b575aeceef8a49774a265e7aa18992dcbed186011828972ec597149f3e9a5f0c7b179181d428045a257096a8a4e98280",
        "100700000760c88b3b0b67a4cb81084a64e1e13faf03ad167e8dc632445b4e47ef02a7cfd511f03f8869fe9b5dc43e2305071332b0cb9929563a7f9f3fea89cee93ce26a0c",
    ],
    [
        "100700000713bcd90cc2f8cf3ac9f4ab2e78c7e90129ec8d8f74968fc8ea9358e9e70a7e1049730ac52b1f7582513a7eeb09bc081a2b8bd596b2fc0f9f813f649a73bec5a5",
        "1007000007df63af45db773341e4316d8226bddc1d4a9bcae08bc287609ab7b0d6b4be687fd45ff1f03c3708a70379f14b39179ce513a1aa3dc40c2f32bfd572f543307ae3",
        "10070000078d4687e27cc2ab3347b1fcbb2b73860ea6e2a5e5cff8bf08f72302039f6ee2103aa754f83d2f854006e5d1c78e12f431ab1acf51138b7781b9501814643c09b3",
    ]
    ]

PREMINE_KEYS = [
    [
        "5UknnBacqeaFB1V5K51CvSz2x2TRA2C5g47GRJ4L7XyeZs613o14i8ijtoGbdrgPwpG8SdNz949gAX7u33bNGfSVpa68uj6XmsVsPMtFGafqhXSbmK1pHc5G8ACUVfmGCntSp1wHAtYu1ZvPnQtj7Jfe9Xci7EkMuFzH8DWMQTPBXoA1cBB5fLxY89jBqRztCzGi7BAY",
        "5UknnBacpmQf38DbrgmSN1oDj2Rre1KgYEQML2j6DMYyH4xq1vcv222eQjSkczaU1BezzLZNaB19NE9jUhnxTZ6G8bfYWQQr5aek2QsqNPD85rBKBXF2DdDwQcvCvwdP1rQU7ty3wGvMWSUrMK75s38iqATMmoabmo2ySHKS6467NhYycd1e82c771BQPQfc4Y6VpTMz",
        "5UknnBacqBwUFP9EKJZDBfmasU7Si2T9htowHSoLUBHpauZxoDpUGCYzFuXBSVp8cgE234WEwg61D1uiEZfGDGgstEpTCH4EL6iDTVX6giakFiVB5gVuY9Ea8vqaQgk9h9enVh7j8CfBZRZJwmgyRRL5jjebDNgnEbeWrUkgxuTF3JDjMXDKbH3Yk5kwgAwqfjd4tiSr",
    ],
    [
        "5UknnBacpXrPEFSnqn4TnpwvaPn1UFsLzABAyvLJoeehoE32SLteEJJFmnGzv9AvQDgLHZfUb1xoA7uSFHAXd2w446Z6N5f4nnaZiPt3wKnFKhArY3BYCi4MnjMRBDyAvsjahdmAWSTok6X82LEtVBRerCNnT7ocB5erSjSAGZHS56oSoFUE2NuLDeE5BKqm2WLXS5Sb",
        "5UknnBacrGXa8EBVjSaF5ni7QL2HhxtxoLCFngSzb9Djubmtrqy3JbucTL52AAn1gWbNNo5vX1JA7dnpExg2Nu3BBaN2h1Zuj3oxkTCVih4xjcUfFHTbnKMvEGwkmAKdz1BEackTKH5yMyCv5NgUNqgFcidYpGHsaMmGJ9KfhZPfbTWGcusGRixpvpfFqLCH1rxZCyoY",
        "5UknnBacqZwAhHe4WPCnQemh5GewFMUPxmrhiwC7jNUTtYTPcsFA1wjF96EiXLsWnELWAvafyVhnA9Y9YYZQUivftV2UYLFrJy1hkBcPh6UCL5j3VGwYgs9po2pHEi8qF1XELJRVpULijRipPUzKGGUmcezQsMpVJS8CdDxXkcSo1TSnqLo98JriSPDifqmmVC2kY51z",
    ]
    ]

class BpqTest(BitcoinTestFramework):

    def set_test_params(self):
        self.num_nodes = 4
        self.setup_clean_chain = True

    def setup_network(self):
        self.setup_nodes()

        connect_nodes_bi(self.nodes, 0, 1)
        connect_nodes_bi(self.nodes, 1, 2)
        connect_nodes_bi(self.nodes, 0, 2)
        connect_nodes_bi(self.nodes, 0, 3)

    def create_bitcoin_addr(self):

        result = self.nodes[3].getbalance()
        self.log.info("nodes[3] balance: %s", result)

        self.addr_L21 = self.nodes[2].getnewaddress("L21", "legacy")
        self.log.info("nodes[2] addr_L21: %s", self.addr_L21)

        self.nodes[3].sendtoaddress(self.addr_L21, 200.0, "", "", True)
        pass

    def make_premine(self):

        self.log.info("Bitcoin premine...")

        bitcoin_coinbase_addr = self.nodes[3].getnewaddress("", "legacy")

        bitcoin_blocks_count = REGTEST_BIT_PREMINE

        for i in range(200 // 50):
            peer = i % self.num_nodes
            self.nodes[peer].generatetoaddress(50, bitcoin_coinbase_addr)
            self.sync_all()

        bitcoin_blocks_count -= 200

        self.create_bitcoin_addr()

        for i in range(bitcoin_blocks_count // 50):
            peer = i % self.num_nodes
            self.nodes[peer].generatetoaddress(50, bitcoin_coinbase_addr)
            self.sync_all()

        self.log.info("BPQ premine...")

        for i in range(REGTEST_BPQ_PREMINE // 10):
            peer = i % self.num_nodes
            self.nodes[peer].generate(10)
            self.sync_all()


    def create_spend_tx(self, node, block_n, redeemScript, out_address):

        blockId = self.nodes[0].getblockhash(REGTEST_BIT_PREMINE+1)
        block = self.nodes[0].getblock(blockId)
        txId = block['tx'][0]

        #self.log.info("spending tx: %s", txId)

        rawTx = node.gettransaction(txId)
        #self.log.info("rawTx: %s", rawTx)

        decTx = node.decoderawtransaction(rawTx['hex'])        
        #self.log.info("decTx: %s", decTx)

        vout = decTx['vout'][0]
        
        #self.log.info("value: %s", vout['value'])

        from_address = node.validateaddress(vout['scriptPubKey']['addresses'][0])
        #self.log.info("from_address: %s", from_address)

        inputs = [{ 
            "txid" : txId, 
            "vout" : vout['n'], 
            "scriptPubKey" : vout['scriptPubKey']['hex'], 
            "redeemScript" : redeemScript,
            "amount" : vout['value']
        }]

        amount = (vout['value'] * Decimal('0.9')).quantize(Decimal('0.00000001'))
        outputs = { out_address : amount }
        rawTx = node.createrawtransaction(inputs, outputs)

        return rawTx, inputs, amount


    def spend_premine_test(self):

        # premine_addr_0['address']
        # premine_addr_0['redeemScript']
        premine_addr_0 = self.nodes[3].createmultisig(2, PREMINE_PUBKEYS[0] )
        premine_addr_1 = self.nodes[3].createmultisig(2, PREMINE_PUBKEYS[1] )

        self.log.info("premine_addr_0: %s", premine_addr_0['address'])
        self.log.info("premine_addr_1: %s", premine_addr_1['address'])

        self.nodes[0].importaddress(premine_addr_0['address'], "premine0")
        self.nodes[0].importaddress(premine_addr_1['address'], "premine1")

        addr_0 = self.nodes[0].validateaddress(self.nodes[0].getnewaddress("addr_0"))
        addr_1 = self.nodes[1].validateaddress(self.nodes[1].getnewaddress("addr_1"))
        addr_2 = self.nodes[2].validateaddress(self.nodes[2].getnewaddress("addr_2"))

        # create transaction
        rawTx, inputs, amount_0 = self.create_spend_tx(self.nodes[0], REGTEST_BIT_PREMINE+1, 
            premine_addr_0['redeemScript'], addr_0['address'])

        #self.log.info("Tx inputs: %s", inputs)
        #self.log.info("decTx: %s", self.nodes[0].decoderawtransaction(rawTx))

        # sign 1
        rawTxPartialSigned = self.nodes[0].signrawtransaction(rawTx, inputs, [
            { "key" : PREMINE_KEYS[0][0], "index": 1 }])
        #self.log.info("rawTxPartialSigned: %s", rawTxPartialSigned)
        assert_equal(rawTxPartialSigned['complete'], False)
        rawTx = rawTxPartialSigned['hex']
        #self.log.info("decTxPartialSigned: %s", self.nodes[0].decoderawtransaction(rawTxPartialSigned['hex']))

        assert_equal(rawTxPartialSigned['complete'], False)

        # sign 2
        rawTxPartialSigned = self.nodes[1].signrawtransaction(rawTx, inputs, [
            { "key" : PREMINE_KEYS[0][1], "index": 1 }])
        #self.log.info("rawTxPartialSigned: %s", rawTxPartialSigned)
        assert_equal(rawTxPartialSigned['complete'], True)
        rawTx = rawTxPartialSigned['hex']

        # send transaction
        spendTx0 = self.nodes[3].sendrawtransaction(rawTx)

        self.nodes[3].generate(6)
        self.sync_all()

        result = self.nodes[0].getbalance()
        #self.log.info("node0 balance: %s", result)
        assert_equal(result, amount_0)

        pass

    def first_transaction_test(self):

        self.addr_Q01 = self.nodes[0].getnewaddress("Q01")
        self.log.info("addr_Q01: %s", self.addr_Q01)

        result = self.nodes[2].getbalance()
        self.log.info("node[2] balance: %s", result)

        result = self.nodes[2].listaccounts()
        self.log.info("node[2] accounts: %s", result)

        self.nodes[2].sendfrom("L21", self.addr_Q01, 50.0)

        self.sync_all()
        self.nodes[3].generate(6)
        self.sync_all()

        pass

    def reject_bitcoin_test(self):
        
        addr_L31 = self.nodes[3].getnewaddress("L31", "legacy")
        #self.nodes[3].sendtoaddress(addr_L31, 1.0)
        assert_raises_rpc_error(-4, "Transaction must contain only BPQ outputs", 
            self.nodes[3].sendtoaddress, addr_L31, 1.0)

        addr_Q31 = self.nodes[3].getnewaddress("Q31")
        self.nodes[3].sendtoaddress(addr_Q31, 1.0)

    def run_test(self):

        self.make_premine()

        self.nodes[1].generate(1)
        self.nodes[2].generate(1)
        self.nodes[3].generate(1)

        self.reject_bitcoin_test()

        self.sync_all()

        self.spend_premine_test()

        self.first_transaction_test()

        pass
        
if __name__ == '__main__':
    BpqTest().main()
