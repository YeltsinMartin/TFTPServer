#pragma once
#include"Client.h"
#include <array>
using namespace std;

Client::Client(char* ibuffer, SOCKET& iserverSocket, sockaddr_in& iclientAddr) {
	port = iclientAddr.sin_port;
	ackBlockNum = 0;
	datalen = 0;
	ZeroMemory(buffer, TFTP_BUFLENGTH);
	ZeroMemory(dataBuffer, sizeof(dataBuffer));

	memcpy(buffer, ibuffer, TFTP_BUFLENGTH);
	currentStatus = ACTIVE;
	previousStatus = CLIENT_ERROR;
	myServerSocket = iserverSocket;
	myClientSocket = iclientAddr;
	clientAddrSize = sizeof(myClientSocket);
	retryCount = 0;
}

int Client::getPort() const {
	return port;
}

void Client::stepClientStatus() {
	// determine current state
	short opt = 0;
	memcpy(&opt, &buffer, 2);
	opt = ntohs(opt);
	//cout << opt << endl;
	if (opt != 0) {
		switch (opt) {
		case WRQ:
			currentStatus = WRITE_REQUEST;
			break;
		case DATA:
			currentStatus = WRITE_ACK;
			break;
		case RRQ:
			currentStatus = READ_REQUEST;
			break;
		case ACK:
			if (CLIENT_ERROR == previousStatus) {
				currentStatus = TIMED_OUT;
			}
			else {
				currentStatus = READ_ACK;
			}
			break;
		case OPTERROR:
			currentStatus = TIMED_OUT;
			break;
		default:
			break;
		}
		previousStatus = currentStatus;
	}
	else {
		currentStatus = PAUSE;
	}
}

