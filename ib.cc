#include "ib.h"

#include <cstdlib>
#include <cassert>
#include <cstdio>

IbDeviceHandle::IbDeviceHandle(const char *name):
  ibv_context_(nullptr,
    int_deleter_wrapper(ibv_close_device, "ibv_close_device"))
{

  struct ibv_device **ibv_dev = ibv_get_device_list(NULL);
  struct ibv_device **current_device = ibv_dev;

  while(*current_device) {
    if(!strcmp((*current_device)->name, name)) {
      struct ibv_context *maybe_device_ctx = ibv_open_device(*current_device);
      if(maybe_device_ctx == NULL) {
        perror("ibv_open_device");
        std::abort();
      }
      
      ibv_context_.reset(maybe_device_ctx);
      ibv_free_device_list(ibv_dev); 
      break;
    }
  }
  assert(false);
}

IbPdHandle::IbPdHandle(const IbDeviceHandle& dev):
  ibv_pd_(nullptr, ibv_dealloc_pd)
{
  struct ibv_pd *pd = ibv_alloc_pd(dev.ibv_context_.get());
  if(pd == NULL) {
    perror("ibv_alloc_pd");
    std::abort();
  }
  ibv_pd_.reset(pd);
}
