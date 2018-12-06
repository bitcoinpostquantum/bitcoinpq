cmake_minimum_required(VERSION 3.5)
include(ExternalProject)

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR 
    "In-source builds not allowed. Please make a new directory. You may need to remove CMakeCache.txt.
    mkdir .build
    cd .build
    cmake ..
     " )
endif()


option(BUILD_QT_CLIENT "Build BPQ client application" OFF)
option(BUILD_TESTS "Build test application" OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 11)
set(bitcoin bpq)

project(BPQ161 VERSION 1.6.1 LANGUAGES CXX C)

# Bitcoin project sources
if (APPLE)
  set(OPENSSL_ROOT_DIR /usr/local/Cellar/openssl/*)  
  find_library(SECURITY_FRAMEWORK NAMES Security REQUIRED)
  find_library(COREFOUNDATION_FRAMEWORK NAMES CoreFoundation REQUIRED)
  find_library(CORESERVICES_FRAMEWORK NAMES CoreServices REQUIRED)
  find_library(APPKIT_FRAMEWORK NAMES AppKit REQUIRED)

  set(APPLE_FRAMEWORKS ${SECURITY_FRAMEWORK} ${COREFOUNDATION_FRAMEWORK} ${CORESERVICES_FRAMEWORK} ${APPKIT_FRAMEWORK})
endif()
find_package(OpenSSL   REQUIRED)
find_package(LibEvent  REQUIRED)
find_package(Boost     REQUIRED filesystem system program_options thread unit_test_framework)
find_package(Miniupnpc REQUIRED)
find_package(BerkeleyDB REQUIRED)
find_package(Sodium     REQUIRED)

add_subdirectory(${CMAKE_SOURCE_DIR}/src/bpqcrypto) # with botan

set(SRC ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src
                    ${BOTAN_INCLUDE_DIR})

set(lib${bitcoin}_server_a_SOURCES
                                  ${SRC}/src/addrdb.cpp 
                                  ${SRC}/src/addrman.cpp 
                                  ${SRC}/src/bloom.cpp 
                                  ${SRC}/src/blockencodings.cpp 
                                  ${SRC}/src/chain.cpp 
                                  ${SRC}/src/checkpoints.cpp 
                                  ${SRC}/src/consensus/tx_verify.cpp 
                                  ${SRC}/src/httprpc.cpp 
                                  ${SRC}/src/httpserver.cpp 
                                  ${SRC}/src/init.cpp 
                                  ${SRC}/src/dbwrapper.cpp 
                                  ${SRC}/src/merkleblock.cpp 
                                  ${SRC}/src/miner.cpp 
                                  ${SRC}/src/net.cpp 
                                  ${SRC}/src/net_processing.cpp 
                                  ${SRC}/src/noui.cpp 
                                  ${SRC}/src/policy/fees.cpp 
                                  ${SRC}/src/policy/policy.cpp 
                                  ${SRC}/src/policy/rbf.cpp 
                                  ${SRC}/src/pow.cpp 
                                  ${SRC}/src/rest.cpp 
                                  ${SRC}/src/rpc/blockchain.cpp 
                                  ${SRC}/src/rpc/mining.cpp 
                                  ${SRC}/src/rpc/misc.cpp 
                                  ${SRC}/src/rpc/net.cpp 
                                  ${SRC}/src/rpc/rawtransaction.cpp 
                                  ${SRC}/src/rpc/safemode.cpp 
                                  ${SRC}/src/rpc/server.cpp
                                  ${SRC}/src/script/sigcache.cpp 
                                  ${SRC}/src/script/ismine.cpp 
                                  ${SRC}/src/timedata.cpp 
                                  ${SRC}/src/torcontrol.cpp 
                                  ${SRC}/src/txdb.cpp 
                                  ${SRC}/src/txmempool.cpp 
                                  ${SRC}/src/ui_interface.cpp 
                                  ${SRC}/src/validation.cpp 
                                  ${SRC}/src/validationinterface.cpp 
                                  ${SRC}/src/versionbits.cpp)
add_library(${bitcoin}_server STATIC ${lib${bitcoin}_server_a_SOURCES})
target_compile_options(${bitcoin}_server PRIVATE -DHAVE_CONFIG_H)
source_group(Server FILES ${lib${bitcoin}_server_a_SOURCES})

set(lib${bitcoin}_crypto_a_SOURCES
                                  ${SRC}/src/crypto/aes.cpp 
                                  ${SRC}/src/crypto/aes.h 
                                  ${SRC}/src/crypto/chacha20.h 
                                  ${SRC}/src/crypto/chacha20.cpp 
                                  ${SRC}/src/crypto/common.h 
                                  ${SRC}/src/crypto/hmac_sha256.cpp 
                                  ${SRC}/src/crypto/hmac_sha256.h 
                                  ${SRC}/src/crypto/hmac_sha512.cpp 
                                  ${SRC}/src/crypto/hmac_sha512.h 
                                  ${SRC}/src/crypto/ripemd160.cpp 
                                  ${SRC}/src/crypto/ripemd160.h 
                                  ${SRC}/src/crypto/sha1.cpp 
                                  ${SRC}/src/crypto/sha1.h 
                                  ${SRC}/src/crypto/sha256.cpp 
                                  ${SRC}/src/crypto/sha256.h 
                                  ${SRC}/src/crypto/sha512.cpp 
                                  ${SRC}/src/crypto/sha512.h
                                  ${SRC}/src/crypto/sha256_sse4.cpp
                                  ${SRC}/src/crypt_xmss.cpp
                                  ${SRC}/src/crypt_ecdsa.cpp
                                  ${SRC}/src/crypto/equihash.cpp 
                                  ${SRC}/src/crypto/equihash.h
                                  ${SRC}/src/crypto/equihash.tcc)

add_library(${bitcoin}_crypto STATIC ${lib${bitcoin}_crypto_a_SOURCES})
target_compile_options(${bitcoin}_crypto PRIVATE -DHAVE_CONFIG_H)
source_group(Crypto FILES ${lib${bitcoin}_crypto_a_SOURCES})

set(lib${bitcoin}_consensus_a_SOURCES
                                  ${SRC}/src/amount.h
                                  ${SRC}/src/arith_uint256.cpp
                                  ${SRC}/src/arith_uint256.h
                                  ${SRC}/src/chainparams.cpp
                                  ${SRC}/src/chainparams.h
                                  ${SRC}/src/consensus/merkle.cpp
                                  ${SRC}/src/consensus/merkle.h
                                  ${SRC}/src/consensus/params.h 
                                  ${SRC}/src/consensus/validation.h 
                                  ${SRC}/src/hash.cpp 
                                  ${SRC}/src/hash.h 
                                  ${SRC}/src/prevector.h 
                                  ${SRC}/src/primitives/block.cpp 
                                  ${SRC}/src/primitives/block.h 
                                  ${SRC}/src/primitives/transaction.cpp 
                                  ${SRC}/src/primitives/transaction.h 
                                  ${SRC}/src/pubkey.cpp 
                                  ${SRC}/src/pubkey.h 
                                  ${SRC}/src/script/bpqconsensus.cpp 
                                  ${SRC}/src/script/interpreter.cpp 
                                  ${SRC}/src/script/interpreter.h 
                                  ${SRC}/src/script/script.cpp 
                                  ${SRC}/src/script/script.h 
                                  ${SRC}/src/script/script_error.cpp 
                                  ${SRC}/src/script/script_error.h 
                                  ${SRC}/src/script/standard.cpp 
                                  ${SRC}/src/script/standard.h
                                  ${SRC}/src/serialize.h 
                                  ${SRC}/src/tinyformat.h 
                                  ${SRC}/src/uint256.cpp 
                                  ${SRC}/src/uint256.h 
                                  ${SRC}/src/utilstrencodings.cpp 
                                  ${SRC}/src/utilstrencodings.h 
                                  ${SRC}/src/version.h)
add_library(${bitcoin}_consensus STATIC ${lib${bitcoin}_consensus_a_SOURCES})
target_compile_options(${bitcoin}_consensus PRIVATE -DHAVE_CONFIG_H)
source_group(Consensus FILES ${lib${bitcoin}_consensus_a_SOURCES})

set(lib${bitcoin}_common_a_SOURCES
                        ${SRC}/src/base58.cpp 
                        ${SRC}/src/bech32.cpp 
                        ${SRC}/src/chainparams.cpp 
                        ${SRC}/src/coins.cpp 
                        ${SRC}/src/compressor.cpp 
                        ${SRC}/src/core_read.cpp 
                        ${SRC}/src/core_write.cpp 
                        ${SRC}/src/key.h
                        ${SRC}/src/key.cpp
                        ${SRC}/src/keystore.cpp 
                        ${SRC}/src/netaddress.cpp 
                        ${SRC}/src/netbase.cpp 
                        ${SRC}/src/policy/feerate.cpp 
                        ${SRC}/src/protocol.cpp 
                        ${SRC}/src/scheduler.cpp 
                        ${SRC}/src/script/sign.cpp 
                        ${SRC}/src/script/standard.cpp 
                        ${SRC}/src/warnings.cpp 
                        ${SRC}/src/${BITCOIN_CORE_H})
add_library(${bitcoin}_common STATIC ${lib${bitcoin}_common_a_SOURCES})
target_compile_options(${bitcoin}_common PRIVATE -DHAVE_CONFIG_H)
source_group(Common FILES ${lib${bitcoin}_common_a_SOURCES})

set(lib${bitcoin}_util_a_SOURCES
                        ${SRC}/src/support/lockedpool.cpp 
                        ${SRC}/src/chainparamsbase.cpp 
                        ${SRC}/src/clientversion.cpp 
                        ${SRC}/src/compat/glibc_sanity.cpp 
                        ${SRC}/src/compat/glibcxx_sanity.cpp 
                        ${SRC}/src/compat/strnlen.cpp 
                        ${SRC}/src/fs.cpp 
                        ${SRC}/src/random.cpp 
                        ${SRC}/src/rpc/protocol.cpp 
                        ${SRC}/src/rpc/util.cpp 
                        ${SRC}/src/support/cleanse.cpp 
                        ${SRC}/src/sync.cpp 
                        ${SRC}/src/threadinterrupt.cpp 
                        ${SRC}/src/util.cpp 
                        ${SRC}/src/utilmoneystr.cpp 
                        ${SRC}/src/utilstrencodings.cpp 
                        ${SRC}/src/utiltime.cpp 
                        ${BITCOIN_CORE_H})
add_library(${bitcoin}_util STATIC ${lib${bitcoin}_util_a_SOURCES})
target_compile_options(${bitcoin}_util PRIVATE -DHAVE_CONFIG_H)
source_group(Util FILES ${lib${bitcoin}_util_a_SOURCES})

set(lib${bitcoin}_wallet_a_SOURCES
                        ${SRC}/src/wallet/crypter.cpp 
                        ${SRC}/src/wallet/db.cpp 
                        ${SRC}/src/wallet/feebumper.cpp 
                        ${SRC}/src/wallet/fees.cpp 
                        ${SRC}/src/wallet/init.cpp 
                        ${SRC}/src/wallet/rpcdump.cpp 
                        ${SRC}/src/wallet/rpcwallet.cpp 
                        ${SRC}/src/wallet/wallet.cpp 
                        ${SRC}/src/wallet/walletdb.cpp 
                        ${SRC}/src/wallet/walletutil.cpp 
                        ${BITCOIN_CORE_H})
add_library(${bitcoin}_wallet STATIC ${lib${bitcoin}_wallet_a_SOURCES})
target_compile_options(${bitcoin}_wallet PRIVATE -DHAVE_CONFIG_H)
source_group(Wallet FILES ${lib${bitcoin}_wallet_a_SOURCES})

add_library(${bitcoin}_cli STATIC ${SRC}/src/rpc/client.cpp)

if(UNIX)
  find_package(ZeroMQ)

  set(libbpq_zmq_a_SOURCES 
                            ${SRC}/src/zmq/zmqabstractnotifier.cpp 
                            ${SRC}/src/zmq/zmqnotificationinterface.cpp 
                            ${SRC}/src/zmq/zmqpublishnotifier.cpp)
  add_library(${bitcoin}_zmq STATIC ${libbpq_zmq_a_SOURCES})
endif()

include_directories(${SRC}/src
                    ${SRC}/src/univalue/include
                    ${SRC}/src/leveldb/include
                    ${SRC}/src/leveldb/helpers/memenv
                    ${SRC}/src/secp256k1/include                    
                    ${OPENSSL_INCLUDE_DIR}
                    ${BerkeleyDB_INCLUDE_DIR}
                    ${Boost_INCLUDE_DIR}
                    ${BOTAN_INCLUDE_DIR}
                    )

# External библиотеки компилируются внутри
# Univalue
set(libunivalue_la_SOURCES
                          ${SRC}/src/univalue/lib/univalue.cpp
                          ${SRC}/src/univalue/lib/univalue_get.cpp
                          ${SRC}/src/univalue/lib/univalue_read.cpp
                          ${SRC}/src/univalue/lib/univalue_write.cpp)

add_library(univalue STATIC ${libunivalue_la_SOURCES})

# secp256k1
add_executable(gen_context ${SRC}/src/secp256k1/src/gen_context.c)

set(libsecp256k1_la_SOURCES 
                          ${SRC}/src/secp256k1/src/secp256k1.c)
include_directories(${SRC}/src/secp256k1)                    
add_library(secp256k1 STATIC ${libsecp256k1_la_SOURCES})

target_compile_options(secp256k1 PRIVATE -DSECP256K1_BUILD -DHAVE_CONFIG_H)
add_dependencies(secp256k1 gen_context)

add_custom_command(TARGET gen_context POST_BUILD
                   COMMAND gen_context
                   WORKING_DIRECTORY ${SRC}/src/secp256k1)


# leveldb
set(libleveldb_la_SOURCES 
                          ${SRC}/src/leveldb/db/builder.cc
                          ${SRC}/src/leveldb/db/c.cc
                          ${SRC}/src/leveldb/db/dbformat.cc
                          ${SRC}/src/leveldb/db/db_impl.cc
                          ${SRC}/src/leveldb/db/db_iter.cc
                          ${SRC}/src/leveldb/db/dumpfile.cc
                          ${SRC}/src/leveldb/db/filename.cc
                          ${SRC}/src/leveldb/db/log_reader.cc
                          ${SRC}/src/leveldb/db/log_writer.cc
                          ${SRC}/src/leveldb/db/memtable.cc
                          ${SRC}/src/leveldb/db/repair.cc
                          ${SRC}/src/leveldb/db/table_cache.cc
                          ${SRC}/src/leveldb/db/version_edit.cc
                          ${SRC}/src/leveldb/db/version_set.cc
                          ${SRC}/src/leveldb/db/write_batch.cc
                          ${SRC}/src/leveldb/table/block_builder.cc
                          ${SRC}/src/leveldb/table/block.cc
                          ${SRC}/src/leveldb/table/filter_block.cc
                          ${SRC}/src/leveldb/table/format.cc
                          ${SRC}/src/leveldb/table/iterator.cc
                          ${SRC}/src/leveldb/table/merger.cc
                          ${SRC}/src/leveldb/table/table_builder.cc
                          ${SRC}/src/leveldb/table/table.cc
                          ${SRC}/src/leveldb/table/two_level_iterator.cc
                          ${SRC}/src/leveldb/util/arena.cc
                          ${SRC}/src/leveldb/util/bloom.cc
                          ${SRC}/src/leveldb/util/cache.cc
                          ${SRC}/src/leveldb/util/coding.cc
                          ${SRC}/src/leveldb/util/comparator.cc
                          ${SRC}/src/leveldb/util/crc32c.cc
                          ${SRC}/src/leveldb/util/env.cc
                          ${SRC}/src/leveldb/util/env_posix.cc
                          ${SRC}/src/leveldb/util/filter_policy.cc
                          ${SRC}/src/leveldb/util/hash.cc
                          ${SRC}/src/leveldb/util/histogram.cc
                          ${SRC}/src/leveldb/util/logging.cc
                          ${SRC}/src/leveldb/util/options.cc
                          ${SRC}/src/leveldb/util/status.cc)

if (UNIX AND NOT APPLE)
  set(leveldb_FLAGS -DOS_LINUX -DLEVELDB_PLATFORM_POSIX)
  set(libleveldb_la_SOURCES ${libleveldb_la_SOURCES} 
                            ${SRC}/src/leveldb/port/port_posix.cc
                            ${SRC}/src/leveldb/port/port_posix_sse.cc)
elseif(APPLE)  
  set(leveldb_FLAGS -DOS_MACOSX -DLEVELDB_PLATFORM_POSIX)
  set(libleveldb_la_SOURCES ${libleveldb_la_SOURCES} 
                            ${SRC}/src/leveldb/port/port_posix.cc
                            ${SRC}/src/leveldb/port/port_posix_sse.cc)
elseif(WIN32)   
endif()  
  
include_directories(${SRC}/src/leveldb)                    
add_library(leveldb STATIC ${libleveldb_la_SOURCES})
target_compile_options(leveldb PRIVATE ${leveldb_FLAGS} -Wno-deprecated-declarations)

add_library(memenv STATIC ${SRC}/src/leveldb/helpers/memenv/memenv.cc)
target_compile_options(memenv PRIVATE ${leveldb_FLAGS} -Wno-deprecated-declarations)

add_executable(${bitcoin}d ${SRC}/src/bitcoind.cpp)
target_compile_options(${bitcoin}d PRIVATE -DHAVE_CONFIG_H)

target_link_libraries(${bitcoin}d
                      ${bitcoin}_server
                      ${bitcoin}_common
                      ${bitcoin}_consensus
                      ${bitcoin}_util
                      ${bitcoin}_wallet
                      ${bitcoin}_crypto
                      ${bitcoin}_zmq

                      univalue
                      secp256k1
                      leveldb
                      memenv
                      bpqcrypto
                      ${BerkeleyDB_LIB}
                      ${OPENSSL_LIBRARIES}
                      ${Boost_LIBRARIES}
                      ${LIBEVENT_LIB}
                      ${LIBEVENT_PTHREADS_LIB}
                      ${MINIUPNP_LIBRARIES}
                      ${BOTAN_LIBRARY}
                      ${Sodium_LIB}
                      ${APPLE_FRAMEWORKS}
                      ${ZeroMQ_LIBRARY}
                      )
add_dependencies(${bitcoin}d 
                  ${bitcoin}_server
                  ${bitcoin}_crypto
                  ${bitcoin}_common
                  ${bitcoin}_consensus
                  ${bitcoin}_util
                  ${bitcoin}_wallet
                  univalue 
                  secp256k1 
                  leveldb)

set(${bitcoin}_cli_SOURCES ${SRC}/src/bitcoin-cli.cpp)
if(WIN32)
  set(${bitcoin}_cli_SOURCES ${${bitcoin}_cli_SOURCES} ${SRC}/src/bitcoin-cli-res.rc)
endif()  

add_executable(${bitcoin}-cli ${${bitcoin}_cli_SOURCES})
target_compile_options(${bitcoin}-cli PRIVATE -DHAVE_CONFIG_H)

target_link_libraries(${bitcoin}-cli
                      ${bitcoin}_cli
                      ${bitcoin}_util
                      ${bitcoin}_crypto
                      univalue
                      ${OPENSSL_LIBRARIES}
                      ${Boost_LIBRARIES}
                      ${LIBEVENT_LIB}
                      ${BOTAN_LIBRARY}
                      ${Sodium_LIB}
                      )
add_dependencies(${bitcoin}-cli
                      ${bitcoin}_cli
                      ${bitcoin}_util
                      ${bitcoin}_crypto
                      univalue)

if(BUILD_QT_CLIENT)
  set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ~/Qt/5.10.1/clang_64/lib/cmake)
  find_package(Qt5 COMPONENTS Core Widgets Network DBus LinguistTools)
  if (Qt5Core_FOUND AND Qt5Widgets_FOUND AND Qt5Network_FOUND AND Qt5DBus_FOUND)
    include(${bitcoin}-qt)
  endif()
endif()  

if(BUILD_TESTS)
  include(${bitcoin}-tests)
endif()  
