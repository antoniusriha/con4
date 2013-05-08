/*
 * networkgameview.cpp
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

#include "networkgameview.h"
#include "ui_networkgameview.h"
#include "textvalidationcriteria.h"

NetworkGameView::NetworkGameView(QWidget *parent)
	: QWidget(parent), ui(new Ui::NetworkGameView), _conf(0)
{
	ui->setupUi(this);

	ui->gameNameTextBox->criteria()->append(new NotEmptyCriterium());
	ui->gameNameTextBox->criteria()->append(new NoSemiColonCriterium());
	ui->gameNameTextBox->criteria()->append(new NoNewLineCriterium());
	ui->gameNameTextBox->setPlaceholderText("Game name");

	ui->player1Conf->nameCriteria()->append(new NoSemiColonCriterium());
	ui->player1Conf->nameCriteria()->append(new NoNewLineCriterium());

	connect(ui->gameNameTextBox, SIGNAL(changed()), this, SLOT(_update()));
	connect(ui->player1Conf, SIGNAL(changed()), this, SLOT(_update()));
	connect(ui->player2ColorButton, SIGNAL(changed()), this, SLOT(_update()));
	connect(ui->ipAddressTextBox, SIGNAL(changed()), this, SLOT(_update()));
	connect(ui->portSpinButton, SIGNAL(valueChanged(int)),
			this, SLOT(_update()));
	connect(ui->boardConfView, SIGNAL(changed()), this, SLOT(_update()));

	setEnabled(false);
}

NetworkGameView::~NetworkGameView()
{
	delete ui;
	if (isInitialized()) delete _conf;
}

bool NetworkGameView::isInitialized() const { return _conf; }

void NetworkGameView::initialize(IndexServiceList &list)
{
	_conf = new NetworkGameHostConf(list);
	ui->gameNameTextBox->setText("Con4 network game");
	ui->player1Conf->setName(_conf->initiatorName().string());
	ui->player1Conf->setColor(QColor(Qt::red));
	ui->player2ColorButton->setColor(QColor(Qt::yellow));
	ui->ipAddressTextBox->setIpAddress("127.0.0.1");
	_update();
	setEnabled(true);
}

NetworkGameHostConf NetworkGameView::conf() const
{
	if (!isInitialized())
		throw InvalidOperationException("The object has not been initialized.");
	return *_conf;
}

void NetworkGameView::_update()
{
	if (!isInitialized()) return;

	if (ui->gameNameTextBox->isValid())
		_conf->setName(NetworkString(ui->gameNameTextBox->text()));
	else _conf->setName(NetworkString("Con4 network game"));

	if (ui->player1Conf->isValid())
		_conf->setInitiatorName(ui->player1Conf->name());
	else _conf->setInitiatorName("Player 1");

	if (ui->player1Conf->color() != ui->player2ColorButton->color())
		_conf->setPlayerColors(ui->player1Conf->color(),
							   ui->player2ColorButton->color());
	else {
		ui->player1Conf->setColor(_conf->player1Color());
		ui->player2ColorButton->setColor(_conf->player2Color());
	}

	if (ui->ipAddressTextBox->isValid())
		_conf->setIpAddress(QHostAddress(ui->ipAddressTextBox->ipAddress()));
	else _conf->setIpAddress(QHostAddress::LocalHost);
	_conf->setPort((quint16)ui->portSpinButton->value());

	_conf->setDims(ui->boardConfView->dims());
}
