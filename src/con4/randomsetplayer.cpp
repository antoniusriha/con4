/*
 * randomsetplayer.cpp
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

#include "randomsetplayer.h"

RandomSetPlayer::RandomSetPlayer(Game &game, FieldValue player, QObject *parent)
	: AiPlayer(game, player, parent), _game(game), _player(player), _timer()
{
	connect(&game, SIGNAL(started(FieldValue)),
			this, SLOT(_started(FieldValue)));
	connect(&game, SIGNAL(set(FieldValue,Game::BoardIndex)),
			this, SLOT(_set(FieldValue,Game::BoardIndex)));

	_timer.setSingleShot(true);
	_timer.setInterval(1000);
	connect(&_timer, SIGNAL(timeout()), this, SLOT(_set()));
}

void RandomSetPlayer::_started(FieldValue startPlayer)
{
	if (startPlayer == _player) _set();
}

void RandomSetPlayer::_set(FieldValue player, Game::BoardIndex)
{
	if (player != _player) _timer.start();
}

void RandomSetPlayer::_set()
{
	if (_game.finished() || _game.aborted()) return;
	Game::Dimensions dims = _game.dims();
	Game::BoardIndex idx;
	do idx = _game.index(rand() % dims.width(), rand() % dims.depth());
	while (!_game.set(idx));
}

RandomSetPlayerInfo::RandomSetPlayerInfo(QObject *parent)
	: AiPlayerInfo(parent) {}

RandomSetPlayer *RandomSetPlayerInfo::create(Game &game, FieldValue player,
											 QObject *parent) const
{
	RandomSetPlayer *aiPlayer = new RandomSetPlayer(game, player, parent);
	aiPlayer->setDelay(delay());
	return aiPlayer;
}
