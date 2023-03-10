#pragma once

#include <fstream>
#include <mutex>
#include <string>
#include <iostream>
#include "Node.h"

const std::string STORE_FILE = "./store/dumpFile";
const std::string DELIMITER = ":";

static std::mutex mtx;

template <typename K, typename V>
class SkipList{
    public:
     SkipList();
     SkipList(int maxLevel);
     ~SkipList();

     Node<K, V>* createNode(K, V, int);
     void displayList();
     int insertElement(const K, const V);
     bool searchElement(K, V&);
     bool deleteElement(K);

     int size();
     void dumpFile();
     void loadFile();
    
    private:
     void get_key_value_from_string(const std::string& str, std::string* key, std::string* value);
     bool is_valid_string(const std::string& str);
     int getRandomLevel();

    private:
     int _maxLevel;  //跳表的最大层数
     int _curLevel;  //当前所在的层数
     Node<K, V>* _header;  //跳表头节点的指针
     int _elementCount;   //当前的元素的个数
     std::ofstream _fileWriter;
     std::ifstream _fileReader;
};

template<typename K, typename V>
SkipList<K, V>::SkipList(){
    this->_maxLevel = 32;
    this->_curLevel = 0;
    this->_elementCount = 0;
    K k;
    V v;
    this->_header = new Node<K, V>(k, v, _maxLevel);
    loadFile();
}

template<typename K, typename V>
SkipList<K, V>::SkipList(int max_level){
    this->_maxLevel = max_level;
    this->_curLevel = 0;
    this->_elementCount = 0;
    K k;
    V v;
    this->_header = new Node<K, V>(k, v, _maxLevel);
}
template<typename K, typename V>
SkipList<K, V>::~SkipList(){
    if(_fileWriter.is_open()) _fileWriter.close();
    if(_fileReader.is_open()) _fileReader.close();
    delete _header;
}

//创建一个新节点
template<typename K, typename V>
Node<K, V>* SkipList<K, V>::createNode(const K k, const V v, int level) {
    Node<K, V> *n = new Node<K, V>(k, v, level);
    return n;
}

template<typename K, typename V>
int SkipList<K, V>::insertElement(const K key, const V value){
    mtx.lock();
    Node<K, V> *current = this->_header;  //先拿到头节点
    Node<K, V> **update = new Node<K, V>* [_maxLevel+1]();   //update数组存的是：当前层最后一个key小于我们要插入节点的key的节点。我们要将新节点插入该节点的后面。
    //memset(update, 0, sizeof(Node<K, V>*)*(_maxLevel+1));  //空间保证够，初始化

    for(int i=_curLevel;i>=0;i--){  //从当前层到0层
        //从每一层的最左侧开始遍历
        while(current->_forward[i] != nullptr && current->_forward[i]->getKey() < key) { //如果current在该层的下一个节点的_key小于要找的key，就循环
            current = current->_forward[i]; //forward存的是该节点在当前层的下一个节点
        }
        update[i] = current;//保存
    }

    //
    current = current->_forward[0]; //到达最底层，即0层，并且当前的forward指针指向第一个大于待插入节点的节点
    //如果key已经存在，就修改对应的value值
    if(current !=nullptr && current->getKey() == key){
        std::cout << "key: " << key << ", exits" << std::endl;
        current->setValue(value);
        mtx.unlock();
        return 1;
    }
    //如果key不存在，插入
    if(current == nullptr || current->getKey() != key){
        //为当前待插入节点生成随机层数
        int random_level = getRandomLevel();
        //如果随机数大于当前的层数
        if(random_level > _curLevel) {
            for(int i = _curLevel+1; i< random_level+1; i++){
                update[i] = _header;
            }
            _curLevel = random_level;
        }
        //创建节点
        Node<K, V>* insert_node = createNode(key, value, random_level);
        //插入节点
        for (int i=0; i<= random_level; i++){  //在每一层的[0, random_level)
            insert_node->_forward[i] = update[i]->_forward[i];  //新节点与后面相链接
            update[i]->_forward[i] = insert_node;   //新节点与前面相链接
        }
        _elementCount++;   //元素的总数
    }
    mtx.unlock();
    return 0;
}

