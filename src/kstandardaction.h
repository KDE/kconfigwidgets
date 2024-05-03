/*
    This file is part of the KDE libraries
    SPDX-FileCopyrightText: 1999, 2000 Kurt Granroth <granroth@kde.org>
    SPDX-FileCopyrightText: 2001, 2002 Ellis Whitehead <ellis@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/
#ifndef KSTANDARDACTION_H
#define KSTANDARDACTION_H

#include <QAction>
#include <QList>
#include <QStringList>

#include <KStandardShortcut>
#include <KToggleAction>
#include <kconfigwidgets_export.h>
#include <khamburgermenu.h>
#include <krecentfilesaction.h>
#include <ktogglefullscreenaction.h>

#include <optional>
#include <type_traits>

class QObject;
class QWidget;
class QAction;
class KToggleFullScreenAction;

/**
 * Convenience methods to access all standard KDE actions.
 *
 * These actions should be used instead of hardcoding menubar and
 * toolbar items.  Using these actions helps your application easily
 * conform to the <a href="https://develop.kde.org/hig/">KDE Human Interface Guidelines</a>.
 *
 * All of the documentation for QAction holds for KStandardAction
 * also.  When in doubt on how things work, check the QAction
 * documentation first.
 * Please note that calling any of these methods automatically adds the action
 * to the actionCollection() of the QObject given by the 'parent' parameter.
 *
 * <b>Simple Example:</b>\n
 *
 * In general, using standard actions should be a drop in replacement
 * for regular actions. For example, if you previously had:
 * @code
 * QAction *newAct = new QAction(QIcon::fromTheme("document-new"),
 *                               i18n("&New"),
 *                               this);
 * newAct->setShortcut(KStandardShortcut::shortcut(KStandardShortcut::New).constFirst());
 * connect(newAct, &QAction::triggered, this, &ClassFoo::fileNew);
 * @endcode
 *
 * You can replace it with:
 * @code
 * QAction *newAct = KStandardAction::openNew(this, &ClassFoo::fileNew, this);
 * @endcode
 *
 * Alternatively you can instantiate the action using the StandardAction enums
 * provided.  This author can't think of a reason why you would want to, but, hey,
 * if you do, here's how:
 *
 * \code
 * QAction *newAct = KStandardAction::create(KStandardAction::New, this, &ClassFoo::fileNew, this);
 * \endcode
 *
 * <b>Relationship with KActionCollection from KXMLGui</b>\n
 *
 * If a KActionCollection is passed as the parent then the action will be
 * automatically added to that collection:
 * \code
 * QAction *cut = KStandardAction::cut(this, &ClassFoo::editCut, actionCollection());
 * \endcode
 *
 * Each action has a unique internal name which can be queried using the
 * name method.  For example KStandardAction::name(KStandardAction::Cut)
 * would return 'edit_cut'.  This name can be used as a unique identifier
 * for the actions.  So if you wanted to add an existing standard action
 * to an action collection you can do so like this:
 * \code
 * QAction *cut = KStandardAction::cut(this, &ClassFoo::editCut, this);
 * actionCollection()->addAction(KStandardAction::name(KStandardAction::Cut), cut);
 * \endcode
 *
 * You can then get a pointer to the action using
 * \code
 * QAction *cutPtr = actionCollection()->action(KStandardAction::name(KStandardAction::Cut));
 * \endcode
 *
 * @author Kurt Granroth <granroth@kde.org>
 */
