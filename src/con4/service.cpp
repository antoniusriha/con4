/*
 * service.cpp
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

#include <stdexcept>
#include "service.h"

using namespace std;

Service::Service (QHostAddress host, quint16 port)
    : _host (host), _port (port) {}

QStringList Service::sendMsg (QString msg) {
    QTcpSocket socket;
    socket.connectToHost (host (), port ());
    if (!socket.waitForConnected (5000))
        throw runtime_error ("Error: Could not connect to server.");

    socket.write (msg.toUtf8 ());
    if (!socket.waitForBytesWritten ()) {
        socket.disconnect ();
        throw runtime_error ("Error: Failed to send data to server.");
    }

    if (!socket.waitForReadyRead ()) {
        socket.disconnect ();
        throw runtime_error ("Error: No response from server.");
    }

    char buffer [MSG_BUF_SIZE];
    int bytesRead = socket.read (buffer, MSG_BUF_SIZE - 1);
    socket.disconnect ();
    buffer [bytesRead] = 0;

    QString qsResp = QString::fromUtf8 (buffer);
    return qsResp.split (MSG_SPLIT_CHAR);
}

Response &Service::fail (Response &resp, QString errMsg) {
    resp.ErrMsg = errMsg;
    resp.Success = false;
    return resp;
}

quint16 Service::toQuint16 (QString qsPort, bool *ok) {
    qsPort.remove (QRegExp ("^[0]*"));
    QTextStream ts (&qsPort);
    quint16 port = 0;
    ts >> port;

    if (qsPort.size () == QString (port).size ()) {
        if (ok != 0) *ok = true;
    } else {
        port = 0;
        if (ok != 0) *ok = false;
    }
    return port;
}
