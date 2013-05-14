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
#include "randomsetplayer.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow),
	  _indexServiceList(*Application::instance().indexServices()),
	  _aiFactories(), _gameHosts()
{
	ui->setupUi(this);

	connect(&_indexServiceList, SIGNAL(created(IndexService*,int)),
			this, SLOT(_update()));
	connect(&_indexServiceList, SIGNAL(deletedAt(int)), this, SLOT(_update()));

	_aiFactories.append(new RandomSetPlayerInfo(this));

	ui->localGameConf->initialize(_aiFactories);
	ui->networkGameConfView->initialize(_indexServiceList, _aiFactories);
	ui->joinGameConf->initialize(_indexServiceList, _aiFactories);

	if (_indexServiceList.empty()) ui->networkGameConfView->setEnabled(false);

	connect(ui->joinGameConf, SIGNAL(joinClicked(JoinNetworkGameHostConf)),
			this, SLOT(_joinClicked(JoinNetworkGameHostConf)));
}

MainWindow::~MainWindow()
{
	qDeleteAll(_aiFactories);
	delete ui;
}

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
	connect(gameHost, SIGNAL(gameOver(GameHost*)),
			this, SLOT(_quitGame(GameHost*)));
	_gameHosts.append(gameHost);
}

void MainWindow::createNetworkGameClicked()
{
	GameHost *gameHost = GameHost::CreateNetworkGame(
				ui->networkGameConfView->conf(), this);
	connect(gameHost, SIGNAL(gameOver(GameHost*)),
			this, SLOT(_quitGame(GameHost*)));
	_gameHosts.append(gameHost);
}

void MainWindow::_joinClicked(JoinNetworkGameHostConf conf)
{
	GameHost *gameHost = GameHost::JoinNetworkGame(conf, this);
	connect(gameHost, SIGNAL(gameOver(GameHost*)),
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

void MainWindow::closeEvent(QCloseEvent *event)
{
	if (_gameHosts.isEmpty()) event->accept();
	else {
		QMessageBox::information(this, "Window close", "The main window cannot "
								 "be closed, when games are still running. "
								 "End all games and try again.");
		event->ignore();
	}
}
