/*
 * NetInitiator.cpp
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

#include "netinitiator.h"
#include "messages.h"

NetInitiator::NetInitiator(NetInitiatorConf conf, QObject *parent)
	: QObject(parent), _game(conf),
	  _manager(*conf.indexServiceList(), _game, this), _endpoint(10000, this)
{
	if (!_endpoint.listen(_game.port())) {
		throw Exception(QString("Unable to listen on port %1.")
						.arg(_game.port()));
	}

	connect(&_manager,
			SIGNAL(registerGameFinished(bool,QList<ErroneousService>)),
			this, SIGNAL(registerGameFinished(bool,QList<ErroneousService>)));
	connect(&_manager, SIGNAL(sealGameFinished(bool,QList<ErroneousService>)),
			this, SIGNAL(sealGameFinished(bool,QList<ErroneousService>)));
	connect(&_manager,
			SIGNAL(unregisterGameFinished(bool,QList<ErroneousService>)),
			this, SIGNAL(unregisterGameFinished(bool,QList<ErroneousService>)));
}

void NetInitiator::registerGame() { _manager.registerGame(); }

void NetInitiator::acceptJoinRequest()
{
	// seal game
	// TODO

	// join success
	SendRequest *req = new SendRequest(Messages::joinGameSuccess(Messages::V2));
	connect(req, SIGNAL(finished(Request*)), req, SLOT(deleteLater()));
	_endpoint.send(*req);

	// start game
	_game.start();
	if (_game.curPlayer() == Player2) {
		SendRequest *startReq = new SendRequest(Messages::startGame());
		connect(startReq, SIGNAL(finished(Request*)),
				startReq, SLOT(deleteLater()));
		_endpoint.send(*startReq);
	}
}

void NetInitiator::rejectJoinRequest(QString reason)
{
	SendRequest *req = new SendRequest(Messages::joinGameFailed(reason));
	connect(req, SIGNAL(finished(Request*)), req, SLOT(deleteLater()));
	_endpoint.send(*req);
}

/*
void NetInitiator::_set(FieldValue player, int width, int depth)
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

void NetInitiator::_handleMsg(QStringList msgTokens)
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

void NetInitiator::_messageReceived(Message msg)
{
	if (!msg.isValid()) return;

	if (!game()->hasStarted()) {
		// expect join request
//		if (!msg.header() ==)
	} else {

	}
}
*/
