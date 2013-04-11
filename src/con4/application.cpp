/*
 * application.cpp
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
#include "con4globals.h"
#include "application.h"

Application::Application () {
    QSettings settings;
    int size = settings.beginReadArray (IDX_SRV_ARRAY);
    for (int i = 0; i < size; i++) {
        settings.setArrayIndex (i);
        QString name = settings.value (IDX_SRV_NAME).toString ();
        QString host = settings.value (IDX_SRV_ADDR).toString ();
        int iPort = settings.value (IDX_SRV_PORT).toInt ();
//        QTextStream ts (&qsPort);
        quint16 port = (quint16)iPort;
//        ts >> port;
        IndexService *service = new IndexService (
                    QHostAddress (host), port, name);
        _indexServices.append (service);
    }
    settings.endArray ();
}

Application::~Application () {
    for (int i = 0; i < _indexServices.size (); i++)
        delete _indexServices [i];
}
