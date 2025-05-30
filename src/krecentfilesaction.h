/*
    This file is part of the KDE libraries
    SPDX-FileCopyrightText: 1999 Reginald Stadlbauer <reggie@kde.org>
    SPDX-FileCopyrightText: 1999 Simon Hausmann <hausmann@kde.org>
    SPDX-FileCopyrightText: 2000 Nicolas Hadacek <haadcek@kde.org>
    SPDX-FileCopyrightText: 2000 Kurt Granroth <granroth@kde.org>
    SPDX-FileCopyrightText: 2000 Michael Koch <koch@kde.org>
    SPDX-FileCopyrightText: 2001 Holger Freyther <freyther@kde.org>
    SPDX-FileCopyrightText: 2002 Ellis Whitehead <ellis@kde.org>
    SPDX-FileCopyrightText: 2003 Andras Mantia <amantia@kde.org>
    SPDX-FileCopyrightText: 2005-2006 Hamish Rodda <rodda@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KRECENTFILESACTION_H
#define KRECENTFILESACTION_H

#include <KSelectAction>
#include <QUrl>
#include <kconfigwidgets_export.h>

#include <QMimeType>

class KConfigGroup;
class KRecentFilesActionPrivate;

/*!
 * \class KRecentFilesAction
 * \inmodule KConfigWidgets
 *
 * \brief Recent files action.
 *
 * This class is an action to handle a recent files submenu.
 * The best way to create the action is to use KStandardAction::openRecent.
 * Then you simply need to call loadEntries on startup, saveEntries
 * on shutdown, addURL when your application loads/saves a file.
 */
class KCONFIGWIDGETS_EXPORT KRecentFilesAction : public KSelectAction
{
    Q_OBJECT
    Q_PROPERTY(int maxItems READ maxItems WRITE setMaxItems)
    Q_DECLARE_PRIVATE(KRecentFilesAction)

public:
    /*!
     * Constructs an action with the specified parent.
     *
     * \a parent The parent of this action.
     */
    explicit KRecentFilesAction(QObject *parent);

    /*!
     * Constructs an action with text; a shortcut may be specified by
     * the ampersand character (e.g. \"&Option\" creates a shortcut with key \c O )
     *
     * This is the most common KAction used when you do not have a
     * corresponding icon (note that it won't appear in the current version
     * of the "Edit ToolBar" dialog, because an action needs an icon to be
     * plugged in a toolbar...).
     *
     * \a text The text that will be displayed.
     *
     * \a parent The parent of this action.
     */
    KRecentFilesAction(const QString &text, QObject *parent);

    /*!
     * Constructs an action with text and an icon; a shortcut may be specified by
     * the ampersand character (e.g. \"&Option\" creates a shortcut with key \c O )
     *
     * This is the other common KAction used.  Use it when you
     * \c do have a corresponding icon.
     *
     * \a icon The icon to display.
     *
     * \a text The text that will be displayed.
     *
     * \a parent The parent of this action.
     */
    KRecentFilesAction(const QIcon &icon, const QString &text, QObject *parent);

    ~KRecentFilesAction() override;

    /*!
     * Adds \a action to the list of URLs, with \a url and title \a name.
     *
     * Do not use addAction(QAction*), as no url will be associated, and
     * consequently urlSelected() will not be emitted when \a action is selected.
     */
    void addAction(QAction *action, const QUrl &url, const QString &name, const QMimeType &mimeType = QMimeType());

    QAction *removeAction(QAction *action) override;

    /*!
     *  Returns the maximum of items in the recent files list.
     */
    int maxItems() const;

    /*!
     *  Sets the maximum of items in the recent files list.
     *  The default for this value is 10 set in the constructor.
     *
     *  If this value is lesser than the number of items currently
     *  in the recent files list the last items are deleted until
     *  the number of items are equal to the new maximum.
     *
     *  Negative values will be normalized to 0.
     */
    void setMaxItems(int maxItems);

    /*!
     *  Loads the recent files entries from a given KConfigGroup object.
     *  You can provide the name of the group used to load the entries.
     *  If the groupname is empty, entries are loaded from a group called 'RecentFiles'.
     *  Local file entries that do not exist anymore are not restored.
     *
     */
    void loadEntries(const KConfigGroup &config);

    /*!
     *  Saves the current recent files entries to a given KConfigGroup object.
     *  You can provide the name of the group used to load the entries.
     *  If the groupname is empty, entries are saved to a group called 'RecentFiles'.
     *
     */
    void saveEntries(const KConfigGroup &config);

    /*!
     * Add URL to the recent files list. This will enable this action.
     *
     * \a url The URL of the file
     *
     * \a name The user visible pretty name that appears before the URL
     *
     * \note URLs corresponding to local files in the temporary directory
     * (see QDir::tempPath()) are automatically ignored by this method.
     */
    void addUrl(const QUrl &url, const QString &name = QString());

    /*!
     * Add URL to the recent files list. This will enable this action.
     *
     * \a url The URL of the file
     *
     * \a name The user visible pretty name that appears before the URL
     *
     * \a mimeType Specify the mimeType of the file
     *
     * \note URLs corresponding to local files in the temporary directory
     * (see QDir::tempPath()) are automatically ignored by this method.
     *
     * \since 6.9
     */
    void addUrl(const QUrl &url, const QString &name, const QString &mimeType);

    /*!
     * Remove an URL from the recent files list.
     *
     * \a url The URL of the file
     */
    void removeUrl(const QUrl &url);

    /*!
     *  Retrieve a list of all URLs in the recent files list.
     */
    QList<QUrl> urls() const;

public Q_SLOTS:
    /*!
     * Clears the recent files list.
     * Note that there is also an action shown to the user for clearing the list.
     */
    virtual void clear();

Q_SIGNALS:
    /*!
     *  This signal gets emitted when the user selects an URL.
     *
     *  \a url The URL that the user selected.
     */
    void urlSelected(const QUrl &url);

    /*!
     *  This signal gets emitted when the user clear list.
     *  So when user store url in specific config file it can saveEntry.
     *  \since 4.3
     */
    void recentListCleared();

private:
    // Internal
    KCONFIGWIDGETS_NO_EXPORT void clearEntries();
    // Don't warn about the virtual overload. As the comment of the other
    // addAction() says, addAction( QAction* ) should not be used.
    using KSelectAction::addAction;

private:
    std::unique_ptr<KRecentFilesActionPrivate> const d_ptr;
};

#endif
