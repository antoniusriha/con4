/*
 * endpointhelper.cpp
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

#include "endpointhelper.h"

ReceiveUnit::ReceiveUnit(Endpoint *endpoint, Message msg, QObject *parent)
	: ProcessingUnit(parent), _endpoint(endpoint), _msg(msg) {}

void ReceiveUnit::process()
{
	emit _endpoint->messageReceived(_msg);
	emit finished(this);
}

SendUnit::SendUnit(Endpoint *endpoint, SendRequest *req, QObject *parent)
	: ProcessingUnit(parent), _endpoint(endpoint), _req(req), _nBytes(0) {}

void SendUnit::process()
{
	connect(_endpoint->_socket, SIGNAL(bytesWritten(qint64)),
			this, SLOT(_bytesWritten(qint64)));
	connect(_endpoint->_socket, SIGNAL(error(QAbstractSocket::SocketError)),
			this, SLOT(_error()));

	QByteArray bytes = _req->message().raw();
	int bWritten = 0;
	_nBytes = bytes.size();
	for (int i = 0; i < _nBytes; i += bWritten)
		bWritten = _endpoint->_socket->write(bytes.mid(i));
}

void SendUnit::_bytesWritten(qint64 bytes)
{
	_nBytes -= bytes;
	if (_nBytes == 0) {
		disconnect(_endpoint->_socket, SIGNAL(bytesWritten(qint64)),
				   this, SLOT(_bytesWritten(qint64)));
		disconnect(_endpoint->_socket,
				   SIGNAL(error(QAbstractSocket::SocketError)),
				   this, SLOT(_error()));
		_req->endRequest(true);
		emit finished(this);
	}
}

void SendUnit::_error()
{
	_nBytes = 0;
	disconnect(_endpoint->_socket, SIGNAL(bytesWritten(qint64)),
			   this, SLOT(_bytesWritten(qint64)));
	disconnect(_endpoint->_socket,
			   SIGNAL(error(QAbstractSocket::SocketError)),
			   this, SLOT(_error()));
	_req->endRequest(false, _endpoint->_socket->errorString());
	emit finished(this);
}

SendAndReceiveUnit::SendAndReceiveUnit(Endpoint *endpoint,
									   SendAndReceiveRequest *req,
									   QObject *parent)
	: ProcessingUnit(parent), _endpoint(endpoint), _req(req), _timer(),
	  _nBytes(0), _waitingForEndReceive(false)
{
	_timer.setSingleShot(true);
	connect(&_timer, SIGNAL(timeout()), this, SLOT(_timeout()));
}

void SendAndReceiveUnit::process()
{
	connect(_endpoint->_socket, SIGNAL(bytesWritten(qint64)),
			this, SLOT(_bytesWritten(qint64)));
	connect(_endpoint->_socket, SIGNAL(error(QAbstractSocket::SocketError)),
			this, SLOT(_error()));

	QByteArray bytes = _req->msgSent().raw();
	int bWritten = 0;
	_nBytes = bytes.size();
	_timer.start(_req->timeout());
	for (int i = 0; i < _nBytes; i += bWritten)
		bWritten = _endpoint->_socket->write(bytes.mid(i));
}

void SendAndReceiveUnit::_bytesWritten(qint64 bytes)
{
	_nBytes -= bytes;
	if (_nBytes == 0) {
		disconnect(_endpoint->_socket, SIGNAL(bytesWritten(qint64)),
				   this, SLOT(_bytesWritten(qint64)));
		connect(_endpoint->_socket, SIGNAL(readyRead()),
				this, SLOT(_readyRead()));
		disconnect(_endpoint->_socket, SIGNAL(readyRead()),
				   _endpoint, SLOT(_readyRead()));
		_waitingForEndReceive = !_endpoint->_residuum.isEmpty();
	}
}

void SendAndReceiveUnit::_readyRead()
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

void SendAndReceiveUnit::_error()
{
	_nBytes = 0;
	disconnect(_endpoint->_socket, SIGNAL(bytesWritten(qint64)),
			   this, SLOT(_bytesWritten(qint64)));
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

void SendAndReceiveUnit::_timeout()
{
	_nBytes = 0;
	disconnect(_endpoint->_socket, SIGNAL(bytesWritten(qint64)),
			   this, SLOT(_bytesWritten(qint64)));
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

DisconnectUnit::DisconnectUnit(Endpoint *endpoint, Request *req,
							   QObject *parent)
	: ProcessingUnit(parent), _endpoint(endpoint), _req(req) {}

void DisconnectUnit::process()
{
	connect(_endpoint->_socket, SIGNAL(disconnected()),
			this, SLOT(_disconnected()));
	connect(_endpoint->_socket, SIGNAL(error(QAbstractSocket::SocketError)),
			this, SLOT(_error()));
	_endpoint->_socket->disconnectFromHost();
}

void DisconnectUnit::_disconnected()
{
	disconnect(_endpoint->_socket, SIGNAL(disconnected()),
			   this, SLOT(_disconnected()));
	disconnect(_endpoint->_socket,
			   SIGNAL(error(QAbstractSocket::SocketError)),
			   this, SLOT(_error()));
	_req->endRequest(true);
	emit finished(this);
}

void DisconnectUnit::_error()
{
	disconnect(_endpoint->_socket, SIGNAL(disconnected()),
			   this, SLOT(_disconnected()));
	disconnect(_endpoint->_socket,
			   SIGNAL(error(QAbstractSocket::SocketError)),
			   this, SLOT(_error()));
	_req->endRequest(false, _endpoint->_socket->errorString());
	emit finished(this);
}
