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

BoardView::BoardView(QWidget *parent)
    : QGLWidget(parent), ui(new Ui::BoardView), _conf(), _dims(), _game(0),
      _grid(_dims, _conf), _disks(0)
{
	ui->setupUi(this);
    _setWidgetsVisible(false);
    connect(&_conf, SIGNAL(changed()), this, SLOT(updateGL()));
}

BoardView::~BoardView()
{
    delete ui;
}

Game::Dimensions BoardView::dims() const
{
    if (_game) return _game->dims();
    return _dims;
}

void BoardView::setDims(Game::Dimensions value)
{
    _game = 0;
    _setWidgetsVisible(false);
    if (value == _dims) return;
    _dims = value;
    updateGL();
}

void BoardView::setGame(Game *value)
{
    _setWidgetsVisible(value);
    if (value == _game) return;
    _game = value;
    updateGL();
}

void BoardView::initializeGL()
{
}

void BoardView::paintGL()
{
}

void BoardView::resizeGL(int width, int height)
{
}

void BoardView::mousePressEvent(QMouseEvent *event)
{
}

void BoardView::mouseMoveEvent(QMouseEvent *event)
{
}

void BoardView::wheelEvent(QWheelEvent *event)
{
}

void BoardView::keyPressEvent(QKeyEvent *event)
{
}

void BoardView::_setWidgetsVisible(bool value)
{
    ui->titleLabel->setVisible(value);
    ui->descriptionLabel->setVisible(value);
    ui->statusLabel->setVisible(value);
    ui->pushButton->setVisible(value);
}
