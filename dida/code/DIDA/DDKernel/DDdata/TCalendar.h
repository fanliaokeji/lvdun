// ***************************************************************************
// *	本代码参考了中国中国农历函数库
// ***************************************************************************
#pragma once
#include<math.h>
#include <string>
#include "..\Sqlite3\CSqlite3Data.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

struct CTCalendar{
	int month;                  /*  农历月：无数据为0，闰月为负，如-3表示闰三月  */
	int day;                    /*  农历日                                  */
	int gan;                    /*  天干                                   */
	int zhi;                    /*  地支                                    */
	std::string solarcalendar;   /*阳历      格式：20140101              */
	std::string cmonth;             /*  农历月                                  */
	std::string cday;               /*  农历日                                  */
	std::string yearganzhi;         /*  年干支                                    */
	std::string monthganzhi;			/*  月干支                                    */
	std::string dayganzhi;			/*  日干支                                    */
	std::string shengxiao;          /*  生肖                                    */
	std::string sterm;              /*  节气                                    */
	std::string caltype;            /*  公历类型：-1表示不存在                    */
	std::string weekday;            /*  星期几                                  */
	std::string zodiac;             /*  星座                                    */
	std::string holiday;            /*  公历节日                                 */
	std::string choliday;           /*  农历节日                                 */
};

class DDCTCalendar:public Sqlite3Data
{

public:


	DDCTCalendar(void);
	~DDCTCalendar(void);
	int ctcl_solar_to_lunar(int y,int m,int d,struct CTCalendar* ctc);
	bool InitConnection(const std::string& strDBPath);

private:
	int ctcl_calendar_type(int y, int m, int d, int opt);/*  判断Gregorian(格里)历还是Julian(儒略)历  */
	int ctcl_date_to_days(int y,int m,int d);/*  返回y年m月d日在y年年内走过的天数  */
	int ctcl_days_to_date(int y,int x);/*  返回阳历y年日差天数为x时所对应的月日数  */
	int days_from_era(int y);/*  返回y年的年差天数  */
	int ctcl_standard_days(int y,int m,int d);/*  返回等效标准天数  */
	double ctcl_julian_day(int y,int m,int d,int h,int min,int sec,int zone);/*  返回儒略日  */
	int ctcl_day_of_week(int y,int m,int d);/*  返回y年m月d日的星期值，0为星期日  */
	double ctcl_solar_term(int y,int n,int t);/*  返回y年第n个节气（小寒为1）的日差天数,t取值为0或1,分别表示平气和定气  */
	double ctcl_tail(double x);/*  求x的小数部分  */
	double ctcl_rem(double x,double w);/*  广义求余  */
	double ctcl_round(double x);
	int ctcl_gan(int x);/*  返回甲子数x对应的天干数（如33为3）  */
	int ctcl_zhi(int x);/*  返回甲子数x对应的地支数（如33为9）  */
	int ctcl_year_ganzhi(int y,int m,int d);/*  返回y年m月d日h时的年干支数（1-60）  */
	int ctcl_ganzhi_round(int start, int offset, int round); 
	int ctcl_month_ganzhi(int y,int m,int d); /*  返回y年m月d日h时的月干支数（1-60）  */
	int ctcl_day_ganzhi(int y,int m,int d); /*  返回y年m月d日h时的日干支数（1-60）  */
	double ang(double x,double t,double c1,double t0,double t2,double t3);
	int ctcl_lunar_date(int y,int m,int d);
	int ctcl_leap_month(int y);/*  返回y年的闰月，无闰返回0  */
	int ctcl_lunar_month(int y,int m,int d);/*  返回y年m月d日对应的农历月份，闰月用负数表示  */
	std::string lunarstr;
	int maxyear;
	void logSqlError( const std::string& Type, const std::string& error);
	std::string ctcl_common_query(std::string type,const char* szSQL);
};

