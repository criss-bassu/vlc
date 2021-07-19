/*****************************************************************************
 * Copyright (C) 2021 the VideoLAN team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#include "color_scheme_model.hpp"

#include "qt.hpp"

class ColorSchemeModel::DefaultSchemeList : public ColorSchemeModel::SchemeList
{
public:
    DefaultSchemeList()
        : m_items {{qtr("System"), ColorScheme::System}, {qtr("Day"), ColorScheme::Day}, {qtr("Night"), ColorScheme::Night}}
    {
    }

    ColorScheme scheme(int i) const override
    {
        return m_items.at(i).scheme;
    }

    QString text(int i) const override
    {
        return m_items.at(i).text;
    }

    int size() const override
    {
        return m_items.size();
    }

private:
    struct Item
    {
        QString text;
        ColorScheme scheme;
    };

    const QVector<Item> m_items;
};

ColorSchemeModel::ColorSchemeModel(QObject* parent)
    : QAbstractListModel(parent)
    , m_list {new DefaultSchemeList}
    , m_currentIndex {0}
{
}

int ColorSchemeModel::rowCount(const QModelIndex &) const
{
    return m_list->size();
}

Qt::ItemFlags ColorSchemeModel::flags (const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);
    if (index.isValid())
        return defaultFlags | Qt::ItemIsUserCheckable;
    return defaultFlags;
}

QVariant ColorSchemeModel::data(const QModelIndex &index, const int role) const
{
    if (!checkIndex(index, CheckIndexOption::IndexIsValid))
        return QVariant {};

    if (role == Qt::CheckStateRole)
        return m_currentIndex == index.row() ? Qt::Checked : Qt::Unchecked;

    if (role == Qt::DisplayRole)
        return m_list->text(index.row());

    return QVariant {};
}

bool ColorSchemeModel::setData(const QModelIndex &index,
                               const QVariant &value, const int role)
{
    if (role == Qt::CheckStateRole
            && checkIndex(index, CheckIndexOption::IndexIsValid)
            && index.row() != m_currentIndex
            && value.type() == QVariant::Bool
            && value.toBool())
    {
        setCurrentIndex(index.row());
        return true;
    }

    return false;
}

int ColorSchemeModel::currentIndex() const
{
    return m_currentIndex;
}

void ColorSchemeModel::setCurrentIndex(const int newIndex)
{
    if (m_currentIndex == newIndex)
        return;

    assert(newIndex >= 0 && newIndex < m_list->size());
    const auto oldIndex = this->index(m_currentIndex);
    m_currentIndex = newIndex;
    emit dataChanged(index(m_currentIndex), index(m_currentIndex), {Qt::CheckStateRole});
    emit dataChanged(oldIndex, oldIndex, {Qt::CheckStateRole});
    emit currentChanged();
}

QString ColorSchemeModel::currentText() const
{
    return m_list->text(m_currentIndex);
}

void ColorSchemeModel::setCurrentScheme(const ColorScheme scheme)
{
    for (int i = 0; i < m_list->size(); ++i)
    {
        if (m_list->scheme(i) == scheme)
        {
            setCurrentIndex(i);
            break;
        }
    }
}

ColorSchemeModel::ColorScheme ColorSchemeModel::currentScheme() const
{
    return m_list->scheme(m_currentIndex);
}
