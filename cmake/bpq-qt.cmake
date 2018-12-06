

#set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ~/Qt/5.10.1/clang_64)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOMOC_MOC_OPTIONS -DQ_OS_MAC)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS} 
                                       ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}
                                       ${Qt5Network_EXECUTABLE_COMPILE_FLAGS}
                                       ${Qt5DBus_EXECUTABLE_COMPILE_FLAGS})

include_directories(${Qt5Core_INCLUDE_DIRS} 
                    ${Qt5Widgets_INCLUDE_DIRS} 
                    ${Qt5Network_INCLUDE_DIRS} 
                    ${Qt5DBus_INCLUDE_DIRS} 
                    ${SRC}/src)
# bpq qt core #
set(QT_TS
              ${SRC}/src/qt/locale/bitcoin_af.ts 
              ${SRC}/src/qt/locale/bitcoin_af_ZA.ts 
              ${SRC}/src/qt/locale/bitcoin_am.ts 
              ${SRC}/src/qt/locale/bitcoin_ar.ts 
              ${SRC}/src/qt/locale/bitcoin_be_BY.ts 
              ${SRC}/src/qt/locale/bitcoin_bg_BG.ts 
              ${SRC}/src/qt/locale/bitcoin_bg.ts 
              ${SRC}/src/qt/locale/bitcoin_bn.ts 
              ${SRC}/src/qt/locale/bitcoin_bs.ts 
              ${SRC}/src/qt/locale/bitcoin_ca_ES.ts 
              ${SRC}/src/qt/locale/bitcoin_ca.ts 
              ${SRC}/src/qt/locale/bitcoin_ca@valencia.ts 
              ${SRC}/src/qt/locale/bitcoin_cs_CZ.ts 
              ${SRC}/src/qt/locale/bitcoin_cs.ts 
              ${SRC}/src/qt/locale/bitcoin_cy.ts 
              ${SRC}/src/qt/locale/bitcoin_da.ts 
              ${SRC}/src/qt/locale/bitcoin_de_DE.ts 
              ${SRC}/src/qt/locale/bitcoin_de.ts 
              ${SRC}/src/qt/locale/bitcoin_el_GR.ts 
              ${SRC}/src/qt/locale/bitcoin_el.ts 
              ${SRC}/src/qt/locale/bitcoin_en_AU.ts 
              ${SRC}/src/qt/locale/bitcoin_en_GB.ts 
              ${SRC}/src/qt/locale/bitcoin_en.ts 
              ${SRC}/src/qt/locale/bitcoin_eo.ts 
              ${SRC}/src/qt/locale/bitcoin_es_419.ts 
              ${SRC}/src/qt/locale/bitcoin_es_AR.ts 
              ${SRC}/src/qt/locale/bitcoin_es_CL.ts 
              ${SRC}/src/qt/locale/bitcoin_es_CO.ts 
              ${SRC}/src/qt/locale/bitcoin_es_DO.ts 
              ${SRC}/src/qt/locale/bitcoin_es_ES.ts 
              ${SRC}/src/qt/locale/bitcoin_es_MX.ts 
              ${SRC}/src/qt/locale/bitcoin_es.ts 
              ${SRC}/src/qt/locale/bitcoin_es_UY.ts 
              ${SRC}/src/qt/locale/bitcoin_es_VE.ts 
              ${SRC}/src/qt/locale/bitcoin_et_EE.ts 
              ${SRC}/src/qt/locale/bitcoin_et.ts 
              ${SRC}/src/qt/locale/bitcoin_eu_ES.ts 
              ${SRC}/src/qt/locale/bitcoin_fa_IR.ts 
              ${SRC}/src/qt/locale/bitcoin_fa.ts 
              ${SRC}/src/qt/locale/bitcoin_fi.ts 
              ${SRC}/src/qt/locale/bitcoin_fr_CA.ts 
              ${SRC}/src/qt/locale/bitcoin_fr_FR.ts 
              ${SRC}/src/qt/locale/bitcoin_fr.ts 
              ${SRC}/src/qt/locale/bitcoin_gl.ts 
              ${SRC}/src/qt/locale/bitcoin_he_IL.ts 
              ${SRC}/src/qt/locale/bitcoin_he.ts 
              ${SRC}/src/qt/locale/bitcoin_hi_IN.ts 
              ${SRC}/src/qt/locale/bitcoin_hr.ts 
              ${SRC}/src/qt/locale/bitcoin_hu_HU.ts 
              ${SRC}/src/qt/locale/bitcoin_hu.ts 
              ${SRC}/src/qt/locale/bitcoin_id_ID.ts 
              ${SRC}/src/qt/locale/bitcoin_id.ts 
              ${SRC}/src/qt/locale/bitcoin_is.ts 
              ${SRC}/src/qt/locale/bitcoin_it_IT.ts 
              ${SRC}/src/qt/locale/bitcoin_it.ts 
              ${SRC}/src/qt/locale/bitcoin_ja_JP.ts 
              ${SRC}/src/qt/locale/bitcoin_ja.ts 
              ${SRC}/src/qt/locale/bitcoin_ka.ts 
              ${SRC}/src/qt/locale/bitcoin_kk_KZ.ts 
              ${SRC}/src/qt/locale/bitcoin_km_KH.ts 
              ${SRC}/src/qt/locale/bitcoin_ko_KR.ts 
              ${SRC}/src/qt/locale/bitcoin_ko.ts 
              ${SRC}/src/qt/locale/bitcoin_ku_IQ.ts 
              ${SRC}/src/qt/locale/bitcoin_ky.ts 
              ${SRC}/src/qt/locale/bitcoin_la.ts 
              ${SRC}/src/qt/locale/bitcoin_lt.ts 
              ${SRC}/src/qt/locale/bitcoin_lv_LV.ts 
              ${SRC}/src/qt/locale/bitcoin_mk_MK.ts 
              ${SRC}/src/qt/locale/bitcoin_ml.ts 
              ${SRC}/src/qt/locale/bitcoin_mn.ts 
              ${SRC}/src/qt/locale/bitcoin_mr_IN.ts 
              ${SRC}/src/qt/locale/bitcoin_ms_MY.ts 
              ${SRC}/src/qt/locale/bitcoin_ms.ts 
              ${SRC}/src/qt/locale/bitcoin_my.ts 
              ${SRC}/src/qt/locale/bitcoin_nb_NO.ts 
              ${SRC}/src/qt/locale/bitcoin_nb.ts 
              ${SRC}/src/qt/locale/bitcoin_ne.ts 
              ${SRC}/src/qt/locale/bitcoin_nl_NL.ts 
              ${SRC}/src/qt/locale/bitcoin_nl.ts 
              ${SRC}/src/qt/locale/bitcoin_pam.ts 
              ${SRC}/src/qt/locale/bitcoin_pl_PL.ts 
              ${SRC}/src/qt/locale/bitcoin_pl.ts 
              ${SRC}/src/qt/locale/bitcoin_pt_BR.ts 
              ${SRC}/src/qt/locale/bitcoin_pt_PT.ts 
              ${SRC}/src/qt/locale/bitcoin_pt.ts 
              ${SRC}/src/qt/locale/bitcoin_ro_RO.ts 
              ${SRC}/src/qt/locale/bitcoin_ro.ts 
              ${SRC}/src/qt/locale/bitcoin_ru_RU.ts 
              ${SRC}/src/qt/locale/bitcoin_ru.ts 
              ${SRC}/src/qt/locale/bitcoin_si.ts 
              ${SRC}/src/qt/locale/bitcoin_sk.ts 
              ${SRC}/src/qt/locale/bitcoin_sl_SI.ts 
              ${SRC}/src/qt/locale/bitcoin_sn.ts 
              ${SRC}/src/qt/locale/bitcoin_sq.ts 
              ${SRC}/src/qt/locale/bitcoin_sr@latin.ts 
              ${SRC}/src/qt/locale/bitcoin_sr.ts 
              ${SRC}/src/qt/locale/bitcoin_sv.ts 
              ${SRC}/src/qt/locale/bitcoin_ta_IN.ts 
              ${SRC}/src/qt/locale/bitcoin_ta.ts 
              ${SRC}/src/qt/locale/bitcoin_te.ts 
              ${SRC}/src/qt/locale/bitcoin_th_TH.ts 
              ${SRC}/src/qt/locale/bitcoin_th.ts 
              ${SRC}/src/qt/locale/bitcoin_tr_TR.ts 
              ${SRC}/src/qt/locale/bitcoin_tr.ts 
              ${SRC}/src/qt/locale/bitcoin_uk.ts 
              ${SRC}/src/qt/locale/bitcoin_ur_PK.ts 
              ${SRC}/src/qt/locale/bitcoin_uz@Cyrl.ts 
              ${SRC}/src/qt/locale/bitcoin_vi.ts 
              ${SRC}/src/qt/locale/bitcoin_vi_VN.ts 
              ${SRC}/src/qt/locale/bitcoin_zh_CN.ts 
              ${SRC}/src/qt/locale/bitcoin_zh-Hans.ts 
              ${SRC}/src/qt/locale/bitcoin_zh_HK.ts 
              ${SRC}/src/qt/locale/bitcoin_zh.ts
              ${SRC}/src/qt/locale/bitcoin_zh_TW.ts)

