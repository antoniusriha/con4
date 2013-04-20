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

#include <stdexcept>
#include <QStringList>
#include "con4netglobals.h"
#include "initiatorservice.h"

using namespace std;

InitiatorService::InitiatorService(NetworkGame *game, QObject *parent)
	: NetworkPlayerService(game, parent), _joined(false), _socket(), _timer()
{
	connect(&_timer, SIGNAL(timeout()), this, SLOT(update()));
	_connect();
	_timer.start(500);

	connect(game, SIGNAL(set(FieldValue,int,int)),
			this, SLOT(set(FieldValue,int,int)));
}

InitiatorService::~InitiatorService()
{
	_timer.stop();
}

bool InitiatorService::join(QString &errMsg)
{
	if (_joined) {
		errMsg = "Already joined a game.";
		return false;
	}

	errMsg = "";
	QString msg = QString("join_game;%1;%2;%3\n").arg("guest")
			.arg(networkGame()->name()).arg("V2");

	QStringList respTokens;
	try {
		respTokens = _sendMsg(msg);
	} catch (runtime_error &ex) {
		errMsg = ex.what();
	}

	if (respTokens.empty())
		errMsg = INVALID_RESP_ERR;

	if (respTokens.at(0) == "join_game_failed") {
		if (respTokens.size() > 1) errMsg = respTokens.at(1);
		else errMsg = "The server has denied you access to the game.";
	}

	if (errMsg.isEmpty()) {
		_joined = true;
		return true;
	} else {
		_socket.disconnectFromHost();
		return false;
	}
}

void InitiatorService::update()
{
	_connect();
	if (_socket.state() == QAbstractSocket::ConnectedState) {
		if (_socket.bytesAvailable() > 0) {
			char buffer[MSG_BUF_SIZE];
			int bytesRead = _socket.read(buffer, MSG_BUF_SIZE - 1);
			buffer[bytesRead] = 0;
			QString qsResp = QString::fromUtf8(buffer);
			_handleMsg(qsResp.split(MSG_SPLIT_CHAR));
		}
	}
}

void InitiatorService::set(FieldValue player, int width, int depth)
{
	if (player == Player2) {
		int height;
		game()->isFull(width, depth, height);
		int x = depth * game()->width() * game()->height() +
				height * game()->width() + width;
		_sendMsg(QString("move;%1\n").arg(x));
	}
}

bool InitiatorService::_connect()
{
	if (_socket.state() == QAbstractSocket::ConnectedState)
		return true;

	_socket.connectToHost(networkGame()->ipAddress(), networkGame()->port());
	if (!_socket.waitForConnected(5000))
		return false;
	return true;
}

QStringList InitiatorService::_sendMsg(QString msg)
{
	_connect();
	_socket.write(msg.toUtf8());
	if (!_socket.waitForBytesWritten())
		throw runtime_error("Error: Failed to send data to server.");

	if (!_socket.waitForReadyRead())
		throw runtime_error("Error: No response from server.");

	char buffer[MSG_BUF_SIZE];
	int bytesRead = _socket.read(buffer, MSG_BUF_SIZE - 1);
	buffer[bytesRead] = 0;
	QString qsResp = QString::fromUtf8(buffer);
	return qsResp.split(MSG_SPLIT_CHAR);
}

void InitiatorService::_handleMsg(QStringList msgTokens)
{
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
}

void InitiatorService::_handleSyncGameBoard(QStringList msgTokens)
{
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
}

void InitiatorService::_handleUpdateGameBoard(QStringList msgTokens)
{
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
}

void InitiatorService::_abort(QString reason)
{
	_sendMsg(QString("abort_game;%1\n").arg(reason));
	game()->abort(Player2, reason);
	_socket.disconnectFromHost();
}

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
