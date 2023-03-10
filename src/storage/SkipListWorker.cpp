#include <algorithm>
#include <iostream>
#include <string>
#include "SkipListWorker.h"

#define BUFFSIZE 1024
#define LEVELOFSKIPLIST 12

SkipList<std::string, std::string> SkipListWorker:: _myDB(LEVELOFSKIPLIST);
ThreadPool SkipListWorker::_readThreadPool(2);

std::unordered_map<std::string, parseState> SkipListWorker::_cmdMap = {
    {"SET", parseState::SET},
    {"GET", parseState::GET},
    {"DEL", parseState::DEL},
    {"LOAD", parseState::LOAD},
    {"DUMP", parseState::DUMP}
};

std::vector<std::function<std::string(std::vector<std::string>&)>> SkipListWorker::_handler = {
    {SkipListWorker::handlerERR},
    {SkipListWorker::handlerSET},
    {SkipListWorker::handlerGET},
    {SkipListWorker::handlerDEL},
    {SkipListWorker::handlerLOAD},
    {SkipListWorker::handlerDUMP}
};

std::string SkipListWorker::handlerERR(std::vector<std::string>& cmdList) {
    std::cout << "handlerERR" << std::endl;
    for(auto& str : cmdList){
        std::cout << str << " ";
    }
    std::cout << std::endl;
    return "error : please enter a correct command : SET/DEL/GET/LOAD/DUMP\n";
}

std::string SkipListWorker::handlerSET(std::vector<std::string>& cmdList) {
    if(cmdList.size() != 3){
        return "error : please enter a correct command : SET key value\n";
    }
    if(!_myDB.insertElement(cmdList[1], cmdList[2])) {
        return "ok : SET successfully\n";
    }else{
        return "warning : " + cmdList[1] + "is already exited\n";
    }
}

std::string SkipListWorker::handlerDEL(std::vector<std::string>& cmdList) {
    if(cmdList.size() != 2){
        return "error : please enter a correct command : DEL key\n";
    }
    if(_myDB.deleteElement(cmdList[1])) {
        return "ok : DEL successfully\n";
    }else{
        return "warning : " + cmdList[1] + " is not existed\n";
    }
}

std::string SkipListWorker::handlerGET(std::vector<std::string>& cmdList) {
    if (cmdList.size() != 2) {
        return "please enter a correct command : GET key\n";
    }
    std::string value;
    bool searchResult = _readThreadPool.Add([&](){
        return _myDB.searchElement(cmdList[1], value);
    }).get();

    if (searchResult) {
        return "ok : " + value + "\n";
    }
    else {
        return "warning : " + cmdList[1] + " is not existed\n";
    }
}

std::string SkipListWorker::handlerDUMP(std::vector<std::string>& cmdList) {
    _myDB.dumpFile();
    return "ok : dump file successfully\n";
}

std::string SkipListWorker::handlerLOAD(std::vector<std::string>& cmdList) {
    _myDB.loadFile();
    return "ok : load file successfully\n";
}

parseState SkipListWorker::parseCommand(std::string cmd, std::vector<std::string>& cmdList) {
    std::string res;
    int left=0,right=0;
    while(right < cmd.size()) {
        while(right < cmd.size() && cmd[right] == ' '){
            ++right;
        }
        left = right;
        while(right < cmd.size() && cmd[right] != ' '){
            ++right;
        }
        std::string&& str = cmd.substr(left, right - left);
        if(str.size() != 0){
            cmdList.emplace_back(str);
        }
        right ++;
    }
    std::string cmdName = cmdList.front();
    std::transform(cmdName.begin(), cmdName.end(), cmdName.begin(), toupper);
    if(_cmdMap.count(cmdName) == 0) {
        std::cout << "cmdName do not exist..." << std::endl;
        return parseState::ERR;
    }
    std::cout << "cmdList in parseCommand(2) is :" << std::endl;
    for(auto& str : cmdList){
        std::cout << str << " ";
    }
    std::cout << std::endl;
    std::cout << "cmdName in parseCommand(2) is '" << cmdName << "'" << std::endl;
    return _cmdMap[cmdName];
}

std::string SkipListWorker::dealWithCmd(std::string cmd) {
    std::vector<std::string> cmdList;
    parseState state = SkipListWorker::parseCommand(cmd, cmdList);
    return _handler[state](cmdList);
}


