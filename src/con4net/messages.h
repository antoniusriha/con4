/*
 * messages.h
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

#ifndef MESSAGES_H
#define MESSAGES_H

#include "message.h"
#include "networkgame.h"

namespace Messages
{
enum ProtocolVersion
{
	V1,
	V2
};

Message registerGame(NetworkGame &game);
Message registerSuccess(NetworkGame &game);
Message registerFailed(QString &reason);
Message requestGameList();
Message answerGameList(QList<NetworkGame *> &games);
Message unregisterGame(NetworkGame &game);
Message unregisterSuccess();
Message unregisterFailed(QString &reason);
Message joinGame(QString &playerName, NetworkGame &game);
Message joinGameSuccess(ProtocolVersion protocolVersion);
Message joinGameFailed(QString &reason);
Message sealGame(NetworkGame &game);
Message sealGameSuccess();
Message sealGameFailed(QString &reason);
Message startGame();
Message move(NetworkGame &game, int wVal, int hVal, int dVal);
Message synchronizeGameBoard(NetworkGame &game);
Message updateGameBoard(NetworkGame &game, int wVal, int hVal, int dVal,
						FieldValue fieldValue);
Message movedFailed(const QString &reason);
Message endGame(FieldValue winner);
Message abortGame(QString &reason);
Message heartBeat();

QString &sanitize(QString &parameter);
Message failed(const char *header, QString &reason);
int fieldNumber(NetworkGame &game, int wVal, int hVal, int dVal);
}

#endif // MESSAGES_H
