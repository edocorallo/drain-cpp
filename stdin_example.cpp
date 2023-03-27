#include "template_miner.hpp"

int main()
{
    TemplateMiner template_miner;

    std::string log_line; log_line.reserve(100);
    std::vector<std::string> result;
    while(true)
    {
        std::getline(std::cin, log_line);
        if (log_line == "q" || log_line == "quit") break;
        result = template_miner.add_log_message(log_line);
        std::cout<<std::endl<<"change_type: "<<result.at(0)<<" on cluster: "<<result.at(1)<<" of size: "<<result.at(2)<<'\n';
        std::cout<<"Template mined: "<<result.at(3)<<'\n'<<"Total size: "<<result.at(4)<<'\n';
    }
    template_miner.drain.print_tree();
    template_miner.save_state("persistence.bin",{"closing"});
}