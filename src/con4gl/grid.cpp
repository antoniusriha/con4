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

Grid::Grid(const BoardConf &conf, QObject *parent)
	: QObject(parent), _conf(conf), _quad(gluNewQuadric()) {}

Grid::~Grid() { gluDeleteQuadric(_quad); }

void Grid::draw() const
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, _conf.boardBaseF());

	Game::Dimensions dims = _conf.dims();
	float colsDistance = _conf.colsDistance();

	// center board
	float boardWidth = dims.width() * colsDistance;
	float boardDepth = dims.depth() * colsDistance;
	glTranslatef(-boardWidth / 2, 0, boardDepth / 2);
	_drawBoardBottom(boardWidth, boardDepth);
	_drawCylinders(dims);
}

void Grid::_drawBoardBottom(float width, float depth) const
{
	glBegin(GL_QUADS);

	float boardBaseHeight = _conf.boardBaseHeight;

	//Quad 1
	glNormal3f(1.0f, 0.0f, 0.0f); //N1
	glTexCoord2f(0.0f, 1.0f); glVertex3f( width, boardBaseHeight,    0.0); //V2
	glTexCoord2f(0.0f, 0.0f); glVertex3f( width,             0.0,    0.0); //V1
	glTexCoord2f(1.0f, 0.0f); glVertex3f( width,             0.0, -depth); //V3
	glTexCoord2f(1.0f, 1.0f); glVertex3f( width, boardBaseHeight, -depth); //V4

	//Quad 2
	glNormal3f(0.0f, 0.0f, -1.0f); //N2
	glTexCoord2f(0.0f, 1.0f); glVertex3f( width, boardBaseHeight, -depth); //V4
	glTexCoord2f(0.0f, 0.0f); glVertex3f( width,             0.0, -depth); //V3
	glTexCoord2f(1.0f, 0.0f); glVertex3f(   0.0,             0.0, -depth); //V5
	glTexCoord2f(1.0f, 1.0f); glVertex3f(   0.0, boardBaseHeight, -depth); //V6

	//Quad 3
	glNormal3f(-1.0f, 0.0f, 0.0f); //N3
	glTexCoord2f(0.0f, 1.0f); glVertex3f(   0.0, boardBaseHeight, -depth); //V6
	glTexCoord2f(0.0f, 0.0f); glVertex3f(   0.0,             0.0, -depth); //V5
	glTexCoord2f(1.0f, 0.0f); glVertex3f(   0.0,             0.0,    0.0); //V7
	glTexCoord2f(1.0f, 1.0f); glVertex3f(   0.0, boardBaseHeight,    0.0); //V8

	//Quad 4
	glNormal3f(0.0f, 0.0f, 1.0f); //N4
	glTexCoord2f(0.0f, 1.0f); glVertex3f(   0.0, boardBaseHeight,    0.0); //V8
	glTexCoord2f(0.0f, 0.0f); glVertex3f(   0.0,             0.0,    0.0); //V7
	glTexCoord2f(1.0f, 0.0f); glVertex3f( width,             0.0,    0.0); //V1
	glTexCoord2f(1.0f, 1.0f); glVertex3f( width, boardBaseHeight,    0.0); //V2

	//Quad 5
	glNormal3f(0.0f, 1.0f, 0.0f); //N5
	glTexCoord2f(0.0f, 1.0f); glVertex3f(   0.0, boardBaseHeight, -depth); //V6
	glTexCoord2f(0.0f, 0.0f); glVertex3f(   0.0, boardBaseHeight,    0.0); //V8
	glTexCoord2f(1.0f, 0.0f); glVertex3f( width, boardBaseHeight,    0.0); //V2
	glTexCoord2f(1.0f, 1.0f); glVertex3f( width, boardBaseHeight, -depth); //V4

	//Quad 6
	glNormal3f(1.0f, -1.0f, 0.0f); //N6
	glTexCoord2f(0.0f, 1.0f); glVertex3f(   0.0,             0.0,    0.0); //V7
	glTexCoord2f(0.0f, 0.0f); glVertex3f(   0.0,             0.0, -depth); //V5
	glTexCoord2f(1.0f, 0.0f); glVertex3f( width,             0.0, -depth); //V3
	glTexCoord2f(1.0f, 1.0f); glVertex3f( width,             0.0,    0.0); //V1

	glEnd();
}

void Grid::_drawCylinders(Game::Dimensions &dims) const
{
	float sphereDiameter = _conf.sphereRadius * 2;
	float height = sphereDiameter * dims.height();
	float boardBaseHeight = _conf.boardBaseHeight;
	float colsDist = _conf.colsDistance();

	// rotate on x,z
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glTranslatef(colsDist / 2, colsDist / 2, boardBaseHeight);
	for (int i = 0; i < dims.depth(); i++) {
		for (int j = 0; j < dims.width(); j++) {
			gluCylinder(_quad, 0.01, 0.01, height, 15, 15);
			glTranslatef(colsDist, 0.0, 0.0);
		}
		glTranslatef(-colsDist * dims.width(), colsDist, 0.0);
	}
	glTranslatef(-colsDist / 2, -colsDist * dims.depth() - colsDist / 2,
				 -boardBaseHeight);
	glRotatef(90.0, 1.0, 0.0, 0.0);
}
