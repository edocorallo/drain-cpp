#ifndef _TEMPLATE_MINER_H_
#define _TEMPLATE_MINER_H_

#include<chrono>
#include<fstream>

#include"drain.hpp"
#include"masking.hpp"

using std::operator""s;

class TemplateMiner
{
public:

    Drain drain;
    LogMasker masker;
    bool compress = false;
    std::chrono::duration<double> snapshot_interval_s= 300s;
    std::chrono::time_point<std::chrono::system_clock> last_save_time;


    TemplateMiner();
    ~TemplateMiner();
    void save_tree(std::stringstream& ss, std::shared_ptr<Node>& node);
    void load_tree(std::string& serialized_tree);
    void load_state(const std::string& file_name);
    void save_state(const std::string& file_name, const std::string& snapshot_reason);
    std::string get_snapshot_reason(std::string& change_type);
    std::vector<std::string> add_log_message (std::string& log_message);
    
};
//friend std::istream& operator >> (std::istream& IS, TemplateMiner& TM)

TemplateMiner::TemplateMiner(/* args */){
    this->drain = Drain(4, 0.4, 100);
    this->masker = LogMasker();
    this->last_save_time = std::chrono::system_clock::now();
    this->load_state("persistence.bin");
}

TemplateMiner::~TemplateMiner(){

}


void TemplateMiner::save_tree (std::stringstream& fs, std::shared_ptr<Node>& node) {        
        //node key
    fs<<node->key<<";"<<node->depth<<";";
    std::cout <<node->key<<";"<<node->depth<<";";
        //node clusters
    // for (auto cluster : node->clusters){<---copied a logcluster?!
    for (uint i =0; i<node->clusters.size(); i++){
        fs << node->clusters.at(i).cluster_id <<"/"<< node->clusters.at(i).size<<"/";
        std::cout << node->clusters.at(i).cluster_id<<"/"<<node->clusters.at(i).size<<"/";
        for (auto token : node->clusters.at(i).log_template_tokens){fs<<token<<",";std::cout<<token<<",";}
        fs<<"_";
        std::cout<<"_";
    }

    if(node->depth == this->drain.depth) {fs<<"}"; std::cout<<"}"; return;}




    //key_to_child_node
    for (auto pair = node->key_to_child_node.begin(); pair != node->key_to_child_node.end(); pair++ ){
        fs<<"{";std::cout<<"{"; save_tree(fs, pair->second);}
    fs<<"}"; std::cout<<"}"; 
};

void TemplateMiner::load_tree(std::string& serialized_tree){
    if(serialized_tree.find_first_of("{") == serialized_tree.npos) return;//base case: no parenthesis
    
    while (serialized_tree.starts_with("}")) {//broken parenthesis to be fixed
        serialized_tree.erase(0,1);
        serialized_tree.push_back('}');
    }
    
    //get rid of leading "{" and ending "}"
    serialized_tree = serialized_tree.substr(1, serialized_tree.size()-2 );
    //std::cout<<"serialized tree: "<<serialized_tree<<std::endl;
    std::string string_to_parse;

    //finding an open parenthesis should mean you'll find a closed one
    if(std::size_t bracket = serialized_tree.find("{") != serialized_tree.npos){
        string_to_parse = serialized_tree.substr(0,serialized_tree.find_first_of("{}"));
        serialized_tree = serialized_tree.substr(serialized_tree.find_first_of("{}"), serialized_tree.size()-1 );
    }
    else {
        string_to_parse=serialized_tree;
    }
    
    //std::cout<<"string to parse: "<<string_to_parse<<std::endl;
    //std::cout<<"remaining string: "<<serialized_tree<<std::endl;

    std::stringstream ss(string_to_parse);
    std::string node_key, node_depth, cluster, cluster_id, cluster_size, token;
    std::vector<std::string> template_tokens;
    uint templ_token_lenght=0;
    static std::string branch_key;
    branch_key.reserve(50);

   //parsing 
    std::getline(ss,node_key,';');
    std::getline(ss,node_depth,';');
    uint depth = std::stoi(node_depth);
    
    
    if(depth == 1){//ROOT node is always constructed within drain, within template_miner. no need to do it again
        std::shared_ptr<Node> node = std::make_shared<Node>(node_key,depth);
        std::pair<std::string, std::shared_ptr<Node>> pair (node_key, node);
        this->drain.root_node.key_to_child_node.insert(pair);//
        branch_key=node_key;
    }
    if (depth == 2){

        std::shared_ptr<Node> node = std::make_shared<Node>(node_key,depth);
        std::pair<std::string, std::shared_ptr<Node>> pair (node_key, node);
        this->drain.root_node.key_to_child_node.at(branch_key)->key_to_child_node.insert(pair);

        while(std::getline(ss,cluster,'_')){
            std::stringstream scluster(cluster);
            std::getline(scluster,cluster_id,'/');
            std::getline(scluster,cluster_size,'/');
            uint size = std::stoi(cluster_size);
            while(std::getline(scluster,token,',')){
                template_tokens.push_back(token);
                templ_token_lenght++;
            }
        this->drain.clusters.emplace_back(template_tokens,cluster_id,size);
        node->clusters.emplace_back(template_tokens,cluster_id,size);
        }
    
    }

    return load_tree(serialized_tree);
}


