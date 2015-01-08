// ***************************************************************************
// *	������ο����й��й�ũ��������
// ***************************************************************************
#include "stdafx.h"
#include<string.h>
//#include "sqlite3.h"

#include "TCalendar.h"
const int beginning_year=-849;  //��¼�ӹ�Ԫǰ850�꿪ʼ
const int baseIndex = 0;

//
//ũ��������"lunar.db"��sqlite3���ݿ��У������������ֲ
//

DDCTCalendar::DDCTCalendar(void)
{
}

DDCTCalendar::~DDCTCalendar(void)
{
	
}

// /***************************************************************************
// *������int ctcl_calendar_type(int y,int m,int d,int opt);
// *���ܣ��ж�Gregorian(����)������Julian(����)��
// *����������y��m��(1,2,..,12,��ͬ)d��,opt=1,2,3�ֱ��ʾ��׼����,Gregorge����Julian��
// *����ֵ��1(������)��0(������)��-1(������)
// ***************************************************************************/

int DDCTCalendar::ctcl_calendar_type(int y, int m, int d, int opt)
{
  int days_of_month[13]={0,31,28,31,30,31,30,31,30,30,31,30,31};
  if(opt==1){
    if(y>1582 || (y==1582 && m>10) || (y==1582 && m==10 && d>14) ){
      if( (y%400==0) || (y%4==0 && y%100!=0) )days_of_month[2]++;
      if(m>0 && m<=12 && d>0 && d<=days_of_month[m])
	return(1);  //Gregorian
      else
	return(-1);
    }else if(y==1582 && m==10 && d>=5 && d<=14){ 
      return(-1);  /*  ��ʮ������ʷ�ϲ�����  */
    }else{
      if(y%4==0)days_of_month[2]++;
      if(m>0 && m<=12 && d>0 && d<=days_of_month[m])
	return(0);  //Julian
      else 
	return(-1);
    }
  }
  
  if(opt==2)
    return(1);  //Gregorian
  return(0);  //Julian
}
// /**************************************************************************
// *������int ctcl_date_to_days(int y,int m,int d);
// *���ܣ�����y��m��d����y�������߹�������
// **************************************************************************/

int DDCTCalendar::ctcl_date_to_days(int y,int m,int d)
{
  int days=0,i;
  int type=ctcl_calendar_type(y,m,d,1);
  int dm[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};//Days of each month
  if(type!=0){
    if( (y%100!=0 && y%4==0) || (y%400==0) )
      dm[2]+=1;
  }else{
    if(y%4==0)
      dm[2]+=1;
  }
  for(i=0;i<=m-1;i++){
    days+=dm[i];
  }
  days+=d;
  if(y==1582){
    if(type==1)
      days-=10;
    if(type==-1)
      days=-1;  /*  ��Ч  */
  }
  return days;
}

// /***************************************************************************
// *���� int ctcl_days_to_date(int y,int x);
// *���ܣ���������y���ղ�����Ϊxʱ����Ӧ��������
// *     ����y=2000��x=274ʱ������1001(��ʾ10��1�գ�������100*m+d)��
// **************************************************************************/

int DDCTCalendar::ctcl_days_to_date(int y,int x)
{
  int m=1;
  int i;
  for(i=1;i<=12;i++){
    int d=ctcl_date_to_days(y,i+1,1)-ctcl_date_to_days(y,i,1);
    if(x<=d || i==12){
      m=i;
      break;
    }else{ 
       x-=d; 
    }
  }
  return 100*m+x;
}

// /***************************************************************************
// *������int day_from_era(int y);
// *���ܣ�����y������������y��1��1�վ���Ӧ���ֵ�1��1��1�յ�������
// ***************************************************************************/

int DDCTCalendar::days_from_era(int y)
{
  int days=(y-1)*365 + (y-1)/4;  /*  Julian���������  */
  if(y>1582)
    days+= -((y-1)/100) + (y-1)/400;  /*  Gregorian���������  */
  return days;
}

// /***************************************************************************
// *������int ctcl_standard_days(int y,int m,int d);
// *���ܣ����ص�Ч��׼����
// *     ��y��m��d����Ӧ���ֵ�1��1��1�յĵ�Ч(����Gregorian����Julian��
// *       ��ͳһ��)������
// ***************************************************************************/

int DDCTCalendar::ctcl_standard_days(int y,int m,int d)
{
  int days=(y-1)*365 + (y-1)/4 + ctcl_date_to_days(y,m,d) - 2;  /*  Julian�ĵ�Ч��׼����  */
  if(y>1582)
    days+= -((y-1)/100) + ((y-1)/400) + 2;  /*  Gregorian�ĵ�Ч��׼����  */
  return days;
}

