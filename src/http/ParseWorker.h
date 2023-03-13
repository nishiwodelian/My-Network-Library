#include <vector>
#include <string>
#include <algorithm>

//const char CRLF[] = "\r\n";

class ParseWorker{
    public:
     ParseWorker()
        : size_(0),
          context_(),
          readerIndex_(0)
        {}

     void SetStr(std::string str){
        size_ = str.size();
        readerIndex_ = 0;
        context_.resize(str.size());
        context_.assign(str.begin(), str.end());
     }
     
     std::string Getstr(){
      std::string str(context_.data(), context_.size());
      return str;
     }

     const char* findCRLF() const
     {
        const char* crlf = std::search(peek(), end(), CRLF, CRLF+2);
        return crlf == end() ? NULL : crlf;
     }
     const char* peek() const
     {
        return begin() + readerIndex_;
     }
     void retrieveUntil(const char *end){
        size_t len = end - peek();
        if(end-begin() < size_){
            readerIndex_ += len;
        }else{
            size_ = 0;
            readerIndex_ = 0 ;
        }
     }
    
    private:
     const char* begin() const
     {
        return &(*context_.begin());
     }


     const char* end() const
     {
        return begin() + size_;
     }

     std::vector<char> context_;
     size_t readerIndex_;
     size_t size_;
     inline static const char*  CRLF  = "\r\n";
      
};

