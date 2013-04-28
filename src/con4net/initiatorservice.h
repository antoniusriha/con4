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
#include <QStringList>
#include "networkplayerservice.h"
#include "clientendpoint.h"
#include "messages.h"

class InitiatorService : public NetworkPlayerService
{
	Q_OBJECT

public:
	class InitiatorServiceConf
	{
	public:
		class Exception : public std::invalid_argument
		{
		public:
			explicit Exception(QString what)
				: invalid_argument(what.toStdString()) {}
		};

		InitiatorServiceConf(NetworkGame &game);

		NetworkGame *game() const { return &_game; }

		NetworkString playerName() const { return _playerName; }
		void setPlayerName(NetworkString value);

	private:
		NetworkGame &_game;
		NetworkString _playerName;
	};

	class InvalidOperationException : public std::logic_error
	{
	public:
		explicit InvalidOperationException(QString what)
			: logic_error(what.toStdString()) {}
	};

	InitiatorService(InitiatorServiceConf conf, QObject *parent = 0);
	~InitiatorService();

	NetworkString playerName() const { return _playerName; }

	void connectToServer();
	void join();
	void move(int fieldNumber) const;
	void abortGame(QString reason) const;

signals:
	void connectToServerFinished(bool success, QString failReason);
	void joinGameSuccess();
	void joinGameFailed(QString reason);
	void startGame();
	void synchronizeGameBoard(int **fieldNumberAndValue);
	void updatedGameBoard(int fieldNumber, FieldValue value);
	void movedFailed(QString reason);
	void endGame(int result);
	void abortGame(QString reason);

private slots:
	void aborted(FieldValue requester, QString reason);
	void finished(FieldValue winner);
	void set(FieldValue player, Game::BoardIndex index);
	void started(FieldValue startPlayer);
	void undone(FieldValue player, Game::BoardIndex index);
	void _msgReceived(Message msg);

private:
	void _handleMsg(QStringList msgTokens);
	void _handleSyncGameBoard(QStringList msgTokens);
	void _handleUpdateGameBoard(QStringList msgTokens);
	void _abort(QString reason);

	bool _joined;
	NetworkString _playerName;
	ClientEndpoint _endpoint;
	Messages::ProtocolVersion _protocolVersion;
};

#endif // INITIATORSERVICE_H
