#include "template_miner.hpp"
int main(int argc, char* argv[]){
    std::cout<<argc<<'\n';
    if(argc > 1){
        std::ifstream infile(argv[1]);
        TemplateMiner template_miner;

        std::string log_line; log_line.reserve(200);
        std::vector<std::string> result;

        while(std::getline(infile, log_line)){
            //if (log_line == "q" || log_line == "quit") break;
            result = template_miner.add_log_message(log_line);
            std::cout<<"change_type: "<<result.at(0)<<" on cluster: "<<result.at(1)<<" of size: "<<result.at(2)<<'\n';
            std::cout<<"Template mined: "<<result.at(3)<<'\n'<<"Total size: "<<result.at(4)<<'\n';
        }
        template_miner.drain.print_tree();
        std::string closing = "closing";
        template_miner.save_state("persistence.bin",closing);
    }
    else{
        
        TemplateMiner template_miner;

        std::string log_line; log_line.reserve(100);
        std::vector<std::string> result;

        while(true){
            std::getline(std::cin, log_line);
            if (log_line == "q" || log_line == "quit") break;
            result = template_miner.add_log_message(log_line);
            std::cout<<"change_type: "<<result.at(0)<<" on cluster: "<<result.at(1)<<" of size: "<<result.at(2)<<'\n';
            std::cout<<"Template mined: "<<result.at(3)<<'\n'<<"Total size: "<<result.at(4)<<'\n';
        }
        template_miner.drain.print_tree();
        // std::shared_ptr<Node> root = std::make_shared<Node>(template_miner.drain.root_node);//copied node
        // template_miner.save_tree(root);
        std::string closing = "closing";

        template_miner.save_state("persistence.bin",closing);
    }

}

/*
    result.push_back(change_type);
    result.push_back(cluster.cluster_id); 
    result.push_back(std::to_string(cluster.size)); 
    result.push_back(cluster.get_template()); 
    result.push_back(std::to_string(this->drain.clusters.size()));
*/