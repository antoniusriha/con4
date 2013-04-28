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

#include <QStringList>
#include "con4netglobals.h"
#include "initiatorservice.h"

using namespace std;

InitiatorService::InitiatorServiceConf::InitiatorServiceConf(NetworkGame &game)
	: _game(game) {}

void InitiatorService::InitiatorServiceConf::setPlayerName(NetworkString value)
{
	if (value.string().isEmpty())
		throw Exception("PlayerName must not be empty.");
	_playerName = value;
}

InitiatorService::InitiatorService(InitiatorServiceConf conf, QObject *parent)
	: NetworkPlayerService(*conf.game(), parent), _joined(false),
	  _playerName(conf.playerName()), _endpoint(10000, this)
{
	connect(&_endpoint, SIGNAL(messageReceived(Message)),
			this, SLOT(_msgReceived(Message)));
	connect(&_endpoint, SIGNAL(connectToServerFinished(bool,QString)),
			this, SLOT(connectToServerFinished(bool, QString)));
}

InitiatorService::~InitiatorService() {}

void InitiatorService::connectToServer()
{
	_endpoint.connectToServer(game()->ipAddress(), game()->port());
}

void InitiatorService::join()
{
	if (_joined) throw InvalidOperationException("Already joined a game.");
	_endpoint.send(Messages::joinGame(_playerName, game()->name()));
}

void InitiatorService::set(FieldValue player, Game::BoardIndex index)
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

		_endpoint.send(Messages::move(dims, vals));
	}
}

void InitiatorService::_msgReceived(Message msg)
{
	if (!game()->hasStarted()) {
		/*
		  expecting:
			- join answer
			- start signal
			- sync board/update board
		*/
		NetworkString failReason;
		if (Messages::parseJoinGameSuccess(msg, _protocolVersion)) {
			_joined = true;

		} else if (Messages::parseJoinGameFailed(msg, failReason)) {
			_endpoint.disconnectFromHost();
		} else if (Messages::parseStartGame(msg)) {

		}
	}

	/*
	if (msgTokens.empty()) return;
	QString header = msgTokens.at(0);

	if (_joined && !networkGame()->hasStarted()) {
		// expect start signal or update resp. synchronize
		if (header == "start_game") {
			networkGame()->start(Player2);
		} else if (header == "synchronize_game_board") {
			networkGame()->start(Player1);
			_handleSyncGameBoard(msgTokens);
		} else if (header == "update_game_board") {
			networkGame()->start(Player1);
			_handleUpdateGameBoard(msgTokens);
		}
	} else if (networkGame()->hasStarted()) {
		// expect sync/update, abort, end_game
		if (header == "synchronize_game_board") {
			_handleSyncGameBoard(msgTokens);
		} else if (header == "update_game_board") {
			_handleUpdateGameBoard(msgTokens);
		} else if (header == "end_game") {
			if (!game()->finished())
				game()->abort(Player2, "Unfair server game play");
			_socket.disconnectFromHost();
		} else if (header == "abort_game") {
			game()->abort(Player1, msgTokens.size() > 1 ? msgTokens.at(1)
														: "Server error");
			_socket.disconnectFromHost();
		}
	}
	  */
}

void InitiatorService::_handleSyncGameBoard(QStringList msgTokens)
{
	/*
	if ((msgTokens.size() - 2)/2 != game()->width() * game()->height() *
			game()->depth()) {
		_abort("Invalid game board synchronization");
		return;
	}

	int width = -1, height = -1, depth = -1, disksSet = 0;
	for (int i = 2; i < msgTokens.size(); i += 2) {
		int x = msgTokens.at(i).toInt();
		int v = msgTokens.at(i + 1).toInt();

		int xw, xh, xd;
		if (!_getCoords(x, xw, xh, xd)) {
			_abort("Invalid game board synchronization");
			return;
		}

		if (v > 0) disksSet++;
		if (v != game()->get(xw, xh, xd)) {
			if (width != -1 || height != -1 || depth != -1 || v < 1) {
				_abort("Invalid game board synchronization");
				return;
			}
			width = xw;
			height = xh;
			depth = xd;
		}
	}

	if (disksSet - 1 != game()->setCount() || width == -1 || depth == -1) {
		_abort("Invalid game board synchronization");
		return;
	}

	if (!game()->set(width, depth))
		_abort("Invalid game board synchronization");
		*/
}

void InitiatorService::_handleUpdateGameBoard(QStringList msgTokens)
{
	/*
	if (msgTokens.size() < 3) {
		_abort("Invalid game board upadate");
		return;
	}

	int x = msgTokens.at(1).toInt();
	FieldValue v = (FieldValue)msgTokens.at(2).toInt();

	int xw, xh, xd;
	if (!_getCoords(x, xw, xh, xd) || game()->get(xw, xh, xd) != None) {
		_abort("Invalid game board upadate");
		return;
	}

	if (!game()->set(xw, xd)) _abort("Invalid game board upadate");
	*/
}

void InitiatorService::_abort(QString reason)
{
	_endpoint.send(Messages::abortGame(reason));
	_endpoint.disconnectFromHost();
	game()->abort(Player2, reason);
}



/*
bool InitiatorService::_getCoords(int fieldNumber, int &width, int &height,
								  int &depth)
{
	if (fieldNumber <= 0) return false;

	div_t r = div(fieldNumber, game()->width() * game()->height());
	depth = r.quot;
	r = div(r.rem, game()->width());
	height = r.quot;
	width = r.rem;

	if (width >= game()->width() || height >= game()->height()) return false;
	if (game()->nDims() == 3 && depth >= game()->depth()) return false;
}
*/
