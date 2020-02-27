#ifndef MODIFY_QP_H_
#define MODIFY_QP_H_

#include <infiniband/verbs.h>

// template<typename T>
// struct QpAttr;
// 
// template<typename ...Args>
// int modify_qp(struct ibv_qp *qp, Args... args) {
// }
// 
// struct QpAttrType {
//   static struct ibv_qp *qp;
// };
// 
// template<typename T>
// struct ss_st_name;
// template<>
// struct ss_st_name<decltype(ibv_qp_attr::ss_field_name)> {
//   static constexpr T x;
//   static constexpr decltype(ss_name_flag) flag = ss_name_flag;
// };

// IBV_QP_STATE  Modify qp_state
// 
// IBV_QP_CUR_STATE	 Set cur_qp_state
// 
// IBV_QP_EN_SQD_ASYNC_NOTIFY  Set en_sqd_async_notify
// 
// IBV_QP_ACCESS_FLAGS  Set qp_access_flags
// 
// IBV_QP_PKEY_INDEX  Set pkey_index
// 
// IBV_QP_PORT  Set port_num
// 
// IBV_QP_QKEY  Set qkey
// 
// IBV_QP_AV  Set ah_attr
// 
// IBV_QP_PATH_MTU	Set path_mtu
// 
// IBV_QP_TIMEOUT  Set timeout
// 
// IBV_QP_RETRY_CNT	 Set retry_cnt
// 
// IBV_QP_RNR_RETRY	 Set rnr_retry
// 
// IBV_QP_RQ_PSN  Set rq_psn
// 
// IBV_QP_MAX_QP_RD_ATOMIC	Set max_rd_atomic
// 
// IBV_QP_ALT_PATH	Set the alternative path via: alt_ah_attr, alt_pkey_index, alt_port_num, alt_timeout
// 
// IBV_QP_MIN_RNR_TIMER  Set min_rnr_timer
// 
// IBV_QP_SQ_PSN  Set sq_psn
// 
// IBV_QP_MAX_DEST_RD_ATOMIC  Set max_dest_rd_atomic
// 
// IBV_QP_PATH_MIG_STATE  Set path_mig_state
// 
// IBV_QP_CAP  Set cap
// 
// IBV_QP_DEST_QPN	Set dest_qp_num
// IBV_QP_RATE_LIMIT	 Set rate_limit
//


#endif  // MODIFY_QP_H_

