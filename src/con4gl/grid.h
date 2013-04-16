/*
 * grid.h
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

#ifndef GRID_H
#define GRID_H

#include <GL/glu.h>
#include "../con4core/game.h"
#include "glelement.h"

class Grid : public GLElement
{
	Q_OBJECT
public:
	Grid(Game *game, QObject *parent = 0);
	~Grid();

	void draw() const;

	QColor playerColor (FieldValue player) const {
		if (player == None) return QColor::Invalid;
		if (player == Player1) return _player1Color;
		else return _player2Color;
	}

	float colsDistance () const { return _colsDistance; }

	void setPlayerColor(FieldValue player, QColor color);
	void setColsDistance (float value) { _colsDistance = value; }

	bool moveCursorUp();
	bool moveCursorDown();
	bool moveCursorRight();
	bool moveCursorLeft();
	bool setDisk();
	void endGame();

private:
	void _drawBoardBottom(float width, float depth) const;
	void _drawCylinders() const;

	QColor _player1Color, _player2Color;
	float _bottomColor[4], _player1Colorf[4], _player2Colorf[4],
		  _player1WinColor[4], _player2WinColor[4];
	int *_conWidthIdcs, *_conHeightIdcs, *_conDepthIdcs;
	float _colsDistance, _boardBaseHeight, _sphereRadius;
	int _wCursor, _dCursor;
	Game *_game;
	GLUquadricObj *_quadric;
};

#endif // GRID_H
