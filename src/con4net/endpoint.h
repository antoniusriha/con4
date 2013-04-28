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
#include <QQueue>
#include <QTcpSocket>
#include <QTimer>
#include <QSharedPointer>
#include "message.h"

class Request : public QObject
{
	Q_OBJECT

public:
	explicit Request(QObject *parent = 0);
	virtual ~Request() {}

	bool success() const { return _success; }
	QString errorString() const { return _errorString; }

signals:
	void finished();

protected:
	void endRequest(bool success, QString errorString = QString());

private:
	bool _success, _ended;
	QString _errorString;
};

typedef QSharedPointer<Request> RequestPtr;

class SendingState;
class SendingAndReceivingState;
class DisconnectingState;

class SendRequest : public Request
{
	SendRequest(Message msg, QObject *parent = 0);
	Message message() const { return _msg; }

private:
	Message _msg;
	friend class SendingState;
};

class SendAndReceiveRequest : public Request
{
	SendAndReceiveRequest(Message msgSent, QObject *parent = 0);
	Message msgSent() const { return _msgSent; }
	Message msgReceived() const { return _msgReceived; }

private:

	Message _msgSent, _msgReceived;
	friend class SendingAndReceivingState;
};

class DisconnectRequest : public Request
{
	DisconnectRequest(QObject *parent = 0);

private:
	friend class DisconnectingState;
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

	int defaultTimeout() const { return _defaultTimeout; }
	void setDefaultTimeout(int value);

	bool connected() const;

	Request send(Message msg);
	Request sendAndReceive(Message msg);
	Request sendAndReceive(Message msg, int timeout);
	Request disconnectFromHost();
	
signals:
	void sendFinished(bool success, Message msg, QString failMessage);
	void sendAndReceiveFinished(bool success, Message msg, QString failMessage);
	void disconnectFromHostFinished(bool success, QString failMessage);
	void messageReceived(Message msg);

protected:
	explicit Endpoint(int defaultTimeout, QObject *parent = 0);

	QTcpSocket *socket() const { return _socket; }
	void setSocket(QTcpSocket &value);

private slots:
//	void _bytesWritten(quint64 bytes);
//	void _readyRead();
//	void _error();
//	void _sendAndReceiveTimeout();

private:
	class EndpointState
	{
	public:
		virtual bool connected() const { return false; }
		virtual void setSocket(Endpoint *endpoint, QTcpSocket &value) = 0;
		virtual void send(Endpoint *endpoint, Message msg) = 0;
		virtual void sendAndReceive(Endpoint *endpoint, Message msg,
									int timeout) = 0;
		virtual void disconnectFromHost(Endpoint *endpoint) = 0;
	};

	class DisconnectedState : public EndpointState
	{
	public:
		void setSocket(Endpoint *endpoint, QTcpSocket &value);
		void send(Endpoint *, Message);
		void sendAndReceive(Endpoint *, Message, int);
		void disconnectFromHost(Endpoint *endpoint);
	};

	class ListeningState : public EndpointState
	{
	public:
		bool connected() const { return true; }
		void send(Endpoint *endpoint, Message msg);
		void sendAndReceive(Endpoint *endpoint, Message msg, int timeout);
		void disconnectFromHost(Endpoint *endpoint);
	};

	class SendingState : public EndpointState
	{
	public:
		bool connected() const { return true; }
		void send(Endpoint *endpoint, Message msg);
		void sendAndReceive(Endpoint *endpoint, Message msg, int timeout);
		void disconnectFromHost(Endpoint *endpoint);
	};

	class SendingAndReceivingState : public EndpointState
	{
	public:
		bool connected() const { return true; }
		void send(Endpoint *endpoint, Message msg);
		void sendAndReceive(Endpoint *endpoint, Message msg, int timeout);
		void disconnectFromHost(Endpoint *endpoint);
	};

	class DisconnectingState : public EndpointState
	{
	public:
		bool connected() const { return true; }
		void send(Endpoint *endpoint, Message msg);
		void sendAndReceive(Endpoint *endpoint, Message msg, int timeout);
		void disconnectFromHost(Endpoint *endpoint);
	};



	enum ProcessingState {
		Disconnecting,
		Idle,
		Send,
		SendAndReceive
	};

	struct ProcessingUnit
	{
		enum Type { Send, SendAndReceive, Receive };
		Message msg;
		Type type;
		int timeout;
	};

	void _init(int defaultTimeout);
	void _processMsg();
	void _processSend();
	void _processSendAndReceive(int timeout);

	QQueue<ProcessingUnit> _msgQueue;
	Message _curMsg;
	ProcessingState _processingState;
	QTcpSocket *_socket;
	int _defaultTimeout, _nBytes;
	QTimer _timer;
	QByteArray _residuum;
	bool _waitingForEndReceive;

	EndpointState *_state;
	friend class EndpointState;
};

#endif // ENDPOINT_H
