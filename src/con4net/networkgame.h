/*
 * networkgame.h
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

#ifndef NETWORKGAME_H
#define NETWORKGAME_H

#include <stdexcept>
#include <QUuid>
#include <QHostAddress>
#include "message.h"
#include "../con4core/game.h"

class NetworkGameConf
{
public:
	class Exception : public std::invalid_argument
	{
	public:
		explicit Exception(NetworkString what)
			: invalid_argument(what.string().toStdString()) {}
	};

	NetworkGameConf();

	Game::Dimensions dims() const { return _dims; }
	void setDims(Game::Dimensions value) { _dims = value; }

	NetworkString initiatorName() const { return _initiatorName; }
	void setInitiatorName(NetworkString value);

	NetworkString name() const { return _name; }
	void setName(NetworkString value);

	QHostAddress ipAddress() const { return _ipAddress; }
	void setIpAddress(QHostAddress value);

	quint16 port() const { return _port; }
	void setPort(quint16 value);

private:
	Game::Dimensions _dims;
	NetworkString _initiatorName, _name;
	QHostAddress _ipAddress;
	quint16 _port;
};

class NetworkGame : public Game
{
	Q_OBJECT

public:
	NetworkGame(NetworkGameConf conf, QObject *parent = 0);
	~NetworkGame();

	NetworkString name() const { return _conf.name(); }
	NetworkString initiatorName() const { return _conf.initiatorName(); }
	QHostAddress ipAddress() const { return _conf.ipAddress(); }
	quint16 port() const { return _conf.port(); }
	QUuid guid() const { return _guid; }
	void setGuid(QUuid value) { _guid = value; }

private:
	NetworkGameConf _conf;
	QUuid _guid;
};

#endif // NETWORKGAME_H
