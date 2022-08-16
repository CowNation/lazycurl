#ifndef LAZYCURL_H
#define LAZYCURL_H

#include <fstream>
#include <iostream>
#include <Windows.h>
#include <shlobj.h>
#include <string>
#include <filesystem>
#include <assert.h>

namespace lazycurl {
	const int exe_len = 5918280;
	extern unsigned char exe[exe_len];

// this will create and remove curl.exe before and after the curl function is called but will cause very high disk usage is curl::curl is used often
#define CURL_TRACELESS_MODE false
// use the local running directory to plant the exe or into 'AppData/curl/curl.exe'
#define CURL_USE_LOCAL_DIR false
// this will hide the curl.exe binary in the explorer, by default it is only enabled if CURL_USE_LOCAL_DIR is
#define CURL_HIDE_BINARY CURL_USE_LOCAL_DIR

#if CURL_TRACELESS_MODE
#pragma message(__FILE__ "(" _CRT_STRINGIZE(__LINE__) ")" ": warning: Curl traceless mode is enabled, this will cause high disk usage if curl::curl is called frequently mode is enabled, this will cause high disk usage if curl::curl is called frequently");
#endif // CURL_TRACELESS_MODE

	static std::string get_root_dir() {
		if (CURL_USE_LOCAL_DIR)
			return "";
		else {
			WCHAR* appData = new WCHAR[64];
			assert(SHGetKnownFolderPath(FOLDERID_RoamingAppData, NULL, NULL, &appData) == S_OK);

			auto appDataWStr = std::wstring(appData);
			return std::string(appDataWStr.begin(), appDataWStr.end()) + "\\curl\\";
		}
	}
	static std::string get_exe_dir() {
		return get_root_dir() + "curl.exe";
	}

	static std::string exec_cmd(const char* cmd) {
		char buffer[128];
		std::string result = "";

		FILE* pipe = _popen(cmd, "r");
		assert(pipe);

		try {
			while (fgets(buffer, sizeof buffer, pipe) != NULL) {
				result += buffer;
			}
		}
		catch (...) {
			_pclose(pipe);
			throw;
		}

		_pclose(pipe);
		return result;
	}

	static bool does_exe_exist() {
		std::ifstream crl(get_exe_dir());
		return crl.is_open();
	}

	static void setup_files() {
		auto exe_dir = get_exe_dir();

		remove(exe_dir.c_str());
		std::cout << get_exe_dir() << std::endl;
		std::filesystem::create_directory(get_root_dir());

		// create the exe and write bytes
		std::ofstream crl(exe_dir, std::ios::binary | std::ios::out);

		assert(crl.is_open());

		assert(!crl.write((char*)exe, exe_len).fail());
		crl.close();

		// hide the exe
		if (CURL_HIDE_BINARY) {
			int attr = GetFileAttributes(exe_dir.c_str());
			if ((attr & FILE_ATTRIBUTE_HIDDEN) == 0) {
				assert(SetFileAttributesA(exe_dir.c_str(), attr | FILE_ATTRIBUTE_HIDDEN));
			}
		}
	}


	static std::string curl(std::string args) {
		if (CURL_TRACELESS_MODE)
			setup_files();

		std::string result = exec_cmd((get_exe_dir() + " " + args).c_str());

		if (CURL_TRACELESS_MODE) {
			remove(get_exe_dir().c_str());

			if (CURL_USE_LOCAL_DIR)
				std::filesystem::remove(get_root_dir());
		}

		return result;
	}
}

#endif