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
#include "aiplayerinfo.h"
#include "../con4net/netinitiator.h"
#include "../con4net/netopponent.h"
#include "../con4gl/boardview.h"

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

	AiPlayerInfo *player1AiInfo() const { return _player1AiInfo; }
	void setPlayer1AiInfo(AiPlayerInfo *value) { _player1AiInfo = value; }
	AiPlayerInfo *player2AiInfo() const { return _player2AiInfo; }
	void setPlayer2AiInfo(AiPlayerInfo *value) { _player2AiInfo = value; }

private:
	Game::Dimensions _dims;
	QString _player1Name, _player2Name;
	AiPlayerInfo *_player1AiInfo, *_player2AiInfo;
};

class NetworkGameHostConf : public GameHostConf, public NetInitiatorConf
{
public:
	NetworkGameHostConf(IndexServiceList &list);

	AiPlayerInfo *player1AiInfo() const { return _player1AiInfo; }
	void setPlayer1AiInfo(AiPlayerInfo *value) { _player1AiInfo = value; }

private:
	AiPlayerInfo *_player1AiInfo;
};

class JoinNetworkGameHostConf : public GameHostConf
{
public:
	JoinNetworkGameHostConf();

	bool isValid() const { return _opp; }

	NetworkString player2Name() const { return _player2Name; }
	void setPlayer2Name(NetworkString value) { _player2Name = value; }

	NetOpponent *opponent() const { return _opp; }
	void setOpponent(NetOpponent &value) { _opp = &value; }

	AiPlayerInfo *player2AiInfo() const { return _player2AiInfo; }
	void setPlayer2AiInfo(AiPlayerInfo *value) { _player2AiInfo = value; }

private:
	NetworkString _player2Name;
	NetOpponent *_opp;
	AiPlayerInfo *_player2AiInfo;
};

class GameHost : public QObject
{
	Q_OBJECT
public:
	static GameHost *CreateLocalGame(LocalGameHostConf conf,
									 QObject *parent = 0);
	static GameHost *CreateNetworkGame(NetworkGameHostConf conf,
									   QObject *parent = 0);
	static GameHost *JoinNetworkGame(JoinNetworkGameHostConf conf,
									 QObject *parent = 0);

signals:
	void gameOver(GameHost *sender);

protected:
	explicit GameHost(GameHostConf conf, QObject *parent = 0);

	BoardView _window;

private slots:
	void _windowClosed();
};

class LocalGameHost : public GameHost
{
	Q_OBJECT

public:
	explicit LocalGameHost(LocalGameHostConf conf, QObject *parent = 0);

private:
	Game _game;
	AiPlayer *_player1, *_player2;
};

class NetworkGameHost : public GameHost
{
	Q_OBJECT

public:
	explicit NetworkGameHost(NetworkGameHostConf conf, QObject *parent = 0);

private slots:
	void _error(QString errorString);
	void _joinRequested(QString playerName);

private:
	NetInitiator _init;
	AiPlayer *_player1;
};

class JoinGameHost : public GameHost
{
	Q_OBJECT

public:
	explicit JoinGameHost(JoinNetworkGameHostConf conf, QObject *parent = 0);

private slots:
	void _joinGameFinished(bool success, QString);

private:
	NetOpponent &_opp;
	AiPlayer *_player2;
};

#endif // GAMEHOST_H
