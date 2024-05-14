# SPDX-FileCopyrightText: g10 Code GmbH
# SPDX-Contributor: 2024 Carl Schwan <carl.schwan@gnupg.com>
# SPDX-License-Identifier: BSD-2-Clause

add_library(MessageBoxStorageIntegrationPlugin
    MODULE kmessageboxdontaskagainstorageplugin.cpp)

target_link_libraries(MessageBoxStorageIntegrationPlugin
    PRIVATE
        KF6::WidgetsAddons
        KF6::ConfigCore
)

install(TARGETS MessageBoxStorageIntegrationPlugin
        DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf6)
