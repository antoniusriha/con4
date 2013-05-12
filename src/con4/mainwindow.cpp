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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "indexserversview.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow),
	  _application(Application::instance()),
	  _indexServiceList(*_application.indexServices()),
	  _gameHosts(), _joinNetGameConf()
{
	ui->setupUi(this);

	connect(&_indexServiceList, SIGNAL(created(IndexService*,int)),
			this, SLOT(_update()));
	connect(&_indexServiceList, SIGNAL(deletedAt(int)), this, SLOT(_update()));

	ui->networkGameConfView->initialize(_indexServiceList);
	ui->joinGameConf->initialize(_indexServiceList);

//	_localGameConf.setDims(ui->boardConf->dims());

	if (_indexServiceList.empty()) ui->networkGameConfView->setEnabled(false);

//	connect(ui->pageGame, SIGNAL(closing()), this, SLOT(_quitGame()));

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

//	connect(ui->boardConf, SIGNAL(error(QString)),
//			ui->lblStatus, SLOT(setText(QString)));
//	connect(ui->networkBoardConf, SIGNAL(error(QString)),
//			ui->lblStatus, SLOT(setText(QString)));

	connect(ui->joinGameConf, SIGNAL(joinClicked(JoinNetworkGameHostConf)),
			this, SLOT(_joinClicked(JoinNetworkGameHostConf)));
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::gameTypeSelectionChanged()
{
	ui->lblStatus->setText("");
	if (ui->rbNewGame->isChecked()) {
		ui->gbSetupGame->setTitle("Setup new local game");
		ui->stackGameType->setCurrentWidget(ui->pageNewGameSetup);
	} else if (ui->rbNewNetworkGame->isChecked()) {
		ui->gbSetupGame->setTitle("Setup new network game");
		ui->stackGameType->setCurrentWidget(ui->pageNewNetworkGameSetup);
	} else if (ui->rbNetworkGame->isChecked()) {
		ui->gbSetupGame->setTitle("Join a network game");
		ui->stackGameType->setCurrentWidget(ui->pageJoinGameSetup);
	}
}

void MainWindow::viewIndexServersClicked ()
{
	IndexServersView dlg(_indexServiceList, this);
	dlg.exec();
}

void MainWindow::startClicked ()
{
	GameHost *gameHost = GameHost::CreateLocalGame(
				ui->localGameConf->conf(), this);
	connect(gameHost, SIGNAL(quit(GameHost*)),
			this, SLOT(_quitGame(GameHost*)));
	_gameHosts.append(gameHost);
}

void MainWindow::createNetworkGameClicked()
{
	GameHost *gameHost = GameHost::CreateNetworkGame(
				ui->networkGameConfView->conf(), this);
	connect(gameHost, SIGNAL(quit(GameHost*)),
			this, SLOT(_quitGame(GameHost*)));
	_gameHosts.append(gameHost);
}

void MainWindow::_joinClicked(JoinNetworkGameHostConf conf)
{
	GameHost *gameHost = GameHost::JoinNetworkGame(conf, this);
	connect(gameHost, SIGNAL(quit(GameHost*)),
			this, SLOT(_quitGame(GameHost*)));
	_gameHosts.append(gameHost);
}

void MainWindow::_update()
{
	ui->networkGameConfView->setEnabled(!_indexServiceList.empty());
}

void MainWindow::_quitGame(GameHost *sender)
{
	_gameHosts.removeOne(sender);
	delete sender;
}



//void MainWindow::joinGame(QString playerName)
//{
	/*
	QString text = QString("%1 would like to join the game. Accept?")
			.arg(playerName);

	if (QMessageBox::question(this, "Player joining", text,
							  QMessageBox::Yes | QMessageBox::No,
							  QMessageBox::Yes) == QMessageBox::Yes) {
		_oppService->acceptJoinRequest();
		ui->pageGame->setPlayer2Name(playerName);
	} else _oppService->rejectJoinRequest(
				"The server user doesn't want to play with you.");
				*/
//}
