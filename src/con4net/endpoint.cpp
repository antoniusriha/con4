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

#include <QTimer>
#include "endpoint.h"

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

class ReceiveUnit : public ProcessingUnit
{
public:
	ReceiveUnit(Endpoint *endpoint, Message msg, QObject *parent = 0)
		: ProcessingUnit(parent), _endpoint(endpoint), _msg(msg) {}

	void process()
	{
		emit _endpoint->messageReceived(_msg);
		emit finished(this);
	}

private:
	Endpoint *_endpoint;
	Message _msg;
};

class SendUnit : public ProcessingUnit
{
public:
	SendUnit(Endpoint *endpoint, SendRequest *req, QObject *parent = 0)
		: ProcessingUnit(parent), _endpoint(endpoint), _req(req), _nBytes(0) {}

	void process()
	{
		connect(_endpoint->_socket, SIGNAL(bytesWritten(qint64)),
				this, SLOT(_bytesWritten(quint64)));
		connect(_endpoint->_socket, SIGNAL(error(QAbstractSocket::SocketError)),
				this, SLOT(_error()));

		QByteArray bytes = _req->message().raw();
		int bWritten = 0;
		_nBytes = bytes.size();
		for (int i = 0; i < _nBytes; i += bWritten)
			bWritten = _endpoint->_socket->write(bytes.mid(i));
	}

private slots:
	void _bytesWritten(quint64 bytes)
	{
		_nBytes -= bytes;
		if (_nBytes == 0) {
			disconnect(_endpoint->_socket, SIGNAL(bytesWritten(qint64)),
					   this, SLOT(_bytesWritten(quint64)));
			disconnect(_endpoint->_socket,
					   SIGNAL(error(QAbstractSocket::SocketError)),
					   this, SLOT(_error()));
			_req->endRequest(true);
			emit finished(this);
		}
	}

	void _error()
	{
		_nBytes = 0;
		disconnect(_endpoint->_socket, SIGNAL(bytesWritten(qint64)),
				   this, SLOT(_bytesWritten(quint64)));
		disconnect(_endpoint->_socket,
				   SIGNAL(error(QAbstractSocket::SocketError)),
				   this, SLOT(_error()));
		_req->endRequest(false, _endpoint->_socket->errorString());
		emit finished(this);
	}

private:
	Endpoint *_endpoint;
	SendRequest *_req;
	int _nBytes;
};

class SendAndReceiveUnit : public ProcessingUnit
{
public:
	SendAndReceiveUnit(Endpoint *endpoint, SendAndReceiveRequest *req,
					   QObject *parent = 0)
		: ProcessingUnit(parent), _endpoint(endpoint), _req(req), _timer(),
		  _nBytes(0), _waitingForEndReceive(false)
	{
		_timer.setSingleShot(true);
		connect(&_timer, SIGNAL(timeout()), this, SLOT(_timeout()));
	}

	void process()
	{
		connect(_endpoint->_socket, SIGNAL(bytesWritten(qint64)),
				this, SLOT(_bytesWritten(quint64)));
		connect(_endpoint->_socket, SIGNAL(error(QAbstractSocket::SocketError)),
				this, SLOT(_error()));

		QByteArray bytes = _req->msgSent().raw();
		int bWritten = 0;
		_nBytes = bytes.size();
		_timer.start(_req->timeout());
		for (int i = 0; i < _nBytes; i += bWritten)
			bWritten = _endpoint->_socket->write(bytes.mid(i));
	}

private slots:
	void _bytesWritten(quint64 bytes)
	{
		_nBytes -= bytes;
		if (_nBytes == 0) {
			disconnect(_endpoint->_socket, SIGNAL(bytesWritten(qint64)),
					   this, SLOT(_bytesWritten(quint64)));
			connect(_endpoint->_socket, SIGNAL(readyRead()),
					this, SLOT(_readyRead()));
			disconnect(_endpoint->_socket, SIGNAL(readyRead()),
					   _endpoint, SLOT(_readyRead()));
			_waitingForEndReceive = !_endpoint->_residuum.isEmpty();
		}
	}

