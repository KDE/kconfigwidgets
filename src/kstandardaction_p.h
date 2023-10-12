/*
    This file is part of the KDE libraries
    SPDX-FileCopyrightText: 1999, 2000 Kurt Granroth <granroth@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KSTANDARDACTION_PRIVATE_H
#define KSTANDARDACTION_PRIVATE_H

#include <QAction>
#include <QApplication>

#include <KLazyLocalizedString>
#include <KLocalizedString>
#include <KStandardShortcut>

#include <string>

namespace KStandardAction
{

// Helper class for storing raw data in static tables which can be used for QString instance
// creation at runtime without copying/converting to new memalloc'ed memory, as well as avoiding
// that way storing the strings directly as QStrings resulting in non-constexpr init code on
// library loading
struct RawStringData {
    template<std::size_t StringSize>
    constexpr inline RawStringData(const char16_t (&_data)[StringSize])
        : data(_data)
        , size(std::char_traits<char16_t>::length(_data))
    {
    }
    constexpr inline RawStringData(std::nullptr_t)
    {
    }
    constexpr inline RawStringData() = default;

    inline QString toString() const
    {
        if (!data) {
            return QString();
        }

        return Qt::Literals::StringLiterals::operator""_s(data, size);
    }

private:
    const char16_t *const data = nullptr;
    const std::size_t size = 0;
};

struct KStandardActionInfo {
    StandardAction id;
    KStandardShortcut::StandardShortcut idAccel;
    const char *psName;
    const KLazyLocalizedString psLabel;
    const KLazyLocalizedString psToolTip;
    const RawStringData psIconName;
};
// clang-format off
static const KStandardActionInfo g_rgActionInfo[] = {
    { New,           KStandardShortcut::New, "file_new", kli18n("&New"), kli18n("Create new document"), u"document-new" },
    { Open,          KStandardShortcut::Open, "file_open", kli18n("&Open..."), kli18n("Open an existing document"), u"document-open" },
    { OpenRecent,    KStandardShortcut::AccelNone, "file_open_recent", kli18n("Open &Recent"), kli18n("Open a document which was recently opened"), u"document-open-recent" },
    { Save,          KStandardShortcut::Save, "file_save", kli18n("&Save"), kli18n("Save document"), u"document-save" },
    { SaveAs,        KStandardShortcut::SaveAs, "file_save_as", kli18n("Save &As..."), kli18n("Save document under a new name"), u"document-save-as" },
    { Revert,        KStandardShortcut::Revert, "file_revert", kli18n("Re&vert"), kli18n("Revert unsaved changes made to document"), u"document-revert" },
    { Close,         KStandardShortcut::Close, "file_close", kli18n("&Close"), kli18n("Close document"), u"document-close" },
    { Print,         KStandardShortcut::Print, "file_print", kli18n("&Print..."), kli18n("Print document"), u"document-print" },
    { PrintPreview,  KStandardShortcut::PrintPreview, "file_print_preview", kli18n("Print Previe&w"), kli18n("Show a print preview of document"), u"document-print-preview" },
    { Mail,          KStandardShortcut::Mail, "file_mail", kli18n("&Mail..."), kli18n("Send document by mail"), u"mail-send" },
    { Quit,          KStandardShortcut::Quit, "file_quit", kli18n("&Quit"), kli18n("Quit application"), u"application-exit" },

    { Undo,          KStandardShortcut::Undo, "edit_undo", kli18n("&Undo"), kli18n("Undo last action"), u"edit-undo" },
    { Redo,          KStandardShortcut::Redo, "edit_redo", kli18n("Re&do"), kli18n("Redo last undone action"), u"edit-redo" },
    { Cut,           KStandardShortcut::Cut, "edit_cut", kli18n("Cu&t"), kli18n("Cut selection to clipboard"), u"edit-cut" },
    { Copy,          KStandardShortcut::Copy, "edit_copy", kli18n("&Copy"), kli18n("Copy selection to clipboard"), u"edit-copy" },
    { Paste,         KStandardShortcut::Paste, "edit_paste", kli18n("&Paste"), kli18n("Paste clipboard content"), u"edit-paste" },
    { Clear,         KStandardShortcut::Clear, "edit_clear", kli18n("C&lear"), {}, u"edit-clear" },
    { SelectAll,     KStandardShortcut::SelectAll, "edit_select_all", kli18n("Select &All"), {}, u"edit-select-all" },
    { Deselect,      KStandardShortcut::Deselect, "edit_deselect", kli18n("Dese&lect"), {}, u"edit-select-none" },
    { Find,          KStandardShortcut::Find, "edit_find", kli18n("&Find..."), {}, u"edit-find" },
    { FindNext,      KStandardShortcut::FindNext, "edit_find_next", kli18n("Find &Next"), {}, u"go-down-search" },
    { FindPrev,      KStandardShortcut::FindPrev, "edit_find_prev", kli18n("Find Pre&vious"), {}, u"go-up-search" },
    { Replace,       KStandardShortcut::Replace, "edit_replace", kli18n("&Replace..."), {}, u"edit-find-replace" },

    { ActualSize,    KStandardShortcut::ActualSize, "view_actual_size", kli18n("Zoom to &Actual Size"), kli18n("View document at its actual size"), u"zoom-original" },
    { FitToPage,     KStandardShortcut::FitToPage, "view_fit_to_page", kli18n("&Fit to Page"), kli18n("Zoom to fit page in window"), u"zoom-fit-page" },
    { FitToWidth,    KStandardShortcut::FitToWidth, "view_fit_to_width", kli18n("Fit to Page &Width"), kli18n("Zoom to fit page width in window"), u"zoom-fit-width" },
    { FitToHeight,   KStandardShortcut::FitToHeight, "view_fit_to_height", kli18n("Fit to Page &Height"), kli18n("Zoom to fit page height in window"), u"zoom-fit-height" },
    { ZoomIn,        KStandardShortcut::ZoomIn, "view_zoom_in", kli18n("Zoom &In"), {}, u"zoom-in" },
    { ZoomOut,       KStandardShortcut::ZoomOut, "view_zoom_out", kli18n("Zoom &Out"), {}, u"zoom-out" },
    { Zoom,          KStandardShortcut::Zoom, "view_zoom", kli18n("&Zoom..."), kli18n("Select zoom level"), u"zoom" },
    { Redisplay,     KStandardShortcut::Reload, "view_redisplay", kli18n("&Refresh"), kli18n("Refresh document"), u"view-refresh" },

    { Up,            KStandardShortcut::Up, "go_up", kli18n("&Up"), kli18n("Go up"), u"go-up" },
    // The following three have special i18n() needs for sLabel
    { Back,          KStandardShortcut::Back, "go_back", {}, {}, u"go-previous" },
    { Forward,       KStandardShortcut::Forward, "go_forward", {}, {}, u"go-next" },
    { Home,          KStandardShortcut::Home, "go_home", {}, {}, u"go-home" },
    { Prior,         KStandardShortcut::Prior, "go_previous", kli18n("&Previous Page"), kli18n("Go to previous page"), u"go-previous-view-page" },
    { Next,          KStandardShortcut::Next, "go_next", kli18n("&Next Page"), kli18n("Go to next page"), u"go-next-view-page" },
    { Goto,          KStandardShortcut::Goto, "go_goto", kli18n("&Go To..."), {}, {} },
    { GotoPage,      KStandardShortcut::GotoPage, "go_goto_page", kli18n("&Go to Page..."), {}, u"go-jump" },
    { GotoLine,      KStandardShortcut::GotoLine, "go_goto_line", kli18n("&Go to Line..."), {}, {} },
    { FirstPage,     KStandardShortcut::Begin, "go_first", kli18n("&First Page"), kli18n("Go to first page"), u"go-first-view-page" },
    { LastPage,      KStandardShortcut::End, "go_last", kli18n("&Last Page"), kli18n("Go to last page"), u"go-last-view-page" },
    { DocumentBack,  KStandardShortcut::DocumentBack, "go_document_back", kli18n("&Back"), kli18n("Go back in document"), u"go-previous" },
    { DocumentForward, KStandardShortcut::DocumentForward, "go_document_forward", kli18n("&Forward"), kli18n("Go forward in document"), u"go-next" },

    { AddBookmark,   KStandardShortcut::AddBookmark, "bookmark_add", kli18n("&Add Bookmark"), {}, u"bookmark-new" },
    { EditBookmarks, KStandardShortcut::EditBookmarks, "bookmark_edit", kli18n("&Edit Bookmarks..."), {}, u"bookmarks-organize" },

    { Spelling,      KStandardShortcut::Spelling, "tools_spelling", kli18n("&Spelling..."), kli18n("Check spelling in document"), u"tools-check-spelling" },

    { ShowMenubar,   KStandardShortcut::ShowMenubar, "options_show_menubar", kli18n("Show &Menubar"), kli18n("Show or hide menubar"), u"show-menu" },
    { ShowToolbar,   KStandardShortcut::ShowToolbar, "options_show_toolbar", kli18n("Show &Toolbar"), kli18n("Show or hide toolbar"), {} },
    { ShowStatusbar, KStandardShortcut::ShowStatusbar, "options_show_statusbar", kli18n("Show St&atusbar"), kli18n("Show or hide statusbar"), {} },
    { FullScreen,    KStandardShortcut::FullScreen, "fullscreen", kli18n("F&ull Screen Mode"), {}, u"view-fullscreen" },
    { KeyBindings,   KStandardShortcut::KeyBindings, "options_configure_keybinding", kli18n("Configure Keyboard S&hortcuts..."), {}, u"configure-shortcuts" },
    { Preferences,   KStandardShortcut::Preferences, "options_configure", kli18n("&Configure %1..."), {}, u"configure" },
    { ConfigureToolbars, KStandardShortcut::ConfigureToolbars, "options_configure_toolbars", kli18n("Configure Tool&bars..."), {}, u"configure-toolbars" },
    { ConfigureNotifications, KStandardShortcut::ConfigureNotifications, "options_configure_notifications", kli18n("Configure &Notifications..."), {}, u"preferences-desktop-notification" },

    // the idea here is that Contents is used in menus, and Help in dialogs, so both share the same
    // shortcut
    { HelpContents,  KStandardShortcut::Help, "help_contents", kli18n("%1 &Handbook"), {}, u"help-contents" },
    { WhatsThis,     KStandardShortcut::WhatsThis, "help_whats_this", kli18n("What's &This?"), {}, u"help-contextual" },
    { TipofDay,      KStandardShortcut::TipofDay, "help_show_tip", kli18n("Tip of the &Day"), {}, u"help-hint" },
    { ReportBug,     KStandardShortcut::ReportBug, "help_report_bug", kli18n("&Report Bug..."), {}, u"tools-report-bug" },
    { SwitchApplicationLanguage, KStandardShortcut::SwitchApplicationLanguage, "switch_application_language", kli18n("Configure &Language..."), {}, u"preferences-desktop-locale" },
    { AboutApp,      KStandardShortcut::AccelNone, "help_about_app", kli18n("&About %1"), {}, nullptr },
    { AboutKDE,      KStandardShortcut::AccelNone, "help_about_kde", kli18n("About &KDE"), {}, u"kde" },
    { DeleteFile,    KStandardShortcut::DeleteFile, "deletefile", kli18n("&Delete"), {}, u"edit-delete" },
    { RenameFile,    KStandardShortcut::RenameFile, "renamefile", kli18n("&Rename..."), {}, u"edit-rename" },
    { MoveToTrash,   KStandardShortcut::MoveToTrash, "movetotrash", kli18n("&Move to Trash"), {}, u"trash-empty" },
    { Donate,        KStandardShortcut::Donate, "help_donate", kli18n("&Donate"), {}, u"help-donate"},
    { HamburgerMenu, KStandardShortcut::OpenMainMenu, "hamburger_menu", kli18n("Open &Menu"), {}, u"application-menu" },
    { ActionNone,    KStandardShortcut::AccelNone, nullptr, {}, {}, nullptr }
};
// clang-format on

inline const KStandardActionInfo *infoPtr(StandardAction id)
{
    for (uint i = 0; g_rgActionInfo[i].id != ActionNone; i++) {
        if (g_rgActionInfo[i].id == id) {
            return &g_rgActionInfo[i];
        }
    }

    return nullptr;
}

static inline QStringList internal_stdNames()
{
    QStringList result;

    for (uint i = 0; g_rgActionInfo[i].id != ActionNone; i++)
        if (!g_rgActionInfo[i].psLabel.isEmpty()) {
            if (QByteArray(g_rgActionInfo[i].psLabel.untranslatedText()).contains("%1"))
            // Prevents KLocalizedString::toString() from complaining about unsubstituted placeholder.
            {
                result.append(g_rgActionInfo[i].psLabel.subs(QString()).toString());
            } else {
                result.append(g_rgActionInfo[i].psLabel.toString());
            }
        }

    return result;
}

class AutomaticAction : public QAction
{
    Q_OBJECT

public:
    AutomaticAction(const QIcon &icon, const QString &text, KStandardShortcut::StandardShortcut standardShortcut, const char *slot, QObject *parent);
public Q_SLOTS:
    inline void cut()
    {
        invokeEditSlot("cut");
    }
    inline void copy()
    {
        invokeEditSlot("copy");
    }
    inline void paste()
    {
        invokeEditSlot("paste");
    }
    inline void clear()
    {
        invokeEditSlot("clear");
    }
    inline void selectAll()
    {
        invokeEditSlot("selectAll");
    }

    void invokeEditSlot(const char *slot)
    {
        if (qApp->focusWidget()) {
            QMetaObject::invokeMethod(qApp->focusWidget(), slot);
        }
    }
};

}

#endif
