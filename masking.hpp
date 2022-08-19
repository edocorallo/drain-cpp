#ifndef _MASKING_H_
#define _MASKING_H_

#include<regex>
#include<utility>

//https://github.com/Tencent/rapidjson/ 
//configparser
//logging
// std::string config_path = "";
// class MaskingInstruction{//this whole class server the sole purpose to be filled with config values...
//     public:
//         std::string regex_pattern;
//         std::string mask_with;
//         std::basic_regex<char> regex;
//         MaskingInstruction(std::string regex_pattern,std::string mask_with);
// };





class LogMasker{
    public:
        std::vector<std::pair<std::regex, std::string>> MaskingInstructions;
        LogMasker();
        std::string mask(std::string& content);
        //void get_config(std::string config_path);
};


LogMasker::LogMasker(){
    
    std::regex ID_regex("(\\s([0-9a-f]{2,}:){3,}([0-9a-f]{2,})\\s?)");
    std::regex IP_regex("(\\s\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\s?)");
    std::regex URL_regex("\\s\\w+]?://(?:[a-zA-Z]|[0-9]|[$-_@.&+]|[!*\\(\\),]|(?:[0-9a-fA-F][0-9a-fA-F])\\s?)+");
    std::regex HEX_regex("(\\s0x[a-f0-9A-F]+\\s?)");
    std::regex NUM_regex("(\\s[\\-\\+]?\\d+\\s?)");//still problematic
    
    //std::regex TSK_regex("((?<=[^A-Za-z0-9])|^)('[A-Za-z\\s]+')((?=[^A-Za-z0-9])|$)"); StoRM ATLAS
    //std::regex TKN_regex("((?<=[^A-Za-z0-9])|^)(\\w{8}-\\w{4}-\\w{4}-\\w{4}-\\w{12})((?=[^A-Za-z0-9])|$)");StoRM ATLAS



    //This below is ugly as fuck, but i can't care less about tidiness right now
    MaskingInstructions.reserve(5);
    this->MaskingInstructions.push_back(std::make_pair(ID_regex, std::string("<ID>")));//For StoRM-ATLAS FE
    this->MaskingInstructions.push_back(std::make_pair(IP_regex, std::string("<IP>")));//For Messed up IPs at ATLAS
    this->MaskingInstructions.push_back(std::make_pair(URL_regex, std::string("<URL>")));//URLs
    this->MaskingInstructions.push_back(std::make_pair(HEX_regex, std::string("<HEX>")));//Hexadecimals
    this->MaskingInstructions.push_back(std::make_pair(NUM_regex, std::string(" <NUM> ")));//numbers, even though drain ignores numbers
    
};

std::string LogMasker::mask(std::string & content){
    for(auto mi : MaskingInstructions)
        content = std::regex_replace(content, mi.first, mi.second);
    return content;

};


#endif


/*
AD FUTURUM lookahead and lookbehind are not part of stl regex 
lookahead      ((?<=[^A-Za-z0-9])|^)
lookbehind     ((?=[^A-Za-z0-9])|$)
*/