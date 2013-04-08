/*
 * gameconfview.cpp
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

#include "gameconfview.h"
#include "ui_gameconfview.h"

GameConfView::GameConfView (QWidget *parent) :
    QWidget (parent), ui (new Ui::GameConfView) {

    ui->setupUi (this);
    newGameSetupView = (NewGameSetupView *)ui->phNewGameSetupView;
    joinGameSetupView = (JoinGameSetupView *)ui->phJoinGameSetupView;
    //    QObject::connect (&_myThread, SIGNAL (sendData (int)), this,
    //                      SLOT (setText (int)), Qt::QueuedConnection);

    QObject::connect (newGameSetupView, SIGNAL (statusChanged (QString)),
                      ui->lblStatus, SLOT (setText (QString)));
    QObject::connect (joinGameSetupView, SIGNAL (statusChanged (QString)),
                      ui->lblStatus, SLOT (setText (QString)));
}

GameConfView::~GameConfView () {
    delete ui;
}

void GameConfView::gameTypeSelectionChanged (bool state) {
    ui->lblStatus->setText ("");
    if (ui->rbNewGame->isChecked ())
        ui->stackedWidget->setCurrentWidget (ui->phNewGameSetupView);
    else ui->stackedWidget->setCurrentWidget (ui->phJoinGameSetupView);
}
