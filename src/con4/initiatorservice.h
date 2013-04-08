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

#include "networkgame.h"
#include "service.h"

class InitiatorService : public Service {
    Q_OBJECT
public:
    InitiatorService (QHostAddress host, quint16 port);

    void joinGame (NetworkGame &game) const;
    void move (int fieldNumber) const;
    void abortGame (QString reason) const;

signals:
    void joinGameSuccess ();
    void joinGameFailed (QString reason);
    void startGame ();
    void synchronizeGameBoard (int **fieldNumberAndValue);
    void updatedGameBoard (int fieldNumber, FieldValue value);
    void moved_failed (QString reason);
    void endGame (int result);
    void abortGame (QString reason);
};

#endif // INITIATORSERVICE_H
