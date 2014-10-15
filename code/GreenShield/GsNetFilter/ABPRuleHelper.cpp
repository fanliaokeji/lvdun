#include "stdafx.h"
#include "ABPRuleHelper.h"

bool ABPRuleToECMAScriptRegex(const std::string& abp_rule, std::string& regex_pattern, bool greedy)
{
	if(abp_rule.empty()) {
		return false;
	}
	regex_pattern.clear();
	int state = 0;
	for(std::size_t index = 0; index < abp_rule.size(); ) {
		char ch = abp_rule[index];
		switch(state) {
			case 0:
				if(ch == '!' || ch == '@' || ch == '~' || ch == '/') {
					return false;
				}
				else if(ch == '|') {
					if(abp_rule.size() == index + 1) {
						return false;
					}
					if(abp_rule[index + 1] != '|') {
						regex_pattern.push_back('^');
						state = 1;
					}
					else {
						regex_pattern += "^http://(.*\\.)?";
						++index;
						state = 1;
					}
				}
				else {
					regex_pattern += ".*";
					state = 1;
					continue;
				}
				break;
			case 1:
				if(ch == '*') {
					if(index != 0 && abp_rule[index-1] != '*') {
						regex_pattern += ".*";
					}
				}
				else if(ch == '^') {
					if((index == 0 || abp_rule[index-1] == '*') && !greedy) {
						regex_pattern.push_back('?');
					}
					regex_pattern += "[^0-9a-zA-Z\\-\\.%]";
				}
				else if(ch == '.' || ch == '['
					|| ch == '{' || ch == '}'
					|| ch == '(' || ch == ')'
					|| ch == '\\' || ch == '+'
					|| ch == '?' || ch == '$') {
					if((index == 0 || abp_rule[index-1] == '*') && !greedy) {
						regex_pattern.push_back('?');
					}
					regex_pattern.push_back('\\');
					regex_pattern.push_back(ch);
				}
				else if(ch == '|') {
					if(index + 1 != abp_rule.size()) {
						return false;
					}
					else {
						if((index == 0 || abp_rule[index-1] == '*') && !greedy) {
							regex_pattern.push_back('?');
						}
						regex_pattern.push_back('$');
						return true;
					}
				}
				else {
					if((index == 0 || abp_rule[index-1] == '*') && !greedy) {
						regex_pattern.push_back('?');
					}
					regex_pattern.push_back(ch);
				}
				break;
		}
		++index;
	}
	if(abp_rule[abp_rule.size() - 1] != '*') {
		regex_pattern += ".*";
	}
	return true;
}

bool ReplaceRuleToECMAScriptRegex(const std::string& rule, std::string& pattern, std::string& replace, bool& icase)
{
	pattern.clear();
	replace.clear();
	if(rule.size() < 5) {
		return false;
	}
	if(rule[0] != '$') {
		return false;
	}

	icase = false;
	if(rule[1] == 'S') {
		icase = true;
	}
	else if(rule[1] != 's') {
		return false;
	}

	if(rule[2] != '@') {
		return false;
	}
	
	std::size_t index = 3;
	for(; index < rule.size() && rule[index] != '@'; ++index) {
		char ch = rule[index];
		if(ch == '?') {
			pattern.push_back('.');
		}
		else if(ch == '*') {
			if(pattern.empty() || index + 1 == rule.size() || rule[index + 1] == '@') {
				return false;
			}
			if(rule[index - 1] != '*') {
				pattern += ".*?";
			}
		}
		else if(ch == '.' || ch == '['
			|| ch == '{' || ch == '}'
			|| ch == '(' || ch == ')'
			|| ch == '\\' || ch == '+'
			|| ch == '|' || ch == '^'
			|| ch == '$') {
			pattern.push_back('\\');
			pattern.push_back(ch);
		}
		else {
			pattern.push_back(ch);
		}
	}
	if(index == rule.size()) {
		return false;
	}

	++index;

	for(; index < rule.size() && rule[index] != '@'; ++index) {
		replace.push_back(rule[index]);
	}
	if(index == rule.size() || index + 1 != rule.size()) {
		return false;
	}
	return true;
}
