/*
 * randomsetplayer.h
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

#ifndef RANDOMSETPLAYER_H
#define RANDOMSETPLAYER_H

#include <QTimer>
#include "aiplayerinfo.h"

class RandomSetPlayer : public AiPlayer
{
	Q_OBJECT
public:
	explicit RandomSetPlayer(Game &game, FieldValue player,
							 QObject *parent = 0);

	int delay() const { return _timer.interval(); }
	void setDelay(int ms) { _timer.setInterval(ms); }

private slots:
	void _started(FieldValue startPlayer);
	void _set(FieldValue player, Game::BoardIndex);
	void _set();

private:
	Game &_game;
	FieldValue _player;
	QTimer _timer;
};

class RandomSetPlayerInfo : public AiPlayerInfo
{
public:
	explicit RandomSetPlayerInfo(QObject *parent = 0);
	QString name() const { return "Funky Fox"; }
	RandomSetPlayer *create(Game &game, FieldValue player,
							QObject *parent = 0) const;
};

#endif // RANDOMSETPLAYER_H
