/*
 * disks.h
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

#ifndef DISKS_H
#define DISKS_H

#include <GL/glu.h>
#include "boardconf.h"

class Disks : public QObject
{
	Q_OBJECT
public:
	explicit Disks(const BoardConf &conf, const Game &game,
				   QObject *parent = 0);
	~Disks() { gluDeleteQuadric(_quad); }

	Game::BoardIndex cursor() const { return _cursor; }
	void setCursor(Game::BoardIndex value) { _cursor = value; }

	void draw() const;

private slots:
	void _update();

private:
	void _drawDisks() const;
	void _drawCursor() const;

	const BoardConf &_conf;
	const Game &_game;
	Game::BoardIndex _cursor;
	GLUquadric *_quad;
	float _player1WinColor[4], _player2WinColor[4];
};

#endif // DISKS_H
