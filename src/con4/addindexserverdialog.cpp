/*
 * addindexserverdialog.cpp
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

#include <QHostAddress>
#include <QPushButton>
#include "addindexserverdialog.h"
#include "ui_addindexserverdialog.h"
#include "textvalidationcriteria.h"

class UniqueNameCriterium : public ValidatingTextBox::Criterium
{
public:
	UniqueNameCriterium(IndexServiceList &list) : Criterium(), _list(list) {}

private:
	bool test(QString value) const
	{
		for (int i = 0; i < _list.size (); i++)
			if (_list.at(i)->name() == value) return false;
		return true;
	}

	QString errorString() const { return "The name exists already."; }

	IndexServiceList &_list;
};

AddIndexServerDialog::AddIndexServerDialog(IndexServiceList &list,
										   QWidget *parent)
	: QDialog(parent), ui(new Ui::AddIndexServerDialog), _list(list)
{
	ui->setupUi(this);
	ui->serverNameTextBox->criteria()->append(new NotEmptyCriterium());
	ui->serverNameTextBox->criteria()->append(new NoSemiColonCriterium());
	ui->serverNameTextBox->criteria()->append(new NoNewLineCriterium());
	ui->serverNameTextBox->criteria()->append(new UniqueNameCriterium(list));

	connect(ui->serverNameTextBox, SIGNAL(changed()), this, SLOT(_update()));
	connect(ui->ipAddressTextBox, SIGNAL(changed()), this, SLOT(_update()));

	_update();
}

AddIndexServerDialog::~AddIndexServerDialog() { delete ui; }

void AddIndexServerDialog::accept()
{
	_list.create(QHostAddress(ui->ipAddressTextBox->ipAddress()),
				 (quint16)ui->portSpinBox->value(),
				 ui->serverNameTextBox->text());
	QDialog::accept();
}

void AddIndexServerDialog::_update()
{
	bool enable = ui->serverNameTextBox->isValid() &&
			ui->ipAddressTextBox->isValid();
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(enable);
}
