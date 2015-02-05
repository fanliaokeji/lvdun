#include "stdafx.h"

#include "ADBFilter.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

extern ConfigRuleMap map_VideoState;
extern rwmutex rw_cfgmutex;
static std::string parseDomain(std::string host)
{
	int spos=0;
	int endpos=-1;
	if (host[spos] == '*') {
		spos+=1;
	}
	if (host[spos] == '*') {
		spos += 1;
	}
	if (host.find("http://",spos)==spos) {
		spos+=7;
	}
	if(host.find("https://",spos)==spos) {
		spos+=8;
	}
	endpos=host.find('/',spos);
	if (endpos == std::string::npos)
	{
		endpos=host.length();
		if (host[endpos - 1] == '*' || host[endpos - 1] == '^')
			endpos -= 1;
		if (host[endpos - 1] == '*' || host[endpos - 1] == '^')
			endpos -= 1;
	}
	bool ignoreNextDot=false;
	//std::cout << host.c_str()<<std::endl;
	for(int i=spos;i<endpos;i++) 
	{

		if(ignoreNextDot && host[i]=='.') 
		{
			spos=i+1;
			ignoreNextDot=false;
		}
		if(host[i]=='*' || host[i]=='^' ||  host[i] == '?')
		{
			ignoreNextDot=true;
			spos=i+1;
		}
	}

	host=host.substr(spos,endpos-spos);
	if(host.length() < 1 || host[0]=='.' || host[host.length()-1]=='.' || host.find('.') == std::string::npos)
		host = "";
	return host;
}

//不弹过滤框，只有在VideoState里存在且等于2的才不弹
static bool isMatchState(std::vector<std::string> & v_domain)
{
	readLock rdLock(rw_cfgmutex);
	for (std::vector<std::string>::const_iterator c_iter = v_domain.begin();c_iter!= v_domain.end();c_iter++)
	{
		ConfigRuleMap::iterator map_iter =map_VideoState.find(*c_iter);
		if (map_iter != map_VideoState.end() && map_iter->second == 2)
		{
			return false;
		}
		
	}
	return true; 
}
/*
  首先manager已经判断过是过滤而不是隐藏规则了
  以@@开始，则是白名单，manager会优先考虑
  以||开始则是不匹配协议名的过滤，并去掉||
  以|开始，则去掉|，否则在开始处添加*
  含有$类型指定规则，去掉这些字符串，并处理类型
  以|结尾，去掉|，否则在结尾处添加*
  */

