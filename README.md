# drain-cpp
A c++ implementation of Drain3 (https://github.com/logpai/Drain3) 
"
Drain3 is an online log template miner that can extract templates (clusters) from a stream of log messages in a timely manner. It employs a parse tree with fixed depth to guide the log group search process, which effectively avoids constructing a very deep and unbalanced tree.

Drain3 continuously learns on-the-fly and extracts log templates from raw log entries.

Read more information about Drain from the following paper:

Pinjia He, Jieming Zhu, Zibin Zheng, and Michael R. Lyu. Drain: An Online Log Parsing Approach with Fixed Depth Tree, Proceedings of the 24th International Conference on Web Services (ICWS), 2017.

"

Please compile with -std=c++2a because i made wide use of modern standard language, or alternatively you can change the source code to not use that flag, couldn't care less. 


This is an exercise in futility, there is better code maintained by more experienced developers that does the same stuff this one does, but tidier. 
Seriously, go check LogPai's repository for Drain3.
