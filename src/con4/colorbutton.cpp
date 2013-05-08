/*
 * colorbutton.cpp
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

#include "colorbutton.h"
#include <QColorDialog>

ColorButton::ColorButton(QWidget *parent) : QPushButton(parent)
{
	connect(this, SIGNAL(clicked()), this, SLOT(_clicked()));
	setColor(QColor(Qt::black));
}

void ColorButton::setColor(QColor value)
{
	if (!value.isValid() || value == _color) return;
	_color = value;
	emit changed();

	// from: http://stackoverflow.com/questions/282938/show-result-of-color-
	// selection-in-qt
	const QString style("QPushButton { background-color : %1; color : %2; }");
	QColor idealTextColor = _getIdealTextColor(value);
	setStyleSheet(style.arg(value.name()).arg(idealTextColor.name()));
}

void ColorButton::_clicked()
{
	QColorDialog dlg;
	dlg.setCurrentColor(_color);
	dlg.exec();
	setColor(dlg.selectedColor());
}

//==============================================================================
//  Nom : getIdealTextColor
//! @return an ideal label color, based on the given background color.
//! Based on http://www.codeproject.com/cs/media/IdealTextColor.asp
//==============================================================================
QColor ColorButton::_getIdealTextColor(const QColor& rBackgroundColor) const
{
	const int THRESHOLD = 105;
	int BackgroundDelta = (rBackgroundColor.red() * 0.299) +
			(rBackgroundColor.green() * 0.587) +
			(rBackgroundColor.blue() * 0.114);
	return QColor((255- BackgroundDelta < THRESHOLD) ? Qt::black : Qt::white);
}