FilterRule::FilterRule( const std::string & r)
{
	std::string rule=boost::trim_copy(r);
    //m_rule=rule;
    this->m_isException=false;
    this->m_isMatchProtocol=true;
    m_filterThirdParty=false;
    m_type=0;
	m_iresponse = -1;
	boost::iterator_range<string::iterator> ran = boost::ifind_last(rule,"|#|");
	if (ran.begin() != rule.end())
	{
		std::string::size_type pos = ran.begin()-rule.begin();
		std::string striresponse = rule.substr(pos+3);
		if (!striresponse.empty())
		{
			m_iresponse = atol(striresponse.c_str());
		}
		
		rule = rule.substr(0,pos);
	}

	boost::iterator_range<string::iterator> range = boost::ifind_last(rule,"$$$");
	if (range.begin() != rule.end())
	{
		std::string::size_type pos = range.begin()-rule.begin();
		std::string strState = rule.substr(pos+3);
		boost::split(m_stateDomains,strState,boost::is_any_of("|"));
		rule = rule.substr(0,pos);
	}

	m_rule=rule;
	if(boost::istarts_with(rule,"@@")) {
        this->m_isException=true;
		boost::erase_first(rule, "@@");
	}
	if(boost::istarts_with(rule,"||")) {
        this->m_isMatchProtocol=false;
		boost::erase_first(rule,"||");
	}
	if(boost::istarts_with(rule,"|"))
		boost::erase_first(rule,"|");
	else
		rule.insert(0,"*");
	int dollarPos=rule.find_last_of('$');
	if(dollarPos!= std::string::npos) {
		//const UChar* types = rule.substring(dollarPos).charactersWithNullTermination();
		std::string token;
		for (UINT i = dollarPos+1; i<rule.length(); i++) {
			if (rule[i] != ',' && i != rule.length()-1) {
				token += rule[i];
			} else {
				if(i==rule.length()-1)
					token += rule[i];
                bool dor=false;
                if(token[0]=='~') {
                    token=token.substr(1);
                    dor=true;
                }
				if (boost::iequals(token,"script"))
                    m_type |= dor?~FILTER_TYPE_SCRIPT:FILTER_TYPE_SCRIPT;
				else if (boost::iequals(token,"image"))
                    m_type |= dor?~FILTER_TYPE_IMAGE:FILTER_TYPE_IMAGE;
				else if (boost::iequals(token,"background"))
                    m_type |= dor?~FILTER_TYPE_BACKGROUND:FILTER_TYPE_BACKGROUND ;
				else if (boost::iequals(token,"object"))
                    m_type |= dor?~FILTER_TYPE_OBJECT:FILTER_TYPE_OBJECT ;
				else if (boost::iequals(token,"stylesheet"))
                    m_type |= dor?~FILTER_TYPE_STYLESHEET:FILTER_TYPE_STYLESHEET ;
				else if (boost::iequals(token,"xbl"))
                    m_type |= dor?~FILTER_TYPE_XBL:FILTER_TYPE_XBL ;
				else if (boost::iequals(token,"ping"))
                    m_type |= dor?~FILTER_TYPE_PING:FILTER_TYPE_PING ;
				else if (boost::iequals(token,"xmlhttprequest"))
                    m_type |= dor?~FILTER_TYPE_XMLHTTPREQUEST:FILTER_TYPE_XMLHTTPREQUEST ;
				else if (boost::iequals(token,"object_subrequest"))
                    m_type |= dor?~FILTER_TYPE_OBJECT_SUBREQUEST:FILTER_TYPE_OBJECT_SUBREQUEST ;
				else if (boost::iequals(token,"dtd"))
                    m_type |= dor?~FILTER_TYPE_DTD:FILTER_TYPE_DTD ;
				else if (boost::iequals(token,"subdocument"))
                    m_type |= dor?~FILTER_TYPE_SUBDOCUMENT:FILTER_TYPE_SUBDOCUMENT ;
				else if (boost::iequals(token,"document"))
                    m_type |= dor?~FILTER_TYPE_DOCUMENT:FILTER_TYPE_DOCUMENT ;
				else if (boost::iequals(token,"elemhide"))
                    m_type |= dor?~FILTER_TYPE_ELEMHIDE:FILTER_TYPE_ELEMHIDE ;
                else if (boost::iequals(token,"third-party")){
                    this->m_filterThirdParty=true;
                    this->m_matchFirstParty=dor;
                }
				else if (boost::istarts_with(token,"domain")) {
                    //type |= FILTER_TYPE_DOMAIN;
                    token=token.substr(7);
                    this->processDomains(token);
                }
				token="";
			}
		}
		rule=rule.substr(0,dollarPos);
	}
	if (rule[rule.length() - 1] == '|')
		rule = rule.substr(0,rule.length() - 1);
	else
		rule.append("*");
    this->m_reFilter=rule;


}
void FilterRule::print()
{

}

/*
 * 获取该规则期望过滤的域名
 */
void FilterRule::getDomains(StringVector & domains)
{
	if(!m_domains.empty())
	{
		domains.insert(domains.end(),m_domains.begin(),m_domains.end());
		return;
	}
	std::string s=parseDomain(m_reFilter);
	if(!s.empty())
		domains.push_back(s);
}
void FilterRule::processDomains(std::string &ds)
{
 	std::string token;
	for(UINT  i=0;i<ds.length();i++) {
		if (ds[i] != '|' && i != ds.length()-1) {
			token += ds[i];
		} else {
			if(i==ds.length()-1)
				token += ds[i];
			if(token[0]=='~')
				this->m_inverseDomains.push_back(token.substr(1));
			else
				this->m_domains.push_back(token);
			token="";
		}
	}
}

