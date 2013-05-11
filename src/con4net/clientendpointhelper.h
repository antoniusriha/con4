/*
 * clientendpointhelper.h
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

#ifndef CLIENTENDPOINTHELPER_H
#define CLIENTENDPOINTHELPER_H

#include "clientendpoint.h"

class ConnectUnit : public ProcessingUnit
{
	Q_OBJECT

public:
	ConnectUnit(QTcpSocket *socket, QHostAddress ipAddress, quint16 port,
				Request *req, QObject *parent = 0);

protected:
	void process();

private slots:
	void _error();
	void _connected();

private:
	void _cleanup();

	QTcpSocket *_socket;
	QHostAddress _ipAddress;
	quint16 _port;
	Request *_req;
};

#endif // CLIENTENDPOINTHELPER_H
