#include "stdafx.h"
#include "boost/regex.hpp"

#include "FilterManager.h"
#include <fstream>
#include "ADBFilter.h"
#include "Lock.h"

struct FilterRuleList {
	FilterRule * r;
	FilterRuleList * next;
    inline bool doFilter(const Url & mainURL,const Url & url, FilterType t) {
		FilterRuleList * l = this;
		while (l) {
            if (l->r->shouldFilter(mainURL,url,t))
				return true;
			l = l->next;
		}
		return false;
	}
};


struct HideRuleList {
	HideRule * r;
	HideRuleList * next;
};


struct ReplaceRuleList {
	ReplaceRule * r;
	ReplaceRuleList * next;
	inline void getRelpace(const Url & url,std::vector<std::string> &v_Parse) {
		ReplaceRuleList * l = this;
		while (l) {
			if (l->r->shouldReplace(url))
				v_Parse.push_back(l->r->getReplace());
			l = l->next;
		}
		return ;
	}
};



bool FilterManager::FilterRuleMap::doFilter(const Url & mainURL,const std::string &key, const Url & url,FilterType t) {
    FilterRuleMap::iterator mapit=find(key);
	if (mapit != end()) {
		//qDebug()<<"finds key:"<<key;
		unsigned int address = (unsigned int) mapit->second;
		if (unMatchRules.find(address) != unMatchRules.end()) // already match this rules;
			return false;
		this->unMatchRules.insert(address);  //add(address)
		if (mapit->second->doFilter(mainURL, url, t))
			return true;
	}
	//qDebug()<<"not find key:"<<key;
	return false;
}

FilterManager::FilterRuleMap::~FilterRuleMap() {
	for (FilterRuleMap::iterator it = this->begin(); it != this->end(); ++it) {
		FilterRuleList * l = it->second;
		FilterRuleList * n = l->next;
		while (n) {
			FilterRuleList * temp = n->next;
			delete n;
			n = temp;
		}
		delete l;
	}
}


void FilterManager::ReplaceRuleMap::getReplace(const std::string &key,const Url & url,std::vector<std::string> &v_Parse) {
	ReplaceRuleMap::iterator mapit=find(key);
	if (mapit != end()) {
		unsigned int address = (unsigned int) mapit->second;
		if (unMatchRules.find(address) != unMatchRules.end()) // already match this rules;
			return ;
		this->unMatchRules.insert(address);  //add(address)
		mapit->second->getRelpace(url,v_Parse);
	}
	return ;
}

FilterManager::ReplaceRuleMap::~ReplaceRuleMap() {
	for (ReplaceRuleMap::iterator it = this->begin(); it != this->end(); ++it) {
		ReplaceRuleList * l = it->second;
		ReplaceRuleList * n = l->next;
		while (n) {
			ReplaceRuleList * temp = n->next;
			delete n;
			n = temp;
		}
		delete l;
	}
}


FilterManager::FilterManager(const std::wstring & filename) {
	ifstream infile(filename.c_str());
	if (!infile)
	{ 
		//打开失败
	}
	else {
		char rule[1000];
		infile.getline(rule, 1000); //忽略第一行版本号
		while (!infile.eof())
		{
			memset(rule, 0, 1000);
			infile.getline(rule, 1000);
			if (rule[0] == '!')
			{
				//行注释
			}
			else {
				int n = strlen(rule);
				if (rule[n - 1] == '\n' || rule[n - 1] == '\r') {
					rule[n - 1] = '\0';
				}
				if (rule[n - 2] == '\n' || rule[n - 2] == '\r') {
					rule[n - 2] = '\0';
				} //clear \n \r
				if (strlen(rule) <= 3) //too short rule ,maybe some error
					continue;
				std::string s(rule);
				 boost::regex expression(std::string(".*\\$s@.*@.*@$"));
                //qDebug()<<"add rule:"<<s;
				if (boost::icontains(s,"##")) {
					HideRule hr(s);
					this->addRule(&hr);
				} 
				else if (boost::regex_match(s, expression)){
					this->addRule(new ReplaceRule(s));
				}
				else {
					this->addRule(new FilterRule(s));
				}
			}
		}
	}
}

FilterManager::FilterManager(const StringVector & rules)
{
	for(int i=0;i<rules.size();i++)
	{
		std::string r=rules[i];
		if(r[0]=='!') 
		{
			continue;
		}
		if(boost::icontains(r,"##"))
		{
            //HideRule rule(r);
            //rule.print();
		}
		else 
		{
            this->addRule(new FilterRule(r));
		}
	}

}