set(QT_FORMS_UI
              ${SRC}/src/qt/forms/addressbookpage.ui 
              ${SRC}/src/qt/forms/askpassphrasedialog.ui 
              ${SRC}/src/qt/forms/coincontroldialog.ui 
              ${SRC}/src/qt/forms/editaddressdialog.ui 
              ${SRC}/src/qt/forms/firsttransactionpage.ui
              ${SRC}/src/qt/forms/helpmessagedialog.ui 
              ${SRC}/src/qt/forms/intro.ui 
              ${SRC}/src/qt/forms/modaloverlay.ui 
              ${SRC}/src/qt/forms/openuridialog.ui 
              ${SRC}/src/qt/forms/optionsdialog.ui 
              ${SRC}/src/qt/forms/overviewpage.ui 
              ${SRC}/src/qt/forms/receivecoinsdialog.ui 
              ${SRC}/src/qt/forms/receiverequestdialog.ui 
              ${SRC}/src/qt/forms/debugwindow.ui 
              ${SRC}/src/qt/forms/sendcoinsdialog.ui 
              ${SRC}/src/qt/forms/sendcoinsentry.ui 
              ${SRC}/src/qt/forms/signverifymessagedialog.ui 
              ${SRC}/src/qt/forms/transactiondescdialog.ui
              )

