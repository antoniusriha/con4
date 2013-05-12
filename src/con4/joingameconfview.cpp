/*
 * joingameconfview.cpp
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

#include "joingameconfview.h"
#include "ui_joingameconfview.h"
#include "textvalidationcriteria.h"
#include "networkgameitemmodel.h"
#include "indexserversview.h"

JoinGameConfView::JoinGameConfView(QWidget *parent)
	: QWidget(parent), ui(new Ui::JoinGameConfView), _conf(), _list(0)
{
	ui->setupUi(this);
	ui->player1ColorButton->setColor(QColor(Qt::red));
	ui->joinPlayerConf->setName("Player 2");
	ui->joinPlayerConf->setColor(QColor(Qt::yellow));
	ui->joinPlayerConf->nameCriteria()->append(new NoSemiColonCriterium());
	ui->joinPlayerConf->nameCriteria()->append(new NoNewLineCriterium());
}

JoinGameConfView::~JoinGameConfView() { delete ui; }

void JoinGameConfView::initialize(IndexServiceList &list)
{
	if (isInitialized()) return;

	_list = &list;
	NetworkGameItemModel *model =
			new NetworkGameItemModel(list, this);
	ui->joinGamesTreeView->setModel(model);

	connect(&list, SIGNAL(created(IndexService*,int)), this, SLOT(_update()));
	connect(&list, SIGNAL(deletedAt(int)), this, SLOT(_update()));
	connect(&list, SIGNAL(refreshed()), this, SLOT(_refreshed()));

	connect(ui->joinGamesTreeView, SIGNAL(clicked(QModelIndex)),
			this, SLOT(_update()));

	connect(ui->player1ColorButton, SIGNAL(changed()), this, SLOT(_update()));
	connect(ui->joinPlayerConf, SIGNAL(changed()), this, SLOT(_update()));

	_update();
}

void JoinGameConfView::_update()
{
	if (!isInitialized()) return;

	bool listNotEmpty = !_list->empty();
	ui->joinGamesTreeView->setEnabled(listNotEmpty);
	ui->refreshButton->setEnabled(listNotEmpty);
	ui->joinButton->setEnabled(listNotEmpty);
	if (!listNotEmpty) return;

	QItemSelectionModel *selModel = ui->joinGamesTreeView->selectionModel();
	if (selModel->hasSelection()) {
		QModelIndex index = selModel->selectedRows().at(0);
		QModelIndex parent = index.parent();
		if (parent == QModelIndex()) return;

		NetOpponent *opp =
				_list->at(parent.row())->opponents()->at(index.row());
		_conf.setOpponent(*opp);
	}

	if (ui->joinPlayerConf->color() != ui->player1ColorButton->color())
		_conf.setPlayerColors(ui->player1ColorButton->color(),
							  ui->joinPlayerConf->color());
	else {
		ui->joinPlayerConf->setColor(_conf.player2Color());
		ui->player1ColorButton->setColor(_conf.player1Color());
	}

	if (ui->joinPlayerConf->isValid())
		_conf.setPlayer2Name(ui->joinPlayerConf->name());

	ui->joinButton->setEnabled(_conf.isValid());
}

void JoinGameConfView::_refreshClicked()
{
	ui->refreshButton->setEnabled(false);
	_list->refresh();
}

void JoinGameConfView::_refreshed()
{
	ui->joinGamesTreeView->expandAll();
	ui->refreshButton->setEnabled(true);
}

void JoinGameConfView::_manageIndexServersClicked()
{
	IndexServersView dlg(*_list, this);
	dlg.exec();
}

void JoinGameConfView::_joinClicked() { emit joinClicked(_conf); }
