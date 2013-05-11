/*
 * indexservicemanager.cpp
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

#include "indexservicemanager.h"

IndexServiceManager::IndexServiceManager(IndexServiceList &list,
										 NetworkGame &game, QObject *parent)
	: QObject(parent), _list(list), _activeServices(), _game(game), _tData(0)
{
	for (int i = 0; i < list.size(); i++)
		_activeServices.append(list.at(i));
	connect(&_list, SIGNAL(deleting(IndexService*,int)),
			this, SLOT(_indexServiceDeleting(IndexService*)));
}

void IndexServiceManager::registerGame() { _runRequest(Register); }

void IndexServiceManager::sealGame() { _runRequest(Seal); }

void IndexServiceManager::unregisterGame() { _runRequest(Unregister); }

void IndexServiceManager::_indexServiceDeleting(IndexService *service)
{
	NetworkGameRequest *req = new NetworkGameRequest(_game, *service, this);
	connect(req, SIGNAL(finished(Request*)), req, SLOT(deleteLater()));
	service->unregisterGame(*req);
	_activeServices.removeOne(service);
	if (_tData) _tData->nServices--;
	if (_activeServices.size() == 0)
		_game.abort(Player1, "All index services have been removed.");
}

void IndexServiceManager::_reqFinished(Request *request)
{
	NetworkGameRequest *req = static_cast<NetworkGameRequest *>(request);
	if (req->success()) _tData->success = true;
	else {
		ErroneousService svc;
		svc.service = req->indexService();
		svc.errorString = req->errorString();
		_tData->errServices.append(svc);
	}

	delete req;
	if (++_tData->count == _tData->nServices) {
		switch (_tData->reqType) {
		case Register:
			emit registerGameFinished(_tData->success, _tData->errServices);
			break;
		case Seal:
			emit sealGameFinished(_tData->success, _tData->errServices);
			break;
		case Unregister:
			emit unregisterGameFinished(_tData->success, _tData->errServices);
			break;
		}
	}
}

void IndexServiceManager::_runRequest(RequestType reqType)
{
	_tData = new TransactionData(reqType, _activeServices.size(), this);
	IndexService *service;
	NetworkGameRequest *req;
	for (int i = 0; i < _tData->nServices; i++) {
		service = _activeServices.at(i);
		req = new NetworkGameRequest(_game, *service, this);
		connect(req, SIGNAL(finished(Request*)),
				this, SLOT(_reqFinished(Request*)));
		switch (reqType) {
		case Register: service->registerGame(*req);
			break;
		case Seal: service->sealGame(*req);
			break;
		case Unregister: service->unregisterGame(*req);
			break;
		}
	}
}