set(QT_MOC_CPP 
              ${SRC}/src/qt/moc_addressbookpage.cpp 
              ${SRC}/src/qt/moc_addresstablemodel.cpp 
              ${SRC}/src/qt/moc_askpassphrasedialog.cpp 
              ${SRC}/src/qt/moc_bantablemodel.cpp 
              ${SRC}/src/qt/moc_bitcoinaddressvalidator.cpp 
              ${SRC}/src/qt/moc_bitcoinamountfield.cpp 
              ${SRC}/src/qt/moc_bitcoingui.cpp 
              ${SRC}/src/qt/moc_bitcoinunits.cpp 
              ${SRC}/src/qt/moc_callback.cpp 
              ${SRC}/src/qt/moc_clientmodel.cpp 
              ${SRC}/src/qt/moc_coincontroldialog.cpp 
              ${SRC}/src/qt/moc_coincontroltreewidget.cpp 
              ${SRC}/src/qt/moc_csvmodelwriter.cpp 
              ${SRC}/src/qt/moc_editaddressdialog.cpp 
              ${SRC}/src/qt/moc_firsttransactionpage.cpp
              ${SRC}/src/qt/moc_guiutil.cpp 
              ${SRC}/src/qt/moc_intro.cpp 
              ${SRC}/src/qt/moc_macdockiconhandler.cpp 
              ${SRC}/src/qt/moc_macnotificationhandler.cpp 
              ${SRC}/src/qt/moc_modaloverlay.cpp 
              ${SRC}/src/qt/moc_notificator.cpp 
              ${SRC}/src/qt/moc_openuridialog.cpp 
              ${SRC}/src/qt/moc_optionsdialog.cpp 
              ${SRC}/src/qt/moc_optionsmodel.cpp 
              ${SRC}/src/qt/moc_overviewpage.cpp 
              ${SRC}/src/qt/moc_peertablemodel.cpp 
              ${SRC}/src/qt/moc_paymentserver.cpp 
              ${SRC}/src/qt/moc_qvalidatedlineedit.cpp 
              ${SRC}/src/qt/moc_qvaluecombobox.cpp 
              ${SRC}/src/qt/moc_receivecoinsdialog.cpp 
              ${SRC}/src/qt/moc_receiverequestdialog.cpp 
              ${SRC}/src/qt/moc_recentrequeststablemodel.cpp 
              ${SRC}/src/qt/moc_rpcconsole.cpp 
              ${SRC}/src/qt/moc_sendcoinsdialog.cpp 
              ${SRC}/src/qt/moc_sendcoinsentry.cpp 
              ${SRC}/src/qt/moc_signverifymessagedialog.cpp 
              ${SRC}/src/qt/moc_splashscreen.cpp 
              ${SRC}/src/qt/moc_trafficgraphwidget.cpp 
              ${SRC}/src/qt/moc_transactiondesc.cpp 
              ${SRC}/src/qt/moc_transactiondescdialog.cpp 
              ${SRC}/src/qt/moc_transactionfilterproxy.cpp 
              ${SRC}/src/qt/moc_transactiontablemodel.cpp 
              ${SRC}/src/qt/moc_transactionview.cpp 
              ${SRC}/src/qt/moc_utilitydialog.cpp 
              ${SRC}/src/qt/moc_walletframe.cpp 
              ${SRC}/src/qt/moc_walletmodel.cpp 
              ${SRC}/src/qt/moc_walletview.cpp)


