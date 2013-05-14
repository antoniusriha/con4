/*
 * localgameconfview.cpp
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

#include "localgameconfview.h"
#include "ui_localgameconfview.h"

LocalGameConfView::LocalGameConfView(QWidget *parent)
	: QWidget(parent), ui(new Ui::LocalGameConfView), _conf()
{
	ui->setupUi(this);
	ui->player1Conf->setName(_conf.player1Name());
	ui->player1Conf->setColor(QColor(Qt::red));
	ui->player2Conf->setName(_conf.player2Name());
	ui->player2Conf->setColor(QColor(Qt::yellow));

	connect(ui->player1Conf, SIGNAL(changed()), this, SLOT(_update()));
	connect(ui->player2Conf, SIGNAL(changed()), this, SLOT(_update()));
	connect(ui->boardConf, SIGNAL(changed()), this, SLOT(_update()));

	_update();
}

LocalGameConfView::~LocalGameConfView() { delete ui; }

void LocalGameConfView::initialize(QList<AiPlayerInfo *> &aiPlayerFactories)
{
	ui->player1Conf->initialize(aiPlayerFactories);
	ui->player2Conf->initialize(aiPlayerFactories);
}

bool LocalGameConfView::isInitialized() const
{
	return ui->player1Conf->isInitialized() &&
			ui->player2Conf->isInitialized();
}

void LocalGameConfView::_update()
{
	QString playerName;
	if (ui->player1Conf->isValid()) playerName = ui->player1Conf->name();
	else playerName = "Player 1";
	_conf.setPlayerNames(playerName, _conf.player2Name());

	if (ui->player2Conf->isValid()) playerName = ui->player2Conf->name();
	else playerName = "Player 2";
	_conf.setPlayerNames(_conf.player1Name(), playerName);

	if (ui->player1Conf->color() != ui->player2Conf->color())
		_conf.setPlayerColors(ui->player1Conf->color(),
							  ui->player2Conf->color());
	else {
		ui->player1Conf->setColor(_conf.player1Color());
		ui->player2Conf->setColor(_conf.player2Color());
	}

	_conf.setPlayer1AiInfo(ui->player1Conf->aiPlayerInfo());
	_conf.setPlayer2AiInfo(ui->player2Conf->aiPlayerInfo());

	_conf.setDims(ui->boardConf->dims());
}
