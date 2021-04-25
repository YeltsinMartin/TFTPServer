#ifndef LOGGER
#define LOGGER

#include <vector>
#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>

using namespace std;

class Logger {
	struct serverFileSystem {
		vector <string> fileNameList;
		vector <int> countList;
		vector <int> newFileFlag;
	};

	serverFileSystem folder;

	fstream logFile;

	Logger();

	static Logger* pInstance;

public:

	static Logger* getInstance();

	bool requestFile(string pfileName);

	void logStat();

	bool addNewFile(string pfileName);

	~Logger();
};
#endif
