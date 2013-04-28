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

/*
using namespace std;

OpponentServiceConf::OpponentServiceConf(QList<IndexService *> indexServices)
	: _networkGameConf()
{
	setIndexServices(indexServices);
}

void OpponentServiceConf::setIndexServices(QList<IndexService *> value)
{
	if (value.empty()) throw invalid_argument(
				"value must be a non-empty list of index services.");
	_indexServices = value;
}

OpponentService::OpponentService(OpponentServiceConf conf, QObject *parent)
	: NetworkPlayerService(new NetworkGame(conf.networkGameConf()), parent),
	  _indexServices(conf.indexServices()),
	  _endpoint(*new ServerEndpoint(10000, this))
{
	connect(game(), SIGNAL(_set(FieldValue,int,int)),
			this, SLOT(_set(FieldValue, int, int)));
}

OpponentService::~OpponentService() { delete game(); }

bool OpponentService::startService(QString *errMsg)
{
	if (!_endpoint.listen(game()->port())) {
		if (errMsg) *errMsg = QString("Unable to listen on port ")
				.arg(game()->port());
		return false;
	}

	bool registerSuccess = false;
	QString finalErrMsg;
	for (int i = 0; i < _indexServices.size(); i++) {
		QString errMsg;
		if (!_indexServices.at(i)->registerGame(*game())) {
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
	for (int i = 0; i < _indexServices.size(); i++)
		_indexServices.at(i)->sealGame(*game());

	// join success
	_endpoint.send(Messages::joinGameSuccess(Messages::V2));

	// start game
	game()->start();
	if (game()->curPlayer() == Player2)
		_endpoint.send(Messages::startGame());
}

void OpponentService::rejectJoinRequest(QString reason)
{
	_endpoint.send(Messages::joinGameFailed(reason));
}

void OpponentService::_set(FieldValue player, int width, int depth)
{
	if (player == Player1) {
		Game::BoardIndex idx = game()->index(width, depth);
		game()->full(idx);

		_endpoint.send(Messages::updateGameBoard(*game(), width,
												 height, depth, player));
		if (game()->finished()) {
			if (game()->isDraw())
				_endpoint.send(Messages::endGame(None));
			else _endpoint.send(Messages::endGame(Player1));
			_endpoint.disconnectFromHost();
			for (int i = 0; i < _indexServices.size(); i++) {
				QString errMsg;
				_indexServices.at(i)->unregisterGame(*game(), errMsg);
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
		if (msgTokens.at(2) != game()->name()) return;
		emit joinGame(msgTokens.at(1));
	} else {
		// expect: move, abort
		if (header == "move") {
			if (msgTokens.size() < 2) return;
			int x = msgTokens.at(1).toInt();
			div_t r = div(x, game()->width() * game()->height());
			int xd = r.quot;
			r = div(r.rem, game()->width());
			if (!game()->set(r.rem, xd))
				_endpoint.send(Messages::movedFailed("Invalid move"));
			else {
				if (game()->finished()) {
					if (game()->isDraw())
						_endpoint.send(Messages::endGame(None));
					else _endpoint.send(Messages::endGame(Player2));
					_endpoint.disconnectFromHost();
					for (int i = 0; i < _indexServices.size(); i++) {
						QString errMsg;
						_indexServices.at(i)->unregisterGame(*game(), errMsg);
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
*/
