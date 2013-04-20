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

class Game : public QObject
{
	Q_OBJECT

public:
	Game();
	Game(int nConnect, int width, int height, int depth);
	virtual ~Game();

	int nConnect() const { return _nConnect; }
	int height() const { return _height; }
	int width() const { return _width; }
	int depth() const { return _depth; }

	bool setNConnect(int value)
	{
		if (_hasStarted) return false;
		_nConnect = value;
		return true;
	}

	bool setHeight(int value)
	{
		if (_hasStarted) return false;
		_height = value;
		return true;
	}

	bool setWidth(int value)
	{
		if (_hasStarted) return false;
		_width = value;
		return true;
	}

	bool setDepth(int value)
	{
		if (_hasStarted) return false;
		_depth = value;
		return true;
	}

	FieldValue curPlayer() const
	{
		return _hasStarted ? _board->curPlayer() : None;
	}

	FieldValue winner() const { return _finished ? _board->winner() : None; }
	bool isDraw() const { return _finished && winner() == None; }
	Board board() const { return _hasStarted ? *_board : Board(); }
	bool canUndo() const { return _hasStarted; }
	bool hasStarted() const { return _hasStarted; }
	bool finished() const { return _finished; }
	bool aborted() const { return _aborted; }
	int setCount() const { return _disksSet; }

	// Queries
	bool isConfValid(QString &errMsg);
	int nDims() const { return depth() == 1 ? 2 : 3; }

	bool isFull(int width, int depth, int &height)
	{
		return _hasStarted ? _board->isFull(width, depth, height) : false;
	}

	bool connected(int widthIdcs[], int heightIdcs[], int depthIdcs[]) const
	{
		if (!_hasStarted) return false;
		return _board->connected(heightIdcs, widthIdcs, depthIdcs);
	}

	// Data access
	FieldValue get(int width, int height, int depth) const
	{
		return _hasStarted ? _board->get(height, width, depth) : None;
	}

	// Data manipulation
	bool set(int width, int depth);
	bool undo(int &width, int &height, int &depth);

	bool start(FieldValue startPlayer = None);
	bool abort(FieldValue requester, QString reason);

signals:
	void started(FieldValue startPlayer);
	void set(FieldValue player, int width, int depth);
	void undone(FieldValue player, int width, int height, int depth);
	void finished(FieldValue winner);
	void aborted(FieldValue requester, QString reason);

private:
	Game(const Game &);
	Game &operator=(const Game &);

	int _nConnect, _width, _height, _depth, _disksSet, _totalDisks;
	bool _hasStarted, _aborted, _finished;
	Board *_board;
};

#endif // GAME_H
