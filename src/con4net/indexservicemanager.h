/*
 * indexservicemanager.h
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

#ifndef INDEXSERVICEMANAGER_H
#define INDEXSERVICEMANAGER_H

#include "indexservicelist.h"

struct ErroneousService
{
	IndexService *service;
	QString errorString;
};

class IndexServiceManager : public QObject
{
	Q_OBJECT

public:
	IndexServiceManager(IndexServiceList &list, NetworkGame &game,
						QObject *parent = 0);

	void registerGame();
	void sealGame();
	void unregisterGame();

signals:
	void registerGameFinished(bool overallSuccess,
							  QList<ErroneousService> errServices);
	void sealGameFinished(bool overallSuccess,
						  QList<ErroneousService> errServices);
	void unregisterGameFinished(bool overallSuccess,
								QList<ErroneousService> errServices);

private slots:
	void _indexServiceDeleting(IndexService *service);
	void _reqFinished(Request *request);

private:
	enum RequestType { Register, Seal, Unregister };

	struct TransactionData : public QObject
	{
		TransactionData(RequestType reqType, int nServices, QObject *parent = 0)
			: QObject(parent), reqType(reqType), nServices(nServices), count(0),
			  success(false), errServices() {}

		RequestType reqType;
		int nServices, count;
		bool success;
		QList<ErroneousService> errServices;
	};

	void _runRequest(RequestType reqType);

	IndexServiceList &_list;
	QList<IndexService *> _activeServices;
	NetworkGame &_game;
	TransactionData *_tData;
};

#endif // INDEXSERVICEMANAGER_H
