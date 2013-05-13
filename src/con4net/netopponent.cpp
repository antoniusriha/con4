/*
 * initiatorservice.cpp
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

#include "netopponent.h"

NetOpponent::NetOpponent(NetworkGameConf conf, QObject *parent)
	: QObject(parent), _joined(false), _game(conf),
	  _playerName(conf.initiatorName()),
	  _endpoint(conf.ipAddress(), conf.port(), 10000, this)
{
	connect(&_game, SIGNAL(aborted(FieldValue,QString)),
			this, SLOT(_aborted(FieldValue,QString)));
	connect(&_game, SIGNAL(set(FieldValue,Game::BoardIndex)),
			this, SLOT(_set(FieldValue,Game::BoardIndex)));

	connect(&_endpoint, SIGNAL(messageReceived(Message)),
			this, SLOT(_msgReceived(Message)));

	setPlayerName("Player2");
}

NetOpponent::~NetOpponent() {}

void NetOpponent::setPlayerName(NetworkString value)
{
	if (value.string().isEmpty())
		throw ArgumentException("PlayerName must not be empty.");
	_playerName = value;
}

void NetOpponent::connectToServer()
{
	Request *req = new Request();
	connect(req, SIGNAL(finished(Request*)),
			this, SLOT(_connectFinished(Request*)));
	_endpoint.connectToServer(*req);
}

void NetOpponent::_connectFinished(Request *request)
{
	emit connectToServerFinished(request->success(), request->errorString());
	delete request;
}

void NetOpponent::join()
{
	if (_joined) throw InvalidOperationException("Already joined a game.");

	SendAndReceiveRequest *req = new SendAndReceiveRequest(
				Messages::joinGame(_playerName, game()->name()));
	connect(req, SIGNAL(finished(Request*)),
			this, SLOT(_joinFinished(Request*)));
	_endpoint.sendAndReceive(*req);
}

void NetOpponent::stop()
{
}

void NetOpponent::_joinFinished(Request *request)
{
	if (request->success()) {
		SendAndReceiveRequest *req =
				static_cast<SendAndReceiveRequest *>(request);
		NetworkString reason;
		Messages::ProtocolVersion pVersion;
		if (Messages::parseJoinGameSuccess(req->msgReceived(), pVersion)) {
			_joined = true;
			emit joinGameFinished(true, QString());
		} else if (Messages::parseJoinGameFailed(req->msgReceived(), reason))
			emit joinGameFinished(false, reason.string());
		else emit joinGameFinished(false, Messages::InvalidRespErr);
	}
	delete request;
}

void NetOpponent::_set(FieldValue player, Game::BoardIndex index)
{
	if (player == Player2) {
		Messages::Vector3 dims;
		dims.w = game()->dims().width();
		dims.h = game()->dims().height();
		dims.d = game()->dims().depth();

		Messages::Vector3 vals;
		vals.w = index.wVal();
		vals.h = index.hVal();
		vals.d = index.dVal();

		SendRequest *req = new SendRequest(Messages::move(dims, vals), this);
		connect(req, SIGNAL(finished(Request*)),
				this, SLOT(_moveFinished(Request*)));
		_endpoint.send(*req);
	}
}

void NetOpponent::_moveFinished(Request *request)
{
	if (!request->success()) _abort("Failed to communicate with server.");
	delete request;
}

void NetOpponent::_msgReceived(Message msg)
{
	if (!_joined) return;

	// abort can come at any time
	NetworkString reason;
	if (Messages::parseAbortGame(msg, reason)) {
		_game.abort(Player1, reason.string());
		return;
	}

	Messages::FieldState state;
	Messages::Vector3 dims, vals;
	dims.w = game()->dims().width();
	dims.h = game()->dims().height();
	dims.d = game()->dims().depth();
	QList<Messages::Field> fields;
	if (!game()->hasStarted()) {
		/*
		  expecting:
			- start signal
			- sync board/update board
		*/
		if (Messages::parseStartGame(msg)) _game.start(Player2);
		else if (Messages::parseSynchronizeGameBoard(msg, dims, fields)) {
			_game.start(Player1);
			_handleSyncGameBoard(fields);
		} else if (Messages::parseUpdateGameBoard(msg, dims, vals, state)) {
			_game.start(Player1);
			_handleUpdateGameBoard(vals, state);
		}
	} else {
		if (Messages::parseEndGame(msg, state)) {
			if (!game()->finished()) _abort(Messages::InvalidRespErr);
			Request *req = new Request();
			connect(req, SIGNAL(finished(Request*)), req, SLOT(deleteLater()));
			_endpoint.disconnectFromHost(*req);
		} else if (Messages::parseMovedFailed(msg, reason))
			_abort(reason.string());
		else if (Messages::parseSynchronizeGameBoard(msg, dims, fields))
			_handleSyncGameBoard(fields);
		else if (Messages::parseUpdateGameBoard(msg, dims, vals, state))
			_handleUpdateGameBoard(vals, state);
	}
}

void NetOpponent::_handleSyncGameBoard(QList<Messages::Field> fields)
{
	Game::BoardIndex idxSet;
	for (int i = 0; i < fields.size(); i++) {
		const Messages::Field field = fields.at(i);
		Game::BoardIndex idx = game()->index(field.index.w, field.index.d,
											 field.index.h);
		if (game()->get(idx) == None) {
			if (field.state == Messages::Player1) {
				// fail if more than one field has been set, else prep set
				if (idxSet.isValid()) {
					_abort("Invalid game board synchronization");
					return;
				} else idxSet = idx;
			} else if (field.state == Messages::Player2) {
				_abort("Invalid game board synchronization");
				return;
			}
		} else {
			if ((int)game()->get(idx) != (int)field.state) {
				_abort("Invalid game board synchronization");
				return;
			}
		}
	}

	if (idxSet.isValid()) _game.set(idxSet);
}

void NetOpponent::_handleUpdateGameBoard(Messages::Vector3 vals,
										 Messages::FieldState state)
{
	Game::BoardIndex idx = game()->index(vals.w, vals.d, vals.h);
	if (game()->get(idx) == None) {
		if (state == Messages::Player1) _game.set(idx);
		else if (state == Messages::Player2)
			_abort("Invalid game board update");
	} else {
		if ((int)_game.get(idx) != (int)state)
			_abort("Invalid game board update");
	}
}

void NetOpponent::_abort(QString reason)
{
	_game.abort(Player2, reason);
}

void NetOpponent::_aborted(FieldValue requester, QString reason)
{
	if (requester == Player2) {
		SendRequest *req = new SendRequest(Messages::abortGame(reason));
		connect(req, SIGNAL(finished(Request*)), req, SLOT(deleteLater()));
		_endpoint.send(*req);
	}
	Request *req = new Request();
	connect(req, SIGNAL(finished(Request*)), req, SLOT(deleteLater()));
	_endpoint.disconnectFromHost(*req);
}
