/*
 * tst_indexservice.cpp
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

#include <QtCore/QString>
#include <QtTest/QtTest>

#include "tst_global.h"
#include "../../src/con4/indexservice.h"

class IndexServiceTest : public QObject {
    Q_OBJECT
public:
    IndexServiceTest ();

private Q_SLOTS:
    void registerGame_data ();
    void registerGame ();
    void requestGameList_data ();
    void requestGameList ();
};

IndexServiceTest::IndexServiceTest() {}

void IndexServiceTest::registerGame_data () {
    QTest::addColumn<QString> ("qsHost");
    QTest::addColumn<quint16> ("port");
    QTest::addColumn<QString> ("name");

    QString qsPort = "8000";
    QTextStream ts (&qsPort);
    quint16 port = 0;
    ts >> port;

    QTest::newRow ("Localhost") << "127.0.0.1" << port << "local index server";
}

void IndexServiceTest::registerGame () {
//    QFETCH (QString, qsHost);
//    QFETCH (quint16, port);
//    QFETCH (QString, name);
//    QHostAddress host = QHostAddress (qsHost);

//    QHostAddress gameHost = QHostAddress ("127.0.0.1");
//    QString qsGamePort = "9999";
//    QTextStream ts (&qsGamePort);
//    quint16 gamePort = 0;
//    ts >> gamePort;
//    NetworkGame *game = new NetworkGame (gameHost, ++gamePort);
//    game->setHeight (4);
//    game->setWidth (5);
//    game->setDepth (6);
//    game->setPlayer1 ("Franz");
//    game->setName ("Franzigame");
//    IndexService *service = new IndexService (host, port, name);
//    Response resp = service->registerGame (game);
//    if (!resp.Success) qDebug () << resp.ErrMsg;
//    QCOMPARE (resp.Success, true);
//    delete game;
//    delete service;
}

void IndexServiceTest::requestGameList_data () {
    registerGame_data ();
}

void IndexServiceTest::requestGameList () {
//    QFETCH (QString, qsHost);
//    QFETCH (quint16, port);
//    QFETCH (QString, name);
//    QHostAddress host = QHostAddress (qsHost);
//    IndexService *service = new IndexService (host, port, name);
//    GameListResponse resp = service->requestGameList ();
//    if (!resp.Success) qDebug () << resp.ErrMsg;
//    QCOMPARE (resp.Success, true);
//    for (QList<NetworkGame *>::iterator it = resp.Games.begin (); it != resp.Games.end (); it++) {
//        qDebug () << "Name  : " << (*it)->name ();
//        qDebug () << "Player: " << (*it)->player1 ();
//        qDebug () << "Name  : " << (*it)->host ();
//        qDebug () << "Port  : " << (*it)->port ();
//        qDebug () << "Dims  : " << (*it)->width () << "x" << (*it)->height () << "x" << (*it)->depth ();
//        qDebug () << "Open  : " << !(*it)->hasStarted ();
//        qDebug ();
//        delete *it;
//    }
//    delete service;
}

DECLARE_TEST (IndexServiceTest)
#include "tst_indexservice.moc"