void TemplateMiner::save_state(const std::string& filename, const std::string& snapshot_reason ){//fs.write(stringname.c_str(),sizeof(char)*stringname.size)

    auto fs = std::ofstream(filename, std::ios::out | std::ios::binary );
    if (!fs.is_open()) std::cout << "failed to open " << filename << '\n';
    else{
        std::cout<<"\nsave_state begins \n===================================="<<std::endl;
        
        std::stringstream ss;

        std::shared_ptr<Node> root = std::make_shared<Node>(this->drain.root_node);//copy made here
        //stay tuned to see if I can make a shared_ptr without copying all this shit
        ss<<"{";
        save_tree(ss,root);
        std::size_t bytes = sizeof(char)*ss.str().size();
        fs.write(ss.str().c_s(),bytes);
        fs.close();
        std::cout<<"\n====================================\n save_state ends"<<std::endl;
    }
   
    fs.close();
};

void TemplateMiner::load_state(const std::string& filename){
/*
Example of serialized tree to deserialize:
{ROOT;0;{3;1;{pls;2;C2/1/pls,help,me,_C3/1/pls,go,help,_}{bbb;2;C5/1/bbb,ddd,eee,_}{aaa;2;C1/2/aaa,<*>,ccc,_}}{7;1;{god;2;C4/1/god,is,dead,and,we,killed,it,_}}}
; - divides node's fields
{ - divides parent from child
/ - divides elements in LogCluster
, - divides elemnts in log_template_tokens vector
_ - divides clusters in the same node
} - go back to parent node
(between the {...} there are leafs, whereas between {...{ there are branching nodes)
depth=0                                                (ROOT)
                                                    /           \
depth=1                                           (3)           (7)
                                               /   |   \           \
depth=2                                    (pls) (bbb) (aaa)       (god) 

Node (3) has 3 childs and Node (7) only has one
Leaf (pls) has two clusters stored in it: C2 and C3, while other leafs has only one cluster stored;

*/



    std::ifstream fs(filename, std::ios::in | std::ios::binary);
    if (!fs.is_open()) std::cout << "failed to open " << filename << '\n';
    else{
        //if file is empty, skip the loading phase
        if (fs.peek() == std::ifstream::traits_type::eof()){
            std::cout<<"empty file, continuing...\n";
            return;
        }
        fs.seekg(0,fs.end);
        uint lenght = fs.tellg();
        fs.seekg(0,fs.beg);

        char* buffer = new char[lenght];
        fs.read(buffer,lenght);
        fs.close();
        std::string serialized_tree(buffer);
        delete[] buffer;
        if(serialized_tree.ends_with("\n")) {
        serialized_tree.pop_back();
        }
        load_tree(serialized_tree);
        std::cout<<"Restored "<<this->drain.clusters.size()<<" clusters with a total of "<<this->drain.get_total_cluster_size()<<" lines."<<std::endl;
        for ( auto it = std::begin(this->drain.clusters); it != std::end(this->drain.clusters); ++it){
            std::cout<<"cluster_id: "<<it->cluster_id<<", size: "<<it->size<<std::endl;
            std::cout<<"template: "<<it->get_template()<<std::endl;
        }       
    }
};

std::string TemplateMiner::get_snapshot_reason(std::string& change_type){
    if (change_type != "none") return change_type;
    std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now()-this->last_save_time;
    if (elapsed_seconds >= this->snapshot_interval_s) return "periodic";
    return "none";
};

std::vector<std::string> TemplateMiner::add_log_message(std::string& log_message){
    std::string masked_content = this->masker.mask(log_message);

    //std::cout<<"masked_content: "<<masked_content<<'\n';

    std::pair<LogCluster,std::string> bundle = this->drain.add_log_message(masked_content);

    //LogCluster cluster = bundle.first; std::string change_type = bundle.second;//NOTED
    
    //for (auto & token : cluster.log_template_tokens) std::cout<<token<<'\n';

    std::vector<std::string> result; result.reserve(5);
    result.push_back(bundle.second);
    result.push_back(bundle.first.cluster_id); 
    result.push_back(std::to_string(bundle.first.size)); 
    result.push_back(bundle.first.get_template()); 
    result.push_back(std::to_string(this->drain.get_total_cluster_size())); 
    
    std::string snapshot_reason = this->get_snapshot_reason(bundle.second);
    //std::cout<<"snapshot_reason: "<<snapshot_reason<<std::endl;
    if(snapshot_reason != "none"){
        std::cout<<"calling save_state\n";
        this->save_state("persistence.bin", snapshot_reason);
        this->last_save_time = std::chrono::system_clock::now();
    }
    return result;
};

#endif