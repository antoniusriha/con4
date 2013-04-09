/*
 * board.cpp
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

#include <algorithm>
#include <stdexcept>
#include "board.h"

using namespace std;
using namespace boost;

bool Board::isBoardConfValid (int nConnect, int height, int dim2, int dim3, string &errMsg) {
    errMsg = "";
    if (nConnect < 4 || nConnect > MAX_DIMS / 2)
        errMsg = "nConnect must be greater than 4 and lower than MAX_DIMS / 2.";
    if (height < nConnect || height > MAX_DIMS)
        errMsg = "height must be greater than nConnect and lower than MAX_DIMS.";
    if (dim2 < nConnect || dim2 > MAX_DIMS)
        errMsg = "dim2 must be greater than nConnect and lower than MAX_DIMS.";
    if (dim3 != 1 && (dim3 < nConnect || dim3 > MAX_DIMS))
        errMsg = "dim3 must be 1 (for 2D) or greater than nConnect and lower than MAX_DIMS (for 3D).";
    if ((height * dim2 * dim3) % 2 != 0)
        errMsg = "The total number of fields must be even.";
    return errMsg == "";
}

Board::Board (int nConnect, int height, int dim2, int dim3, bool canUndo)
    : _nConnect (nConnect), _canUndo (canUndo), _isFinished (false),
      _curPlayer (Player1), _array (extents [height][dim2][dim3]) {
    string errMsg;
    if (!isBoardConfValid (nConnect, height, dim2, dim3, errMsg))
        throw invalid_argument (errMsg);

    if (_canUndo) _undoStack = stack<IndexArray> ();
}

Board::Board (const Board &board, bool canUndo)
    : _nConnect (board._nConnect), _canUndo (canUndo), _isFinished (board._isFinished),
    _curPlayer (board._curPlayer), _array (board._array) {
    if (canUndo) _undoStack = stack<IndexArray> ();
}

FieldValue Board::winner () const {
    return _isFinished ? (_curPlayer == Player1 ? Player2 : Player1) : None;
}

bool Board::isFull (int dim2, int dim3, int &height) {
    if (nDims () == 2) dim3 = 0;
    height = 0; bool isNotFull = true;
    for (; (isNotFull = (height < this->height ()))
         && (_array [height][dim2][dim3] != None); height++);
    return !isNotFull;
}

FieldValue Board::get (int height, int dim2, int dim3) const {
    return _array [height][dim2][dim3];
}

//bool Board::connected (int *idcs []if (nDims () == 2) dim3 = 0;) {
//    if (!_isFinished) return false;
//    int
//    idcs = new int*[_nConnect];
//}

bool Board::set (int dim2, int dim3) {
    if (_isFinished) throw logic_error ("Game is over.");
    if (nDims () == 2) dim3 = 0;
    int height;
    if (isFull (dim2, dim3, height)) return false;

    IndexArray idx = {{ height, dim2, dim3 }};
    if (!_isIndexValid (idx))
        throw invalid_argument ("Invalid index");

    if (_canUndo) _undoStack.push (idx);
    _array (idx) = _curPlayer;
    _isFinished = _connected (idx);
    _curPlayer = _curPlayer == Player1 ? Player2 : Player1;
    return true;
}

bool Board::undo (int &height, int &dim2, int &dim3) {
    if (!_canUndo) throw logic_error ("canUndo is false.");
    if (_undoStack.empty()) return false;

    _isFinished = false;
    IndexArray idx = _undoStack.top();
    dim2 = idx [1]; dim3 = idx [2];
    height = this->height () - 1;
    for (; (height >= 0) && (_array [height][dim2][dim3] == None); height--);
    _array [height][dim2][dim3] = None;

    _curPlayer = _curPlayer == Player1 ? Player2 : Player1;
    _undoStack.pop();
    return true;
}

ostream &operator<< (ostream &os, const Board &obj) {
    for (int i = obj.height () - 1; i >= 0; i--) {
        for (int j = obj.dim3 () - 1; j >= 0; j--) {
            if (j >= 0) os << endl << string (4 * (obj.dim3 () - j - 1), ' ');
            for (int k = 0; k < obj.dim2 (); k++)
                os << obj._array [i][k][j] << "  ";
        }
        os << endl;
    }
    return os;
}

bool Board::_connected (IndexArray index) const {
    bool fail = true;
    IndexArray idx;
    for (int i = -1; fail && i <= 1; i++) {
        for (int j = -1; fail && j <= 1; j++) {
            for (int k = -1; fail && k <= 1; k++) {
                if (i == 0 && j == 0 && k == 0) continue;
                std::copy (index.begin (), index.end (), idx.begin ());
                for (int m = 1; (fail = m < _nConnect); m++) {
                    idx [0] += i; idx [1] += j; idx [2] += k;
                    if (!_isIndexValid (idx)) break;
                    FieldValue val = _array (idx);
                    if (val != _curPlayer) break;
                }
            }
        }
    }
    return !fail;
}

bool Board::_isIndexValid (IndexArray idx) const {
    return idx [0] >= 0 && idx [1] >= 0 && idx [2] >= 0 &&
           idx [0] < height () && idx [1] < dim2 () && idx [2] < dim3 ();
}
