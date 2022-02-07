#ifndef _DRAIN_H_
#define _DRAIN_H_

#include<iostream>
#include<utility>
#include<memory>
#include<optional>
#include<cassert>
#include<exception>

#include "Node.hpp"
#include "logCluster.hpp"

//#include <boost/algorithm/string/join.hpp> Sadge
//#include <format> Sadge

std::string param_str="<*>";


class Drain{
    public:

    uint depth;
    uint max_children;
    float sim_th;
    Node root_node;
    std::vector<LogCluster> clusters;//just to count all clusters and their own sizes

    Drain(const uint& depth=4, const float& sim_th=0.4, const uint& max_children=100);

    bool has_numbers(std::string& s);
    LogCluster tree_search(Node& root_node, std::vector<std::string>& tokens);
    void add_seq_to_prefix_tree(Node& root_node, LogCluster& cluster);
    std::pair<float,int> get_seq_distance(std::vector<std::string>& templ, std::vector<std::string>& seq);//?
    LogCluster fast_match(std::vector<LogCluster>& cluster_list, std::vector<std::string>& tokens);
    std::vector<std::string> get_template(std::vector<std::string>& templ, std::vector<std::string>& seq);
    void print_tree();
    void print_node(Node& node, const uint& depth);
    //std::string num_to_cluster_id(int num);
    std::pair<LogCluster,std::string> add_log_message(std::string& content);
    long unsigned int get_total_cluster_size();

    // std::ostream& operator << (std::ostream& os, Drain& drain);
    // std::istream& operator >> (std::istream& is, Drain& drain); 
};


Drain::Drain (const uint & depth /*=4*/, const float& sim_th /*=0.4*/, const uint& max_children /*=100*/){
    this->depth = depth-2;
    this->sim_th = sim_th;
    this->max_children = max_children;
    this->root_node = Node("ROOT",0);
    this->clusters.reserve(100);//ALL THESE RESERVES SHOULD BE MORE PRUDENT
};

bool Drain::has_numbers(std::string& s){
    return (s.find_first_of("0123456789") != std::string::npos);
};

LogCluster Drain::tree_search(Node& root_node, std::vector<std::string>& tokens){//returns std::optional<LogCluster>
    std::stringstream ss; ss<<tokens.size(); std::string tokensize = ss.str();
    LogCluster out_cluster;
    std::shared_ptr<Node> parent_node;
    
    try{//try traversing from Root to tokensize
        parent_node = root_node.key_to_child_node.at(tokensize);
    }
    catch (const std::exception&){
        std::clog<<"No template with the same token count"<<'\n';
        throw;//just throw it dude
    }
    if(!tokens.size()){//DA RIVEDERE
        out_cluster = root_node.key_to_child_node.at(tokensize)->clusters.at(0);
        std::cout<<"empty log line \n";
        return out_cluster;//empty string
    }
    
    uint current_depth = 1;
    for (auto token : tokens){
    //if at max depth or last token in tokens, break
        if ((current_depth == this->depth) || (current_depth == tokens.size())) break;

        //std::unordered_map<std::string,std::shared_ptr<Node>> new_key_to_child_node = parent_node->key_to_child_node;//WeirdChamp
        if (parent_node->key_to_child_node.find(token) != parent_node->key_to_child_node.end())
            parent_node = parent_node->key_to_child_node.at(token);
        else if (parent_node->key_to_child_node.find(param_str) != parent_node->key_to_child_node.end())
            parent_node = parent_node->key_to_child_node.at(param_str);
        else
            throw std::logic_error("token not found in tree_search().");
        current_depth++;    
    }

    try{
    out_cluster = this->fast_match(parent_node->clusters,tokens);//copia
    }   catch (...){
            throw;
        }
    std::cout<<"cluster found in tree_search"<<'\n';

    //std::cout<<"out_cluster.log_template_tokens.size() in tree_search: "<<out_cluster.log_template_tokens.size()<<'\n';
    return out_cluster;
};

