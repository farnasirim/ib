#ifndef IB_H_
#define IB_H_
#include <memory>
#include <functional>
#include <type_traits>

#include <infiniband/verbs.h>

#include "ib_traits.h"
#include "ib_utils.h"

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

struct ibv_context *ibv_device_context_by_name_(const char *name);

using IbvDeviceContextByName = IbResource<
                      decltype(&ibv_device_context_by_name_), &ibv_device_context_by_name_,
                      decltype(&ibv_close_device), &ibv_close_device
                      >;

using IbvAllocPd = IbResource<
                      decltype(&ibv_alloc_pd), &ibv_alloc_pd,
                      decltype(&ibv_dealloc_pd), &ibv_dealloc_pd
                      >;

using IbvRegMr = IbResource<
                      decltype(&ibv_reg_mr), &ibv_reg_mr,
                      decltype(&ibv_dereg_mr), &ibv_dereg_mr
                      >;

#endif  // IB_H_
