/*
 * disks.cpp
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

#include "disks.h"

Disks::Disks(const BoardConf &conf, const Game &game, QObject *parent)
	: QObject(parent), _conf(conf), _game(game), _cursor(_game.index()),
	  _quad(gluNewQuadric()), _player1WinColor(4), _player2WinColor(4)
{
	connect(&conf, SIGNAL(changed()), this, SLOT(_update()));
	_update();
}

void Disks::draw() const
{
	_drawDisks();
	if (!_game.finished()) _drawCursor();
}

void Disks::_update()
{
	BoardConf::toColorVec(_conf.player1Color().lighter(175), _player1WinColor);
	BoardConf::toColorVec(_conf.player2Color().lighter(175), _player2WinColor);
}

void Disks::_drawDisks() const
{
	float colsDist = _conf.colsDistance();
	float boardBaseHeight = _conf.boardBaseHeight;
	float sphereRadius = _conf.sphereRadius;
	Game::Dimensions dims = _conf.dims();

	glTranslatef(colsDist / 2, boardBaseHeight + sphereRadius, -colsDist / 2);
	for (int i = 0; i < dims.height(); i++) {
		for (int j = 0; j < dims.depth(); j++) {
			for (int k = 0; k < dims.width(); k++) {
				Game::BoardIndex idx = _game.index(k, j, i);
				FieldValue field = _game.get(idx);
				if (field != None) {

					const float *color;
					QVector<Game::BoardIndex> conIdcs;
					if (_game.connected(conIdcs) && conIdcs.contains(idx)) {
						if (field == Player1)
							color = _player1WinColor.constData();
						else color = _player2WinColor.constData();
					} else {
						if (field == Player1) color = _conf.player1ColorF();
						else color = _conf.player2ColorF();
					}

					glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,
								 color);
					gluSphere(_quad, sphereRadius, 35, 35);
				}
				glTranslatef(colsDist, 0.0, 0.0);
			}
			glTranslatef(-colsDist * dims.width(), 0.0, -colsDist);
		}
		glTranslatef(0.0, sphereRadius * 2, colsDist * dims.depth());
	}
}

void Disks::_drawCursor() const
{
	if (!_conf.isPlayerEnabled(_game.curPlayer())) return;

	int wCursor = _cursor.wVal();
	int dCursor = _cursor.dVal();
	float colsDist = _conf.colsDistance();
	float sphereRadius = _conf.sphereRadius;

	glTranslatef(wCursor * colsDist, sphereRadius / 2, -dCursor * colsDist);
	if (_game.curPlayer() == Player1)
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,
					 _conf.player1ColorF());
	else glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,
					  _conf.player2ColorF());
	gluSphere(_quad, sphereRadius, 35, 35);
}
