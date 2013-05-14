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

#include "boardconf.h"


void BoardConf::toColorVec(QColor color, QVector<float> &colorVec)
{
	colorVec[0] = color.redF();
	colorVec[1] = color.greenF();
	colorVec[2] = color.blueF();
	colorVec[3] = color.alphaF();
}

BoardConf::BoardConf(QObject *parent) : QObject(parent), _dims(),
	_colsDistance(0.5), _background(4), _boardBase(4), _cylinders(4),
	_player1Color(4), _player2Color(4), _player1Enabled(true),
	_player2Enabled(true), _gameTitle(), _gameDescription(),
	_player1Name("Player 1"), _player2Name("Player 2")
{
	setBackground(QColor(65, 74, 76, 200));
	setBoardBase(QColor(Qt::lightGray));
	setCylinders(QColor(Qt::lightGray));
	setPlayer1Color(QColor(Qt::red));
	setPlayer2Color(QColor(Qt::yellow));
}

bool BoardConf::isPlayerEnabled(FieldValue player) const
{
	return (player == Player1 && _player1Enabled) ||
		   (player == Player2 && _player2Enabled);
}

QColor BoardConf::_getColor(const QVector<float> color) const
{
	return QColor::fromRgbF(color[0], color[1], color[2], color[3]);
}

void BoardConf::_setColor(QColor value, QVector<float> &field)
{
	if (value.redF() == field[0] && value.greenF() == field[1] &&
		value.blueF() == field[2] && value.alphaF() == field[3]) return;
	toColorVec(value, field);
	emit changed();
}
