/*
 * addindexserverdialog.cpp
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

#include <QHostAddress>
#include <QMessageBox>
#include <QSettings>
#include "con4globals.h"
#include "application.h"
#include "addindexserverdialog.h"
#include "ui_addindexserverdialog.h"

AddIndexServerDialog::AddIndexServerDialog (QWidget *parent)
    : QDialog (parent), ui (new Ui::AddIndexServerDialog) {
    ui->setupUi(this);
}

AddIndexServerDialog::~AddIndexServerDialog () {
    delete ui;
}

void AddIndexServerDialog::done (int result) {
    if (QDialog::Accepted == result) {
        int nErr = 0;
        QString errMsg;

        QHostAddress ip;
        if (!ip.setAddress (ui->txtIp->text ().trimmed ())) {
            nErr++;
            errMsg.append ("Error: Please specify a valid ip address (e.g.: 10.0.0.1)\n");
        }

        QString name = ui->txtName->text ().trimmed ();
        if (name.isEmpty ()) name = ip.toString ();
        if (_nameExists (name)) {
            nErr++;
            errMsg.append ("Error: The specified name (or ip address, if no name was provided) exists already.\n")
                  .append ("Please choose another one.\n");
        }

        if (nErr > 0) {
            QString finalErrMsg = QString ("%1 error(s) occured:\n").arg (nErr).append (errMsg);
            QMessageBox::critical (this, "Error", finalErrMsg);
        } else {
            quint16 port = (quint16)ui->sbPort->value ();
            IndexService *indexService = new IndexService (ip, port, name);
            Application::instance ().indexServices ()->append (indexService);

            // save to settings
            QSettings settings;
            int size = settings.beginReadArray (IDX_SRV_ARRAY);
            settings.endArray ();
            settings.beginWriteArray (IDX_SRV_ARRAY);
            settings.setArrayIndex (size);
            settings.setValue (IDX_SRV_NAME, name);
            settings.setValue (IDX_SRV_ADDR, ip.toString ());
            settings.setValue (IDX_SRV_PORT, port);
            settings.endArray();

            QDialog::done (result);
        }
    } else QDialog::done (result);
}

bool AddIndexServerDialog::_nameExists (QString name) {
    QList<IndexService *> *services = Application::instance ().indexServices ();
    for (int i = 0; i < services->size (); i++)
        if ((*services) [i]->name () == name) return true;
    return false;
}
