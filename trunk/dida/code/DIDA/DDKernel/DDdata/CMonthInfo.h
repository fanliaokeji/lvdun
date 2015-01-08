#include <string>
#include <vector>
#include <map>
#include "TCalendar.h"

class CMonthInfo
{
public:
	CMonthInfo(void);
	~CMonthInfo(void);
public:
	BOOL InitInfo(const std::string &strDBPath);
	void GetTargetCalendar(int year,int month,int day=0);
private:
	void GetTargetDateCalendar(int year,int month,int day);
	std::string  SpecialHoliday(int year,int month,int day);
public:
	std::vector<CTCalendar*> m_vCalendar ;
private:
	DDCTCalendar m_calendar;
};