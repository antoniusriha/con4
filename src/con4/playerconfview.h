/*
 * playerconfview.h
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

#ifndef PLAYERCONFVIEW_H
#define PLAYERCONFVIEW_H

#include "validatingtextbox.h"
#include "aiplayerinfo.h"

namespace Ui {
class PlayerConfView;
}

class PlayerConfView : public QWidget
{
	Q_OBJECT

public:
	enum PlayerType { Human, Computer };

	explicit PlayerConfView(QWidget *parent = 0);
	~PlayerConfView();

	void initialize(QList<AiPlayerInfo *> &aiPlayerInfoList);
	bool isInitialized() const { return _aiFactories; }

	bool isValid() const;
	QList<ValidatingTextBox::Criterium *> *nameCriteria();

	QString name() const;
	void setName(QString value) const;

	QColor color() const;
	void setColor(QColor value);

	PlayerType playerType() const;
	void setHumanPlayer();
	const QList<AiPlayerInfo *> *aiPlayerInfoList() const;
	AiPlayerInfo *aiPlayerInfo();
	bool setAiPlayerInfo(AiPlayerInfo &value);

	int moveDelay() const;
	void setMoveDelay(int value);

signals:
	void changed();

private slots:
	void _playerTypeChanged(int selIndex);
	void _delayChanged(int value);

private:
	Ui::PlayerConfView *ui;
	QList<AiPlayerInfo *> *_aiFactories;
};

#endif // PLAYERCONFVIEW_H
