/*
 * ngltreeitem.cpp
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

#include "ngltreeitem.h"

NglTreeItem::NglTreeItem(const QVector<QVariant> &data, NglTreeItem *parent)
	: _itemData(data), _parentItem(parent) {}

NglTreeItem::~NglTreeItem() { qDeleteAll(_childItems); }

NglTreeItem *NglTreeItem::child(int number)
{
	return _childItems.value(number);
}

int NglTreeItem::childCount() const
{
	return _childItems.count();
}

int NglTreeItem::childNumber() const
{
	if (_parentItem)
		return _parentItem->_childItems.indexOf(
					const_cast<NglTreeItem *>(this));
	return 0;
}

int NglTreeItem::columnCount() const
{
	return _itemData.count();
}

QVariant NglTreeItem::data(int column) const
{
	return _itemData.value(column);
}

bool NglTreeItem::insertChildren(int position, int count, int columns)
{
	if (position < 0 || position > _childItems.size())
		return false;
	for (int row = 0; row < count; ++row) {
		QVector<QVariant> data(columns);
		NglTreeItem *item = new NglTreeItem(data, this);
		_childItems.insert(position, item);
	}

	return true;
}

bool NglTreeItem::insertColumns(int position, int columns)
{
	if (position < 0 || position > _itemData.size())
		return false;

	for (int column = 0; column < columns; ++column)
		_itemData.insert(position, QVariant());

	foreach (NglTreeItem *child, _childItems)
		child->insertColumns(position, columns);

	return true;
}

NglTreeItem *NglTreeItem::parent()
{
	return _parentItem;
}

bool NglTreeItem::removeChildren(int position, int count)
{
	if (position < 0 || position + count > _childItems.size())
		return false;

	for (int row = 0; row < count; ++row)
		delete _childItems.takeAt(position);

	return true;
}

bool NglTreeItem::removeColumns(int position, int columns)
{
	if (position < 0 || position + columns > _itemData.size())
		return false;

	for (int column = 0; column < columns; ++column)
		_itemData.remove(position);

	foreach (NglTreeItem *child, _childItems)
		child->removeColumns(position, columns);

	return true;
}

bool NglTreeItem::setData(int column, const QVariant &value)
{
	if (column < 0 || column >= _itemData.size())
		return false;

	_itemData[column] = value;
	return true;
}
