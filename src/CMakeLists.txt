add_library(KF5ConfigWidgets)
add_library(KF5::ConfigWidgets ALIAS KF5ConfigWidgets)

target_sources(KF5ConfigWidgets PRIVATE
  kcodecaction.cpp
  kcolorscheme.cpp
  kcolorschememanager.cpp
  kcolorschememodel.cpp
  kcommandbar.cpp
  kcommandbarmodel_p.cpp
  kconfigdialog.cpp
  kconfigviewstatesaver.cpp
  kconfigdialogmanager.cpp
  kcmodule.cpp
  khelpclient.cpp
  khamburgermenu.cpp
  khamburgermenuhelpers.cpp
  klanguagebutton.cpp
  klanguagename.cpp
  krecentfilesaction.cpp
  kstandardaction.cpp
  kstatefulbrush.cpp
  ktipdialog.cpp

  kconfigwidgets.qrc
)

if (WIN32)
  target_sources(KF5ConfigWidgets PRIVATE
    windowsmessagesnotifier.cpp
  )
endif()

ecm_qt_declare_logging_category(KF5ConfigWidgets
    HEADER kconfigwidgets_debug.h
    IDENTIFIER KCONFIG_WIDGETS_LOG
    CATEGORY_NAME kf.configwidgets
    OLD_CATEGORY_NAMES kf5.kconfigwidgets
    DESCRIPTION "KConfigWidgets"
    EXPORT KCONFIGWIDGETS
)
if (TARGET Qt${QT_MAJOR_VERSION}::DBus)
    target_sources(KF5ConfigWidgets PRIVATE kpastetextaction.cpp)
endif()

ecm_generate_export_header(KF5ConfigWidgets
    BASE_NAME KConfigWidgets
    GROUP_BASE_NAME KF
    VERSION ${KF_VERSION}
    DEPRECATED_BASE_VERSION 0
    DEPRECATION_VERSIONS 4.0 5.0 5.23 5.32 5.38 5.39 5.64 5.78 5.80 5.82 5.83 5.84 5.85 5.90 5.93
    EXCLUDE_DEPRECATED_BEFORE_AND_AT ${EXCLUDE_DEPRECATED_BEFORE_AND_AT}
)

target_include_directories(KF5ConfigWidgets INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF}/KConfigWidgets>")

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

if (WIN32)
  target_link_libraries(KF5ConfigWidgets PRIVATE advapi32)
endif()

if (WITH_KAUTH)
  if (QT_MAJOR_VERSION STREQUAL "6")
    target_link_libraries(KF5ConfigWidgets PUBLIC KF5::AuthCore)
  else()
    target_link_libraries(KF5ConfigWidgets PUBLIC KF5::Auth)
  endif()
else()
  target_compile_definitions(KF5ConfigWidgets PUBLIC -DKCONFIGWIDGETS_NO_KAUTH)
endif()
if (TARGET Qt${QT_MAJOR_VERSION}::DBus)
  target_link_libraries(KF5ConfigWidgets PRIVATE Qt${QT_MAJOR_VERSION}::DBus) # KPasteTextAction talks to Klipper via DBus
endif()

if (TARGET Qt6::Core5Compat)
    target_link_libraries(KF5ConfigWidgets PUBLIC Qt6::Core5Compat)
endif()

set_target_properties(KF5ConfigWidgets PROPERTIES VERSION   ${KCONFIGWIDGETS_VERSION}
                                                  SOVERSION ${KCONFIGWIDGETS_SOVERSION}
                                                  EXPORT_NAME ConfigWidgets
)

ecm_generate_headers(KConfigWidgets_HEADERS
  HEADER_NAMES
  KCodecAction
  KColorScheme
  KColorSchemeManager
  KColorSchemeModel
  KCommandBar
  KConfigDialog
  KConfigViewStateSaver
  KConfigDialogManager
  KCModule
  KHamburgerMenu
  KHelpClient
  KLanguageButton
  KLanguageName
  KPasteTextAction
  KRecentFilesAction
  KStatefulBrush
  KViewStateMaintainer
  KStandardAction
  KTipDialog

  REQUIRED_HEADERS KConfigWidgets_HEADERS
)

install(TARGETS KF5ConfigWidgets EXPORT KF5ConfigWidgetsTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/kconfigwidgets_export.h
  ${KConfigWidgets_HEADERS}
  ktip.h
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/KConfigWidgets COMPONENT Devel
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
            "KCONFIGWIDGETS_DEPRECATED_VERSION_BELATED(x, y, xt, yt, t)"
            "KCONFIGWIDGETS_ENUMERATOR_DEPRECATED_VERSION(x, y, t)"
            "KCONFIGWIDGETS_ENUMERATOR_DEPRECATED_VERSION_BELATED(x, y, xt, yt, t)"
        PREDEFINED_MACROS
            "KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(x, y)=1"
            "KCONFIGWIDGETS_BUILD_DEPRECATED_SINCE(x, y)=1"
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
endif()

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME KConfigWidgets LIB_NAME KF5ConfigWidgets DEPS "KCodecs KWidgetsAddons KConfigGui KAuth" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF}/KConfigWidgets)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