//打印跳表中的所有数据-每一层都打印
template<typename K, typename V>
void SkipList<K,V>::displayList(){
    std::cout << "\n****Skip List****" << "\n" ;
    //从最底层开始打印
    for(int i=0; i<= _curLevel;i++){
        Node<K, V> *node = this->_header->_forward[i];
        std::cout << "Level " << i << ": ";
        while(node != nullptr) {
            std::cout << node->getKey() << ":" << node->getValue() << ";";
            node = node->_forward[i];
        }
        std::cout << std::endl;
    }
}

//将数据从内存写入文件
template<typename K, typename V>
void SkipList<K , V>::dumpFile() {
    std::cout << "dump file ----------------" << std::endl;
    _fileWriter.open(STORE_FILE);
    Node<K, V> *node = this->_header->_forward[0];
    //在第0层遍历
    while(node != NULL) {
        _fileWriter << node->getKey() << ":" << node->getValue() << "\n" ;
        node = node->_forward[0];
    }
    _fileWriter.flush();
    _fileWriter.close();
    return;
}

//从磁盘中加载数据
template<typename K, typename V>
void SkipList<K, V>::loadFile(){
    _fileReader.open(STORE_FILE);
    std::cout << "load file ----------------" << std::endl;
    std::string line;
    std::string *key = new std::string();
    std::string *value = new std::string();
    while( getline(_fileReader, line)) {
        get_key_value_from_string(line, key, value);
        if(key->empty() || value->empty()) {
            continue;
        }
        insertElement(*key, *value);   //执行插入函数
    }
    _fileReader.close();
}

//拿到当前跳表的节点个数
template<typename K, typename V>
int SkipList<K, V>::size(){
    return _elementCount;
}

//从文件中的一行读取key和value
template<typename K, typename V>
void SkipList<K, V>::get_key_value_from_string(const std::string& str, std::string* key, std::string* value) {
    if(!is_valid_string(str)){
        return;
    }
    *key = str.substr(0, str.find(DELIMITER));
    *value = str.substr(str.find(DELIMITER)+1, str.length());
}

//是否是有效的string
template<typename K, typename V>
bool SkipList<K, V>::is_valid_string(const std::string& str) {
    if(str.empty()){
        return false;
    }
    if(str.find(DELIMITER) == std::string::npos){
        return false;
    }
    return true;
}

//删除跳表中的元素，根据key值去跳表中查找
template<typename K, typename V>
bool SkipList<K, V>::deleteElement(K key) {
    mtx.lock();
    Node<K, V> *current = this->_header; ///拿到头节点
    Node<K, V> **update = new Node<K, V>*[_maxLevel + 1]();
    //memset(update, 0, sizeof(Node<K, V>*)*(_maxLevel+1));
    //从最高层开始，同插入函数
    for(int i=_curLevel; i>=0; i--){
        while(current->_forward[i] !=nullptr && current->_forward[i]->getKey() < key){
            current = current->_forward[i];
        }
        update[i] = current;
    }
    current = current->_forward[0]; //待删除节点
    if(current != nullptr && current->getKey() == key) {
        for(int i=0;i<=_curLevel;i++){  //从底层开始
            if(update[i]->_forward[i] != current) break;
            update[i]->_forward[i] = current->_forward[i];
        }
        delete current;//释放目标节点的内存
        //从最顶层开始往下删除空层，中间的空层删除不了
        while(_curLevel > 0 && _header->_forward[_curLevel] == nullptr){
            _curLevel --;
        }
        std::cout << "Successfully deleted key " << key << std::endl;
        _elementCount --; //更新元素的个数

        mtx.unlock();
        return true;
    }else{
        mtx.unlock();
        return false;
    }
}

template<typename K, typename V>
bool SkipList<K,V>::searchElement(K key, V& value){
    std::cout << "search element ----------------" << std::endl;
    Node<K, V> *current = _header;
    for(int i = _curLevel; i>=0; i--){
        while(current->_forward[i] != nullptr && current->_forward[i]->getKey() < key){
            current = current->_forward[i];
        }
    }
    //此时current为要搜索的点
    current = current->_forward[0];

    if(current!=nullptr && current->getKey() == key){
        value = current->getValue();
        std::cout << "Found key: " << key << ", value: " << current->getValue() << std::endl;
        return true;
    }
     std::cout << "Not Found Key:" << key << std::endl;
    return false;
}

//生成随机层数
template<typename K, typename V>
int SkipList<K,V>::getRandomLevel(){
    int k=0;
    while(rand() % 2){
        k++;
    }
    k = (k < _maxLevel) ? k:_maxLevel;
    return k;
}

