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

#include "con4globals.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "indexserversview.h"
#include "application.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	_player1Color = QColor(Qt::red);
	_player2Color = QColor(Qt::yellow);
	_setPlayerColor(_player1Color, ui->btnPlayer1);
	_setPlayerColor(_player2Color, ui->btnPlayer2);

	if (Application::instance().indexServices()->empty()) {
		ui->gbNetworkPlayer->setEnabled(false);
		ui->networkBoardConf->setEnabled(false);
		ui->tvJoinGames->setEnabled(false);
		ui->btnRefresh->setEnabled(false);
		ui->btnJoin->setEnabled(false);
	}

	_localGame = new Game(4, ui->boardConf->boardWidth(),
						  ui->boardConf->boardHeight(),
						  ui->boardConf->boardDepth());

	_networkGame = new Game(4, ui->networkBoardConf->boardWidth(),
							ui->networkBoardConf->boardHeight(),
							ui->networkBoardConf->boardDepth());

	connect(ui->pageGame, SIGNAL(close()), this, SLOT(closeGame()));
}

MainWindow::~MainWindow() {
    delete ui;
	delete _localGame;
	delete _networkGame;
}

void MainWindow::gameTypeSelectionChanged (bool state) {
    ui->lblStatus->setText ("");
    if (ui->rbNewGame->isChecked ())
        ui->stackGameType->setCurrentWidget (ui->pageNewGameSetup);
	else if (ui->rbNewNetworkGame->isChecked ())
		ui->stackGameType->setCurrentWidget (ui->pageNewNetworkGameSetup);
	else ui->stackGameType->setCurrentWidget (ui->pageJoinGameSetup);
}

void MainWindow::viewIndexServersClicked () {
    IndexServersView dlg (this);
    dlg.exec ();
}

void MainWindow::startClicked () {
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
}

void MainWindow::createNetworkGameClicked()
{

}

void MainWindow::joinClicked()
{
}

void MainWindow::refreshClicked()
{
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
	delete _localGame;
	_localGame = new Game(4, ui->boardConf->boardWidth(),
						  ui->boardConf->boardHeight(),
						  ui->boardConf->boardDepth());
	ui->stackAppView->setCurrentWidget(ui->pageGameConf);
}

// from: http://stackoverflow.com/questions/282938/show-result-of-color-selection-in-qt
void MainWindow::_setPlayerColor(QColor color, QPushButton *button)
{
	const QString COLOR_STYLE("QPushButton { background-color : %1; color : %2; }");
	QColor idealTextColor = _getIdealTextColor(color);
	button->setStyleSheet(COLOR_STYLE.arg(color.name()).arg(idealTextColor.name()));
}

//==============================================================================
//  Nom : getIdealTextColor
//! @return an ideal label color, based on the given background color.
//! Based on http://www.codeproject.com/cs/media/IdealTextColor.asp
//==============================================================================
QColor MainWindow::_getIdealTextColor(const QColor& rBackgroundColor) const
{
	const int THRESHOLD = 105;
	int BackgroundDelta = (rBackgroundColor.red() * 0.299) + (rBackgroundColor.green() * 0.587) + (rBackgroundColor.blue() * 0.114);
	return QColor((255- BackgroundDelta < THRESHOLD) ? Qt::black : Qt::white);
}
