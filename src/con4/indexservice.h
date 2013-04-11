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

#include <QUuid>
#include <QMutex>
#include <QHostAddress>
#include <QUdpSocket>

#include "service.h"
#include "opponentservice.h"
#include "initiatorservice.h"

class IndexService : public Service {
    Q_OBJECT
public:
    IndexService (QHostAddress host, quint16 port, QString name);

    QString name () const { return _name; }
    QHostAddress ipAddress () const { return _host; }
    quint16 port () const { return _port; }

    bool requestGameList (QList<Game *> &list, QString &errMsg);
    bool registerGame (OpponentService *service, QUuid &guid, QString &errMsg);
    bool unregisterGame (OpponentService *service, QString &errMsg);
    bool sealGame (OpponentService *service, QString &errMsg);

//    void registerGameAsync (NetworkGame *game);
//    void requestGameListAsync ();

signals:
//    void registerGameCompleted (GameResponse response);
//    void requestGameListCompleted (GameListResponse response);

private slots:
//    void _registerGameFinished ();
//    void _requestGameListFinished ();

private:
//    void run ();

//    QFutureWatcher<GameResponse> _regGameRespWatcher;
//    QFutureWatcher<GameListResponse> _reqGameListRespWatcher;

    QStringList sendMsg (QString msg);

    QHostAddress _host;
    quint16 _port;
    QString _name;

    QList<OpponentService *> _oppServices;
    QList<InitiatorService *> _initServices;

    QUdpSocket _udpSocket;
    QMutex _mutex;
};

#endif // INDEXSERVICE_H