bool FilterRule::isMatchThirdParty(const Url &mainURL, const Url &other)
{
    if(!this->m_filterThirdParty)
        return true;
    if(mainURL.GetString().empty())
        return true;

	return boost::iends_with(other.GetHost(),mainURL.GetHost()) == this->m_matchFirstParty;
}


bool FilterRule::isMatchDomains(const Url &url)
{
    //String h=url.host().lower();
	std::string h = url.GetHost();
    if(m_domains.empty() && m_inverseDomains.empty()) //not specify which domains;
        return true;
    for(StringVector::iterator it=this->m_domains.begin();it!=m_domains.end();++it) {
        if(boost::iends_with(h,*it))
            return true;
    }
    if(m_inverseDomains.empty()) //no inverse domains,so this filter rule should not apply
        return false;
    for(StringVector::iterator it=this->m_inverseDomains.begin();
    it!=m_inverseDomains.end();++it) {
        if(boost::iends_with(h,*it))
            return false;
    }
    return true;
}

/*
 when t is 0, the caller wants that the FilterRule decide the url type itself

 */
bool FilterRule::isMatchType(const Url &u, FilterType t)
{
    if(m_type==0) {
        return true;
    }
    if(t!=0) 
	{
		return t&m_type?true:false;
    }
	std::string url=u.GetString();
    t=m_type;
    t&=~FILTER_TYPE_SCRIPT;
    if(m_type&FILTER_TYPE_SCRIPT && boost::iends_with(url,".js")) {
        return true;
    }
    t&=~FILTER_TYPE_IMAGE;
    if(m_type&FILTER_TYPE_IMAGE) {
        if(boost::iends_with(url,".png")
            ||boost::iends_with(url,".jpg")
            ||boost::iends_with(url,".gif")
            ||boost::iends_with(url,".bmp")
            ||boost::iends_with(url,".jpeg")
            ||boost::iends_with(url,".ico"))
            return true;
    }
    t&=~FILTER_TYPE_STYLESHEET;
    if(m_type&FILTER_TYPE_STYLESHEET && boost::iends_with(url,".css")) {
        return true;
    }
    t&=~FILTER_TYPE_OBJECT;
    if(m_type & FILTER_TYPE_OBJECT && boost::iends_with(url,".swf")) { //filter flash
    	return true;
    }
    return false;
}

static bool adbMatch(const char * s,  const char *  p,bool caseSensitivie=false);


int FilterRule::shouldFilter(const Url & mainURL,const Url & u,FilterType t)
{
	if (!m_stateDomains.empty() && !isMatchState(m_stateDomains))
	{
		return 0;
	}
	std::string url=u.GetString();
	bool ret;
    if(!this->m_isMatchProtocol) {
		//url=url.substr(url.length()-u.GetScheme().length());
		if (!u.GetScheme().empty())
		{
			url=url.substr(u.GetScheme().length()+3);
		}
	}
    if(!this->isMatchThirdParty(mainURL,u)) {
        return 0;
    }
    if(!this->isMatchDomains(u) || !this->isMatchDomains(mainURL)) {
        return 0;
    }
    if(!isMatchType(u,t)) {
        return 0;
    }
    ret=adbMatch(url.c_str(),m_reFilter.c_str());
	if (ret)
	{
		if (m_iresponse > 0 && m_iresponse<1000)
		{
			return m_iresponse;  
		}
		return 1;
	}
	return 0;
}

///////////////////////////////////////////////////////////////////


/*
 * 	if getCaseChar is noncasesensitive ,the result will be noncasesentive
 *
 */
inline char getCaseChar( char c,bool caseSensitive)
{
    if(caseSensitive)
        return c;
    if(c>='A' && c<='Z')
        c='a'+c-'A';
    return c;
}
/*
  Separator character is anything but a letter,
a digit, or one of the following: – . %.
  */
