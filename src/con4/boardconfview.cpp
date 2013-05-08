/*
 * boardconfview.cpp
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

#include "boardconfview.h"
#include "ui_boardconfview.h"

BoardConfView::BoardConfView(QWidget *parent) : QWidget(parent), _dims(),
	_prevWidth(_dims.width()), _prevHeight(_dims.height()),
	_prevDepth(_dims.depth()), ui(new Ui::BoardConfView)
{
	ui->setupUi(this);
	ui->widthSpinButton->setValue(_dims.width());
	ui->heightSpinButton->setValue(_dims.height());
	ui->depthSpinButton->setValue(_dims.depth());
}

BoardConfView::~BoardConfView() { delete ui; }

void BoardConfView::_threeDToggled(bool checked)
{
	if (checked) _dims.setDepth(ui->depthSpinButton->value());
	else _dims.setDepth(1);
	emit changed();
}

void BoardConfView::_widthChanged(int value)
{
	try {
		_dims.setWidth(value);
		_prevWidth = value;
		emit changed();
	} catch (Game::Dimensions::Exception) {
		ui->widthSpinButton->setValue(_prevWidth);
	}
}

void BoardConfView::_heightChanged(int value)
{
	try {
		_dims.setHeight(value);
		_prevHeight = value;
		emit changed();
	} catch (Game::Dimensions::Exception) {
		ui->heightSpinButton->setValue(_prevHeight);
	}
}

void BoardConfView::_depthChanged(int value)
{
	try {
		_dims.setDepth(value);
		_prevDepth = value;
		emit changed();
	} catch (Game::Dimensions::Exception) {
		ui->depthSpinButton->setValue(_prevDepth);
	}
}
