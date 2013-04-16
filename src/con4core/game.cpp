/*
 * game.cpp
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

#include <string>
#include <stdlib.h>
#include "game.h"

using namespace std;

Game::Game() : _nConnect(4), _width(8), _height(8), _depth(4),
    _disksSet(0), _totalDisks(0), _hasStarted(false),
    _aborted(false), _finished(false) {}

Game::Game(int nConnect, int width, int height, int depth)
    : _nConnect(nConnect), _width(width), _height(height),
      _depth(depth), _disksSet(0), _totalDisks(0),
      _hasStarted(false), _aborted(false), _finished(false) {}

Game::~Game() { if (_hasStarted) delete _board; }

bool Game::isConfValid(QString &errMsg)
{
    string msg;
    bool result = Board::isBoardConfValid(_nConnect, _height,
                                          _width, _depth, msg);
    errMsg = QString::fromUtf8(msg.c_str());
    return result;
}

bool Game::set(int width, int depth)
{
    if (!_hasStarted || _finished || _aborted) return false;
    if (_board->set(width, depth)) {
        FieldValue player = curPlayer() == Player1 ? Player2 : Player1;
        _disksSet++;
        emit set(player, width, depth);

        if (_board->isFinished()) {
            _finished = true;
            emit finished(player);
        } else if (_disksSet == _totalDisks) {
            _finished = true;
            emit finished(None);
        }
        return true;
    } else return false;
}

bool Game::undo(int &width, int &height, int &depth)
{
    if (!_hasStarted || _aborted) return false;
    if (_board->undo(height, width, depth)) {
        _finished = false;
        _disksSet--;
        emit undone(curPlayer() == Player1 ? Player2 : Player1,
                    width, height, depth);
        return true;
    } else return false;
}

bool Game::start()
{
    if (_hasStarted || _finished || _aborted) return false;
    QString errMsg;
    if (!isConfValid(errMsg)) return false;
    FieldValue startPlayer = (FieldValue)(rand() % 2 + 1);
    _board = new Board(_nConnect, _height, _width, _depth, startPlayer, true);
    _totalDisks = _height * _width * _depth;
    _hasStarted = true;
    emit started(startPlayer);
    return true;
}

bool Game::abort(FieldValue requester, QString reason)
{
    if (!_hasStarted || _finished || _aborted) return false;
    _aborted = true;
    emit aborted(requester, reason);
    return true;
}
