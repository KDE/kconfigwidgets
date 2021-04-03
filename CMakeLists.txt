cmake_minimum_required(VERSION 3.5)

set(KF_VERSION "5.81.0") # handled by release scripts
set(KF_DEP_VERSION "5.81.0") # handled by release scripts
project(KConfigWidgets VERSION ${KF_VERSION})

include(FeatureSummary)
find_package(ECM 5.81.0  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://commits.kde.org/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

set(REQUIRED_QT_VERSION 5.14.0)
find_package(Qt5 "${REQUIRED_QT_VERSION}" CONFIG REQUIRED Widgets)
if (NOT ANDROID)
  find_package(Qt5 "${REQUIRED_QT_VERSION}" CONFIG REQUIRED DBus)
endif()
include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(KDEGitCommitHooks)

include(ECMGenerateExportHeader)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMAddQch)
include(ECMQtDeclareLoggingCategory)

set(EXCLUDE_DEPRECATED_BEFORE_AND_AT 0 CACHE STRING "Control the range of deprecated API excluded from the build [default=0].")

option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")

option(BUILD_DESIGNERPLUGIN "Build plugin for Qt Designer" ON)
add_feature_info(DESIGNERPLUGIN ${BUILD_DESIGNERPLUGIN} "Build plugin for Qt Designer")

ecm_setup_version(PROJECT VARIABLE_PREFIX KCONFIGWIDGETS
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kconfigwidgets_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5ConfigWidgetsConfigVersion.cmake"
                        SOVERSION 5)

if (NOT ANDROID)
  find_package(KF5Auth ${KF_DEP_VERSION} REQUIRED)
endif()
find_package(KF5CoreAddons ${KF_DEP_VERSION} REQUIRED)
find_package(KF5Codecs ${KF_DEP_VERSION} REQUIRED)
find_package(KF5Config ${KF_DEP_VERSION} REQUIRED)
find_package(KF5DocTools ${KF_DEP_VERSION})
find_package(KF5GuiAddons ${KF_DEP_VERSION} REQUIRED)
find_package(KF5I18n ${KF_DEP_VERSION} REQUIRED)
find_package(KF5WidgetsAddons ${KF_DEP_VERSION} REQUIRED)

remove_definitions(-DQT_NO_CAST_FROM_ASCII)
remove_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)
add_definitions(-DQT_NO_FOREACH)
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050e00)
add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x055000)
add_definitions(-DKWIDGETSADDONS_DISABLE_DEPRECATED_BEFORE_AND_AT=0x054D00) # We need KSelectAction::triggered(QString)

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

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
# Remove these 3 lines when the ECM version is bumped to 5.80
include(KDEClangFormat)
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
