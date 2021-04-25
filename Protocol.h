#pragma once
#include"Pkg.h"
#include <string>


#define DATA_MODE "octet"
#define TFTPMODE_NETASCII 0
#define TFTPMODE_OCTET 1

enum ERRORCODE {
	FILE_NOT_FOUND = 1,
	ACCESS_VOILATION,
	DISK_FULL,
	ILLEGAL,
	UNKN_TX_ID,
	FILE_ALREADY_EXISTS,
	NO_USER
};

enum OPTCODE {
	RRQ = 1,
	WRQ,
	DATA,
	ACK,
	OPTERROR
};

using namespace std;

class Protocol {


public:
	char* rrq, * wrq, * ack, *err, *arr;
	char errorMsg[40];

	Protocol();

protected:

	void makeRRQ(string& ifileName, int& Odatalen);

	void makeWRQ(string& ifileName, int& Odatalen);

	void makeDataPacket(const char* idataBlock, const int& iblockNum, bool iendTransmission);

	void makeACKPacket(u_short& iblockNum, int& Odatalen);

	void makeErrorPacket(const int& ierrNo, int& Odatalen);
}; 