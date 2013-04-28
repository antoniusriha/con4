/*
 * opponentservice.h
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

#ifndef OPPONENTSERVICE_H
#define OPPONENTSERVICE_H

#include <QUuid>
#include <QStringList>
#include "serverendpoint.h"
#include "networkplayerservice.h"
#include "indexservicelist.h"

class OpponentServiceConf
{
public:
	OpponentServiceConf(QList<IndexService *> indexServices);

	NetworkGameConf networkGameConf() const { return _networkGameConf; }
	void setNetworkGameConf(NetworkGameConf value) { _networkGameConf = value; }

	QList<IndexService *> indexServices() const { return _indexServices; }
	void setIndexServices(QList<IndexService *> value);

private:
	NetworkGameConf _networkGameConf;
	QList<IndexService *> _indexServices;
};

class OpponentService : public NetworkPlayerService
{
	Q_OBJECT

public:
	OpponentService(OpponentServiceConf conf, QObject *parent = 0);
	~OpponentService();

	bool startService(QString *errMsg = 0);

	void acceptJoinRequest();
	void rejectJoinRequest(QString reason);
	void startGame() const;
	void synchronizeGameBoard(int **fieldNumberAndValue) const;
	void updatedGameBoard(int fieldNumber, int value) const;
	void moved_failed(QString reason) const;
	void endGame(int result) const;
	void abortGame(QString reason) const;

signals:
	void joinGame(QString playerName);
	void move(int fieldIndex);
	void abortGame(QString reason);

private slots:
	void _messageReceived(Message msg);
	void aborted(FieldValue requester, QString reason);
	void finished(FieldValue winner);
	void set(FieldValue player, Game::BoardIndex index);
	void started(FieldValue startPlayer);
	void undone(FieldValue player, Game::BoardIndex index);

private:
	void _handleMsg(QStringList msgTokens);
	bool _sendMsg(QString msg);

	QList<IndexService *> _indexServices;
	ServerEndpoint &_endpoint;
};

#endif // OPPONENTSERVICE_H
