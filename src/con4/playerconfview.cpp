/*
 * playerconfview.cpp
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

#include "playerconfview.h"
#include "ui_playerconfview.h"
#include "textvalidationcriteria.h"

PlayerConfView::PlayerConfView(QWidget *parent)
	: QWidget(parent), ui(new Ui::PlayerConfView)
{
	ui->setupUi(this);
	ui->nameTextBox->criteria()->append(new NotEmptyCriterium());
	ui->nameTextBox->setPlaceholderText("Player name");

	connect(ui->nameTextBox, SIGNAL(changed()), this, SIGNAL(changed()));
	connect(ui->colorButton, SIGNAL(changed()), this, SIGNAL(changed()));
	connect(ui->moveDelaySpinButton, SIGNAL(valueChanged(int)),
			this, SIGNAL(changed()));
}

PlayerConfView::~PlayerConfView() { delete ui; }

bool PlayerConfView::isValid() const
{
	return ui->nameTextBox->isValid();
}

QList<ValidatingTextBox::Criterium *> *PlayerConfView::nameCriteria()
{
	return ui->nameTextBox->criteria();
}

QString PlayerConfView::name() const { return ui->nameTextBox->text(); }

void PlayerConfView::setName(QString value) const
{
	ui->nameTextBox->setText(value);
}

QColor PlayerConfView::color() const { return ui->colorButton->color(); }

void PlayerConfView::setColor(QColor value)
{
	ui->colorButton->setColor(value);
}

PlayerConfView::PlayerType PlayerConfView::playerType() const
{
	return (PlayerType)ui->playerTypeComboBox->currentIndex();
}

void PlayerConfView::setPlayerType(PlayerType type)
{
	ui->playerTypeComboBox->setCurrentIndex((int)type);
}

int PlayerConfView::moveDelay() const
{
	return ui->moveDelaySpinButton->value();
}

void PlayerConfView::setMoveDelay(int value)
{
	ui->moveDelaySpinButton->setValue(value);
}

void PlayerConfView::_playerTypeChanged(int selIndex)
{
	bool enable = (PlayerType)selIndex == Computer;
	ui->moveDelayLabel->setEnabled(enable);
	ui->moveDelaySpinButton->setEnabled(enable);
	emit changed();
}
