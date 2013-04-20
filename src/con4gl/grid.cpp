/*
 * grid.cpp
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

#include "grid.h"

Grid::Grid(Game *game, QObject *parent)
	: GLElement(parent), _game (game)
{
	setPlayerColor(Player1, QColor(Qt::red));
	setPlayerColor(Player2, QColor(Qt::yellow));
	toColorVec(QColor(Qt::lightGray), _bottomColor);

	_sphereRadius = 0.1;
	_boardBaseHeight = 0.1;

	_wCursor = 0;
	_dCursor = 0;

	_quadric = gluNewQuadric();
}

Grid::~Grid()
{
	gluDeleteQuadric(_quadric);
	if (_game->finished()) {
		delete [] _conWidthIdcs;
		delete [] _conHeightIdcs;
		delete [] _conDepthIdcs;
	}
}

void Grid::draw() const
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, _bottomColor);

	// center board
	float boardWidth = _game->width() * _colsDistance;
	float boardDepth = _game->depth() * _colsDistance;
	glTranslatef(-boardWidth / 2, 0, boardDepth / 2);
	_drawBoardBottom(boardWidth, boardDepth);

	_drawCylinders();

	if (!_game->hasStarted()) return;

	glTranslatef(_colsDistance / 2, _boardBaseHeight + _sphereRadius, -_colsDistance / 2);
	for (int i = 0; i < _game->height(); i++) {
		for (int j = 0; j < _game->depth(); j++) {
			for (int k = 0; k < _game->width(); k++) {
				FieldValue field = _game->get(k, i, j);
				if (field != None) {
					if (field == Player1)
						glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, _player1Colorf);
					else glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, _player2Colorf);
					gluSphere(_quadric, _sphereRadius, 35, 35);
				}
				glTranslatef(_colsDistance, 0.0, 0.0);
			}
			glTranslatef(-_colsDistance * _game->width(), 0.0, -_colsDistance);
		}
		glTranslatef(0.0, _sphereRadius * 2, _colsDistance * _game->depth());
	}

	if (!_game->finished()) {
		glTranslatef(_wCursor * _colsDistance, _sphereRadius / 2, -_dCursor * _colsDistance);
		if (_game->curPlayer() == Player1)
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, _player1Colorf);
		else glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, _player2Colorf);
		gluSphere(_quadric, _sphereRadius, 35, 35);
	} else {
		if (_game->winner() == Player1)
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, _player1WinColor);
		else glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, _player2WinColor);
		glTranslatef(0.0, -_game->height() * _sphereRadius * 2, 0.0);
		for (int i = 0; i < _game->nConnect(); i++) {
			int wIdx = _conWidthIdcs[i];
			int hIdx = _conHeightIdcs[i];
			int dIdx = _conDepthIdcs[i];
			glTranslatef(wIdx * _colsDistance, hIdx * _sphereRadius * 2, -dIdx * _colsDistance);
			gluSphere(_quadric, _sphereRadius * 1.001, 35, 35);
			glTranslatef(-wIdx * _colsDistance, -hIdx * _sphereRadius * 2, dIdx * _colsDistance);
		}
	}
}

void Grid::setPlayerColor(FieldValue player, QColor color)
{
	if (player == None) return;
	if (player == Player1) {
		_player1Color = color;
		toColorVec(color, _player1Colorf);
		toColorVec(color.lighter(175), _player1WinColor);
	} else {
		_player2Color = color;
		toColorVec(color, _player2Colorf);
		toColorVec(color.lighter(175), _player2WinColor);
	}
}

bool Grid::moveCursorUp()
{
	if (_dCursor < _game->depth() - 1) _dCursor++;
	else return false;
	return true;
}

bool Grid::moveCursorDown()
{
	if (_dCursor > 0) _dCursor--;
	else return false;
	return true;
}

bool Grid::moveCursorRight()
{
	if (_wCursor < _game->width() - 1) _wCursor++;
	else return false;
	return true;
}

bool Grid::moveCursorLeft()
{
	if (_wCursor > 0) _wCursor--;
	else return false;
	return true;
}

bool Grid::setDisk()
{
	bool success = _game->set(_wCursor, _dCursor);
	if (_game->finished()) {
		_conWidthIdcs = new int[_game->nConnect()];
		_conHeightIdcs = new int[_game->nConnect()];
		_conDepthIdcs = new int[_game->nConnect()];
		_game->connected(_conWidthIdcs, _conHeightIdcs, _conDepthIdcs);
	}
	return success;
}

void Grid::_drawBoardBottom(float width, float depth) const
{
	glBegin(GL_QUADS);

	//Quad 1
	glNormal3f(1.0f, 0.0f, 0.0f); //N1
	glTexCoord2f(0.0f, 1.0f); glVertex3f( width, _boardBaseHeight,    0.0); //V2
	glTexCoord2f(0.0f, 0.0f); glVertex3f( width,              0.0,    0.0); //V1
	glTexCoord2f(1.0f, 0.0f); glVertex3f( width,              0.0, -depth); //V3
	glTexCoord2f(1.0f, 1.0f); glVertex3f( width, _boardBaseHeight, -depth); //V4

	//Quad 2
	glNormal3f(0.0f, 0.0f, -1.0f); //N2
	glTexCoord2f(0.0f, 1.0f); glVertex3f( width, _boardBaseHeight, -depth); //V4
	glTexCoord2f(0.0f, 0.0f); glVertex3f( width,              0.0, -depth); //V3
	glTexCoord2f(1.0f, 0.0f); glVertex3f(   0.0,              0.0, -depth); //V5
	glTexCoord2f(1.0f, 1.0f); glVertex3f(   0.0, _boardBaseHeight, -depth); //V6

	//Quad 3
	glNormal3f(-1.0f, 0.0f, 0.0f); //N3
	glTexCoord2f(0.0f, 1.0f); glVertex3f(   0.0, _boardBaseHeight, -depth); //V6
	glTexCoord2f(0.0f, 0.0f); glVertex3f(   0.0,              0.0, -depth); //V5
	glTexCoord2f(1.0f, 0.0f); glVertex3f(   0.0,              0.0,    0.0); //V7
	glTexCoord2f(1.0f, 1.0f); glVertex3f(   0.0, _boardBaseHeight,    0.0); //V8

	//Quad 4
	glNormal3f(0.0f, 0.0f, 1.0f); //N4
	glTexCoord2f(0.0f, 1.0f); glVertex3f(   0.0, _boardBaseHeight,    0.0); //V8
	glTexCoord2f(0.0f, 0.0f); glVertex3f(   0.0,              0.0,    0.0); //V7
	glTexCoord2f(1.0f, 0.0f); glVertex3f( width,              0.0,    0.0); //V1
	glTexCoord2f(1.0f, 1.0f); glVertex3f( width, _boardBaseHeight,    0.0); //V2

	//Quad 5
	glNormal3f(0.0f, 1.0f, 0.0f); //N5
	glTexCoord2f(0.0f, 1.0f); glVertex3f(   0.0, _boardBaseHeight, -depth); //V6
	glTexCoord2f(0.0f, 0.0f); glVertex3f(   0.0, _boardBaseHeight,    0.0); //V8
	glTexCoord2f(1.0f, 0.0f); glVertex3f( width, _boardBaseHeight,    0.0); //V2
	glTexCoord2f(1.0f, 1.0f); glVertex3f( width, _boardBaseHeight, -depth); //V4

	//Quad 6
	glNormal3f(1.0f, -1.0f, 0.0f); //N6
	glTexCoord2f(0.0f, 1.0f); glVertex3f(   0.0,              0.0,    0.0); //V7
	glTexCoord2f(0.0f, 0.0f); glVertex3f(   0.0,              0.0, -depth); //V5
	glTexCoord2f(1.0f, 0.0f); glVertex3f( width,              0.0, -depth); //V3
	glTexCoord2f(1.0f, 1.0f); glVertex3f( width,              0.0,    0.0); //V1

	glEnd();
}

void Grid::_drawCylinders() const
{
	float sphereDiameter = _sphereRadius * 2;
	float height = sphereDiameter * _game->height();

	// rotate on x,z
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glTranslatef(_colsDistance / 2, _colsDistance / 2, _boardBaseHeight);
	for (int i = 0; i < _game->depth(); i++) {
		for (int j = 0; j < _game->width(); j++) {
			gluCylinder(_quadric, 0.01, 0.01, height, 15, 15);
			glTranslatef(_colsDistance, 0.0, 0.0);
		}
		glTranslatef(-_colsDistance * _game->width(), _colsDistance, 0.0);
	}
	glTranslatef(-_colsDistance / 2, -_colsDistance * _game->depth() - _colsDistance / 2, -_boardBaseHeight);
	glRotatef(90.0, 1.0, 0.0, 0.0);
}
