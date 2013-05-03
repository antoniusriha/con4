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

bool Messages::parseRegisterSuccess(Message msg, QUuid &guid)
{
	if (msg.header().string() != "register_success" ||
		msg.params()->size() < 1) return false;
	guid = QUuid(msg.params()->at(0).string());
	return !guid.isNull();
}

Message Messages::registerFailed(NetworkString reason)
{
	return failed("register_failed", reason);
}

bool Messages::parseRegisterFailed(Message msg, NetworkString &reason)
{
	return parseFailed(msg, "register_failed", reason);
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

bool Messages::parseUnregisterSuccess(Message msg)
{
	return msg.header().string() == "unregister_success";
}

Message Messages::unregisterFailed(NetworkString reason)
{
	return failed("unregister_failed", reason);
}

bool Messages::parseUnregisterFailed(Message msg, NetworkString &reason)
{
	return parseFailed(msg, "unregister_failed", reason);
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

bool Messages::parseAnswerGameList(Message msg, QList<GameData> &gameData)
{
	if (msg.header().string() != "answer_game_list" ||
		msg.params()->size() < 1) return false;

	quint16 port;
	QHostAddress ipAddr;
	Vector3 dims;
	GameData data;
	int baseIdx;
	int n = (msg.params()->size() - 1) / 8;
	for (int i = 0; i < n; i++) {
		baseIdx = i * 8 + 1;
		dims.w = msg.params()->at(baseIdx + 3).string().toUInt();
		dims.h = msg.params()->at(baseIdx + 4).string().toUInt();
		dims.d = msg.params()->at(baseIdx + 5).string().toUInt();
		port = (quint16)msg.params()->at(baseIdx + 7).string().toInt();

		if (dims.w == 0 || dims.h == 0 || dims.d == 0 || port == 0 ||
			!ipAddr.setAddress(msg.params()->at(baseIdx + 6).string()))
			continue;

		data.state = msg.params()->at(baseIdx).string().toLower() == "open" ?
					Messages::Open : Messages::Sealed;
		data.initiatorName = msg.params()->at(baseIdx + 1);
		data.gameName = msg.params()->at(baseIdx + 2);
		data.dims = dims;
		data.ipAddress = ipAddr;
		data.port = port;
		gameData.append(data);
	}
	return true;
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

bool Messages::parseSealGameSuccess(Message msg)
{
	return msg.header().string() == "seal_game_success";
}

Message Messages::sealGameFailed(NetworkString reason)
{
	return failed("seal_game_failed", reason);
}

bool Messages::parseSealGameFailed(Message msg, NetworkString &reason)
{
	return parseFailed(msg, "seal_game_failed", reason);
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

bool Messages::parseJoinGameSuccess(Message msg,
									ProtocolVersion &protocolVersion)
{
	if (msg.header().string() != "join_game_success" ||
		msg.params()->size() < 1) return false;
	QString pVersion = msg.params()->at(0).string().toLower();

	if (pVersion == "v1") protocolVersion = V1;
	else if (pVersion == "v2") protocolVersion = V2;
	else return false;
	return true;
}

Message Messages::joinGameFailed(NetworkString reason)
{
	return failed("join_game_failed", reason);
}

bool Messages::parseJoinGameFailed(Message msg, NetworkString &reason)
{
	return parseFailed(msg, "join_game_failed", reason);
}

Message Messages::startGame()
{
	return Message(NetworkString("start_game"));
}

bool Messages::parseStartGame(Message msg)
{
	return msg.header().string() == "start_game";
}

Message Messages::move(Vector3 dims, Vector3 vals)
{
	Message msg(NetworkString("move"));
	msg.params()->append(QString::number(toFieldNumber(dims, vals)));
	return msg;
}

Message Messages::synchronizeGameBoard(Vector3 dims, QList<Field> fields)
{
	Message msg(NetworkString("synchronize_game_board"));
	msg.params()->append(QString::number(dims.w * dims.h * dims.d));
	for (int i = 0; i < fields.size(); i++) {
		Field f = fields.at(i);
		int fn = toFieldNumber(dims, f.index);
		msg.params()->append(QString::number(fn));
		msg.params()->append(QString::number((int)f.state));
	}
	return msg;
}

bool Messages::parseSynchronizeGameBoard(Message msg, Vector3 dims,
										 QList<Field> &fields)
{
	if (msg.header().string() != "synchronize_game_board") return false;
	int nFields = dims.w * dims.h * dims.d;
	if (msg.params()->size() - 1 != nFields * 2) return false;

	Field field;
	for (int i = 1; i < msg.params()->size(); i += 2) {
		bool ok;
		int fn = msg.params()->at(i).string().toInt(&ok);
		if (!ok) return false;

		int fs = msg.params()->at(i + 1).string().toInt(&ok);
		if (!ok || fs < 0 || fs > 2) return false;

		if (!toVals(dims, fn, field.index)) return false;
		field.state = (FieldState)fs;
		fields.append(field);
	}
	return true;
}

Message Messages::updateGameBoard(Vector3 dims, Vector3 vals,
								  FieldState fieldState)
{
	Message msg(NetworkString("update_game_board"));
	msg.params()->append(QString::number(toFieldNumber(dims, vals)));
	msg.params()->append(QString::number((int)fieldState));
	return msg;
}

bool Messages::parseUpdateGameBoard(Message msg, Vector3 dims, Vector3 &vals,
									FieldState &fieldState)
{
	if (msg.header().string() != "update_game_board" ||
		msg.params()->size() < 2) return false;

	bool ok;
	int fn = msg.params()->at(0).string().toInt(&ok);
	if (!ok) return false;

	int fs = msg.params()->at(1).string().toInt(&ok);
	if (!ok || fs < 0 || fs > 2) return false;

	if (!toVals(dims, fn, vals)) return false;
	fieldState = (FieldState)fs;
	return true;
}

Message Messages::movedFailed(NetworkString reason)
{
	return failed("moved_failed", reason);
}

bool Messages::parseMovedFailed(Message msg, NetworkString &reason)
{
	return parseFailed(msg, "moved_failed", reason);
}

Message Messages::endGame(FieldState winner)
{
	Message msg(NetworkString("end_game"));
	msg.params()->append(QString::number((int)winner));
	return msg;
}

bool Messages::parseEndGame(Message msg, FieldState &winner)
{
	if (msg.header().string() != "end_game" || msg.params()->size() < 1)
		return false;

	bool ok;
	int fs = msg.params()->at(1).string().toInt(&ok);
	if (!ok || fs < 0 || fs > 2) return false;
	winner = (FieldState)fs;
	return true;
}

Message Messages::abortGame(NetworkString reason)
{
	return failed("abort_game", reason);
}

bool Messages::parseAbortGame(Message msg, NetworkString &reason)
{
	return parseFailed(msg, "abort_game", reason);
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

bool Messages::parseFailed(Message msg, NetworkString header,
						   NetworkString &reason)
{
	if (msg.header().string() != header.string() ||
		msg.params()->size() < 1) return false;
	reason = msg.params()->at(0);
	return true;
}

int Messages::toFieldNumber(Vector3 dims, Vector3 vals)
{
	return vals.d * dims.w * dims.h + vals.h * dims.w + vals.w;
}

bool Messages::toVals(Vector3 dims, int fieldNumber, Vector3 &vals)
{
	if (dims.w == 0 || dims.h == 0) return false;
	div_t r = div(fieldNumber, dims.w * dims.h);
	vals.d = r.quot;
	r = div(r.rem, dims.w);
	vals.h = r.quot;
	vals.w = r.rem;
	return true;
}
