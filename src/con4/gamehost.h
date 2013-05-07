/*
 * gamehost.h
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

#ifndef GAMEHOST_H
#define GAMEHOST_H

#include <QColor>
#include "../con4net/netinitiator.h"
#include "../con4net/netopponent.h"
#include "../con4gl/glwidget.h"

class GameHostConf
{
public:
	class ArgumentException : public std::invalid_argument
	{
	public:
		ArgumentException(QString what)
			: invalid_argument(what.toStdString()) {}
	};

	GameHostConf() { setPlayerColors(QColor(Qt::red), QColor(Qt::yellow)); }

	QColor player1Color() const { return _player1Color; }
	QColor player2Color() const { return _player2Color; }
	void setPlayerColors (QColor player1Color, QColor player2Color);

private:
	QColor _player1Color, _player2Color;
};

class LocalGameHostConf : public GameHostConf
{
public:
	LocalGameHostConf();

	Game::Dimensions dims() const { return _dims; }
	void setDims(Game::Dimensions value) { _dims = value; }

	QString player1Name() const { return _player1Name; }
	QString player2Name() const { return _player2Name; }
	void setPlayerNames(QString player1Name, QString player2Name);

private:
	Game::Dimensions _dims;
	QString _player1Name, _player2Name;
};

class NetworkGameHostConf : public GameHostConf
{
public:
	NetworkGameHostConf(IndexServiceList &list)
		: GameHostConf(), _netInitiatorConf(list) {}

	NetInitiatorConf netInitiatorConf() const { return _netInitiatorConf; }
	void setNetInitiatorConf(NetInitiatorConf value);

private:
	NetInitiatorConf _netInitiatorConf;
};

class JoinNetworkGameHostConf : public GameHostConf
{
public:
	JoinNetworkGameHostConf() : GameHostConf() { setPlayer2Name("Player2"); }

	NetworkString player2Name() const { return _player2Name; }
	void setPlayer2Name(NetworkString value) { _player2Name = value; }

	NetworkGameConf networkGameConf() const { return _networkGameConf; }
	void setNetworkGameConf(NetworkGameConf value) { _networkGameConf = value; }

private:
	NetworkString _player2Name;
	NetworkGameConf _networkGameConf;
};

class GameHost : public QObject
{
	Q_OBJECT
public:
	static GameHost *CreateLocalGame(LocalGameHostConf conf,
									 QObject *parent = 0);
	static GameHost *CreateNetworkGame(NetworkGameHostConf conf,
									   QObject *parent = 0);
	static GameHost *CreateNetworkGameWithAiPlayer(NetworkGameConf conf,
												   QObject *parent = 0);
	static GameHost *JoinNetworkGame(JoinNetworkGameHostConf conf,
									 QObject *parent = 0);
	static GameHost *JoinNetworkGameWithAiPlayer(JoinNetworkGameHostConf conf,
												 QObject *parent = 0);

private:
	explicit GameHost(QObject *parent = 0);
	
private:
	Game *_game;
	Player *_player1, *_player2;
	GLWidget _window;
};

#endif // GAMEHOST_H
