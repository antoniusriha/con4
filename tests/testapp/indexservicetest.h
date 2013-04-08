/*
 * indexservicetest.h
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

#ifndef INDEXSERVICETEST_H
#define INDEXSERVICETEST_H

#include <QObject>
#include <QString>
#include <QTextStream>
#include <QHostAddress>

#include "../../src/con4/indexservice.h"
#include "../../src/con4/networkgame.h"

class IndexServiceTest : public QObject {
    Q_OBJECT
public:
    explicit IndexServiceTest (QObject *parent = 0);
    ~IndexServiceTest ();
    void runRegisterGameTest ();
    void runRequestGameListTest ();
    
private slots:
    void _registerGameCompleted (Response resp);
    void _requestGameListCompleted (GameListResponse resp);

private:
    IndexService _service;
    NetworkGame *_game;
};

#endif // INDEXSERVICETEST_H
