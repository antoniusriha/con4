/*
 * game.h
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

#ifndef GAME_H
#define GAME_H

#include <QObject>
#include "board.h"
#include "gameinterface.h"

class Game : public QObject {
    Q_OBJECT
public:
    Game (GameInterface *initiator);
    virtual ~Game ();

    const GameInterface *const initiator () const { return _initiator; }
    GameInterface *opponent () const { return _opponent; }
    int height () const { return _height; }
    int width () const { return _width; }
    int depth () const { return _depth; }
    bool hasStarted () const { return _hasStarted; }

    void setOpponent (GameInterface *value) { _opponent = value; }
    void setHeight (int value) { _height = value; }
    void setWidth (int value) { _width = value; }
    void setDepth (int value) { _depth = value; }

    virtual bool areSettingsValid () const;
protected:
    bool _hasStarted;

private:
    Game (const Game &);
    Game &operator= (const Game &);

    const GameInterface *const _initiator;
    GameInterface *_opponent;
    int _height, _width, _depth;
    Board *_board;

    friend class GameInterface;
};

#endif // GAME_H
