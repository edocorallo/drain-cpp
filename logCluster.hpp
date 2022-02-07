#ifndef _LOG_CLUSTER_H_
#define _LOG_CLUSTER_H_

#include<vector>
#include<string>
#include<sstream>


class LogCluster{
    public:

    
    std::string cluster_id;
    long unsigned int size;
    std::vector<std::string>log_template_tokens;

    //LogCluster(std::vector<std::string>& log_template_tokens,std::string & cluster_id);
    LogCluster(std::vector<std::string>& log_template_tokens,std::string & cluster_id, unsigned long size );
    LogCluster(const LogCluster& cluster);
    LogCluster(const LogCluster&& cluster) noexcept;
    LogCluster();

    LogCluster& operator = (LogCluster&& other); 
    LogCluster& operator = (const LogCluster& other);

    std::string get_template() const;
    std::string to_str() const;// maybe I need something that represents my cluster
};

// std::istream& operator >> (std::istream& is, LogCluster& cluster);
// std::ostream& operator << (std::ostream& os, LogCluster& cluster);

// LogCluster::LogCluster(std::vector<std::string>& log_templ_tokens, std::string& cluster_id){
//     this->log_template_tokens = log_templ_tokens;
//     this->cluster_id = cluster_id;
//     this->size = 1;//how many logs got stored inside this cluster
// };

LogCluster::LogCluster(std::vector<std::string>& log_template_tokens, std::string & cluster_id, unsigned long size ){
    this->log_template_tokens = log_template_tokens;
    this->cluster_id = cluster_id;
    this->size = size;
};

LogCluster::LogCluster(const LogCluster& cluster){
    this->log_template_tokens = cluster.log_template_tokens;
    this->cluster_id = cluster.cluster_id;
    this->size = cluster.size;
    std::cout<<"COPIED LOGCLUSTER\n";    
};

LogCluster::LogCluster(const LogCluster&& cluster) noexcept : 
    log_template_tokens(std::move(cluster.log_template_tokens)),
    cluster_id(std::move(cluster.cluster_id)),
    size(std::move(cluster.size))
    {
        std::cout<<"LOGCLUSTER MOVED\n";
    };

LogCluster::LogCluster(){

};

LogCluster& LogCluster::operator = (LogCluster&& other){
    this->log_template_tokens = std::move(other.log_template_tokens);
    this->cluster_id = std::move(other.cluster_id);
    this->size = std::move(other.size);
    std::cout<<"LOGCLUSTER HAS BEEN MOVE-ASSIGNED\n";
    return *this;
}; 

LogCluster& LogCluster::operator = (const LogCluster& other){
    log_template_tokens = other.log_template_tokens;
    cluster_id = other.cluster_id;
    size = other.size;
    std::cout<<"LOGCLUSTER HAS BEEN COPY-ASSIGNED\n";
    return *this;
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
    ss<<"   ID: "<<this->cluster_id << " ( size: " << this->size << " )   template: "<<this->get_template();
    return ss.str();

    //return std::format("{} (size: {}): {} ",this->cluster_id, this->size, this->get_template()); 
};


// std::istream& operator >> (std::istream is, LogCluster& cluster){
//     is >> cluster.cluster_id >> cluster.size;
//     int tokens_size;
//     is >> tokens_size;
//     std::string tokens;
//     is >> tokens;
//     std::istringstream ss (tokens);
//     std::string token;
//     cluster.log_template_tokens.reserve(tokens_size);
//     while ( std::getline(ss, token, ',')) cluster.log_template_tokens.push_back(token);
//     return is;   
// };

// std::ostream& operator << (std::ostream os, LogCluster& cluster){
//     os << cluster.cluster_id << cluster.size << (int)cluster.log_template_tokens.size();
//     //using "," as delimitator for tokens
//     for (auto token : cluster.log_template_tokens)
//         os << token << ",";
//     os << '\b';//erase the last ","  
     
// };
#endif