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

class NotEmptyCriterium : public PlayerConfView::NameCriterium
{
	bool testPlayerName(QString value) const { return !value.isEmpty(); }
	QString errorString() const { return "Name must not be empty."; }
};

PlayerConfView::PlayerConfView(QWidget *parent)
	: QWidget(parent), ui(new Ui::PlayerConfView), _nameCriteria()
{
	ui->setupUi(this);
	_nameCriteria.append(new NotEmptyCriterium());
	connect(ui->colorButton, SIGNAL(changed()), this, SIGNAL(changed()));
	connect(ui->moveDelaySpinButton, SIGNAL(valueChanged(int)),
			this, SIGNAL(changed()));
}

PlayerConfView::~PlayerConfView()
{
	delete ui;
	qDeleteAll(_nameCriteria);
}

bool PlayerConfView::isValid() const
{
	NameCriterium *criterium;
	QString text = ui->nameLineEdit->text();
	for (int i = 0; i < _nameCriteria.size(); i++) {
		criterium = _nameCriteria.at(i);
		if (!criterium->testPlayerName(text)) {
			ui->errorLabel->setText(criterium->errorString());
			return false;
		}
	}
	ui->errorLabel->setText("");
	return true;
}

QString PlayerConfView::name() const { return ui->nameLineEdit->text(); }

void PlayerConfView::setName(QString value) const
{
	ui->nameLineEdit->setText(value);
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

void PlayerConfView::_nameEdited(QString)
{
	isValid();
	emit changed();
}

void PlayerConfView::_playerTypeChanged(int selIndex)
{
	bool enable = (PlayerType)selIndex == Computer;
	ui->moveDelayLabel->setEnabled(enable);
	ui->moveDelaySpinButton->setEnabled(enable);
	emit changed();
}
