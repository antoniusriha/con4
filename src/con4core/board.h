/*
 * board.h
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

#ifndef BOARD_H
#define BOARD_H

#include <list>
#include <stack>
#include <string>
#include <boost/multi_array.hpp>

enum FieldValue
{
	None,
	Player1,
	Player2
};

class Board
{
public:
	Board();
	Board(int nConnect, int height, int dim2, int dim3,
		  FieldValue curPlayer = Player1, bool canUndo = false);
	Board(const Board &board, bool canUndo); // custom copy

	// Data access & simple queries
    int nDims() const { return dim3() == 1 ? 2 : 3; }
    int height() const { return *_array.shape(); }
    int dim2() const { return *(_array.shape() + 1); }
    int dim3() const { return *(_array.shape() + 2); }
	bool finished() const { return _finished; }
	FieldValue curPlayer() const { return _curPlayer; }
	FieldValue winner() const;
	int nConnect() const { return _nConnect; }
	bool canUndo() const { return _canUndo; }
	bool lastIndex(int &hVal, int &d2Val, int &d3Val) const;

	// Queries
	bool isValid() const { return _nConnect <= 0; }
	bool full(int d2Val, int d3Val, int &hVal) const;
	bool connected(int hVals[], int d2Vals[], int d3Vals[]) const;

	// Data access
	FieldValue get(int hVal, int d2Val, int d3Val) const;

	// Data manipulation
	bool set(int d2Val, int d3Val, int &hVal);
	bool undo(int &hVal, int &d2Val, int &d3Val);

	friend std::ostream &operator<<(std::ostream &os, const Board &obj);

private:
	typedef boost::array<int, 3> IndexArray;

	bool _connected();
	bool _isIndexValid(IndexArray idx) const;

	int _nConnect;
	bool _canUndo, _finished;

	FieldValue _curPlayer;
	boost::multi_array<FieldValue, 3> _array;
	std::stack<IndexArray> _undoStack;
	IndexArray _lastIndex;
	std::list<IndexArray> _conIdcs;
};

#endif // BOARD_H
