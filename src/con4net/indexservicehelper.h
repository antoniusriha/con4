/*
 * indexservicehelper.h
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

#ifndef INDEXSERVICEHELPER_H
#define INDEXSERVICEHELPER_H

#include "indexservice.h"

class IndexServiceUnit : public ProcessingUnit
{
	Q_OBJECT

protected:
	IndexServiceUnit(ClientEndpoint *endpoint, Request *req,
					 QObject *parent = 0);

	ClientEndpoint *endpoint() const { return _endpoint; }
	Request *req() const { return _req; }
	SendAndReceiveRequest *internalReq() const { return _internalReq; }

	virtual Message getMsg() = 0;
	virtual void onReqSuccess() = 0;

private slots:
	void _conFinished();
	void _finished();
	void _disconFinished() { emit finished(this); }

private:
	void process() { _endpoint->connectToServer(*_conReq); }

	ClientEndpoint *_endpoint;
	Request *_conReq, *_req, *_disconReq;
	SendAndReceiveRequest *_internalReq;
};

class RegisterUnit : public IndexServiceUnit
{
	Q_OBJECT

public:
	RegisterUnit(ClientEndpoint *endpoint, NetworkGameRequest *req,
				 QObject *parent = 0);

private:
	Message getMsg();
	void onReqSuccess();
};

class UnregisterUnit : public IndexServiceUnit
{
	Q_OBJECT

public:
	UnregisterUnit(ClientEndpoint *endpoint, NetworkGameRequest *req,
				   QObject *parent = 0);

private:
	Message getMsg();
	void onReqSuccess();
};

class SealUnit : public IndexServiceUnit
{
	Q_OBJECT

public:
	SealUnit(ClientEndpoint *endpoint, NetworkGameRequest *req,
			 QObject *parent = 0);

private:
	Message getMsg();
	void onReqSuccess();
};

class RequestGameListUnit : public IndexServiceUnit
{
public:
	RequestGameListUnit(IndexService *service, Request *req,
						QObject *parent = 0);

private:
	Message getMsg() { return Messages::requestGameList(); }
	void onReqSuccess();

	IndexService *_service;
};

#endif // INDEXSERVICEHELPER_H
