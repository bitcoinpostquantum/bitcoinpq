

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/cmake)

include(BoostTestHelpers)
include(CTest)

set(BITCOIN_TESTS
                ${SRC}/src/test/arith_uint256_tests.cpp 
                ${SRC}/src/test/scriptnum10.h 
                ${SRC}/src/test/addrman_tests.cpp 
                ${SRC}/src/test/amount_tests.cpp 
                ${SRC}/src/test/allocator_tests.cpp 
                ${SRC}/src/test/base32_tests.cpp 
                ${SRC}/src/test/base58_tests.cpp 
                ${SRC}/src/test/base64_tests.cpp 
                ${SRC}/src/test/bech32_tests.cpp 
                ${SRC}/src/test/bip32_tests.cpp 
                ${SRC}/src/test/blockchain_difficulty_tests.cpp 
                ${SRC}/src/test/blockencodings_tests.cpp 
                ${SRC}/src/test/bloom_tests.cpp 
                ${SRC}/src/test/bswap_tests.cpp 
                ${SRC}/src/test/checkqueue_tests.cpp 
                ${SRC}/src/test/coins_tests.cpp 
                ${SRC}/src/test/compress_tests.cpp 
                ${SRC}/src/test/crypto_tests.cpp 
                ${SRC}/src/test/cuckoocache_tests.cpp 
                ${SRC}/src/test/DoS_tests.cpp
                ${SRC}/src/test/equihash_tests.cpp
                ${SRC}/src/test/getarg_tests.cpp 
                ${SRC}/src/test/hash_tests.cpp 
                ${SRC}/src/test/key_tests.cpp 
                ${SRC}/src/test/limitedmap_tests.cpp 
                ${SRC}/src/test/dbwrapper_tests.cpp 
                ${SRC}/src/test/main_tests.cpp 
                ${SRC}/src/test/mempool_tests.cpp 
                ${SRC}/src/test/merkle_tests.cpp 
                ${SRC}/src/test/merkleblock_tests.cpp 
                ${SRC}/src/test/miner_tests.cpp 
                ${SRC}/src/test/multisig_tests.cpp 
                ${SRC}/src/test/net_tests.cpp 
                ${SRC}/src/test/netbase_tests.cpp 
                ${SRC}/src/test/pmt_tests.cpp 
                ${SRC}/src/test/policyestimator_tests.cpp 
                ${SRC}/src/test/pow_tests.cpp 
                ${SRC}/src/test/prevector_tests.cpp 
                ${SRC}/src/test/raii_event_tests.cpp 
                ${SRC}/src/test/random_tests.cpp 
                ${SRC}/src/test/reverselock_tests.cpp 
                #${SRC}/src/test/rpc_tests.cpp 
                ${SRC}/src/test/sanity_tests.cpp 
                ${SRC}/src/test/scheduler_tests.cpp 
                ${SRC}/src/test/script_P2SH_tests.cpp 
                ${SRC}/src/test/script_tests.cpp 
                ${SRC}/src/test/script_standard_tests.cpp 
                ${SRC}/src/test/scriptnum_tests.cpp 
                ${SRC}/src/test/serialize_tests.cpp 
                ${SRC}/src/test/sighash_tests.cpp 
                ${SRC}/src/test/sigopcount_tests.cpp 
                ${SRC}/src/test/skiplist_tests.cpp 
                ${SRC}/src/test/streams_tests.cpp 
                ${SRC}/src/test/test_bitcoin.cpp 
                ${SRC}/src/test/test_bitcoin.h 
                ${SRC}/src/test/timedata_tests.cpp 
                ${SRC}/src/test/torcontrol_tests.cpp 
                ${SRC}/src/test/transaction_tests.cpp 
                ${SRC}/src/test/txvalidation_tests.cpp 
                ${SRC}/src/test/txvalidationcache_tests.cpp 
                ${SRC}/src/test/versionbits_tests.cpp 
                ${SRC}/src/test/uint256_tests.cpp 
                ${SRC}/src/test/util_tests.cpp)


