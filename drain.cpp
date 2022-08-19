#ifndef _DRAIN_
#define _DRAIN_

#include "drain.hpp"

Drain::Drain (const uint & depth /*=4*/, const float& sim_th /*=0.4*/, const uint& max_children /*=100*/){
    this->depth = depth-2;
    this->sim_th = sim_th;
    this->max_children = max_children;
    this->root_node = Node("ROOT",0);
};

bool Drain::has_numbers(std::string& s){
    return (s.find_first_of("0123456789") != std::string::npos);
};

auto Drain::tree_search(Node& root_node, std::vector<std::string>& tokens){//returns std::optional<LogCluster>
    std::stringstream ss; ss<<tokens.size(); std::string tokensize = ss.str();
    auto out_cluster = std::make_optional<LogCluster> ();
    Node parent_node;
    try{
        parent_node = *root_node.key_to_child_node.at(tokensize);
    }
    catch (const std::exception&){
        std::clog<<"No template with the same token count"<<'\n';
        return out_cluster;
    }
    if(!tokens.size()){
        out_cluster = root_node.key_to_child_node.at(tokensize)->clusters[0];
        return out_cluster;//empty string
    }
    
    uint current_depth = 1;
    for (auto token : tokens){
    //if at max depth or last token in tokens, break
        if ((current_depth == this->depth) || (current_depth == tokens.size())) break;

        std::unordered_map<std::string,Node*> new_key_to_child_node = parent_node.key_to_child_node;
        if (new_key_to_child_node.find(token) != new_key_to_child_node.end())
            parent_node= *new_key_to_child_node[token];
        else if (new_key_to_child_node.find(param_str) != new_key_to_child_node.end())
            parent_node=*new_key_to_child_node[param_str];
        else
            return out_cluster;
        current_depth++;
        
    }
    out_cluster = this->fast_match(parent_node.clusters,tokens);
    return out_cluster;
};

void Drain::add_seq_to_prefix_tree(Node& root_node, LogCluster& cluster){
    std::stringstream ss; ss<<cluster.log_template_tokens.size(); std::string tokensize = ss.str();
    Node first_layer_node (tokensize,1);
    if(root_node.key_to_child_node.find(tokensize) != root_node.key_to_child_node.end()){
        root_node.key_to_child_node[tokensize]= &first_layer_node;
    }
    else 
        Node first_layer_node = *root_node.key_to_child_node[tokensize];

    Node parent_node = first_layer_node;

    //handle empty string case:
    if (cluster.log_template_tokens.size() == 0){
        parent_node.clusters.push_back(cluster);
        return;
    }
    uint current_depth = 1;
    for (auto token : cluster.log_template_tokens){
        //add current og cluster to leaf node
        if ((current_depth == this->depth) || (current_depth == cluster.log_template_tokens.size())){
            parent_node.clusters.push_back(cluster);
            break; 
        }

        if(parent_node.key_to_child_node.find(token) == parent_node.key_to_child_node.end()){
            if (!this->has_numbers(token)){
                if(parent_node.key_to_child_node.find(param_str) != parent_node.key_to_child_node.end()){
                    if (parent_node.key_to_child_node.size() < this->max_children){
                        Node new_node (token, current_depth+1);
                        parent_node.key_to_child_node[token] = &new_node;
                        parent_node = new_node;                
                    }
                    else parent_node = *parent_node.key_to_child_node[param_str];
                }
                else {
                    if((parent_node.key_to_child_node.size() + 1) < this->max_children){
                        Node new_node (token,current_depth+1);
                        parent_node.key_to_child_node[token] = &new_node;
                        parent_node=new_node;
                    }
                    else if((parent_node.key_to_child_node.size() + 1) == this->max_children){
                        Node new_node (param_str,current_depth+1);
                        parent_node.key_to_child_node[param_str] = &new_node;
                        parent_node = new_node;
                    }  
                    else parent_node = * parent_node.key_to_child_node[param_str];
                }
            }
            else {//has_numbers(token) = true
                if(parent_node.key_to_child_node.find(param_str) == parent_node.key_to_child_node.end()){
                    Node new_node(param_str,current_depth+1);
                    parent_node.key_to_child_node[param_str] = &new_node;
                    parent_node = new_node;                    
                }
                else parent_node = *parent_node.key_to_child_node[param_str];
            }
        }
        //token found in key_to_child_node
        else parent_node = *parent_node.key_to_child_node[token];
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

std::optional<LogCluster> Drain::fast_match(std::vector<LogCluster>& cluster_list, std::vector<std::string>& tokens){
    LogCluster match_cluster;
    float max_sim = -1.;
    int max_param_count = -1;

    for(auto cluster : cluster_list){
        std::pair<float,int> seq_dist = this->get_seq_distance(cluster.log_template_tokens,tokens);
        if (seq_dist.first > max_sim || (seq_dist.first == max_sim && seq_dist.second > max_param_count)){
            max_sim = seq_dist.first;
            max_param_count = seq_dist.second;
            match_cluster = cluster;
        }
    }
    if (max_sim >= this->sim_th){
        return std::make_optional<LogCluster>(match_cluster);
    }
    return std::make_optional<LogCluster>();
};

std::vector<std::string> Drain::get_template(std::vector<std::string>& templ, std::vector<std::string>& seq){
    assert (templ.size() == seq.size());
    std::vector<std::string> out_templ;
    uint i = 0;
    for(auto word : templ){
        if ( word == seq[i] ) out_templ.push_back(word);
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

    std::string update_type;
    LogCluster out_cluster;

    std::size_t start = content.find_first_not_of(" \t\v\r\n");
    std::size_t end = content.find_last_not_of(" \t\v\r\n");
    content = content.substr(start, end - start + 1);

    std::vector<std::string> content_tokens;
    while ((start = content.find_first_of(" \t\v\r\n")) != std::string::npos) {
    content_tokens.push_back(content.substr(0, start));
    content.erase(0, start);
    }

    auto match_cluster = this->tree_search(this->root_node,content_tokens);

    if(!match_cluster.has_value()){
        out_cluster = LogCluster(content_tokens,cluster_id);
        this->clusters.push_back(out_cluster);
        this->add_seq_to_prefix_tree(this->root_node, out_cluster);
        update_type = "cluster_created";
    }
    else{
        out_cluster = match_cluster.value();
        std::vector<std::string> new_templ_tokens = this->get_template(content_tokens,out_cluster.log_template_tokens); 
        std::stringstream sst;
        for(auto token : new_templ_tokens){
            sst<<token<<" ";
        }
        std::string new_templ = sst.str();
        if ( new_templ != out_cluster.get_template()){
            out_cluster.log_template_tokens = new_templ_tokens;
            update_type = "cluster_template_changed";
        }
        else update_type = "none";
        out_cluster.size++;
    }
    return std::make_pair(out_cluster, update_type);
};

long unsigned int Drain::get_total_cluster_size(){
    long unsigned int size = 0;
    for (auto c : this->clusters) size+=c.size;
    return size;
};




#endif