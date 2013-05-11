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

#include <stdexcept>
#include <QUuid>
#include <QUdpSocket>
#include "clientendpoint.h"
#include "networkgame.h"
#include "messages.h"
#include "netopponent.h"

class IndexServiceConf
{
public:
	class Exception : public std::invalid_argument
	{
	public:
		explicit Exception(QString what)
			: invalid_argument(what.toStdString()) {}
	};

	IndexServiceConf();

	QString name() const { return _name; }
	void setName(QString value);

	QHostAddress ipAddress() const { return _ipAddress; }
	void setIpAddress(QHostAddress value);

	quint16 port() const { return _port; }
	void setPort(quint16 value);

private:
	QString _name;
	QHostAddress _ipAddress;
	quint16 _port;
};

class IndexService;

class NetworkGameRequest : public Request
{
public:
	NetworkGameRequest(NetworkGame &game, IndexService &indexService,
					   QObject *parent = 0);
	NetworkGame *game() const { return &_game; }
	IndexService *indexService() const { return &_indexService; }

private:
	NetworkGame &_game;
	IndexService &_indexService;
};

class RequestGameListUnit;

class IndexService : public QObject
{
	Q_OBJECT

public:
	class InvalidOperationException : public std::logic_error
	{
	public:
		explicit InvalidOperationException(QString what)
			: logic_error(what.toStdString()) {}
	};

	IndexService(IndexServiceConf conf);
	~IndexService();

	QString name() const { return _conf.name(); }
	QHostAddress ipAddress() const { return _conf.ipAddress(); }
	quint16 port() const { return _conf.port(); }

	QList<const NetworkGame *> games() const;

	void registerGame(NetworkGameRequest &request);
	void unregisterGame(NetworkGameRequest &request);
	void refreshGameList(Request &request);
	void sealGame(NetworkGameRequest &request);

private:
	ProcessingQueue _queue;
	ClientEndpoint _endpoint;
	IndexServiceConf _conf;
	QList<NetOpponent *> _opps;
	QList<NetworkGame *> _regGames;
	QUdpSocket _udpSocket;

	friend class RequestGameListUnit;
};

#endif // INDEXSERVICE_H
