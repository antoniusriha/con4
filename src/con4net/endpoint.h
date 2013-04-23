/*
 * endpoint.h
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

#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <QQueue>
#include <QTcpSocket>
#include <QTimer>
#include "message.h"

class Endpoint : public QObject
{
	Q_OBJECT

public:
	enum EndpointError {
		Timeout,
		Fatal
	};

	const static int maxTimeout = 30000;
	const static int minTimeout = 0;

	int timeout() const { return _timeout; }
	void setTimeout(int value);

	void sendMessage(Message msg);
	void disconnectFromHost();
	
signals:
	void messageReceived(Message msg);
	void error(EndpointError err, QString msg,
			   Message msgProcessed = Message());

protected:
	explicit Endpoint(int timeout, QObject *parent = 0);

	QTcpSocket *socket() const { return _socket; }
	void setSocket(QTcpSocket &value);

private slots:
	void _bytesWritten(quint64 bytes);
	void _readyRead();
	void _writeTimeout();
	void _readTimeout();
	void _error(QAbstractSocket::SocketError err);

private:
	void _init(int timeout);
	void _processMsg();

	QQueue<Message> _msgQueue;
	Message _curMsg;
	bool _processing;
	QTcpSocket *_socket;
	int _timeout, _nBytes;
	QTimer *_writeTimer, *_readTimer;
	QByteArray _residuum;
};

#endif // ENDPOINT_H
