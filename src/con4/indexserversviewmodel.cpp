/*
 * indexserversviewmodel.cpp
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

#include "indexserversviewmodel.h"
#include "application.h"

IndexServersViewModel::IndexServersViewModel (QObject *parent)
    : QAbstractTableModel (parent) {
    _indexServices = Application::instance ().indexServices ();
}

int IndexServersViewModel::rowCount (const QModelIndex &parent) const {
    return _indexServices->size ();
}

QVariant IndexServersViewModel::data (const QModelIndex &index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole ||
        index.row () < 0 || index.row () >= _indexServices->size ())
        return QVariant ();

    IndexService *service = _indexServices->at (index.row ());
    if (index.column () == 0) return service->name ();
    else if (index.column () == 1) return service->ipAddress ().toString ();
    else if (index.column () == 2) return service->port ();
    return QVariant ();
}

QVariant IndexServersViewModel::headerData (int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal || section < 0 || section > 2 || role != Qt::DisplayRole)
        return QVariant ();

    if (section == 0) return "Name";
    else if (section == 1) return "IP Address";
    else if (section == 2) return "Port";
    return QVariant ();
}

void IndexServersViewModel::updateOnRowAdded () {
    int idx = _indexServices->size ();
    beginInsertRows (QModelIndex (), idx, idx);
    endInsertRows ();
}

void IndexServersViewModel::updateOnRowsRemoved (int first, int last) {
    beginRemoveRows (QModelIndex (), first, last);
    endRemoveRows ();
}
