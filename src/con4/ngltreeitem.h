/*
 * ngltreeitem.h
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

#ifndef NGLTREEITEM_H
#define NGLTREEITEM_H

#include <QVector>
#include <QVariant>

class NglTreeItem
{
public:
	NglTreeItem(const QVector<QVariant> &data, NglTreeItem *parent = 0);
	~NglTreeItem() { qDeleteAll(_childItems); }

	NglTreeItem *child(int number) { return _childItems.value(number); }
	int childCount() const { return _childItems.count(); }
	int columnCount() const { return _itemData.count(); }
	QVariant data(int column) const { return _itemData.value(column); }
	bool insertChildren(int position, int count, int columns);
	bool insertColumns(int position, int columns);
	NglTreeItem *parent() { return _parentItem; }
	bool removeChildren(int position, int count);
	bool removeColumns(int position, int columns);
	int childNumber() const;
	bool setData(int column, const QVariant &value);

private:
	QList<NglTreeItem *> _childItems;
	QVector<QVariant> _itemData;
	NglTreeItem *_parentItem;
};

#endif // NGLTREEITEM_H