FilterManager::~FilterManager()
{
	for (FilterRuleVector::iterator it=m_AllFilterRules.begin();
			it!=m_AllFilterRules.end();++it) {
		delete *it;
	}

	for (ReplaceRuleVector::iterator it=m_AllReplaceRules.begin();
		it!=m_AllReplaceRules.end();++it) {
			delete *it;
	}

//hiderule process
}

#define RULE_KEY_HASH_LENGTH 8
inline int abpmin(int f1,int f2)
{
    return f1<f2?f1:f2;
}

static bool collectShortcuts(const std::string & str, StringVector & vs) 
{
	static boost::unordered_set<std::string> commonFilters;
	if(commonFilters.empty()) {
#if RULE_KEY_HASH_LENGTH==7 // 7
		commonFilters.insert("http://");
		commonFilters.insert("ttp://w");
		commonFilters.insert("tp://ww");
		commonFilters.insert("p://www");
		commonFilters.insert("://www.");
#elif RULE_KEY_HASH_LENGTH==8 // 8
		commonFilters.insert("http://w");
		commonFilters.insert("ttp://ww");
		commonFilters.insert("tp://www");
		commonFilters.insert("p://www.");
#elif RULE_KEY_HASH_LENGTH==9 // 9
		commonFilters.insert("http://ww");
		commonFilters.insert("ttp://www");
		commonFilters.insert("tp://www.");
#endif

	}
	int i = 0;
	bool isFindShoutcut = false;
	while (i < abpmin(str.length() - RULE_KEY_HASH_LENGTH,80)) 
	{
		unsigned int j = i;
		for (; j < str.length(); j++) {
			if ((str[j] == '*' || str[j] == '^')) {
				break;
			}
		}
		for (unsigned int k = i; j - k >= RULE_KEY_HASH_LENGTH; k++) {
			std::string key = str.substr(k, RULE_KEY_HASH_LENGTH);
			if(commonFilters.find(key)!=commonFilters.end())
				continue;
			isFindShoutcut = true;
			vs.push_back(key); //append(key);
		}
		i = j + 1;
	}
	return isFindShoutcut;
}

void FilterManager::insertRuleToFilterRuleMap(FilterRuleMap * rules,const std::string & key, FilterRule * r)
{
	FilterRuleMap::iterator it = rules->find(key);
	if (it != rules->end()) { // already exists
		FilterRuleList * l = it->second;
		while (l->next)
			l = l->next;
		FilterRuleList * n = new FilterRuleList();
		n->r = r;
		n->next = NULL;
		l->next = n;
	} else {
		FilterRuleList * n = new FilterRuleList();
		n->r = r;
		n->next = NULL;
		rules->insert(FilterRuleMap::value_type(key, n)); //add(key, n)
	}
}

void FilterManager::addRule(FilterRule * r)
{
	this->m_AllFilterRules.push_back(r); //for delete FilterRule;
	FilterRuleMap * rules;
	FilterRuleVector * unshortcutRules;
	StringVector domains;
	r->getDomains(domains);
	if (r->isWhiteFilter()) {
		rules = domains.empty()?&m_ShortcutWhiteRules:&m_DomainWhiteRules;
		unshortcutRules = &m_UnshortcutWhiteRules;
	}
	else {
		rules = domains.empty() ? &m_ShortcutFilterRules : &m_DomainFilterRules;
		unshortcutRules = &m_UnshortcutFilterRules;
	}
	if(!domains.empty()) {
		for(int i=0;i<domains.size();i++) {
			insertRuleToFilterRuleMap(rules,domains[i],r);
		}
		return;
	}
	/*
	 * 没有域名信息才会进行8个字符8个字符的索引
	 */
	const std::string & reFilter = r->getRegularFilter();
	StringVector shortcuts;
	if (collectShortcuts(reFilter, shortcuts)) {
		unsigned int i = 0;
		for (; i < shortcuts.size(); i++) {
			insertRuleToFilterRuleMap(rules,shortcuts[i],r);
		}
	} else
		unshortcutRules->push_back(r);//append(r);
}

void FilterManager::addRule(HideRule * r) {
	const StringVector & domains=r->domains();
	const std::string & sel=r->selector();
	for(StringVector::const_iterator it=domains.begin();it!=domains.end();it++) 
	{
		const std::string & key=*it;
		HideRuleMap::iterator hit=this->m_hiderules.find(key);
		if(hit!=m_hiderules.end()) {
			hit->second=hit->second+","+sel;
		}
		else {
			m_hiderules.insert(HideRuleMap::value_type(*it, sel));
		}
	}
}

