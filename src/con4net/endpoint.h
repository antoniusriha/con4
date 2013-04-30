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

#include <stdexcept>
#include <QTcpSocket>
#include "message.h"
#include "processingqueue.h"

class Request : public QObject
{
	Q_OBJECT

public:
	explicit Request(QObject *parent = 0);
	virtual ~Request() {}

	bool success() const { return _success; }
	QString errorString() const { return _errorString; }

	void endRequest(bool success, QString errorString = QString());

signals:
	void finished();

private:
	bool _success, _ended;
	QString _errorString;
};


class ReceiveUnit;
class SendUnit;
class SendAndReceiveUnit;
class DisconnectUnit;

class SendRequest : public Request
{
public:
	SendRequest(Message msg, QObject *parent = 0);
	Message message() const { return _msg; }

private:
	Message _msg;
	friend class SendUnit;
};

class SendAndReceiveRequest : public Request
{
public:
	SendAndReceiveRequest(Message msgSent, int timeout = 10000,
						  QObject *parent = 0);
	Message msgSent() const { return _msgSent; }
	Message msgReceived() const { return _msgReceived; }
	int timeout() const { return _timeout; }

private:
	Message _msgSent, _msgReceived;
	int _timeout;
	friend class SendAndReceiveUnit;
};

class Endpoint : public QObject
{
	Q_OBJECT

public:
	class InvalidOperationException : public std::logic_error
	{
	public:
		explicit InvalidOperationException(QString what)
			: logic_error(what.toStdString()) {}
	};

	const static int maxTimeout = 30000;
	const static int minTimeout = 0;

	virtual ~Endpoint();

	int defaultTimeout() const { return _defaultTimeout; }
	void setDefaultTimeout(int value);

	void send(SendRequest &request);
	void sendAndReceive(SendAndReceiveRequest &request);
	void disconnectFromHost(Request &request);

signals:
	void messageReceived(Message msg);

protected:
	explicit Endpoint(int defaultTimeout, QObject *parent = 0);

	QTcpSocket *socket() const { return _socket; }
	void setSocket(QTcpSocket &value);

private slots:
	void _readyRead();

private:
	ProcessingQueue _queue;
	QTcpSocket *_socket;
	int _defaultTimeout;
	QByteArray _residuum;

	friend class ReceiveUnit;
	friend class SendUnit;
	friend class SendAndReceiveUnit;
	friend class DisconnectUnit;
};

#endif // ENDPOINT_H
