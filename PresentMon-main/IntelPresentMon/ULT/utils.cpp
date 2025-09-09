#include "utils.h"

#include <Windows.h>

string GetCurrentProcessName(){
 static const int kPathSize = 1024;
 char currpath[kPathSize];
 string exename;
 string exepath;
 if (GetModuleFileNameA(NULL, currpath, kPathSize) > 0) {
	 exename = exepath = currpath;
	 size_t sep = exename.find_last_of("\\");
	 if (sep != std::string::npos) {
		 exename = exename.substr(sep + 1, exename.size() - sep - 1);
		 exepath = exepath.substr(0, sep);
	 }
 }

 return exename;
}
