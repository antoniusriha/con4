/*
 * networkgameitemmodel.cpp
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

#include "networkgameitemmodel.h"
#include "ngltreeitem.h"

NetworkGameItemModel::NetworkGameItemModel(IndexServiceList &list,
										   QObject *parent)
	: QAbstractItemModel(parent), _list(list), _rootItem(0)
{
	QVector<QVariant> headers(4);
	headers[0] = "Name";
	headers[1] = "Initiator name";
	headers[2] = "Dimensions";
	headers[3] = "IP Address/Port";
	_rootItem = new NglTreeItem(headers);

	connect(&list, SIGNAL(refreshing()), this, SLOT(_refreshing()));
	connect(&list, SIGNAL(refreshed()), this, SLOT(_refreshed()));
}

NetworkGameItemModel::~NetworkGameItemModel() { delete _rootItem; }

QModelIndex NetworkGameItemModel::index(int row, int column,
										const QModelIndex &parent) const
{
	if (row < 0 || column < 0 || (parent.isValid() && parent.column() != 0))
		return QModelIndex();

	NglTreeItem *parentItem = _getItem(parent);
	NglTreeItem *childItem = parentItem->child(row);
	if (childItem) return createIndex(row, column, childItem);
	return QModelIndex();
}

QModelIndex NetworkGameItemModel::parent(const QModelIndex &child) const
{
	if (!child.isValid()) return QModelIndex();
	NglTreeItem *childItem = _getItem(child);
	NglTreeItem *parentItem = childItem->parent();
	if (parentItem == _rootItem) return QModelIndex();
	return createIndex(parentItem->childNumber(), 0, parentItem);
}

int NetworkGameItemModel::rowCount(const QModelIndex &parent) const
{
	NglTreeItem *parentItem = _getItem(parent);
	return parentItem->childCount();
}

int NetworkGameItemModel::columnCount(const QModelIndex &) const
{
	return _rootItem->columnCount();
}

QVariant NetworkGameItemModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || role != Qt::DisplayRole || index.row() < 0 ||
		index.column() < 0)
		return QVariant::Invalid;

	NglTreeItem *item = _getItem(index);
	return item->data(index.column());
}

QVariant NetworkGameItemModel::headerData(int section,
										  Qt::Orientation orientation,
										  int role) const
{
	if (orientation != Qt::Horizontal || section < 0 ||
		section > _colCountGame || role != Qt::DisplayRole)
		return QVariant::Invalid;
	return _rootItem->data(section);
}

void NetworkGameItemModel::_refreshed()
{
	QList<NetworkGame *> games;

	_rootItem->removeChildren(0, _rootItem->childCount());
	for (int i = 0; i < _list.size(); i++) {
		IndexService *service = _list.at(i);
		int position = _rootItem->childCount();
		_rootItem->insertChildren(position, 1, _colCountIdxSrv);
		NglTreeItem *item = _rootItem->child(position);

		QString data = QString("%1 (%2:%3)").arg(service->name())
			.arg(service->ipAddress().toString()).arg(service->port());
		item->setData(0, data);

		games = service->games();
		item->insertChildren(0, games.size(), _colCountGame);
		for (int j = 0; j < games.size(); j++) {
			const NetworkGame *game = games.at(j);
			NglTreeItem *gameItem = item->child(j);
			gameItem->setData(0, game->name().string());
			gameItem->setData(1, game->initiatorName().string());

			QString dimsStr;
			Game::Dimensions dims = game->dims();
			if (dims.depth() == 1)
				dimsStr = QString("%1x%2").arg(dims.width()).arg(dims.height());
			else {
				dimsStr = QString("%1x%2x%3").arg(dims.width())
						.arg(dims.height()).arg(dims.depth());
			}
			gameItem->setData(2, dimsStr);

			QString ipAndPort = QString("%1:%2")
					.arg(game->ipAddress().toString()).arg(game->port());
			gameItem->setData(3, ipAndPort);
		}
	}

	endResetModel();
}

NglTreeItem *NetworkGameItemModel::_getItem(const QModelIndex &index) const
{
	if (index.isValid()) {
		NglTreeItem *item = static_cast<NglTreeItem *>(index.internalPointer());
		if (item) return item;
	}
	return _rootItem;
}
