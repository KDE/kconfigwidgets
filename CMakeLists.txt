cmake_minimum_required(VERSION 3.5)

set(KF5_VERSION "5.60.0") # handled by release scripts
set(KF5_DEP_VERSION "5.59.0") # handled by release scripts
project(KConfigWidgets VERSION ${KF5_VERSION})

include(FeatureSummary)
find_package(ECM 5.59.0  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://projects.kde.org/projects/kdesupport/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

set(REQUIRED_QT_VERSION 5.11.0)
find_package(Qt5 "${REQUIRED_QT_VERSION}" CONFIG REQUIRED Widgets)
if (NOT ANDROID)
  find_package(Qt5 "${REQUIRED_QT_VERSION}" CONFIG REQUIRED DBus)
endif()
include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)

include(GenerateExportHeader)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMAddQch)
include(ECMQtDeclareLoggingCategory)

option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")

ecm_setup_version(PROJECT VARIABLE_PREFIX KCONFIGWIDGETS
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kconfigwidgets_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5ConfigWidgetsConfigVersion.cmake"
                        SOVERSION 5)

if (NOT ANDROID)
  find_package(KF5Auth ${KF5_DEP_VERSION} REQUIRED)
endif()
find_package(KF5CoreAddons ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5Codecs ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5Config ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5DocTools ${KF5_DEP_VERSION})
find_package(KF5GuiAddons ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5I18n ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5WidgetsAddons ${KF5_DEP_VERSION} REQUIRED)

remove_definitions(-DQT_NO_CAST_FROM_ASCII)
remove_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)
add_definitions(-DQT_NO_FOREACH)

add_definitions(-DTRANSLATION_DOMAIN=\"kconfigwidgets5\")
if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")
    ki18n_install(po)
    if (KF5DocTools_FOUND)
        kdoctools_install(po)
    endif()

    file(GLOB lang_dirs "po/*")
    foreach(lang_dir ${lang_dirs})
       get_filename_component(lang ${lang_dir} NAME)
       if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/po/${lang}/kf5_entry.desktop")
          install( FILES po/${lang}/kf5_entry.desktop DESTINATION ${KDE_INSTALL_LOCALEDIR}/${lang} )
       endif()
    endforeach()
endif()

if (KF5DocTools_FOUND)
    add_subdirectory(docs)
endif()

add_subdirectory(src)
if (BUILD_TESTING)
    add_subdirectory(tests)
    add_subdirectory(autotests)
endif()

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5ConfigWidgets")

if (BUILD_QCH)
    ecm_install_qch_export(
        TARGETS KF5ConfigWidgets_QCH
        FILE KF5ConfigWidgetsQchTargets.cmake
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KF5ConfigWidgetsQchTargets.cmake\")")
endif()

include(CMakePackageConfigHelpers)

set(HAVE_KAUTH FALSE)
if (TARGET KF5::Auth)
  set(HAVE_KAUTH TRUE)
endif()
configure_package_config_file(
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
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5} COMPONENT Devel
)

# contains list of debug categories, for kdebugsettings
install(FILES kconfigwidgets.categories  DESTINATION  ${KDE_INSTALL_LOGGINGCATEGORIESDIR})

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
