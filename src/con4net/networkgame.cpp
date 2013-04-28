/*
 * networkgame.cpp
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

#include "networkgame.h"

NetworkGameConf::NetworkGameConf() : _dims()
{
	setInitiatorName("Initiator");
	setName("Initiator's network game");
	setIpAddress(QHostAddress::LocalHost);
	setPort(9999);
}

void NetworkGameConf::setInitiatorName(NetworkString value)
{
	if (value.string().isEmpty())
		throw Exception("InitiatorName must be a non-empty string.");
	_initiatorName = value;
}

void NetworkGameConf::setName(NetworkString value)
{
	if (value.string().isEmpty())
		throw Exception("Name must be a non-empty string.");
	_name = value;
}

void NetworkGameConf::setIpAddress(QHostAddress value)
{
	if (value.isNull())
		throw Exception("IpAddress must be a non-null host address.");
	_ipAddress = value;
}

void NetworkGameConf::setPort(quint16 value)
{
	if (value < 1025)
		throw Exception("Port must be a port number above 1024.");
	_port = value;
}

NetworkGame::NetworkGame(NetworkGameConf conf)
	: Game(conf.dims()), _conf(conf), _guid() {}

NetworkGame::~NetworkGame() {}