// /***************************************************************************
// *������double ctcl_julian_day(int y,int m,int d,int h,int min,int sec,int zone);
// *���ܣ����������գ�zoneʱ��y��m��d��hʱmin��sec�����������Ԫǰ4713��
// *      1��1�ո�������ʱ������12ʱ��������
// ***************************************************************************/

double DDCTCalendar::ctcl_julian_day(int y,int m,int d,int h,int min,int sec,int zone)
{
  int type=ctcl_calendar_type(y,m,d,1);
  double jt=(h+(min+sec/60.0)/60.0)/24.0-0.5-zone/24.0;
  double jd=(type!=0) ? (ctcl_standard_days(y,m,d)+1721425+jt) : 
    (ctcl_standard_days(y,m,d)+1721425+jt);/*  ������  */
  return jd;
}

// /***************************************************************************
// *������int ctcl_day_of_week(int y,int m,int d);
// *���ܣ�����y��m��d�յ�����ֵ��0Ϊ������
// ***************************************************************************/
int DDCTCalendar::ctcl_day_of_week(int y,int m,int d)
{
  return ctcl_standard_days(y,m,d)%7;
}



// /***************************************************************************
// *������double ctcl_solar_term(int y,int n,int t);
// *���ܣ�����y���n��������С��Ϊ1�����ղ�����,tȡֵΪ0��1,�ֱ��ʾƽ���Ͷ���
// ***************************************************************************/

double DDCTCalendar::ctcl_solar_term(int y,int n,int t)
{
  double jd = y * (365.2423112 - 6.4e-14*(y-100)*(y-100) - 3.047e-8*(y-100))
    + 15.218427 * n + 1721050.71301;/*  ������  */
  double zeta = 3.0e-4 * y - 0.372781384 - 0.2617913325 * n;/*  �Ƕ�  */
  double yd = ( 1.945 * sin(zeta) - 0.01206 * sin(2*zeta) ) 
    * ( 1.048994 - 2.583e-5 *y );/*  ���ʵ����  */
  double sd =-18e-4*sin(2.313908653*y-0.439822951-3.0443*n);/*  ˷��ʵ����  */
  return (t==1)?(jd+yd+sd-ctcl_standard_days(y,1,0)-1721425)
    :(jd-ctcl_standard_days(y,1,0)-1721425);
}

// /****************************************************************************
// *������double ctcl_tail(double x);
// *���ܣ���x��С������
// ***************************************************************************/

double DDCTCalendar::ctcl_tail(double x)
{
  return x-floor(x);
}
// /****************************************************************************
// *������double ctcl_rem(double x,double w);
// *���ܣ���������
// ***************************************************************************/

double DDCTCalendar::ctcl_rem(double x,double w)
{
  return ctcl_tail(x/w)*w;
}

// /****************************************************************************
// *������double ctcl_round(double x);
// *���ܣ���������
// ***************************************************************************/

double DDCTCalendar::ctcl_round(double x){
  return floor(x+0.5);
}

// /***************************************************************************
// *������int ctcl_gan(int x);
// *���ܣ����ؼ�����x��Ӧ�����������33Ϊ3��
// ***************************************************************************/

int DDCTCalendar::ctcl_gan(int x)
{
  return x%10;
}

// /***************************************************************************
// *������int ctcl_zhi(int x);
// *���ܣ����ؼ�����x��Ӧ�ĵ�֧������33Ϊ9��
// ***************************************************************************/

int DDCTCalendar::ctcl_zhi(int x)
{
  return x%12;
}

// /***************************************************************************
// *������int ctcl_year_ganzhi(int y,int m,int d,int h);
// *���ܣ�����y��m��d��hʱ�����֧����1-60��
// ***************************************************************************/

int DDCTCalendar::ctcl_year_ganzhi(int y,int m,int d,int h)
{
  if ( (ctcl_date_to_days(y,m,d)+h/24.0) 
       < ctcl_solar_term(y,3,1)-1.0 )/*  �ж��Ƿ������  */
    y-=1;
  return (int)ctcl_round( ctcl_rem(y-3.0,60.0) ); 
}
// ***************************************************************************
// **TODO:ctcl_month_ganzhi, ctcl_day_ganzhi, ctcl_hour_ganzhi
// ***************************************************************************