namespace KStandardAction
{
/**
 * The standard menubar and toolbar actions.
 */
enum StandardAction {
    ActionNone,
    // File Menu
    New, ///< Create a new document or window.
    Open, ///< Open an existing file.
    OpenRecent, ///< Open a recently used document.
    Save, ///< Save the current document.
    SaveAs, ///< Save the current document under a different name.
    Revert, ///< Revert the current document to the last saved version.
    Close, ///< Close the current document.
    Print, ///< Print the current document.
    PrintPreview, ///< Show a print preview of the current document.
    Mail, ///< Send the current document by mail.
    Quit, ///< Quit the program.
    // Edit Menu
    Undo, ///< Undo the last operation.
    Redo, ///< Redo the last operation.
    Cut, ///< Cut selected area and store it in the clipboard.
    Copy, ///< Copy selected area and store it in the clipboard.
    Paste, ///< Paste the contents of clipboard at the current mouse or cursor.
    SelectAll, ///< Select all elements in the current document.
    Deselect, ///< Deselect any selected elements in the current document.
    Find, ///< Initiate a 'find' request in the current document.
    FindNext, ///< Find the next instance of a stored 'find'
    FindPrev, ///< Find a previous instance of a stored 'find'.
    Replace, ///< Find and replace matches.
    // View Menu
    ActualSize, ///< View the document at its actual size.
    FitToPage, ///< Fit the document view to the size of the current window.
    FitToWidth, ///< Fit the document view to the width of the current window.
    FitToHeight, ///< Fit the document view to the height of the current window.
    ZoomIn, ///< Zoom in the current document.
    ZoomOut, ///< Zoom out the current document.
    Zoom, ///< Select the current zoom level.
    Redisplay, ///< Redisplay or redraw the document.
    // Go Menu
    Up, ///< Move up (web style menu).
    Back, ///< Move back (web style menu).
    Forward, ///< Move forward (web style menu).
    Home, ///< Go to the "Home" position or document.
    Prior, ///< Scroll up one page.
    Next, ///< Scroll down one page.
    Goto, ///< Jump to some specific location in the document.
    GotoPage, ///< Go to a specific page.
    GotoLine, ///< Go to a specific line.
    FirstPage, ///< Jump to the first page.
    LastPage, ///< Jump to the last page.
    DocumentBack, ///< Move back (document style menu).
    DocumentForward, ///< Move forward (document style menu).
    // Bookmarks Menu
    AddBookmark, ///< Add the current page to the bookmarks tree.
    EditBookmarks, ///< Edit the application bookmarks.
    // Tools Menu
    Spelling, ///< Pop up the spell checker.
    // Settings Menu
    ShowMenubar, ///< Show/Hide the menubar.
    ShowToolbar, ///< Show/Hide the toolbar.
    ShowStatusbar, ///< Show/Hide the statusbar.
    KeyBindings, ///< Display the configure key bindings dialog.
    Preferences, ///< Display the preferences/options dialog.
    ConfigureToolbars, ///< Display the toolbar configuration dialog.
    // Help Menu
    HelpContents, ///< Display the handbook of the application.
    WhatsThis, ///< Trigger the What's This cursor.
    ReportBug, ///< Open up the Report Bug dialog.
    AboutApp, ///< Display the application's About box.
    AboutKDE, ///< Display the About KDE dialog.
    // Other standard actions
    ConfigureNotifications, ///< Display the notifications configuration dialog.
    FullScreen, ///< Switch to/from full screen mode.
    Clear, ///< Clear the content of the focus widget.
    SwitchApplicationLanguage, ///< Display the Switch Application Language dialog.
    DeleteFile, ///< Permanently deletes files or folders. @since 5.25
    RenameFile, ///< Renames files or folders. @since 5.25
    MoveToTrash, ///< Moves files or folders to the trash. @since 5.25
    Donate, ///< Open donation page on kde.org. @since 5.26
    HamburgerMenu ///< Opens a menu that substitutes the menubar. @since 5.81
};

/**
 * Creates an action corresponding to one of the
 * KStandardAction::StandardAction actions, which is connected to the given
 * object and @p slot, and is owned by @p parent.
 *
 * The signal that is connected to @p slot is triggered(bool), except for the case of
 * OpenRecent standard action, which uses the urlSelected(const QUrl &) signal of
 * KRecentFilesAction.
 *
 * @param id The StandardAction identifier to create a QAction for.
 * @param recvr The QObject to receive the signal, or @c nullptr if no notification
 *              is needed.
 * @param slot  The slot to connect the signal to (remember to use the SLOT() macro).
 * @param parent The QObject that should own the created QAction, or @c nullptr if no parent will
 *               own the QAction returned (ensure you delete it manually in this case).
 */
#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use overload with new connect syntax")
QAction *create(StandardAction id, const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * @internal
 */
KCONFIGWIDGETS_EXPORT QAction *_k_createInternal(StandardAction id, QObject *parent);

/**
 * This overloads create() to allow using the new connect syntax
 * @note if you use @c OpenRecent as @p id, you should manually connect to the urlSelected(const QUrl &)
 * signal of the returned KRecentFilesAction instead or use KStandardAction::openRecent(Receiver *, Func).
 *
 * If not explicitly specified, @p connectionType will be AutoConnection for all actions
 * except for ConfigureToolbars it will be QueuedConnection.
 *
 * @see create(StandardAction, const QObject *, const char *, QObject *)
 * @since 5.23 (The connectionType argument was added in 5.95)
 */
#ifdef K_DOXYGEN
inline QAction *create(StandardAction id, const QObject *recvr, Func slot, QObject *parent, std::optional<Qt::ConnectionType> connectionType = std::nullopt)
#else
template<class Receiver, class Func>
inline typename std::enable_if<!std::is_convertible<Func, const char *>::value, QAction>::type *
create(StandardAction id, const Receiver *recvr, Func slot, QObject *parent, std::optional<Qt::ConnectionType> connectionType = std::nullopt)
#endif
{
    QAction *action = _k_createInternal(id, parent);
    // ConfigureToolbars is special because of bug #200815
    const Qt::ConnectionType defaultConnectionType = (id == ConfigureToolbars) ? Qt::QueuedConnection : Qt::AutoConnection;
    QObject::connect(action, &QAction::triggered, recvr, slot, connectionType.value_or(defaultConnectionType));
    return action;
}

/**
 * This will return the internal name of a given standard action.
 */
KCONFIGWIDGETS_EXPORT QString name(StandardAction id);

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Returns a list of all standard names. Used by KAccelManager
 * to give those higher weight.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "This is for internal usage only and if you really need to use it, use KStandardActions::stdNames instead.")
QStringList stdNames();
#endif

/**
 * Returns a list of all actionIds.
 *
 * @since 4.2
 */
KCONFIGWIDGETS_EXPORT QList<StandardAction> actionIds();

/**
 * Returns the standardshortcut associated with @a actionId.
 *
 * @param id    The identifier whose associated shortcut is wanted.
 *
 * @since 4.2
 */
KCONFIGWIDGETS_EXPORT KStandardShortcut::StandardShortcut shortcutForActionId(StandardAction id);

// clang-format off
// we have to disable the templated function for const char* as Func, since it is ambiguous otherwise
// TODO: KF6: unify const char* version and new style by removing std::enable_if
#ifdef K_DOXYGEN
#define KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(name, enumValue) \
    inline QAction *name(const QObject *recvr, Func slot, QObject *parent);
#else
#define KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(name, enumValue) \
    template<class Receiver, class Func> \
    inline typename std::enable_if<!std::is_convertible<Func, const char*>::value, QAction>::type *name(const Receiver *recvr, Func slot, QObject *parent) \
    { return create(enumValue, recvr, slot, parent); }
#endif
// clang-format on

// clang-format off
#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
// we have to disable the templated function for const char* as Func, since it is ambiguous otherwise
// TODO: KF6: unify const char* version and new style by removing std::enable_if
#ifdef K_DOXYGEN
#define KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(name, enumValue) \
    [[deprecated("Use KStandardActions instead")]] \
    inline QAction *name(const QObject *recvr, Func slot, QObject *parent);
#else
#define KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(name, enumValue) \
    template<class Receiver, class Func> \
    KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::" #name " instead") \
    inline typename std::enable_if<!std::is_convertible<Func, const char*>::value, QAction>::type *name(const Receiver *recvr, Func slot, QObject *parent) \
    { return create(enumValue, recvr, slot, parent); }
#endif
#endif
// clang-format on

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Create a new document or window.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::openNew instead")
QAction *openNew(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Create a new document or window.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(openNew, New)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Open an existing file.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardAction::open overload with new connect syntax instead")
QAction *open(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Open an existing file.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(open, Open)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Open a recently used document. The signature of the slot being called
 * is of the form slotURLSelected( const QUrl & ).
 * @param recvr object to receive slot
 * @param slot The SLOT to invoke when a URL is selected. The slot's
 * signature is slotURLSelected( const QUrl & ).
 * @param parent parent widget
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardAction::openRecent overload with new connect syntax instead")
KRecentFilesAction *openRecent(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * The same as openRecent(const QObject *, const char *, QObject *), but using new-style connect syntax
 * @see openRecent(const QObject *, const char *, QObject *)
 * @since 5.23
 */
#ifdef K_DOXYGEN
inline KRecentFilesAction *openRecent(const QObject *recvr, Func slot, QObject *parent)
#else
template<class Receiver, class Func>
inline typename std::enable_if<!std::is_convertible<Func, const char *>::value, KRecentFilesAction>::type *
openRecent(const Receiver *recvr, Func slot, QObject *parent)
#endif
{
    QAction *action = _k_createInternal(OpenRecent, parent);
    KRecentFilesAction *recentAction = qobject_cast<KRecentFilesAction *>(action);
    Q_ASSERT(recentAction);
    QObject::connect(recentAction, &KRecentFilesAction::urlSelected, recvr, slot);
    return recentAction;
}

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Save the current document.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::save instead")
QAction *save(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Save the current document.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(save, Save)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Save the current document under a different name.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::saveAs instead")
QAction *saveAs(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Save the current document under a different name.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(saveAs, SaveAs)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Revert the current document to the last saved version
 * (essentially will undo all changes).
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::revert instead")
QAction *revert(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Revert the current document to the last saved version
 * (essentially will undo all changes).
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(revert, Revert)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Close the current document.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::close instead")
QAction *close(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Close the current document.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(close, Close)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Print the current document.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::print instead")
QAction *print(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Print the current document.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(print, Print)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Show a print preview of the current document.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::printPreview instead")
QAction *printPreview(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Show a print preview of the current document.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(printPreview, PrintPreview)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Send the current document by mail.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::saveAs instead")
QAction *mail(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Mail this document.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(mail, Mail)

/**
 * Quit the program.
 *
 * Note that you probably want to connect this action to either QWidget::close()
 * or QApplication::closeAllWindows(), but not QApplication::quit(), so that
 * KMainWindow::queryClose() is called on any open window (to warn the user
 * about unsaved changes for example).
 */
#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::quit instead")
QAction *quit(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Quit the program.
 * @see quit(const QObject *recvr, const char *slot, QObject *parent)
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(quit, Quit)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Undo the last operation.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::undo instead")
QAction *undo(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Undo the last operation.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(undo, Undo)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Redo the last operation.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::redo instead")
QAction *redo(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Redo the last operation.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(redo, Redo)

/**
 * Cut selected area and store it in the clipboard.
 * Calls cut() on the widget with the current focus.
 */
KCONFIGWIDGETS_EXPORT QAction *cut(QObject *parent);

/**
 * Copy selected area and store it in the clipboard.
 * Calls copy() on the widget with the current focus.
 */
KCONFIGWIDGETS_EXPORT QAction *copy(QObject *parent);

/**
 * Paste the contents of clipboard at the current mouse or cursor
 * Calls paste() on the widget with the current focus.
 */
KCONFIGWIDGETS_EXPORT QAction *paste(QObject *parent);

// TODO K3ListView is long gone. Is this still relevant?
/**
 * Clear selected area.  Calls clear() on the widget with the current focus.
 * Note that for some widgets, this may not provide the intended behavior.  For
 * example if you make use of the code above and a K3ListView has the focus, clear()
 * will clear all of the items in the list.  If this is not the intended behavior
 * and you want to make use of this slot, you can subclass K3ListView and reimplement
 * this slot.  For example the following code would implement a K3ListView without this
 * behavior:
 *
 * \code
 * class MyListView : public K3ListView {
 *   Q_OBJECT
 * public:
 *   MyListView( QWidget * parent = 0, const char * name = 0, WFlags f = 0 ) : K3ListView( parent, name, f ) {}
 *   virtual ~MyListView() {}
 * public Q_SLOTS:
 *   virtual void clear() {}
 * };
 * \endcode
 */
KCONFIGWIDGETS_EXPORT QAction *clear(QObject *parent);

/**
 * Calls selectAll() on the widget with the current focus.
 */
KCONFIGWIDGETS_EXPORT QAction *selectAll(QObject *parent);

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Cut selected area and store it in the clipboard.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::cut instead")
QAction *cut(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Cut selected area and store it in the clipboard.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(cut, Cut)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Copy the selected area into the clipboard.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::copy instead")
QAction *copy(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Copy the selected area into the clipboard.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(copy, Copy)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Paste the contents of clipboard at the current mouse or cursor
 * position.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::paste instead")
QAction *paste(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Paste the contents of clipboard at the current mouse or cursor
 * position.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(paste, Paste)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Clear the content of the focus widget
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::clear instead")
QAction *clear(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Clear the content of the focus widget
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(clear, Clear)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Select all elements in the current document.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::selectAll instead")
QAction *selectAll(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Select all elements in the current document.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(selectAll, SelectAll)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Deselect any selected elements in the current document.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::deselect instead")
QAction *deselect(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Deselect any selected elements in the current document.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(deselect, Deselect)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Initiate a 'find' request in the current document.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::find instead")
QAction *find(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Initiate a 'find' request in the current document.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(find, Find)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Find the next instance of a stored 'find'.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::findNext instead")
QAction *findNext(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Find the next instance of a stored 'find'.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(findNext, FindNext)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Find a previous instance of a stored 'find'.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::findPrev instead")
QAction *findPrev(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Find a previous instance of a stored 'find'.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(findPrev, FindPrev)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Find and replace matches.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::replace instead")
QAction *replace(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Find and replace matches.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(replace, Replace)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * View the document at its actual size.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::actualSize instead")
QAction *actualSize(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * View the document at its actual size.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(actualSize, ActualSize)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Fit the document view to the size of the current window.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::fitToPage instead")
QAction *fitToPage(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Fit the document view to the size of the current window.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(fitToPage, FitToPage)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Fit the document view to the width of the current window.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::fitToWidth instead")
QAction *fitToWidth(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Fit the document view to the width of the current window.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(fitToWidth, FitToWidth)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Fit the document view to the height of the current window.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::fitToHeight instead")
QAction *fitToHeight(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Fit the document view to the height of the current window.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(fitToHeight, FitToHeight)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Zoom in the current document view.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::zoomIn instead")
QAction *zoomIn(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Zoom in the current document view.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(zoomIn, ZoomIn)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Zoom out the current document view.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::zoomOut instead")
QAction *zoomOut(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Zoom out the current document view.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(zoomOut, ZoomOut)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Select the current zoom level.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::zoom instead")
QAction *zoom(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Select the current zoom level.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(zoom, Zoom)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Redisplay or redraw the document.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::redisplay instead")
QAction *redisplay(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Redisplay or redraw the document.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(redisplay, Redisplay)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Move up (web style menu).
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::up instead")
QAction *up(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Move up (web style menu).
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(up, Up)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Move back (web style menu).
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::back instead")
QAction *back(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Move back (web style menu).
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(back, Back)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Move forward (web style menu).
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::forward instead")
QAction *forward(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Move forward (web style menu).
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(forward, Forward)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Go to the "Home" position or document.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::home instead")
QAction *home(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Go to the "Home" position or document.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(home, Home)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Scroll up one page.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::prior instead")
QAction *prior(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Scroll up one page.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(prior, Prior)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Scroll down one page.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::next instead")
QAction *next(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Scroll down one page.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(next, Next)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Jump to some specific location in the document.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::goTo instead")
QAction *goTo(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Jump to some specific location in the document.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(goTo, Goto)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Go to a specific page.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::goToPage instead")
QAction *gotoPage(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Go to a specific page.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(gotoPage, GotoPage)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Go to a specific line.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::goToLine instead")
QAction *gotoLine(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Go to a specific line.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(gotoLine, GotoLine)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Jump to the first page.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::firstPage instead")
QAction *firstPage(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Jump to the first page.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(firstPage, FirstPage)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Jump to the last page.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::lastPage instead")
QAction *lastPage(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Jump to the last page.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(lastPage, LastPage)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Move back (document style menu).
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::documentBack instead")
QAction *documentBack(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Move back (document style menu).
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(documentBack, DocumentBack)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Move forward (document style menu).
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::documentForward instead")
QAction *documentForward(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Move forward (document style menu).
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(documentForward, DocumentForward)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Add the current page to the bookmarks tree.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::addBookmark instead")
QAction *addBookmark(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Add the current page to the bookmarks tree.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(addBookmark, AddBookmark)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Edit the application bookmarks.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::editBookmarks instead")
QAction *editBookmarks(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Edit the application bookmarks.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(editBookmarks, EditBookmarks)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Pop up the spell checker.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::spelling instead")
QAction *spelling(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Pop up the spell checker.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(spelling, Spelling)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Show/Hide the menubar.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardAction::showMenubar overload with new connect syntax instead")
KToggleAction *showMenubar(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * The same as showMenubar(const QObject *, const char *, QObject *), but using new-style connect syntax
 * @see showMenubar(const QObject *, const char *, QObject *)
 * @since 5.23
 */
#ifdef K_DOXYGEN
inline KToggleAction *showMenubar(const QObject *recvr, Func slot, QObject *parent)
#else
template<class Receiver, class Func>
inline typename std::enable_if<!std::is_convertible<Func, const char *>::value, KToggleAction>::type *
showMenubar(const Receiver *recvr, Func slot, QObject *parent)
#endif
{
    QAction *ret = create(ShowMenubar, recvr, slot, parent);
    Q_ASSERT(qobject_cast<KToggleAction *>(ret));
    return static_cast<KToggleAction *>(ret);
}

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Show/Hide the statusbar.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardAction::showStatusBar overload with new connect syntax instead")
KToggleAction *showStatusbar(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Show/Hide the statusbar.
 * @since 5.23
 */
#ifdef K_DOXYGEN
inline KToggleAction *showStatusbar(const QObject *recvr, Func slot, QObject *parent)
#else
template<class Receiver, class Func>
inline typename std::enable_if<!std::is_convertible<Func, const char *>::value, KToggleAction>::type *
showStatusbar(const Receiver *recvr, Func slot, QObject *parent)
#endif
{
    QAction *ret = create(ShowStatusbar, recvr, slot, parent);
    Q_ASSERT(qobject_cast<KToggleAction *>(ret));
    return static_cast<KToggleAction *>(ret);
}

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Switch to/from full screen mode
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardAction::fullScreen overload with new connect syntax instead")
KToggleFullScreenAction *fullScreen(const QObject *recvr, const char *slot, QWidget *window, QObject *parent);
#endif

/**
 * Switch to/from full screen mode
 * @since 5.23
 */
#ifdef K_DOXYGEN
inline KToggleFullScreenAction *fullScreen(const QObject *recvr, Func slot, QWidget *window, QObject *parent)
#else
template<class Receiver, class Func>
inline typename std::enable_if<!std::is_convertible<Func, const char *>::value, KToggleFullScreenAction>::type *
fullScreen(const Receiver *recvr, Func slot, QWidget *window, QObject *parent)
#endif
{
    QAction *a = create(FullScreen, recvr, slot, parent);
    Q_ASSERT(qobject_cast<KToggleFullScreenAction *>(a));
    KToggleFullScreenAction *ret = static_cast<KToggleFullScreenAction *>(a);
    ret->setWindow(window);
    return ret;
}

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Display the configure keyboard shortcuts dialog.
 *
 * Note that you might be able to use the pre-built KXMLGUIFactory's function:
 * @code
 * KStandardAction::keyBindings(guiFactory(), &KXMLGUIFactory::showConfigureShortcutsDialog, actionCollection());
 * @endcode
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::keyBindings instead")
QAction *keyBindings(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Display the configure key bindings dialog.
 * @see keyBindings(const QObject *recvr, const char *slot, QObject *parent)
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(keyBindings, KeyBindings)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Display the preferences/options dialog.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::preferences instead")
QAction *preferences(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Display the preferences/options dialog.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(preferences, Preferences)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Display the toolbar configuration dialog.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::configureToolbars instead")
QAction *configureToolbars(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Display the toolbar configuration dialog.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(configureToolbars, ConfigureToolbars)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Display the notifications configuration dialog.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::configureNotifications instead")
QAction *configureNotifications(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Display the notifications configuration dialog.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(configureNotifications, ConfigureNotifications)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Display the Switch Application Language dialog.
 * @since 5.67
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::switchApplicationLanguage instead")
QAction *switchApplicationLanguage(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Display the Switch Application Language dialog.
 * @since 5.67
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(switchApplicationLanguage, SwitchApplicationLanguage)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Display the handbook of the application.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::helpContents instead")
QAction *helpContents(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Display the handbook of the application.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(helpContents, HelpContents)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Trigger the What's This cursor.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::whatsThis instead")
QAction *whatsThis(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Trigger the What's This cursor.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(whatsThis, WhatsThis)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Open up the Report Bug dialog.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::reportBug instead")
QAction *reportBug(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Open up the Report Bug dialog.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(reportBug, ReportBug)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Display the application's About box.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::aboutApp instead")
QAction *aboutApp(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Display the application's About box.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(aboutApp, AboutApp)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Display the About KDE dialog.
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::aboutKDE instead")
QAction *aboutKDE(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Display the About KDE dialog.
 * @since 5.23
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(aboutKDE, AboutKDE)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Permanently deletes files or folders.
 * @since 5.25
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::deleteFile instead")
QAction *deleteFile(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Permanently deletes files or folders.
 * @since 5.25
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(deleteFile, DeleteFile)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Renames files or folders.
 * @since 5.25
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::renameFile instead")
QAction *renameFile(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Renames files or folders.
 * @since 5.25
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(renameFile, RenameFile)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Moves files or folders to the trash.
 * @since 5.25
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::moveToTrash instead")
QAction *moveToTrash(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Moves files or folders to the trash.
 * @since 5.25
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(moveToTrash, MoveToTrash)

#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
/**
 * Open donation page on kde.org.
 * @since 5.26
 */
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardActions::donate instead")
QAction *donate(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Open donation page on kde.org.
 * @since 5.26
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT_DEPRECATED(donate, Donate)

/**
 * Opens a menu that substitutes the menubar.
 * @since 5.81
 */
#if KCONFIGWIDGETS_ENABLE_DEPRECATED_SINCE(6, 3)
KCONFIGWIDGETS_EXPORT
KCONFIGWIDGETS_DEPRECATED_VERSION(6, 3, "Use KStandardAction::hamburgerMenu with new connect syntax instead")
KHamburgerMenu *hamburgerMenu(const QObject *recvr, const char *slot, QObject *parent);
#endif

/**
 * Opens a menu that substitutes the menubar.
 * @since 5.81
 */
KSTANDARDACTION_WITH_NEW_STYLE_CONNECT(hamburgerMenu, HamburgerMenu)

}

#endif // KSTDACTION_H
