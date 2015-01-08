// ***************************************************************************
// *	本代码参考了中国中国农历函数库
// ***************************************************************************
#include "stdafx.h"
#include<string.h>
//#include "sqlite3.h"

#include "TCalendar.h"
const int beginning_year=-849;  //记录从公元前850年开始
const int baseIndex = 0;

//
//农历保存在"lunar.db"的sqlite3数据库中，方便扩充和移植
//

DDCTCalendar::DDCTCalendar(void)
{
}

DDCTCalendar::~DDCTCalendar(void)
{
	
}

// /***************************************************************************
// *函数：int ctcl_calendar_type(int y,int m,int d,int opt);
// *功能：判断Gregorian(格里)历还是Julian(儒略)历
// *参数：阳历y年m月(1,2,..,12,下同)d日,opt=1,2,3分别表示标准日历,Gregorge历和Julian历
// *返回值：1(格里历)，0(儒略历)或-1(不存在)
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
      return(-1);  /*  这十天在历史上不存在  */
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
// *函数：int ctcl_date_to_days(int y,int m,int d);
// *功能：返回y年m月d日在y年年内走过的天数
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
      days=-1;  /*  无效  */
  }
  return days;
}

// /***************************************************************************
// *函数 int ctcl_days_to_date(int y,int x);
// *功能：返回阳历y年日差天数为x时所对应的月日数
// *     （如y=2000，x=274时，返回1001(表示10月1日，即返回100*m+d)）
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
// *函数：int day_from_era(int y);
// *功能：返回y年的年差天数（y年1月1日距相应历种的1年1月1日的天数）
// ***************************************************************************/

int DDCTCalendar::days_from_era(int y)
{
  int days=(y-1)*365 + (y-1)/4;  /*  Julian的年差天数  */
  if(y>1582)
    days+= -((y-1)/100) + (y-1)/400;  /*  Gregorian的年差天数  */
  return days;
}

// /***************************************************************************
// *函数：int ctcl_standard_days(int y,int m,int d);
// *功能：返回等效标准天数
// *     （y年m月d日相应历种的1年1月1日的等效(即对Gregorian历与Julian历
// *       是统一的)天数）
// ***************************************************************************/

int DDCTCalendar::ctcl_standard_days(int y,int m,int d)
{
  int days=(y-1)*365 + (y-1)/4 + ctcl_date_to_days(y,m,d) - 2;  /*  Julian的等效标准天数  */
  if(y>1582)
    days+= -((y-1)/100) + ((y-1)/400) + 2;  /*  Gregorian的等效标准天数  */
  return days;
}

// /***************************************************************************
// *函数：double ctcl_julian_day(int y,int m,int d,int h,int min,int sec,int zone);
// *功能：返回儒略日（zone时区y年m月d日h时min分sec秒距儒略历公元前4713年
// *      1月1日格林尼治时间正午12时的天数）
// ***************************************************************************/

double DDCTCalendar::ctcl_julian_day(int y,int m,int d,int h,int min,int sec,int zone)
{
  int type=ctcl_calendar_type(y,m,d,1);
  double jt=(h+(min+sec/60.0)/60.0)/24.0-0.5-zone/24.0;
  double jd=(type!=0) ? (ctcl_standard_days(y,m,d)+1721425+jt) : 
    (ctcl_standard_days(y,m,d)+1721425+jt);/*  儒略日  */
  return jd;
}

// /***************************************************************************
// *函数：int ctcl_day_of_week(int y,int m,int d);
// *功能：返回y年m月d日的星期值，0为星期日
// ***************************************************************************/
int DDCTCalendar::ctcl_day_of_week(int y,int m,int d)
{
  return ctcl_standard_days(y,m,d)%7;
}



// /***************************************************************************
// *函数：double ctcl_solar_term(int y,int n,int t);
// *功能：返回y年第n个节气（小寒为1）的日差天数,t取值为0或1,分别表示平气和定气
// ***************************************************************************/

