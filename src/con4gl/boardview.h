/*
 * boardview.h
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

#ifndef BOARDVIEW_H
#define BOARDVIEW_H

#include <QtOpenGL>
#include "boardconf.h"
#include "grid.h"
#include "disks.h"
#include "../con4core/game.h"

namespace Ui {
class BoardView;
}

class BoardView : public QGLWidget
{
	Q_OBJECT
	
public:
    explicit BoardView(QWidget *parent = 0);
    ~BoardView();

    QSize minimumSizeHint() const { return QSize(800, 600); }
    QSize sizeHint() const { return QSize(1024, 768); }

    BoardConf *conf() { return &_conf; }

    const Game *game() const { return _game; }
    void setGame(Game *value);
	
private:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);

    void _setWidgetsVisible(bool value);
    void _setXRotation(int angle);
    void _setYRotation(int angle);
    void _setZRotation(int angle);
    bool _isCurPlayerEnabled();

    Ui::BoardView *ui;
    BoardConf _conf;
    Game::Dimensions _dims;
    Game *_game;
    Grid _grid;
    Disks *_disks;
    float _distance;
    int _xRot, _yRot, _zRot;
    QPoint _lastPos;
};

#endif // BOARDVIEW_H
