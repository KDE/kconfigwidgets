
find_package(Qt5 5.2.0 CONFIG REQUIRED Test)

include(ECMAddTests)

ecm_add_test(kstandardactiontest.cpp LINK_LIBRARIES Qt5::Test KF5::ConfigWidgets)

set(kconfigdialog_unittest_SRCS kconfigdialog_unittest.cpp)
kconfig_add_kcfg_files(kconfigdialog_unittest_SRCS GENERATE_MOC signaltest.kcfgc)
ecm_add_test(${kconfigdialog_unittest_SRCS} TEST_NAME "kconfigdialog_unittest" LINK_LIBRARIES Qt5::Test KF5::ConfigWidgets)

