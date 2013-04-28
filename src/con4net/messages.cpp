/*
 * messages.cpp
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

#include "messages.h"

Message Messages::registerGame(GameData game)
{
	Message msg(NetworkString("register_game"));
	msg.params()->append(game.initiatorName);
	msg.params()->append(game.gameName);
	msg.params()->append(QString::number(game.dims.w));
	msg.params()->append(QString::number(game.dims.h));
	msg.params()->append(QString::number(game.dims.d));
	msg.params()->append(game.ipAddress.toString());
	msg.params()->append(QString::number(game.port));
	return msg;
}

Message Messages::registerSuccess(QUuid guid)
{
	Message msg(NetworkString("register_success"));
	msg.params()->append(guid.toString());
	return msg;
}

Message Messages::registerFailed(NetworkString reason)
{
	return failed("register_failed", reason);
}

Message Messages::unregisterGame(QUuid guid)
{
	Message msg(NetworkString("unregister_game"));
	msg.params()->append(guid.toString());
	return msg;
}

Message Messages::unregisterSuccess()
{
	return Message(NetworkString("unregister_success"));
}

Message Messages::unregisterFailed(NetworkString reason)
{
	return failed("unregister_failed", reason);
}

Message Messages::requestGameList()
{
	return Message(NetworkString("request_game_list"));
}

Message Messages::answerGameList(QList<GameData> gameData)
{
	Message msg(NetworkString("answer_game_list"));
	msg.params()->append(QString::number(gameData.size()));
	for (int i = 0; i < gameData.size(); i++) {
		GameData game = gameData.at(i);
		if (game.state == Sealed) msg.params()->append("Sealed");
		else msg.params()->append("Open");
		msg.params()->append(game.initiatorName);
		msg.params()->append(game.gameName);
		msg.params()->append(QString::number(game.dims.w));
		msg.params()->append(QString::number(game.dims.h));
		msg.params()->append(QString::number(game.dims.d));
		msg.params()->append(game.ipAddress.toString());
		msg.params()->append(QString::number(game.port));
	}
	return msg;
}

Message Messages::sealGame(QUuid guid)
{
	Message msg(NetworkString("seal_game"));
	msg.params()->append(guid.toString());
	return msg;
}

Message Messages::sealGameSuccess()
{
	return Message(NetworkString("seal_game_success"));
}

Message Messages::sealGameFailed(NetworkString reason)
{
	return failed("seal_game_failed", reason);
}

Message Messages::joinGame(NetworkString playerName, NetworkString gameName)
{
	Message msg(NetworkString("join_game"));
	msg.params()->append(playerName);
	msg.params()->append(gameName);
	msg.params()->append("V2");
	return msg;
}

Message Messages::joinGameSuccess(ProtocolVersion protocolVersion)
{
	Message msg(NetworkString("join_game_success"));
	msg.params()->append(protocolVersion == V1 ? "V1" : "V2");
	return msg;
}

Message Messages::joinGameFailed(NetworkString reason)
{
	return failed("join_game_failed", reason);
}

Message Messages::startGame()
{
	return Message(NetworkString("start_game"));
}

Message Messages::move(Vector3 dims, Vector3 vals)
{
	Message msg(NetworkString("move"));
	msg.params()->append(QString::number(fieldNumber(dims, vals)));
	return msg;
}

Message Messages::synchronizeGameBoard(Vector3 dims, QList<Field> fields)
{
	Message msg(NetworkString("synchronize_game_board"));
	msg.params()->append(QString::number(dims.w * dims.h * dims.d));
	for (int i = 0; i < fields.size(); i++) {
		Field f = fields.at(i);
		int fn = fieldNumber(dims, f.index);
		msg.params()->append(QString::number(fn));
		msg.params()->append(QString::number((int)f.state));
	}
	return msg;
}

Message Messages::updateGameBoard(Vector3 dims, Vector3 vals,
								  FieldState fieldState)
{
	Message msg(NetworkString("update_game_board"));
	msg.params()->append(QString::number(fieldNumber(dims, vals)));
	msg.params()->append(QString::number((int)fieldState));
	return msg;
}

Message Messages::movedFailed(NetworkString reason)
{
	return failed("moved_failed", reason);
}

Message Messages::endGame(FieldState winner)
{
	Message msg(NetworkString("end_game"));
	msg.params()->append(QString::number((int)winner));
	return msg;
}

Message Messages::abortGame(NetworkString reason)
{
	return failed("abort_game", reason);
}

Message Messages::heartBeat()
{
	return Message(NetworkString("heartbeat"));
}

Message Messages::failed(NetworkString header, NetworkString reason)
{
	Message msg(header);
	msg.params()->append(reason);
	return msg;
}

int Messages::fieldNumber(Vector3 dims, Vector3 vals)
{
	return vals.d * dims.w * dims.h + vals.h * dims.w + vals.w;
}
