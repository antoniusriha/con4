/*
 * networkplayerservice.cpp
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

#include <QNetworkInterface>
#include "networkplayerservice.h"

NetworkPlayerService::NetworkPlayerService (QString initiatorName,
    QString gameName, int width, int height, int depth)
    : _gameName (gameName), _initiatorName (initiatorName),
      _game (4, width, height, depth) {}

void NetworkPlayerService::assignIpAddress () {
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses ();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size (); i++) {
        if (ipAddressesList.at (i) != QHostAddress::LocalHost &&
            ipAddressesList.at (i).toIPv4Address ()) {
            _ipAddress = ipAddressesList.at (i);
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (_ipAddress.isNull ())
        _ipAddress = QHostAddress (QHostAddress::LocalHost);
}
