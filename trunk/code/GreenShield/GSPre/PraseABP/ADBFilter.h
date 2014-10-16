#ifndef FILTER_H
#define FILTER_H

#include <boost/algorithm/string.hpp>
 
#include <vector>
#include <string>

#include "FilterManager.h"
#include <hash_map>


class FilterRule {
public:
	FilterRule(const std::string & rule);
    bool shouldFilter(const Url & mainURL,const Url & url, FilterType t);
    bool isWhiteFilter() { return m_isException;}
    bool isNeedMimeType() { return m_type!=0;}
    void getDomains(StringVector & domains);
    const std::string & getRegularFilter(){ return m_reFilter;}
    const std::string & getWholeRule() { return m_rule;}

	void print();

private:
	bool m_isMatchProtocol;
    bool m_isException;
	std::string m_reFilter;
	//std::string m_reFilter;
    std::string m_rule;
    FilterType m_type;
    bool m_filterThirdParty;
    bool m_matchFirstParty;
	std::vector<std::string> m_domains;
	std::vector<std::string> m_inverseDomains;

private:
    bool isMatchType(const Url & url,FilterType t);
    bool isMatchThirdParty(const Url & host,const Url & other);
    bool isMatchDomains( const Url & url);
    void processDomains(std::string & ds);
};

class HideRule {
public:

	HideRule(const std::string & r);
	const StringVector & domains() { return m_domains;}
	//example.com,~foo.example.com##*.sponsor
	//*.sponsor就是selector
	const std::string & selector() { return m_sel;}
	void print();
private:
	std::string m_sel;
	std::vector<std::string> m_domains;
};

class ReplaceRule {
public:

	ReplaceRule(const std::string & r);

	bool shouldReplace(const Url & u);
	const std::string & getReplace(){ return m_reParse;};

	void getDomains(std::string & domain);
	const std::string & getRegularFilter(){ return m_reReplace;}
private:
	std::string m_reParse;
	std::vector<std::string> m_domains;
	std::string m_reReplace;
	std::string m_rule;
	bool m_isMatchProtocol;
	
};

#endif // FILTER_H
