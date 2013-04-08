#include <iostream>
#include <string>
#include <sstream>

#include <QString>
#include <QtNetwork/QTcpSocket>

#define BUF_SIZE 1024
#define SYNOPSIS "echo_client <IPv4 Address> <Port>"

using namespace std;

static char buffer[BUF_SIZE];

int main(int argc, char** argv)
{
	/*
	 * Argumentbehandlung (IP und Port aus Konsole lesen)
	*/
	if (argc != 3) {
		cerr << SYNOPSIS << endl;
		return 1;
	}

	QString host(argv[1]);
	
	quint16 port;
	stringstream sport(argv[2]);
	if ( !(sport >> port) || !sport.eof()) {
		cerr << "Error: " << argv[2] << " is not a valid port" << endl;
	}

	/*
	 * Kommunikation mit Echo Server
	*/

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
		
		cout << endl;
		QString resp = QString (buffer);
		qDebug () << resp << endl;
	}

	socket.disconnect();

	return 0;
}
