/*
 * endpointhelper.h
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

#ifndef ENDPOINTHELPER_H
#define ENDPOINTHELPER_H

#include <QTimer>
#include "endpoint.h"

class ReceiveUnit : public ProcessingUnit
{
	Q_OBJECT

public:
	ReceiveUnit(Endpoint *endpoint, Message msg, QObject *parent = 0);

private:
	void process();

	Endpoint *_endpoint;
	Message _msg;
};

class SendUnit : public ProcessingUnit
{
	Q_OBJECT

public:
	SendUnit(Endpoint *endpoint, SendRequest *req, QObject *parent = 0);

private slots:
	void _bytesWritten(qint64 bytes);
	void _error();

private:
	void process();

	Endpoint *_endpoint;
	SendRequest *_req;
	int _nBytes;
};

class SendAndReceiveUnit : public ProcessingUnit
{
	Q_OBJECT

public:
	SendAndReceiveUnit(Endpoint *endpoint, SendAndReceiveRequest *req,
					   QObject *parent = 0);

private slots:
	void _bytesWritten(qint64 bytes);
	void _readyRead();
	void _error();
	void _timeout();

private:
	void process();

	Endpoint *_endpoint;
	SendAndReceiveRequest *_req;
	QTimer _timer;
	int _nBytes;
	bool _waitingForEndReceive;
};

class DisconnectUnit : public ProcessingUnit
{
	Q_OBJECT

public:
	DisconnectUnit(Endpoint *endpoint, Request *req, QObject *parent = 0);

private slots:
	void _disconnected();
	void _error();

private:
	void process();

	Endpoint *_endpoint;
	Request *_req;
};

#endif // ENDPOINTHELPER_H
