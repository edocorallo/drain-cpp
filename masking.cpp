#ifndef _MASKING_
#define _MASKING_

#include "masking.hpp"

LogMasker::LogMasker(){//This below is ugly as fuck, but i can't care less about tidiness right now
    MaskingInstructions.reserve(10);
   this->MaskingInstructions.push_back(std::make_pair(std::regex("((?<=[^A-Za-z0-9])|^)([a-zA-Z-0-9./]+(=[a-zA-Z-0-9'./@ :]+)+(, \\.)?)((?=[^A-Za-z0-9])|$)"), std::string("<ID>")));//For StoRM-ATLAS FE
    this->MaskingInstructions.push_back(std::make_pair(std::regex("((?<=[^A-Za-z0-9])|^)((:*)([a-f0-9]{1,4}(\\.|:+)){3,7}[a-f0-9]{1,4})((?=[^A-Za-z0-9])|$)"), std::string("<IP>")));//For Messed up IPs at ATLAS
    this->MaskingInstructions.push_back(std::make_pair(std::regex("((?<=[^A-Za-z0-9])|^)(\\w{8}-\\w{4}-\\w{4}-\\w{4}-\\w{12})((?=[^A-Za-z0-9])|$)"), std::string("<TKN>")));//For StoRM-ATLAS FE
    this->MaskingInstructions.push_back(std::make_pair(std::regex("((?<=[^A-Za-z0-9])|^)\\w+]?://(?:[a-zA-Z]|[0-9]|[$-_@.&+]|[!*\\(\\),]|(?:[0-9a-fA-F][0-9a-fA-F]))+((?=[^A-Za-z0-9])|$)"), std::string("<URL>")));
    this->MaskingInstructions.push_back(std::make_pair(std::regex("((?<=[^A-Za-z0-9])|^)(0x[a-f0-9A-F]+)((?=[^A-Za-z0-9])|$)"), std::string("<HEX>")));//hexadecimal
    this->MaskingInstructions.push_back(std::make_pair(std::regex("((?<=[^A-Za-z0-9])|^)'([\\-\\+]?\\d+)'((?=[^A-Za-z0-9])|$)"), std::string("<NUM>")));//Numbers
        this->MaskingInstructions.push_back(std::make_pair(std::regex("((?<=[^A-Za-z0-9])|^)'([a-zA-Z-0-9./]+(=[a-zA-Z-0-9./@ :]+)+(, )?)'((?=[^A-Za-z0-9])|$)"), std::string("<ID>")));//Identifiers
 this->MaskingInstructions.push_back(std::make_pair(std::regex("((?<=[^A-Za-z0-9])|^)('[A-Za-z\\s]+')((?=[^A-Za-z0-9])|$)"), std::string("<TSK>")));//Task
};

std::string LogMasker::mask(std::string & content){
    for(auto mi : MaskingInstructions)
        content = std::regex_replace(content, mi.first, mi.second);
    return content;

};

#endif
/*
        {"regex_pattern":"((?<=[^A-Za-z0-9])|^)'([a-zA-Z-0-9./]+(=[a-zA-Z-0-9./@ :]+)+(, )?)'((?=[^A-Za-z0-9])|$)","mask_with": "ID"},
	    {"regex_pattern":"((?<=[^A-Za-z0-9])|^)([a-zA-Z-0-9./]+(=[a-zA-Z-0-9'./@ :]+)+(, \\.)?)((?=[^A-Za-z0-9])|$)","mask_with":"ID"},
	    {"regex_pattern":"((?<=[^A-Za-z0-9])|^)((:*)([a-f0-9]{1,4}(\\.|:+)){3,7}[a-f0-9]{1,4})((?=[^A-Za-z0-9])|$)","mask_with": "IP"},
	    {"regex_pattern":"((?<=[^A-Za-z0-9])|^)(\\w{8}-\\w{4}-\\w{4}-\\w{4}-\\w{12})((?=[^A-Za-z0-9])|$)","mask_with":"TKN"},
	    {"regex_pattern":"((?<=[^A-Za-z0-9])|^)\\w+]?://(?:[a-zA-Z]|[0-9]|[$-_@.&+]|[!*\\(\\),]|(?:[0-9a-fA-F][0-9a-fA-F]))+((?=[^A-Za-z0-9])|$)", "mask_with": "URL"},
	    {"regex_pattern":"((?<=[^A-Za-z0-9])|^)((: ?)(\\S+@\\S+))((?=[^A-Za-z0-9])|$)", "mask_with": "SEQ"},
	    {"regex_pattern":"((?<=[^A-Za-z0-9])|^)(0x[a-f0-9A-F]+)((?=[^A-Za-z0-9])|$)", "mask_with": "HEX"},
        {"regex_pattern":"((?<=[^A-Za-z0-9])|^)'([\\-\\+]?\\d+)'((?=[^A-Za-z0-9])|$)", "mask_with": "NUM"},
	    {"regex_pattern":"((?<=[^A-Za-z0-9])|^)('[A-Za-z\\s]+')((?=[^A-Za-z0-9])|$)","mask_with":"TSK"}
*/
int main(){}