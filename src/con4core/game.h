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
	class Dimensions
	{
	public:
		const static int MaxDim = 100;

		Dimensions();
		Dimensions(int nConnect, int width, int height, int depth);

		int nConnect() const { return _nConnect; }
		int width() const { return _width; }
		int height() const { return _height; }
		int depth() const { return _depth; }

		void setNConnect(int value);
		void setWidth(int value);
		void setHeight(int value);
		void setDepth(int value);

		int nDims() const { return _depth == 1 ? 2 : 3; }

	private:
		void _init(int nConnect, int width, int height, int depth);

		int _nConnect, _width, _height, _depth;
	};

	Game(Dimensions dims = Dimensions(), QObject *parent = 0);
	virtual ~Game();

	Dimensions dims() const { return _dims; }
	void setDims(Dimensions dims);

	FieldValue curPlayer() const;
	FieldValue winner() const { return _finished ? _board->winner() : None; }
	bool isDraw() const { return _finished && winner() == None; }
	Board board() const { return _hasStarted ? *_board : Board(); }
	bool canUndo() const { return _hasStarted; }
	bool hasStarted() const { return _hasStarted; }
	bool finished() const { return _finished; }
	bool aborted() const { return _aborted; }
	int setCount() const { return _disksSet; }

	// Queries
	int nDims() const { return _dims.nDims(); }
	bool full(int wVal, int dVal, int &hVal);
	bool connected(int wVals[], int hVals[], int dVals[]) const;

	// Data access
	FieldValue get(int wVal, int hVal, int dVal) const;

	// Data manipulation
	bool set(int wVal, int dVal, int &hVal);
	bool undo(int &wVal, int &hVal, int &dVal);

	bool start(FieldValue startPlayer = None);
	bool abort(FieldValue requester, QString reason);

signals:
	void started(FieldValue startPlayer);
	void set(FieldValue player, int wVal, int hVal, int dVal);
	void undone(FieldValue player, int wVal, int hVal, int dVal);
	void finished(FieldValue winner);
	void aborted(FieldValue requester, QString reason);

private:
	Dimensions _dims;
	int _disksSet, _totalDisks;
	bool _hasStarted, _aborted, _finished;
	Board *_board;
};

#endif // GAME_H
