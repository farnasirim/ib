#ifndef IB_H_
#define IB_H_

#include <memory>
#include <functional>

#include <infiniband/verbs.h>

template<typename T>
using void_deleter = std::function<void(T*)>;

template<typename T>
void_deleter<T> int_deleter_wrapper(int (*orig_deleter)(T*), std::string msg)
{
  return [orig_deleter, msg_capture = std::move(msg)](T *obj) -> void {
    if(orig_deleter(obj)) {
      perror(msg_capture.c_str());
    }
  };
}

class IbDeviceHandle {
 public:
  IbDeviceHandle(const char *name);
  std::unique_ptr<struct ibv_context, void_deleter<ibv_context>> ibv_context_;
};

class IbPdHandle {
 public:
  IbPdHandle(const IbDeviceHandle&);
  std::unique_ptr<struct ibv_pd, void_deleter<ibv_pd>> ibv_pd_;
};


#endif  // IB_H_
