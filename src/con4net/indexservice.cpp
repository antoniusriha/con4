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
#include "indexservicehelper.h"

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

NetworkGameRequest::NetworkGameRequest(NetworkGame &game,
									   IndexService &indexService,
									   QObject *parent)
	: Request(parent), _game(game), _indexService(indexService) {}

IndexService::IndexService(IndexServiceConf conf) : _queue(),
	_endpoint(conf.ipAddress(), conf.port(), 10000), _conf(conf) {}

IndexService::~IndexService()
{
	for (int i = 0; i < _opps.size(); i++)
		delete _opps.takeFirst();
}

QList<NetworkGame *> IndexService::games()
{
	QList<NetworkGame *> games;
	for (int i = 0; i < _opps.size(); i++)
		games.append(_opps.at(i)->game());
	return games;
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
