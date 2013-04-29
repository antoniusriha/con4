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

using namespace std;

Request::Request(QObject *parent)
	: QObject(parent), _success(false), _ended(false), _errorString() {}

void Request::endRequest(bool success, QString errorString)
{
	if (_ended) return;
	_ended = true;
	_success = success;
	_errorString = errorString;
	emit finished();
}

SendRequest::SendRequest(Message msg, QObject *parent)
	: Request(parent), _msg(msg) {}

SendAndReceiveRequest::SendAndReceiveRequest(Message msgSent, int timeout,
											 QObject *parent)
	: Request(parent), _msgSent(msgSent), _timeout(timeout) {}

DisconnectRequest::DisconnectRequest(QObject *parent) : Request(parent) {}

class ReceiveRequest : public Request
{
public:
	ReceiveRequest(Message msg, QObject *parent = 0)
		: Request(parent), _msg(msg) {}
	Message message() const { return _msg; }
private:
	Message _msg;
	friend class Endpoint;
};

class ProcessingUnit
{
public:
	enum Type { Invalid, Send, SendAndReceive, Receive, Disconnect };

	ProcessingUnit() : _type(Invalid), _req(0) {}

	ProcessingUnit(Type type, Request *req) : _type(type), _req(req)
	{
		if (type == Invalid) _req = 0;
		else if (!req) throw invalid_argument("req cannot be 0, "
											  "if type is not Invalid.");
	}

	~ProcessingUnit() { if (type == Receive) delete req; }

	Type type() const { return _type; }
	Request *req() const { return _req; }
	bool isValid() const { return _type != Invalid; }

private:
	ProcessingUnit(const ProcessingUnit &);
	ProcessingUnit &operator=(const ProcessingUnit &);

