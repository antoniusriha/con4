/*
 * netinitiatorhelper.cpp
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

#include "netinitiatorhelper.h"

IndexServiceWrapper::IndexServiceWrapper(IndexService &service,
										 NetworkGame &game, QObject *parent)
	: QObject(parent), _service(service), _game(game),
	_state(NotRegistered), _regReq(game), _sealReq(game), _unregReq(game)
{
  connect(&_regReq, SIGNAL(finished(Request*)),
		  this, SLOT(_registerGameFinished(Request*)));
  connect(&_sealReq, SIGNAL(finished(Request*)),
		  this, SLOT(_sealGameFinished(Request*)));
  connect(&_unregReq, SIGNAL(finished(Request*)),
		  this, SLOT(_unregisterGameFinished(Request*)));
}

void IndexServiceWrapper::_registerGameFinished(Request *request)
{
	if (request->success()) _state = Registered;
	emit registerGameFinished(this);
}

void IndexServiceWrapper::_sealGameFinished(Request *request)
{
	if (request->success()) _state = Sealed;
	emit sealGameFinished(this);
}

void IndexServiceWrapper::_unregisterGameFinished(Request *request)
{
	if (request->success()) _state = Unregistered;
	emit unregisterGameFinished(this);
}


IndexServiceManager::IndexServiceManager(IndexServiceList &list,
										 NetworkGame &game, QObject *parent)
	: QObject(parent), _list(list), _game(game), _wrappers()
{
	connect(&_list, SIGNAL(deleting(IndexService*)),
			this, SLOT(_indexServiceDeleting(IndexService*)));
	for (int i = 0; i < list.size(); i++)
		_wrappers.append(new IndexServiceWrapper(*list.at(i), game));
}

void IndexServiceManager::registerGame()
{
	for (int i = 0; i < _wrappers.size(); i++) {
		IndexServiceWrapper *wrapper = _wrappers.at(i);
		connect(wrapper, SIGNAL(registerGameFinished(IndexServiceWrapper*)),
				this, SLOT(_registerGameFinished(IndexServiceWrapper*)));
		wrapper->registerGame();
	}
}

void IndexServiceManager::sealGame()
{
	for (int i = 0; i < _wrappers.size(); i++) {
		IndexServiceWrapper *wrapper = _wrappers.at(i);
		connect(wrapper, SIGNAL(sealGameFinished(IndexServiceWrapper*)),
				this, SLOT(_sealGameFinished(IndexServiceWrapper*)));
		wrapper->sealGame();
	}
}

void IndexServiceManager::unregisterGame()
{
	for (int i = 0; i < _wrappers.size(); i++) {
		IndexServiceWrapper *wrapper = _wrappers.at(i);
		connect(wrapper,
				SIGNAL(unregisterGameFinished(IndexServiceWrapper*)),
				this, SLOT(_unregisterGameFinished(IndexServiceWrapper*)));
		wrapper->unregisterGame();
	}
}

void IndexServiceManager::_indexServiceDeleting(IndexService *service)
{
	for (int i = 0; i < _wrappers.size(); i++) {
		if (_wrappers.at(i)->service() == service) {
			IndexServiceWrapper *wrapper = _wrappers.takeAt(i);
			wrapper->unregisterGame();
			delete wrapper;
			return;
		}
	}

	if (_wrappers.size() == 0)
		_game.abort(Player1, "All index services have been removed.");
}

void IndexServiceManager::_registerGameFinished(IndexServiceWrapper *wrapper)
{
}

void IndexServiceManager::_sealGameFinished(IndexServiceWrapper *wrapper)
{
}

void IndexServiceManager::_unregisterGameFinished(IndexServiceWrapper *wrapper)
{
}
