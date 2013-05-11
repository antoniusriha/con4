/*
 * endpoint.cpp
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

#include "endpoint.h"
#include "endpointhelper.h"

Request::Request(QObject *parent)
	: QObject(parent), _success(false), _ended(false), _errorString() {}

void Request::endRequest(bool success, QString errorString)
{
	if (_ended) return;
	_ended = true;
	_success = success;
	_errorString = errorString;
	emit finished(this);
}

SendRequest::SendRequest(Message msg, QObject *parent)
	: Request(parent), _msg(msg) {}

SendAndReceiveRequest::SendAndReceiveRequest(Message msgSent, int timeout,
											 QObject *parent)
	: Request(parent), _msgSent(msgSent), _timeout(timeout) {}

Endpoint::~Endpoint()
{
	if (_socket) delete _socket;
}

void Endpoint::setDefaultTimeout(int value)
{
	if (value < minTimeout) value = minTimeout;
	if (value > maxTimeout) value = maxTimeout;
	_defaultTimeout = value;
}

void Endpoint::send(SendRequest &request)
{
	SendUnit *unit = new SendUnit(this, &request);
	_queue.add(unit);
}

void Endpoint::sendAndReceive(SendAndReceiveRequest &request)
{
	SendAndReceiveUnit *unit = new SendAndReceiveUnit(this, &request);
	_queue.add(unit);
}

void Endpoint::disconnectFromHost(Request &request)
{
	DisconnectUnit *unit = new DisconnectUnit(this, &request);
	_queue.add(unit);
}

Endpoint::Endpoint(int defaultTimeout, QObject *parent)
	: QObject(parent), _queue(), _socket(0),
	  _defaultTimeout(defaultTimeout), _residuum() {}

void Endpoint::setSocket(QTcpSocket &value)
{
	if (_socket) delete _socket;
	_socket = &value;
	connect(_socket, SIGNAL(readyRead()), this, SLOT(_readyRead()));
}

void Endpoint::_readyRead()
{
	QByteArray data = _socket->readAll();
	if (data.isEmpty()) return;

	QVector<Message> msgs = Message::fromBytes(_residuum + data, _residuum);
	for (int i = 0; i < msgs.size(); i++) {
		ReceiveUnit *unit = new ReceiveUnit(this, msgs.at(i));
		_queue.add(unit);
	}
}
