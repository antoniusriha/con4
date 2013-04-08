/*
 * indexservicetest.cpp
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

#include "indexservicetest.h"

IndexServiceTest::IndexServiceTest (QObject *parent) : QObject(parent),
    _service (QHostAddress ("127.0.0.1"), Service::toQuint16 ("8000"), "local") {}

IndexServiceTest::~IndexServiceTest () {

}

bool IndexServiceTest::runRegisterGameTest() {
    QHostAddress gameHost = QHostAddress ("127.0.0.1");
    quint16 gamePort = Service::toQuint16 ("9999");

    _game = new NetworkGame (gameHost, gamePort);
    _game->setHeight (4);
    _game->setWidth (5);
    _game->setDepth (6);
    _game->setPlayer1 ("Sebastian");
    _game->setName ("Bastigame");

    connect (&_service, SIGNAL (registerGameCompleted (Response)),
             this, SLOT (_registerGameCompleted (Response)));
    _service.registerGameAsync (_game);
}

void IndexServiceTest::_registerGameCompleted (Response resp) {
    if (!resp.Success) qDebug () << resp.ErrMsg;
    else qDebug () << "SUCCESS: Game registered.";
    delete _game;
}

bool IndexServiceTest::runRequestGameListTest() {
    connect (&_service, SIGNAL (requestGameListCompleted (GameListResponse)),
             this, SLOT (_requestGameListCompleted (GameListResponse)));
    _service.requestGameListAsync ();
}

void IndexServiceTest::_requestGameListCompleted (GameListResponse resp) {
    if (!resp.Success) qDebug () << resp.ErrMsg;
    else qDebug () << "SUCCESS: Game list received.";
}
