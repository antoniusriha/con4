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

#include <stdexcept>
#include <QObject>
#include <QVector>
#include "board.h"

class Game : public QObject
{
	Q_OBJECT

public:
	class Dimensions
	{
	public:
		class Exception : public std::invalid_argument
		{
		public:
			explicit Exception(QString what)
				: invalid_argument(what.toStdString()) {}
		};

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

	class BoardIndex
	{
	public:
		class Exception : public std::out_of_range
		{
		public:
			explicit Exception() : out_of_range(_what) {}
		private:
			const static char *_what;
		};

		BoardIndex();
		BoardIndex(const Game &game, int wVal, int hVal, int dVal);

		const Game *game() const { return _game; }
		bool isValid() const { return _game != 0; }

		int wVal() const { return _wVal; }
		int hVal() const { return _hVal; }
		int dVal() const { return _dVal; }

		void setWVal(int value);
		void setHVal(int value);
		void setDVal(int value);

	private:
		const Game *_game;
		int _wVal, _hVal, _dVal;
	};

	class InvalidIndexException : public std::invalid_argument
	{
	public:
		explicit InvalidIndexException() : invalid_argument(_what) {}
	private:
		const static char *_what;
	};

	class InvalidOperationException : public std::logic_error
	{
	public:
		explicit InvalidOperationException(QString what)
			: logic_error(what.toStdString()) {}
	};

	const static char *invalidIdxErrMsg;

	Game(Dimensions dims = Dimensions(), QObject *parent = 0);
	virtual ~Game();

	Dimensions dims() const;
	void setDims(Dimensions dims);

	BoardIndex index() const;
	BoardIndex index(int wVal, int dVal, int hVal = 0) const;

	FieldValue curPlayer() const { return _board.curPlayer(); }
	FieldValue winner() const { return _finished ? _board.winner() : None; }
	bool isDraw() const { return _finished && winner() == None; }
	Board board() const { return _board; }
	bool canUndo() const { return _hasStarted; }
	bool hasStarted() const { return _hasStarted; }
	bool finished() const { return _finished; }
	bool aborted() const { return _aborted; }
	int setCount() const { return _disksSet; }

	// Queries
	bool full(BoardIndex &idx) const;
	bool connected(QVector<BoardIndex> &wVals) const;

	// Data access
	FieldValue get(BoardIndex idx) const;

	// Data manipulation
	bool set(BoardIndex &idx);
	bool undo(BoardIndex &idx);

	void start(FieldValue startPlayer = None);
	void abort(FieldValue requester, QString reason);

signals:
	void started(FieldValue startPlayer);
	void set(FieldValue player, BoardIndex index);
	void undone(FieldValue player, BoardIndex index);
	void finished(FieldValue winner);
	void aborted(FieldValue requester, QString reason);

private:
	int _disksSet, _totalDisks;
	bool _hasStarted, _aborted, _finished;
	Board _board;
};

#endif // GAME_H
