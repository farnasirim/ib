#include <iostream>
#include <vector>

#include "ib.h"

int main(int argc, char **argv) {
  IbvDeviceContextByName ib_context("mlx5_1");
  IbvAllocPd pd(ib_context.get());
  return 0;
}
