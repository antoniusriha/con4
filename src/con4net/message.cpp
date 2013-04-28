/*
 * message.cpp
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
#include "message.h"

using namespace std;

NetworkString::NetworkString(QString string)
{
	setString(string);
}

NetworkString::NetworkString(const char *string)
{
	setString(string);
}

void NetworkString::setString(QString value)
{
	if (value.contains(Message::MsgEndChar) ||
		value.contains(Message::MsgSplitChar)) {
		QString errMsg = QString("value must not contain the "
								 "characters '%1' or '%2'.")
				.arg(Message::MsgEndChar).arg(Message::MsgSplitChar);
		throw invalid_argument(errMsg.toStdString());
	}
	_string = value;
}

QVector<Message> Message::fromBytes(QByteArray bytes, QByteArray &residuum)
{
	QList<QByteArray> msgsBytes = bytes.split(MsgEndChar);
	QVector<Message> msgs(msgsBytes.size() - 1);
	for (int i = 0; i < msgsBytes.size() - 1; i++)
		msgs[i] = Message(msgsBytes.at(i));
	residuum = msgsBytes.last();
	return msgs;
}

Message::Message(const NetworkString &header) : _header(header), _params() {}

Message::Message(const QByteArray &bytes) : _header(), _params()
{
	QList<QByteArray> tokens = bytes.split(MsgSplitChar);
	if (!tokens.empty()) _header = QString::fromUtf8(tokens.first());
	for (int i = 1; i < tokens.size(); i++)
		_params.append(QString::fromUtf8(tokens.at(i)));
}

QByteArray Message::raw() const
{
	QString msg = _header.string();
	for (int i = 0; i < _params.size(); i++)
		msg.append(MsgSplitChar + _params.at(i).string());
	msg.append(MsgEndChar);
	return msg.toUtf8();
}
