/*
 * boardview.cpp
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

#include "boardview.h"
#include "ui_boardview.h"

static void qNormalizeAngle(int &angle)
{
	while (angle < 0)
		angle += 360 * 16;
	while (angle > 360 * 16)
		angle -= 360 * 16;
}

BoardView::BoardView(QWidget *parent)
	: QGLWidget(parent), ui(new Ui::BoardView), _conf(), _dims(), _game(0),
	  _grid(_conf), _disks(0)
{
	_distance = -5;
	_xRot = _yRot = _zRot = 0;

	ui->setupUi(this);
	_setWidgetsVisible(false);
	connect(&_conf, SIGNAL(changed()), this, SLOT(_update()));
}

BoardView::~BoardView() { delete ui; }

void BoardView::setGame(Game *value)
{
	if (value == _game) return;
	if (value) {
		_conf.setDims(value->dims());
		_disks = new Disks(_conf, *value, this);
		connect(value, SIGNAL(set(FieldValue,Game::BoardIndex)),
				this, SLOT(_update()));
	} else {
		disconnect(_game, SIGNAL(set(FieldValue,Game::BoardIndex)),
				   this, SLOT(_update()));
		delete _disks;
	}
	_setWidgetsVisible(value);
	_game = value;
	updateGL();
}

void BoardView::_update()
{
	ui->titleLabel->setVisible(!_conf.gameTitle().isEmpty());
	ui->titleLabel->setText(_conf.gameTitle());
	ui->descriptionLabel->setVisible(!_conf.gameDescription().isEmpty());
	ui->descriptionLabel->setText(_conf.gameDescription());

	QColor c = _conf.background();
	QString style = QString("QLabel { background-color: "
							"rgba(%1, %2, %3, %4); }")
			.arg(c.red()).arg(c.green()).arg(c.blue()).arg(c.alpha());
	ui->titleLabel->setStyleSheet(style);
	ui->descriptionLabel->setStyleSheet(style);
	ui->statusLabel->setStyleSheet(style);

	updateGL();
}

void BoardView::initializeGL()
{
	// color: outer space black
	qglClearColor(_conf.background());

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_MULTISAMPLE);
	static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

void BoardView::paintGL()
{
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glTranslatef(0.0, -0.5, _distance);
	glRotatef(_xRot / 16.0, 1.0, 0.0, 0.0);
	glRotatef(_yRot / 16.0, 0.0, 1.0, 0.0);
	glRotatef(_zRot / 16.0, 0.0, 0.0, 1.0);

	_grid.draw();
	if (_disks) _disks->draw();
}

void BoardView::resizeGL(int width, int height)
{
	if (height == 0) height = 1;
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)width/(float)height, 0.1, 1000);

	glMatrixMode(GL_MODELVIEW);
}

void BoardView::mousePressEvent(QMouseEvent *event)
{
	_lastPos = event->pos();
	event->accept();
}

void BoardView::mouseMoveEvent(QMouseEvent *event)
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

void BoardView::wheelEvent(QWheelEvent *event)
{
	int delta = event->delta();
	if (event->orientation() == Qt::Vertical) {
		if (delta < 0) _distance *= 1.1;
		else if (delta > 0) _distance *= 0.9;
		updateGL();
	}
	event->accept();
}

void BoardView::keyPressEvent(QKeyEvent *event)
{
	if (!_disks || !_conf.isPlayerEnabled(_game->curPlayer())) {
		QWidget::keyPressEvent(event);
		return;
	}

	Game::BoardIndex idx = _disks->cursor();
	switch (event->key()) {
	case Qt::Key_Left:
	case Qt::Key_A:
		if (idx.wVal() < 1) return;
		idx.setWVal(idx.wVal() - 1);
		_disks->setCursor(idx);
		break;
	case Qt::Key_Right:
	case Qt::Key_D:
		if (idx.wVal() >= _game->dims().width() - 1) return;
		idx.setWVal(idx.wVal() + 1);
		_disks->setCursor(idx);
		break;
	case Qt::Key_Up:
	case Qt::Key_W:
		if (idx.dVal() >= _game->dims().depth() - 1) return;
		idx.setDVal(idx.dVal() + 1);
		_disks->setCursor(idx);
		break;
	case Qt::Key_Down:
	case Qt::Key_S:
		if (idx.dVal() < 1) return;
		idx.setDVal(idx.dVal() - 1);
		_disks->setCursor(idx);
		break;
	case Qt::Key_Return:
	case Qt::Key_Space:
		_game->set(idx);
		break;
	default:
		QWidget::keyPressEvent(event);
		return;
	}
	updateGL();
}

void BoardView::_setWidgetsVisible(bool value)
{
	ui->titleLabel->setVisible(value);
	ui->descriptionLabel->setVisible(value);
	ui->statusLabel->setVisible(value);
	ui->pushButton->setVisible(value);
}

void BoardView::_setXRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != _xRot) {
		_xRot = angle;
		updateGL();
	}
}

void BoardView::_setYRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != _yRot) {
		_yRot = angle;
		updateGL();
	}
}

void BoardView::_setZRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != _zRot) {
		_zRot = angle;
		updateGL();
	}
}
