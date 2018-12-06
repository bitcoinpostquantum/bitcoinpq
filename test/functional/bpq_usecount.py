#!/usr/bin/env python3

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import *
from decimal import Decimal
import traceback

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

    def send_change_test(self):

        result = self.nodes[0].getaddressesbyaccount("Q01")
        #self.log.info("[0] Q01: %s", result)

        addr_Q33 = self.nodes[3].getaccountaddress("Q33")
        tx = self.nodes[0].sendfrom("Q01", addr_Q33, 1.0)
        self.log.info("[0] Q01->Q33 tx: %s", tx)

        result = self.nodes[0].getaddressesbyaccount("Q01")
        #self.log.info("[0] Q01: %s", result)

        # check change out to same address

        raw_tx = self.nodes[0].getrawtransaction(tx)
        dec_tx = self.nodes[0].decoderawtransaction(raw_tx)

        change_addr = None
        for out in dec_tx["vout"]:
            addr = out["scriptPubKey"]["addresses"][0]
            if addr != addr_Q33:
                change_addr = addr
                break

        #self.log.info("[0] tx input  addr: %s", self.addr_Q01)
        #self.log.info("[0] tx change addr: %s", change_addr)

        assert(self.addr_Q01 == change_addr)

        self.sync_all()
        self.nodes[2].generate(6)
        self.sync_all()

    def sync_use_count_test(self):

        sk_Q01 = self.nodes[0].dumpprivkey(self.addr_Q01)

        use_count_q01 = self.nodes[0].validateaddress(self.addr_Q01)['embedded']['usecount']
        self.log.info("[0] Q01 base usecount: %d", use_count_q01)

        #self.nodes[1].importprivkey(sk_Q01, "Q01")

        addr_Q32 = self.nodes[3].getaccountaddress("Q32")
        tx0 = self.nodes[0].sendfrom("Q01", addr_Q32, 1.0)
        self.log.info("[0] Q01->Q32 tx: %s", tx0)

        # check mempool
        self.sync_all()

        use_count_0 = self.nodes[0].validateaddress(self.addr_Q01)['embedded']['usecount']
        self.log.info("[0] Q01 usecount0: %d", use_count_0)
        #assert_equal(use_count_0, use_count_q01+1)

        result = self.nodes[3].getbalance("Q32", 0)
        self.log.info("[3] balance: %s", result)
        assert_equal(result, 1.0)

        #coins_1 = self.nodes[1].getbalance("Q01", 0)
        #assert_equal(coins_1, 0.0)

        self.nodes[1].importprivkey(sk_Q01, "Q01")

        coins_0 = self.nodes[0].getbalance("Q01", 0)
        self.log.info("[0] balance: %s", coins_0)

        coins_1 = self.nodes[1].getbalance("Q01", 0)
        self.log.info("[1] balance: %s", coins_1)

        #result = self.nodes[0].listreceivedbyaddress(0, True, True)
        #self.log.info("[0] recvs: %s", result)
        #result = self.nodes[1].listreceivedbyaddress(0, True, True)
        #self.log.info("[1] recvs: %s", result)

        use_count_1 = self.nodes[1].validateaddress(self.addr_Q01)['embedded']['usecount']
        self.log.info("[1] Q01 usecount1: %d ( mempool )", use_count_1)
        assert_equal(use_count_0, use_count_1)

        # check confirmed blocks

        self.nodes[2].generate(6)
        self.sync_all()

        result = self.nodes[3].getbalance("Q32", 0)
        assert_equal(result, 1.0)

        use_count_0 = self.nodes[0].validateaddress(self.addr_Q01)['embedded']['usecount']
        use_count_1 = self.nodes[1].validateaddress(self.addr_Q01)['embedded']['usecount']
        self.log.info("[1] Q01 usecount1: %d ( blockchain )", use_count_1)
        assert_equal(use_count_0, use_count_1)

        # another tx

        tx1 = self.nodes[0].sendfrom("Q01", addr_Q32, 1.0)
        self.log.info("[0] Q01->Q32 tx: %s", tx1)

        self.sync_all()
        
        use_count_0 = self.nodes[0].validateaddress(self.addr_Q01)['embedded']['usecount']
        use_count_1 = self.nodes[1].validateaddress(self.addr_Q01)['embedded']['usecount']
        self.log.info("[0] Q01 usecount0: %d", use_count_0)
        self.log.info("[1] Q01 usecount1: %d", use_count_1)

        #assert_equal(use_count_0, 3)
        assert_equal(use_count_0, use_count_1)

        self.nodes[2].generate(6)
        self.sync_all()

        use_count_0 = self.nodes[0].validateaddress(self.addr_Q01)['embedded']['usecount']
        use_count_1 = self.nodes[1].validateaddress(self.addr_Q01)['embedded']['usecount']
        self.log.info("[0] Q01 usecount0: %d", use_count_0)
        self.log.info("[1] Q01 usecount1: %d", use_count_1)

        #assert_equal(use_count_0, 3)
        assert_equal(use_count_0, use_count_1)

    def test_max_use_count(self):

        addr_Q02 = self.nodes[0].getaccountaddress("Q02")
        self.nodes[0].sendfrom("Q01", addr_Q02, 10.0)
        self.sync_all()
        self.nodes[2].generate(6)
        self.sync_all()

        coins_0 = self.nodes[0].getbalance("Q02", 0)
        self.log.info("[0] Q02 balance: %s", coins_0)

        addr_Q14 = self.nodes[0].getaccountaddress("Q14")

        max_i = 0
        for i in range(1025):
            try:
                self.nodes[0].sendfrom("Q02", addr_Q14, 0.001, 0)

                if (i % 40) == 39:
                    self.sync_all()
                    self.nodes[2].generate(1)
                    self.sync_all()
                    
            except:
                max_i = i
                self.log.info("[0] exception i: %d", i)
                break

        assert(max_i < 1024)

        use_count_0 = self.nodes[0].validateaddress(addr_Q02)['embedded']['usecount']
        self.log.info("[0] Q02 usecount0: %d", use_count_0)
        assert(use_count_0 >= 512 and use_count_0 <= 514)

    def run_test(self):

        self.nodes[2].generate(110)
        self.sync_all()

        self.addr_Q01 = self.nodes[0].getaccountaddress("Q01")

        self.nodes[2].sendtoaddress(self.addr_Q01, 100.0)

        self.nodes[2].generate(6)
        self.sync_all()

        result = self.nodes[0].getbalance()
        assert_equal(result, 100.0)

        # check: send change to from address
        self.send_change_test()

        self.sync_use_count_test()

        self.test_max_use_count()
        pass
        
if __name__ == '__main__':
    BpqTest().main()
