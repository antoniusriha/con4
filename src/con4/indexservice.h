/*
 * indexservice.h
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

#ifndef INDEXSERVICE_H
#define INDEXSERVICE_H

#include <QObject>
#include <QMutex>
#include <QUuid>
#include <QHostAddress>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QUdpSocket>
#include <QtConcurrentRun>
#include <QFutureWatcher>

#include "networkgame.h"
#include "service.h"

struct GameListResponse : Response {
    QList<NetworkGame *> Games;
};

class IndexService : public Service {
    Q_OBJECT
public:
    IndexService (QHostAddress host, quint16 port, QString name);
    
    Response registerGame (NetworkGame *game);
    GameListResponse requestGameList ();

    void registerGameAsync (NetworkGame *game);
    void requestGameListAsync ();

    void unregisterGame (QUuid guid) const;
    void sealGame (QUuid guid) const;
    void heartBeat () const;

signals:
    void registerGameCompleted (Response response);
    void requestGameListCompleted (GameListResponse response);
    void answerGameList (NetworkGame *list, int size);
    void unregisterGameFailed (QString msg);
    void unregisterGameSuccess ();
    void heartBeat ();

private slots:
    void _registerGameFinished ();
    void _requestGameListFinished ();

private:
    void run ();

    QFutureWatcher<Response> _regGameRespWatcher;
    QFutureWatcher<GameListResponse> _reqGameListRespWatcher;
    int _regGamesCount;
    QString _name;
    QUdpSocket _udpSocket;
    QMutex _mutex;
};

#endif // INDEXSERVICE_H
