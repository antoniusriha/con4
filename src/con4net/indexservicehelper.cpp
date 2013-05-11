/*
 * indexservicehelper.cpp
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

#include "indexservicehelper.h"

IndexServiceUnit::IndexServiceUnit(ClientEndpoint *endpoint, Request *req,
								   QObject *parent)
	: ProcessingUnit(parent), _endpoint(endpoint), _req(req)
{
	_conReq = new Request(this);
	_disconReq = new Request(this);
	connect(_conReq, SIGNAL(finished(Request*)), this, SLOT(_conFinished()));
	connect(_disconReq, SIGNAL(finished(Request*)),
			this, SLOT(_disconFinished()));
}

void IndexServiceUnit::_conFinished()
{
	if (_conReq->success()) {
		_internalReq = new SendAndReceiveRequest(getMsg(), 10000, this);
		connect(_internalReq, SIGNAL(finished(Request*)),
				this, SLOT(_finished()));
		_endpoint->sendAndReceive(*_internalReq);
	} else {
		_req->endRequest(false, _conReq->errorString());
		emit finished(this);
	}
}

void IndexServiceUnit::_finished()
{
	if (_internalReq->success()) onReqSuccess();
	else _req->endRequest(false, _internalReq->errorString());
	_endpoint->disconnectFromHost(*_disconReq);
}

RegisterUnit::RegisterUnit(ClientEndpoint *endpoint, NetworkGameRequest *req,
						   QObject *parent)
	: IndexServiceUnit(endpoint, req, parent) {}

Message RegisterUnit::getMsg()
{
	NetworkGameRequest *request = static_cast<NetworkGameRequest *>(req());
	NetworkGame *game = request->game();
	Messages::Vector3 dims;
	dims.w = game->dims().width();
	dims.h = game->dims().height();
	dims.d = game->dims().depth();

	Messages::GameData gameData;
	gameData.dims = dims;
	gameData.gameName = game->name();
	gameData.initiatorName = game->initiatorName();
	gameData.ipAddress = game->ipAddress();
	gameData.port = game->port();
	gameData.state = Messages::Open;

	return Messages::registerGame(gameData);
}

void RegisterUnit::onReqSuccess()
{
	NetworkGameRequest *request = static_cast<NetworkGameRequest *>(req());
	QUuid guid;
	NetworkString reason;
	if (Messages::parseRegisterSuccess(
			internalReq()->msgReceived(), guid)) {
		request->game()->setGuid(guid);
		request->endRequest(true);
	} else if (Messages::parseRegisterFailed(
				   internalReq()->msgReceived(), reason)) {
		request->endRequest(false, reason.string());
	} else request->endRequest(false, Messages::InvalidRespErr);
}

UnregisterUnit::UnregisterUnit(ClientEndpoint *endpoint,
							   NetworkGameRequest *req, QObject *parent)
	: IndexServiceUnit(endpoint, req, parent) {}

Message UnregisterUnit::getMsg()
{
	NetworkGameRequest *request = static_cast<NetworkGameRequest *>(req());
	return Messages::unregisterGame(request->game()->guid());
}

void UnregisterUnit::onReqSuccess()
{
	NetworkGameRequest *request = static_cast<NetworkGameRequest *>(req());
	NetworkString reason;
	if (Messages::parseUnregisterSuccess(internalReq()->msgReceived()))
		request->endRequest(true);
	else if (Messages::parseUnregisterFailed(
				   internalReq()->msgReceived(), reason)) {
		request->endRequest(false, reason.string());
	} else request->endRequest(false, Messages::InvalidRespErr);
}

SealUnit::SealUnit(ClientEndpoint *endpoint, NetworkGameRequest *req,
				   QObject *parent) : IndexServiceUnit(endpoint, req, parent) {}

Message SealUnit::getMsg()
{
	NetworkGameRequest *request = static_cast<NetworkGameRequest *>(req());
	return Messages::sealGame(request->game()->guid());
}

void SealUnit::onReqSuccess()
{
	NetworkGameRequest *request = static_cast<NetworkGameRequest *>(req());
	NetworkString reason;
	if (Messages::parseSealGameSuccess(internalReq()->msgReceived()))
		request->endRequest(true);
	else if (Messages::parseSealGameFailed(
				   internalReq()->msgReceived(), reason)) {
		request->endRequest(false, reason.string());
	} else request->endRequest(false, Messages::InvalidRespErr);
}

RequestGameListUnit::RequestGameListUnit(IndexService *service, Request *req,
										 QObject *parent)
	: IndexServiceUnit(&service->_endpoint, req, parent), _service(service) {}

void RequestGameListUnit::onReqSuccess()
{
	QList<Messages::GameData> gameData;
	if (Messages::parseAnswerGameList(
			internalReq()->msgReceived(), gameData)) {

		QList<NetOpponent *> &opps = _service->_opps;

		// remove games which are not entailed in the download list
		bool exists;
		for (int i = opps.size() - 1; i >= 0; i--) {
			exists = false;
			for (int j = 0; j < gameData.size(); j++) {
				if (opps.at(i)->game()->name().string() ==
						gameData.at(j).gameName.string()) {
					exists = true;
					break;
				}
			}

			if (!exists) {
				// remove game
				opps.at(i)->stop();
				delete opps.takeAt(i);
			}
		}

		// add game with name A, if no game with
		// name A exists and game is Open
		NetworkGameConf conf;
		for (int i = 0; i < gameData.size(); i++) {
			Messages::GameData data = gameData.at(i);
			if (data.state == Messages::Sealed) continue;

			// if game exists already, skip
			bool cont = false;
			for (int i = 0; i < opps.size(); i++) {
				if ((cont = (opps.at(i)->game()->name().string() ==
							 data.gameName.string()))) break;
			}
			if (cont) continue;

			try {
				Game::Dimensions dims(4, data.dims.w,
									  data.dims.h, data.dims.d);
				conf.setDims(dims);
				conf.setInitiatorName(data.initiatorName);
				conf.setIpAddress(data.ipAddress);
				conf.setName(data.gameName);
				conf.setPort(data.port);
				opps.append(new NetOpponent(conf));
			} catch (Game::Dimensions::Exception) {
			} catch (NetworkGameConf::Exception) {}
		}
		req()->endRequest(true);
	} else req()->endRequest(false, Messages::InvalidRespErr);
}
