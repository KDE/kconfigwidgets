set(kconfigwidgets_SRCS
  kcodecaction.cpp
  kcolorscheme.cpp
  kcolorschememanager.cpp
  kconfigdialog.cpp
  kconfigviewstatesaver.cpp
  kconfigdialogmanager.cpp
  kcmodule.cpp
  khelpclient.cpp
  klanguagebutton.cpp
  klanguagename.cpp
  krecentfilesaction.cpp
  kstandardaction.cpp
  ktipdialog.cpp
)
ecm_qt_declare_logging_category(kconfigwidgets_SRCS
    HEADER kconfigwidgets_debug.h
    IDENTIFIER KCONFIG_WIDGETS_LOG
    CATEGORY_NAME kf.configwidgets
    OLD_CATEGORY_NAMES kf5.kconfigwidgets
    DESCRIPTION "KConfigWidgets"
    EXPORT KCONFIGWIDGETS
)
if (TARGET Qt5::DBus)
  list(APPEND kconfigwidgets_SRCS kpastetextaction.cpp)
endif()

qt5_add_resources(kconfigwidgets_SRCS kconfigwidgets.qrc)

add_library(KF5ConfigWidgets ${kconfigwidgets_SRCS})
add_library(KF5::ConfigWidgets ALIAS KF5ConfigWidgets)
ecm_generate_export_header(KF5ConfigWidgets
    BASE_NAME KConfigWidgets
    GROUP_BASE_NAME KF
    VERSION ${KF_VERSION}
    DEPRECATED_BASE_VERSION 0
    DEPRECATION_VERSIONS 4.0 5.0 5.23 5.32 5.38 5.39 5.64 5.78
    EXCLUDE_DEPRECATED_BEFORE_AND_AT ${EXCLUDE_DEPRECATED_BEFORE_AND_AT}
)

target_include_directories(KF5ConfigWidgets INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/KConfigWidgets>")

target_link_libraries(KF5ConfigWidgets
  PUBLIC
    KF5::Codecs # KCodecActions uses KCharsets, KEncodingProber
    KF5::WidgetsAddons # For K*Action, KPage*, KViewStateSerializer, KAcceleratorManager, K*GuiItem
    KF5::ConfigGui # KStandardAction uses KStandardShortcut
  PRIVATE
    KF5::CoreAddons # KCModule uses KAboutData
    KF5::GuiAddons # KColorScheme uses KColorUtils
    KF5::I18n # For action and widget texts
)
if (TARGET KF5::Auth)
  target_link_libraries(KF5ConfigWidgets PUBLIC KF5::Auth) # KCModule uses KAuth::Action TODO KF6 Change to KF5::AuthCore
else()
  target_compile_definitions(KF5ConfigWidgets PUBLIC -DKCONFIGWIDGETS_NO_KAUTH)
endif()
if (TARGET Qt5::DBus)
  target_link_libraries(KF5ConfigWidgets PRIVATE Qt5::DBus) # KPasteTextAction talks to Klipper via DBus
endif()

set_target_properties(KF5ConfigWidgets PROPERTIES VERSION   ${KCONFIGWIDGETS_VERSION_STRING}
                                                  SOVERSION ${KCONFIGWIDGETS_SOVERSION}
                                                  EXPORT_NAME ConfigWidgets
)

ecm_generate_headers(KConfigWidgets_HEADERS
  HEADER_NAMES
  KCodecAction
  KColorScheme
  KColorSchemeManager
  KConfigDialog
  KConfigViewStateSaver
  KConfigDialogManager
  KCModule
  KHelpClient
  KLanguageButton
  KLanguageName
  KPasteTextAction
  KRecentFilesAction
  KViewStateMaintainer
  KStandardAction
  KTipDialog

  REQUIRED_HEADERS KConfigWidgets_HEADERS
)

find_package(PythonModuleGeneration)

if (PythonModuleGeneration_FOUND)
  ecm_generate_python_binding(
    TARGET KF5::ConfigWidgets
    PYTHONNAMESPACE PyKF5
    MODULENAME KConfigWidgets
    SIP_INCLUDES
    RULES_FILE "${CMAKE_SOURCE_DIR}/cmake/rules_PyKF5.py"
    SIP_DEPENDS
      QtCore/QtCoremod.sip
      PyKF5/KCodecs/KCodecsmod.sip
      PyKF5/KWidgetsAddons/KWidgetsAddonsmod.sip
      PyKF5/KConfigGui/KConfigGuimod.sip
      PyKF5/KAuth/KAuthmod.sip
    HEADERS
      kcodecaction.h
      kcolorscheme.h
      kcolorschememanager.h
      kconfigdialog.h
      kconfigviewstatesaver.h
      kconfigdialogmanager.h
      kcmodule.h
      khelpclient.h
      klanguagebutton.h
      kpastetextaction.h
      krecentfilesaction.h
      kviewstatemaintainer.h
      kstandardaction.h
      ktipdialog.h
  )
endif()

install(TARGETS KF5ConfigWidgets EXPORT KF5ConfigWidgetsTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/kconfigwidgets_export.h
  ${KConfigWidgets_HEADERS}
  ktip.h
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/KConfigWidgets COMPONENT Devel
)

install(PROGRAMS preparetips5 DESTINATION ${KDE_INSTALL_BINDIR} )
install( FILES entry.desktop  DESTINATION  ${KDE_INSTALL_LOCALEDIR}/en_US RENAME kf5_entry.desktop )

ecm_qt_install_logging_categories(
    EXPORT KCONFIGWIDGETS
    FILE kconfigwidgets.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)

if(BUILD_DESIGNERPLUGIN)
    add_subdirectory(designer)
endif()

if(BUILD_QCH)
    ecm_add_qch(
        KF5ConfigWidgets_QCH
        NAME KConfigWidgets
        BASE_NAME KF5ConfigWidgets
        VERSION ${KF_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
            ${KConfigWidgets_HEADERS}
        MD_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md"
        IMAGE_DIRS "${CMAKE_SOURCE_DIR}/docs/pics"
        LINK_QCHS
            KF5Codecs_QCH
            KF5WidgetsAddons_QCH
            KF5Config_QCH
            KF5Auth_QCH
        INCLUDE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
            KCONFIGWIDGETS_EXPORT
            KCONFIGWIDGETS_DEPRECATED
            KCONFIGWIDGETS_DEPRECATED_EXPORT
            "KCONFIGWIDGETS_DEPRECATED_VERSION(x, y, t)"
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
endif()

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME KConfigWidgets LIB_NAME KF5ConfigWidgets DEPS "KCodecs KWidgetsAddons KConfigGui KAuth" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/KConfigWidgets)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