#if (ENABLE_WALLET)
set(BITCOIN_TESTS ${BITCOIN_TESTS}
                ${SRC}/src/wallet/test/wallet_test_fixture.cpp 
                ${SRC}/src/wallet/test/wallet_test_fixture.h 
                ${SRC}/src/wallet/test/accounting_tests.cpp 
                ${SRC}/src/wallet/test/wallet_tests.cpp 
                ${SRC}/src/wallet/test/crypto_tests.cpp)
#endif()


set(test_test_bitcoin_SOURCES ${BITCOIN_TESTS} ${JSON_TEST_FILES} ${RAW_TEST_FILES})

# Для некоторых тестов генерация headers из json 
find_program(HEXDUMP hexdump) 
find_program(SED sed)

set(JSON_TEST_FILES 
                      ${SRC}/src/test/data/script_tests.json 
                      ${SRC}/src/test/data/base58_keys_valid.json 
                      ${SRC}/src/test/data/base58_encode_decode.json 
                      ${SRC}/src/test/data/base58_keys_invalid.json 
                      ${SRC}/src/test/data/tx_invalid.json 
                      ${SRC}/src/test/data/tx_valid.json 
                      ${SRC}/src/test/data/sighash.json)

foreach(json ${JSON_TEST_FILES})
    get_filename_component(test_name ${json} NAME_WE)
    add_custom_command(OUTPUT ${json}.h
                COMMAND ${CMAKE_COMMAND} -E echo "namespace json_tests{ " > ${json}.h
                COMMAND ${CMAKE_COMMAND} -E echo "static unsigned const char ${test_name}[] = {" >> ${json}.h
                COMMAND ${HEXDUMP} -v -e "8/1 \"0x%02x, \"" -e "\"\\n\"" ${json} | ${SED} -e "s/0x  ,//g" >> ${json}.h
                COMMAND ${CMAKE_COMMAND} -E echo "};};" >> ${json}.h
                COMMENT "Generate test headers from file ${json}"
                VERBATIM)
    set_property(SOURCE ${json}.h PROPERTY SKIP_AUTOMOC ON)
    set_property(SOURCE ${json}.h PROPERTY SKIP_AUTOUIC ON)
    set(test_test_bitcoin_SOURCES ${test_test_bitcoin_SOURCES} ${json}.h)
endforeach(json)   



include_directories(${SRC}/src/test/data)
add_executable(test_${bitcoin}                
                ${SRC}/src/test/test_bitcoin_main.cpp 
                ${test_test_bitcoin_SOURCES})

target_compile_definitions(test_${bitcoin} PRIVATE -DBOOST_TEST_DYN_LINK -DHAVE_CONFIG_H)

target_link_libraries(test_${bitcoin}
                        ${bitcoin}_server
                        ${bitcoin}_cli
                        ${bitcoin}_common
                        ${bitcoin}_util
                        ${bitcoin}_consensus
                        ${bitcoin}_wallet
                        ${bitcoin}_crypto

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
                        ${Sodium_LIB})

# Парсим исходники чтобы разделить тесты
foreach(test ${BITCOIN_TESTS})

    add_bitcoin_test(test_${bitcoin} ${test})

endforeach(test)   


find_package(PythonInterp 3 REQUIRED)
set(abs_top_srcdir ${CMAKE_SOURCE_DIR})
set(abs_top_builddir ${CMAKE_BINARY_DIR})
configure_file(${CMAKE_SOURCE_DIR}/test/config.ini.in ${CMAKE_SOURCE_DIR}/test/config.ini)

