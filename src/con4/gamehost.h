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

#include <QObject>
#include <QColor>
#include "../con4net/opponentservice.h"
#include "../con4net/initiatorservice.h"
#include "../con4gl/glwidget.h"

class GameConf
{
public:
	GameConf();

	QColor player1Color() const { return _player1Color; }
	QColor player2Color() const { return _player2Color; }
	void setPlayerColors (QColor player1Color, QColor player2Color);

private:
	QColor _player1Color, _player2Color;
};

class LocalGameConf : public GameConf
{
public:
	LocalGameConf();

	Game::Dimensions dims() const { return _dims; }
	void setDims(Game::Dimensions value) { _dims = value; }

	QString player1Name() const { return _player1Name; }
	QString player2Name() const { return _player2Name; }
	void setPlayerNames(QString player1Name, QString player2Name);
private:
	Game::Dimensions _dims;
	QString _player1Name, _player2Name;
};

class NetworkGameConf : public GameConf
{
public:
	NetworkGameConf(QList<IndexService *> indexServices);

	Game::Dimensions dims() const { return _dims; }
	void setDims(Game::Dimensions value) { _dims = value; }

	QString player1Name() const { return _player1Name; }
	void setPlayer1Name(QString value);

	QString gameName() const { return _gameName; }
	void setGameName(QString value);

	QHostAddress ipAddress() const { return _ipAddress; }
	void setIpAddress(QHostAddress value);

	quint16 port() const { return _port; }
	void setPort(quint16 value);

	QList<IndexService *> indexServices() const { return _indexServices; }
	void setIndexServices(QList<IndexService *> value);

private:
	Game::Dimensions _dims;
	QString _player1Name, _gameName;
	QHostAddress _ipAddress;
	quint16 _port;
	QList<IndexService *> _indexServices;
};

class JoinNetworkGameConf : public GameConf
{
public:
	JoinNetworkGameConf(NetworkGame *game);

	QString player2Name() const { return _player2Name; }
	void setPlayer2Name(QString value);

	NetworkGame *game() const { return _game; }
	void setGame(NetworkGame *value);

private:
	QString _player2Name;
	NetworkGame *_game;
};

class GameHost : public QObject
{
	Q_OBJECT
public:
	static GameHost *CreateLocalGame(LocalGameConf conf, QObject *parent = 0);
	static GameHost *CreateNetworkGame(NetworkGameConf conf,
									   QObject *parent = 0);
	static GameHost *CreateNetworkGameWithAiPlayer(NetworkGameConf conf,
												   QObject *parent = 0);
	static GameHost *JoinNetworkGame(JoinNetworkGameConf conf,
									 QObject *parent = 0);
	static GameHost *JoinNetworkGameWithAiPlayer(JoinNetworkGameConf conf,
												 QObject *parent = 0);

private:
	explicit GameHost(QObject *parent = 0);
	
private:
	Game *_game;
	Player *_player1, *_player2;
	GLWidget *_window;
};

#endif // GAMEHOST_H