void Drain::add_seq_to_prefix_tree(Node& root_node, LogCluster& cluster){

    std::stringstream ss; ss<<cluster.log_template_tokens.size(); std::string tokensize = ss.str();
    std::shared_ptr<Node> parent_node;
    if(root_node.key_to_child_node.find(tokensize) == root_node.key_to_child_node.end()){//If tokensize is not a key in key_to_child_node, create one 
        parent_node = std::make_shared<Node>(tokensize,1);
        std::pair<std::string,std::shared_ptr<Node>> new_pair(tokensize,parent_node);
        root_node.key_to_child_node.insert(new_pair);
    }
    else 
        parent_node = root_node.key_to_child_node.at(tokensize);

    //handle empty string case:
    if (cluster.log_template_tokens.size() == 0){
        parent_node->clusters.push_back(cluster);//could use emplace_back()
        return;
    }
    uint current_depth = 1;
    for (auto token : cluster.log_template_tokens){
        //add current log cluster to leaf node
        if ((current_depth == this->depth) || (current_depth == cluster.log_template_tokens.size())){
            parent_node->clusters.emplace_back(cluster.log_template_tokens, cluster.cluster_id, cluster.size);//could use emplace_back()
            break; 
        }
        //If the token is not matched in the current layer of the existing tree
        if(parent_node->key_to_child_node.find(token) == parent_node->key_to_child_node.end()){
            if (!this->has_numbers(token)){
                //if param_str is found in the current layer of the existing tree
                if(parent_node->key_to_child_node.find(param_str) != parent_node->key_to_child_node.end()){
                    //check if current parent_node has reached its maximum size before inserting the new pair
                    if (parent_node->key_to_child_node.size() < this->max_children){
                        std::shared_ptr<Node> new_node = std::make_shared<Node>(token, current_depth+1);
                        std::pair<std::string,std::shared_ptr<Node>> new_pair(token, new_node);
                        parent_node->key_to_child_node.insert(new_pair);
                        parent_node = new_node;                
                    }
                    else parent_node = parent_node->key_to_child_node.at(param_str);
                }//if param_str is not found in the current layer of the existing tree
                else {

                    if((parent_node->key_to_child_node.size() + 1) < this->max_children){
                        std::shared_ptr<Node> new_node = std::make_shared<Node>(token,current_depth+1);
                        std::pair<std::string,std::shared_ptr<Node>> new_pair(token, new_node);
                        parent_node->key_to_child_node.insert(new_pair);
                        parent_node = new_node;
                    }//if current parent node has reached max_child, we use the special node "<*>"
                    else if((parent_node->key_to_child_node.size() + 1) == this->max_children){
                        std::shared_ptr<Node> new_node = std::make_shared<Node>(param_str, current_depth+1);
                        std::pair<std::string,std::shared_ptr<Node>> new_pair(param_str, new_node);
                        parent_node->key_to_child_node.insert(new_pair);
                        parent_node = new_node;
                    }  
                    else parent_node = parent_node->key_to_child_node.at(param_str);
                }
            }
            else {//has_numbers(token) = true
                if(parent_node->key_to_child_node.find(param_str) == parent_node->key_to_child_node.end()){
                    std::shared_ptr<Node> new_node = std::make_shared<Node>(param_str, current_depth+1);
                    std::pair<std::string,std::shared_ptr<Node>> new_pair(param_str, new_node);
                    parent_node->key_to_child_node.insert(new_pair);
                    parent_node = new_node;
                }
                else parent_node = parent_node->key_to_child_node.at(param_str);
            }
        }
        //token found in key_to_child_node
        else parent_node = parent_node->key_to_child_node.at(token);
        current_depth++;
    }
};

std::pair<float,int> Drain::get_seq_distance(std::vector<std::string>& templ, std::vector<std::string>& seq){
    assert(templ.size() == seq.size());
    int sim_tokens = 0;
    int param_count = 0;

    for(uint i = 0; i < seq.size(); i++){
        if (templ[i] == param_str){
            param_count +=1;
            continue;
        }
        if (templ[i] == seq[i]) sim_tokens += 1;
    }
    std::pair<float,int> output = std::make_pair((float)sim_tokens / seq.size(),param_count);
    return output;
};

LogCluster Drain::fast_match(std::vector<LogCluster>& cluster_list, std::vector<std::string>& tokens){
    LogCluster match_cluster;
    float max_sim = -1.;
    int max_param_count = -1;
    uint cluster_index=0;
    uint matched_index = 0;
    for(auto cluster : cluster_list){//ONCE AGAIN for(auto x : collection) makes copies
    //for (auto cluster = cluster_list.begin(); cluster != cluster_list.end();cluster++){
        std::pair<float,int> seq_dist = this->get_seq_distance(cluster.log_template_tokens,tokens);
        if (seq_dist.first > max_sim || (seq_dist.first == max_sim && seq_dist.second > max_param_count)){
            max_sim = seq_dist.first;
            max_param_count = seq_dist.second;
            match_cluster = std::move(cluster);
            matched_index = cluster_index;
        }
        cluster_index++;
    }
    if (max_sim >= this->sim_th){
        //std::cout<<"match_cluster.log_template_tokens.size() in fast_match(): "<<match_cluster.log_template_tokens.size()<<'\n';
        match_cluster.size++;
        cluster_list.at(matched_index).size++;
        std::vector<std::string> new_templ_tokens = this->get_template(tokens,cluster_list.at(matched_index).log_template_tokens);
        cluster_list.at(matched_index).log_template_tokens=new_templ_tokens;
        return match_cluster;
    }
    throw std::logic_error("cluster not found in fast_match()");
};