# Для функциональных тестов на python3 нужен каталог src
add_custom_command(
    TARGET ${bitcoin}d POST_BUILD VERBATIM
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/src
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${bitcoin}d>" ${CMAKE_BINARY_DIR}/src)
add_custom_command(
    TARGET ${bitcoin}-cli POST_BUILD VERBATIM
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/src
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${bitcoin}-cli>" ${CMAKE_BINARY_DIR}/src/)


#file(GLOB functional_tests ${CMAKE_SOURCE_DIR}/test/functional/*.py)
#list(REMOVE_ITEM functional_tests ${CMAKE_SOURCE_DIR}/test/functional/test_runner.py)
set(functional_tests 
    # Scripts that are run by the travis build process.
    # Longest test should go first, to favor running tests in parallel
                    ${CMAKE_SOURCE_DIR}/test/functional/wallet_hd.py
                    ${CMAKE_SOURCE_DIR}/test/functional/wallet_backup.py
                    # vv Tests less than 5m vv
                    ${CMAKE_SOURCE_DIR}/test/functional/feature_block.py
                    ${CMAKE_SOURCE_DIR}/test/functional/rpc_fundrawtransaction.py
                    ${CMAKE_SOURCE_DIR}/test/functional/p2p_compactblocks.py
                    ${CMAKE_SOURCE_DIR}/test/functional/feature_segwit.py
                    # vv Tests less than 2m vv
                    ${CMAKE_SOURCE_DIR}/test/functional/wallet_basic.py
                    ${CMAKE_SOURCE_DIR}/test/functional/wallet_accounts.py
                    ${CMAKE_SOURCE_DIR}/test/functional/p2p_segwit.py
                    ${CMAKE_SOURCE_DIR}/test/functional/wallet_dump.py
                    ${CMAKE_SOURCE_DIR}/test/functional/rpc_listtransactions.py
                    # vv Tests less than 60s vv
                    ${CMAKE_SOURCE_DIR}/test/functional/p2p_sendheaders.py
                    ${CMAKE_SOURCE_DIR}/test/functional/wallet_zapwallettxes.py
                    ${CMAKE_SOURCE_DIR}/test/functional/wallet_importmulti.py
                    ${CMAKE_SOURCE_DIR}/test/functional/mempool_limit.py
                    ${CMAKE_SOURCE_DIR}/test/functional/rpc_txoutproof.py
                    ${CMAKE_SOURCE_DIR}/test/functional/wallet_listreceivedby.py
                    ${CMAKE_SOURCE_DIR}/test/functional/wallet_abandonconflict.py
                    ${CMAKE_SOURCE_DIR}/test/functional/feature_csv_activation.py
                    ${CMAKE_SOURCE_DIR}/test/functional/rpc_rawtransaction.py
                    ${CMAKE_SOURCE_DIR}/test/functional/wallet_address_types.py
                    ${CMAKE_SOURCE_DIR}/test/functional/feature_reindex.py
                    # vv Tests less than 30s vv
                    ${CMAKE_SOURCE_DIR}/test/functional/wallet_keypool_topup.py
                    ${CMAKE_SOURCE_DIR}/test/functional/interface_zmq.py
                    ${CMAKE_SOURCE_DIR}/test/functional/interface_bitcoin_cli.py
                    ${CMAKE_SOURCE_DIR}/test/functional/mempool_resurrect.py
                    #"${CMAKE_SOURCE_DIR}/test/functional/wallet_txn_doublespend.py --mineblock"
                    ${CMAKE_SOURCE_DIR}/test/functional/wallet_txn_clone.py
                    #"${CMAKE_SOURCE_DIR}/test/functional/wallet_txn_clone.py --segwit"
                    ${CMAKE_SOURCE_DIR}/test/functional/rpc_getchaintips.py
                    ${CMAKE_SOURCE_DIR}/test/functional/interface_rest.py
                    ${CMAKE_SOURCE_DIR}/test/functional/mempool_spend_coinbase.py
                    ${CMAKE_SOURCE_DIR}/test/functional/mempool_reorg.py
                    ${CMAKE_SOURCE_DIR}/test/functional/mempool_persist.py
                    ${CMAKE_SOURCE_DIR}/test/functional/wallet_multiwallet.py
                    #"${CMAKE_SOURCE_DIR}/test/functional/wallet_multiwallet.py --usecli"
                    ${CMAKE_SOURCE_DIR}/test/functional/interface_http.py
                    ${CMAKE_SOURCE_DIR}/test/functional/rpc_users.py
                    ${CMAKE_SOURCE_DIR}/test/functional/feature_proxy.py
                    ${CMAKE_SOURCE_DIR}/test/functional/rpc_signrawtransaction.py
                    ${CMAKE_SOURCE_DIR}/test/functional/p2p_disconnect_ban.py
                    ${CMAKE_SOURCE_DIR}/test/functional/rpc_decodescript.py
                    ${CMAKE_SOURCE_DIR}/test/functional/rpc_blockchain.py
                    ${CMAKE_SOURCE_DIR}/test/functional/rpc_deprecated.py
                    ${CMAKE_SOURCE_DIR}/test/functional/wallet_disable.py
                    ${CMAKE_SOURCE_DIR}/test/functional/rpc_net.py
                    ${CMAKE_SOURCE_DIR}/test/functional/wallet_keypool.py
                    ${CMAKE_SOURCE_DIR}/test/functional/p2p_mempool.py
                    ${CMAKE_SOURCE_DIR}/test/functional/mining_prioritisetransaction.py
                    ${CMAKE_SOURCE_DIR}/test/functional/p2p_invalid_block.py
                    ${CMAKE_SOURCE_DIR}/test/functional/p2p_invalid_tx.py
                    ${CMAKE_SOURCE_DIR}/test/functional/feature_versionbits_warning.py
                    ${CMAKE_SOURCE_DIR}/test/functional/rpc_preciousblock.py
                    ${CMAKE_SOURCE_DIR}/test/functional/wallet_importprunedfunds.py
                    ${CMAKE_SOURCE_DIR}/test/functional/rpc_signmessage.py
                    ${CMAKE_SOURCE_DIR}/test/functional/feature_nulldummy.py
                    ${CMAKE_SOURCE_DIR}/test/functional/wallet_import_rescan.py
                    ${CMAKE_SOURCE_DIR}/test/functional/mining_basic.py
                    ${CMAKE_SOURCE_DIR}/test/functional/wallet_bumpfee.py
                    ${CMAKE_SOURCE_DIR}/test/functional/rpc_named_arguments.py
                    ${CMAKE_SOURCE_DIR}/test/functional/wallet_listsinceblock.py
                    ${CMAKE_SOURCE_DIR}/test/functional/p2p_leak.py
                    ${CMAKE_SOURCE_DIR}/test/functional/wallet_encryption.py
                    ${CMAKE_SOURCE_DIR}/test/functional/feature_dersig.py
                    ${CMAKE_SOURCE_DIR}/test/functional/feature_cltv.py
                    ${CMAKE_SOURCE_DIR}/test/functional/rpc_uptime.py
                    ${CMAKE_SOURCE_DIR}/test/functional/wallet_resendwallettransactions.py
                    ${CMAKE_SOURCE_DIR}/test/functional/feature_minchainwork.py
                    ${CMAKE_SOURCE_DIR}/test/functional/p2p_fingerprint.py
                    ${CMAKE_SOURCE_DIR}/test/functional/feature_uacomment.py
                    ${CMAKE_SOURCE_DIR}/test/functional/p2p_unrequested_blocks.py
                    ${CMAKE_SOURCE_DIR}/test/functional/feature_logging.py
                    ${CMAKE_SOURCE_DIR}/test/functional/p2p_node_network_limited.py
                    ${CMAKE_SOURCE_DIR}/test/functional/feature_config_args.py)



foreach(test ${functional_tests})
    get_filename_component(test_name ${test} NAME_WE)
    add_test(NAME "Python-${test_name}"
             COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/functional/test_runner.py --jobs=1 ${test_name}.py
             WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endforeach(test)   

