/*
 * indexservicetablemodel.cpp
 *
 * Author:
 *       Antonius Riha <antoniusriha@gmail.com>
 *
 * Copyright (c) 2013 Antonius Riha
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "indexservicetablemodel.h"

IndexServiceTableModel::IndexServiceTableModel(IndexServiceList &list,
											   QObject *parent)
	: QAbstractTableModel(parent), _list(list)
{
	connect(&list, SIGNAL(creating(IndexService*,int)),
			this, SLOT(_creating(IndexService*,int)));
	connect(&list, SIGNAL(created(IndexService*,int)),
			this, SLOT(_created(IndexService*,int)));
	connect(&list, SIGNAL(deleting(IndexService*,int)),
			this, SLOT(_deleting(IndexService*,int)));
	connect(&list, SIGNAL(deletedAt(int)), this, SLOT(_deletedAt(int)));
}

int IndexServiceTableModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid()) return 0;
	return _list.size();
}

int IndexServiceTableModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid()) return 0;
	return 3;
}

QVariant IndexServiceTableModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || role != Qt::DisplayRole || index.row() < 0 ||
			index.row() >= _list.size() || index.column() < 0)
		return QVariant::Invalid;

	IndexService *item = _list.at(index.row());
	if (index.column() == 0) return item->name();
	else if (index.column() == 1) return item->ipAddress().toString();
	else if (index.column() == 2) return item->port();
	return QVariant::Invalid;
}

QVariant IndexServiceTableModel::headerData(int section,
											Qt::Orientation orientation,
											int role) const
{
	if (orientation != Qt::Horizontal || section < 0 || section > 2 ||
			role != Qt::DisplayRole) return QVariant::Invalid;

	if (section == 0) return "Name";
	else if (section == 1) return "IP Address";
	else if (section == 2) return "Port";
	return QVariant::Invalid;
}

void IndexServiceTableModel::_creating(IndexService *, int index)
{
	beginInsertRows(QModelIndex(), index, index);
}

void IndexServiceTableModel::_created(IndexService *, int) { endInsertRows(); }

void IndexServiceTableModel::_deleting(IndexService *, int index)
{
	beginRemoveRows(QModelIndex(), index, index);
}

void IndexServiceTableModel::_deletedAt(int) { endRemoveRows(); }
