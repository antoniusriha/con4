/*
 * joingamesetupview.cpp
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
#include <QDialog>
#include "joingamesetupview.h"
#include "ui_joingamesetupview.h"
#include "con4globals.h"
#include "application.h"
#include "indexserversview.h"

JoinGameSetupView::JoinGameSetupView (QWidget *parent)
    : QWidget (parent), ui (new Ui::JoinGameSetupView) {
    ui->setupUi (this);

}

JoinGameSetupView::~JoinGameSetupView () { delete ui; }

void JoinGameSetupView::joinClicked () {
    emit statusChanged ("Joining...");
}

void JoinGameSetupView::refreshClicked () {
    emit statusChanged ("Refreshing...");

    QSettings settings;
    int size = settings.beginReadArray (IDX_SRV_ARRAY);
    for (int i = 0; i < size; i++) {
        settings.setArrayIndex (i);
        QString name = settings.value (IDX_SRV_NAME).toString ();
        QString ip = settings.value (IDX_SRV_ADDR).toString ();
        QString qsPort = settings.value (IDX_SRV_PORT).toString ();



        QTextStream ts (&qsPort);
        quint16 port = 0;
        ts >> port;



    }
    settings.endArray ();


//    QStandardItemModel model (0, 3);
//    for (int row = 0; row < 4; ++row)  {
//        for (int column = 0; column < 4; ++column)  {
//            QStandardItem *item = new QStandardItem(QString("row %0, column %1").arg(row).arg(column));
//            model.setItem(row, column, item);
//        }
//    }



    /*
      Status, Spielername, Spielname, Breite, Höhe, Tiefe, IP-Adresse, Port, IndexServerName (IS-IP, IS-Port)

Anzahl der Spiele S,
(Status, Spielername, Spielname, Breite, Höhe, Tiefe, IP-Adresse, Port) × S
    */

/*


    QString host(argv[1]);

    quint16 port;
    stringstream sport(argv[2]);
    if ( !(sport >> port) || !sport.eof()) {
        cerr << "Error: " << argv[2] << " is not a valid port" << endl;
    }

    *
     * Kommunikation mit Echo Server
    *

    // Mit Host verbinden
    QTcpSocket socket;
    socket.connectToHost(host, port);

    if (!socket.waitForConnected()) {
        cerr << "Error: Could not connect to " << argv[1] << ":" << port << endl;
        return 1;
    }

    cout << "Connected." << endl;

    // Antwort von der Konsole lesen, an Server schicken und auf Antwort warten
    string input;
    while (!cin.eof()) {
        cout << "Input: ";
        getline(cin, input);
        input += "\n";

        socket.write(input.c_str());
        if (!socket.waitForBytesWritten()) {
            cout << "Failed to send data to server." << endl;
            return 1;
        }

        if (!socket.waitForReadyRead()) {
            cerr << "No response from server." << endl;
            return 1;
        }

        int bytes_read = socket.read(buffer, BUF_SIZE-1);
        cout << bytes_read << endl;

        buffer[bytes_read] = 0; // String terminieren

        cout << "Response from server: " << buffer << endl;
    }

    socket.disconnect();
    */
    emit statusChanged ("List refreshed.");
}

void JoinGameSetupView::viewIndexServersClicked () {
    IndexServersView dlg (this);
    dlg.exec ();
}
