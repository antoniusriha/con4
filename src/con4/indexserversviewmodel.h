/*
 * indexserversviewmodel.h
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

#ifndef INDEXSERVERSVIEWMODEL_H
#define INDEXSERVERSVIEWMODEL_H

#include <QList>
#include <QAbstractTableModel>
#include "../con4net/indexservice.h"

class IndexServersViewModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit IndexServersViewModel (QObject *parent = 0);
    
    int rowCount (const QModelIndex &parent) const;
    int columnCount (const QModelIndex &parent) const { return 3; }
    QVariant data (const QModelIndex &index, int role) const;
    QVariant headerData (int section, Qt::Orientation orientation, int role) const;
    void updateOnRowAdded ();
    void updateOnRowsRemoved (int first, int last);

private:
    QList<IndexService *> *_indexServices;
};

#endif // INDEXSERVERSVIEWMODEL_H
