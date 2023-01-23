add_library(KF6ConfigWidgets)
add_library(KF6::ConfigWidgets ALIAS KF6ConfigWidgets)

target_sources(KF6ConfigWidgets PRIVATE
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

  kcodecaction.h
  kcolorscheme.h
  kcolorschememanager.h
  kcolorschememodel.h
  kcommandbar.h
  kcommandbarmodel_p.h
  kconfigdialog.h
  kconfigviewstatesaver.h
  kconfigdialogmanager.h
  kcmodule.h
  khelpclient.h
  khamburgermenu.h
  khamburgermenuhelpers_p.h
  klanguagebutton.h
  klanguagename.h
  krecentfilesaction.h
  kstandardaction.h
  kstatefulbrush.h
)

ecm_qt_declare_logging_category(KF6ConfigWidgets
    HEADER kconfigwidgets_debug.h
    IDENTIFIER KCONFIG_WIDGETS_LOG
    CATEGORY_NAME kf.configwidgets
    OLD_CATEGORY_NAMES kf5.kconfigwidgets
    DESCRIPTION "KConfigWidgets"
    EXPORT KCONFIGWIDGETS
)

if (WITH_KAUTH)
    set(WITH_KAUTH_DEFINE_VALUE 1)
else()
    set(WITH_KAUTH_DEFINE_VALUE 0)
endif()
set(define_with_kauth_code "#define KCONFIGWIDGETS_WITH_KAUTH ${WITH_KAUTH_DEFINE_VALUE}\n")

ecm_generate_export_header(KF6ConfigWidgets
    BASE_NAME KConfigWidgets
    GROUP_BASE_NAME KF
    VERSION ${KF_VERSION}
    DEPRECATED_BASE_VERSION 0
    DEPRECATION_VERSIONS
    EXCLUDE_DEPRECATED_BEFORE_AND_AT ${EXCLUDE_DEPRECATED_BEFORE_AND_AT}
    CUSTOM_CONTENT_FROM_VARIABLE define_with_kauth_code
)

target_include_directories(KF6ConfigWidgets INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF}/KConfigWidgets>")

target_link_libraries(KF6ConfigWidgets
  PUBLIC
    KF6::WidgetsAddons # For K*Action, KPage*, KViewStateSerializer, KAcceleratorManager, K*GuiItem
    KF6::ConfigGui # KStandardAction uses KStandardShortcut
  PRIVATE
    KF6::CoreAddons # KCModule uses KAboutData
    KF6::GuiAddons # KColorScheme uses KColorUtils
    KF6::I18n # For action and widget texts
)

target_link_libraries(KF6ConfigWidgets
  PRIVATE
  KF6::Codecs # KCodecActions uses KCharsets, KEncodingProber
)

if (WITH_KAUTH)
  if (QT_MAJOR_VERSION STREQUAL "6")
    target_link_libraries(KF6ConfigWidgets PUBLIC KF6::AuthCore)
  else()
    target_link_libraries(KF6ConfigWidgets PUBLIC KF6::Auth)
  endif()
endif()

if (TARGET Qt6::Core5Compat)
    target_link_libraries(KF6ConfigWidgets PUBLIC Qt6::Core5Compat)
endif()

set_target_properties(KF6ConfigWidgets PROPERTIES VERSION   ${KCONFIGWIDGETS_VERSION}
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
  KRecentFilesAction
  KStatefulBrush
  KViewStateMaintainer
  KStandardAction

  REQUIRED_HEADERS KConfigWidgets_HEADERS
)

install(TARGETS KF6ConfigWidgets EXPORT KF6ConfigWidgetsTargets ${KF_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/kconfigwidgets_export.h
  ${KConfigWidgets_HEADERS}
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
        KF6ConfigWidgets_QCH
        NAME KConfigWidgets
        BASE_NAME KF6ConfigWidgets
        VERSION ${KF_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
            ${KConfigWidgets_HEADERS}
        MD_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md"
        IMAGE_DIRS "${CMAKE_SOURCE_DIR}/docs/pics"
        LINK_QCHS
            KF6Codecs_QCH
            KF6WidgetsAddons_QCH
            KF6Config_QCH
            KF6Auth_QCH
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
ecm_generate_pri_file(BASE_NAME KConfigWidgets LIB_NAME KF6ConfigWidgets DEPS "KCodecs KWidgetsAddons KConfigGui KAuth" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF}/KConfigWidgets)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
