#pragma once
#include <string>

class CShell {

public:

	static bool CreateShortCutLink(
		const std::wstring& display_name,
		const std::wstring& src_path,
		const std::wstring& dest_dir,
		const std::wstring& link_arguments,
		const std::wstring& description,
		const std::wstring& icon_path);

	static bool GetShortCutInfo(std::wstring strFileName, std::wstring& strTarget, std::wstring& strArguments);

};
