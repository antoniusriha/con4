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

#include "indexservice.h"

using namespace std;

IndexService::IndexService (QHostAddress host, quint16 port, QString name)
    : Service (host, port), _name (name) {

}

void IndexService::run () {
    forever {

    }
}

GameResponse IndexService::createGame (QString initiator, QString gameName,
                                       int width, int height, int depth) {


//    NetworkGame *game;// = new NetworkGame (gameName, initiator, )
//    QString msg = QString ("register_game;%1;%2;%3;%4;%5;%6;%7\n").arg (game->player1 ())
//            .arg (game->name ()).arg (game->width ()).arg (game->height ())
//            .arg (game->depth ()).arg (game->host ().toString ()).arg (game->port ());

//    Response resp;
//    QStringList respTokens;
//    try {
//        respTokens = sendMsg (msg);
//    } catch (runtime_error &ex) {
//        return fail (resp, ex.what ());
//    }

//    if (respTokens.size () < 2) return fail (resp, INVALID_RESP_ERR);

//    QString header = respTokens [0];
//    bool success = header == "register_success";

//    QUuid guid;
//    if (success) {
//        guid = QUuid (respTokens [1]);
//        if (guid.isNull ()) return fail (resp, INVALID_RESP_ERR);
//        game->setGuid (guid);
//        resp.Success = true;
//        return resp;
//    } else return fail (resp, respTokens [1]);
}

void IndexService::registerGameAsync (NetworkGame *game) {
//    connect (&_regGameRespWatcher, SIGNAL (finished ()), this, SLOT (_registerGameFinished ()));
//    QFuture<Response> future =
//            QtConcurrent::run (this, &IndexService::createGame, game);
//    _regGameRespWatcher.setFuture (future);
}

void IndexService::_registerGameFinished () {
    emit createGameCompleted (_regGameRespWatcher.result ());
}

GameListResponse IndexService::requestGameList () {
//    GameListResponse resp;
//    QStringList respTokens;
//    try {
//        respTokens = sendMsg ("request_game_list\n");
//    } catch (runtime_error &ex) {
//        return (GameListResponse &)fail (resp, QString (ex.what ()));
//    }

//    if (respTokens.size () < 2)
//        return (GameListResponse &)fail (resp, INVALID_RESP_ERR);
//    int n = (respTokens.size () - 2) / 8;
//    QList<NetworkGame *> gameList;
//    for (int i = 0; i < n; i++) {
//        int baseIdx = i * 8;
//        bool open = respTokens [baseIdx] == "Open";
//        QString playerName = respTokens [baseIdx + 1];
//        QString gameName = respTokens [baseIdx + 2];
//        int width = respTokens [baseIdx + 3].toInt ();
//        int height = respTokens [baseIdx + 4].toInt ();
//        bool depthOk;
//        int depth = (respTokens [baseIdx + 5]).toInt (&depthOk);
//        QHostAddress host;
//        bool hostOk = host.setAddress (respTokens [baseIdx + 6]);
//        quint16 port = toQuint16 (respTokens [baseIdx + 7]);

//        if (width == 0 || height == 0 || !depthOk || !hostOk || port == 0)
//            continue;

//        NetworkGame *game = new NetworkGame (gameName, playerName, host,
//                                             port, width, height, depth, !open);
//        if (game->areSettingsValid ()) gameList.append (game);
//        else delete game;
//    }

//    resp.Games = gameList;
//    resp.Success = true;
//    return resp;
}

void IndexService::requestGameListAsync () {
    connect (&_reqGameListRespWatcher, SIGNAL (finished ()),
             this, SLOT (_requestGameListFinished ()));
    QFuture<GameListResponse> future =
            QtConcurrent::run (this, &IndexService::requestGameList);
    _reqGameListRespWatcher.setFuture (future);
}

void IndexService::_requestGameListFinished () {
    emit requestGameListCompleted (_reqGameListRespWatcher.result ());
}

//Response IndexService::unregisterGame (NetworkGame *game) {
//    QString msg = QString ("unregister_game;%1\n").arg (game->guid ());
//    Response resp;
//    QStringList respTokens;
//    try {
//        respTokens = sendMsg (msg);
//    } catch (runtime_error &ex) {
//        return fail (resp, ex.what ());
//    }
//    if (respTokens.size () == 0) return fail (resp, INVALID_RESP_ERR);

//    if (respTokes [0] == "unregister_game_success") {
//        resp.Success = true;
//        return resp;
//    }

//    return fail (resp, respTokens.size () > 1 ?
//                     respTokens [1] : "Unspecified error.");
//}

Response IndexService::sealGame (NetworkGame *game) {
    QString msg = QString ("seal_game;%1\n").arg (game->guid ());
    Response resp;
    QStringList respTokens;
    try {
        respTokens = sendMsg (msg);
    } catch (runtime_error &ex) {
        return fail (resp, ex.what ());
    }
    if (respTokens.size () == 0) return fail (resp, INVALID_RESP_ERR);

    if (respTokens [0] == "seal_game_success") {
        resp.Success = true;
        return resp;
    }

    return fail (resp, respTokens.size () > 1 ?
                     respTokens [1] : "Unspecified error.");
}
