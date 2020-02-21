#include <cstdlib>

#include <malloc.h>

#include <iostream>
#include <vector>

#include "ib.h"

int main(int argc, char **argv) {
  IbvDeviceContextByName ib_context("mlx5_1");
  IbvAllocPd pd(ib_context.get());

  size_t num_messages = 128;
  size_t msg_size = 256;
  void *mem = static_cast<char*>(memalign(4096, msg_size * num_messages));
  int flags =  IBV_ACCESS_LOCAL_WRITE |
    IBV_ACCESS_REMOTE_READ |
    IBV_ACCESS_REMOTE_WRITE;

  IbvRegMr mr(pd.get(), mem, msg_size, flags);


  return 0;
}
