/*
 * indexservice.cpp
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

#include <stdexcept>
#include <QStringList>
#include <QTcpSocket>
#include "con4netglobals.h"
#include "indexservice.h"

using namespace std;

IndexService::IndexService (QHostAddress host, quint16 port, QString name)
    : _host (host), _port (port), _name (name) {}

//void IndexService::run () {
//    forever {
//    }
//}

bool IndexService::registerGame (OpponentService *service, QUuid &guid, QString &errMsg) {
    _oppServices.append (service);
    service->startService ();

    // register game
    Game *game = service->game ();
    QString msg = QString ("register_game;%1;%2;%3;%4;%5;%6;%7\n").arg (service->initiatorName ())
            .arg (service->gameName ()).arg (game->width ()).arg (game->height ())
            .arg (game->depth ()).arg (service->ipAddress ().toString ()).arg (service->port ());

    QStringList respTokens;
    try {
        respTokens = sendMsg (msg);
    } catch (runtime_error &ex) {
        errMsg = ex.what ();
        return false;
    }

    if (respTokens.size () < 2) {
        errMsg = INVALID_RESP_ERR;
        return false;
    }

    QString header = respTokens [0];
    bool success = header == "register_success";

    if (success) {
        guid = QUuid (respTokens [1]);
        if (guid.isNull ()) {
            errMsg = INVALID_RESP_ERR;
            return false;
        }
        return true;
    } else {
        errMsg = respTokens [1];
        return false;
    }
}

//void IndexService::registerGameAsync (NetworkGame *game) {
//    connect (&_regGameRespWatcher, SIGNAL (finished ()), this, SLOT (_registerGameFinished ()));
//    QFuture<Response> future =
//            QtConcurrent::run (this, &IndexService::createGame, game);
//    _regGameRespWatcher.setFuture (future);
//}

//void IndexService::_registerGameFinished () {
//    emit createGameCompleted (_regGameRespWatcher.result ());
//}

bool IndexService::requestGameList (QList<Game *> &list, QString &errMsg) {
    QStringList respTokens;
    try {
        respTokens = sendMsg ("request_game_list\n");
    } catch (runtime_error &ex) {
        errMsg = ex.what ();
        return false;
    }

    if (respTokens.size () < 2) {
        errMsg = INVALID_RESP_ERR;
        return false;
    }

    int n = (respTokens.size () - 2) / 8;
    for (int i = 0; i < n; i++) {
        int baseIdx = i * 8;
        if (respTokens [baseIdx] != "Open")
            continue;

        QString playerName = respTokens [baseIdx + 1];
        QString gameName = respTokens [baseIdx + 2];
        int width = respTokens [baseIdx + 3].toInt ();
        int height = respTokens [baseIdx + 4].toInt ();
        bool depthOk;
        int depth = (respTokens [baseIdx + 5]).toInt (&depthOk);
        QHostAddress host;
        bool hostOk = host.setAddress (respTokens [baseIdx + 6]);
        quint16 port = toQuint16 (respTokens [baseIdx + 7]);

        if (width == 0 || height == 0 || !depthOk || !hostOk || port == 0)
            continue;

        // if game exists already
        bool cont = false;
        for (int i = 0; i < _initServices.size (); i++) {
            InitiatorService *initService = _initServices.at (i);
            if (cont = (initService->gameName () == gameName)) {
                list.append (initService->game ());
                break;
            }
        }
        if (cont) continue;

        // else create new game and init service
		InitiatorService *initService;// =
//                new InitiatorService (playerName, gameName, width, height, depth);
        Game *game = initService->game ();
        QString msg;
        if (game->isConfValid (msg)) {
            _initServices.append (initService);
            list.append (game);
        } else delete initService;
    }

    return true;
}

//void IndexService::requestGameListAsync () {
//    connect (&_reqGameListRespWatcher, SIGNAL (finished ()),
//             this, SLOT (_requestGameListFinished ()));
//    QFuture<GameListResponse> future =
//            QtConcurrent::run (this, &IndexService::requestGameList);
//    _reqGameListRespWatcher.setFuture (future);
//}

//void IndexService::_requestGameListFinished () {
//    emit requestGameListCompleted (_reqGameListRespWatcher.result ());
//}

bool IndexService::unregisterGame (OpponentService *service, QString &errMsg) {
    QString msg = QString ("unregister_game;%1\n").arg (service->guid ());
    QStringList respTokens;
    try {
        respTokens = sendMsg (msg);
    } catch (runtime_error &ex) {
        errMsg = ex.what ();
        return false;
    }

    if (respTokens.size () == 0) {
        errMsg = INVALID_RESP_ERR;
        return false;
    }

    if (respTokens [0] == "unregister_game_success") return true;
    errMsg = respTokens.size () > 1 ? respTokens [1] : "Unspecified error.";
    return false;
}

bool IndexService::sealGame (OpponentService *service, QString &errMsg) {
    QString msg = QString ("seal_game;%1\n").arg (service->guid ());
    QStringList respTokens;
    try {
        respTokens = sendMsg (msg);
    } catch (runtime_error &ex) {
        errMsg = ex.what ();
        return false;
    }

    if (respTokens.size () == 0) {
        errMsg = INVALID_RESP_ERR;
        return false;
    }

    if (respTokens [0] == "seal_game_success") return true;
    errMsg = respTokens.size () > 1 ? respTokens [1] : "Unspecified error.";
    return false;
}

QStringList IndexService::sendMsg (QString msg) {
    QTcpSocket socket;
    socket.connectToHost (_host, _port);
    if (!socket.waitForConnected (5000))
        throw runtime_error ("Error: Could not connect to server.");

    socket.write (msg.toUtf8 ());
    if (!socket.waitForBytesWritten ()) {
        socket.disconnect ();
        throw runtime_error ("Error: Failed to send data to server.");
    }

    if (!socket.waitForReadyRead ()) {
        socket.disconnect ();
        throw runtime_error ("Error: No response from server.");
    }

    char buffer [MSG_BUF_SIZE];
    int bytesRead = socket.read (buffer, MSG_BUF_SIZE - 1);
    socket.disconnect ();
    buffer [bytesRead] = 0;

    QString qsResp = QString::fromUtf8 (buffer);
    return qsResp.split (MSG_SPLIT_CHAR);
}