static int  isSeperator( char ch)
{
    if(ch>='a' && ch <='z')
        return 0;
    if(ch>='A' && ch<='Z')
        return 0;
	if(ch>='0' && ch<='9')
		return 0;
    if(ch=='-' || ch=='.' || ch=='%' || ch=='_')
        return 0;
    return 1;
}
inline int abpmax(int f1,int f2)
{
    return f1>f2?f1:f2;
}
inline int abpmin(int f1,int f2)
{
    return f1<f2?f1:f2;
}

/*
find t in s and return a pointer.
if found ,return the first place t appears in s
else NULL;
*/
static const char* strfind(  const char* s,  const char* t)
{
    const char* bp;
    const char* sp;
    while (*s)
    {
        bp=s;
        sp=t;
        do
        {
            if(!*sp)
                return s;

        }
        while(*bp++==*sp++);
        s++;
    }
    return NULL;
}
/*
  get a much bigger step for *.
  */
static inline int  patternStep( const char * s, const  char * p)
{
    char temp[8];
    int step=0;
    const char * t=p;
    while(*t!='*' && *t!='^' && *t!='\0')
    {
        step++;
        t++;
    }
    if(!step) //just one character ,such as ^,*
        return 1;
    memset(temp,0,sizeof(temp));
    strncpy_s(temp,p,abpmin(sizeof(temp)-1,step));
    //printf("temp=%s,step=%d\n",temp,step);
    const char * res=strfind(s,temp);
    if(!res) //没有找到
        return strlen(s); //移动整个字符串
    else
        return abpmax(1,res-s); //找到第一个匹配的字符串的位置
}
/*
   test if a given string  and a pattern  matches use adblock plus rule
   give a string s and a pattern p ,
   if they match,return 1, then return 0
   */
static bool adbMatch(const char *  s,  const char *  p,bool caseSensitivie) {
    for (;;) {
        switch(*p++) {
            case '*' :	// match 0-n of any characters
                if (!*p) return true; // do trailing * quickly
                while (!adbMatch(s, p,caseSensitivie))
                {
#if 0
                     if (!*s++) return 0;
#else
                    if(!*s) return false;
                    s+=patternStep(s,p);
#endif
                }
                return true;
            case '^':
                if(isSeperator(*s))
                {
                    s++;
                    break;
                }
                else
                    return false;//expect a sepetor,
            case '\0':	// end of pattern
                return !*s;
			case '?':
				s++;
				break;
            default  :
                if (getCaseChar(*s++,caseSensitivie) !=
                    getCaseChar(*(p-1),caseSensitivie)) return false;
                break;
        }
    }
}

HideRule::HideRule(const std::string & r)
{
	std::string rule=boost::trim_copy(r);
	int pos=rule.find("##",0);
	std::string temp;
	for(int i=0;i<pos;i++) {
		if(rule[i]==',') {
			if(!temp.empty()) {
				m_domains.push_back(temp);
				temp="";
			}
			continue;
		}
		temp += rule[i];
		if(i==pos-1)
			m_domains.push_back(temp);
	}
	m_sel=rule.substr(pos+2);
}

void HideRule::print()
{

}

ReplaceRule::ReplaceRule(const std::string & r)
{

	std::string rule=boost::trim_copy(r);
	//m_rule=rule;

	boost::iterator_range<string::iterator> range = boost::ifind_last(rule,"$$$");
	if (range.begin() != rule.end())
	{
		std::string::size_type pos = range.begin()-rule.begin();
		std::string strState = rule.substr(pos+3);
		boost::split(m_stateDomains,strState,boost::is_any_of("|"));
		rule = rule.substr(0,pos);
	}
	m_rule=rule;
	this->m_isMatchProtocol=true;

	if(boost::istarts_with(rule,"||")) {
		this->m_isMatchProtocol=false;
		boost::erase_first(rule,"||");
	}
	if(boost::istarts_with(rule,"|"))
		boost::erase_first(rule,"|");
	else
		rule.insert(0,"*");
	int dollarPos=rule.find_last_of('$');
	if(dollarPos!= std::string::npos) {
		std::string token;
		rule=rule.substr(0,dollarPos);
		this->m_reParse=m_rule.substr(dollarPos+1);
	}
	if (rule[rule.length() - 1] == '|')
		rule = rule.substr(0,rule.length() - 1);
	else
		rule.append("*");
	this->m_reReplace=rule;
}

