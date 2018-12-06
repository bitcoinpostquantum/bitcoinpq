# Copyright (c) 2013-2016 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

DIST_SUBDIRS = secp256k1 univalue

AM_LDFLAGS = $(PTHREAD_CFLAGS) $(LIBTOOL_LDFLAGS) $(HARDENED_LDFLAGS)
AM_CXXFLAGS = $(HARDENED_CXXFLAGS) $(ERROR_CXXFLAGS)
AM_CPPFLAGS = $(HARDENED_CPPFLAGS)
EXTRA_LIBRARIES =

if EMBEDDED_UNIVALUE
LIBUNIVALUE = univalue/libunivalue.la

$(LIBUNIVALUE): $(wildcard univalue/lib/*) $(wildcard univalue/include/*)
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F)
else
LIBUNIVALUE = $(UNIVALUE_LIBS)
endif

BPQ_INCLUDES=-I$(builddir) $(BDB_CPPFLAGS) $(BOOST_CPPFLAGS) $(LEVELDB_CPPFLAGS) \
    $(CRYPTO_CFLAGS) $(SSL_CFLAGS) $(BOTAN_CFLAGS)

BPQ_INCLUDES += -I$(srcdir)/secp256k1/include
BPQ_INCLUDES += $(UNIVALUE_CFLAGS)

LIBBPQ_SERVER=libbpq_server.a
LIBBPQ_COMMON=libbpq_common.a
LIBBPQ_CONSENSUS=libbpq_consensus.a
LIBBPQ_CLI=libbpq_cli.a
LIBBPQ_UTIL=libbpq_util.a
LIBBPQ_CRYPTO=crypto/libbpq_crypto.a
LIBBPQQT=qt/libbpqqt.a
LIBSECP256K1=secp256k1/libsecp256k1.la

if ENABLE_ZMQ
LIBBPQ_ZMQ=libbpq_zmq.a
endif
if BUILD_BPQ_LIBS
LIBBPQCONSENSUS=libbpqconsensus.la
endif
if ENABLE_WALLET
LIBBPQ_WALLET=libbpq_wallet.a
endif

$(LIBSECP256K1): $(wildcard secp256k1/src/*) $(wildcard secp256k1/include/*)
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F)

# Make is not made aware of per-object dependencies to avoid limiting building parallelization
# But to build the less dependent modules first, we manually select their order here:
EXTRA_LIBRARIES += \
  $(LIBBPQ_CRYPTO) \
  $(LIBBPQ_UTIL) \
  $(LIBBPQ_COMMON) \
  $(LIBBPQ_CONSENSUS) \
  $(LIBBPQ_SERVER) \
  $(LIBBPQ_CLI) \
  $(LIBBPQ_WALLET) \
  $(LIBBPQ_ZMQ)

lib_LTLIBRARIES = $(LIBBPQCONSENSUSL)

bin_PROGRAMS =
noinst_PROGRAMS =
TESTS =
BENCHMARKS =

if BUILD_BPQD
  bin_PROGRAMS += bpqd
endif

if BUILD_BPQ_UTILS
  bin_PROGRAMS += bpq-cli bpq-tx
endif

.PHONY: FORCE check-symbols check-security
# bpq core #
BPQ_CORE_H = \
  addrdb.h \
  addrman.h \
  base58.h \
  bech32.h \
  bloom.h \
  blockencodings.h \
  chain.h \
  chainparams.h \
  chainparamsbase.h \
  chainparamsseeds.h \
  checkpoints.h \
  checkqueue.h \
  clientversion.h \
  coins.h \
  compat.h \
  compat/byteswap.h \
  compat/endian.h \
  compat/sanity.h \
  compressor.h \
  consensus/consensus.h \
  consensus/tx_verify.h \
  core_io.h \
  core_memusage.h \
  crypt_ecdsa.h \
  crypt_xmss.h \
  cuckoocache.h \
  fs.h \
  httprpc.h \
  httpserver.h \
  indirectmap.h \
  init.h \
  key.h \
  keystore.h \
  dbwrapper.h \
  limitedmap.h \
  memusage.h \
  merkleblock.h \
  miner.h \
  net.h \
  net_processing.h \
  netaddress.h \
  netbase.h \
  netmessagemaker.h \
  noui.h \
  policy/feerate.h \
  policy/fees.h \
  policy/policy.h \
  policy/rbf.h \
  pow.h \
  protocol.h \
  random.h \
  reverse_iterator.h \
  reverselock.h \
  rpc/blockchain.h \
  rpc/client.h \
  rpc/mining.h \
  rpc/protocol.h \
  rpc/safemode.h \
  rpc/server.h \
  rpc/register.h \
  rpc/util.h \
  scheduler.h \
  script/sigcache.h \
  script/sign.h \
  script/standard.h \
  script/ismine.h \
  streams.h \
  support/allocators/secure.h \
  support/allocators/zeroafterfree.h \
  support/cleanse.h \
  support/events.h \
  support/lockedpool.h \
  sync.h \
  threadsafety.h \
  threadinterrupt.h \
  timedata.h \
  torcontrol.h \
  txdb.h \
  txmempool.h \
  ui_interface.h \
  undo.h \
  util.h \
  utilmoneystr.h \
  utiltime.h \
  validation.h \
  validationinterface.h \
  versionbits.h \
  wallet/coincontrol.h \
  wallet/crypter.h \
  wallet/db.h \
  wallet/feebumper.h \
  wallet/fees.h \
  wallet/init.h \
  wallet/rpcwallet.h \
  wallet/wallet.h \
  wallet/walletdb.h \
  wallet/walletutil.h \
  warnings.h \
  zmq/zmqabstractnotifier.h \
  zmq/zmqconfig.h\
  zmq/zmqnotificationinterface.h \
  zmq/zmqpublishnotifier.h

include Makefile.bpqcrypto.include

obj/build.h: FORCE
	@$(MKDIR_P) $(builddir)/obj
	@$(top_srcdir)/share/genbuild.sh "$(abs_top_builddir)/src/obj/build.h" \
	  "$(abs_top_srcdir)"
libbpq_util_a-clientversion.$(OBJEXT): obj/build.h

# server: shared between bpqd and bpq-qt
libbpq_server_a_CPPFLAGS = $(AM_CPPFLAGS) $(BPQ_INCLUDES) $(MINIUPNPC_CPPFLAGS) $(EVENT_CFLAGS) $(EVENT_PTHREADS_CFLAGS)
libbpq_server_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libbpq_server_a_SOURCES = \
  addrdb.cpp \
  addrman.cpp \
  bloom.cpp \
  blockencodings.cpp \
  chain.cpp \
  checkpoints.cpp \
  consensus/tx_verify.cpp \
  crypt_ecdsa.cpp \
  crypt_xmss.cpp \
  httprpc.cpp \
  httpserver.cpp \
  init.cpp \
  dbwrapper.cpp \
  merkleblock.cpp \
  miner.cpp \
  net.cpp \
  net_processing.cpp \
  noui.cpp \
  policy/fees.cpp \
  policy/policy.cpp \
  policy/rbf.cpp \
  pow.cpp \
  rest.cpp \
  rpc/blockchain.cpp \
  rpc/mining.cpp \
  rpc/misc.cpp \
  rpc/net.cpp \
  rpc/rawtransaction.cpp \
  rpc/safemode.cpp \
  rpc/server.cpp \
  script/sigcache.cpp \
  script/ismine.cpp \
  timedata.cpp \
  torcontrol.cpp \
  txdb.cpp \
  txmempool.cpp \
  ui_interface.cpp \
  validation.cpp \
  validationinterface.cpp \
  versionbits.cpp \
  $(BPQ_CORE_H)

if ENABLE_ZMQ
libbpq_zmq_a_CPPFLAGS = $(BPQ_INCLUDES) $(ZMQ_CFLAGS)
libbpq_zmq_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libbpq_zmq_a_SOURCES = \
  zmq/zmqabstractnotifier.cpp \
  zmq/zmqnotificationinterface.cpp \
  zmq/zmqpublishnotifier.cpp
endif


# wallet: shared between bpqd and bpq-qt, but only linked
# when wallet enabled
libbpq_wallet_a_CPPFLAGS = $(AM_CPPFLAGS) $(BPQ_INCLUDES)
libbpq_wallet_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libbpq_wallet_a_SOURCES = \
  wallet/crypter.cpp \
  wallet/db.cpp \
  wallet/feebumper.cpp \
  wallet/fees.cpp \
  wallet/init.cpp \
  wallet/rpcdump.cpp \
  wallet/rpcwallet.cpp \
  wallet/wallet.cpp \
  wallet/walletdb.cpp \
  wallet/walletutil.cpp \
  $(BPQ_CORE_H)

# crypto primitives library
crypto_libbpq_crypto_a_CPPFLAGS = $(AM_CPPFLAGS)
crypto_libbpq_crypto_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
crypto_libbpq_crypto_a_SOURCES = \
  crypto/aes.cpp \
  crypto/aes.h \
  crypto/chacha20.h \
  crypto/chacha20.cpp \
  crypto/common.h \
  crypto/equihash.cpp \
  crypto/equihash.h \
  crypto/equihash.tcc \
  crypto/hmac_sha256.cpp \
  crypto/hmac_sha256.h \
  crypto/hmac_sha512.cpp \
  crypto/hmac_sha512.h \
  crypto/ripemd160.cpp \
  crypto/ripemd160.h \
  crypto/sha1.cpp \
  crypto/sha1.h \
  crypto/sha256.cpp \
  crypto/sha256.h \
  crypto/sha512.cpp \
  crypto/sha512.h

if USE_ASM
crypto_libbpq_crypto_a_SOURCES += crypto/sha256_sse4.cpp
endif

# consensus: shared between all executables that validate any consensus rules.
libbpq_consensus_a_CPPFLAGS = $(AM_CPPFLAGS) $(BPQ_INCLUDES)
libbpq_consensus_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libbpq_consensus_a_SOURCES = \
  amount.h \
  arith_uint256.cpp \
  arith_uint256.h \
  chainparams.cpp \
  chainparams.h \
  consensus/merkle.cpp \
  consensus/merkle.h \
  consensus/params.h \
  consensus/validation.h \
  crypt_ecdsa.cpp \
  crypt_ecdsa.h \
  crypt_xmss.cpp \
  crypt_xmss.h \
  hash.cpp \
  hash.h \
  prevector.h \
  primitives/block.cpp \
  primitives/block.h \
  primitives/transaction.cpp \
  primitives/transaction.h \
  pubkey.cpp \
  pubkey.h \
  script/bpqconsensus.cpp \
  script/interpreter.cpp \
  script/interpreter.h \
  script/script.cpp \
  script/script.h \
  script/script_error.cpp \
  script/script_error.h \
  script/standard.cpp \
  script/standard.h \
  serialize.h \
  tinyformat.h \
  uint256.cpp \
  uint256.h \
  utilstrencodings.cpp \
  utilstrencodings.h \
  version.h

# common: shared between bpqd, and bpq-qt and non-server tools
libbpq_common_a_CPPFLAGS = $(AM_CPPFLAGS) $(BPQ_INCLUDES)
libbpq_common_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libbpq_common_a_SOURCES = \
  base58.cpp \
  bech32.cpp \
  chainparams.cpp \
  coins.cpp \
  compressor.cpp \
  core_read.cpp \
  core_write.cpp \
  crypt_ecdsa.cpp \
  crypt_xmss.cpp \
  key.cpp \
  keystore.cpp \
  netaddress.cpp \
  netbase.cpp \
  policy/feerate.cpp \
  protocol.cpp \
  scheduler.cpp \
  script/sign.cpp \
  script/standard.cpp \
  warnings.cpp \
  $(BPQ_CORE_H)

# util: shared between all executables.
# This library *must* be included to make sure that the glibc
# backward-compatibility objects and their sanity checks are linked.
libbpq_util_a_CPPFLAGS = $(AM_CPPFLAGS) $(BPQ_INCLUDES)
libbpq_util_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libbpq_util_a_SOURCES = \
  support/lockedpool.cpp \
  chainparamsbase.cpp \
  clientversion.cpp \
  compat/glibc_sanity.cpp \
  compat/glibcxx_sanity.cpp \
  compat/strnlen.cpp \
  fs.cpp \
  random.cpp \
  rpc/protocol.cpp \
  rpc/util.cpp \
  support/cleanse.cpp \
  sync.cpp \
  threadinterrupt.cpp \
  util.cpp \
  utilmoneystr.cpp \
  utilstrencodings.cpp \
  utiltime.cpp \
  $(BPQ_CORE_H)

if GLIBC_BACK_COMPAT
libbpq_util_a_SOURCES += compat/glibc_compat.cpp
endif

# cli: shared between bpq-cli and bpq-qt
libbpq_cli_a_CPPFLAGS = $(AM_CPPFLAGS) $(BPQ_INCLUDES)
libbpq_cli_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libbpq_cli_a_SOURCES = \
  rpc/client.cpp \
  $(BPQ_CORE_H)

nodist_libbpq_util_a_SOURCES = $(srcdir)/obj/build.h
#

# bpqd binary #
bpqd_SOURCES = bitcoind.cpp
bpqd_CPPFLAGS = $(AM_CPPFLAGS) $(BPQ_INCLUDES)
bpqd_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
bpqd_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if TARGET_WINDOWS
bpqd_SOURCES += bitcoind-res.rc
endif

bpqd_LDADD = \
  $(LIBBPQ_SERVER) \
  $(LIBBPQ_COMMON) \
  $(LIBUNIVALUE) \
  $(LIBBPQ_UTIL) \
  $(LIBBPQ_WALLET) \
  $(LIBBPQ_ZMQ) \
  $(LIBBPQ_CONSENSUS) \
  $(LIBBPQ_BPQCRYPTO) \
  $(LIBBPQ_CRYPTO) \
  $(LIBLEVELDB) \
  $(LIBLEVELDB_SSE42) \
  $(LIBMEMENV) \
  $(LIBSECP256K1)

bpqd_LDADD += $(BOOST_LIBS) $(BDB_LIBS) $(SSL_LIBS) $(CRYPTO_LIBS) $(MINIUPNPC_LIBS) \
    $(EVENT_PTHREADS_LIBS) $(EVENT_LIBS) $(ZMQ_LIBS) $(BOTAN_LIBS) $(LIBEQUIHASH_LIBS)

# bpq-cli binary #
bpq_cli_SOURCES = bitcoin-cli.cpp
bpq_cli_CPPFLAGS = $(AM_CPPFLAGS) $(BPQ_INCLUDES) $(EVENT_CFLAGS)
bpq_cli_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
bpq_cli_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if TARGET_WINDOWS
bpq_cli_SOURCES += bitcoin-cli-res.rc
endif

bpq_cli_LDADD = \
  $(LIBBPQ_CLI) \
  $(LIBUNIVALUE) \
  $(LIBBPQ_UTIL) \
  $(LIBBPQ_CRYPTO)

bpq_cli_LDADD += $(BOOST_LIBS) $(SSL_LIBS) $(CRYPTO_LIBS) $(EVENT_LIBS) $(BOTAN_LIBS) $(LIBEQUIHASH_LIBS)
#

# bpq-tx binary #
bpq_tx_SOURCES = bitcoin-tx.cpp
bpq_tx_CPPFLAGS = $(AM_CPPFLAGS) $(BPQ_INCLUDES)
bpq_tx_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
bpq_tx_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if TARGET_WINDOWS
bpq_tx_SOURCES += bitcoin-tx-res.rc
endif

bpq_tx_LDADD = \
  $(LIBUNIVALUE) \
  $(LIBBPQ_COMMON) \
  $(LIBBPQ_UTIL) \
  $(LIBBPQ_CONSENSUS) \
  $(LIBBPQ_BPQCRYPTO) \
  $(LIBBPQ_CRYPTO) \
  $(LIBSECP256K1)

bpq_tx_LDADD += $(BOOST_LIBS) $(CRYPTO_LIBS) $(BOTAN_LIBS)
#

# bpqconsensus library #
if BUILD_BPQ_LIBS
include_HEADERS = script/bpqconsensus.h
libbpqconsensus_la_SOURCES = $(crypto_libbpq_crypto_a_SOURCES) $(libbpq_consensus_a_SOURCES)
libbpqconsensus_la_SOURCES += \
  script/standard.cpp \
  base58.cpp \
  bech32.cpp \
  key.cpp \
  keystore.cpp \
  chainparams.cpp \
  support/lockedpool.cpp \
  chainparamsbase.cpp \
  util.cpp \
  utiltime.cpp \
  random.cpp \
  fs.cpp \
  support/cleanse.cpp


if GLIBC_BACK_COMPAT
  libbpqconsensus_la_SOURCES += compat/glibc_compat.cpp
endif

libbpqconsensus_la_LDFLAGS = $(AM_LDFLAGS) -no-undefined $(RELDFLAGS)
libbpqconsensus_la_LIBADD = $(LIBSECP256K1) $(LIBUNIVALUE) $(BOOST_LIBS) $(BOTAN_LIBS) $(CRYPTO_LIBS) $(LIBBPQ_BPQCRYPTO) $(LIBEQUIHASH_LIBS)
libbpqconsensus_la_CPPFLAGS = $(AM_CPPFLAGS) -I$(builddir)/obj -I$(srcdir)/secp256k1/include -DBUILD_BPQ_INTERNAL $(BPQ_INCLUDES)
libbpqconsensus_la_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)

endif
#

CTAES_DIST =  crypto/ctaes/bench.c
CTAES_DIST += crypto/ctaes/ctaes.c
CTAES_DIST += crypto/ctaes/ctaes.h
CTAES_DIST += crypto/ctaes/README.md
CTAES_DIST += crypto/ctaes/test.c

CLEANFILES = $(EXTRA_LIBRARIES)

CLEANFILES += *.gcda *.gcno
CLEANFILES += compat/*.gcda compat/*.gcno
CLEANFILES += consensus/*.gcda consensus/*.gcno
CLEANFILES += crypto/*.gcda crypto/*.gcno
CLEANFILES += policy/*.gcda policy/*.gcno
CLEANFILES += primitives/*.gcda primitives/*.gcno
CLEANFILES += script/*.gcda script/*.gcno
CLEANFILES += support/*.gcda support/*.gcno
CLEANFILES += univalue/*.gcda univalue/*.gcno
CLEANFILES += wallet/*.gcda wallet/*.gcno
CLEANFILES += wallet/test/*.gcda wallet/test/*.gcno
CLEANFILES += zmq/*.gcda zmq/*.gcno
CLEANFILES += obj/build.h

EXTRA_DIST = $(CTAES_DIST)


config/bpq-config.h: config/stamp-h1
	@$(MAKE) -C $(top_builddir) $(subdir)/$(@)
config/stamp-h1: $(top_srcdir)/$(subdir)/config/bpq-config.h.in $(top_builddir)/config.status
	$(AM_V_at)$(MAKE) -C $(top_builddir) $(subdir)/$(@)
$(top_srcdir)/$(subdir)/config/bpq-config.h.in:  $(am__configure_deps)
	$(AM_V_at)$(MAKE) -C $(top_srcdir) $(subdir)/config/bpq-config.h.in

clean-local:
	-$(MAKE) -C secp256k1 clean
	-$(MAKE) -C univalue clean
	-rm -f leveldb/*/*.gcda leveldb/*/*.gcno leveldb/helpers/memenv/*.gcda leveldb/helpers/memenv/*.gcno
	-rm -f config.h
	-rm -rf test/__pycache__

.rc.o:
	@test -f $(WINDRES)
	## FIXME: How to get the appropriate modulename_CPPFLAGS in here?
	$(AM_V_GEN) $(WINDRES) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(CPPFLAGS) -DWINDRES_PREPROC -i $< -o $@

check-symbols: $(bin_PROGRAMS)
if GLIBC_BACK_COMPAT
	@echo "Checking glibc back compat..."
	$(AM_V_at) READELF=$(READELF) CPPFILT=$(CPPFILT) $(top_srcdir)/contrib/devtools/symbol-check.py < $(bin_PROGRAMS)
endif

check-security: $(bin_PROGRAMS)
if HARDEN
	@echo "Checking binary security..."
	$(AM_V_at) READELF=$(READELF) OBJDUMP=$(OBJDUMP) $(top_srcdir)/contrib/devtools/security-check.py < $(bin_PROGRAMS)
endif

%.pb.cc %.pb.h: %.proto
	@test -f $(PROTOC)
	$(AM_V_GEN) $(PROTOC) --cpp_out=$(@D) --proto_path=$(<D) $<

if EMBEDDED_LEVELDB
include Makefile.leveldb.include
endif

if ENABLE_TESTS
include Makefile.test.include
endif

if ENABLE_BENCH
include Makefile.bench.include
endif

if ENABLE_QT
include Makefile.qt.include
endif

if ENABLE_QT_TESTS
include Makefile.qttest.include
endif
