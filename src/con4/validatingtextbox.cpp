/*
 * validatingtextbox.cpp
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

#include "validatingtextbox.h"
#include "ui_validatingtextbox.h"

ValidatingTextBox::ValidatingTextBox(QWidget *parent)
	: QWidget(parent), ui(new Ui::ValidatingTextBox), _criteria()
{
	ui->setupUi(this);
}

ValidatingTextBox::~ValidatingTextBox()
{
	delete ui;
	qDeleteAll(_criteria);
}

QString ValidatingTextBox::text() const { return ui->lineEdit->text(); }

void ValidatingTextBox::setText(QString value) { ui->lineEdit->setText(value); }

QString ValidatingTextBox::placeholderText() const
{
	return ui->lineEdit->placeholderText();
}

void ValidatingTextBox::setPlaceholderText(QString value)
{
	ui->lineEdit->setPlaceholderText(value);
}

bool ValidatingTextBox::isValid() const
{
	Criterium *criterium;
	QString text = ui->lineEdit->text();
	for (int i = 0; i < _criteria.size(); i++) {
		criterium = _criteria.at(i);
		if (!criterium->test(text)) {
			ui->errorLabel->setText(criterium->errorString());
			return false;
		}
	}
	ui->errorLabel->setText("");
	return true;
}

void ValidatingTextBox::_textChanged(QString)
{
	isValid();
	emit changed();
}
