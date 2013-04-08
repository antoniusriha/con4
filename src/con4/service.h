/*
 * service.h
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

#ifndef SERVICE_H
#define SERVICE_H

#include <QStringList>
#include <QThread>
#include <QtNetwork/QHostAddress>
#include <QTcpSocket>

#define MSG_BUF_SIZE 1024
#define MSG_SPLIT_CHAR QChar::fromAscii(';')
#define INVALID_RESP_ERR "Error: Invalid response from server."

struct Response {
    bool Success;
    QString ErrMsg;
};

class Service : public QThread {
    Q_OBJECT
public:
    static quint16 toQuint16 (QString qsPort, bool *ok = 0);

    Service (QHostAddress host, quint16 port);

protected:
    QHostAddress host () const { return _host; }
    quint16 port () const { return _port; }

    QStringList sendMsg (QString msg);
    Response &fail (Response &resp, QString errMsg);

private:
    QHostAddress _host;
    quint16 _port;
};

#endif // SERVICE_H
