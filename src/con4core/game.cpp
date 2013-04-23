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

#include <stdexcept>
#include <string>
#include "game.h"

using namespace std;

Game::Dimensions::Dimensions() { _init(4, 8, 8, 4); }

Game::Dimensions::Dimensions(int nConnect, int width, int height, int depth)
{
	_init(nConnect, width, height, depth);
}

void Game::Dimensions::setNConnect(int value)
{
	if (value < 4 || value > MaxDim / 2)
		throw invalid_argument("nConnect must be greater than 4 and "
							   "lower than MaxDim / 2.");
	_nConnect = value;
}

void Game::Dimensions::setWidth(int value)
{
	if (value < _nConnect || value > MaxDim)
		throw invalid_argument("width must be greater than nConnect "
							   "and lower than MaxDim.");
	_width = value;
}

void Game::Dimensions::setHeight(int value)
{
	if (value < _nConnect || value > MaxDim)
		throw invalid_argument("height must be greater than nConnect and "
							   "lower than MaxDim.");
	_height = value;
}

void Game::Dimensions::setDepth(int value)
{
	if (value != 1 && (value < _nConnect || value > MaxDim))
		throw invalid_argument("depth must be 1 (for 2D) or greater than "
							   "nConnect and lower than MaxDim (for 3D).");
	_depth = value;
}

void Game::Dimensions::_init(int nConnect, int width, int height, int depth)
{
	setNConnect(nConnect);
	setWidth(width);
	setHeight(height);
	setDepth(depth);
}

Game::Game(Dimensions dims, QObject *parent) : QObject(parent), _disksSet(0),
	_totalDisks(0), _hasStarted(false), _aborted(false), _finished(false)
{
	setDims(dims);
}

Game::~Game() { if (_hasStarted) delete _board; }

void Game::setDims(Dimensions dims)
{
	if (_hasStarted) throw logic_error("Cannot set dimensions when "
									   "game has already started.");
	_dims = dims;
}

FieldValue Game::curPlayer() const
{
	return _hasStarted ? _board->curPlayer() : None;
}

bool Game::full(int wVal, int dVal, int &hVal)
{
	return _hasStarted ? _board->full(wVal, dVal, hVal) : false;
}

bool Game::connected(int wVals[], int hVals[], int dVals[]) const
{
	if (!_hasStarted) return false;
	return _board->connected(hVals, wVals, dVals);
}

FieldValue Game::get(int wVal, int hVal, int dVal) const
{
	return _hasStarted ? _board->get(hVal, wVal, dVal) : None;
}

bool Game::set(int wVal, int dVal, int &hVal)
{
	if (!_hasStarted || _finished || _aborted) return false;
	if (_board->set(wVal, dVal, hVal)) {
		FieldValue player = curPlayer() == Player1 ? Player2 : Player1;
		_disksSet++;
		emit set(player, wVal, hVal, dVal);

		if (_board->finished()) {
			_finished = true;
			emit finished(player);
		} else if (_disksSet == _totalDisks) {
			_finished = true;
			emit finished(None);
		}
		return true;
	} else return false;
}

bool Game::undo(int &wVal, int &hVal, int &dVal)
{
	if (!_hasStarted || _aborted) return false;
	if (_board->undo(hVal, wVal, dVal)) {
		_finished = false;
		_disksSet--;
		emit undone(curPlayer() == Player1 ? Player2 : Player1,
					wVal, hVal, dVal);
		return true;
	} else return false;
}

bool Game::start(FieldValue startPlayer)
{
	if (_hasStarted || _finished || _aborted) return false;
	if (startPlayer == None) startPlayer = (FieldValue)(rand() % 2 + 1);
	_board = new Board(_dims.nConnect(), _dims.height(), _dims.width(),
					   _dims.depth(), startPlayer, true);
	_totalDisks = _dims.height() * _dims.width() * _dims.depth();
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
