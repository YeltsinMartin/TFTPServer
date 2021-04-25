#include "ServerBase.h"

extern char IP[], PORT[];
ServerBase::ServerBase() {
	ZeroMemory(buffer, TFTP_BUFLENGTH);

	clientAddrSize = sizeof(clientAddr);
	socketMode = 1; // 0 blocking 1 non-blocking

	//WSA initialize
	WSADATA wsadata;
	WORD version = MAKEWORD(2, 2);
	bool lWSAstatus = WSAStartup(version, &wsadata);
	bool lcreateStatus = createSocket();
	bool lbindStatus = bindSocket();

	if ((NO_ERROR != lWSAstatus) || !(lcreateStatus && lbindStatus)) {
		cout << "Error at Windows Socket Architecture initialization. Error code: " << WSAGetLastError() << endl;
		cout << "Quitting" << endl;
		system("pause");
		exit(0);
	}
	cout << "Windows Socket Initialized." << endl;
}

ServerBase::~ServerBase() {
	WSACleanup();
	cout << "Windows Socket is closed." << endl;
	cout << "TFTP protocol is closed." << endl;

}

bool ServerBase::createSocket() {

	status = serverSocket = socket(PF_INET, SOCK_DGRAM, 0);

	if (status == SOCKET_ERROR) {
		cout << "Error at socket creating. Error code: " << WSAGetLastError() << endl;
		WSACleanup();
		return false;
	}

	//enable for non blocking
	if (NO_ERROR != ioctlsocket(serverSocket, FIONBIO, &socketMode)) {
		cout << "ioctlsocket failed with error: " << WSAGetLastError() << endl;
	}

	return true;

}

bool ServerBase::bindSocket() {
	serverAddr.sin_family = AF_INET;
	//cout << IP << PORT;
	serverAddr.sin_port = htons(atoi(PORT));
	serverAddr.sin_addr.S_un.S_addr = inet_addr(IP);
	ZeroMemory(serverAddr.sin_zero, sizeof(serverAddr.sin_zero));
	cout << "Server config " << IP << "\t"<< atoi(PORT) << endl;
	status = bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (status == SOCKET_ERROR) {
		cout << "Error at binding. Error code: " << WSAGetLastError() << endl;
		WSACleanup();
		return false;
	}
	return true;
}

