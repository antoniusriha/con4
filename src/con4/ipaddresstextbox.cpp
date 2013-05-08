/*
 * ipaddresstextbox.cpp
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

#include <QGridLayout>
#include <QHostAddress>
#include "ipaddresstextbox.h"

class IpAddressCriterium : public ValidatingTextBox::Criterium
{
	bool test(QString value) const
	{
		QHostAddress addr;
		return addr.setAddress(value);
	}

	QString errorString() const { return "Invalid ip address."; }
};

IpAddressTextBox::IpAddressTextBox(QWidget *parent) : QWidget(parent)
{
	QGridLayout *layout = new QGridLayout(this);
	layout->setMargin(0);
	_validatingTextBox = new ValidatingTextBox(this);
	_validatingTextBox->criteria()->append(new IpAddressCriterium());
	layout->addWidget(_validatingTextBox);
	connect(_validatingTextBox, SIGNAL(changed()), this, SIGNAL(changed()));
}

QString IpAddressTextBox::ipAddress() const
{
	return _validatingTextBox->text();
}

void IpAddressTextBox::setIpAddress(QString value)
{
	_validatingTextBox->setText(value);
}
