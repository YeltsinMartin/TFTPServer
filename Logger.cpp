#include"Logger.h"

extern string gRootPath;

Logger* Logger::pInstance = nullptr;

Logger::Logger(){
	
	for (const auto& fileName : filesystem::directory_iterator(gRootPath)) {
		folder.fileNameList.push_back(fileName.path().filename().string());
		folder.countList.push_back(0);
		folder.newFileFlag.push_back(0);
	}
}

bool Logger::requestFile(string pfileName) {

	bool isFilePresent = false;
	for (unsigned int i = 0; i < folder.fileNameList.size(); i++) {

		if (folder.fileNameList.at(i).find(pfileName) != -1) {
			folder.countList.at(i)++; //increment the value
			isFilePresent = true;
			break;
		}
	}
	return isFilePresent;
}

void Logger::logStat() {

	logFile.open(string(gRootPath).append("Log.txt"), ios::app); //Open Log file

	logFile << "#####	Priting the file system status	#####\n";
	logFile << "FileName\t\tDownload\t\tNewFile\n";

	for (unsigned int i = 0; i < folder.countList.size(); i++) {
		logFile << folder.fileNameList.at(i) << [=]() {if (folder.fileNameList.at(i).length() > 8) return "\t\t"; else return "\t\t\t";   }() << folder.countList.at(i) << "\t\t\t" <<
			[=]()-> const char* {if (folder.newFileFlag.at(i)) return "Yes\n"; else return "No\n"; }();
	}
	logFile << "*********************************************\n\n\n";
	//After printing clear the New File Flag. At this point it will be old file.
	for (auto& i : folder.newFileFlag) {
		i = 0;
	}
	logFile.close();
	cout << "Logged data to File\n";
}

bool Logger::addNewFile(string pfileName) {
	//add file and set a flag for logging
	bool isFileInList = false;
	for (auto fileName : folder.fileNameList) {
		if (pfileName.compare(fileName) == 0){
			isFileInList = true;
			break;
		}
	}
	if (!isFileInList) {
		folder.fileNameList.push_back(pfileName);
		folder.countList.push_back(0);
		folder.newFileFlag.push_back(1);
	}
	return isFileInList;
}

Logger* Logger::getInstance() {

	if(nullptr == pInstance) pInstance = new Logger();
	return pInstance;
}

Logger::~Logger() {
	
}