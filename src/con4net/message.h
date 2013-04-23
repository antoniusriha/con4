/*
 * message.h
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

#ifndef MESSAGE_H
#define MESSAGE_H

#include <QVector>
#include <QString>
#include <QStringList>

class Message
{
public:
	const static char MsgEndChar = '\n';
	const static char MsgSplitChar = ';';
	static QVector<Message> fromBytes(QByteArray bytes, QByteArray &residuum);

	Message();
	Message(const QString &header);
	Message(const char *header);
	Message(const QByteArray &bytes);

	QString header() const { return _header; }
	QStringList *params() { return &_params; }

	QByteArray text() const;

	bool isValid() const { return !_header.isEmpty(); }

private:
	QString _header;
	QStringList _params;
};

#endif // MESSAGE_H
