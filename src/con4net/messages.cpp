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

Message Messages::registerGame(NetworkGame &game)
{
	Message msg("register_game");
	msg.params()->append(game.initiatorName());
	msg.params()->append(game.name());
	msg.params()->append(QString::number(game.width()));
	msg.params()->append(QString::number(game.height()));
	msg.params()->append(QString::number(game.depth()));
	msg.params()->append(game.ipAddress().toString());
	msg.params()->append(QString::number(game.port()));
	return msg;
}

Message Messages::registerSuccess(NetworkGame &game)
{
	Message msg("register_success");
	msg.params()->append(game.guid().toString());
	return msg;
}

Message Messages::registerFailed(QString &reason)
{
	Message msg("register_failed");
	msg.params()->append(reason);
	return msg;
}

Message Messages::requestGameList()
{
	return Message("request_game_list");
}

Message Messages::answerGameList(QList<NetworkGame *> &games)
{
	Message msg("answer_game_list");
	msg.params()->append(QString::number(games.size()));
	for (int i = 0; i < games.size(); i++) {
		NetworkGame *game = games.at(i);
		if (game->hasStarted()) msg.params()->append("Sealed");
		else msg.params()->append("Open");
		msg.params()->append(game->initiatorName());
		msg.params()->append(game->name());
		msg.params()->append(QString::number(game->width()));
		msg.params()->append(QString::number(game->height()));
		msg.params()->append(QString::number(game->depth()));
		msg.params()->append(game->ipAddress().toString());
		msg.params()->append(QString::number(game->port()));
	}
	return msg;
}

Message Messages::unregisterGame(NetworkGame &game)
{
	Message msg("unregister_game");
	msg.params()->append(game.guid().toString());
	return msg;
}

Message Messages::unregisterSuccess()
{
	return Message("unregister_success");
}

Message Messages::unregisterFailed(QString &reason)
{
	Message msg("unregister_failed");
	msg.params()->append(reason);
	return msg;
}

Message Messages::joinGame(QString &playerName, NetworkGame &game)
{
	Message msg("join_game");
	msg.params()->append(playerName);
	msg.params()->append(game.name());
	msg.params()->append("V2");
	return msg;
}

Message Messages::joinGameSuccess(ProtocolVersion protocolVersion)
{
	Message msg("join_game_success");
	msg.params()->append(protocolVersion == V1 ? "V1" : "V2");
	return msg;
}

Message Messages::joinGameFailed(QString &reason)
{
	Message msg("join_game_failed");
	msg.params()->append(reason);
	return msg;
}

Message Messages::sealGame(NetworkGame &game)
{
	Message msg("seal_game");
	msg.params()->append(game.guid().toString());
	return msg;
}

Message Messages::sealGameSuccess()
{
	return Message("seal_game_success");
}

Message Messages::sealGameFailed(QString &reason)
{
	Message msg("seal_game_failed");
	msg.params()->append(reason);
	return msg;
}

Message Messages::startGame()
{
	return Message("start_game");
}

Message Messages::move(NetworkGame &game, int wVal, int hVal, int dVal)
{
	Message msg("move");
	msg.params()->append(QString::number(fieldNumber(game, wVal, hVal, dVal)));
	return msg;
}

Message Messages::synchronizeGameBoard(NetworkGame &game)
{
	Message msg("synchronize_game_board");
	msg.params()->append(QString::number(
							 game.width() * game.height() * game.depth()));
	for (int i = 0; i < game.height(); i++) {
		for (int j = 0; j < game.width(); j++) {
			for (int k = 0; k < game.depth(); k++) {
				msg.params()->append(
							QString::number(fieldNumber(game, j, i, k)));
				msg.params()->append(QString::number(game.get(j, i, k)));
			}
		}
	}
	return msg;
}

Message Messages::updateGameBoard(NetworkGame &game, int wVal, int hVal,
								  int dVal, FieldValue fieldValue)
{
	Message msg("update_game_board");
	msg.params()->append(QString::number(fieldNumber(game, wVal, hVal, dVal)));
	msg.params()->append(QString::number(fieldValue));
	return msg;
}

Message Messages::movedFailed(const QString &reason)
{
	Message msg("moved_failed");
	msg.params()->append(reason);
	return msg;
}

Message Messages::endGame(FieldValue winner)
{
	Message msg("end_game");
	msg.params()->append(QString::number(winner));
	return msg;
}

Message Messages::abortGame(QString &reason)
{
	Message msg("abort_game");
	msg.params()->append(reason);
	return msg;
}

Message Messages::heartBeat()
{
	return Message("heartbeat");
}

QString &Messages::sanitize(QString &parameter)
{
	parameter.remove('\n');
	parameter.remove(';');
}

Message Messages::failed(const char *header, QString &reason)
{
	Message msg(header);
	msg.params()->append(sanitize(reason));
	return msg;
}

int Messages::fieldNumber(NetworkGame &game, int wVal, int hVal, int dVal)
{
	return dVal * game.width() * game.height() + hVal * game.width() + wVal;
}
