/*
 * indexservicelist.h
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

#ifndef INDEXSERVICELIST_H
#define INDEXSERVICELIST_H

#include <QAbstractTableModel>
#include "../con4core/settings.h"
#include "../con4net/indexservice.h"

class IndexServiceList : public QAbstractTableModel
{
	Q_OBJECT

public:
	static const QString IdxSrvArrayKey, IdxSrvNameKey,
		IdxSrvAddrKey, IdxSrvPortKey;

	explicit IndexServiceList(Settings *settings, QObject *parent = 0);
	~IndexServiceList();

	IndexService *at(int index) { return _list.at(index); }
	void create(QHostAddress host, quint16 port, QString name);
	bool deleteItem(IndexService *item);
	bool deleteAt(int index);

	int size() const { return _list.size(); }
	bool empty() const { return _list.size() == 0; }
	int indexOf(IndexService *item) const { return _list.indexOf(item); }

signals:
	void created(IndexService *item);
	void deleting(IndexService *item);
	void deletedAt(int index);

private:
	void _updateSettings();

	QList<IndexService *> _list;
	Settings *_settings;
	QList<QString> _keys;
};

#endif // INDEXSERVICELIST_H
