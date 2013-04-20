/*
 * settings.cpp
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

#include <QSettings>
#include "settings.h"

Settings::Settings() : _keys(QList<QString>()) {}

bool Settings::registerKey(QString key)
{
	if (_keys.contains(key)) return false;
	_keys.append(key);
	return true;
}

Settings::ValuesArray Settings::getArray(QString arrayName,
										 QList<QString> attrNames)
{
	QSettings settings;
	ValuesArray result;
    int size = settings.beginReadArray(arrayName);
	for (int i = 0; i < size; i++) {
        settings.setArrayIndex(i);
		QList<QVariant> values;
        for (int j = 0; j < attrNames.size(); j++)
            values.append(settings.value(attrNames[j]));
		result.append(values);
	}
    settings.endArray();
    return result;
}

void Settings::setArray(QString arrayName, QList<QString> attrNames,
                        ValuesArray attrVals)
{
    QSettings settings;
    settings.beginGroup(arrayName);
    settings.remove("");
    settings.endGroup();

    settings.beginWriteArray(arrayName);
    for (int i = 0; i < attrVals.size(); i++) {
        settings.setArrayIndex(i);
        for (int j = 0; j < attrNames.size(); j++)
            settings.setValue(attrNames.at(j), attrVals.at(i).at(j));
    }
    settings.endArray ();
}
