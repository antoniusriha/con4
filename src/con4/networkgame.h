/*
 * networkgame.h
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

#ifndef NETWORKGAME_H
#define NETWORKGAME_H

#include <QString>
#include <QUuid>
#include <QtNetwork/QHostAddress>
#include "game.h"

class NetworkGame : public Game {
    Q_OBJECT
public:
    NetworkGame ();
    NetworkGame (QHostAddress host, quint16 port);
    NetworkGame (QString name, QString player1, QHostAddress host, quint16 port,
                 int width, int height, int depth, bool hasStarted);
    ~NetworkGame ();

    QString name () const { return _name; }
    QHostAddress host () const { return _host; }
    quint16 port () const { return _port; }
    QUuid guid () const { return _guid; }

    void setName (QString value) { _name = value; }
    void setGuid (QUuid value) { _guid = value; }

    bool areSettingsValid () const;
private:
    const bool _isInitiator;
    QUuid _guid;
    QString _name;
    QHostAddress _host;
    quint16 _port;
};

#endif // NETWORKGAME_H
