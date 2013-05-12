/*
 * gamehost.cpp
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

#include "gamehost.h"

void GameHostConf::setPlayerColors(QColor player1Color, QColor player2Color)
{
	if (!player1Color.isValid())
		throw ArgumentException("player1Color must be a valid color.");
	if (!player2Color.isValid())
		throw ArgumentException("player2Color must be a valid color.");
	if (player1Color == player2Color)
		throw ArgumentException(
				"The colors of player 1 and player 2 must be different.");
	_player1Color = player1Color;
	_player2Color = player2Color;
}

LocalGameHostConf::LocalGameHostConf() : GameHostConf(), _dims()
{
	setPlayerNames("Player1", "Player2");
}

void LocalGameHostConf::setPlayerNames(QString player1Name, QString player2Name)
{
	if (player1Name.isEmpty())
		throw ArgumentException("player1Name must be a non-empty string.");
	if (player2Name.isEmpty())
		throw ArgumentException("player2Name must be a non-empty string.");
	if (player1Name == player2Name)
		throw ArgumentException(
				"The names of player 1 and player 2 must be different.");
	_player1Name = player1Name;
	_player2Name = player2Name;
}

JoinNetworkGameHostConf::JoinNetworkGameHostConf()
	: GameHostConf(), _opp(0) { setPlayer2Name("Player2"); }

GameHost *GameHost::CreateLocalGame(LocalGameHostConf conf, QObject *parent)
{
	return new LocalGameHost(conf, parent);
}

GameHost *GameHost::CreateNetworkGame(NetworkGameHostConf conf, QObject *parent)
{
	return new NetworkGameHost(conf, parent);
}

GameHost *GameHost::CreateNetworkGameWithAiPlayer(NetworkGameConf conf,
												  QObject *parent)
{
}

GameHost *GameHost::JoinNetworkGame(JoinNetworkGameHostConf conf,
									QObject *parent)
{
	return new JoinGameHost(conf, parent);
}

GameHost *GameHost::JoinNetworkGameWithAiPlayer(JoinNetworkGameHostConf conf,
												QObject *parent)
{
}

GameHost::GameHost(GameHostConf conf, QObject *parent)
	: QObject(parent), _window()
{
	_window.setPlayer1Color(conf.player1Color());
	_window.setPlayer2Color(conf.player2Color());
	_window.show();
	connect(&_window, SIGNAL(closed()), this, SLOT(_windowClosed()));
}

void GameHost::_windowClosed() { emit quit(this); }

LocalGameHost::LocalGameHost(LocalGameHostConf conf, QObject *parent)
	: GameHost(conf, parent), _game(conf.dims(), this)
{
	_window.setPlayer1Name(conf.player1Name());
	_window.setPlayer2Name(conf.player2Name());
	_window.startGame(&_game);
	_game.start();
}

NetworkGameHost::NetworkGameHost(NetworkGameHostConf conf, QObject *parent)
	: GameHost(conf, parent), _init(conf, this)
{
	_window.setPlayer1Name(conf.initiatorName().string());
	connect(&_init, SIGNAL(registerGameFinished(bool,QList<ErroneousService>)),
			this, SLOT(_registerGameFinished(bool,QList<ErroneousService>)));
	_init.registerGame();
}

void NetworkGameHost::_registerGameFinished(bool success,
											QList<ErroneousService> errServices)
{
	if (success) _window.startGame(_init.game(), true, false);
	else {
		QString errMsg = "";
		for (int i = 0; i < errServices.size(); i++) {
			IndexService *svc = errServices.at(i).service;
			errMsg += QString("Index server <em>%1</em> (%2:%3) failed: %4\n")
					.arg(svc->name()).arg(svc->ipAddress().toString())
					.arg(svc->port()).arg(errServices.at(i).errorString);
		}
		QMessageBox::critical(&_window, "Registration failure", errMsg);
		_window.close();
	}
}

JoinGameHost::JoinGameHost(JoinNetworkGameHostConf conf, QObject *parent)
	: GameHost(conf, parent), _opp(*conf.opponent())
{
	_window.setPlayer2Name(conf.player2Name().string());
	connect(&_opp, SIGNAL(joinGameFinished(bool,QString)),
			this, SLOT(_joinGameFinished(bool,QString)));
	_opp.connectToServer();
	_opp.join();
}

void JoinGameHost::_joinGameFinished(bool success, QString errString)
{
	if (success) _window.startGame(_opp.game(), false, true);
}
