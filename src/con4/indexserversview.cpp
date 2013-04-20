/*
 * indexserversview.cpp
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

#include "indexserversview.h"
#include "ui_indexserversview.h"
#include "addindexserverdialog.h"

IndexServersView::IndexServersView(IndexServiceList *model, QWidget *parent)
    : QDialog(parent), ui(new Ui::IndexServersView), _model(model)
{
    ui->setupUi(this);
    ui->tableView->setModel(_model);
}

IndexServersView::~IndexServersView() { delete ui; }

void IndexServersView::deleteClicked()
{
    QItemSelectionModel *selModel = ui->tableView->selectionModel();
    if (!selModel->hasSelection()) return;

    QList<IndexService *> services;
    QModelIndexList idcs = selModel->selectedRows();
    for (int i = 0; i < idcs.size(); i++) {
        int idx = idcs.at(i).row();
        services.append(_model->at(idx));
    }

    for (int i = 0; i < idcs.size(); i++)
        _model->deleteItem(services.at(i));
}

void IndexServersView::addClicked()
{
    AddIndexServerDialog dlg(_model, this);
    dlg.exec();
}