void Client::stepClient() {

	if (READ_REQUEST == currentStatus) {
		// we got a read request
		ackBlockNum = 0;
		char* only_filename = &buffer[2];
		char file_path[512];
		strcpy(file_path, gRootPath.c_str());
		memcpy(file_path + strlen(gRootPath.c_str()), only_filename, strlen(only_filename) + 1);
		if (!Logger::getInstance()->requestFile(only_filename)) {
			// File not available send an err packet
			int datalen;
			makeErrorPacket(FILE_NOT_FOUND, datalen);
			sendto(myServerSocket, err, datalen, 0, (struct sockaddr*)&myClientSocket, clientAddrSize);
			delete[] err;
			currentStatus = TIMED_OUT; // since we sent a error msg we can close the connection
		}
		else {
			//file is present open the file and start sending
			outgoingFile.open(file_path, ios::binary);
			if (outgoingFile.is_open()) {
				cout << "Read Request: " << only_filename << endl;
				// connection established we can send first data paket
				datalen = 4;
				sendData(true);
			}
			else {
				int datalen;
				makeErrorPacket(ACCESS_VOILATION, datalen);
				sendto(myServerSocket, err, datalen, 0, (struct sockaddr*)&myClientSocket, clientAddrSize);
				delete[] err;
				cout << "Sending error ACCESS_VOILATION: " << only_filename << endl;
				currentStatus = TIMED_OUT; // since we sent a error msg we can close the connection
				outgoingFile.close();
			}
		}
	}
	else if (READ_ACK == currentStatus)
	{
		//at this point  we have an open file
		memcpy(&receivedAckBlockNum, &buffer[2], 2);
		receivedAckBlockNum = ntohs(receivedAckBlockNum);
		// if we receive the old block send the new data else send old data until we get ack
		//cout << "recv#" << receivedAckBlockNum << "ack#" << ackBlockNum << endl;
		if (receivedAckBlockNum != ackBlockNum) {
			sendData(true);
		}
		else {
			if (!lastPacketSent) {
				ackBlockNum = receivedAckBlockNum + 1;
				sendData(false);
			}
			else {
				currentStatus = TIMED_OUT;
			}
		}
	}
	else if (PAUSE == currentStatus) {
		// do nothing, not your turn this time :)
	}
	else if (WRITE_REQUEST == currentStatus) {
		ackBlockNum = 0;
		lastPacketSent = false;
		char* only_filename = &buffer[2];
		char file_path[512];
		strcpy(file_path, gRootPath.c_str());
		memcpy(file_path + strlen(gRootPath.c_str()), only_filename, strlen(only_filename) + 1);
	
		if (Logger::getInstance()->addNewFile(only_filename)) {
			// file is already present send a error message
			std::cout << only_filename << " File already exists, Sending Error\n";
			int datalen;
			makeErrorPacket(FILE_ALREADY_EXISTS, datalen);
			sendto(myServerSocket, err, datalen, 0, (struct sockaddr*)&myClientSocket, clientAddrSize);
			delete[] err;
			currentStatus = TIMED_OUT;
		}
		else {
			incomingFile.open(file_path, ios::binary);
			if (incomingFile.is_open()) {
				cout << "Write Request: " << only_filename << endl;// send ack;
				int datalen = 0;
				makeACKPacket(ackBlockNum, datalen);
				sendto(myServerSocket, ack, datalen, 0, (struct sockaddr*)&myClientSocket, clientAddrSize);
				delete[] ack;
			}
			else {
				int datalen;
				makeErrorPacket(ACCESS_VOILATION, datalen);
				sendto(myServerSocket, err, datalen, 0, (struct sockaddr*)&myClientSocket, clientAddrSize);
				delete[] err;
				cout << "Sending error ACCESS_VOILATION: " << only_filename << endl;
				currentStatus = TIMED_OUT; // since we sent a error msg we can close the connection
			}
		}
	}
	else if (WRITE_ACK == currentStatus) {
		memcpy(&receivedAckBlockNum, &buffer[2], 2);
		receivedAckBlockNum = ntohs(receivedAckBlockNum);
		int datalen = bytes - 4;
		//cout <<"reACK# " << receivedAckBlockNum << " bytes " <<bytes<< "datlen "<< datalen << "prt" << port<<endl;
		if (datalen > 0) {
			incomingFile.write(&buffer[4], datalen); // need to get this
			makeACKPacket(receivedAckBlockNum, datalen);
			sendto(myServerSocket, ack, datalen, 0, (struct sockaddr*)&myClientSocket, clientAddrSize);
			delete[] ack;
		}
		if (bytes <= 512) {
			incomingFile.close();
			currentStatus = TIMED_OUT;
		}
	}
	ZeroMemory(buffer, TFTP_BUFLENGTH); // clear the buffer so that it will not work on stale data
}

void Client::sendData(bool old){

	if (old) {
		makeDataPacket(dataBuffer, ackBlockNum, false);
		sendto(myServerSocket, arr, datalen, 0, (struct sockaddr*)&myClientSocket, clientAddrSize);
		delete[] arr;
		//cout << "sending old block " << ackBlockNum << "\t" << datalen << endl;
	}
	else {
		int n = 0;
		if (outgoingFile.is_open() && !outgoingFile.eof()) {
			ZeroMemory(dataBuffer, TFTP_DATALEN);
			outgoingFile.read(dataBuffer, TFTP_DATALEN);
			//cout << dataBuffer;
			n = outgoingFile.gcount();
		}
		if (n < 512) {
			outgoingFile.close();
			lastPacketSent = true;
			currentStatus = TIMED_OUT;
			//cout << "setting time out";
		}
		makeDataPacket(dataBuffer, ackBlockNum, false);
		datalen = n + 4;
		sendto(myServerSocket, arr, datalen, 0, (struct sockaddr*)&myClientSocket, clientAddrSize);
		delete[] arr;
		//cout << "snd ack# " << ackBlockNum << "\t" << datalen << endl;
		
	}

}

ClientStatus Client::getClientStatus() const {
	return currentStatus;
}

void Client::setBuffer(char* ibuffer) {
	memcpy(buffer, ibuffer, TFTP_BUFLENGTH);
}

void Client::setBytes(int& ibytes) {
	bytes = ibytes;
}

Client::~Client() {

}