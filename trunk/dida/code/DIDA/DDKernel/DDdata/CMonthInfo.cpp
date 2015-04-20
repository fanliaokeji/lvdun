#include "stdafx.h"
#include "CMonthInfo.h"
//#include <boost/timer.hpp>  
#include <boost/date_time.hpp>
#include "TCalendar.h"
//#include <boost/date_time/gregorian/gregorian.hpp>  
using namespace boost::gregorian; 

CMonthInfo::CMonthInfo(void)
{
}

CMonthInfo::~CMonthInfo(void)
{
	TSTRACEAUTO();
	for (std::vector<CTCalendar*>::const_iterator c_iter =  m_vCalendar.begin() ; c_iter != m_vCalendar.end();c_iter++)
	{
		delete (*c_iter);
	}
	
}

BOOL CMonthInfo::InitInfo(const std::string &strDBPath)
{
	TSTRACEAUTO();
	if (!m_calendar.InitConnection(strDBPath))
	{
		return FALSE;
	}
	return TRUE;
}

std::string CMonthInfo::SpecialHoliday(int year,int month,int day)
{
	typedef nth_day_of_the_week_in_month type_ndwm;  
	type_ndwm ndwmfather(type_ndwm::third, 0, 6);  
	date dayfather = ndwmfather.get_date(year);      //父亲节 每年6月份的第3个星期日 
	if (dayfather.month() == month && dayfather.day() == day)
	{
		return "\347\210\266\344\272\262\350\212\202";
	}
	type_ndwm ndwmmother(type_ndwm::second, 0, 5);  //母亲节 每年5月份的第2个星期日
	date daymother = ndwmmother.get_date(year);   
	if (daymother.month() == month && daymother.day() == day)
	{
		return "\346\257\215\344\272\262\350\212\202";
	}
	type_ndwm ndwmthanks (type_ndwm::fourth, 4, 11);  //感恩节 每年11月份的第4个星期日
	date daythanks = ndwmthanks.get_date(year);   
	if (daythanks.month() == month && daythanks.day() == day)
	{
		return "\346\204\237\346\201\251\350\212\202";
	}
	return "";
}

void CMonthInfo::GetTargetDateCalendar(int year,int month,int day)
{
	CTCalendar  *ctc = new CTCalendar ;
	if (-1 == m_calendar.ctcl_solar_to_lunar(year,month,day,ctc))
	{
		return;
	}
	std::string strHoliday = SpecialHoliday(year,month,day);
	if (!strHoliday.empty())
	{
		ctc->holiday.insert(0,strHoliday);
	}
	m_vCalendar.push_back(ctc);
}

void CMonthInfo::GetTargetCalendar(int year,int month,int day)
{
	TSTRACEAUTO();
	if (day != 0)
	{
		GetTargetDateCalendar(year,month,day);
	}
	else
	{
		date select_date(year,month,1);
		day_iterator day_itr(select_date);
		for (int day_week = select_date.day_of_week();day_week>0; --day_week)
		{
			--day_itr;
		}
		for (int index = 0;index < 42;++index)
		{
			GetTargetDateCalendar((*day_itr).year(),(*day_itr).month(),(*day_itr).day());
			++day_itr;
		}
	}

}