set(QT_MOC
          ${SRC}/src/qt/bitcoin.moc 
          ${SRC}/src/qt/bitcoinamountfield.moc 
          ${SRC}/src/qt/callback.moc 
          ${SRC}/src/qt/firsttransactionpage.moc 
          ${SRC}/src/qt/intro.moc 
          ${SRC}/src/qt/overviewpage.moc 
          ${SRC}/src/qt/rpcconsole.moc)

set(QT_QRC_CPP        ${SRC}/src/qt/qrc_bitcoin.cpp)
set(QT_QRC            ${SRC}/src/qt/bitcoin.qrc)
set(QT_QRC_LOCALE_CPP ${SRC}/src/qt/qrc_bitcoin_locale.cpp)
set(QT_QRC_LOCALE     ${SRC}/src/qt/bitcoin_locale.qrc)


find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})

# Initially create generated files, if they don't yet exist
set(PROTOBUF_PROTO    ${SRC}/src/qt/paymentrequest.proto)
IF( NOT EXISTS ${SRC}/src/qt/paymentrequest.pb.cc)
  MESSAGE(STATUS "Create generated protobuf files ...")
  execute_process(
    COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} --proto_path=${SRC}/src/qt --cpp_out=${SRC}/src/qt ${PROTOBUF_PROTO}
  )
ENDIF()

set(PROTOBUF_CC   ${SRC}/src/qt/paymentrequest.pb.cc
                  ${SRC}/src/qt/paymentrequest.pb.h)

