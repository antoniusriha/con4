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

Request::Request(QObject *parent)
	: QObject(parent), _success(false), _ended(false), _errorString() {}

void Request::endRequest(bool success, QString errorString)
{
	_success = success;
	_errorString = errorString;
	emit finished();
}

SendRequest::SendRequest(Message msg, QObject *parent)
	: Request(parent), _msg(msg) {}

void Endpoint::DisconnectedState::setSocket(Endpoint *endpoint,
											QTcpSocket &value)
{
	if (endpoint->_socket) {
		if (endpoint->_socket->state() != QAbstractSocket::UnconnectedState)

			endpoint->disconnectFromHost();
	}
//		if (_socket) {


//		}

	// old socket:
	// disconnect signals: readyRead, error, disconnected
	// disconnect socket
	// delete socket

	// new socket:
	// connect to signals: readyRead, error, disconnected

}

void Endpoint::DisconnectedState::send(Endpoint *, Message)
{
	throw Endpoint::InvalidOperationException("Cannot send messages, "
											  "when not connected.");
}

void Endpoint::DisconnectedState::sendAndReceive(Endpoint *, Message, int)
{
	throw Endpoint::InvalidOperationException("Cannot send messages, "
											  "when not connected.");
}

void Endpoint::DisconnectedState::disconnectFromHost(Endpoint *endpoint)
{
	emit endpoint->disconnectFromHostFinished(true, QString());
}










/*


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

void Endpoint::send(Message msg)
{
	ProcessingUnit unit;
	unit.msg = msg;
	unit.timeout = defaultTimeout();
	unit.type = ProcessingUnit::Send;
	_msgQueue.enqueue(unit);
	_processMsg();
}

void Endpoint::sendAndReceive(Message msg)
{
	sendAndReceive(msg, _defaultTimeout);
}

void Endpoint::sendAndReceive(Message msg, int timeout)
{
	ProcessingUnit unit;
	unit.msg = msg;
	unit.timeout = timeout;
	unit.type = ProcessingUnit::SendAndReceive;
	_msgQueue.enqueue(unit);
	_processMsg();
}

void Endpoint::disconnectFromHost()
{
	_msgQueue.clear();
	_socket->disconnectFromHost();
}

Endpoint::Endpoint(int defaultTimeout, QObject *parent)
	: QObject(parent), _timer()
{
	_init(defaultTimeout);
}

void Endpoint::setSocket(QTcpSocket &value)
{
	if (_socket)
		disconnect(_socket, SIGNAL(_readyRead()), this, SLOT(_readyRead()));
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
			emit sendFinished(true, _curMsg, QString());
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

	ProcessingUnit unit;
	if (_processingState == SendAndReceive) {
		int msgIdx = 0;

		if (_waitingForEndReceive) {
			_waitingForEndReceive = false;
			unit.msg = msgs.first();
			unit.type = ProcessingUnit::Receive;
			_msgQueue.enqueue(unit);
			msgIdx = 1;
		}

		if (msgs.size() > msgIdx) {
			disconnect(_socket, SIGNAL(readyRead()), this, SLOT(_readyRead()));
			disconnect(_socket, SIGNAL(error(QAbstractSocket::SocketError)),
					   this, SLOT(_error()));
			_timer.stop();

			// all surplus msgs go as new units into the queue
			for (int i = msgIdx + 1; i < msgs.size(); i++) {
				unit.msg = msgs.at(i);
				unit.type = ProcessingUnit::Receive;
				_msgQueue.enqueue(unit);
			}

			// return control
			emit sendAndReceiveFinished(true, msgs.at(msgIdx), QString());
			_processingState = Idle;
			_processMsg();
			// end
		}
	} else {
		for (int i = 0; i < msgs.size(); i++) {
			unit.msg = msgs.at(i);
			unit.type = ProcessingUnit::Receive;
			_msgQueue.enqueue(unit);
		}
		_processMsg();
	}
}

void Endpoint::_error()
{
	_nBytes = 0;
	disconnect(_socket, SIGNAL(bytesWritten(qint64)),
			   this, SLOT(_bytesWritten(quint64)));
	disconnect(_socket, SIGNAL(error(QAbstractSocket::SocketError)),
			   this, SLOT(_error()));
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
	_curMsg = unit.msg;
	if (unit.type == ProcessingUnit::Send) _processSend();
	else if (unit.type == ProcessingUnit::SendAndReceive)
		_processSendAndReceive(unit.timeout);
	else if (unit.type == ProcessingUnit::Receive) {
		emit messageReceived(unit.msg);
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

	QByteArray bytes = _curMsg.raw();
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

*/
