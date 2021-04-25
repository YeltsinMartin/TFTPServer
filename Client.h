#pragma once
#include "Protocol.h"
#include "Logger.h"
enum  ClientStatus {
	ACTIVE,
	TIMED_OUT,
	WRITE_REQUEST,
	READ_REQUEST,
	WRITE_ACK,
	READ_ACK,
	CLIENT_ERROR,
	PAUSE
};

extern string gRootPath;

class Client final : public Protocol{

	public:

		Client(char* ibuffer, SOCKET &iserverSocket, sockaddr_in& iclientAddr);

		int getPort() const;

		void stepClientStatus();

		void stepClient();

		ClientStatus getClientStatus() const;

		void setBuffer(char* ibuffer);

		void setBytes(int& ibytes);

		~Client();

	private:

		void sendData(bool old);

		bool lastPacketSent;

		char buffer[TFTP_BUFLENGTH], dataBuffer[TFTP_DATALEN];

		int datalen, clientAddrSize, bytes;
		
		u_short receivedAckBlockNum, ackBlockNum;

		SOCKET myServerSocket;

		sockaddr_in myClientSocket;

		ofstream incomingFile;

		ifstream outgoingFile;

		int n;

	protected:

		ClientStatus currentStatus, previousStatus;

		unsigned int port;

		int retryCount;

};