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

#include <QUuid>
#include <QHostAddress>
#include "message.h"

namespace Messages
{

const QString InvalidRespErr = "Error: Invalid response from server.";

enum ProtocolVersion { V1, V2 };

enum GameState { Open, Sealed };

struct Vector3 { uint w, h, d; };

struct GameData
{
	GameState state;
	NetworkString initiatorName, gameName;
	Vector3 dims;
	QHostAddress ipAddress;
	quint16 port;
};

Message registerGame(GameData game);
bool parseRegisterGame(Message msg, GameData &gameData);

Message registerSuccess(QUuid guid);
bool parseRegisterSuccess(Message msg, QUuid &guid);

Message registerFailed(NetworkString reason);
bool parseRegisterFailed(Message msg, NetworkString &reason);

Message unregisterGame(QUuid guid);
bool parseUnregisterGame(Message msg, QUuid &guid);

Message unregisterSuccess();
bool parseUnregisterSuccess(Message msg);

Message unregisterFailed(NetworkString reason);
bool parseUnregisterFailed(Message msg, NetworkString &reason);

Message requestGameList();
bool parseRequestGameList(Message msg);

Message answerGameList(QList<GameData> gameData);
bool parseAnswerGameList(Message msg, QList<GameData> &gameData);

Message sealGame(QUuid guid);
bool parseSealGame(Message msg, QUuid &guid);

Message sealGameSuccess();
bool parseSealGameSuccess(Message msg);

Message sealGameFailed(NetworkString reason);
bool parseSealGameFailed(Message msg, NetworkString &reason);

Message joinGame(NetworkString playerName, NetworkString gameName);
bool parseJoinGame(Message msg, NetworkString &playerName,
				   NetworkString &gameName);

Message joinGameSuccess(ProtocolVersion protocolVersion);
bool parseJoinGameSuccess(Message msg, ProtocolVersion &protocolVersion);

Message joinGameFailed(NetworkString reason);
bool parseJoinGameFailed(Message msg, NetworkString &reason);

Message startGame();
bool parseStartGame(Message msg);

Message move(Vector3 dims, Vector3 vals);
bool parseMove(Message msg, Vector3 &dims, Vector3 &index);

enum FieldState { None, Player1, Player2 };
struct Field { Vector3 index; FieldState state; };
Message synchronizeGameBoard(Vector3 dims, QList<Field> fields);
bool parseSynchronizeGameBoard(Message msg, Vector3 dims, QList<Field> &fields);

Message updateGameBoard(Vector3 dims, Vector3 vals, FieldState fieldState);
bool parseUpdateGameBoard(Message msg, Vector3 dims, Vector3 &vals,
						  FieldState &fieldState);

Message movedFailed(NetworkString reason);
bool parseMovedFailed(Message msg, NetworkString &reason);

Message endGame(FieldState winner);
bool parseEndGame(Message msg, FieldState &winner);

Message abortGame(NetworkString reason);
bool parseAbortGame(Message msg, NetworkString &reason);

Message heartBeat();
bool parseHeartBeat(Message msg);

Message failed(NetworkString header, NetworkString reason);
bool parseFailed(Message msg, NetworkString header, NetworkString &reason);
int toFieldNumber(Vector3 dims, Vector3 vals);
bool toVals(Vector3 dims, int fieldNumber, Vector3 &vals);

}

#endif // MESSAGES_H
