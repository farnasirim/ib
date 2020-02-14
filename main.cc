#include <iostream>
#include <vector>

#include "ib.h"

int main(int argc, char **argv) {
  IbvDeviceContextByName ib_context("ubverbs0");

  struct ibv_context *con = NULL;
  IbvAllocPd pd(con);
  struct ibv_pd *ib_pd = pd.get();

  std::cout << (ib_pd == NULL) << std::endl;
  return 0;
}