void FilterManager::insertRuleToFilterRuleMap(ReplaceRuleMap * rules,const std::string & key, ReplaceRule * r)
{
	ReplaceRuleMap::iterator it = rules->find(key);
	if (it != rules->end()) { // already exists
		ReplaceRuleList * l = it->second;
		while (l->next)
			l = l->next;
		ReplaceRuleList * n = new ReplaceRuleList();
		n->r = r;
		n->next = NULL;
		l->next = n;
	} else {
		ReplaceRuleList * n = new ReplaceRuleList();
		n->r = r;
		n->next = NULL;
		rules->insert(ReplaceRuleMap::value_type(key, n)); //add(key, n)
	}
}

void FilterManager::addRule(ReplaceRule * r)
{
	this->m_AllReplaceRules.push_back(r); 
	ReplaceRuleMap * rules = &m_DomainReplaceRules;

	std::string domain;
	r->getDomains(domain);
	if(!domain.empty()) {
			insertRuleToFilterRuleMap(rules,domain,r);
		return;
	}
}

bool FilterManager::addRule(std::string rule)
{
	//not implemented
	return true;
}

bool FilterManager::hideRule(int id)
{
	//not implemented
	return true;
}
// 0:弹框
// 1:过滤
// 2:不过滤
bool FilterManager::updateConfigVideoHost(const Url & url,int istate)
{
	FliterLib::CriticalSectionLockGuard lck(this->cs);
	std::string host = url.GetHost();
	ConfigRuleMap::iterator hit=this->m_ConfigVideoRules.find(host);
	if(hit!=m_ConfigVideoRules.end())
	{
		hit->second=istate;
	}
	else {
		m_ConfigVideoRules.insert(ConfigRuleMap::value_type(host, istate));
	}

	return true;
}

// true:添加到白名单
// false:从白名单中删除
bool FilterManager::updateConfigWhiteHost(const Url & url,bool bEnable)
{
	FliterLib::CriticalSectionLockGuard lck(this->cs);
	std::string host = url.GetHost();
	//ConfigRuleMap::iterator hit=this->m_ConfigWhileRules.find(host);
	if (bEnable)
	{
		ConfigRuleMap::iterator hit=this->m_ConfigWhiteRules.find(host);
		if(hit == m_ConfigWhiteRules.end())
		{
			m_ConfigWhiteRules.insert(ConfigRuleMap::value_type(host, 0));
		}
	}
	else
	{
		m_ConfigWhiteRules.erase(host);
	}
	return true;
}



