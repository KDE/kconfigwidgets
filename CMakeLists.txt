cmake_minimum_required(VERSION 2.8.12)

project(KConfigWidgets)

find_package(ECM 0.0.12 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

set(REQUIRED_QT_VERSION "5.2.0")
find_package(Qt5 "${REQUIRED_QT_VERSION}" CONFIG REQUIRED Widgets DBus)
include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings)
include(KDECMakeSettings)
include(FeatureSummary)
include(GenerateExportHeader)
include(ECMSetupVersion)
include(ECMGenerateHeaders)

set(KF5_VERSION "4.98.0") # handled by release scripts # handled by release scripts
set(KF5_DEP_VERSION "4.98.0") # handled by release scripts

ecm_setup_version(${KF5_VERSION} VARIABLE_PREFIX KCONFIGWIDGETS
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kconfigwidgets_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5ConfigWidgetsConfigVersion.cmake"
                        SOVERSION 5)

if(NOT kdelibs_SOURCE_DIR)
  find_package(KF5Auth ${KF5_DEP_VERSION} REQUIRED)
  find_package(KF5CoreAddons ${KF5_DEP_VERSION} REQUIRED)
  find_package(KF5Codecs ${KF5_DEP_VERSION} REQUIRED)
  find_package(KF5Config ${KF5_DEP_VERSION} REQUIRED)
  find_package(KF5DocTools ${KF5_DEP_VERSION} REQUIRED)
  find_package(KF5GuiAddons ${KF5_DEP_VERSION} REQUIRED)
  find_package(KF5I18n ${KF5_DEP_VERSION} REQUIRED)
  find_package(KF5WidgetsAddons ${KF5_DEP_VERSION} REQUIRED)
else()
    set(_kdeBootStrapping TRUE)
    include(${CMAKE_SOURCE_DIR}/tier2/kdoctools/KF5DocToolsMacros.cmake)
    set(KDOCTOOLS_CUSTOMIZATION_DIR "${KDocTools_BINARY_DIR}/src/customization/")
endif()

remove_definitions(-DQT_NO_CAST_FROM_ASCII)
remove_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)

add_definitions(-DTRANSLATION_DOMAIN=\"kconfigwidgets5\")
add_subdirectory(docs)
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(autotests)

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KF5ConfigWidgets")

include(ECMPackageConfigHelpers)

ecm_configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/KF5ConfigWidgetsConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/KF5ConfigWidgetsConfig.cmake"
  INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/KF5ConfigWidgetsConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/KF5ConfigWidgetsConfigVersion.cmake"
  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
  COMPONENT Devel
)

install(EXPORT KF5ConfigWidgetsTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KF5ConfigWidgetsTargets.cmake NAMESPACE KF5:: )

install(FILES
   ${CMAKE_CURRENT_BINARY_DIR}/kconfigwidgets_version.h
  DESTINATION ${KF5_INCLUDE_INSTALL_DIR} COMPONENT Devel
)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
