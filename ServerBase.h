#pragma once

#include"Pkg.h"
#include "Client.h"
#include <WinSock2.h>
#include <iostream>
#include <vector>

using namespace std;
#pragma comment(lib, "Ws2_32.lib")

class ServerBase {

protected:
	int status, clientAddrSize;
	SOCKET serverSocket;
	sockaddr_in serverAddr, clientAddr;
	u_long socketMode;
protected:
	char buffer[TFTP_BUFLENGTH];

	vector<Client*> clients;

private:
	bool createSocket();

	bool bindSocket();

public:

	ServerBase();

	virtual void stepNetwork() = 0;

	~ServerBase();

};