set(BPQ_QT_H
                      ${SRC}/src/qt/addressbookpage.h 
                      ${SRC}/src/qt/addresstablemodel.h 
                      ${SRC}/src/qt/askpassphrasedialog.h 
                      ${SRC}/src/qt/bantablemodel.h 
                      ${SRC}/src/qt/bitcoinaddressvalidator.h 
                      ${SRC}/src/qt/bitcoinamountfield.h 
                      ${SRC}/src/qt/bitcoingui.h 
                      ${SRC}/src/qt/bitcoinunits.h 
                      ${SRC}/src/qt/callback.h 
                      ${SRC}/src/qt/clientmodel.h 
                      ${SRC}/src/qt/coincontroldialog.h 
                      ${SRC}/src/qt/coincontroltreewidget.h 
                      ${SRC}/src/qt/csvmodelwriter.h 
                      ${SRC}/src/qt/editaddressdialog.h 
                      ${SRC}/src/qt/firsttransactionpage.h
                      ${SRC}/src/qt/guiconstants.h 
                      ${SRC}/src/qt/guiutil.h 
                      ${SRC}/src/qt/intro.h 
                      ${SRC}/src/qt/macdockiconhandler.h 
                      ${SRC}/src/qt/macnotificationhandler.h 
                      ${SRC}/src/qt/modaloverlay.h 
                      ${SRC}/src/qt/networkstyle.h 
                      ${SRC}/src/qt/notificator.h 
                      ${SRC}/src/qt/openuridialog.h 
                      ${SRC}/src/qt/optionsdialog.h 
                      ${SRC}/src/qt/optionsmodel.h 
                      ${SRC}/src/qt/overviewpage.h 
                      ${SRC}/src/qt/paymentrequestplus.h 
                      ${SRC}/src/qt/paymentserver.h 
                      ${SRC}/src/qt/peertablemodel.h 
                      ${SRC}/src/qt/platformstyle.h 
                      ${SRC}/src/qt/qvalidatedlineedit.h 
                      ${SRC}/src/qt/qvaluecombobox.h 
                      ${SRC}/src/qt/receivecoinsdialog.h 
                      ${SRC}/src/qt/receiverequestdialog.h 
                      ${SRC}/src/qt/recentrequeststablemodel.h 
                      ${SRC}/src/qt/rpcconsole.h 
                      ${SRC}/src/qt/sendcoinsdialog.h 
                      ${SRC}/src/qt/sendcoinsentry.h 
                      ${SRC}/src/qt/signverifymessagedialog.h 
                      ${SRC}/src/qt/splashscreen.h 
                      ${SRC}/src/qt/trafficgraphwidget.h 
                      ${SRC}/src/qt/transactiondesc.h 
                      ${SRC}/src/qt/transactiondescdialog.h 
                      ${SRC}/src/qt/transactionfilterproxy.h 
                      ${SRC}/src/qt/transactionrecord.h 
                      ${SRC}/src/qt/transactiontablemodel.h 
                      ${SRC}/src/qt/transactionview.h 
                      ${SRC}/src/qt/utilitydialog.h 
                      ${SRC}/src/qt/walletframe.h 
                      ${SRC}/src/qt/walletmodel.h 
                      ${SRC}/src/qt/walletmodeltransaction.h 
                      ${SRC}/src/qt/walletview.h 
                      ${SRC}/src/qt/winshutdownmonitor.h)

set(RES_ICONS
                      ${SRC}/src/qt/res/icons/add.png 
                      ${SRC}/src/qt/res/icons/address-book.png 
                      ${SRC}/src/qt/res/icons/about.png 
                      ${SRC}/src/qt/res/icons/about_qt.png 
                      ${SRC}/src/qt/res/icons/bitcoin.ico 
                      ${SRC}/src/qt/res/icons/bitcoin_testnet.ico 
                      ${SRC}/src/qt/res/icons/bitcoin.png 
                      ${SRC}/src/qt/res/icons/chevron.png 
                      ${SRC}/src/qt/res/icons/clock1.png 
                      ${SRC}/src/qt/res/icons/clock2.png 
                      ${SRC}/src/qt/res/icons/clock3.png 
                      ${SRC}/src/qt/res/icons/clock4.png 
                      ${SRC}/src/qt/res/icons/clock5.png 
                      ${SRC}/src/qt/res/icons/configure.png 
                      ${SRC}/src/qt/res/icons/connect0.png 
                      ${SRC}/src/qt/res/icons/connect1.png 
                      ${SRC}/src/qt/res/icons/connect2.png 
                      ${SRC}/src/qt/res/icons/connect3.png 
                      ${SRC}/src/qt/res/icons/connect4.png 
                      ${SRC}/src/qt/res/icons/debugwindow.png 
                      ${SRC}/src/qt/res/icons/edit.png 
                      ${SRC}/src/qt/res/icons/editcopy.png 
                      ${SRC}/src/qt/res/icons/editpaste.png 
                      ${SRC}/src/qt/res/icons/export.png 
                      ${SRC}/src/qt/res/icons/eye.png 
                      ${SRC}/src/qt/res/icons/eye_minus.png 
                      ${SRC}/src/qt/res/icons/eye_plus.png 
                      ${SRC}/src/qt/res/icons/filesave.png 
                      ${SRC}/src/qt/res/icons/fontbigger.png 
                      ${SRC}/src/qt/res/icons/fontsmaller.png 
                      ${SRC}/src/qt/res/icons/hd_disabled.png 
                      ${SRC}/src/qt/res/icons/hd_enabled.png 
                      ${SRC}/src/qt/res/icons/history.png 
                      ${SRC}/src/qt/res/icons/info.png 
                      ${SRC}/src/qt/res/icons/key.png 
                      ${SRC}/src/qt/res/icons/lock_closed.png 
                      ${SRC}/src/qt/res/icons/lock_open.png 
                      ${SRC}/src/qt/res/icons/network_disabled.png 
                      ${SRC}/src/qt/res/icons/open.png 
                      ${SRC}/src/qt/res/icons/overview.png 
                      ${SRC}/src/qt/res/icons/quit.png 
                      ${SRC}/src/qt/res/icons/receive.png 
                      ${SRC}/src/qt/res/icons/remove.png 
                      ${SRC}/src/qt/res/icons/send.png 
                      ${SRC}/src/qt/res/icons/synced.png 
                      ${SRC}/src/qt/res/icons/transaction0.png 
                      ${SRC}/src/qt/res/icons/transaction2.png 
                      ${SRC}/src/qt/res/icons/transaction_abandoned.png 
                      ${SRC}/src/qt/res/icons/transaction_conflicted.png 
                      ${SRC}/src/qt/res/icons/tx_inout.png 
                      ${SRC}/src/qt/res/icons/tx_input.png 
                      ${SRC}/src/qt/res/icons/tx_output.png 
                      ${SRC}/src/qt/res/icons/tx_mined.png 
                      ${SRC}/src/qt/res/icons/warning.png 
                      ${SRC}/src/qt/res/icons/verify.png)

