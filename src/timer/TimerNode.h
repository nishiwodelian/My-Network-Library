

#include <functional>

class TimerNode {
    public:
     using TimerCallback = std::function<void()>;
     TimerNode(TimerCallback cb, int timeout, double interval);
     ~TimerNode() = default;
     void run() const {
        callback_();
     }

     size_t expiration() const{
        return expiration_;
     }
     void restart();
     void update(int timeout);
     bool isValid();
     void setDeleted() {
        deleted_ = true;
     }
     bool isDeleted() const {
        return deleted_;
     }
     bool repeat() const {
         return repeat_;
     }


    private:
     const TimerCallback callback_;
     size_t expiration_;
     const double interval_;
     const bool repeat_;
     bool deleted_;
};

