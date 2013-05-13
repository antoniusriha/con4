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

	connect(&_endpoint, SIGNAL(messageReceived(Message)),
			this, SLOT(_messageReceived(Message)));

	connect(&_manager,
			SIGNAL(registerGameFinished(bool,QList<ErroneousService>)),
			this, SLOT(_registerGameFinished(bool,QList<ErroneousService>)));
	connect(&_manager, SIGNAL(sealGameFinished(bool,QList<ErroneousService>)),
			this, SLOT(_managerFinished(bool,QList<ErroneousService>)));
	connect(&_manager,
			SIGNAL(unregisterGameFinished(bool,QList<ErroneousService>)),
			this, SLOT(_managerFinished(bool,QList<ErroneousService>)));

	connect(&_game, SIGNAL(started(FieldValue)),
			this, SLOT(_started(FieldValue)));
	connect(&_game, SIGNAL(set(FieldValue,Game::BoardIndex)),
			this, SLOT(_set(FieldValue,Game::BoardIndex)));
	connect(&_game, SIGNAL(finished(FieldValue)),
			this, SLOT(_finished(FieldValue)));
	connect(&_game, SIGNAL(aborted(FieldValue,QString)),
			this, SLOT(_aborted(FieldValue,QString)));
}

void NetInitiator::registerGame() { _manager.registerGame(); }

void NetInitiator::_registerGameFinished(bool success,
										 QList<ErroneousService> errServices)
{
	if (!success) emit error(_compileErrMsg(errServices));
}

void NetInitiator::acceptJoinRequest()
{
	SendRequest *req = new SendRequest(Messages::joinGameSuccess(Messages::V2));
	connect(req, SIGNAL(finished(Request*)),
			this, SLOT(_joinSuccessFinished(Request*)));
	_endpoint.send(*req);
}

void NetInitiator::rejectJoinRequest(QString reason)
{
	SendRequest *req = new SendRequest(Messages::joinGameFailed(reason));
	connect(req, SIGNAL(finished(Request*)), req, SLOT(deleteLater()));
	_endpoint.send(*req);
	_disconnect();
}

void NetInitiator::_joinSuccessFinished(Request *request)
{
	if (request->success()) _manager.sealGame();
	else {
		emit error(request->errorString());
		_game.abort(Player1, request->errorString());
	}
	delete request;
}

void NetInitiator::_managerFinished(bool success,
									QList<ErroneousService> errServices)
{
	if (success) _game.start();
	else {
		QString errMsg = _compileErrMsg(errServices);
		emit error(errMsg);
		_game.abort(Player1, errMsg);
	}
}

void NetInitiator::_started(FieldValue startPlayer)
{
	if (startPlayer == Player2) {
		SendRequest *req = new SendRequest(Messages::startGame(), this);
		connect(req, SIGNAL(finished(Request*)),
				this, SLOT(_reqFinished(Request*)));
		_endpoint.send(*req);
	}
}

void NetInitiator::_set(FieldValue player, Game::BoardIndex index)
{
	if (player == Player1) {
		Messages::Vector3 dims, vals;
		dims.w = _game.dims().width();
		dims.h = _game.dims().height();
		dims.d = _game.dims().depth();

		vals.w = index.wVal();
		vals.h = index.hVal();
		vals.d = index.dVal();

		Messages::FieldState state = Messages::Player1;

		SendRequest *req = new SendRequest(
					Messages::updateGameBoard(dims, vals, state), this);
		connect(req, SIGNAL(finished(Request*)),
				this, SLOT(_reqFinished(Request*)));
		_endpoint.send(*req);
	}
}

void NetInitiator::_finished(FieldValue winner)
{
	Messages::FieldState w = (Messages::FieldState)(int)winner;
	SendRequest *req = new SendRequest(Messages::endGame(w), this);
	connect(req, SIGNAL(finished(Request*)), req, SLOT(deleteLater()));
	_endpoint.send(*req);
	_manager.unregisterGame();
	_disconnect();
}

void NetInitiator::_aborted(FieldValue requester, QString reason)
{
	if (requester == Player1) {
		NetworkString r(reason.remove(Message::MsgEndChar)
						.remove(Message::MsgSplitChar));
		SendRequest *req = new SendRequest(Messages::abortGame(r), this);
		connect(req, SIGNAL(finished(Request*)), this, SLOT(deleteLater()));
		_endpoint.send(*req);
	}
	_manager.unregisterGame();
	_disconnect();
}

void NetInitiator::_messageReceived(Message msg)
{
	if (!_game.hasStarted()) {
		NetworkString playerName;
		if (Messages::parseJoinGame(msg, playerName))
			emit joinRequested(playerName.string());
	} else {
		Messages::Vector3 dims, index;
		dims.w = _game.dims().width();
		dims.h = _game.dims().height();
		dims.d = _game.dims().depth();
		if (_game.curPlayer() == Player2 &&
			Messages::parseMove(msg, dims, index)) _handleMove(index);
	}
}

void NetInitiator::_reqFinished(Request *request)
{
	if (!request->success()) {
		emit error(request->errorString());
		_game.abort(Player1, request->errorString());
	}
	delete request;
}

void NetInitiator::_handleMove(Messages::Vector3 index)
{
	Game::BoardIndex idx = _game.index(index.w, index.d);
	if (!_game.set(idx)) {
		SendRequest *req = new SendRequest(
					Messages::movedFailed("Invalid move."), this);
		connect(req, SIGNAL(finished(Request*)),
				this, SLOT(_reqFinished(Request*)));
		_endpoint.send(*req);
	}
}

void NetInitiator::_disconnect()
{
	Request *req = new Request(this);
	connect(req, SIGNAL(finished(Request*)), req, SLOT(deleteLater()));
	_endpoint.disconnectFromHost(*req);
}

QString NetInitiator::_compileErrMsg(QList<ErroneousService> errServices) const
{
	QString errMsg = "";
	for (int i = 0; i < errServices.size(); i++) {
		IndexService *svc = errServices.at(i).service;
		errMsg += QString("Index server %1 (%2:%3) failure:"
						  " %4\n").arg(svc->name())
				.arg(svc->ipAddress().toString()).arg(svc->port())
				.arg(errServices.at(i).errorString);
	}
	return errMsg;
}
