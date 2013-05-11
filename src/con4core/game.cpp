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
#include "game.h"

Game::Dimensions::Dimensions() { _init(4, 8, 8, 4); }

Game::Dimensions::Dimensions(int nConnect, int width, int height, int depth)
{
	_init(nConnect, width, height, depth);
}

void Game::Dimensions::setNConnect(int value)
{
	if (value < 4 || value > MaxDim / 2)
		throw Exception("nConnect must be greater than 4 and "
						"lower than MaxDim / 2.");
	_nConnect = value;
}

void Game::Dimensions::setWidth(int value)
{
	if (value < _nConnect || value > MaxDim)
		throw Exception("width mus	qDeleteAll(_gameHosts);t be greater than nConnect "
						"and lower than MaxDim.");
	_width = value;
}

void Game::Dimensions::setHeight(int value)
{
	if (value < _nConnect || value > MaxDim)
		throw Exception("height must be greater than nConnect and "
						"lower than MaxDim.");
	_height = value;
}

void Game::Dimensions::setDepth(int value)
{
	if (value != 1 && (value < _nConnect || value > MaxDim))
		throw Exception("depth must be 1 (for 2D) or greater or equal "
						"than nConnect and lower than MaxDim (for 3D).");
	_depth = value;
}

void Game::Dimensions::_init(int nConnect, int width, int height, int depth)
{
	setNConnect(nConnect);
	setWidth(width);
	setHeight(height);
	setDepth(depth);
}

const char *Game::BoardIndex::Exception::_what =
		"value is not within the valid range.";

Game::BoardIndex::BoardIndex() : _game(0), _wVal(0), _hVal(0), _dVal(0) {}

Game::BoardIndex::BoardIndex(const Game &game, int wVal, int hVal, int dVal)
	: _game(&game)
{
	setWVal(wVal);
	setHVal(hVal);
	setDVal(dVal);
}

void Game::BoardIndex::setWVal(int value)
{
	if (value < 0 && value >= _game->_board.dim2()) throw Exception();
	_wVal = value;
}

void Game::BoardIndex::setHVal(int value)
{
	if (value < 0 && value >= _game->_board.height()) throw Exception();
	_hVal = value;
}

void Game::BoardIndex::setDVal(int value)
{
	if (value < 0 && value >= _game->_board.dim3()) throw Exception();
	_dVal = value;
}

const char *Game::InvalidIndexException::_what =
		"Invalid index. Create a valid index with Game::index()";

Game::Game(Dimensions dims, QObject *parent) : QObject(parent), _disksSet(0),
    _totalDisks(0), _hasStarted(false), _aborted(false), _finished(false),
    _board(dims.nConnect(), dims.height(), dims.width(), dims.depth())
{
	setDims(dims);
}

Game::Dimensions Game::dims() const
{
	return Dimensions(_board.nConnect(), _board.dim2(),
					  _board.height(), _board.dim3());
}

void Game::setDims(Dimensions dims)
{
	if (_hasStarted) throw InvalidOperationException(
				"Cannot set dimensions when game has already started.");
	_board = Board(dims.nConnect(), dims.height(),
				   dims.width(), dims.depth(), None);
}

Game::BoardIndex Game::index() const
{
	return index(0, 0);
}

Game::BoardIndex Game::index(int wVal, int dVal, int hVal) const
{
	return BoardIndex(*this, wVal, hVal, dVal);
}

bool Game::full(BoardIndex &idx) const
{
	if (idx.game() != this) throw InvalidIndexException();

	int hVal;
	bool full = _board.full(idx.wVal(), idx.dVal(), hVal);
	idx.setHVal(hVal);
	return full;
}

bool Game::connected(QVector<BoardIndex> &vals) const
{
	int nCon = _board.nConnect();
	int wVals[nCon], hVals[nCon], dVals[nCon];
	if (!_board.connected(hVals, wVals, dVals)) return false;

	vals = QVector<BoardIndex>(nCon);
	for (int i = 0; i < nCon; i++)
		vals[i] = index(wVals[i], dVals[i], hVals[i]);
	return true;
}

FieldValue Game::get(BoardIndex idx) const
{
	if (idx.game() != this) throw InvalidIndexException();
	return _board.get(idx.hVal(), idx.wVal(), idx.dVal());
}

bool Game::set(BoardIndex &idx)
{
	if (idx.game() != this) throw InvalidIndexException();
	if (!_hasStarted)
		throw InvalidOperationException(
				"Cannot set, since game has not yet started.");
	if (_finished)
		throw InvalidOperationException(
				"Cannot set, since game is already finished.");
	if (_aborted)
		throw InvalidOperationException(
				"Cannot set, since game has been aborted.");

	int hVal;
	if (_board.set(idx.wVal(), idx.dVal(), hVal)) {
		idx.setHVal(hVal);
		FieldValue player = curPlayer() == Player1 ? Player2 : Player1;
		_disksSet++;
		emit set(player, idx);

		if (_board.finished()) {
			_finished = true;
			emit finished(player);
		} else if (_disksSet == _totalDisks) {
			_finished = true;
			emit finished(None);
		}
		return true;
	} else return false;
}

bool Game::undo(BoardIndex &idx)
{
	if (!_hasStarted)
		throw InvalidOperationException(
				"Cannot undo, since game has not yet started.");
	if (_aborted)
		throw InvalidOperationException(
				"Cannot undo, since game has been aborted.");

	int wVal, hVal, dVal;
	if (_board.undo(hVal, wVal, dVal)) {
		idx = index(wVal, dVal, hVal);
		_finished = false;
		_disksSet--;
		emit undone(curPlayer() == Player1 ? Player2 : Player1, idx);
		return true;
	} else return false;
}

void Game::start(FieldValue startPlayer)
{
	if (_hasStarted)
		throw InvalidOperationException(
				"Cannot start, since game has already started.");
	if (_finished)
		throw InvalidOperationException(
				"Cannot start, since game is already finished.");
	if (_aborted)
		throw InvalidOperationException(
				"Cannot start, since game has been aborted.");

	if (startPlayer == None) startPlayer = (FieldValue)(rand() % 2 + 1);
	_board = Board(_board.nConnect(), _board.height(), _board.dim2(),
				   _board.dim3(), startPlayer, true);
	_totalDisks = _board.height() * _board.dim2() * _board.dim3();
	_hasStarted = true;
	emit started(startPlayer);
}

void Game::abort(FieldValue requester, QString reason)
{
	if (!_hasStarted)
		throw InvalidOperationException(
				"Cannot abort, since game has not yet started.");
	if (_finished)
		throw InvalidOperationException(
				"Cannot abort, since game is already finished.");
	if (_aborted)
		throw InvalidOperationException(
				"Cannot abort, since game has already been aborted.");

	_aborted = true;
	emit aborted(requester, reason);
}
