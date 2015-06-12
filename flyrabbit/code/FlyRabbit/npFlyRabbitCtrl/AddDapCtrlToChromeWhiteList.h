#pragma once
#include <string>

// 将dapCtrlFileName加入到chrome的白名单中，同时移除之前白名单中所有以npKKDapCtrl开头的插件
// 不会检查chrome进程是否存在
//
// @preferencesFileName 参数为Preferences文件的路径
// 对于XP是%USERPROFILE%\Local Settings\Application Data\Google\Chrome\User Data\Default\Preferences
// 对于Vista Or Later 是  %LOCALAPPDATA%\Google\Chrome\User Data\Default\Preferences
// 需要先展开前面的%USERPROFILE% 或 %LOCALAPPDATA%
// @dapCtrlFileName 参数为npDataCtrl.dll的文件名 如npKKDapCtrl.3.1.0.9.(847).dll
// @error_code 错误码
// 0: 成功
// 1: 首选项文件不存在
// 2: 打开或读取首选项文件出错
// 3: 解析首选项json失败
// 4: 找不到某个期望的key
// 5: chrome版本号小于32
// 6: per_plugin中存在意料之外的值
// 7: 首选项文件写入出错 包括移动文件指针到文件头和写入操作
// 9: 其他错误
// @返回值 成功返回true，失败返回false
bool AddDapCtrlToChromeWhiteList(const std::wstring& preferencesFileName, const std::wstring& dapCtrlFileName, int& error_code);
