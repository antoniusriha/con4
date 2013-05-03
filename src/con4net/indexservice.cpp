/*
 * indexservice.cpp
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

#include "indexservice.h"

IndexServiceConf::IndexServiceConf()
{
	setName("Local");
	setIpAddress(QHostAddress::LocalHost);
	setPort(9090);
}

void IndexServiceConf::setName(QString value)
{
	if (value.isEmpty()) throw Exception("Name must not be empty.");
	_name = value;
}

void IndexServiceConf::setIpAddress(QHostAddress value)
{
	if (value.isNull())
		throw Exception("IpAddress must be a non-null host address.");
	_ipAddress = value;
}

void IndexServiceConf::setPort(quint16 value)
{
	if (value < 1025)
		throw Exception("Port must be a port number above 1024.");
	_port = value;
}

class IndexServiceUnit : public ProcessingUnit
{
protected:
	IndexServiceUnit(ClientEndpoint *endpoint, Request *req,
					 QObject *parent = 0)
		: ProcessingUnit(parent), _endpoint(endpoint), _req(req)
	{
		_conReq = new Request(this);
		_disconReq = new Request(this);
		connect(_conReq, SIGNAL(finished(Request*)),
				this, SLOT(_conFinished()));
		connect(_disconReq, SIGNAL(finished(Request*)),
				this, SLOT(_disconFinished()));
	}

	ClientEndpoint *endpoint() const { return _endpoint; }
	Request *req() const { return _req; }
	SendAndReceiveRequest *internalReq() const { return _internalReq; }

	virtual Message getMsg() = 0;
	virtual void onReqSuccess() = 0;

private slots:
	void _conFinished()
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

	void _finished()
	{
		if (_internalReq->success()) onReqSuccess();
		else _req->endRequest(false, _internalReq->errorString());
		_endpoint->disconnectFromHost(*_disconReq);
	}

	void _disconFinished() { emit finished(this); }

private:
	void process() { _endpoint->connectToServer(*_conReq); }

	ClientEndpoint *_endpoint;
	Request *_conReq, *_req, *_disconReq;
	SendAndReceiveRequest *_internalReq;
};

class RegisterUnit : public IndexServiceUnit
{
public:
	RegisterUnit(ClientEndpoint *endpoint, NetworkGameRequest *req,
				 QObject *parent = 0)
		: IndexServiceUnit(endpoint, req, parent) {}

private:
	Message getMsg()
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

	void onReqSuccess()
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
};

class UnregisterUnit : public IndexServiceUnit
{
public:
	UnregisterUnit(ClientEndpoint *endpoint, NetworkGameRequest *req,
				   QObject *parent = 0)
		: IndexServiceUnit(endpoint, req, parent) {}

private:
	Message getMsg()
	{
		NetworkGameRequest *request = static_cast<NetworkGameRequest *>(req());
		return Messages::unregisterGame(request->game()->guid());
	}

	void onReqSuccess()
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
};

class SealUnit : public IndexServiceUnit
{
public:
	SealUnit(ClientEndpoint *endpoint, NetworkGameRequest *req,
			 QObject *parent = 0)
		: IndexServiceUnit(endpoint, req, parent) {}

private:
	Message getMsg()
	{
		NetworkGameRequest *request = static_cast<NetworkGameRequest *>(req());
		return Messages::sealGame(request->game()->guid());
	}

	void onReqSuccess()
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
};

class RequestGameListUnit : public IndexServiceUnit
{
public:
	RequestGameListUnit(IndexService *service, Request *req,
						QObject *parent = 0)
		: IndexServiceUnit(&service->_endpoint, req, parent), _service(service)
	{}

private:
	Message getMsg() { return Messages::requestGameList(); }

	void onReqSuccess()
	{
		QList<Messages::GameData> gameData;
		if (Messages::parseAnswerGameList(
				internalReq()->msgReceived(), gameData)) {

			QList<NetworkGame *> *_games = &_service->_games;
			for (int i = _games->size() - 1; i >=0; i--) {
				NetworkGame *game = _games->at(i);
				if (!game->hasStarted()) {
					_games->removeAt(i);
					delete game;
				}
			}

			NetworkGameConf conf;
			for (int i = 0; i < gameData.size(); i++) {
				Messages::GameData data = gameData.at(i);
				if (data.state == Messages::Sealed) continue;

				// if game exists already, skip
				bool cont = false;
				for (int i = 0; i < _games->size(); i++) {
					if ((cont = (_games->at(i)->name().string() ==
								 data.gameName.string())))
						break;
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
					_games->append(new NetworkGame(conf));
				} catch (Game::Dimensions::Exception) {
				} catch (NetworkGameConf::Exception) {}
			}
			req()->endRequest(true);
		} else req()->endRequest(false, Messages::InvalidRespErr);
	}

	IndexService *_service;
};

IndexService::IndexService(IndexServiceConf conf)
	: _endpoint(conf.ipAddress(), conf.port(), 10000), _conf(conf), _games() {}

IndexService::~IndexService()
{
	for (int i = 0; i < _games.size(); i++)
		delete _games.takeFirst();
}

void IndexService::registerGame(NetworkGameRequest &request)
{
	if (request.game()->hasStarted())
		throw InvalidOperationException("Cannot register games that "
										"have already started.");

	RegisterUnit *unit = new RegisterUnit(&_endpoint, &request);
	_queue.add(unit);
}

void IndexService::unregisterGame (NetworkGameRequest &request)
{
	UnregisterUnit *unit = new UnregisterUnit(&_endpoint, &request);
	_queue.add(unit);
}

void IndexService::refreshGameList(Request &request)
{
	RequestGameListUnit *unit = new RequestGameListUnit(this, &request);
	_queue.add(unit);
}

void IndexService::sealGame(NetworkGameRequest &request)
{
	SealUnit *unit = new SealUnit(&_endpoint, &request);
	_queue.add(unit);
}
