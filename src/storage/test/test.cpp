#include <iostream>
#include "../SkipList.h"

int main(int argv, char* argc[]){
    srand((int)time(0));
    SkipList<int, std::string> mySkipList(6);
    mySkipList.insertElement(23, "詹姆斯");
    mySkipList.insertElement(24, "科比");
    mySkipList.insertElement(13, "哈登");
    mySkipList.insertElement(3, "保罗");
    mySkipList.insertElement(30, "库里");

    mySkipList.displayList();

    

    std::cout << "SkipList's size : " << mySkipList.size() << std::endl;
    mySkipList.dumpFile();

    std::string value;
    mySkipList.searchElement(3, value);
    std::cout << "3 value : " << value << std::endl;
    mySkipList.searchElement(4, value);
    std::cout << "4 value : " << value << std::endl;
    
    mySkipList.insertElement(13, "乔治");
    mySkipList.insertElement(23, "乔丹");

    mySkipList.displayList();

    mySkipList.deleteElement(13);
    mySkipList.deleteElement(4);

    mySkipList.displayList();
}