bool ReplaceRule::shouldReplace(const Url & u)
{
	if (!m_stateDomains.empty() && !isMatchState(m_stateDomains))
	{
		return false;
	}

	std::string url=u.GetString();
	bool ret;
	if(!this->m_isMatchProtocol) 
	{
		//url=url.substr(url.length()-u.GetScheme().length());
		if (!u.GetScheme().empty())
		{
			url=url.substr(u.GetScheme().length()+3);
		}
	}
	ret=adbMatch(url.c_str(),m_reReplace.c_str());
	return ret;
}

void ReplaceRule::getDomains(std::string &domain)
{
	std::string s=parseDomain(m_reReplace);
	if(!s.empty())
		domain = s;
}

RedirectRule::RedirectRule(const std::string & r)
{
	std::string rule=boost::trim_copy(r);
	if(boost::istarts_with(rule,"@#@")) {
		boost::erase_first(rule,"@#@");
	}

	boost::iterator_range<string::iterator> range = boost::ifind_last(rule,"$$$");
	if (range.begin() != rule.end())
	{
		std::string::size_type pos = range.begin()-rule.begin();
		rule = rule.substr(0,pos);
	}
	m_rule=rule;
	this->m_isMatchProtocol=true;
	if(boost::istarts_with(rule,"||")) {
		this->m_isMatchProtocol=false;
		boost::erase_first(rule,"||");
	}
	if(boost::istarts_with(rule,"|"))
		boost::erase_first(rule,"|");
	else
		rule.insert(0,"*");
	int dollarPos=rule.find_last_of('$');
	if(dollarPos!= std::string::npos) {
		std::string token;
		rule=rule.substr(0,dollarPos);
		this->m_reParse=m_rule.substr(dollarPos+2);
	}
	range = boost::ifind_first(rule,"$content=");
	if(range.begin() != rule.end()) {
		std::string::size_type contentPos = range.begin()-rule.begin();
		std::string token=rule.substr(contentPos+9);
		rule=rule.substr(0,contentPos);
		if (!token.empty())
		{
			if (token[0] == '~')
			{
				token = token.substr(1);
				boost::split(m_vwhiteContent,token,boost::is_any_of("|"));
			}
			else
			{
				boost::split(m_vblackContent,token,boost::is_any_of("|"));
			}
			
		}
		
	}

	if (rule[rule.length() - 1] == '|')
		rule = rule.substr(0,rule.length() - 1);
	else
		rule.append("*");
	this->m_reRedirect=rule;
}

static bool isMatchContent(std::vector<std::string> & v_content,std::string url)
{
	for (std::vector<std::string>::const_iterator c_iter = v_content.begin();c_iter!= v_content.end();c_iter++)
	{
		if (url.find((*c_iter).c_str()) != std::string::npos)
		{
			return true;
		}
		
	}
	return false; 
}

bool RedirectRule::shouldRedirect(const Url & u)
{
	std::string url=u.GetString();
	if (m_reParse.empty() || url.find(m_reParse.c_str()) != std::string::npos)
	{
		return false;
	}

	if(!this->m_isMatchProtocol) 
	{
		//url=url.substr(url.length()-u.GetScheme().length());
		if (!u.GetScheme().empty())
		{
			url=url.substr(u.GetScheme().length()+3);
		}
	}
	if (!m_vwhiteContent.empty() && isMatchContent(m_vwhiteContent,url))
	{
		return false;
	}
	
	if (!m_vblackContent.empty() && !isMatchContent(m_vblackContent,url))
	{
		return false;
	}
	
	bool ret = adbMatch(url.c_str(),m_reRedirect.c_str());
	if (ret)
	{
		m_rule;
	}
	
	return ret;
}

void RedirectRule::getDomains(std::string &domain)
{
	std::string s=parseDomain(m_reRedirect);
	if(!s.empty())
		domain = s;
}