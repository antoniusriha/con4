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
	IndexServiceConf conf;
	for (int i = 0; i < servicesData.size(); i++) {
		try {
			conf.setName(servicesData.at(i).at(0).toString());
			QHostAddress address (servicesData.at(i).at(1).toString());
			conf.setIpAddress(address);
			conf.setPort((quint16)servicesData.at(i).at(2).toInt());
			IndexService *item = new IndexService(conf);
			_list.append(item);
		} catch (IndexServiceConf::Exception) {}
	}
}

IndexServiceList::~IndexServiceList() { qDeleteAll(_list); }

void IndexServiceList::create(QHostAddress host, quint16 port, QString name)
{
	IndexServiceConf conf;
	conf.setName(name);
	conf.setIpAddress(host);
	conf.setPort(port);
	IndexService *item = new IndexService(conf);
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
	emit deleting(item);
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
