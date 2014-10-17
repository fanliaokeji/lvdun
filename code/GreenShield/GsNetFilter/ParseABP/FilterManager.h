#ifndef FILTERMANAGER_H
#define FILTERMANAGER_H
#include <vector>
#include <string>

#include "boost/unordered_map.hpp"
#include "boost/unordered_set.hpp"
#include "CParseUrl.h"
#include "../Lock.h"

#define FILTER_TYPE_SCRIPT 0x0001
#define FILTER_TYPE_IMAGE 0X0002
#define FILTER_TYPE_BACKGROUND 0x0004
#define FILTER_TYPE_STYLESHEET 0X0008
#define FILTER_TYPE_OBJECT 0X0010

#define FILTER_TYPE_XBL 0X0020 //不会支持
#define FILTER_TYPE_PING 0X0040

#define FILTER_TYPE_XMLHTTPREQUEST 0x0080
#define FILTER_TYPE_OBJECT_SUBREQUEST 0X0100
#define FILTER_TYPE_DTD 0X0200
#define FILTER_TYPE_SUBDOCUMENT 0X0400
#define FILTER_TYPE_DOCUMENT 0X0800
#define FILTER_TYPE_ELEMHIDE 0X1000
#define FILTER_TYPE_THIRD_PARTY 0x2000
//	#define FILTER_TYPE_DOMAIN 0X4000
//	#define FILTER_TYPE_MATCH_CASE 0X8000
//	#define FILTER_TYPE_COLLAPSE 0x10000

#define  CFG_CHECK_VIDEO 0x0001
#define  CFG_CHECK_WHITE 0x0002
#define  CFG_CHECK_ALL   0xFFFF

typedef unsigned int FilterType;

typedef std::vector<std::string> StringVector;
class FilterRule;
class HideRule;
class FilterRuleList;
class HideRuleList;
class ReplaceRule;
class ReplaceRuleList;

class FilterManager {
	typedef std::vector<FilterRule *> FilterRuleVector;
	
	class FilterRuleMap: public boost::unordered_map<std::string, FilterRuleList*> 
	{
		boost::unordered_set<unsigned int> unMatchRules;
	public:
		~FilterRuleMap();
		//prepare to start find
		inline void prepareStartFind() {
			this->unMatchRules.clear();
		}

		bool doFilter(const Url & mainURL, const std::string & key,const Url & url, FilterType t);
	};
	

	typedef boost::unordered_map<std::string /*domain*/, std::string /*selector*/ > HideRuleMap;
	
	
	typedef std::vector<ReplaceRule *> ReplaceRuleVector;
	class ReplaceRuleMap: public boost::unordered_map<std::string, ReplaceRuleList*> 
	{
		boost::unordered_set<unsigned int> unMatchRules;
	public:
		~ReplaceRuleMap();
		//prepare to start find
		inline void prepareStartFind() {
			this->unMatchRules.clear();
		}

		void getReplace(const std::string &key,const Url & url,std::vector<std::string> &v_Parse);
	};

	
	typedef boost::unordered_map<std::string /*host*/, int /*state*/ > ConfigRuleMap;


public:

	static FilterManager * getManager(const std::wstring & filename);
	static FilterManager * getManager();
	~FilterManager();
	bool addRule(std::string rule);
	bool hideRule(int id);
	bool shouldFilter(const Url & mainURL,const Url & url, FilterType t = 0);

	bool updateConfigVideoHost(const Url & url,int istate = 0);
	bool updateConfigWhiteHost(const Url & url,bool bEnable);
	
	//先检查广告过滤配置 和 白名单里的

	bool checkConfigHost(const Url & mainURL, const Url & url,bool &bFind,int cfgFlag = CFG_CHECK_ALL);

	std::string getcssRules(const Url & url);
	void getreplaceRules(const Url & url,std::vector<std::string> & v_replace);

private:
	//filter
	FilterRuleMap m_DomainWhiteRules; //white list use domain for map
	FilterRuleMap m_DomainFilterRules;

	FilterRuleMap m_ShortcutWhiteRules; //white list, use shortcut for map
	FilterRuleVector m_UnshortcutWhiteRules;

	FilterRuleMap m_ShortcutFilterRules;
	FilterRuleVector m_UnshortcutFilterRules;

	FilterRuleVector m_AllFilterRules;
	
	// hide
	HideRuleMap m_hiderules;
	
	//replace
	ReplaceRuleMap m_DomainReplaceRules;

	ReplaceRuleVector m_AllReplaceRules;
	//config
	ConfigRuleMap m_ConfigVideoRules;
	ConfigRuleMap m_ConfigWhiteRules;

private:
	FilterManager(const std::wstring & filename);
	FilterManager(const StringVector & rules);
private:
	void addRule(FilterRule * r);
	void addRule(HideRule * r);
	void addRule(ReplaceRule * r);

	void insertRuleToFilterRuleMap(FilterRuleMap * rules,const std::string & key, FilterRule * r);
	void insertRuleToFilterRuleMap(ReplaceRuleMap * rules,const std::string & key, ReplaceRule * r);

	bool shouldFilterByShortcut(const Url & mainURL, const Url & url,
			FilterType t);
	bool shouldFilterByDomain(const Url & mainURL, const Url & url,FilterType t,bool & isFind);

	XMLib::CriticalSection cs;

};
#endif // FILTERMANAGER_H
