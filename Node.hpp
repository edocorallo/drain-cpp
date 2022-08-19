#ifndef _NODE_H_
#define _NODE_H_
#include<iostream>
#include<string>
#include<unordered_map>
#include"logCluster.hpp"
class Node{
    public:
    std::string key;
    int depth;
    std::vector<LogCluster> clusters;
    std::unordered_map<std::string,std::shared_ptr<Node>> key_to_child_node;//UHHH
    

    Node(const std::string& key, const uint& depth);
    Node(const Node& node);
    Node(Node&& other) noexcept;
    Node();

    Node& operator = (const Node & other);
    Node& operator = (Node&& other);

};
// std::istream& operator >> (std::istream& is, Node& node);
// std::ostream& operator << (std::ostream& os, const Node& node);

Node::Node(const std::string& key, const uint& depth){
    this->key = key;
    this->depth = depth;
    this->clusters.reserve(10);
};

Node::Node(const Node& node){
    this->key=node.key;
    this->depth=node.depth;
    for(auto pair : node.key_to_child_node) this->key_to_child_node.insert(pair);
    //for(auto it = node.clusters.begin(); it != node.clusters.end();it++) 
    //    this->clusters=node.clusters;/*emplace_back(it->log_template_tokens, it->cluster_id, it->size)*/;
    //std::cout<<"COPIED NODE\n";
};

Node::Node(Node&& other) noexcept :
    key(std::move(other.key)),
    depth(std::move(other.depth)),
    clusters(std::move(other.clusters)),
    key_to_child_node(std::move(other.key_to_child_node))
{
    //std::cout<<"MOVED NODE\n";
};

Node::Node(){

};

Node& Node::operator = (const Node& other){
    key = other.key;
    depth = other.depth;
    clusters = other.clusters;
    key_to_child_node = other.key_to_child_node;
    //std::cout<<"NODE HAS BEEN COPY-ASSIGNED\n";
    return *this;
}

Node& Node::operator = (Node&& other){
    this->key = std::move(other.key);
    this->depth = std::move(other.depth);
    this->clusters = std::move(other.clusters);
    this->key_to_child_node = std::move(other.key_to_child_node);
    //std::cout<<"NODE HAS BEEN MOVE-ASSIGNED\n";
    return *this;
}

// std::ostream& operator << (std::ostream& os, Node& node){
//     os << node.key << node.depth << (int)node.clusters.size();
//     for (auto cluster : node.clusters) os << cluster << ";";
//     os << '\b'<<" "; //erase the last ";", insert " " to go to next "scope"
//     os << (int)node.key_to_child_node.size();
//     for (auto pair : node.key_to_child_node ) os << pair.first <<" "<< *pair.second << ",";
//     return os;
// }

// std::istream& operator >> (std::istream& is, Node& node){
//     is >> node.key;
//     is >> node.depth;
//     int clusters_size;
//     is >> clusters_size;
//     node.clusters.reserve(clusters_size);
//     std::string cluster_string;
//     while( std::getline(is, cluster_string, ';') ){
//         std::istringstream cluster_ss(cluster_string);
//         LogCluster cluster;
//         cluster_ss >> cluster;
//         node.clusters.push_back(cluster);
//     }
//     int map_size;
//     is >> map_size;
//     node.key_to_child_node.reserve(map_size);
//     std::string pair_string, box;
//     while(std::getline(is, pair_string, ',')){
//         std::istringstream pair_ss(pair_string);
//         std::string key;
//         Node value;
//         pair_ss >> key >> value;
//         node.key_to_child_node[key] = &value;
//     }
//     return is;
// }


#endif