	Type _type;
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

bool Endpoint::connected() const
{
	return false;
}

void Endpoint::send(SendRequest &request)
{
	ProcessingUnit unit(ProcessingUnit::Send, &request);
	_msgQueue.enqueue(unit);
	_processMsg();
}

void Endpoint::sendAndReceive(SendAndReceiveRequest &request)
{
	ProcessingUnit unit(ProcessingUnit::SendAndReceive, &request);
	_msgQueue.enqueue(unit);
	_processMsg();
}

void Endpoint::disconnectFromHost(Request &request)
{
	ProcessingUnit unit(ProcessingUnit::Disconnect, &request);
	_msgQueue.enqueue(unit);
	_processMsg();
}

Endpoint::Endpoint(int defaultTimeout, QObject *parent)
	: QObject(parent), _timer()
{
	_init(defaultTimeout);
}

void Endpoint::setSocket(QTcpSocket &value)
{
	if (_socket) delete _socket;
	_socket = &value;
	connect(_socket, SIGNAL(_readyRead()), this, SLOT(_readyRead()));
}

void Endpoint::_bytesWritten(quint64 bytes)
{
	_nBytes -= bytes;
	if (_nBytes == 0) {
		disconnect(_socket, SIGNAL(bytesWritten(qint64)),
				   this, SLOT(_bytesWritten(quint64)));

		if (_processingState == Send) {
			disconnect(_socket, SIGNAL(error(QAbstractSocket::SocketError)),
					   this, SLOT(_error()));
			_curUnit.req->endRequest(true);
			_processingState = Idle;
			_processMsg();
		} else if (_processingState == SendAndReceive) {
			connect(_socket, SIGNAL(readyRead()), this, SLOT(_readyRead()));
			_waitingForEndReceive = !_residuum.isEmpty();
		}
	}
}

void Endpoint::_readyRead()
{
	QByteArray data = _socket->readAll();
	if (data.isEmpty()) return;

	QVector<Message> msgs = Message::fromBytes(_residuum + data, _residuum);
	if (msgs.isEmpty()) return;

	if (_processingState == SendAndReceive) {
		int msgIdx = 0;

		if (_waitingForEndReceive) {
			_waitingForEndReceive = false;
			ProcessingUnit unit(ProcessingUnit::Receive,
								new ReceiveRequest(msgs.first()));
			_msgQueue.enqueue(unit);
			msgIdx = 1;
		}

		if (msgs.size() > msgIdx) {
			disconnect(_socket, SIGNAL(error(QAbstractSocket::SocketError)),
					   this, SLOT(_error()));
			_timer.stop();

			// all surplus msgs go as new units into the queue
			for (int i = msgIdx + 1; i < msgs.size(); i++) {
				ProcessingUnit unit(ProcessingUnit::Receive,
									new ReceiveRequest(msgs.at(i)));
				_msgQueue.enqueue(unit);
			}

			// return control
			emit sendAndReceiveFinished(true, msgs.at(msgIdx), QString());
			_processingState = Idle;
			_processMsg();
		}
	} else {
		for (int i = 0; i < msgs.size(); i++) {
			ProcessingUnit unit(ProcessingUnit::Receive,
								new ReceiveRequest(msgs.at(i)));
			_msgQueue.enqueue(unit);
		}
		_processMsg();
	}
}

void Endpoint::_error()
{
	_nBytes = 0;
	_socket->disconnect(SIGNAL(bytesWritten(qint64)));
	_socket->disconnect(SIGNAL(error(QAbstractSocket::SocketError)));

	if (_curUnit)
	emit sendFinished(false, _curMsg, _socket->errorString());
}

void Endpoint::_sendAndReceiveTimeout()
{
	_nBytes = 0;
	disconnect(_socket, SIGNAL(bytesWritten(qint64)),
			   this, SLOT(_bytesWritten(quint64)));
	disconnect(_socket, SIGNAL(error(QAbstractSocket::SocketError)),
			   this, SLOT(_error()));
	emit sendFinished(false, _curMsg, "Timeout.");
}

void Endpoint::_init(int timeout)
{
	_msgQueue = QQueue<ProcessingUnit>();
	_curMsg = Message();
	_processingState = Idle;
	_socket = 0;
	_defaultTimeout = timeout;
	_nBytes = 0;
	_timer.setSingleShot(true);
	connect(&_timer, SIGNAL(timeout()), this, SLOT(_sendAndReceiveTimeout()));;
	_residuum = QByteArray();
}

void Endpoint::_processMsg()
{
	if (_processingState != Idle || _msgQueue.isEmpty()) return;

	// get next msg to process
	ProcessingUnit unit = _msgQueue.dequeue();
	_curReq = unit.req;
	if (unit.type == ProcessingUnit::Send) _processSend();
	else if (unit.type == ProcessingUnit::SendAndReceive)
		_processSendAndReceive(unit.timeout);
	else if (unit.type == ProcessingUnit::Receive) {
		emit messageReceived(unit.msg);
		_processMsg();
	} else if (unit.type == ProcessingUnit::Disconnect) {
		_msgQueue.clear();
		_socket->disconnectFromHost();
		_socket->waitForDisconnected(_defaultTimeout);
		req->endRequest(true);
		_processMsg();
	}
}

void Endpoint::_processSend()
{
	_processingState = Send;
	connect(_socket, SIGNAL(_bytesWritten(qint64)),
			this, SLOT(_bytesWritten(quint64)));
	connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)),
			this, SLOT(_error()));

	SendRequest *req = static_cast<SendRequest *>(_curReq);
	QByteArray bytes = req->message().raw();
	int bWritten = 0;
	_nBytes = bytes.size();
	for (int i = 0; i < _nBytes; i += bWritten)
		bWritten = _socket->write(bytes.mid(i));
}

void Endpoint::_processSendAndReceive(int timeout)
{
	_processingState = SendAndReceive;
	connect(_socket, SIGNAL(_bytesWritten(qint64)),
			this, SLOT(_bytesWritten(quint64)));
	connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)),
			this, SLOT(_error()));

	QByteArray bytes = _curMsg.raw();
	int bWritten = 0;
	_nBytes = bytes.size();
	_timer.start(timeout);
	for (int i = 0; i < _nBytes; i += bWritten)
		bWritten = _socket->write(bytes.mid(i));
}
