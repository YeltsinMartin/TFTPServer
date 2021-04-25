#include"Server.h"
#define DEBUG

Server::Server() {
	cout << "Server Online!\n";
}

void Server::stepNetwork() {
	ZeroMemory(buffer, TFTP_BUFLENGTH);
	status = recvfrom(serverSocket, buffer, TFTP_BUFLENGTH, 0, (struct sockaddr*)&clientAddr, &clientAddrSize);
	if (SOCKET_ERROR != status) {
		// we have recieved something
		//cout << int(buffer[1]) << endl;
		//check to see if its the same client
		bool lfound = false;
		for (auto myClient : clients) {
			if (myClient->getPort() == clientAddr.sin_port) {	//takes care of matching TID's and discarding other packets
				lfound = true;
				myClient->setBuffer(buffer); 
				myClient->setBytes(status);
				break;
			}
		}
		// no client found so creating a new client and adding to list
		if (!lfound) {
			clients.push_back(new Client( buffer, serverSocket, clientAddr ));
		}
		
	}
	// if so resume the activity
	vector<vector<Client*>::iterator> clearRequests;
	//cout << clients.size() << endl;
	for (auto myClient = clients.begin(); myClient != clients.end(); ++myClient) {
		(*myClient)->stepClientStatus();
		(*myClient)->stepClient();

		//Find if the client timedout or nomally terminated
		if (TIMED_OUT == (*myClient)->getClientStatus()) {
			clearRequests.push_back(myClient);
			delete (*myClient);
		}
	}
	// Kill the timed out clinets
	for (auto& i : clearRequests) {
		clients.erase(i);
	}

}

Server::~Server() {
	cout << "Server Offline";
}
