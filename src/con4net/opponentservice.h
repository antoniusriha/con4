/*
 * opponentservice.h
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

#ifndef OPPONENTSERVICE_H
#define OPPONENTSERVICE_H

#include <QUuid>
#include <QThread>
#include <QTcpServer>
#include "networkplayerservice.h"

class OppServerThread : public QThread
{
    Q_OBJECT

public:
    OppServerThread (int socketDescriptor, QObject *parent = 0);

private:
    void run ();

    int _socketDescriptor;
};

class OppServer : public QTcpServer
{
    Q_OBJECT

protected:
    void incomingConnection (int socketDescriptor);
};

class OpponentService : public NetworkPlayerService
{
    Q_OBJECT

public:
	OpponentService (Game *game, QString initiatorName,
					 QString gameName, QObject *parent);

    quint16 port () const {
        return _hasStarted ? _oppServer.serverPort () : (quint16)0;
    }

    QUuid guid () const { return _guid; }

    bool startService ();

    void joinGameSuccess () const;
    void joinGameFailed (QString reason) const;
    void startGame () const;
    void synchronizeGameBoard (int **fieldNumberAndValue) const;
    void updatedGameBoard (int fieldNumber, int value) const;
    void moved_failed (QString reason) const;
    void endGame (int result) const;
    void abortGame (QString reason) const;

signals:
    void joinGame (Game *game);
    void move (int fieldIndex);
    void abortGame (QString reason);

private:
    bool _hasStarted;
    QUuid _guid;
    OppServer _oppServer;
};

#endif // OPPONENTSERVICE_H
