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

Board::Board () : _nConnect (4), _canUndo (false), _isFinished (false),
	_curPlayer (Player1), _array (extents [8][8][4]) {
	_lastIndex.assign (-1);
}

Board::Board (int nConnect, int height, int dim2, int dim3, FieldValue curPlayer, bool canUndo)
    : _nConnect (nConnect), _canUndo (canUndo), _isFinished (false),
	  _curPlayer (curPlayer), _array (extents [height][dim2][dim3]) {
    string errMsg;
    if (!isBoardConfValid (nConnect, height, dim2, dim3, errMsg))
        throw invalid_argument (errMsg);
	_lastIndex.assign (-1);
}

Board::Board (const Board &board, bool canUndo)
    : _nConnect (board._nConnect), _canUndo (canUndo), _isFinished (board._isFinished),
	  _curPlayer (board._curPlayer), _array (board._array), _lastIndex (board._lastIndex) {}

FieldValue Board::winner () const {
	return _isFinished ? (_curPlayer == Player1 ? Player2 : Player1) : None;
}

bool Board::lastIndex (int &height, int &dim2, int &dim3) const {
	if (_lastIndex.at (0) == -1) return false;
	height = _lastIndex.at (0);
	dim2 = _lastIndex.at (1);
	dim3 = _lastIndex.at (2);
	return true;
}

bool Board::isFull (int dim2, int dim3, int &height) const {
    if (nDims () == 2) dim3 = 0;
    height = 0; bool isNotFull = true;
    for (; (isNotFull = (height < this->height ()))
         && (_array [height][dim2][dim3] != None); height++);
	return !isNotFull;
}

bool Board::connected (int heightIdcs [], int dim2Idcs [], int dim3Idcs []) const {
	if (!_isFinished) return false;
	int i = 0;
	for (list<IndexArray>::const_iterator it = _conIdcs.begin (); it != _conIdcs.end (); it++) {
		heightIdcs [i] = it->at (0);
		dim2Idcs [i] = it->at (1);
		dim3Idcs [i] = it->at (2);
		i++;
	}
	return true;
}

FieldValue Board::get (int height, int dim2, int dim3) const {
    return _array [height][dim2][dim3];
}

bool Board::set (int dim2, int dim3) {
    if (_isFinished) throw logic_error ("Game is over.");
    if (nDims () == 2) dim3 = 0;
    int height;
    if (isFull (dim2, dim3, height)) return false;

	_lastIndex [0] = height;
	_lastIndex [1] = dim2;
	_lastIndex [2] = dim3;
	if (!_isIndexValid (_lastIndex))
        throw invalid_argument ("Invalid index");

	if (_canUndo) _undoStack.push (_lastIndex);
	_array (_lastIndex) = _curPlayer;
	_isFinished = _connected ();
    _curPlayer = _curPlayer == Player1 ? Player2 : Player1;
    return true;
}

bool Board::undo (int &height, int &dim2, int &dim3) {
    if (!_canUndo) throw logic_error ("canUndo is false.");
	if (_undoStack.empty ()) return false;

    _isFinished = false;
	IndexArray idx = _undoStack.top ();
    dim2 = idx [1]; dim3 = idx [2];
    height = this->height () - 1;
    for (; (height >= 0) && (_array [height][dim2][dim3] == None); height--);
    _array [height][dim2][dim3] = None;

    _curPlayer = _curPlayer == Player1 ? Player2 : Player1;
	_undoStack.pop ();

	if (_undoStack.empty ()) _lastIndex.assign (-1);
	else _lastIndex = _undoStack.top ();
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

bool Board::_connected () {
	bool fail = true;
	IndexArray idx;
	IndexArray conIdcs1 [_nConnect], conIdcs2 [_nConnect];

	bool end = false;
	for (int i = -1; fail && !end; i++) {
		for (int j = -1; fail && !end && j <= 1; j++) {
			for (int k = -1; fail && k <= 1; k++) {
				if (i == 0 && j == 0 && k == 0) {
					end = true; break;
				}

				int m = 1;
				IndexArray *conIdcs1Ptr = conIdcs1;
				copy (_lastIndex.begin (), _lastIndex.end (), idx.begin ());
				for (; fail = (m < _nConnect); m++) {
					idx [0] += i; idx [1] += j; idx [2] += k;
					if (!_isIndexValid (idx)) break;
					FieldValue val = _array (idx);
					if (val != _curPlayer) break;
					*conIdcs1Ptr++ = idx;
				}

				IndexArray *conIdcs2Ptr = conIdcs2;
				copy (_lastIndex.begin (), _lastIndex.end (), idx.begin ());
				for (; fail = (m < _nConnect); m++) {
					idx [0] -= i; idx [1] -= j; idx [2] -= k;
					if (!_isIndexValid (idx)) break;
					FieldValue val = _array (idx);
					if (val != _curPlayer) break;
					*conIdcs2Ptr++ = idx;
				}

				if (!fail) {
					for (conIdcs1Ptr--; conIdcs1Ptr >= conIdcs1; conIdcs1Ptr--)
						_conIdcs.push_back (*conIdcs1Ptr);
					_conIdcs.push_back (_lastIndex);
					IndexArray *conIdcs2Ptr2 = conIdcs2;
					for (; conIdcs2Ptr2 < conIdcs2Ptr; conIdcs2Ptr2++)
						_conIdcs.push_back (*conIdcs2Ptr2);
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
