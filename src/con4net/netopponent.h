/*
 * initiatorservice.h
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

#ifndef INITIATORSERVICE_H
#define INITIATORSERVICE_H

#include <stdexcept>
#include <QTcpSocket>
#include "networkplayerservice.h"
#include "clientendpoint.h"
#include "messages.h"

class NetOpponent : public QObject
{
	Q_OBJECT

public:
	class InvalidOperationException : public std::logic_error
	{
	public:
		explicit InvalidOperationException(QString what)
			: logic_error(what.toStdString()) {}
	};

	class ArgumentException : public std::invalid_argument
	{
	public:
		explicit ArgumentException(QString what)
			: invalid_argument(what.toStdString()) {}
	};

	NetOpponent(NetworkGameConf conf, QObject *parent = 0);
	~NetOpponent();

	NetworkGame *game() { return &_game; }

	NetworkString playerName() const { return _playerName; }
	void setPlayerName(NetworkString value);

	void connectToServer();
	void join();
	void stop();

signals:
	void connectToServerFinished(bool success, QString failReason);
	void joinGameFinished(bool success, QString failReason);

private slots:
	void _aborted(FieldValue requester, QString reason);
	void _set(FieldValue player, Game::BoardIndex index);
	void _connectFinished(Request *request);
	void _joinFinished(Request *request);
	void _msgReceived(Message msg);
	void _moveFinished(Request *request);

private:
	void _handleSyncGameBoard(QList<Messages::Field> fields);
	void _handleUpdateGameBoard(Messages::Vector3 vals,
								Messages::FieldState state);
	void _abort(QString reason);

	bool _joined;
	NetworkGame _game;
	NetworkString _playerName;
	ClientEndpoint _endpoint;
};

#endif // INITIATORSERVICE_H
