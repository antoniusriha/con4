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

#include <stack>
#include <string>
#include <boost/multi_array.hpp>

const static int MAX_DIMS = 100;

enum FieldValue {
	None,
	Player1,
	Player2
};

class Board {
public:
    static bool isBoardConfValid (int nConnect, int height, int dim2, int dim3, std::string &errMsg);

    Board (int nConnect, int height, int dim2, int dim3, bool canUndo = false);
    Board (const Board &board, bool canUndo); // custom copy
	
    // Data access & simple queries
    int nDims () const { return dim3 () == 1 ? 2 : 3; }
    int height () const { return *_array.shape (); }
    int dim2 () const { return *(_array.shape () + 1); }
    int dim3 () const { return *(_array.shape () + 2); }
    bool isFinished () const { return _isFinished; }
    FieldValue curPlayer () const { return _curPlayer; }
    FieldValue winner () const;
    int nConnect () const { return _nConnect; }
    bool canUndo () const { return _canUndo; }

    // Queries
    bool isFull (int dim2, int dim3, int &height);
//    bool connected (int *idcs[]);

    // Data access
    FieldValue get (int height, int dim2, int dim3) const;

    // Data manipulation
    bool set (int dim2, int dim3);
    bool undo (int &height, int &dim2, int &dim3);

    friend std::ostream &operator<< (std::ostream &os, const Board &obj);
	
private:
    typedef boost::array<int, 3> IndexArray;

    bool _connected (IndexArray index) const;
    bool _isIndexValid (IndexArray idx) const;

    const int _nConnect;
    const bool _canUndo;
    bool _isFinished;
    FieldValue _curPlayer;
    boost::multi_array<FieldValue, 3> _array;
    std::stack<IndexArray> _undoStack;
};

#endif
