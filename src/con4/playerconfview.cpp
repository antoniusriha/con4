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
	: QWidget(parent), ui(new Ui::PlayerConfView), _aiFactories(0)
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

void PlayerConfView::initialize(QList<AiPlayerInfo *> &aiPlayerFactories)
{
	_aiFactories = &aiPlayerFactories;
	for (int i = 0; i < aiPlayerFactories.size(); i++)
		ui->playerTypeComboBox->addItem(aiPlayerFactories.at(i)->name());
}

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
	int curIdx = ui->playerTypeComboBox->currentIndex();
	return curIdx < 1 ? Human : Computer;
}

void PlayerConfView::setHumanPlayer()
{
	ui->playerTypeComboBox->setCurrentIndex(0);
}

const QList<AiPlayerInfo *> *PlayerConfView::aiPlayerInfoList() const
{
	return _aiFactories;
}

AiPlayerInfo *PlayerConfView::aiPlayerInfo()
{
	int curIdx = ui->playerTypeComboBox->currentIndex();
	if (curIdx < 1) return 0;
	return _aiFactories->at(curIdx - 1);
}

bool PlayerConfView::setAiPlayerInfo(AiPlayerInfo &value)
{
	if (!_aiFactories->contains(&value)) return false;
	int idx = _aiFactories->indexOf(&value) + 1;
	ui->playerTypeComboBox->setCurrentIndex(idx);
	return true;
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

void PlayerConfView::_delayChanged(int value)
{
	AiPlayerInfo *info = aiPlayerInfo();
	if (info) info->setDelay(value);
}
