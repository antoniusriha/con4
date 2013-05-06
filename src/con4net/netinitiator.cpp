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

class IndexServiceWrapper : public QObject
{
	Q_OBJECT

public:
	enum State { NotRegistered, Registered, Sealed, Unregistered };

	IndexServiceWrapper(IndexService &service, NetworkGame &game)
		: _service(service), _game(game), _state(NotRegistered),
		  _regReq(game), _sealReq(game), _unregReq(game)
	{
		connect(&_regReq, SIGNAL(finished(Request*)),
				this, SLOT(_registerGameFinished(Request*)));
		connect(&_sealReq, SIGNAL(finished(Request*)),
				this, SLOT(_sealGameFinished(Request*)));
		connect(&_unregReq, SIGNAL(finished(Request*)),
				this, SLOT(_unregisterGameFinished(Request*)));
	}

	IndexService *service() const { return &_service; }
	State state() const { return _state; }

	NetworkGameRequest *regRequest() const { return &_regReq; }
	NetworkGameRequest *sealRequest() const { return &_sealReq; }
	NetworkGameRequest *unregRequest() const { return &_unregReq; }

	void registerGame() { _service.registerGame(_regReq); }
	void sealGame()	{ _service.sealGame(_sealReq); }
	void unregisterGame() { _service.unregisterGame(_unregReq); }

signals:
	void registerGameFinished(IndexServiceWrapper *wrapper);
	void sealGameFinished(IndexServiceWrapper *wrapper);
	void unregisterGameFinished(IndexServiceWrapper *wrapper);

private slots:
	void _registerGameFinished(Request *request)
	{
		if (request->success()) _state = Registered;
		emit registerGameFinished(this);
	}

	void _sealGameFinished(Request *request)
	{
		if (request->success()) _state = Sealed;
		emit sealGameFinished(this);
	}

	void _unregisterGameFinished(Request *request)
	{
		if (request->success()) _state = Unregistered;
		emit unregisterGameFinished(this);
	}

private:
	IndexService &_service;
	NetworkGame &_game;
	State _state;
	NetworkGameRequest _regReq, _sealReq, _unregReq;
};

NetInitiator::NetInitiator(NetInitiatorConf conf, QObject *parent)
	: QObject(parent), _list(*conf.indexServiceList()),
	  _game(conf.networkGameConf()), _wrappers(), _endpoint(10000, this)
{
	connect(&_list, SIGNAL(deleting(IndexService*)),
			this, SLOT(_indexServiceDeleting(IndexService*)));
	for (int i = 0; i < _list.size(); i++)
		_wrappers.append(new IndexServiceWrapper(*_list.at(i), _game));
}

void NetInitiator::_indexServiceDeleting(IndexService *service)
{
	for (int i = 0; i < _wrappers.size(); i++) {
		if (_wrappers.at(i)->service() == service) {
			IndexServiceWrapper *wrapper = _wrappers.takeAt(i);
			wrapper->unregisterGame();
			delete wrapper;
			return;
		}
	}

	if (_wrappers.size() == 0)
		_game.abort(Player1, "All index services have been removed.");
}

NetInitiator::~NetInitiator()
{
	for (int i = 0; i < _wrappers.size(); i++)
		delete _wrappers.takeFirst();
}

bool NetInitiator::startService(QString *errMsg)
{
	if (!_endpoint.listen(_game.port())) {
		if (errMsg)
			*errMsg = QString("Unable to listen on port %1.").arg(_game.port());
		return false;
	}
	return true;
}

void NetInitiator::registerGame()
{
	for (int i = 0; i < _wrappers.size(); i++) {
		IndexServiceWrapper *wrapper = _wrappers.at(i);
		connect(wrapper, SIGNAL(registerGameFinished(IndexServiceWrapper*)),
				this, SLOT(_registerGameFinished(IndexServiceWrapper*)));
		wrapper->registerGame();
	}
}

void NetInitiator::_registerGameFinished(IndexServiceWrapper *wrapper)
{
	if (wrapper->regRequest()->success())
}

void NetInitiator::_sealGameFinished(IndexServiceWrapper *wrapper)
{
}

void NetInitiator::_unregisterGameFinished(IndexServiceWrapper *wrapper)
{
}

void NetInitiator::acceptJoinRequest()
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

void NetInitiator::rejectJoinRequest(QString reason)
{
	_endpoint.send(Messages::joinGameFailed(reason));
}

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
