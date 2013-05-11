/*
 * clientendpointhelper.cpp
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

#include "clientendpointhelper.h"

ConnectUnit::ConnectUnit(QTcpSocket *socket, QHostAddress ipAddress,
						 quint16 port, Request *req, QObject *parent)
	: ProcessingUnit(parent), _socket(socket), _ipAddress(ipAddress),
	  _port(port), _req(req) {}

void ConnectUnit::process()
{
	connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)),
			this, SLOT(_error()));
	connect(_socket, SIGNAL(connected()), this, SLOT(_connected()));
	_socket->connectToHost(_ipAddress, _port);
}

void ConnectUnit::_error()
{
	_req->endRequest(false, _socket->errorString());
	_cleanup();
}

void ConnectUnit::_connected()
{
	_req->endRequest(true);
	_cleanup();
}

void ConnectUnit::_cleanup()
{
	disconnect(_socket, SIGNAL(connected()), this, SLOT(_connected()));
	disconnect(_socket, SIGNAL(error(QAbstractSocket::SocketError)),
			   this, SLOT(_error()));
	emit finished(this);
}