double DDCTCalendar::ctcl_solar_term(int y,int n,int t)
{
  double jd = y * (365.2423112 - 6.4e-14*(y-100)*(y-100) - 3.047e-8*(y-100))
    + 15.218427 * n + 1721050.71301;/*  儒略日  */
  double zeta = 3.0e-4 * y - 0.372781384 - 0.2617913325 * n;/*  角度  */
  double yd = ( 1.945 * sin(zeta) - 0.01206 * sin(2*zeta) ) 
    * ( 1.048994 - 2.583e-5 *y );/*  年差实均数  */
  double sd =-18e-4*sin(2.313908653*y-0.439822951-3.0443*n);/*  朔差实均数  */
  return (t==1)?(jd+yd+sd-ctcl_standard_days(y,1,0)-1721425)
    :(jd-ctcl_standard_days(y,1,0)-1721425);
}

// /****************************************************************************
// *函数：double ctcl_tail(double x);
// *功能：求x的小数部分
// ***************************************************************************/

double DDCTCalendar::ctcl_tail(double x)
{
  return x-floor(x);
}
// /****************************************************************************
// *函数：double ctcl_rem(double x,double w);
// *功能：广义求余
// ***************************************************************************/

double DDCTCalendar::ctcl_rem(double x,double w)
{
  return ctcl_tail(x/w)*w;
}

// /****************************************************************************
// *函数：double ctcl_round(double x);
// *功能：四舍五入
// ***************************************************************************/

double DDCTCalendar::ctcl_round(double x){
  return floor(x+0.5);
}

// /***************************************************************************
// *函数：int ctcl_gan(int x);
// *功能：返回甲子数x对应的天干数（如33为3）
// ***************************************************************************/

int DDCTCalendar::ctcl_gan(int x)
{
  return x%10;
}

// /***************************************************************************
// *函数：int ctcl_zhi(int x);
// *功能：返回甲子数x对应的地支数（如33为9）
// ***************************************************************************/

int DDCTCalendar::ctcl_zhi(int x)
{
  return x%12;
}

// /***************************************************************************
// *函数：int ctcl_year_ganzhi(int y,int m,int d,int h);
// *功能：返回y年m月d日h时的年干支数（1-60）
// ***************************************************************************/

int DDCTCalendar::ctcl_year_ganzhi(int y,int m,int d,int h)
{
  if ( (ctcl_date_to_days(y,m,d)+h/24.0) 
       < ctcl_solar_term(y,3,1)-1.0 )/*  判断是否过立春  */
    y-=1;
  return (int)ctcl_round( ctcl_rem(y-3.0,60.0) ); 
}
// ***************************************************************************
// **TODO:ctcl_month_ganzhi, ctcl_day_ganzhi, ctcl_hour_ganzhi
// ***************************************************************************

//角度函数(私有)
double DDCTCalendar::ang(double x,double t,double c1,double t0,double t2,double t3)
{
  return ctcl_tail(c1*x)*2*M_PI+t0-t2*t*t-t3*t*t*t;
}

// /****************************************************************************
// *函数：int ctcl_lunar_date(int y,int m,int d);
// *功能：返回农历日数
// ***************************************************************************/

int DDCTCalendar::ctcl_lunar_date(int y,int m,int d)
{
  int lunar_date=-1;
  const double rpi=180/M_PI;
  const double zone=8.0;  /*  时区:东八区  */
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
  for(i=-1;i<=13;i+=1){  /*  k=整数为朔,k=半整数为望  */
    double aa=aa0+0.507984293*i;
    double ab=ab0+6.73377553*i;
    double ac=ac0+6.818486628*i;
    double f1=f0+1.53058868*i+fc*sin(aa)-0.4068*sin(ab)+0.0021*sin(2*aa)
      +0.0161*sin(2*ab)+0.0104*sin(2*ac)-0.0074*sin(aa-ab)-0.0051*sin(aa+ab);
    double j=j0+28*i+f1;  /*  朔或望的等效标准天数及时刻  */
    int diff=ctcl_standard_days(y,m,d)-floor(j);  /*  当前日距朔日的差值  */
    if( diff>=0 && diff<=29 )
      lunar_date=diff+1;
  } 
  return(lunar_date);
}

// /****************************************************************************
// *函数：int ctcl_leap_month(int y);
// *功能：返回y年的闰月，无闰返回0
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
// *函数：int ctcl_lunar_month(int y,int m,int d);
// *功能：返回y年m月d日对应的农历月份，闰月用负数表示
// ***************************************************************************/

