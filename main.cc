#include <cstdlib>
#include <cmath>
#include <thread>
#include <cassert>
#include <cstring>
#include <cstdio>

#include <errno.h>
#include <malloc.h>

#include <iostream>
#include <algorithm>
#include <vector>

#include <libmemcached/memcached.h>

#include "discovery.h"
#include "ib.h"
#include "ib_container.h"
#include "logging.h"
#include "modify_qp.h"

#include "debug_utils.h"

struct payload {
  int code;
  char msg[20];
};

int main(int argc, char **argv) {
  if(argc != 3) {
    std::cerr << "incorrect num args" << std::endl;
    exit(-1);
  }
  int machine_id = atoi(argv[1]);
  char *memcached_confstr = argv[2];

  IbvDeviceContextByName ib_context("mlx5_1");
  IbvAllocPd pd(ib_context.get());

  size_t num_messages = 128;
  size_t msg_size = 256;
  void *mem = static_cast<char *>(memalign(4096, msg_size * num_messages));
  int flags =
      IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_READ | IBV_ACCESS_REMOTE_WRITE;

  IbvRegMr mr(pd.get(), mem, msg_size, flags);

  ibv_device_attr dev_attrs = {};
  {
    int ret = ibv_query_device(ib_context.get(), &dev_attrs);
    deb(((int)dev_attrs.phys_port_cnt));
    assert(!ret);
  }

  struct ibv_port_attr port_attr = {};
  {
    int ret = ibv_query_port(ib_context.get(), 1, &port_attr);
    deb(port_attr.lid);
    assert(!ret);
  }

  std::cout << "before cq" << std::endl;

  IbvCreateCq cq(ib_context.get(), dev_attrs.max_cqe, static_cast<void *>(NULL),
                 static_cast<struct ibv_comp_channel *>(NULL), 0);
  std::cout << "after cq" << std::endl;

  {
    int ret = ibv_query_port(ib_context.get(), 1, &port_attr);
    deb(port_attr.lid);
    assert(!ret);
  }


  struct ibv_qp_init_attr qp_init_attr = {};
  qp_init_attr.send_cq = cq.get();
  qp_init_attr.recv_cq = cq.get();
  // qp_init_attr.cap.max_send_wr = static_cast<uint33_t>(dev_attrs.max_qp_wr;
  qp_init_attr.cap.max_send_wr = 1;
  // qp_init_attr.cap.max_recv_wr = static_cast<uint33_t>(dev_attrs.max_qp_wr);
  qp_init_attr.cap.max_recv_wr = 1;
  qp_init_attr.cap.max_send_sge = 1;
  qp_init_attr.cap.max_recv_sge = 1;
  // qp_init.cap.max_inline_data = 60;
  qp_init_attr.qp_type = IBV_QPT_RC;

  IbvCreateQp qp(pd.get(), &qp_init_attr);

  {
    int ret = ibv_query_port(ib_context.get(), 1, &port_attr);
    deb(port_attr.lid);
    assert(!ret);
  }


  memcached_st *memc = memcached(memcached_confstr, strlen(memcached_confstr));
  assert_p(memc != NULL, "memc");

  auto target_key = std::to_string(1 - machine_id);
  auto adv_key = std::to_string(machine_id);

  deb(adv_key);
  deb(target_key);

  auto local_qp = make_qp_info(port_attr.lid, qp.get()->qp_num);
  // auto remote_qp = exchange_qp_info(local_qp);
  auto remote_qp = exchange_qp_info(memc, adv_key.c_str(), target_key.c_str(), local_qp);

  deb(local_qp.qp_num);
  deb(local_qp.lid);
  std::cout << std::endl;
  deb(remote_qp.qp_num);
  deb(remote_qp.lid);
  std::cout << std::endl;

  {
    int ret = qp_attr::modify_qp(qp,
     qp_attr::qp_state(IBV_QPS_INIT),
     qp_attr::pkey_index(0),
     qp_attr::port_num(1),
     qp_attr::qp_access_flags(IBV_ACCESS_REMOTE_READ
       | IBV_ACCESS_REMOTE_WRITE
       | IBV_ACCESS_REMOTE_ATOMIC)
     );

    perror("init");
    assert_p(ret == 0, "init");
  }

  // struct payload recv_buffer;
  // {
  //   struct ibv_recv_wr *bad_wr = NULL;
  //   int recv_size = sizeof(recv_buffer);

  //   struct ibv_sge sge = {};
  //   sge.length = recv_size;
  //   sge.addr = reinterpret_cast<uintptr_t>(&recv_buffer);
  //   sge.lkey = mr.get()->lkey;

  //   std::uint64_t wrid = machine_id + 12345;

  //   struct ibv_recv_wr recv_wr = {};
  //   recv_wr.next = NULL;
  //   recv_wr.num_sge = 1;
  //   recv_wr.sg_list = &sge;
  //   recv_wr.wr_id = wrid;

  //   int ret = ibv_post_recv(qp.get(), &recv_wr, &bad_wr);
  //   assert_p(ret == 0, "ibv_post_recv");
  // }

  {
    struct ibv_ah_attr ah_attrs = {};
    ah_attrs.is_global = 0;
    ah_attrs.dlid = remote_qp.lid;
    ah_attrs.sl = 0;
    ah_attrs.src_path_bits = 0;
    ah_attrs.port_num = 1;

    int ret = modify_qp(qp,
        qp_attr::qp_state(IBV_QPS_RTR),
        qp_attr::path_mtu(IBV_MTU_256),
        qp_attr::dest_qp_num(remote_qp.qp_num),
        qp_attr::rq_psn(0),
        qp_attr::max_dest_rd_atomic(16),
        qp_attr::min_rnr_timer(12),
        qp_attr::ah_attr(ah_attrs)
        );

    assert_p(ret == 0, "rtr");
  }

  // {
  //   int mask = IBV_QP_STATE
  //     | IBV_QP_AV
  //     | IBV_QP_PATH_MTU
  //     | IBV_QP_DEST_QPN
  //     | IBV_QP_RQ_PSN
  //     | IBV_QP_MAX_DEST_RD_ATOMIC
  //     | IBV_QP_MIN_RNR_TIMER;

  //   std::cout << "doing rtr" << std::endl;
  //   struct ibv_qp_attr attr;

  //   attr.qp_state = IBV_QPS_RTR;
  //   attr.path_mtu = IBV_MTU_4096;
  //   attr.dest_qp_num = remote_qp.qp_num;
  //   attr.rq_psn = 0;
  //   attr.ah_attr.is_global = 0;
  //   attr.ah_attr.dlid = remote_qp.lid;
  //   attr.ah_attr.sl = 0;
  //   attr.ah_attr.src_path_bits = 0;
  //   attr.ah_attr.port_num = 1; // again, 1-based
  //   attr.max_dest_rd_atomic = 1;
  //   attr.min_rnr_timer = 12;

  //   int ret = ibv_modify_qp(qp.get(), &attr, mask);
  //   perror("rtr");
  //   assert(ret == 0);
  // }

  // prompt("rts?");
  {
    std::cout << "doing rts" << std::endl;
    struct ibv_qp_attr attr = {};
    attr.qp_state = IBV_QPS_RTS;
    attr.max_rd_atomic = 1;
    attr.retry_cnt =7;
    attr.rnr_retry = 7;
    attr.sq_psn = 0;
    attr.max_rd_atomic = 1;

    int ret = modify_qp(qp,
        qp_attr::qp_state(IBV_QPS_RTS),
        qp_attr::max_rd_atomic(1),
        qp_attr::retry_cnt(7),
        qp_attr::rnr_retry(7),
        qp_attr::sq_psn(0),
        qp_attr::timeout(0x12),
        qp_attr::max_rd_atomic(1)
      );

    assert_p(ret == 0, "rts");
  }

  std::cout << "all done" << std::endl;

  IbQueryDevice qd(ib_context.get());
  // std::cout << qd.get().max_qp << std::endl;

  auto f = []() {return 1234; };
  int a = 123;
  // constexpr auto cc = f(at, 2);


  //modify_qp(qp);

  std::this_thread::sleep_for(std::chrono::seconds(1));

  memcached_free(memc);
  return 0;
}
