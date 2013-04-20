/*
 * mainwindow.cpp
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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "indexserversview.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow),
	  _application(Application::instance()), _currentGame(0), _oppService(0),
	  _initService(0),
	  _networkGames(new NetworkGameList(_application.indexServices(), this))
{
	ui->setupUi(this);
	_player1Color = QColor(Qt::red);
	_player2Color = QColor(Qt::yellow);
	_setPlayerColor(_player1Color, ui->btnPlayer1);
	_setPlayerColor(_player2Color, ui->btnPlayer2);

	connect(_application.indexServices(), SIGNAL(created(IndexService *)),
			this, SLOT(indexServerCountChanged()));
	connect(_application.indexServices(), SIGNAL(deletedAt(int)),
			this, SLOT(indexServerCountChanged()));

	if (_application.indexServices()->empty()) {
		ui->gbNetworkPlayer->setEnabled(false);
		ui->networkBoardConf->setEnabled(false);
		ui->tvJoinGames->setEnabled(false);
		ui->btnRefresh->setEnabled(false);
		ui->btnJoin->setEnabled(false);
	}

	ui->tvJoinGames->setModel(_networkGames);

	_localGame = new Game(4, ui->boardConf->boardWidth(),
						  ui->boardConf->boardHeight(),
						  ui->boardConf->boardDepth());

	connect(ui->pageGame, SIGNAL(close()), this, SLOT(closeGame()));

	//	QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
	//	// use the first non-localhost IPv4 address
	//	for (int i = 0; i < ipAddressesList.size(); i++) {
	//		if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
	//			ipAddressesList.at(i).toIPv4Address()) {
	//			_ipAddress = ipAddressesList.at(i);
	//			break;
	//		}
	//	}
	//	// if we did not find one, use IPv4 localhost
	//	if (_ipAddress.isNull())
	//		_ipAddress = QHostAddress(QHostAddress::LocalHost);
}

MainWindow::~MainWindow() {
	delete ui;
	delete _localGame;
}

void MainWindow::gameTypeSelectionChanged () {
	ui->lblStatus->setText ("");
	if (ui->rbNewGame->isChecked ())
		ui->stackGameType->setCurrentWidget (ui->pageNewGameSetup);
	else if (ui->rbNewNetworkGame->isChecked ())
		ui->stackGameType->setCurrentWidget (ui->pageNewNetworkGameSetup);
	else ui->stackGameType->setCurrentWidget (ui->pageJoinGameSetup);
}

void MainWindow::viewIndexServersClicked () {
	IndexServersView dlg (_application.indexServices(), this);
	dlg.exec ();
}

void MainWindow::startClicked () {
	_currentGame = _localGame;
	_localGame->setWidth(ui->boardConf->boardWidth());
	_localGame->setHeight(ui->boardConf->boardHeight());
	_localGame->setDepth(ui->boardConf->boardDepth());

	QString errMsg;
	if (!_localGame->isConfValid(errMsg)) {
		QMessageBox::critical(this, "Local game creation error", errMsg);
		return;
	}

	GLWidget *gameWidget = ui->pageGame;
	gameWidget->startGame(_localGame);
	gameWidget->setPlayer1Color(_player1Color);
	gameWidget->setPlayer2Color(_player2Color);
	ui->stackAppView->setCurrentWidget (gameWidget);
	gameWidget->setGameTitle("Local game");
	_localGame->start();
}

void MainWindow::createNetworkGameClicked()
{
	try {
		QHostAddress ipAddress (ui->txtIpAddress->text());
		_oppService = new OpponentService(ui->networkBoardConf->boardWidth(),
										  ui->networkBoardConf->boardHeight(),
										  ui->networkBoardConf->boardDepth(),
										  ui->txtPlayerName->text(),
										  ui->txtGameName->text(), ipAddress,
										  (quint16)ui->sbPort->value(),
										  _application.indexServices());
	} catch (runtime_error &ex) {
		QMessageBox::critical(this, "Network game creation error", ex.what());
		return;
	}

	_currentGame = _oppService->networkGame();
	QString errMsg;
	if (!_currentGame->isConfValid(errMsg)) {
		QMessageBox::critical(this, "Network game creation error", errMsg);
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
}

void MainWindow::joinClicked()
{
	QItemSelectionModel *selModel = ui->tvJoinGames->selectionModel();
	if (!selModel->hasSelection()) return;

	QModelIndex index = selModel->selectedRows().at(0);
	QModelIndex parent = index.parent();
	if (parent == QModelIndex()) return;

	NetworkGame *game = _application.indexServices()->at(
				parent.row())->games()->at(index.row());

	_initService = new InitiatorService(game);

	QString errMsg;
	if (!_initService->join(errMsg)) {
		QMessageBox::critical(this, "Join game error", errMsg);
		delete _initService;
		return;
	}

	_currentGame = game;
	GLWidget *gameWidget = ui->pageGame;
	gameWidget->startGame(_currentGame, false, true);
	gameWidget->setPlayer2Name("My name");
	gameWidget->setPlayer1Color(_player1Color);
	gameWidget->setPlayer2Color(_player2Color);
	ui->stackAppView->setCurrentWidget(gameWidget);
	gameWidget->setGameTitle("Remotely initialized network game");
}

void MainWindow::refreshClicked()
{
	_networkGames->refresh();
	ui->tvJoinGames->expandAll();
	ui->lblStatus->setText(_networkGames->refreshLog());
}

void MainWindow::player1Clicked()
{
	QColorDialog dlg;
	dlg.setCurrentColor(_player1Color);
	dlg.exec();
	_player1Color = dlg.selectedColor();
	_setPlayerColor(_player1Color, ui->btnPlayer1);
}

void MainWindow::player2Clicked()
{
	QColorDialog dlg;
	dlg.setCurrentColor(_player2Color);
	dlg.exec();
	_player2Color = dlg.selectedColor();
	_setPlayerColor(_player2Color, ui->btnPlayer2);
}

void MainWindow::closeGame()
{
	if (_currentGame == _localGame) {
		delete _localGame;
		_localGame = new Game(4, ui->boardConf->boardWidth(),
							  ui->boardConf->boardHeight(),
							  ui->boardConf->boardDepth());
	} else {
		if (_oppService) {
			_currentGame = 0;
			delete _oppService;
		} else if (_initService) {
			_currentGame = 0;
			delete _initService;
		}
	}

	ui->stackAppView->setCurrentWidget(ui->pageGameConf);
}

void MainWindow::indexServerCountChanged()
{
	bool enable = !_application.indexServices()->empty();
	ui->gbNetworkPlayer->setEnabled(enable);
	ui->networkBoardConf->setEnabled(enable);
	ui->tvJoinGames->setEnabled(enable);
	ui->btnRefresh->setEnabled(enable);
	ui->btnJoin->setEnabled(enable);
}

void MainWindow::joinGame(QString playerName)
{
	QString text = QString("%1 would like to join the game. Accept?")
			.arg(playerName);

	if (QMessageBox::question(this, "Player joining", text,
							  QMessageBox::Yes | QMessageBox::No,
							  QMessageBox::Yes) == QMessageBox::Yes) {
		_oppService->joinGameSuccess();
		ui->pageGame->setPlayer2Name(playerName);
	} else _oppService->joinGameFailed(
				"The server user doesn't want to play with you.");
}

// from: http://stackoverflow.com/questions/282938/show-result-of-color-
// selection-in-qt
void MainWindow::_setPlayerColor(QColor color, QPushButton *button)
{
	const QString style("QPushButton { background-color : %1; color : %2; }");
	QColor idealTextColor = _getIdealTextColor(color);
	button->setStyleSheet(style.arg(color.name()).arg(idealTextColor.name()));
}

//==============================================================================
//  Nom : getIdealTextColor
//! @return an ideal label color, based on the given background color.
//! Based on http://www.codeproject.com/cs/media/IdealTextColor.asp
//==============================================================================
QColor MainWindow::_getIdealTextColor(const QColor& rBackgroundColor) const
{
	const int THRESHOLD = 105;
	int BackgroundDelta = (rBackgroundColor.red() * 0.299) +
			(rBackgroundColor.green() * 0.587) +
			(rBackgroundColor.blue() * 0.114);
	return QColor((255- BackgroundDelta < THRESHOLD) ? Qt::black : Qt::white);
}