bool FilterManager::checkConfigHost(const Url & mainURL, const Url & url,bool &bFind,int cfgFlag)
{
	std::string res;
	StringVector dotsplits;
	boost::split(dotsplits,url.GetString(),boost::is_any_of("."));
	if(dotsplits.size()<=1) //domain error? no dot? at least g.cn
		return false;
	int ignore=2;
	std::string l2domain=dotsplits[dotsplits.size()-2];
	if(l2domain =="com" ||
		l2domain=="net" ||
		l2domain=="org" ||
		l2domain=="edu")
		ignore=3;
	for(int i=dotsplits.size()-ignore;i>=0;i--) 
	{
		std::string domain=dotsplits[i];
		for(int j=i+1;j<dotsplits.size();j++) {
			domain=domain+"."+dotsplits[j];
		}
		if (cfgFlag & CFG_CHECK_VIDEO)
		{
			ConfigRuleMap::iterator it_video=this->m_ConfigVideoRules.find(domain);

			if(it_video!=m_ConfigVideoRules.end()) 
			{
				bFind = true;
				int istate = it_video->second;
				if (istate == 0)  //ask
				{
					HWND hNotifyWnd = ::FindWindow(L"{B239B46A-6EDA-4a49-8CEE-E57BB352F933}_dsmainmsg", NULL);
					if(hNotifyWnd != NULL) 
					{
						::PostMessage(hNotifyWnd, WM_USER + 202, WPARAM(1), LPARAM(url.GetString().c_str()));
					}

					return false;
				}
				else if (istate == 1)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
		if (cfgFlag & CFG_CHECK_WHITE)
		{
			ConfigRuleMap::iterator it_White=this->m_ConfigWhiteRules.find(domain);
			if(it_White!=m_ConfigWhiteRules.end()) 
			{
				bFind = true;
				return false;
			}

		}	
	}
	return false;
}

bool FilterManager::shouldFilterByDomain(const Url & mainURL,const Url & url,FilterType t,bool & isFind)
{
	StringVector dotsplits;
	this->m_DomainFilterRules.prepareStartFind();
	this->m_DomainWhiteRules.prepareStartFind();
	boost::split(dotsplits,url.GetHost(),boost::is_any_of("."));
	if (dotsplits.size() <= 1) //domain error? no dot? at least g.cn
		return false;
	for (int i = dotsplits.size() - 2; i >= 0; i--) {
		std::string domain = dotsplits[i];
		for (int j = i + 1; j < dotsplits.size(); j++) {
			domain = domain + "." + dotsplits[j];
		}
		isFind=m_DomainWhiteRules.doFilter(mainURL, domain, url, t);
		if (isFind)
			return false;
		isFind=m_DomainFilterRules.doFilter(mainURL, domain, url, t);
		if (isFind)
			return true;
	}
	return false;
}

bool FilterManager::shouldFilterByShortcut(const Url & mainURL,const Url & url,FilterType t)
{
	StringVector shortcuts;
	collectShortcuts(url.GetString(), shortcuts);
	this->m_ShortcutFilterRules.prepareStartFind();
	for (StringVector::iterator it = shortcuts.begin(); it != shortcuts.end(); ++it) {
		if (m_ShortcutWhiteRules.doFilter(mainURL, *it, url, t))
			return false;
	}
	for (FilterRuleVector::iterator it =
			this->m_UnshortcutWhiteRules.begin(); it
			!= this->m_UnshortcutWhiteRules.end(); ++it) {
		if ((*it)->shouldFilter(mainURL, url, t))
			return false;
	}
	for (StringVector::iterator it = shortcuts.begin(); it != shortcuts.end(); ++it) {
		if (this->m_ShortcutFilterRules.doFilter(mainURL, *it, url, t))
			return true;
	}
	for (FilterRuleVector::iterator it = this->m_UnshortcutFilterRules.begin(); it != this->m_UnshortcutFilterRules.end(); ++it) 
	{
		if ((*it)->shouldFilter(mainURL, url, t))
			return true;
	}
	return false;
}

bool FilterManager::shouldFilter(const Url & mainURL,const Url & url,FilterType t)
{
	FliterLib::CriticalSectionLockGuard lck(this->cs);

	bool bFind =false;
	bool bRet = checkConfigHost(mainURL,url,bFind);
	if (!bFind)
	{
		bRet = shouldFilterByDomain(mainURL,url,t,bFind);
		if(!bFind)
			bRet = shouldFilterByShortcut(mainURL,url,t);
	}
	return bRet;
}

std::string FilterManager::getcssRules(const Url & url)
{
	FliterLib::CriticalSectionLockGuard lck(this->cs);
	bool bFind =false;
	bool bRet = checkConfigHost("",url,bFind);
	if (bFind && !bRet)
	{
		return "";
	}
	
	std::string res;
	StringVector dotsplits;
	boost::split(dotsplits,url.GetHost(),boost::is_any_of("."));
	if(dotsplits.size()<=1) //domain error? no dot? at least g.cn
		return res;
	int ignore=2;
	std::string l2domain=dotsplits[dotsplits.size()-2];
	if(l2domain =="com" ||
			l2domain=="net" ||
			l2domain=="org" ||
			l2domain=="edu")
		ignore=3;
	for(int i=dotsplits.size()-ignore;i>=0;i--) {
		std::string domain=dotsplits[i];
		for(int j=i+1;j<dotsplits.size();j++) {
			domain=domain+"."+dotsplits[j];
		}
		HideRuleMap::iterator it=this->m_hiderules.find(domain);

		if(it!=m_hiderules.end()) {
			res=res.empty()?it->second:res+","+it->second;
		}
	}
	if(!res.empty()) {
		res+="{display:none !important;}";
	}
	return res;
}

void FilterManager::getreplaceRules(const Url & url,std::vector<std::string> & v_replace)
{
	FliterLib::CriticalSectionLockGuard lck(this->cs);

	bool bFind =false;
	bool bRet = checkConfigHost("",url,bFind);
	if (bFind && !bRet)
	{
		return;
	}
	StringVector dotsplits;
	this->m_DomainReplaceRules.prepareStartFind();
	
	int sizwe = m_DomainReplaceRules.size();
	boost::split(dotsplits,url.GetHost(),boost::is_any_of("."));
	if (dotsplits.size() >= 1) //domain error? no dot? at least g.cn
	{
		for (int i = dotsplits.size() - 2; i >= 0; i--) {
			std::string domain = dotsplits[i];
			for (int j = i + 1; j < dotsplits.size(); j++) {
				domain = domain + "." + dotsplits[j];
			}
			m_DomainReplaceRules.getReplace(domain,url,v_replace);
		}
	}
}

static FilterManager * m=NULL;

FilterManager * FilterManager::getManager()
{
	return m;
}
FilterManager * FilterManager::getManager(const std::wstring & filename)
{
	if(m)
		return m;
	m=new FilterManager(filename);
	return m;
}
