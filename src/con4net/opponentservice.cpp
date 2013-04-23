/*
 * opponentservice.cpp
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
#include <QTcpSocket>
#include "con4netglobals.h"
#include "opponentservice.h"
#include "messages.h"

using namespace std;

OpponentService::OpponentService(IndexServiceList *indexServices, QObject *parent)
	: NetworkPlayerService(new NetworkGame(width, height, depth, gameName,
										   initiatorName, ipAddress, port),
						   parent), _indexServices(indexServices),
	  _endpoint(*new ServerEndpoint(10000, this))
{
	connect(networkGame(), SIGNAL(_set(FieldValue,int,int)), this,
			SLOT(_set(FieldValue, int, int)));
}

OpponentService::~OpponentService() { delete networkGame(); }

NetworkGame *OpponentService::createGame(int width, int height, int depth,
										 QString initiatorName,
										 QString gameName,
										 QHostAddress ipAddress, quint16 port)
{

}

bool OpponentService::startService(QString *errMsg)
{
	if (!_endpoint.listen(networkGame()->port())) {
		if (errMsg) *errMsg = QString("Unable to listen on port ")
				.arg(networkGame()->port());
		return false;
	}

	bool registerSuccess = false;
	QString finalErrMsg;
	for (int i = 0; i < _indexServices->size(); i++) {
		QString errMsg;
		if (!_indexServices->at(i)->registerGame(*networkGame(), errMsg)) {
			finalErrMsg.append(errMsg + "\n");
			break;
		}
		registerSuccess = true;
	}

	if (!registerSuccess) {
		if (errMsg) *errMsg = QString("Unable to register game.")
				.append(finalErrMsg);
		return false;
	}

	return true;
}

void OpponentService::acceptJoinRequest()
{
	// seal game
	QString errMsg;
	for (int i = 0; i < _indexServices->size(); i++)
		_indexServices->at(i)->sealGame(*networkGame(), errMsg);

	// join success
	_endpoint.sendMessage(Messages::joinGameSuccess(Messages::V2));

	// start game
	networkGame()->start();
	if (networkGame()->curPlayer() == Player2)
		_endpoint.sendMessage(Messages::startGame());
}

void OpponentService::rejectJoinRequest(QString reason)
{
	_endpoint.sendMessage(Messages::joinGameFailed(reason));
}

void OpponentService::_set(FieldValue player, int width, int depth)
{
	if (player == Player1) {
		int height;
		game()->full(width, depth, height);

		_endpoint.sendMessage(Messages::updateGameBoard(*networkGame(), width,
														height, depth, player));
		if (game()->finished()) {
			if (game()->isDraw())
				_endpoint.sendMessage(Messages::endGame(None));
			else _endpoint.sendMessage(Messages::endGame(Player1));
			_endpoint.disconnectFromHost();
			for (int i = 0; i < _indexServices->size(); i++) {
				QString errMsg;
				_indexServices->at(i)->unregisterGame(*networkGame(), errMsg);
			}
		}
	}
}

void OpponentService::_handleMsg(QStringList msgTokens)
{
	if (msgTokens.empty()) return;
	QString header = msgTokens.at(0);

	if (!game()->hasStarted()) {
		// expect join request
		if (header != "join_game") return;
		if (msgTokens.size() < 4) return;
		if (msgTokens.at(2) != networkGame()->name()) return;
		emit joinGame(msgTokens.at(1));
	} else {
		// expect: move, abort
		if (header == "move") {
			if (msgTokens.size() < 2) return;
			int x = msgTokens.at(1).toInt();
			div_t r = div(x, game()->width() * game()->height());
			int xd = r.quot;
			r = div(r.rem, game()->width());
			if (!networkGame()->set(r.rem, xd))
				_endpoint.sendMessage(Messages::movedFailed("Invalid move"));
			else {
				if (game()->finished()) {
					if (game()->isDraw())
						_endpoint.sendMessage(Messages::endGame(None));
					else _endpoint.sendMessage(Messages::endGame(Player2));
					_endpoint.disconnectFromHost();
					for (int i = 0; i < _indexServices->size(); i++) {
						QString errMsg;
						_indexServices->at(i)->unregisterGame(*networkGame(),
															  errMsg);
					}
				}
			}
		}
	}
}

void OpponentService::_messageReceived(Message msg)
{
	if (!msg.isValid()) return;

	if (!game()->hasStarted()) {
		// expect join request
//		if (!msg.header() ==)
	} else {

	}
}
