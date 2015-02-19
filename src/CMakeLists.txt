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
  kpastetextaction.cpp
  krecentfilesaction.cpp
  kstandardaction.cpp
  ktipdialog.cpp
  kcmoduleqml.cpp
)

add_library(KF5ConfigWidgets ${kconfigwidgets_SRCS})
generate_export_header(KF5ConfigWidgets BASE_NAME KConfigWidgets)
add_library(KF5::ConfigWidgets ALIAS KF5ConfigWidgets)

target_include_directories(KF5ConfigWidgets INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/KConfigWidgets>")

target_link_libraries(KF5ConfigWidgets
  PUBLIC
    KF5::Codecs # KCodecActions uses KCharsets, KEncodingProber
    KF5::WidgetsAddons # For K*Action, KPage*, KViewStateSerializer, KAcceleratorManager, K*GuiItem
    KF5::ConfigGui # KStandardAction uses KStandardShortcut
    KF5::Auth # KCModule uses KAuth::Action
  PRIVATE
    Qt5::DBus # KPasteTextAction talks to Klipper via DBus
    KF5::CoreAddons # KCModule uses KAboutData, KTipDialog uses KRandom
    KF5::GuiAddons # KColorScheme uses KColorUtils
    KF5::I18n # For action and widget texts
    KF5::Package
    KF5::Declarative
    Qt5::Qml
    Qt5::Quick
)

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
  KCModuleQml
  KHelpClient
  KLanguageButton
  KPasteTextAction
  KRecentFilesAction
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
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/KConfigWidgets COMPONENT Devel
)

install(FILES
 ktip-bulb.png
 DESTINATION  ${KDE_INSTALL_DATADIR_KF5}/kconfigwidgets/pics
)

install(PROGRAMS preparetips5 DESTINATION ${KDE_INSTALL_BINDIR} )
install( FILES entry.desktop  DESTINATION  ${KDE_INSTALL_LOCALEDIR}/en_US RENAME kf5_entry.desktop )

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME KConfigWidgets LIB_NAME KF5ConfigWidgets DEPS "KCodecs KWidgetsAddons KConfigGui KAuth" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/KConfigWidgets)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
