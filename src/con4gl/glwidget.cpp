/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "glwidget.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

GLWidget::GLWidget(QWidget *parent)
	: QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
	_grid = 0;
	_distance = -5;
	_xRot = 0;
	_yRot = 0;
	_zRot = 0;

	const QString lblStyle = "QLabel { background-color: black; color: white }";
	QGridLayout *gridLayout = new QGridLayout(this);

	_lblTitle = new QLabel(this);
	_lblTitle->setStyleSheet(lblStyle);
	gridLayout->addWidget(_lblTitle, 0, 0);

	_lblDescription = new QLabel(this);
	_lblDescription->setStyleSheet(lblStyle);
	gridLayout->addWidget(_lblDescription, 1, 0);

	QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
	gridLayout->addItem(spacer, 2, 1);

	_lblStatus = new QLabel(this);
	_lblStatus->setStyleSheet(lblStyle);
	gridLayout->addWidget(_lblStatus, 3, 0);
	_btn = new QPushButton(this);
	_btn->setFocusPolicy(Qt::NoFocus);
	connect(_btn, SIGNAL(clicked()), this, SLOT(btnClicked()));
	gridLayout->addWidget(_btn, 3, 2);
}

GLWidget::~GLWidget()
{
	if (_grid) delete _grid;
}

QSize GLWidget::minimumSizeHint() const
{
	return QSize(800, 600);
}

QSize GLWidget::sizeHint() const
{
	return QSize(1024, 768);
}

void GLWidget::startGame(Game *value, bool player1Enabled, bool player2Enabled)
{
	if (_grid) delete _grid;

	_game = value;
	connect(value, SIGNAL(started(FieldValue)), this, SLOT(started()));
	connect(value, SIGNAL(finished(FieldValue)), this, SLOT(finished(FieldValue)));
	connect(value, SIGNAL(set(FieldValue,int,int)), this, SLOT(set()));
	_btn->setText("Abort game");
	_lblTitle->setText("");
	_lblDescription->setText("");
	_lblStatus->setText("Waiting for start signal...");
	_player1Name = "Player 1";
	_player2Name = "Player 2";
	_player1Enabled = player1Enabled;
	_player2Enabled = player2Enabled;

	_grid = new Grid(value);
	_grid->setColsDistance(0.5);
	updateGL();
}

void GLWidget::setGameTitle(QString value)
{
	_lblTitle->setText(value);
}

void GLWidget::setGameDescription(QString value)
{
	_lblDescription->setText(value);
}

void GLWidget::setPlayer1Name(QString value)
{
	_player1Name = value;
}

void GLWidget::setPlayer2Name(QString value)
{
	_player2Name = value;
}

void GLWidget::setPlayer1Color(QColor color)
{
	if (_grid) _grid->setPlayerColor(Player1, color);
}

void GLWidget::setPlayer2Color(QColor color)
{
	if (_grid) _grid->setPlayerColor(Player2, color);
}

static void qNormalizeAngle(int &angle)
{
	while (angle < 0)
		angle += 360 * 16;
	while (angle > 360 * 16)
		angle -= 360 * 16;
}

void GLWidget::_setXRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != _xRot) {
		_xRot = angle;
		updateGL();
	}
}

void GLWidget::_setYRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != _yRot) {
		_yRot = angle;
		updateGL();
	}
}

void GLWidget::_setZRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != _zRot) {
		_zRot = angle;
		updateGL();
	}
}

void GLWidget::initializeGL()
{
	qglClearColor(QColor(Qt::black));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_MULTISAMPLE);
	static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

void GLWidget::paintGL()
{
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glTranslatef(0.0, -0.5, _distance);
	glRotatef(_xRot / 16.0, 1.0, 0.0, 0.0);
	glRotatef(_yRot / 16.0, 0.0, 1.0, 0.0);
	glRotatef(_zRot / 16.0, 0.0, 0.0, 1.0);
	if (_grid) _grid->draw();
}

void GLWidget::resizeGL(int width, int height)
{
	if (height == 0) height = 1;
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)width/(float)height, 0.1, 1000);

	glMatrixMode(GL_MODELVIEW);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
	_lastPos = event->pos();
	event->accept();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
	int dx = event->x() - _lastPos.x();
	int dy = event->y() - _lastPos.y();

	if (event->buttons() & Qt::LeftButton) {
		_setXRotation(_xRot + 8 * dy);
		_setYRotation(_yRot + 8 * dx);
	} else if (event->buttons() & Qt::RightButton) {
		_setXRotation(_xRot + 8 * dy);
		_setZRotation(_zRot + 8 * dx);
	}
	_lastPos = event->pos();
	event->accept();
}

void GLWidget::wheelEvent(QWheelEvent * event)
{
	int delta = event->delta();
	if (event->orientation() == Qt::Vertical) {
		if (delta < 0) _distance *= 1.1;
		else if (delta > 0) _distance *= 0.9;
		updateGL();
	}
	event->accept();
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
	if (!_grid || !_isCurPlayerEnabled()) {
		QWidget::keyPressEvent(event);
		return;
	}

	switch (event->key()) {
	case Qt::Key_Left:
	case Qt::Key_A:
		_grid->moveCursorLeft();
		break;
	case Qt::Key_Right:
	case Qt::Key_D:
		_grid->moveCursorRight();
		break;
	case Qt::Key_Up:
	case Qt::Key_W:
		_grid->moveCursorUp();
		break;
	case Qt::Key_Down:
	case Qt::Key_S:
		_grid->moveCursorDown();
		break;
	case Qt::Key_Return:
	case Qt::Key_Space:
		_grid->setDisk();
		break;
	default:
		QWidget::keyPressEvent(event);
		return;
	}
	updateGL();
}

void GLWidget::finished(FieldValue winner)
{
	QString w = winner == Player1 ? _player1Name : _player2Name;
	_lblStatus->setText("Winner: " + w);
	_btn->setText("Return to main menu");
}

void GLWidget::btnClicked()
{
	if (!_game->finished()) {
		if (QMessageBox::question(this, "Game abort", "Do you really want to abort the game?",
								  QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::No) return;
		_game->abort(None, "User abort");
	}
	disconnect(_game, SIGNAL(finished(FieldValue)), this, SLOT(finished(FieldValue)));
	emit close();
}

void GLWidget::set()
{
	QString p = _game->curPlayer() == Player1 ? _player1Name : _player2Name;
	p += p.endsWith('s', Qt::CaseInsensitive) ? "'" : "'s";
	_lblStatus->setText("It's " + p + " turn.");
}

void GLWidget::started()
{
	_lblStatus->setText("");
	updateGL();
}

bool GLWidget::_isCurPlayerEnabled()
{
	return (_game->curPlayer() == Player1 && _player1Enabled) ||
		   (_game->curPlayer() == Player2 && _player2Enabled);
}
