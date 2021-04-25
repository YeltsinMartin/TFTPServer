#include<iostream>
#include<chrono>
#include "Server.h"

using namespace std;
using namespace std::chrono;

constexpr auto LOG_PERIOD = 1;		//time in minutes

string gRootPath= "C:\\Users\\yelas\\Documents\\Server\\";
char IP[14], PORT[5];

int main(int argc, char* argv[]) {

	memset(IP, 0, 14);
	memset(PORT, 0, 5);
	strcpy(IP, "127.0.0.1");
	strcpy(PORT, "69");

	if (argc < 3) {
		std::cerr << "Usage:  Server.exe [-host] [-port]" << std::endl;
		cout << "booting with default config\n";
		cout << "IP: 127.0.0.1 PORT: 69\n";
	}
	if (3 == argc) {
		char* c = strstr(argv[1], "-host");
		if (c) strcpy(IP, c + 5);
		c = strstr(argv[2], "-port");
		if (c) strcpy(PORT, c + 5);
	}

	Server server;
	auto time = system_clock::now();
	while (1) {
		server.stepNetwork();
		if (system_clock::now()- time > minutes(LOG_PERIOD)) {
			//log activity
			Logger::getInstance()->logStat();
			time = system_clock::now();
		}
	}
	return 0;
}