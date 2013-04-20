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

using namespace std;

OpponentService::OpponentService(int width, int height, int depth,
								 QString initiatorName, QString gameName,
								 QHostAddress ipAddress, quint16 port,
								 IndexServiceList *indexServices,
								 QObject *parent)
	: NetworkPlayerService(new NetworkGame(width, height, depth, gameName,
										   initiatorName, ipAddress, port),
						   parent), _indexServices(indexServices), _server(),
	  _socket(0), _timer(this)
{
	connect(&_server, SIGNAL(newConnection()), this, SLOT(newConnection()));
	if (!_server.listen(QHostAddress::Any, port)) {
		delete networkGame();
		throw runtime_error("Unable to listen on port " + port);
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
		delete networkGame();
		throw runtime_error("Unable to register game.");
	}

	connect(&_timer, SIGNAL(timeout()), this, SLOT(update()));

	connect(networkGame(), SIGNAL(set(FieldValue,int,int)), this,
			SLOT(set(FieldValue, int, int)));
}

OpponentService::~OpponentService() { delete networkGame(); }

bool OpponentService::joinGameSuccess()
{
	// seal game
	QString errMsg;
	for (int i = 0; i < _indexServices->size(); i++)
		_indexServices->at(i)->sealGame(*networkGame(), errMsg);

	// join success
	QString msg = QString("join_game_success;%1\n").arg("V2");
	_sendMsg(msg);

	// start game
	networkGame()->start();
	if (networkGame()->curPlayer() == Player2) {
		QString startMsg = QString("start_game\n");
		_sendMsg(startMsg);
	}
}

bool OpponentService::joinGameFailed(QString reason)
{
	QString msg = QString("join_game_failed;%1\n").arg(reason);
	return _sendMsg(msg);
}

void OpponentService::newConnection()
{
	_socket = _server.nextPendingConnection();
	_timer.start(500);
}

void OpponentService::update()
{
	if (_socket && _socket->state() == QAbstractSocket::ConnectedState) {
		if (_socket->bytesAvailable() > 0) {
			char buffer[MSG_BUF_SIZE];
			int bytesRead = _socket->read(buffer, MSG_BUF_SIZE - 1);
			buffer[bytesRead] = 0;
			QString qsResp = QString::fromUtf8(buffer);
			_handleMsg(qsResp.split(MSG_SPLIT_CHAR));
		}
	} else {
		_timer.stop();
	}
}

/*

F ̈r die Feldnummer eines 4-Gewinnt-Spiel mit der Tiefe T , H ̈he H, Breite B gilt folgender
u
o
Zusammenhang:
Spielbrett3D [k, i, j] = Spielbrett1D [k × B × H + i × B + j] = Spielbrett1D [F eldnummer]
wobei sich Indizes k, i und j jeweils auf Tiefe, H ̈he und Breite beziehen. Es gilt: 0 ≤ k < T , 0 ≤ i < H,
o
0 ≤ j < B, sowie 0 ≤ F eldnummer < T × B × H und T ≥ 03 , B, H > 0.
Die Gravitation wirkt entlang der H ̈henachse. Koordinate


  */

void OpponentService::set(FieldValue player, int width, int depth)
{
	if (player == Player1) {
		int height;
		game()->isFull(width, depth, height);

		int x = depth * game()->width() * game()->height() +
				height * game()->width() + width;
		QString msg = QString("update_game_board;%1;%2\n").arg(x).arg(1);
		_sendMsg(msg);
		if (game()->finished()) {
			if (game()->isDraw()) _sendMsg(QString("end_game;%1\n").arg(0));
			else _sendMsg(QString("end_game;%1\n").arg(1));
			_socket->disconnectFromHost();
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
				_sendMsg(QString("moved_failed;%1\n").arg("Invalid move"));
			else {
				if (game()->finished()) {
					if (game()->isDraw())
						_sendMsg(QString("end_game;%1\n").arg(0));
					else _sendMsg(QString("end_game;%1\n").arg(2));
					_socket->disconnectFromHost();
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

bool OpponentService::_sendMsg(QString msg)
{
	if (_socket && _socket->state() == QAbstractSocket::ConnectedState) {
		_socket->write(msg.toUtf8());
		if (!_socket->waitForBytesWritten())
			throw runtime_error("Error: Failed to send data to server.");
		return true;
	} else return false;
}
