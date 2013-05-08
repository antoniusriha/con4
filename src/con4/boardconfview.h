/*
 * boardconfview.h
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

#ifndef BOARDCONFVIEW_H
#define BOARDCONFVIEW_H

#include <QWidget>
#include "../con4core/game.h"

namespace Ui {
class BoardConfView;
}

class BoardConfView : public QWidget
{
	Q_OBJECT

public:
	explicit BoardConfView(QWidget *parent = 0);
	~BoardConfView();

	Game::Dimensions dims() const { return _dims; }

signals:
	void changed();

private slots:
	void _threeDToggled(bool checked);
	void _widthChanged(int value);
	void _heightChanged(int value);
	void _depthChanged(int value);

private:
	Game::Dimensions _dims;
	int _prevWidth, _prevHeight, _prevDepth;
	Ui::BoardConfView *ui;
};

#endif // BOARDCONFVIEW_H