//�ǶȺ���(˽��)
double DDCTCalendar::ang(double x,double t,double c1,double t0,double t2,double t3)
{
  return ctcl_tail(c1*x)*2*M_PI+t0-t2*t*t-t3*t*t*t;
}

// /****************************************************************************
// *������int ctcl_lunar_date(int y,int m,int d);
// *���ܣ�����ũ������
// ***************************************************************************/

int DDCTCalendar::ctcl_lunar_date(int y,int m,int d)
{
  int lunar_date=-1;
  const double rpi=180/M_PI;
  const double zone=8.0;  /*  ʱ��:������  */
  double t=(y-1899.5)/100.0;
  double ms=floor((y-1900)*12.3685);
  double f0=ang(ms,t,0,0.75933,2.172e-4,1.55e-7)
    +0.53058868*ms-8.37e-4*t+zone/24.0+0.5;
  double fc=0.1734-3.93e-4*t;
  double j0=693595+29*ms;
  double aa0=ang(ms,t,0.08084821133,359.2242/rpi,
		 0.0000333/rpi,0.00000347/rpi);
  double ab0=ang(ms,t,7.171366127999999e-2,306.0253/rpi,
		 -0.0107306/rpi,-0.00001236/rpi);
  double ac0=ang(ms,t,0.08519585128,21.2964/rpi,
		 0.0016528/rpi,0.00000239/rpi);
  int i;
  for(i=-1;i<=13;i+=1){  /*  k=����Ϊ˷,k=������Ϊ��  */
    double aa=aa0+0.507984293*i;
    double ab=ab0+6.73377553*i;
    double ac=ac0+6.818486628*i;
    double f1=f0+1.53058868*i+fc*sin(aa)-0.4068*sin(ab)+0.0021*sin(2*aa)
      +0.0161*sin(2*ab)+0.0104*sin(2*ac)-0.0074*sin(aa-ab)-0.0051*sin(aa+ab);
    double j=j0+28*i+f1;  /*  ˷�����ĵ�Ч��׼������ʱ��  */
    int diff=ctcl_standard_days(y,m,d)-floor(j);  /*  ��ǰ�վ�˷�յĲ�ֵ  */
    if( diff>=0 && diff<=29 )
      lunar_date=diff+1;
  } 
  return(lunar_date);
}

// /****************************************************************************
// *������int ctcl_leap_month(int y);
// *���ܣ�����y������£����򷵻�0
// ***************************************************************************/

int DDCTCalendar::ctcl_leap_month(int y)
{
	char leap=lunarstr.at(y-beginning_year);
	//char leap;
	if(leap=='a')
		leap='0'+10;
	if(leap=='b')
		leap='0'+11;
	if(leap=='c')
		leap='0'+12;
	return leap-'0';
}

// /****************************************************************************
// *������int ctcl_lunar_month(int y,int m,int d);
// *���ܣ�����y��m��d�ն�Ӧ��ũ���·ݣ������ø�����ʾ
// ***************************************************************************/

int DDCTCalendar::ctcl_lunar_month(int y,int m,int d)
{
  double lunar_date=ctcl_lunar_date(y,m,d);
  int lunar_days=lunar_date-floor(lunar_date/100)*100;  /*  ũ������  */
  int leap_num=0;  /*  �ӵ��굽-849�����������  */
  int i;
  int non_leap;
  int lunar_month;
  
  for(i=-849;i<=y;i++){
    if(ctcl_leap_month(i)!=0)
      leap_num++;
  }
  
  non_leap=ctcl_round((ctcl_standard_days(y,m,d)
	      -ctcl_standard_days(-849,1,21)
	      -lunar_days)/29.530588)-leap_num;
  /*  �ӵ��굽-849�����Ч������(�۳�����)  */
  
  if(y<=240) non_leap++;
  if(y<=237) non_leap--;
  if(y<24) non_leap++;
  if(y<9) non_leap--;
  if(y<=-255) non_leap++;   
  if(y<=-256) non_leap+=2;
  if(y<=-722) non_leap++;  /*  ��ʷ�ϵ��޸��½�  */

  lunar_month=ctcl_round(ctcl_rem(non_leap-3.0,12.0)+1.0);
  if(lunar_month==ctcl_leap_month(y-1) && m==1 && d<lunar_days)
    lunar_month*=-1;    /*  ���y-1��ĩ�������Ҹ��½ӵ���y��,��y�����Ҳ������  */
  else if(lunar_month==ctcl_leap_month(y))
    lunar_month*=-1;
  else if(lunar_month<ctcl_leap_month(y) || ( m<lunar_month && ctcl_leap_month(y) ) )
    lunar_month++;  /*  ���y�������µ�����δ��������ǰ���۳��˱�������£�����Ӧ������  */
  else lunar_month=ctcl_round(ctcl_rem(lunar_month-1,12)+1);

  return lunar_month;  
}

