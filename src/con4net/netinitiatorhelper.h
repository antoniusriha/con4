/*
 * netinitiatorhelper.h
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

#ifndef NETINITIATORHELPER_H
#define NETINITIATORHELPER_H

#include "indexservicelist.h"

enum State { NotRegistered, Registered, Sealed, Unregistered };

class IndexServiceWrapper : public QObject
{
	Q_OBJECT

public:
	IndexServiceWrapper(IndexService &service, NetworkGame &game,
						QObject *parent = 0);

	IndexService *service() const { return &_service; }
	State state() const { return _state; }

	const NetworkGameRequest *regRequest() const { return &_regReq; }
	const NetworkGameRequest *sealRequest() const { return &_sealReq; }
	const NetworkGameRequest *unregRequest() const { return &_unregReq; }

	void registerGame() { _service.registerGame(_regReq); }
	void sealGame()	{ _service.sealGame(_sealReq); }
	void unregisterGame() { _service.unregisterGame(_unregReq); }

signals:
	void registerGameFinished(IndexServiceWrapper *wrapper);
	void sealGameFinished(IndexServiceWrapper *wrapper);
	void unregisterGameFinished(IndexServiceWrapper *wrapper);

private slots:
	void _registerGameFinished(Request *request);
	void _sealGameFinished(Request *request);
	void _unregisterGameFinished(Request *request);

private:
	IndexService &_service;
	NetworkGame &_game;
	State _state;
	NetworkGameRequest _regReq, _sealReq, _unregReq;
};


class IndexServiceManager : public QObject
{
	Q_OBJECT

public:
	struct ErroneousServices
	{
		IndexService *service;
		QString errorString;
	};

	IndexServiceManager(IndexServiceList &list, NetworkGame &game,
						QObject *parent = 0);
	~IndexServiceManager() { qDeleteAll(_wrappers); }

	void registerGame();
	void sealGame();
	void unregisterGame();

signals:
	void registerGameFinished(bool overallSuccess,
							  QList<ErroneousServices> errServices);
	void sealGameFinished(bool overallSuccess,
						  QList<ErroneousServices> errServices);
	void unregisterGameFinished(bool overallSuccess,
								QList<ErroneousServices> errServices);

private slots:
	void _indexServiceDeleting(IndexService *service);
	void _registerGameFinished(IndexServiceWrapper *wrapper);
	void _sealGameFinished(IndexServiceWrapper *wrapper);
	void _unregisterGameFinished(IndexServiceWrapper *wrapper);

private:
	IndexServiceList &_list;
	NetworkGame &_game;
	QList<IndexServiceWrapper *> _wrappers;
};

#endif // NETINITIATORHELPER_H
