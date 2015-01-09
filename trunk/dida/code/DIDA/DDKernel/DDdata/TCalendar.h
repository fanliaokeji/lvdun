// ***************************************************************************
// *	������ο����й��й�ũ��������
// ***************************************************************************
#pragma once
#include<math.h>
#include <string>
#include "..\Sqlite3\CSqlite3Data.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

struct CTCalendar{
	int month;                  /*  ũ���£�������Ϊ0������Ϊ������-3��ʾ������  */
	int day;                    /*  ũ����                                  */
	int gan;                    /*  ���                                   */
	int zhi;                    /*  ��֧                                    */
	std::string solarcalendar;   /*����      ��ʽ��20140101              */
	std::string cmonth;             /*  ũ����                                  */
	std::string cday;               /*  ũ����                                  */
	std::string yearganzhi;         /*  ���֧                                    */
	std::string monthganzhi;			/*  �¸�֧                                    */
	std::string dayganzhi;			/*  �ո�֧                                    */
	std::string shengxiao;          /*  ��Ф                                    */
	std::string sterm;              /*  ����                                    */
	std::string caltype;            /*  �������ͣ�-1��ʾ������                    */
	std::string weekday;            /*  ���ڼ�                                  */
	std::string zodiac;             /*  ����                                    */
	std::string holiday;            /*  ��������                                 */
	std::string choliday;           /*  ũ������                                 */
};

class DDCTCalendar:public Sqlite3Data
{

public:


	DDCTCalendar(void);
	~DDCTCalendar(void);
	int ctcl_solar_to_lunar(int y,int m,int d,struct CTCalendar* ctc);
	bool InitConnection(const std::string& strDBPath);

private:
	int ctcl_calendar_type(int y, int m, int d, int opt);/*  �ж�Gregorian(����)������Julian(����)��  */
	int ctcl_date_to_days(int y,int m,int d);/*  ����y��m��d����y�������߹�������  */
	int ctcl_days_to_date(int y,int x);/*  ��������y���ղ�����Ϊxʱ����Ӧ��������  */
	int days_from_era(int y);/*  ����y����������  */
	int ctcl_standard_days(int y,int m,int d);/*  ���ص�Ч��׼����  */
	double ctcl_julian_day(int y,int m,int d,int h,int min,int sec,int zone);/*  ����������  */
	int ctcl_day_of_week(int y,int m,int d);/*  ����y��m��d�յ�����ֵ��0Ϊ������  */
	double ctcl_solar_term(int y,int n,int t);/*  ����y���n��������С��Ϊ1�����ղ�����,tȡֵΪ0��1,�ֱ��ʾƽ���Ͷ���  */
	double ctcl_tail(double x);/*  ��x��С������  */
	double ctcl_rem(double x,double w);/*  ��������  */
	double ctcl_round(double x);
	int ctcl_gan(int x);/*  ���ؼ�����x��Ӧ�����������33Ϊ3��  */
	int ctcl_zhi(int x);/*  ���ؼ�����x��Ӧ�ĵ�֧������33Ϊ9��  */
	int ctcl_year_ganzhi(int y,int m,int d);/*  ����y��m��d��hʱ�����֧����1-60��  */
	int ctcl_ganzhi_round(int start, int offset, int round); 
	int ctcl_month_ganzhi(int y,int m,int d); /*  ����y��m��d��hʱ���¸�֧����1-60��  */
	int ctcl_day_ganzhi(int y,int m,int d); /*  ����y��m��d��hʱ���ո�֧����1-60��  */
	double ang(double x,double t,double c1,double t0,double t2,double t3);
	int ctcl_lunar_date(int y,int m,int d);
	int ctcl_leap_month(int y);/*  ����y������£����򷵻�0  */
	int ctcl_lunar_month(int y,int m,int d);/*  ����y��m��d�ն�Ӧ��ũ���·ݣ������ø�����ʾ  */
	std::string lunarstr;
	int maxyear;
	void logSqlError( const std::string& Type, const std::string& error);
	std::string ctcl_common_query(std::string type,const char* szSQL);
};

