/*
    This file is part of the KDE project
    SPDX-FileCopyrightText: 2013 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCOLORSCHEMEMANAGER_P_H
#define KCOLORSCHEMEMANAGER_P_H

#include <QAbstractListModel>
#include <QIcon>
#include <memory>

struct KColorSchemeModelData {
    QString name;
    QString path;
    QIcon preview;
};

class KColorSchemeModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit KColorSchemeModel(QObject *parent = nullptr);
    ~KColorSchemeModel() override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    void init();
    mutable QVector<KColorSchemeModelData> m_data;
};

class KColorSchemeManagerPrivate
{
public:
    KColorSchemeManagerPrivate();

    std::unique_ptr<KColorSchemeModel> model;
};

#endif
