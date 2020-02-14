#ifndef IB_H_
#define IB_H_

#include <memory>
#include <functional>
#include <type_traits>

#include <infiniband/verbs.h>

#include "ib_traits.h"
#include "ib_utils.h"

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

class IbMr {
 public:
  IbMr(const IbPdHandle&, void*, size_t, int);
  std::unique_ptr<struct ibv_mr, void_deleter<ibv_mr>> ibv_mr_;
};



template<typename FuncType>
struct func_traits {
  using ReturnType = struct ibv_pd*;
};

template<typename Ret, typename... Args>
struct func_traits<Ret(Args...)> {
 public:
  using ReturnType = Ret;
};

template<typename Factory, Factory f, typename Deleter, Deleter d,
  typename = typename fnp_traits<decltype(f)>::PackedArgs>
class IbResource;

template<typename Factory, Factory f, typename Deleter, Deleter d, typename... Args>
class IbResource <Factory, f, Deleter, d, Pack<Args...>> {
 public:
  using ResourceType = std::remove_pointer_t<typename fnp_traits<decltype(f)>::ReturnType>;

  std::unique_ptr<ResourceType, void_deleter<ResourceType>> ptr_;

  IbResource(Args... args): ptr_(
      factory_wrapper(f, ib_traits<Factory, f>::name())(std::forward<Args...>(args)...),
      int_deleter_wrapper(d, ib_traits<Deleter, d>::name())) {}

  ResourceType *get() const {
    return ptr_.get();
  }
};

using IbvAllocPd = IbResource<
                      decltype(&ibv_alloc_pd), &ibv_alloc_pd,
                      decltype(&ibv_dealloc_pd), &ibv_dealloc_pd
                      >;

#endif  // IB_H_
