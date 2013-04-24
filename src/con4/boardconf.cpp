/*
 * boardconf.cpp
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

#include <stdexcept>
#include "boardconf.h"
#include "ui_boardconf.h"

using namespace std;

BoardConf::BoardConf(QWidget *parent) : QWidget(parent), _dims(),
	_prevWidth(_dims.width()), _prevHeight(_dims.height()),
	_prevDepth(_dims.depth()), ui(new Ui::BoardConf)
{
	ui->setupUi(this);
	ui->sbWidth->setValue(_dims.width());
	ui->sbHeight->setValue(_dims.height());
	ui->sbDepth->setValue(_dims.depth());
}

BoardConf::~BoardConf() { delete ui; }

void BoardConf::chkBoard3dToggled(bool checked)
{
	if (checked) _dims.setDepth(ui->sbDepth->value());
	else _dims.setDepth(1);
}

void BoardConf::widthChanged(int value)
{
	try {
		_dims.setWidth(value);
		_prevWidth = value;
	} catch (logic_error &ex) {
		emit error(ex.what());
		ui->sbWidth->setValue(_prevWidth);
	}
}

void BoardConf::heightChanged(int value)
{
	try {
		_dims.setHeight(value);
		_prevHeight = value;
	} catch (logic_error &ex) {
		emit error(ex.what());
		ui->sbHeight->setValue(_prevHeight);
	}
}

void BoardConf::depthChanged(int value)
{
	try {
		_dims.setDepth(value);
		_prevDepth = value;
	} catch (logic_error &ex) {
		emit error(ex.what());
		ui->sbDepth->setValue(_prevDepth);
	}
}
