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
#include "con4netglobals.h"
#include "indexservice.h"

using namespace std;

IndexService::IndexService (QHostAddress host, quint16 port, QString name)
	: _processing(false), _msgQueue(), _curUnit(), _endpoint(10000, this),
	  _host (host), _port (port), _name (name), _games()
{
	connect(&_endpoint, SIGNAL(sendAndReceiveFinished(bool,Message,QString)),
			this, SLOT(_sendAndReceiveFinished(bool,Message,QString)));
}

IndexService::~IndexService()
{
	for (int i = 0; i < _games.size(); i++)
		delete _games.takeFirst();
}

void IndexService::registerGame(NetworkGame &game)
{
	if (game.hasStarted())
		throw logic_error("Cannot register games that have already started.");

	// register game
	Messages::Vector3 dims;
	dims.w = game.dims().width();
	dims.h = game.dims().height();
	dims.d = game.dims().depth();

	Messages::GameData gameData;
	gameData.dims = dims;
	gameData.gameName = game.name();
	gameData.initiatorName = game.initiatorName();
	gameData.ipAddress = game.ipAddress();
	gameData.port = game.port();
	gameData.state = Messages::Open;

	ProcessingUnit unit;
	unit.msg = Messages::registerGame(gameData);
	unit.type = ProcessingUnit::Register;
	_msgQueue.enqueue(unit);
	_processMsg();
}

void IndexService::unregisterGame (NetworkGame &game)
{
	ProcessingUnit unit;
	unit.msg = Messages::unregisterGame(game.guid());
	unit.type = ProcessingUnit::Unregister;
	_msgQueue.enqueue(unit);
	_processMsg();
}

void IndexService::refreshGameList()
{
	ProcessingUnit unit;
	unit.msg = Messages::requestGameList();
	unit.type = ProcessingUnit::Refresh;
	_msgQueue.enqueue(unit);
	_processMsg();
}

void IndexService::sealGame(NetworkGame &game)
{
	ProcessingUnit unit;
	unit.msg = Messages::sealGame(game.guid());
	unit.type = ProcessingUnit::Seal;
	_msgQueue.enqueue(unit);
	_processMsg();

	/*
	QString msg = QString("seal_game;%1\n").arg(game.guid());
	QStringList respTokens;
	try {
		respTokens = _sendMsg(msg);
	} catch (runtime_error &ex) {
		errMsg = ex.what();
		return false;
	}

	if (respTokens.size() == 0) {
		errMsg = INVALID_RESP_ERR;
		return false;
	}

	if (respTokens[0] == "seal_game_success") return true;
	errMsg = respTokens.size() > 1 ? respTokens[1] : "Unspecified error.";
	return false;
	*/
}

/*
void IndexService::_msgReceived(Message msg)
{
	NetworkString failReason;
	QUuid guid;
	QList<Messages::GameData> gameData;
	if (Messages::parseRegisterSuccess(msg, guid)) {
		emit registerSuccess(guid);
	} else if (Messages::parseRegisterFailed(msg, failReason)) {
		emit registerFailed(failReason);
	} else if (Messages::parseUnregisterSuccess(msg)) {
	} else if (Messages::parseUnregisterFailed(msg, failReason)) {
	} else if (Messages::parseAnswerGameList(msg, gameData)) {
	} else if (Messages::parseSealGameSuccess(msg)) {
	} else if (Messages::parseSealGameFailed(msg, failReason)) {
	}

	_endpoint.disconnectFromHost();
}
*/

void IndexService::_processMsg()
{
	if (_processing || _msgQueue.isEmpty()) return;

	_processing = true;
	_endpoint.connectToServer(_host, _port);
	_curUnit = _msgQueue.dequeue();
	_endpoint.send(_curUnit.msg);
}

void IndexService::_sendAndReceiveFinished(bool success, Message msg,
										   QString failReason)
{
	QString parseErrMsg = "Invalid response msg from server.";

	if (_curUnit.type == ProcessingUnit::Refresh) {
		if (!success) emit refreshGameListFinished(false, failReason);
		else {
			QList<Messages::GameData> gameData;
			if (Messages::parseAnswerGameList(msg, gameData))
				_readGameListAnswer(gameData);
			else emit refreshGameListFinished(false, parseErrMsg);
		}

		QUuid guid;
		NetworkString failMsg;
		if (Messages::parseRegisterSuccess(msg, guid)) {
			emit registerFinished(true, guid, QString());
		} else if (Messages::parseRegisterFailed(msg, failMsg)) {
			emit registerFinished(false, QUuid(), failMsg.string());
		}
	} else if (_curUnit.type == ProcessingUnit::Register) {

	} else if (_curUnit.type == ProcessingUnit::Seal) {

	} else if (_curUnit.type == ProcessingUnit::Unregister) {

	}

	_endpoint.disconnectFromHost();
	_processing = false;
}

void IndexService::_readGameListAnswer(QList<Messages::GameData> &gameData)
{
	NetworkGameConf conf;

	/*
	for (int i = _games.size() - 1; i >=0; i--) {
		NetworkGame *game = _games.at(i);
		if (!game->hasStarted()) {
			_games.removeAt(i);
			delete game;
		}
	}

	QStringList respTokens;
	try {
		respTokens = _sendMsg("request_game_list\n");
	} catch (runtime_error &ex) {
		errMsg = ex.what();
		return false;
	}

	if (respTokens.size() < 2) {
		errMsg = INVALID_RESP_ERR;
		return false;
	}

	int n = (respTokens.size() - 2) / 8;
	for (int i = 0; i < n; i++) {
		if (gameData.at(i).state == Messages::Sealed) continue;

		QString playerName
		QString gameName = respTokens[baseIdx + 2];
		int width = respTokens[baseIdx + 3].toInt();
		int height = respTokens[baseIdx + 4].toInt();
		bool depthOk;
		int depth = (respTokens[baseIdx + 5]).toInt(&depthOk);
		QHostAddress host;
		bool hostOk = host.setAddress(respTokens[baseIdx + 6]);
		quint16 port = (quint16)QString(respTokens[baseIdx +7]).toInt();

		if (width == 0 || height == 0 || !depthOk || !hostOk || port == 0)
			continue;

		// if game exists already, skip
		bool cont = false;
		for (int i = 0; i < _games.size(); i++)
			if ((cont = (_games.at(i)->name() == gameName))) break;
		if (cont) continue;

		Messages::GameData data = gameData.at(i);

		try {
			Game::Dimensions dims(4, data.dims.w, data.dims.h, data.dims.d);
			conf.setDims(dims);
			conf.setInitiatorName(data.initiatorName);
			conf.setIpAddress(data.ipAddress);
			conf.setName(data.gameName);
			conf.setPort(data.port);
			_games.append(new NetworkGame(conf));
		} catch (Game::Dimensions::Exception) {
		} catch (Game::Bo)
	}

	*/
}