	void _readyRead()
	{
		QByteArray data = _endpoint->_socket->readAll();
		if (data.isEmpty()) return;

		QVector<Message> msgs = Message::fromBytes(_endpoint->_residuum + data,
												   _endpoint->_residuum);
		if (msgs.isEmpty()) return;

		int msgIdx = 0;

		if (_waitingForEndReceive) {
			_waitingForEndReceive = false;
			ReceiveUnit *unit = new ReceiveUnit(_endpoint, msgs.first());
			_endpoint->_queue.add(unit);
			msgIdx = 1;
		}

		if (msgs.size() > msgIdx) {
			connect(_endpoint->_socket, SIGNAL(readyRead()),
					_endpoint, SLOT(_readyRead()));
			disconnect(_endpoint->_socket, SIGNAL(readyRead()),
					   this, SLOT(_readyRead()));
			disconnect(_endpoint->_socket,
					   SIGNAL(error(QAbstractSocket::SocketError)),
					   this, SLOT(_error()));
			_timer.stop();

			// all surplus msgs go as new units into the queue
			for (int i = msgIdx + 1; i < msgs.size(); i++) {
				ReceiveUnit *unit = new ReceiveUnit(_endpoint, msgs.at(i));
				_endpoint->_queue.add(unit);
			}

			// return control
			_req->_msgReceived = msgs.at(msgIdx);
			_req->endRequest(true);
			emit finished(this);
		}
	}

	void _error()
	{
		_nBytes = 0;
		disconnect(_endpoint->_socket, SIGNAL(bytesWritten(qint64)),
				   this, SLOT(_bytesWritten(quint64)));
		connect(_endpoint->_socket, SIGNAL(readyRead()),
				_endpoint, SLOT(_readyRead()));
		disconnect(_endpoint->_socket, SIGNAL(readyRead()),
				   this, SLOT(_readyRead()));
		disconnect(_endpoint->_socket,
				   SIGNAL(error(QAbstractSocket::SocketError)),
				   this, SLOT(_error()));
		_req->endRequest(false, _endpoint->_socket->errorString());
		emit finished(this);
	}

	void _timeout()
	{
		_nBytes = 0;
		disconnect(_endpoint->_socket, SIGNAL(bytesWritten(qint64)),
				   this, SLOT(_bytesWritten(quint64)));
		connect(_endpoint->_socket, SIGNAL(readyRead()),
				_endpoint, SLOT(_readyRead()));
		disconnect(_endpoint->_socket, SIGNAL(readyRead()),
				   this, SLOT(_readyRead()));
		disconnect(_endpoint->_socket,
				   SIGNAL(error(QAbstractSocket::SocketError)),
				   this, SLOT(_error()));
		_req->endRequest(false, "Timeout.");
		emit finished(this);
	}

private:
	Endpoint *_endpoint;
	SendAndReceiveRequest *_req;
	QTimer _timer;
	int _nBytes;
	bool _waitingForEndReceive;
};

class DisconnectUnit : public ProcessingUnit
{
public:
	DisconnectUnit(Endpoint *endpoint, Request *req, QObject *parent = 0)
		: ProcessingUnit(parent), _endpoint(endpoint), _req(req) {}

	void process()
	{
		connect(_endpoint->_socket, SIGNAL(disconnected()),
				this, SLOT(_disconnected));
		connect(_endpoint->_socket, SIGNAL(error(QAbstractSocket::SocketError)),
				this, SLOT(_error()));
		_endpoint->_socket->disconnectFromHost();
	}

private slots:
	void _disconnected()
	{
		disconnect(_endpoint->_socket, SIGNAL(disconnected()),
				   this, SLOT(_disconnected));
		disconnect(_endpoint->_socket,
				   SIGNAL(error(QAbstractSocket::SocketError)),
				   this, SLOT(_error()));
		_req->endRequest(true);
		emit finished(this);
	}

	void _error()
	{
		disconnect(_endpoint->_socket, SIGNAL(disconnected()),
				   this, SLOT(_disconnected));
		disconnect(_endpoint->_socket,
				   SIGNAL(error(QAbstractSocket::SocketError)),
				   this, SLOT(_error()));
		_req->endRequest(false, _endpoint->_socket->errorString());
		emit finished(this);
	}

private:
	Endpoint *_endpoint;
	Request *_req;
};

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
	connect(_socket, SIGNAL(_readyRead()), this, SLOT(_readyRead()));
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
