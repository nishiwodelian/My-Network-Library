#include <unordered_map>

class Buffer;
class HttpResponse{
    public:
     enum HttpStatusCode{
        Unknown,
        _200Ok = 200,
        _301MovedPermanently = 301,
        _400BadRequest = 400,
        _404NotFound = 404,
     };
     explicit HttpResponse(bool close)
        : statusCode_(Unknown),
          closeConnection_(close){}
     void setStatusCode(HttpStatusCode code){
        statusCode_ = code;
     }
     void setStatusMessage(const std::string& message){
        statusMessage_ = message;
     }
     void setCloseConnection(bool on){
        closeConnection_ = on;
     }
     bool closeConnection() const {
        return closeConnection_;
     }
     void setContentType(const std::string& contentType){
        addHeader("Content-Type", contentType);
     }
     void addHeader(const std::string& key, const std::string& value){
        headers_[key] = value;
     }
     void setBody(const std::string& body){
        body_ = body;
     }
     void appendToBuffer(Buffer* output) const;

    private:
     std::unordered_map<std::string, std::string> headers_;
     HttpStatusCode statusCode_;
     std::string statusMessage_;
     bool closeConnection_;
     std::string body_;
};