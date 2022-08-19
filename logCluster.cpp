#ifndef _LOG_CLUSTER_
#define _LOG_CLUSTER_

#include<sstream>

#include"logCluster.hpp"

LogCluster::LogCluster(std::vector<std::string>& log_templ_tokens, std::string& cluster_id){
    this->log_template_tokens = log_templ_tokens;
    this->cluster_id = cluster_id;
    this->size = 1;
};

LogCluster::LogCluster(const LogCluster& cluster){
    this->log_template_tokens = cluster.log_template_tokens;
    this->cluster_id = cluster.cluster_id;
    this->size = cluster.size;    
};

LogCluster::LogCluster(){

};

std::string LogCluster::get_template() const{
    /*
    Returns a string containing the elements of log_template_tokens sepatared by a " " white space
    */
   std::stringstream ss;
   for(auto token : this->log_template_tokens){
       ss<<token<<" ";
   }
    return ss.str();
};

std::string LogCluster::to_str() const {
    std::stringstream ss;
    ss<< this->cluster_id << " ( size: " << this->size << " ) : "<<this->get_template();
    return ss.str();

    //return std::format("{} (size: {}): {} ",this->cluster_id, this->size, this->get_template()); 
};
#endif