set(BPQ_QT_BASE_CPP
                      ${SRC}/src/qt/bantablemodel.cpp 
                      ${SRC}/src/qt/bitcoinaddressvalidator.cpp 
                      ${SRC}/src/qt/bitcoinamountfield.cpp 
                      ${SRC}/src/qt/bitcoingui.cpp 
                      ${SRC}/src/qt/bitcoinunits.cpp 
                      ${SRC}/src/qt/clientmodel.cpp 
                      ${SRC}/src/qt/csvmodelwriter.cpp 
                      ${SRC}/src/qt/guiutil.cpp 
                      ${SRC}/src/qt/intro.cpp 
                      ${SRC}/src/qt/modaloverlay.cpp 
                      ${SRC}/src/qt/networkstyle.cpp 
                      ${SRC}/src/qt/notificator.cpp 
                      ${SRC}/src/qt/optionsdialog.cpp 
                      ${SRC}/src/qt/optionsmodel.cpp 
                      ${SRC}/src/qt/peertablemodel.cpp 
                      ${SRC}/src/qt/platformstyle.cpp 
                      ${SRC}/src/qt/qvalidatedlineedit.cpp 
                      ${SRC}/src/qt/qvaluecombobox.cpp 
                      ${SRC}/src/qt/rpcconsole.cpp 
                      ${SRC}/src/qt/splashscreen.cpp 
                      ${SRC}/src/qt/trafficgraphwidget.cpp 
                      ${SRC}/src/qt/utilitydialog.cpp)

if(APPLE)
  set(BPQ_QT_BASE_CPP ${BPQ_QT_BASE_CPP}
              ${SRC}/src/qt/macdockiconhandler.mm 
              ${SRC}/src/qt/macnotificationhandler.mm)
endif()


set(BPQ_QT_WINDOWS_CPP  ${SRC}/src/qt/winshutdownmonitor.cpp)

set(BPQ_QT_WALLET_CPP
                      ${SRC}/src/qt/addressbookpage.cpp 
                      ${SRC}/src/qt/addresstablemodel.cpp 
                      ${SRC}/src/qt/askpassphrasedialog.cpp 
                      ${SRC}/src/qt/coincontroldialog.cpp 
                      ${SRC}/src/qt/coincontroltreewidget.cpp 
                      ${SRC}/src/qt/editaddressdialog.cpp
                      ${SRC}/src/qt/firsttransactionpage.cpp
                      ${SRC}/src/qt/openuridialog.cpp 
                      ${SRC}/src/qt/overviewpage.cpp 
                      ${SRC}/src/qt/paymentrequestplus.cpp 
                      ${SRC}/src/qt/paymentserver.cpp 
                      ${SRC}/src/qt/receivecoinsdialog.cpp 
                      ${SRC}/src/qt/receiverequestdialog.cpp 
                      ${SRC}/src/qt/recentrequeststablemodel.cpp 
                      ${SRC}/src/qt/sendcoinsdialog.cpp 
                      ${SRC}/src/qt/sendcoinsentry.cpp 
                      ${SRC}/src/qt/signverifymessagedialog.cpp 
                      ${SRC}/src/qt/transactiondesc.cpp 
                      ${SRC}/src/qt/transactiondescdialog.cpp 
                      ${SRC}/src/qt/transactionfilterproxy.cpp 
                      ${SRC}/src/qt/transactionrecord.cpp 
                      ${SRC}/src/qt/transactiontablemodel.cpp 
                      ${SRC}/src/qt/transactionview.cpp 
                      ${SRC}/src/qt/walletframe.cpp 
                      ${SRC}/src/qt/walletmodel.cpp 
                      ${SRC}/src/qt/walletmodeltransaction.cpp 
                      ${SRC}/src/qt/walletview.cpp)

