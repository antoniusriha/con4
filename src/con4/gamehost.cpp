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

#include <stdexcept>
#include "gamehost.h"

using namespace std;

GameConf::GameConf()
{
	setPlayerColors(QColor(Qt::red), QColor(Qt::yellow));
}

void GameConf::setPlayerColors(QColor player1Color, QColor player2Color)
{
	if (!player1Color.isValid())
		throw invalid_argument("player1Color must be a valid color.");
	if (!player2Color.isValid())
		throw invalid_argument("player2Color must be a valid color.");
	if (player1Color == player2Color)
		throw invalid_argument(
				"The colors of player 1 and player 2 must be different.");
	_player1Color = player1Color;
	_player2Color = playre2Color;
}

LocalGameConf::LocalGameConf() : GameConf(), _dims()
{
	setPlayerNames("Player1", "Player2");
}

void LocalGameConf::setPlayerNames(QString player1Name, QString player2Name)
{
	if (player1Name.isEmpty())
		throw invalid_argument("player1Name must be a non-empty string.");
	if (player2Name.isEmpty())
		throw invalid_argument("player2Name must be a non-empty string.");
	if (player1Name == player2Name)
		throw invalid_argument(
				"The names of player 1 and player 2 must be different.");
	_player1Name = player1Name;
	_player2Name = player2Name;
}

NetworkGameConf::NetworkGameConf(QList<IndexService *> indexServices)
	: GameConf(), _dims()
{
	setPlayer1Name("Player1");
	setGameName(_player1Name + "'s network game");
	setIpAddress(QHostAddress::LocalHost);
	setPort(9999);
	setIndexServices(indexServices);
}

void NetworkGameConf::setPlayer1Name(QString value)
{
	if (value.isEmpty())
		throw invalid_argument("value must be a non-empty string.");
	if (value.contains(';') || value.contains('\n')) throw invalid_argument(
				"value must not contain the characters '\n' or ';'.");
	_player1Name = value;
}

void NetworkGameConf::setGameName(QString value)
{
	if (value.isEmpty())
		throw invalid_argument("value must be a non-empty string.");
	if (value.contains(';') || value.contains('\n')) throw invalid_argument(
				"value must not contain the characters '\n' or ';'.");
	_gameName = value;
}

void NetworkGameConf::setIpAddress(QHostAddress value)
{
	if (value.isNull())
		throw invalid_argument("value must be a non-null host address.");
	_ipAddress = value;
}

void NetworkGameConf::setPort(quint16 value)
{
	if (value < 1025)
		throw invalid_argument("value must be a port number above 1024.");
	_port = value;
}

void NetworkGameConf::setIndexServices(QList<IndexService *> value)
{
	if (value.empty()) throw invalid_argument(
				"value must be a non-empty list of index services.");
	_indexServices = value;
}

JoinNetworkGameConf::JoinNetworkGameConf(NetworkGame *game) : GameConf()
{
	setPlayer2Name("Player2");
	setGame(game);
}

void JoinNetworkGameConf::setPlayer2Name(QString value)
{
	if (value.isEmpty())
		throw invalid_argument("value must be a non-empty string.");
	if (value.contains(';') || value.contains('\n')) throw invalid_argument(
				"value must not contain the characters '\n' or ';'.");
	_player2Name = value;
}

void JoinNetworkGameConf::setGame(NetworkGame *value)
{
	if (!value) throw invalid_argument("value must not be a null pointer.");
	if (_game->hasStarted())
		throw invalid_argument("The provided game has already started.");
	_game = game;
}

GameHost *GameHost::CreateLocalGame(LocalGameConf conf, QObject *parent)
{
	GameHost *host = new GameHost(parent);
	host->_game = new Game(conf.dims(), host);
	host->_window->setPlayer1Color(conf.player1Color());
	host->_window->setPlayer2Color(conf.player2Color());
	host->_window->setPlayer1Name(conf.player1Name());
	host->_window->setPlayer2Name(conf.player2Name());
	return host;
}

GameHost *GameHost::CreateNetworkGame(NetworkGameConf conf, QObject *parent)
{
	GameHost *host = new GameHost(parent);
	host->_player2 = new OpponentService(conf.indexServices(), host);

	/*

	QHostAddress ipAddress (ui->txtIpAddress->text());
	_oppService = new OpponentService(ui->networkBoardConf->boardWidth(),
									  ui->networkBoardConf->boardHeight(),
									  ui->networkBoardConf->boardDepth(),
									  ui->txtPlayerName->text(),
									  ui->txtGameName->text(), ipAddress,
									  (quint16)ui->sbPort->value(),
									  _application.indexServices());
	QString startErrMsg;
	if (!_oppService->startService(&startErrMsg)) {
		QMessageBox::critical(this, "Network game creation error", startErrMsg);
		delete _oppService;
		return;
	}

	_currentGame = _oppService->networkGame();
	QString errMsg;
	if (!_currentGame->isConfValid(errMsg)) {
		QMessageBox::critical(this, "Network game creation error", errMsg);
		delete _oppService;
		return;
	}

	connect(_oppService, SIGNAL(joinGame(QString)),
			this, SLOT(joinGame(QString)));

	GLWidget *gameWidget = ui->pageGame;
	gameWidget->startGame(_currentGame, true, false);
	gameWidget->setPlayer1Name(ui->txtPlayerName->text());
	gameWidget->setPlayer1Color(_player1Color);
	gameWidget->setPlayer2Color(_player2Color);
	ui->stackAppView->setCurrentWidget(gameWidget);
	gameWidget->setGameTitle("Locally initialized network game");

	  */
}

GameHost *GameHost::CreateNetworkGameWithAiPlayer(NetworkGameConf conf,
												  QObject *parent)
{
}

GameHost *GameHost::JoinNetworkGame(JoinNetworkGameConf conf, QObject *parent)
{
}

GameHost *GameHost::JoinNetworkGameWithAiPlayer(JoinNetworkGameConf conf,
												QObject *parent)
{
}

GameHost::GameHost(QObject *parent) : QObject(parent), _window(this)
{
}
