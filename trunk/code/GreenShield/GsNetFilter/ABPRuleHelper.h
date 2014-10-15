#pragma once
#include <string>

bool ABPRuleToECMAScriptRegex(const std::string& abp_rule, std::string& regex_pattern, bool greedy = true);
bool ReplaceRuleToECMAScriptRegex(const std::string& rule, std::string& pattern, std::string& replace, bool& icase);
