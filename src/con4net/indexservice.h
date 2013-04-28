/*
 * indexservice.h
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

#ifndef INDEXSERVICE_H
#define INDEXSERVICE_H

#include <QUuid>
#include <QHostAddress>
#include <QUdpSocket>
#include "clientendpoint.h"
#include "networkgame.h"
#include "messages.h"

class IndexService : public QObject
{
	Q_OBJECT

public:
	IndexService (QHostAddress host, quint16 port, QString name);
	~IndexService();

	QString name () const { return _name; }
	QHostAddress ipAddress () const { return _host; }
	quint16 port () const { return _port; }
	QList<NetworkGame *> *games() { return &_games; }

	void registerGame(NetworkGame &game);
	void unregisterGame (NetworkGame &game);
	void refreshGameList ();
	void sealGame (NetworkGame &game);

signals:
	void registerFinished(bool success, QUuid guid, QString failReason);
	void unregisterFinished(bool success, QString failReason);
	void refreshGameListFinished(bool success, QString failReason);
	void sealGameFinished(bool success, QString failReason);

private slots:
	void _sendAndReceiveFinished(bool success, Message msg, QString failReason);

private:
	struct ProcessingUnit
	{
		enum MsgType { Register, Unregister, Refresh, Seal };
		Message msg;
		MsgType type;
	};

	void _processMsg();
	void _readGameListAnswer(QList<Messages::GameData> &gameData);

	bool _processing;
	QQueue<ProcessingUnit> _msgQueue;
	ProcessingUnit _curUnit;
	ClientEndpoint _endpoint;
	QHostAddress _host;
	quint16 _port;
	QString _name;
	QList<NetworkGame *> _games;
	QUdpSocket _udpSocket;
};

#endif // INDEXSERVICE_H