int DDCTCalendar::ctcl_lunar_month(int y,int m,int d)
{
  double lunar_date=ctcl_lunar_date(y,m,d);
  int lunar_days=lunar_date-floor(lunar_date/100)*100;  /*  农历日数  */
  int leap_num=0;  /*  从当年到-849年的总闰月数  */
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
  /*  从当年到-849年的有效总月数(扣除闰月)  */
  
  if(y<=240) non_leap++;
  if(y<=237) non_leap--;
  if(y<24) non_leap++;
  if(y<9) non_leap--;
  if(y<=-255) non_leap++;   
  if(y<=-256) non_leap+=2;
  if(y<=-722) non_leap++;  /*  历史上的修改月建  */

  lunar_month=ctcl_round(ctcl_rem(non_leap-3.0,12.0)+1.0);
  if(lunar_month==ctcl_leap_month(y-1) && m==1 && d<lunar_days)
    lunar_month*=-1;    /*  如果y-1年末是闰月且该月接到了y年,则y年年初也是闰月  */
  else if(lunar_month==ctcl_leap_month(y))
    lunar_month*=-1;
  else if(lunar_month<ctcl_leap_month(y) || ( m<lunar_month && ctcl_leap_month(y) ) )
    lunar_month++;  /*  如果y年是闰月但当月未过闰月则前面多扣除了本年的闰月，这里应当补偿  */
  else lunar_month=ctcl_round(ctcl_rem(lunar_month-1,12)+1);

  return lunar_month;  
}

// /***************************************************************************
// *函数：int ctcl_solar_to_lunar(int y,int m,int d,struct CTCalendar* ctc);
// *功能：把公历y年m月d日转换为农历存放在ctc中
// *返回值：-1：输入日期不合法，0：农历无数据，1：一切正常
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
		ctc->cday = "\345\210\235";/*  初  */
		ctc->cday += ctcl_common_query("day",szSQLBuff);
		memset(szSQLBuff,0,MAX_PATH);
	}
	else if(ctc->day<20)
	{
		sprintf(szSQLBuff, "select VALUE from cnumber where ID=%d",ctc->day%10);
		ctc->cday = "\345\215\201";/*  十  */
		ctc->cday += ctcl_common_query("day",szSQLBuff);
		memset(szSQLBuff,0,MAX_PATH);
	}
	else if(ctc->day==20)
	{
		ctc->cday = "\344\272\214\345\215\201";/*  二十  */
	}
	else if(ctc->day<30)
	{
		sprintf(szSQLBuff, "select VALUE from cnumber where ID=%d",ctc->day%10);
		ctc->cday = "\345\273\277";/*  廿  */
		ctc->cday += ctcl_common_query("day",szSQLBuff);
		memset(szSQLBuff,0,MAX_PATH);
	}
	else if(ctc->day==30)
	{
		ctc->cday = "\344\270\211\345\215\201";/*  三十  */
	}

	ctc->month=ctcl_lunar_month(y,m,d);
	if(ctc->month==-12)
	{
		ctc->cmonth = "\351\227\260\345\215\201\344\272\214\346\234\210";/*  "闰十二月"  */
	}
	else if(ctc->month==-11)
	{
		ctc->cmonth = "\351\227\260\345\215\201\344\270\200\346\234\210";/*  "闰十一月"  */
	}
	else if(ctc->month==-1)
	{
		ctc->cmonth = "\351\227\260\346\255\243\346\234\210"; /*  "闰正月"  */
	}
	else if(ctc->month<0)
	{
		ctc->cmonth = "\351\227\260"; /*  "闰"  */
		sprintf(szSQLBuff, "select VALUE from cnumber where ID=%d",-(ctc->month));
		ctc->cmonth.append(ctcl_common_query("month",szSQLBuff));
		ctc->cmonth.append("\346\234\210"); /*   月  */
		memset(szSQLBuff,0,MAX_PATH);
	}
	else if(ctc->month==13)
	{
		ctc->cmonth = "\346\255\243\346\234\210"; /*  "正月"  */
	}
	else
	{
		sprintf(szSQLBuff, "select VALUE from cnumber where ID=%d",ctc->month);
		ctc->cmonth = ctcl_common_query("month",szSQLBuff);
		ctc->cmonth.append("\346\234\210"); /*   月  */
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
// *函数：bool InitConnection(QString Path)
// *功能：connect to sqlite3 database
// *返回值：true ok,
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
// *函数：void MyCTCalendar::logSqlError(  const QString& Type, const QString& error)
// *功能：log error message to a log file
// *返回值：none
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