qt5_wrap_ui(qt_FORMS_HEADERS ${QT_FORMS_UI})

set(qt_libbpqqt_a_SOURCES 
                      ${BPQ_QT_BASE_CPP} 
                      ${BPQ_QT_WALLET_CPP}
                      ${BPQ_QT_H} 
                      ${qt_FORMS_HEADERS}
                      #${QT_QRC_LOCALE} 
                      ${QT_TS} 
                      ${PROTOBUF_PROTO}
                      ${PROTOBUF_CC} 
                      ${RES_ICONS} 
                      ${RES_IMAGES} 
                      ${RES_MOVIES})

set(qt_bpq_qt_SOURCES qt/bitcoin.cpp)
if (APPLE)
  set(qt_bpq_qt_SOURCES ${qt_bpq_qt_SOURCES} ${BPQ_MM})
elseif(WIN32)
  set(qt_bpq_qt_SOURCES ${qt_bpq_qt_SOURCES} ${BPQ_RC})
endif()


qt5_create_translation(QT_TRANSLATION_FILES ${QT_TS})

add_custom_command(OUTPUT  qt_forms_and_tarnslate_copy
                   COMMAND ${CMAKE_COMMAND} -E copy ${qt_FORMS_HEADERS} ${SRC}/src/qt/forms
                   COMMAND ${CMAKE_COMMAND} -E copy ${QT_TRANSLATION_FILES} ${SRC}/src/qt/locale
                   DEPENDS ${qt_FORMS_HEADERS} ${QT_TRANSLATION_FILES}
                   COMMENT "Copy ui forms headers..."
                   )
add_custom_target(qt_forms_and_tarnslate_copy_target ALL DEPENDS qt_forms_and_tarnslate_copy)

add_definitions(${Qt5Widgets_DEFINITIONS})
add_library(${bitcoin}_qt STATIC ${qt_libbpqqt_a_SOURCES})
add_dependencies(${bitcoin}_qt qt_forms_and_tarnslate_copy_target)
target_compile_definitions(${bitcoin}_qt PRIVATE -DHAVE_CONFIG_H)

add_executable(${bitcoin}-qt MACOSX_BUNDLE ${SRC}/src/qt/bitcoin.cpp ${QT_QRC} ${QT_QRC_LOCALE})
qt5_use_modules(${bitcoin}-qt Core Network Widgets DBus LinguistTools)


target_compile_options(${bitcoin}-qt PRIVATE -DHAVE_CONFIG_H )
target_link_libraries(${bitcoin}-qt 
                      ${bitcoin}_server
                      ${bitcoin}_wallet

                      ${bitcoin}_cli

                      ${bitcoin}_common
                      ${bitcoin}_util
                      ${bitcoin}_consensus
                      ${bitcoin}_crypto
                      ${bitcoin}_qt
                      ${bitcoin}_zmq
                      univalue
                      leveldb
                      memenv
                      bpqcrypto
                      ${Boost_LIBRARIES}
                      ${Sodium_LIB}
                      ${PROTOBUF_LIBS}
                      ${PROTOBUF_LIBRARIES}
                      ${BerkeleyDB_LIB}
                      ${OPENSSL_LIBRARIES}
                      ${MINIUPNP_LIBRARIES}
                      secp256k1
                      ${LIBEVENT_PTHREADS_LIB}
                      ${LIBEVENT_LIB}
                      ${BOTAN_LIBRARY}
                      ${APPLE_FRAMEWORKS}
                      ${ZeroMQ_LIBRARY}
                      Qt5::Widgets
                      Qt5::Core
                      Qt5::Network
                      Qt5::DBus
                      )

