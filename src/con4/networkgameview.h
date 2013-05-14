/*
 * networkgameview.h
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

#ifndef NETWORKGAMEVIEW_H
#define NETWORKGAMEVIEW_H

#include <QWidget>
#include "gamehost.h"
#include "aiplayerinfo.h"

namespace Ui {
class NetworkGameView;
}

class NetworkGameView : public QWidget
{
	Q_OBJECT

public:
	class InvalidOperationException : public std::logic_error
	{
	public:
		explicit InvalidOperationException(QString what)
			: logic_error(what.toStdString()) {}
	};

	explicit NetworkGameView(QWidget *parent = 0);
	~NetworkGameView();

	bool isInitialized() const;
	void initialize(IndexServiceList &list,
					QList<AiPlayerInfo *> &aiPlayerFactories);

	NetworkGameHostConf conf() const;

private slots:
	void _update();

private:
	Ui::NetworkGameView *ui;
	NetworkGameHostConf *_conf;
};

#endif // NETWORKGAMEVIEW_H