// /***************************************************************************
// *������int ctcl_solar_to_lunar(int y,int m,int d,struct CTCalendar* ctc);
// *���ܣ��ѹ���y��m��d��ת��Ϊũ�������ctc��
// *����ֵ��-1���������ڲ��Ϸ���0��ũ�������ݣ�1��һ������
// ***************************************************************************/

int DDCTCalendar::ctcl_solar_to_lunar(int y,int m,int d,struct CTCalendar* ctc)
{
	int r,i;
	char buf[10] = {0};
	sprintf(buf,"%d%02d%02d", y,m,d);
	ctc->solarcalendar = buf;
	
	 memset(buf,0,10);
	sprintf(buf,"%02d%02d", m,d);
	
	char szSQLBuff[MAX_PATH] = {0};
    sprintf(szSQLBuff,"select VALUE from holiday where DATA='%s'", buf);
    ctc->holiday=ctcl_common_query("holiday",szSQLBuff);
    memset(szSQLBuff,0,MAX_PATH);

    int type=ctcl_calendar_type(y,m,d,1);
    sprintf(szSQLBuff, "select VALUE from caltype where ID=%d",type);
    ctc->caltype=ctcl_common_query("caltype",szSQLBuff);
    memset(szSQLBuff,0,MAX_PATH);

    if(type==-1)return -1;

    sprintf(szSQLBuff, "select VALUE from weekday where ID=%d",ctcl_day_of_week(y,m,d));
    ctc->weekday=ctcl_common_query("weekday",szSQLBuff);
    memset(szSQLBuff,0,MAX_PATH);

    sprintf(szSQLBuff, "select VALUE from zodiac where DATEB<=%s and DATEE>='%s'",buf,buf);
    ctc->zodiac=ctcl_common_query("zodiac",szSQLBuff);
    memset(szSQLBuff,0,MAX_PATH);

    ctc->gan=ctcl_gan(ctcl_year_ganzhi(y,m,d,12));
    ctc->zhi=ctcl_zhi(ctcl_year_ganzhi(y,m,d,12));

	sprintf(szSQLBuff, "select VALUE from tiangan where ID=%d",ctc->gan);
	ctc->ganzhi=ctcl_common_query("gan",szSQLBuff);
    memset(szSQLBuff,0,MAX_PATH);

	sprintf(szSQLBuff, "select VALUE from dizhi where ID=%d",ctc->zhi);
	ctc->ganzhi.append(ctcl_common_query("zhi",szSQLBuff));
    memset(szSQLBuff,0,MAX_PATH);
	
	 sprintf(szSQLBuff, "select VALUE from shengxiao where ID=%d",ctc->zhi);
	 ctc->shengxiao=ctcl_common_query("shengxiao",szSQLBuff);
	 memset(szSQLBuff,0,MAX_PATH);

	for(i=0;i<24;i++)
	{
		r=ctcl_days_to_date(y,ctcl_solar_term(y,i+1,1));
		if (r == m*100+d )
		{
			sprintf(szSQLBuff, "select VALUE from sterm where ID=%d",i);
			ctc->sterm=ctcl_common_query("sterm",szSQLBuff);
			memset(szSQLBuff,0,MAX_PATH);
		}
	 }

	if(y>=maxyear)
	{
		ctc->day=0;
		ctc->month=0;
		ctc->cday="unknown";
		ctc->cmonth="no data";
		return 0;
	}

	ctc->day=ctcl_lunar_date(y,m,d);
	if(ctc->day<=10)
    {
		sprintf(szSQLBuff, "select VALUE from cnumber where ID=%d",ctc->day);
		ctc->cday = "\345\210\235";/*  ��  */
		ctc->cday += ctcl_common_query("day",szSQLBuff);
		memset(szSQLBuff,0,MAX_PATH);
	}
	else if(ctc->day<20)
	{
		sprintf(szSQLBuff, "select VALUE from cnumber where ID=%d",ctc->day%10);
		ctc->cday = "\345\215\201";/*  ʮ  */
		ctc->cday += ctcl_common_query("day",szSQLBuff);
		memset(szSQLBuff,0,MAX_PATH);
	}
	else if(ctc->day==20)
	{
		ctc->cday = "\344\272\214\345\215\201";/*  ��ʮ  */
	}
	else if(ctc->day<30)
	{
		sprintf(szSQLBuff, "select VALUE from cnumber where ID=%d",ctc->day%10);
		ctc->cday = "\345\273\277";/*  إ  */
		ctc->cday += ctcl_common_query("day",szSQLBuff);
		memset(szSQLBuff,0,MAX_PATH);
	}
	else if(ctc->day==30)
	{
		ctc->cday = "\344\270\211\345\215\201";/*  ��ʮ  */
	}

	ctc->month=ctcl_lunar_month(y,m,d);
	if(ctc->month==-12)
	{
		ctc->cmonth = "\351\227\260\345\215\201\344\272\214\346\234\210";/*  "��ʮ����"  */
	}
	else if(ctc->month==-11)
	{
		ctc->cmonth = "\351\227\260\345\215\201\344\270\200\346\234\210";/*  "��ʮһ��"  */
	}
	else if(ctc->month==-1)
	{
		ctc->cmonth = "\351\227\260\346\255\243\346\234\210"; /*  "������"  */
	}
	else if(ctc->month<0)
	{
		ctc->cmonth = "\351\227\260"; /*  "��"  */
		sprintf(szSQLBuff, "select VALUE from cnumber where ID=%d",-(ctc->month));
		ctc->cmonth.append(ctcl_common_query("month",szSQLBuff));
		ctc->cmonth.append("\346\234\210"); /*   ��  */
		memset(szSQLBuff,0,MAX_PATH);
	}
	else if(ctc->month==13)
	{
		ctc->cmonth = "\346\255\243\346\234\210"; /*  "����"  */
	}
	else
	{
		sprintf(szSQLBuff, "select VALUE from cnumber where ID=%d",ctc->month);
		ctc->cmonth = ctcl_common_query("month",szSQLBuff);
		ctc->cmonth.append("\346\234\210"); /*   ��  */
		memset(szSQLBuff,0,MAX_PATH);
	}
	memset(buf,0,sizeof(buf));
	if(abs(ctc->month)<13)
		sprintf(buf, "%02d%02d", abs(ctc->month),ctc->day);
	else if(abs(ctc->month)==13)
		sprintf(buf, "%02d%02d", 1,ctc->day);
	if(ctc->month>0)
	{
		sprintf(szSQLBuff, "select VALUE from choliday where DATA='%s'",buf);
		ctc->choliday=ctcl_common_query("choliday",szSQLBuff);
    }
	return 1;
}



