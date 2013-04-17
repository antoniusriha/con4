/*
 * mainwindow.h
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include "../con4core/game.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow (QWidget *parent = 0);
    ~MainWindow ();
    
private slots:
	void gameTypeSelectionChanged (bool state);
	void viewIndexServersClicked ();
	void startClicked ();
	void createNetworkGameClicked ();
	void joinClicked ();
	void refreshClicked ();
	void player1Clicked();
	void player2Clicked();
	void closeGame();

private:
	void _setPlayerColor(QColor color, QPushButton *button);
	QColor _getIdealTextColor(const QColor &rBackgroundColor) const;

	QColor _player1Color, _player2Color;
	Game *_localGame, *_networkGame;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
