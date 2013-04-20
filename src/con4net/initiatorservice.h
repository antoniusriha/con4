/*
 * initiatorservice.h
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

#ifndef INITIATORSERVICE_H
#define INITIATORSERVICE_H

#include <QTcpSocket>
#include <QTimer>
#include <QStringList>
#include "networkplayerservice.h"

class InitiatorService : public NetworkPlayerService
{
	Q_OBJECT

public:
	InitiatorService(NetworkGame *game, QObject *parent = 0);
	~InitiatorService();

	bool join(QString &errMsg);
	void move(int fieldNumber) const;
	void abortGame(QString reason) const;

signals:
	void joinGameSuccess();
	void joinGameFailed(QString reason);
	void startGame();
	void synchronizeGameBoard(int **fieldNumberAndValue);
	void updatedGameBoard(int fieldNumber, FieldValue value);
	void movedFailed(QString reason);
	void endGame(int result);
	void abortGame(QString reason);

private slots:
	void update();
	void set(FieldValue player, int width, int depth);

private:
	bool _connect();
	QStringList _sendMsg (QString msg);
	void _handleMsg(QStringList msgTokens);
	void _handleSyncGameBoard(QStringList msgTokens);
	void _handleUpdateGameBoard(QStringList msgTokens);
	void _abort(QString reason);
	bool _getCoords(int fieldNumber, int &width, int &height, int &depth);

	bool _joined;
	QTcpSocket _socket;
	QTimer _timer;
};

#endif // INITIATORSERVICE_H