std::vector<std::string> Drain::get_template(std::vector<std::string>& templ, std::vector<std::string>& seq){
    assert (templ.size() == seq.size());//need something if assert fails
    std::vector<std::string> out_templ;
    uint i = 0;
    for(auto word : templ){
        if ( word == seq.at(i) ) out_templ.push_back(word);
        else out_templ.push_back(param_str);
        i++;
    }
    return out_templ;
};

void Drain::print_node(Node& node, const uint& depth){
    for ( uint i = 0; i<depth; i++) std::cout<<'\t';
    if (node.depth == 0) std::cout<<"<ROOT>";
    else std::cout<<"<"<<node.key<<">";
    std::cout<<'\n';
    if (node.depth == this->depth) return;
    for(auto& i : node.key_to_child_node){
        this->print_node(*node.key_to_child_node.at(i.first),depth+1);
    }
};

void Drain::print_tree(){
    this->print_node(this->root_node,0);
};

std::pair<LogCluster,std::string> Drain::add_log_message(std::string& content){

    std::stringstream ss; ss<<"C"<<this->clusters.size()+1; std::string cluster_id = ss.str();
    std::istringstream iss (content);

    LogCluster out_cluster;
    std::string update_type;
    if (content.find_first_not_of(" \t\v\r\n") != std::string::npos){//some problems with tabs
        std::size_t start = content.find_first_not_of(" \t\v\r\n");
        std::size_t end = content.find_last_not_of(" \t\v\r\n");
        content = content.substr(start, end - start + 1);
    }

    //std::cout<<"stripped content: '"<<content<<"'\n";

    std::vector<std::string> content_tokens;
    while (std::getline(iss,update_type,' ')) {//using update_type because i don't want to initialize another string
        //std::cout<<"content.substr: "<<update_type<<'\n';
        content_tokens.push_back(update_type);
        update_type.erase();
    }
    
    //for(auto token : content_tokens) std::cout<<token<<'\n';

    try{
        out_cluster = this->tree_search(this->root_node,content_tokens);
        //std::cout<<"out_cluster.log_template_tokens.size() in try{} :"<<out_cluster.log_template_tokens.size()<<'\n';
    }   catch(...){
            out_cluster = LogCluster(content_tokens,cluster_id,1);
            this->clusters.emplace_back(content_tokens,cluster_id,1);
            this->add_seq_to_prefix_tree(this->root_node, out_cluster);
            update_type = "cluster_created";
            return std::make_pair(out_cluster, update_type);//seems like I can't avoid this copy-------------
        }
    //std::cout<<"out_cluster.log_template_tokens.size() outside try{} :"<<out_cluster.log_template_tokens.size()<<'\n';
    //for(auto token : content_tokens) std::cout<<token<<'\n';
    //std::cout<<"content_tokens.size(): "<<content_tokens.size()<<", out_cluster.log_template_tokens.size(): "<<out_cluster.log_template_tokens.size()<<'\n';        
    std::vector<std::string> new_templ_tokens = this->get_template(content_tokens,out_cluster.log_template_tokens); 
    std::stringstream sst;
    for(auto token : new_templ_tokens){
        sst<<token<<" ";
    }
    std::string new_templ = sst.str();
    if ( new_templ != out_cluster.get_template()){
        out_cluster.log_template_tokens.erase(out_cluster.log_template_tokens.begin(),out_cluster.log_template_tokens.end());
        // for(uint i = 0; i< new_templ_tokens.size(); i++)
        //     out_cluster.log_template_tokens.push_back(new_templ_tokens.at(i));
        out_cluster.log_template_tokens = new_templ_tokens;
        update_type = "cluster_template_changed";
    }
    else {
        update_type = "none";
    }

    return std::make_pair(out_cluster, update_type);//seems like I can't avoid this copy---------------
};

long unsigned int Drain::get_total_cluster_size(){
    long unsigned int size = 0;
    for (auto it = this->clusters.begin(); it != this->clusters.end(); it++) size += it->size;
    //for (auto c : this->clusters) size+=c.size;
    return size;
};

    // std::ostream& operator << (std::ostream& os, Drain& drain){
    //     os << (int)drain.depth << (int)drain.max_children;
    //     os.write(reinterpret_cast<char*>(&drain.sim_th),sizeof(float));//uhhh
    //     os << drain.root_node;
    //     os << (int)drain.clusters.size();
    //     for(auto c : drain.clusters) os << c << ",";
    //     os <<'\b';
    //     return os;

    // };


    // std::istream& operator >> (std::istream& is, Drain& drain){
    //     is << (int)drain.depth << (int)drain.max_children;
    //     is.read(reinterpret_cast<char*>(&drain.sim_th),sizeof(float));//uhhh
    //     return is;
    // }; 

#endif










