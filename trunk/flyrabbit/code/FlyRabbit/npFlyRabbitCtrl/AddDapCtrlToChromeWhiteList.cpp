#include "AddDapCtrlToChromeWhiteList.h"
#include <windows.h>
#include <string>
#include <iostream>
#include <map>
#include <algorithm>
#include <Shlwapi.h>
#include "jsonxx.h"

template <class Res, class Deletor>
class ScopeResourceHandle {
	Res res_;
	Deletor deletor_;
	ScopeResourceHandle(const ScopeResourceHandle<Res, Deletor>&) {};
	const ScopeResourceHandle& operator=(const ScopeResourceHandle<Res, Deletor>&) { return *this; }
public:
	ScopeResourceHandle(Res res, Deletor deletor) : res_(res){
		this->deletor_ = deletor;
	}
	~ScopeResourceHandle() {
		this->deletor_(this->res_);
	}
	Res get() {
		return this->res_;
	}
};

bool AddDapCtrlToChromeWhiteList(const std::wstring& preferencesFileName, const std::wstring& dapCtrlFileName, int& error_code)
{
	if(dapCtrlFileName.empty()) {
		error_code = 9;
		return false;
	}

	// daoCtrlFileName转UTF8
	int length = ::WideCharToMultiByte(CP_UTF8, 0, dapCtrlFileName.data(), dapCtrlFileName.size(), NULL, 0, NULL, NULL);
	if(length == 0) {
		error_code = 9;
		return false;
	}

	std::string dapCtrlFileNameUTF8;
	dapCtrlFileNameUTF8.resize(length);
	if(::WideCharToMultiByte(CP_UTF8, 0, dapCtrlFileName.data(), dapCtrlFileName.size(), const_cast<char*>(dapCtrlFileNameUTF8.data()), length, NULL, NULL) != length) {
		error_code = 9;
		return false;
	}

	// 检查首选项文件是否存在
	if(::PathFileExists(preferencesFileName.c_str()) == FALSE) {
		error_code = 1;
		return false;
	}

	// 以独占读写的方式打开文件
	HANDLE hFile = CreateFile(preferencesFileName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE) {
		error_code = 2;
		return false;
	}
	
	// 函数返回时自动关闭文件
	ScopeResourceHandle<HANDLE, BOOL(WINAPI*)(HANDLE)> fileHandle(hFile, ::CloseHandle);

	BOOL readResult = FALSE;
	char buffer[1024];
	std::string jsonData;
	for(;;)
	{
		DWORD numberOfBytesRead = 0;
		readResult = ReadFile(hFile, buffer, 1024, &numberOfBytesRead, NULL);
		if(readResult == FALSE || numberOfBytesRead == 0) {
			// 发生错误或读取到文件尾
			break;
		}
		jsonData.append(buffer, numberOfBytesRead);
		// 文件超过5MB不处理
		if(jsonData.size() > 5 * 1024 * 1024) {
			readResult = FALSE;
			break;
		}
	}
	if(readResult == FALSE) {
		error_code = 2;
		return false;
	}
	jsonxx::Object preferencesObj;
	if(!preferencesObj.parse(jsonData)) {
		error_code = 3;
		return false;
	}
	// 先检查chrome的主版本号是否不小于32
	if(!preferencesObj.has<jsonxx::Object>("extensions")) {
		error_code = 4;
		return false;
	}

	const jsonxx::Object& extensionsObj = preferencesObj.get<jsonxx::Object>("extensions");
	if(!extensionsObj.has<jsonxx::String>("last_chrome_version")) {
		error_code = 4;
		return false;
	}
	const std::string last_chrome_version = extensionsObj.get<jsonxx::String>("last_chrome_version");
	int last_chrome_major_version = 0;
	std::stringstream ss;
	ss << last_chrome_version;
	ss >> last_chrome_major_version;
	if(last_chrome_major_version < 32) {
		error_code = 5;
		return false;
	}
	if(!preferencesObj.has<jsonxx::Object>("profile")) {
		error_code = 4;
		return false;
	}
	jsonxx::Object& profileObj = preferencesObj.get<jsonxx::Object>("profile");
	// content_settings
	if(!profileObj.has<jsonxx::Object>("content_settings")) {
		error_code = 4;
		return false;
	}

	jsonxx::Object& contentSettingsObj = profileObj.get<jsonxx::Object>("content_settings");
	// pattern_pairs
	if(!contentSettingsObj.has<jsonxx::Object>("pattern_pairs")) {
		contentSettingsObj << "pattern_pairs" << jsonxx::Object();
		if(!contentSettingsObj.has<jsonxx::Object>("pattern_pairs")) {
			error_code = 4;
			return false;
		}
	}
	jsonxx::Object& patternPairsObj = contentSettingsObj.get<jsonxx::Object>("pattern_pairs");

	// *,*
	if(!patternPairsObj.has<jsonxx::Object>("*,*")) {
		patternPairsObj << "*,*" << jsonxx::Object();
		if(!patternPairsObj.has<jsonxx::Object>("*,*")) {
			error_code = 4;
			return false;
		}
	}
	jsonxx::Object& matchAllObj = patternPairsObj.get<jsonxx::Object>("*,*");

	// per_plugin
	if(!matchAllObj.has<jsonxx::Object>("per_plugin")) {
		matchAllObj << "per_plugin" << jsonxx::Object();
		if(!matchAllObj.has<jsonxx::Object>("per_plugin")) {
			error_code = 4;
			return false;
		}
	}

	jsonxx::Object& perPluginObj = matchAllObj.get<jsonxx::Object>("per_plugin");
	
	jsonxx::Object oldPerPluginObj = perPluginObj;

	perPluginObj.reset();
	bool success = true;
	// 移除所有以dapCtrl开头的插件
	const std::string dapCtrlPrefix = "npKKDapCtrl";
	for(std::map<std::string, jsonxx::Value*>::const_iterator iter = oldPerPluginObj.kv_map().begin(); iter != oldPerPluginObj.kv_map().end(); ++iter) {
		if(iter->first.size() > dapCtrlPrefix.size()
			&& std::equal(dapCtrlPrefix.begin(), dapCtrlPrefix.end(), iter->first.begin())) {
			continue;
		}
		if(!oldPerPluginObj.has<jsonxx::Number>(iter->first)) {
			// 存在意料之外的值类型
			error_code = 6;
			success = false;
			break;
		}
		perPluginObj << iter->first << oldPerPluginObj.get<jsonxx::Number>(iter->first);
	}
	if(!success) {
		return false;
	}

	// 将dapCtrl加入白名单
	perPluginObj << dapCtrlFileNameUTF8 << jsonxx::Number(1);

	jsonData = preferencesObj.json();
	// 移动文件指针到文件头
	if(::SetFilePointer(hFile, 0, NULL, FILE_BEGIN) != 0) {
		error_code = 7;
		return false;
	}
	DWORD bytesOfWrite = 0;
	BOOL writeResult = ::WriteFile(hFile, jsonData.data(), static_cast<DWORD>(jsonData.size()), &bytesOfWrite, NULL);
	if(writeResult == FALSE) {
		error_code = 7;
		return false;
	}
	// 设置当前位置为文件尾
	::SetEndOfFile(hFile);
	return true;
}
