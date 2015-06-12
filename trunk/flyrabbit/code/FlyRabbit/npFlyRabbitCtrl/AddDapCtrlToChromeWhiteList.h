#pragma once
#include <string>

// ��dapCtrlFileName���뵽chrome�İ������У�ͬʱ�Ƴ�֮ǰ��������������npKKDapCtrl��ͷ�Ĳ��
// ������chrome�����Ƿ����
//
// @preferencesFileName ����ΪPreferences�ļ���·��
// ����XP��%USERPROFILE%\Local Settings\Application Data\Google\Chrome\User Data\Default\Preferences
// ����Vista Or Later ��  %LOCALAPPDATA%\Google\Chrome\User Data\Default\Preferences
// ��Ҫ��չ��ǰ���%USERPROFILE% �� %LOCALAPPDATA%
// @dapCtrlFileName ����ΪnpDataCtrl.dll���ļ��� ��npKKDapCtrl.3.1.0.9.(847).dll
// @error_code ������
// 0: �ɹ�
// 1: ��ѡ���ļ�������
// 2: �򿪻��ȡ��ѡ���ļ�����
// 3: ������ѡ��jsonʧ��
// 4: �Ҳ���ĳ��������key
// 5: chrome�汾��С��32
// 6: per_plugin�д�������֮���ֵ
// 7: ��ѡ���ļ�д����� �����ƶ��ļ�ָ�뵽�ļ�ͷ��д�����
// 9: ��������
// @����ֵ �ɹ�����true��ʧ�ܷ���false
bool AddDapCtrlToChromeWhiteList(const std::wstring& preferencesFileName, const std::wstring& dapCtrlFileName, int& error_code);
