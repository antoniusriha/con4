/*
 * indexservicelist.cpp
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

#include "indexservicelist.h"

const QString IndexServiceList::IdxSrvArrayKey = "indexServers";
const QString IndexServiceList::IdxSrvNameKey = "name";
const QString IndexServiceList::IdxSrvAddrKey = "ip";
const QString IndexServiceList::IdxSrvPortKey = "port";

IndexServiceList::IndexServiceList(Settings *settings, QObject *parent)
	: QAbstractTableModel(parent), _list(QList<IndexService *>()),
	  _settings(settings), _keys(QList<QString>())
{
	settings->registerKey(IdxSrvArrayKey);
	settings->registerKey(IdxSrvNameKey);
	settings->registerKey(IdxSrvAddrKey);
	settings->registerKey(IdxSrvPortKey);

	_keys.append(IdxSrvNameKey);
	_keys.append(IdxSrvAddrKey);
	_keys.append(IdxSrvPortKey);

	Settings::ValuesArray servicesData =
			_settings->getArray(IdxSrvArrayKey, _keys);
	for (int i = 0; i < servicesData.size(); i++) {
		QString name = servicesData.at(i).at(0).toString();
		QHostAddress address =
				QHostAddress(servicesData.at(i).at(1).toString());
		quint16 port = (quint16)servicesData.at(i).at(2).toInt();
		IndexService *item = new IndexService(address, port, name);
		_list.append(item);
	}
}

IndexServiceList::~IndexServiceList()
{
	for (int i = 0; i < _list.size(); i++) {
		IndexService *item = _list.at(i);
		_list.removeAt(i);
		delete item;
	}
}

int IndexServiceList::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid()) return 0;
	return _list.size();
}

int IndexServiceList::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid()) return 0;
	return 3;
}

QVariant IndexServiceList::data(const QModelIndex &index, int role) const
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

QVariant IndexServiceList::headerData(int section, Qt::Orientation orientation,
									  int role) const
{
	if (orientation != Qt::Horizontal || section < 0 || section > 2 ||
			role != Qt::DisplayRole) return QVariant::Invalid;

	if (section == 0) return "Name";
	else if (section == 1) return "IP Address";
	else if (section == 2) return "Port";
	return QVariant::Invalid;
}

void IndexServiceList::create(QHostAddress host, quint16 port, QString name)
{
	IndexService *item = new IndexService(host, port, name);
	int index = _list.size();
	beginInsertRows(QModelIndex(), index, index);
	_list.append(item);
	_updateSettings();
	endInsertRows();
	emit created(item);
}

bool IndexServiceList::deleteItem(IndexService *item)
{
	if (!_list.contains(item)) return false;
	int idx = _list.indexOf(item);
	return deleteAt(idx);
}

bool IndexServiceList::deleteAt(int index)
{
	if (index < 0 || index >= _list.size()) return false;
	IndexService *item = _list.at(index);
	beginRemoveRows(QModelIndex(), index, index);
	_list.removeAt(index);
	_updateSettings();
	endRemoveRows();
	delete item;
	emit deletedAt(index);
	return true;
}

void IndexServiceList::_updateSettings()
{
	QList<QString> attrNames;
	attrNames.append(IdxSrvNameKey);
	attrNames.append(IdxSrvAddrKey);
	attrNames.append(IdxSrvPortKey);

	Settings::ValuesArray attrVals;
	for (int i = 0; i < _list.size(); i++) {
		QList<QVariant> vals;
		vals.append(_list.at(i)->name());
		vals.append(_list.at(i)->ipAddress().toString());
		vals.append(_list.at(i)->port());
		attrVals.append(vals);
	}

	_settings->setArray(IdxSrvArrayKey, attrNames, attrVals);
}
