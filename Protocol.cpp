#include "Protocol.h"
#include <iostream>

Protocol::Protocol() {
	rrq = wrq =  ack =  err = arr = nullptr;
	ZeroMemory(errorMsg, 40);

}
void Protocol::makeRRQ( string& ifileName, int& Odatalen) {
	short opcode = htons(RRQ);
	const short opcodeSize = 2;
	short fNameSize = sizeof(ifileName.c_str());
	short modeSize = sizeof(DATA_MODE);

	Odatalen = opcodeSize + fNameSize + 1+ modeSize +1 ;
	rrq = new char[Odatalen];
	ZeroMemory(rrq, Odatalen);
	memcpy(rrq, &opcode, opcodeSize);
	memcpy(rrq + opcodeSize, ifileName.c_str(), ifileName.length());
	memcpy(rrq + opcodeSize + fNameSize, 0, 1);
	memcpy(rrq + opcodeSize + fNameSize +1 , DATA_MODE, modeSize);
	memcpy(rrq + Odatalen-1, 0, 1);
}

void Protocol::makeWRQ(string& ifileName, int& Odatalen) {
	short opcode = htons(RRQ);
	const short opcodeSize = 2;
	short fNameSize = sizeof(ifileName.c_str());
	short modeSize = sizeof(DATA_MODE);

	Odatalen = opcodeSize + fNameSize + 1 + modeSize + 1;
	wrq = new char[Odatalen];
	ZeroMemory(wrq, Odatalen);
	memcpy(wrq, &opcode, opcodeSize);
	memcpy(wrq + opcodeSize, ifileName.c_str(), ifileName.length());
	memcpy(wrq + opcodeSize + fNameSize, 0, 1);
	memcpy(wrq + opcodeSize + fNameSize + 1, DATA_MODE, modeSize);
	memcpy(wrq + Odatalen - 1, 0, 1);
}

void Protocol::makeDataPacket(const char* idataBlock, const int& iblockNum, bool iendTransmission) {

	short opcode = htons(DATA);
	const short opcodeSize = 2;
	unsigned short blockNum = htons(iblockNum);
	short blockSize = 2;
	arr = new char[opcodeSize + blockSize + TFTP_DATALEN];
	ZeroMemory(arr, opcodeSize + blockSize + TFTP_DATALEN);
	memcpy(arr, &opcode, opcodeSize);
	memcpy(arr + opcodeSize, &blockNum, blockSize);
	memcpy(arr + opcodeSize + blockSize, idataBlock, TFTP_DATALEN);
}

void Protocol::makeACKPacket(u_short& iblockNum, int& Odatalen) {
	short opcode = htons(ACK);
	short blockNum = htons(iblockNum);

	Odatalen = 4;
	ack = new char[Odatalen];
	ZeroMemory(ack, Odatalen);

	memcpy(ack, &opcode, 2);
	memcpy(ack + 2, &blockNum, 2);
	///std::cout << int(ack[1])  <<"\t"<< int(ack[2]) << "\t" << int(ack[3]) <<" ACK packet"<< endl;
}

void Protocol::makeErrorPacket(const int& ierrNo, int& Odatalen) {
	short opcode = htons(OPTERROR);
	short errNo = htons(ierrNo);

	ZeroMemory(errorMsg, 40);
	switch (ierrNo) {
	case FILE_NOT_FOUND:
		strcpy(errorMsg , "File not found");
		break;
	case ACCESS_VOILATION:
		strcpy(errorMsg, "Access violation");
		break;
	case DISK_FULL:
		strcpy(errorMsg, "Disk full or allocation exceeded");
		break;
	case ILLEGAL:
		strcpy(errorMsg, "Illegal TFTP operation");
		break;
	case UNKN_TX_ID:
		strcpy(errorMsg, "Unknown transfer ID");
		break;
	case FILE_ALREADY_EXISTS:
		strcpy(errorMsg, "File already exists");
		break;
	case NO_USER:
		strcpy(errorMsg, "No such user");
		break;
	case 0:
	default:
		strcpy(errorMsg, "Not defined, see error message(if any)");
		break;
	}
	Odatalen = 2 + 2 + 40 + 1;
	err = new char[Odatalen];
	ZeroMemory(err, Odatalen);
	memcpy(err, &opcode, 2);
	memcpy(err + 2, &errNo, 2);
	memcpy(err + 4, errorMsg, 40);
}