// /***************************************************************************
// *������bool InitConnection(QString Path)
// *���ܣ�connect to sqlite3 database
// *����ֵ��true ok,
// ***************************************************************************/
bool DDCTCalendar::InitConnection(const std::string& strDBPath)
{
	TSTRACEAUTO();
	if (!Init())
	{
		return false;
	}
	int iRet = m_pfn_sqlite3_open_v2(strDBPath.c_str(),&m_db,SQLITE_OPEN_READONLY,NULL);
	if (SQLITE_OK != iRet)
	{
		TSDEBUG(_T("open database failed,error code is %lu"),iRet);
		return false;
	}

	if (SQLITE_OK != execDML("select * from lunar where MaxYear=2100"))
	{
		TSDEBUG(_T("select * from lunar where MaxYear=2100 error "));
		return false;
	}
	int nCnt = m_vQuery.size();
	if (nCnt<=0)
	{
		return false;
	}
	std::string strMaxYear = m_vQuery[nCnt-1].find("MaxYear")->second;
	if (strMaxYear.empty())
	{
		return false;
	}
    maxyear=atoi(strMaxYear.c_str());
    lunarstr=m_vQuery[nCnt-1].find("DATA")->second; 
	free();
	TSDEBUG(_T("Connection calendar success"));
    return true;
}

// /***************************************************************************
// *������void MyCTCalendar::logSqlError(  const QString& Type, const QString& error)
// *���ܣ�log error message to a log file
// *����ֵ��none
// ***************************************************************************/
void DDCTCalendar::logSqlError(const std::string& strType, const std::string& error)
{

}

std::string DDCTCalendar::ctcl_common_query(std::string type,const char* szSQL)
{
	std::string strUTF8 = "";
	if (SQLITE_OK != execDML(szSQL))
	{
		return strUTF8;
	}
	int nCnt = m_vQuery.size();
	if (nCnt<=0)
	{
		return strUTF8;
	}

	strUTF8=m_vQuery[nCnt-1].find("VALUE")->second; 
	std::wstring str;
	free();
    return strUTF8;
}
