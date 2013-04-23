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

#include <QVector>
#include "endpoint.h"

void Endpoint::setTimeout(int value)
{
	if (value < minTimeout) value = minTimeout;
	if (value > maxTimeout) value = maxTimeout;
	_timeout = value;
}

void Endpoint::sendMessage(Message msg)
{
	_msgQueue.enqueue(msg);
	_processMsg();
}

void Endpoint::disconnectFromHost()
{
	_socket->disconnectFromHost();
}

Endpoint::Endpoint(int timeout, QObject *parent) : QObject(parent),
	_writeTimer(0), _readTimer(0)
{
	_init(timeout);
}

void Endpoint::setSocket(QTcpSocket &value)
{
	if (_socket) {
		disconnect(_socket, SIGNAL(_bytesWritten(qint64)),
				   this, SLOT(_bytesWritten(quint64)));
		disconnect(_socket, SIGNAL(_readyRead()), this, SLOT(_readyRead()));
		disconnect(_socket, SIGNAL(error(QAbstractSocket::SocketError)),
				   this, SLOT(_error(QAbstractSocket::SocketError)));
	}

	_socket = &value;
	connect(_socket, SIGNAL(_bytesWritten(qint64)),
			this, SLOT(_bytesWritten(quint64)));
	connect(_socket, SIGNAL(_readyRead()), this, SLOT(_readyRead()));
	connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)),
			this, SLOT(_error(QAbstractSocket::SocketError)));
}

void Endpoint::_bytesWritten(quint64 bytes)
{
	_writeTimer->stop();
	_nBytes -= bytes;
	if (_nBytes == 0) {
		_processing = false;
		if (!_msgQueue.empty()) _processMsg();
	} else _writeTimer->start();
}

void Endpoint::_readyRead()
{
	if (_socket->bytesAvailable() > 0) {
		_readTimer->stop();

		QByteArray data = _socket->readAll();
		if (data.isEmpty()) {
			// error
			emit error(Fatal, "Unable to receive data. "
							  "Failed to read from socket.");
			return;
		}

		QVector<Message> msgs = Message::fromBytes(_residuum + data, _residuum);
		for (int i = 0; i < msgs.size(); i++)
			emit messageReceived(msgs.at(i));

		if (!_residuum.isEmpty()) _readTimer->start(_timeout);
	}
}

void Endpoint::_writeTimeout()
{
	QString msg = QString("A timeout of %1ms occured while sending a message.")
			.arg(_timeout);
	emit error(Timeout, msg, _curMsg);
}

void Endpoint::_readTimeout()
{
	QString msg = QString("A timeout of %1ms occured while reading messages.")
			.arg(_timeout);
	emit error(Timeout, msg);
}

void Endpoint::_error(QAbstractSocket::SocketError err)
{
	emit error(Fatal, QString("A socket error occured (%1).").arg(err));
}

void Endpoint::_init(int timeout)
{
	_msgQueue = QQueue<Message>();
	_curMsg = Message();
	_processing = false;
	_socket = 0;
	_timeout = timeout;
	_nBytes = 0;
	_writeTimer = new QTimer(this);
	_writeTimer->setSingleShot(true);
	connect(_writeTimer, SIGNAL(timeout()), this, SLOT(_writeTimeout()));
	_readTimer = new QTimer(this);
	_readTimer->setSingleShot(true);
	connect(_readTimer, SIGNAL(timeout()), this, SLOT(_readTimeout()));
	_residuum = QByteArray();
}

void Endpoint::_processMsg()
{
	if (!_processing) _processing = true;
	_curMsg = _msgQueue.dequeue();

	QByteArray bytes = _curMsg.text();
	int bWritten = 0;
	_nBytes = bytes.size();
	for (int i = 0; i < _nBytes; i += bWritten) {
		bWritten = _socket->write(bytes.mid(i));
		if (bWritten < 0) {
			// error
			emit error(Fatal, "Unable to send a message. "
							  "Failed to write to socket.", _curMsg);
			return;
		}
	}
	_writeTimer->start(_timeout);
}
