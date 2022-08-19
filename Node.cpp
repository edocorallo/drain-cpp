#ifndef _NODE_
#define _NODE_

#include"Node.hpp"

Node::Node(const std::string& key, const uint& depth){
    this->key = key;
    this->depth = depth;
};

Node::Node(const Node& node){
    this->key=node.key;
    this->depth=node.depth;
};

Node::Node(){

};


#endif