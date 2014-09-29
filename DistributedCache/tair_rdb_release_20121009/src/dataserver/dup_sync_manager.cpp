/*
 * (C) 2007-2011 Alibaba Group Holding Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * duplicate_manager.cpp is to performe the duplicate func when copy_count > 1
 *
 * Version: $Id: dup_sync_manager.cpp 28 2011-09-19 05:18:09Z xinshu.wzx@taobao.com $
 *
 * Authors:
 *   xinshu <xinshu.wzx@taobao.com>
 *
 */
#include "dup_sync_manager.hpp"
#include "syncproc.hpp"
#include "packet_factory.hpp"
#include "packet_streamer.hpp"
#include "inc_dec_packet.hpp"
#include "storage_manager.hpp"
#include "tair_manager.hpp"
#include "remove_packet.hpp"
#include "lrpop_packet.hpp"
#include "lrpush_packet.hpp"
#include "lset_packet.hpp"
#include "hmset_packet.hpp"
#include "hset_packet.hpp"
#include "hsetnx_packet.hpp"
#include "hdel_packet.hpp"
#include "ltrim_packet.hpp"
#include "lrem_packet.hpp"
#include "sadd_packet.hpp"
#include "srem_packet.hpp"
#include "spop_packet.hpp"
#include "zadd_packet.hpp"
#include "zrem_packet.hpp"
#include "zremrangebyrank_packet.hpp"
#include "zremrangebyscore_packet.hpp"
#include "dup_put_packet.hpp"

namespace tair{

  CPacket_wait_manager::CPacket_wait_manager()
  {
    m_slots_locks = new CSlotLocks(MAP_BUCKET_SLOT);
    for (int i = 0; i < TAIR_MAX_BUCKET_NUMBER; i++)
    {
      m_bucket_count[i] = 0;
    }
  }

  CPacket_wait_manager::~CPacket_wait_manager()
  {
    if (m_slots_locks)
    {
      delete m_slots_locks;
      m_slots_locks = NULL;
    }
  }

  bool CPacket_wait_manager::isBucketFree(int bucket_number)
  {
    assert(bucket_number >= 0 && bucket_number < TAIR_MAX_BUCKET_NUMBER);
    CScopedRwLock __scoped_lock(m_slots_locks->getlock(0), false);
    return m_bucket_count[bucket_number] < MAX_BUCKET_FREE_COUNT;
  }

  int CPacket_wait_manager::changeBucketCount(int bucket_number, int number)
  {
    //dont't lock it. should lock outside
    assert(bucket_number >= 0 && bucket_number < TAIR_MAX_BUCKET_NUMBER);
    m_bucket_count[bucket_number] += number; //-1,or +1
    if (m_bucket_count[bucket_number] < 0) m_bucket_count[bucket_number] = 0;
    return 0;
  }

  int CPacket_wait_manager::get_map_index(uint32_t max_packet_id)
  {
    return 0; //must be zero,bucause of number indictor.
    //int index=max_packet_id%MAP_BUCKET_SLOT;
    //assert(index>= 0 && index<MAP_BUCKET_SLOT);
    //return index;
  }

  //the index must 0-2,already checked.
  bool CPacket_wait_manager::put_timeout_hint(int index, CPacket_Timeout_hint *phint)
  {
    return dup_wait_queue[index].put(phint);
  }

  CPacket_Timeout_hint *CPacket_wait_manager::get_timeout_hint(int index, int msec)
  {
    return dup_wait_queue[index].get(msec);
  }

  int CPacket_wait_manager::addWaitNode(CPacket_wait_Nodes* node, int bucket_number, uint32_t max_packet_id) {
    //check map_size first
    int index = get_map_index(max_packet_id);
    {
      //CRwLock m_lock(m_mutex,RLOCK);
      CScopedRwLock __scoped_lock(m_slots_locks->getlock(index), false);
      if(m_PkgWaitMap[index].size() > TAIR_MAX_DUP_MAP_SIZE) return TAIR_RETURN_DUPLICATE_BUSY;
    }

    //CRwLock m_lock(m_mutex,WLOCK);
    CScopedRwLock __scoped_lock(m_slots_locks->getlock(index), true);
    CDuplicatPkgMapIter itr = m_PkgWaitMap[index].find(max_packet_id);
    if (itr == m_PkgWaitMap[index].end())
    {
      //not found in map .inert it and inster to a queue.
      changeBucketCount(bucket_number, 1);
      m_PkgWaitMap[index][max_packet_id] = node;
      return TAIR_RETURN_SUCCESS;
    }
    else
    {
      //should never happen,but if crash and restared, nay mixed.
      log_error("packet sequnce id is dup");
      return TAIR_RETURN_DUPLICATE_IDMIXED;
    }
  }

  int CPacket_wait_manager::addWaitNode(int area, const data_entry*, const data_entry* value, int bucket_number,
      const vector<uint64_t>& des_server_ids, base_packet* request, uint32_t max_packet_id, int &version)
  {
    //check map_size first
    int index = get_map_index(max_packet_id);
    {
      //CRwLock m_lock(m_mutex,RLOCK);
      CScopedRwLock __scoped_lock(m_slots_locks->getlock(index), false);
      if(m_PkgWaitMap[index].size() > TAIR_MAX_DUP_MAP_SIZE) return TAIR_RETURN_DUPLICATE_BUSY;
    }

    //CRwLock m_lock(m_mutex,WLOCK);
    CScopedRwLock __scoped_lock(m_slots_locks->getlock(index), true);
    CDuplicatPkgMapIter itr = m_PkgWaitMap[index].find(max_packet_id);
    if (itr == m_PkgWaitMap[index].end())
    {
      //not found in map .inert it and inster to a queue.
      CPacket_wait_Nodes *pdelaysign = new CPacket_wait_Nodes(bucket_number, request, des_server_ids, version, value);
      changeBucketCount(bucket_number, 1);
      m_PkgWaitMap[index][max_packet_id] = pdelaysign;
      return TAIR_RETURN_SUCCESS;
    }
    else
    {
      //should never happen,but if crash and restared, nay mixed.
      log_error("packet sequnce id is dup");
      return TAIR_RETURN_DUPLICATE_IDMIXED;
    }
  }

  int CPacket_wait_manager::doResponse(int bucket_number, uint64_t des_server_id, uint32_t max_packet_id,
      struct CPacket_wait_Nodes **ppNode)
  {
    //CRwLock m_lock(m_mutex,WLOCK);
    int index = get_map_index(max_packet_id);
    CScopedRwLock __scoped_lock(m_slots_locks->getlock(index), true);

    CDuplicatPkgMapIter itr = m_PkgWaitMap[index].find(max_packet_id);
    if (itr != m_PkgWaitMap[index].end())
    {
      int ret=itr->second->do_response(bucket_number,des_server_id);
      if (0 == ret)
      {
        changeBucketCount(bucket_number, -1);
        *ppNode= itr->second;
        m_PkgWaitMap[index].erase(itr);
      }
      else
      {
        *ppNode= NULL;
      }

      // at this point, we duplicate successfully and need record this key/value (rsync_manager->add_record(xx))
      // for remote sync, unfortunately, current duplicate_manager doest't maintain neccessary
      // context of request, so we can do noting but ignoring now.
      // Howerver, storage manager who just uses its own binlog can rest easy now, because rsyc_manager->add_record(xx)
      // is meaningless for it actually.
      // TODO: reconstruct duplicate_manager thoroughly.
      return ret;
    }
    else
    {
      //already timeout.
      log_warn("resonse packet %u, but not found", max_packet_id);
	    *ppNode= NULL;
      return TAIR_RETURN_DUPLICATE_DELAY;
    }
  }

  int CPacket_wait_manager::doTimeout( uint32_t max_packet_id)
  {
    //CRwLock m_lock(m_mutex,WLOCK);
    int index = get_map_index(max_packet_id);
    {
      CScopedRwLock __scoped_lock(m_slots_locks->getlock(index), false);
      CDuplicatPkgMapIter itr = m_PkgWaitMap[index].find(max_packet_id);
      if (itr == m_PkgWaitMap[index].end()) return 0;
    }
    //now we should clear it.
    clear_waitnode(max_packet_id);
    return TAIR_RETURN_DUPLICATE_ACK_TIMEOUT;
  }

  int CPacket_wait_manager::clear_waitnode( uint32_t max_packet_id)
  {
    int index=get_map_index(max_packet_id);
    CScopedRwLock __scoped_lock(m_slots_locks->getlock(index), true);

    CDuplicatPkgMapIter itr = m_PkgWaitMap[index].find(max_packet_id);
    if (itr != m_PkgWaitMap[index].end())
    {
      CPacket_wait_Nodes* pNode = itr->second;
      changeBucketCount(pNode->bucket_number, -1);
      m_PkgWaitMap[index].erase(itr);
      delete pNode; pNode = NULL;
      return 0;
    }
    else
    {
	    log_error("clear_waitnode node but not found,packet=%d",max_packet_id);
    }
    return 0;
  }

  dup_sync_sender_manager::dup_sync_sender_manager( tbnet::Transport *transport,
      tair_packet_streamer *streamer, tair_manager* tair_mgr)
  {
    this->tair_mgr = tair_mgr;
    conn_mgr = new tbnet::ConnectionManager(transport, streamer, this);
    conn_mgr->setDefaultQueueTimeout(0 , MISECONDS_BEFOR_SEND_RETRY/2000);
    conn_mgr->setDefaultQueueLimit(0, 5000);
    max_queue_size = 0;
    atomic_set(&packet_id_creater, 0);
    setThreadCount(MAX_DUP_COUNT);
    //this->start();
  }

  dup_sync_sender_manager::~dup_sync_sender_manager()
  {
    this->stop();
    this->wait();
    delete conn_mgr;
  }

  void dup_sync_sender_manager::do_hash_table_changed()
  {
    return ;
  }

  bool dup_sync_sender_manager::is_bucket_available(uint32_t bucket_number)
  {
    return true;
  }

  //xinshu. add new function for directy duplicate.
  int dup_sync_sender_manager::duplicate_data(int area, const data_entry* key, const data_entry* value, int expire_time,
      int bucket_number, const vector<uint64_t>& des_server_ids, base_packet *request, int version)
  {
    if (des_server_ids.empty()) return 0;

    if (!request) return 0;
    //first keep it in queue. until timeout it.
    unsigned int _copy_count = des_server_ids.size();
    uint32_t max_packet_id = atomic_add_return(_copy_count, &packet_id_creater);

    //and keep it in map . until response arrive.
    log_debug("wait bucket %d, packet %d, timeout=%d", bucket_number, max_packet_id, expire_time);
    int ret = packets_mgr.addWaitNode(area, key, value, bucket_number, des_server_ids, request, max_packet_id, version);
    if (ret!=0)
    {
      log_error("addWaitNode error, ret=%d\n",ret);
      return ret;
    }
    ret = direct_send(area, key, value, 0, bucket_number, des_server_ids, max_packet_id);
    if (TAIR_RETURN_SUCCESS != ret)
    {
      //clear waitnode.
      packets_mgr.clear_waitnode(max_packet_id);
      //return failed,the caller will delete the packet.
      return ret;
    }

    return TAIR_DUP_WAIT_RSP;
  }

  int dup_sync_sender_manager::direct_send(int area, const data_entry* key, const data_entry* value, int expire_time,
      int bucket_number, const vector<uint64_t>& des_server_ids, uint32_t max_packet_id)
  {
    unsigned _copy_count = des_server_ids.size();
    //? what if `max_packet_id` is rounded up to 0, up there in `duplicate_data`
    if (0 == max_packet_id)
      max_packet_id = atomic_add_return(_copy_count, &packet_id_creater);

    for(unsigned int i = 0; i < _copy_count; i++)
    {
      //new a dup packet
      request_duplicate* tmp_packet = new request_duplicate();
      tmp_packet->packet_id = max_packet_id;
      tmp_packet->area = area;
      tmp_packet->key = *key;
      tmp_packet->key.server_flag = TAIR_SERVERFLAG_DUPLICATE;
      if (tmp_packet->key.is_alloc() == false) {
        tmp_packet->key.set_data(key->get_data(), key->get_size(), true);
      }
      if (value) {
        tmp_packet->data = *value;
        if (tmp_packet->data.is_alloc() == false) {
          tmp_packet->data.set_data(value->get_data(), value->get_size(), true);
        }
      } else { //remove
        tmp_packet->data.data_meta.flag = TAIR_ITEM_FLAG_DELETED;
      }
      tmp_packet->bucket_number = bucket_number;

      //and send it to slave
      log_debug("duplicate packet %d sent: %s", tmp_packet->packet_id,tbsys::CNetUtil::addrToString(des_server_ids[i]).c_str());
      if (conn_mgr->sendPacket(des_server_ids[i], tmp_packet, NULL, (void*)((long)max_packet_id), true) == false)
      {
        //duplicate sendpacket error.
        log_error("duplicate packet %d faile send: %s",
            tmp_packet->packet_id,tbsys::CNetUtil::addrToString(des_server_ids[i]).c_str());
        delete tmp_packet;
        return TAIR_RETURN_DUPLICATE_BUSY;
      }
    }
    return TAIR_RETURN_SUCCESS;
  }

  /*
  int dup_sync_sender_manager::duplicate_data(int32_t bucket_number, request_prefix_puts *request, vector<uint64_t> &slaves, int version)
  {
    return do_duplicate_data(bucket_number, request, slaves, version);
  }

  int dup_sync_sender_manager::duplicate_data(int32_t bucket_number, request_prefix_removes *request, vector<uint64_t> &slaves, int version)
  {
    return do_duplicate_data(bucket_number, request, slaves, version);
  }

  int dup_sync_sender_manager::duplicate_data(int32_t bucket_number, request_prefix_hides *request, vector<uint64_t> &slaves, int version)
  {
    return do_duplicate_data(bucket_number, request, slaves, version);
  }

  int dup_sync_sender_manager::duplicate_data(int32_t bucket_number, request_prefix_incdec *request, vector<uint64_t> &slaves, int version)
  {
    return do_duplicate_data(bucket_number, request, slaves, version);
  }
  */

  /*
  int dup_sync_sender_manager::duplicate_batch_data(int bucket_number, const mput_record_vec* record_vec,
                                                    const std::vector<uint64_t>& des_server_ids, base_packet *request,
                                                    int version)
  {
    unsigned _copy_count=des_server_ids.size();
    uint32_t max_packet_id = atomic_add_return(_copy_count,&packet_id_creater);

    if(0==max_packet_id)
      max_packet_id = atomic_add_return(_copy_count,&packet_id_creater);

    int ret=packets_mgr.addWaitNode(0,NULL,NULL,bucket_number,des_server_ids,request,max_packet_id,version);
    if(ret != TAIR_RETURN_SUCCESS)
    {
      return ret;
    }

    for(unsigned int  i=0;i < _copy_count; i++)
    {
      request_mput* tmp_packet = new request_mput();
      // just reuse original request's data, if necessray
      if (_copy_count < 2) {
        tmp_packet->swap(*(dynamic_cast<request_mput*>(request)));
      } else {
        tmp_packet->clone(*(dynamic_cast<request_mput*>(request)), true);
      }
      tmp_packet->server_flag = TAIR_SERVERFLAG_DUPLICATE;
      tmp_packet->packet_id = max_packet_id;
      // reset the compressed data as server_flag changed
      if (tmp_packet->compressed != 0) {
        tmp_packet->compressed = 0;
        if (tmp_packet->packet_data != NULL) {
          delete tmp_packet->packet_data;
        }
        tmp_packet->compress();
      }
      //and send it to slave
      log_debug("duplicate packet %d sent: %s",tmp_packet->packet_id,tbsys::CNetUtil::addrToString(des_server_ids[i]).c_str());
      if(conn_mgr->sendPacket(des_server_ids[i], tmp_packet, NULL, (void*)((long)max_packet_id), true) == false)
      {
        //duplicate sendpacket error.
        log_error("duplicate packet %d faile send: %s",tmp_packet->packet_id,tbsys::CNetUtil::addrToString(des_server_ids[i]).c_str());
        delete tmp_packet;
        ret = TAIR_RETURN_DUPLICATE_BUSY;
        break;
      }
    }

    if (TAIR_RETURN_SUCCESS != ret)
    {
      //clear waitnode.
      packets_mgr.clear_waitnode(max_packet_id);
      //return failed,the caller will delete the packet.
      return ret;
    }

    return TAIR_DUP_WAIT_RSP;
  }
  */

  bool dup_sync_sender_manager::has_bucket_duplicate_done(int bucketNumber)
  {
    /**
      while(packages_mgr_mutex.rdlock()) {
      usleep(10);
      }
      map<uint32_t, bucket_waiting_queue>::iterator it = packets_mgr.find(bucketNumber);
      bool res = (it == packets_mgr.end() || it->second.size() == 0 ) ;
      packages_mgr_mutex.unlock();
      return res;
     **/
    return packets_mgr.isBucketFree(bucketNumber);
  }

  int dup_sync_sender_manager::do_duplicate_response(uint32_t bucket_number, uint64_t d_serverId, uint32_t packet_id)
  {
	   CPacket_wait_Nodes *pNode=NULL;
    int ret = packets_mgr.doResponse(bucket_number, d_serverId, packet_id,  &pNode);
    //if all rsp arrive ,resp to client. or get a TAIR_RETURN_DUPLICATE_ACK_WAIT.
    if (0 == ret && pNode)
    {
      ret = rspPacket(pNode);
	    delete (pNode);(pNode)=NULL;
    }
    return ret;
  }

  int dup_sync_sender_manager::rspPacket(const CPacket_wait_Nodes * pNode)
  {
    int rv = 0;
    log_debug("@@ dup sync return %d", pNode->pcode);
    switch  (pNode->pcode)
    {
      case TAIR_REQ_REMOVE_PACKET:
        rv = tair_packet_factory::set_return_packet(pNode->conn,pNode->chid,pNode->pcode,pNode->ret1,"",pNode->conf_version);
        break;
      case TAIR_REQ_PUT_PACKET:
      case TAIR_REQ_LOCK_PACKET:
      case TAIR_REQ_MPUT_PACKET:
      case TAIR_REQ_EXPIRE_PACKET:
          
      case TAIR_REQ_PUTNX_PACKET:
        rv = tair_packet_factory::set_return_packet(pNode->conn,pNode->chid,pNode->pcode,0,"",pNode->conf_version);
        break;
      case TAIR_REQ_LSET_PACKET:
        {
            response_lset *resp = new response_lset();
            resp->setChannelId(pNode->chid);
            resp->config_version = pNode->conf_version;
            resp->set_code(0);
            if (!pNode->conn->postPacket(resp)) {
                delete resp;
                rv = TAIR_RETURN_DUPLICATE_SEND_ERROR;
            }
        }
        break;
      case TAIR_REQ_LTRIM_PACKET:
        {
            response_ltrim *resp = new response_ltrim();
            resp->setChannelId(pNode->chid);
            resp->config_version = pNode->conf_version;
            resp->set_code(0);
            if (!pNode->conn->postPacket(resp)) {
                delete resp;
                rv = TAIR_RETURN_DUPLICATE_SEND_ERROR;
            }
        }
        break;
      case TAIR_REQ_LREM_PACKET:
        {
            response_lrem *resp = new response_lrem();
            resp->setChannelId(pNode->chid);
            resp->config_version = pNode->conf_version;
            resp->code = 0;
            if (!pNode->conn->postPacket(resp)) {
                delete resp;
                rv = TAIR_RETURN_DUPLICATE_SEND_ERROR;
            }
        }
        break;
      case TAIR_REQ_HMSET_PACKET:
        {
            response_hmset *resp = new response_hmset();
            resp->setChannelId(pNode->chid);
            resp->config_version = pNode->conf_version;
            resp->set_code(pNode->ret1);
            if (!pNode->conn->postPacket(resp)) {
                delete resp;
                rv = TAIR_RETURN_DUPLICATE_SEND_ERROR;
            }
        }
        break;
      case TAIR_REQ_HSET_PACKET:
        {
            response_hset *resp = new response_hset();
            resp->setChannelId(pNode->chid);
            resp->config_version = pNode->conf_version;
            resp->set_code(pNode->ret1);
            if (!pNode->conn->postPacket(resp)) {
                delete resp;
                rv = TAIR_RETURN_DUPLICATE_SEND_ERROR;
            }
        }
        break;
      case TAIR_REQ_HSETNX_PACKET:
        {
            response_ltrim *resp = new response_ltrim();
            resp->setChannelId(pNode->chid);
            resp->config_version = pNode->conf_version;
            resp->set_code(0);
            if (!pNode->conn->postPacket(resp)) {
                delete resp;
                rv = TAIR_RETURN_DUPLICATE_SEND_ERROR;
            }
        }
        break;
      case TAIR_REQ_HDEL_PACKET:
        {
            response_hdel *resp = new response_hdel();
            resp->setChannelId(pNode->chid);
            resp->config_version = pNode->conf_version;
            resp->set_code(0);
            if (!pNode->conn->postPacket(resp)) {
                delete resp;
                rv = TAIR_RETURN_DUPLICATE_SEND_ERROR;
            }
        }
        break;
      case TAIR_REQ_SPOP_PACKET:
        {
            response_spop *resp = new response_spop();
            resp->setChannelId(pNode->chid);
            resp->config_version = pNode->conf_version;
            resp->set_code(0);
            if (!pNode->conn->postPacket(resp)) {
                delete resp;
                rv = TAIR_RETURN_DUPLICATE_SEND_ERROR;
            }
        }
        break;
      case TAIR_REQ_SREMMULTI_PACKET:
      case TAIR_REQ_SREM_PACKET:
        {
            response_srem *resp = new response_srem();
            resp->setChannelId(pNode->chid);
            resp->config_version = pNode->conf_version;
            resp->code = 0;
            if (!pNode->conn->postPacket(resp)) {
                delete resp;
                rv = TAIR_RETURN_DUPLICATE_SEND_ERROR;
            }
        }
        break;
      case TAIR_REQ_SADD_PACKET:
      case TAIR_REQ_SADDMULTI_PACKET:
        {
            response_sadd *resp = new response_sadd();
            resp->setChannelId(pNode->chid);
            resp->config_version = pNode->conf_version;
            resp->set_code(pNode->ret1);
            if (!pNode->conn->postPacket(resp)) {
                delete resp;
                rv = TAIR_RETURN_DUPLICATE_SEND_ERROR;
            }
        }
        break;
      case TAIR_REQ_ZADD_PACKET:
        {
            response_zadd *resp = new response_zadd();
            resp->setChannelId(pNode->chid);
            resp->config_version = pNode->conf_version;
            resp->set_code(pNode->ret1);
            if (!pNode->conn->postPacket(resp)) {
                delete resp;
                rv = TAIR_RETURN_DUPLICATE_SEND_ERROR;
            }
        }
        break;
      case TAIR_REQ_ZREM_PACKET:
        {
            response_zrem *resp = new response_zrem();
            resp->setChannelId(pNode->chid);
            resp->config_version = pNode->conf_version;
            resp->set_code(0);
            if (!pNode->conn->postPacket(resp)) {
                delete resp;
                rv = TAIR_RETURN_DUPLICATE_SEND_ERROR;
            }
        }
        break;
      case TAIR_REQ_ZREMRANGEBYRANK_PACKET:
        {
            response_zremrangebyrank *resp = new response_zremrangebyrank();
            resp->setChannelId(pNode->chid);
            resp->config_version = pNode->conf_version;
            resp->set_code(0);
            resp->retnum = pNode->ret1;
            if (!pNode->conn->postPacket(resp)) {
                delete resp;
                rv = TAIR_RETURN_DUPLICATE_SEND_ERROR;
            }
        }
        break;
      case TAIR_REQ_ZREMRANGEBYSCORE_PACKET:
        {
            response_zremrangebyscore *resp = new response_zremrangebyscore();
            resp->setChannelId(pNode->chid);
            resp->config_version = pNode->conf_version;
            resp->retnum = pNode->ret1;
            resp->set_code(0);
            if (!pNode->conn->postPacket(resp)) {
                delete resp;
                rv = TAIR_RETURN_DUPLICATE_SEND_ERROR;
            }
        }
        break;
      case TAIR_REQ_LPUSH_LIMIT_PACKET:
      case TAIR_REQ_RPUSH_LIMIT_PACKET:
      case TAIR_REQ_LPUSHX_LIMIT_PACKET:
      case TAIR_REQ_RPUSHX_LIMIT_PACKET:
        {
          log_debug("start to create response");
          response_lrpush *resp = NULL;
          if (pNode->pcode == TAIR_REQ_LPUSH_LIMIT_PACKET){
                  resp = new response_lrpush(TAIR_RESP_LPUSH_PACKET); 
          }
          else if (pNode->pcode == TAIR_REQ_RPUSH_LIMIT_PACKET) {
                  resp = new response_lrpush(TAIR_RESP_RPUSH_PACKET); 
          }
          else if (pNode->pcode == TAIR_REQ_LPUSHX_LIMIT_PACKET ) {
                  resp = new response_lrpush(TAIR_RESP_LPUSHX_PACKET);
          }
          else { //TAIR_REQ_RPUSHX_LIMIT_PACKET:
                  resp = new response_lrpush(TAIR_RESP_RPUSHX_PACKET);
          }
          resp->setChannelId(pNode->chid);
          resp->config_version = pNode->conf_version;
          resp->code = 0;
          resp->pushed_num = pNode->ret1;
          resp->list_len = pNode->ret2;
          log_debug("resp pcode:%d config_version:%d code:%d chid:%d",
                  resp->getPCode(), resp->config_version, resp->code, resp->getChannelId());
          if (pNode->conn->postPacket(resp) == false)
          {
              log_error("post packet failed");
            delete resp;
            rv = TAIR_RETURN_DUPLICATE_SEND_ERROR;
          }
          else {
              log_debug("post packet success");
          }
        }
        break;

      case TAIR_REQ_LPOP_PACKET:
      case TAIR_REQ_RPOP_PACKET:
        {
            log_debug("start to create response");
            response_lrpop *resp = NULL;
            if (TAIR_REQ_LPOP_PACKET == pNode->pcode) {
                resp = new response_lrpop(TAIR_RESP_LPOP_PACKET);
            }else {
                resp = new response_lrpop(TAIR_RESP_RPOP_PACKET);
            }
            resp->setChannelId(pNode->chid);
            resp->config_version = pNode->conf_version;
            resp->code = pNode->ret1;
            resp->values = pNode->ret_values;
            log_debug("resp pcode:%d config_version:%d code:%d chid:%d",
                    resp->getPCode(), resp->config_version, resp->code, resp->getChannelId());
            if (pNode->conn->postPacket(resp) == false) {
                log_debug("post packet failed");
              delete resp;
              rv = TAIR_RETURN_DUPLICATE_SEND_ERROR;
            }
            else {
                log_debug("post packet success");
            }
        }
        break;

      case TAIR_REQ_INCDEC_PACKET:
        {
          response_inc_dec *resp = new response_inc_dec();
          resp->value = pNode->inc_value_result ;
          resp->config_version = pNode->conf_version;
          resp->setChannelId(pNode->chid);
          if (pNode->conn->postPacket(resp) == false)
          {
            delete resp;
            rv = TAIR_RETURN_DUPLICATE_SEND_ERROR;
          }
        }
        break;
      case TAIR_REQ_LPUSH_PACKET:
      case TAIR_REQ_RPUSH_PACKET:
      case TAIR_REQ_LPUSHX_PACKET:
      case TAIR_REQ_RPUSHX_PACKET:
        {
          log_debug("start to create response");
          response_lrpush *resp = NULL;
          if (pNode->pcode == TAIR_REQ_LPUSH_PACKET){
                  resp = new response_lrpush(TAIR_RESP_LPUSH_PACKET); 
          }
          else if (pNode->pcode == TAIR_REQ_RPUSH_PACKET) {
                  resp = new response_lrpush(TAIR_RESP_RPUSH_PACKET); 
          }
          else if (pNode->pcode == TAIR_REQ_LPUSHX_PACKET ) {
                  resp = new response_lrpush(TAIR_RESP_LPUSHX_PACKET);
          }
          else { //TAIR_REQ_RPUSHX_PACKET:
                  resp = new response_lrpush(TAIR_RESP_RPUSHX_PACKET);
          }
          resp->setChannelId(pNode->chid);
          resp->config_version = pNode->conf_version;
          resp->code = pNode->inc_value_result;
          resp->pushed_num = pNode->ret1;
          resp->list_len = pNode->ret2;
          log_debug("resp pcode:%d config_version:%d code:%d chid:%d",
                  resp->getPCode(), resp->config_version, resp->code, resp->getChannelId());
          if (pNode->conn->postPacket(resp) == false)
          {
              log_error("post packet failed");
            delete resp;
            rv = TAIR_RETURN_DUPLICATE_SEND_ERROR;
          }
          else {
              log_debug("post packet success");
          }
        }
        break;
      default:
        log_warn("unknow handle error! get pcode =%d!", pNode->pcode);
        //rv=tair_packet_factory::set_return_packet(request, 0, "", conf_version);
        break;
    };
    return rv;
  }

  void dup_sync_sender_manager::run(tbsys::CThread *thread, void *arg)
  {
    UNUSED(thread);
    UNUSED(arg);
    return;
    while (!_stop)
    {
      try
      {
        //int index = (long)arg;//if need more thread,shoudld hash it.
      }
      catch(...)
      {
        log_warn("unknow error! get timeoutqueue error!");
      }
    }
  }

  void dup_sync_sender_manager::handleTimeOutPacket(uint32_t packet_id)
  {
    //not exist or timeout.
    packets_mgr.doTimeout(packet_id);
    //todo:if timeout,should response to client or rollback?

    //now i should free the request,not need convert to child class.
  }


  tbnet::IPacketHandler::HPRetCode dup_sync_sender_manager::handlePacket(tbnet::Packet *packet, void *args)
  {
	  int packet_id = reinterpret_cast<long>(args);
    if (!packet->isRegularPacket()) {
      tbnet::ControlPacket *cp = (tbnet::ControlPacket*)packet;
			log_error("cmd:%d,packetid:%d,timeout.", cp->getCommand(),packet_id);
		  handleTimeOutPacket(packet_id);
      return tbnet::IPacketHandler::FREE_CHANNEL;
    }
    int pcode = packet->getPCode();
    log_debug("================= get duplicate response, pcode: %d", pcode);
    if (TAIR_RESP_DUPLICATE_PACKET == pcode) {
      response_duplicate* resp = (response_duplicate*)packet;
      log_debug("response packet %d ,bucket =%d, server=%s",resp->packet_id,resp->bucket_id, \
          tbsys::CNetUtil::addrToString(resp->server_id).c_str());
      int ret = do_duplicate_response(resp->bucket_id, resp->server_id, resp->packet_id);
      if (0 != ret && TAIR_RETURN_DUPLICATE_ACK_WAIT != ret)
      {
        log_error("response packet %d failed,bucket =%d, server=%s, code=%d", resp->packet_id, resp->bucket_id, \
            tbsys::CNetUtil::addrToString(resp->server_id).c_str(), ret);
      }

      resp->free();
    } 

    else if ( pcode == TAIR_RESP_MRETURN_DUP_PACKET) {
      response_mreturn_dup *resp_dup = dynamic_cast<response_mreturn_dup*>(packet);
      if (resp_dup == NULL) {
        log_error("bad packet %d", pcode);
      } else {
        log_debug("duplicate response packet %u, bucket = %d, server = %s", resp_dup->packet_id, resp_dup->bucket_id, tbsys::CNetUtil::addrToString(resp_dup->server_id).c_str());
        CPacket_wait_Nodes *pnode = NULL;
        int ret = packets_mgr.doResponse(resp_dup->bucket_id, resp_dup->server_id, resp_dup->packet_id, &pnode);
        response_mreturn *resp = new response_mreturn();
        resp->swap(*resp_dup);
        if (ret == TAIR_RETURN_SUCCESS && pnode != NULL) {
          resp->setChannelId(pnode->chid);
          resp->config_version = pnode->conf_version;
          if (pnode->conn->postPacket(resp) == false) {
            delete resp;
            ret = TAIR_RETURN_DUPLICATE_SEND_ERROR;
          }
        } else {
          delete resp;
        }
      }
    } 

    /*
    else if (pcode == TAIR_RESP_PREFIX_INCDEC_PACKET) {
      response_prefix_incdec *resp = dynamic_cast<response_prefix_incdec*>(packet);
      if (resp == NULL) {
        log_error("bad packet %d", pcode);
      } else {
        log_debug("duplicate response packet %u, bucket = %d, server = %s",
            resp->packet_id, resp->bucket_id, tbsys::CNetUtil::addrToString(resp->server_id).c_str());
        CPacket_wait_Nodes *pnode = NULL;
        int ret = packets_mgr.doResponse(resp->bucket_id, resp->server_id, resp->packet_id, &pnode);
        if (ret == TAIR_RETURN_SUCCESS && pnode != NULL) {
          resp->server_flag = TAIR_SERVERFLAG_CLIENT;
          resp->setChannelId(pnode->chid);
          resp->config_version = pnode->conf_version;
          if (!pnode->conn->postPacket(resp)) {
            delete resp;
            ret = TAIR_RETURN_DUPLICATE_SEND_ERROR;
          }
        }
      }
    } 
    */
    else {
      log_warn("=====> unknow packet! pcode: %d", pcode);
      packet->free();
    }

    return tbnet::IPacketHandler::KEEP_CHANNEL;
  }
    
  int dup_sync_sender_manager::duplicate_data(request_put *request, int version) {
      log_info("dup_sync_send_manager::duplicate_data");
      int bucket_number = tair_mgr->get_bucket_number(request->key);
      vector<uint64_t> slaves;
      tair_mgr->get_slaves(request->key.server_flag, bucket_number, slaves);
      size_t copy_count = slaves.size();
      uint32_t packet_id = atomic_add_return(copy_count, &packet_id_creater);
      if (packet_id == 0) {
          atomic_add_return(copy_count, &packet_id_creater);
      }

      int ret = packets_mgr.addWaitNode(0, NULL, NULL, bucket_number, slaves, request, packet_id, version);
      if (ret != 0) {
          return ret;
      }

      for (size_t i = 0; i < copy_count; ++i) {
          request_put_dup* packet = new request_put_dup(*request);
          packet->packet_id = packet_id;
          packet->bucket_id = bucket_number;
          uint64_t des_svr = slaves[i];
          std::string ip_str = tbsys::CNetUtil::addrToString(des_svr);
          log_info("duplicate packet %u to %s", packet_id, ip_str.c_str());
          if (conn_mgr->sendPacket(des_svr, packet, NULL, (void*)((long)packet_id), true) == false) {
              log_error("duplicate packet %u to %s busy", packet_id, ip_str.c_str());
              delete packet;
              packets_mgr.clear_waitnode(packet_id);
              ret = TAIR_RETURN_DUPLICATE_BUSY;
              return ret;
          }
      }

      return TAIR_DUP_WAIT_RSP;
  }

  int dup_sync_sender_manager::duplicate_data(request_putnx *request, int version) {
      log_info("dup_sync_send_manager::duplicate_data");
      int bucket_number = tair_mgr->get_bucket_number(request->key);
      vector<uint64_t> slaves;
      tair_mgr->get_slaves(request->key.server_flag, bucket_number, slaves);
      size_t copy_count = slaves.size();
      uint32_t packet_id = atomic_add_return(copy_count, &packet_id_creater);
      if (packet_id == 0) {
          atomic_add_return(copy_count, &packet_id_creater);
      }   

      int ret = packets_mgr.addWaitNode(0, NULL, NULL, bucket_number, slaves, request, packet_id, version);
      if (ret != 0) {
          return ret;
      }   

      for (size_t i = 0; i < copy_count; ++i) {
          request_putnx_dup* packet = new request_putnx_dup(*request);
          packet->packet_id = packet_id;
          packet->bucket_id = bucket_number;
          uint64_t des_svr = slaves[i];
          std::string ip_str = tbsys::CNetUtil::addrToString(des_svr);
          log_info("duplicate packet %u to %s", packet_id, ip_str.c_str());
          if (conn_mgr->sendPacket(des_svr, packet, NULL, (void*)((long)packet_id), true) == false) {
              log_error("duplicate packet %u to %s busy", packet_id, ip_str.c_str());
              delete packet;
              packets_mgr.clear_waitnode(packet_id);
              ret = TAIR_RETURN_DUPLICATE_BUSY;
              return ret;
          }   
      }   
      return TAIR_DUP_WAIT_RSP;
  }

  int dup_sync_sender_manager::duplicate_data(request_ltrim *request, int version) {
      log_info("dup_sync_send_manager::duplicate_data");
      int bucket_number = tair_mgr->get_bucket_number(request->key);
      vector<uint64_t> slaves;
      tair_mgr->get_slaves(request->key.server_flag, bucket_number, slaves);
      size_t copy_count = slaves.size();
      uint32_t packet_id = atomic_add_return(copy_count, &packet_id_creater);
      if (packet_id == 0) {
          atomic_add_return(copy_count, &packet_id_creater);
      }   

      int ret = packets_mgr.addWaitNode(0, NULL, NULL, bucket_number, slaves, request, packet_id, version);
      if (ret != 0) {
          return ret;
      }   

      for (size_t i = 0; i < copy_count; ++i) {
          request_ltrim_dup* packet = new request_ltrim_dup(*request);
          packet->packet_id = packet_id;
          packet->bucket_id = bucket_number;
          uint64_t des_svr = slaves[i];
          std::string ip_str = tbsys::CNetUtil::addrToString(des_svr);
          log_debug("duplicate packet %u to %s", packet_id, ip_str.c_str());
          if (conn_mgr->sendPacket(des_svr, packet, NULL, (void*)((long)packet_id), true) == false) {
              log_error("duplicate packet %u to %s busy", packet_id, ip_str.c_str());
              delete packet;
              packets_mgr.clear_waitnode(packet_id);
              ret = TAIR_RETURN_DUPLICATE_BUSY;
              return ret;
          }   
      }   

      return TAIR_DUP_WAIT_RSP;
  }

  int dup_sync_sender_manager::duplicate_data(request_lrem *request, int version) {
      log_debug("dup_sync_send_manager::duplicate_data");
      int bucket_number = tair_mgr->get_bucket_number(request->key);
      vector<uint64_t> slaves;
      tair_mgr->get_slaves(request->key.server_flag, bucket_number, slaves);
      size_t copy_count = slaves.size();
      uint32_t packet_id = atomic_add_return(copy_count, &packet_id_creater);
      if (packet_id == 0) {
          atomic_add_return(copy_count, &packet_id_creater);
      }   

      int ret = packets_mgr.addWaitNode(0, NULL, NULL, bucket_number, slaves, request, packet_id, version);
      if (ret != 0) {
          return ret;
      }   

      for (size_t i = 0; i < copy_count; ++i) {
          request_lrem_dup* packet = new request_lrem_dup(*request);
          packet->packet_id = packet_id;
          packet->bucket_id = bucket_number;
          uint64_t des_svr = slaves[i];
          std::string ip_str = tbsys::CNetUtil::addrToString(des_svr);
          log_debug("duplicate packet %u to %s", packet_id, ip_str.c_str());
          if (conn_mgr->sendPacket(des_svr, packet, NULL, (void*)((long)packet_id), true) == false) {
              log_error("duplicate packet %u to %s busy", packet_id, ip_str.c_str());
              delete packet;
              packets_mgr.clear_waitnode(packet_id);
              ret = TAIR_RETURN_DUPLICATE_BUSY;
              return ret;
          }   
      }   

      return TAIR_DUP_WAIT_RSP;
  }

  int dup_sync_sender_manager::duplicate_data(request_lset *request, int version) {
      log_debug("dup_sync_send_manager::duplicate_data");
      int bucket_number = tair_mgr->get_bucket_number(request->key);
      vector<uint64_t> slaves;
      tair_mgr->get_slaves(request->key.server_flag, bucket_number, slaves);
      size_t copy_count = slaves.size();
      uint32_t packet_id = atomic_add_return(copy_count, &packet_id_creater);
      if (packet_id == 0) {
          atomic_add_return(copy_count, &packet_id_creater);
      }   

      int ret = packets_mgr.addWaitNode(0, NULL, NULL, bucket_number, slaves, request, packet_id, version);
      if (ret != 0) {
          return ret;
      }   

      for (size_t i = 0; i < copy_count; ++i) {
          request_lset_dup* packet = new request_lset_dup(*request);
          packet->packet_id = packet_id;
          packet->bucket_id = bucket_number;
          uint64_t des_svr = slaves[i];
          std::string ip_str = tbsys::CNetUtil::addrToString(des_svr);
          log_debug("duplicate packet %u to %s", packet_id, ip_str.c_str());
          if (conn_mgr->sendPacket(des_svr, packet, NULL, (void*)((long)packet_id), true) == false) {
              log_error("duplicate packet %u to %s busy", packet_id, ip_str.c_str());
              delete packet;
              packets_mgr.clear_waitnode(packet_id);
              ret = TAIR_RETURN_DUPLICATE_BUSY;
              return ret;
          }   
      }   

      return TAIR_DUP_WAIT_RSP;
  }

  int dup_sync_sender_manager::duplicate_data(request_hmset *request, int rc, int version) {
      log_debug("dup_sync_send_manager::duplicate_data");
      int bucket_number = tair_mgr->get_bucket_number(request->key);
      vector<uint64_t> slaves;
      tair_mgr->get_slaves(request->key.server_flag, bucket_number, slaves);
      size_t copy_count = slaves.size();
      uint32_t packet_id = atomic_add_return(copy_count, &packet_id_creater);
      if (packet_id == 0) {
          atomic_add_return(copy_count, &packet_id_creater);
      }   

      //int ret = packets_mgr.addWaitNode(0, NULL, NULL, bucket_number, slaves, request, packet_id, version);
      CPacket_wait_Nodes* node = new CPacket_wait_Nodes(bucket_number, request, slaves, version, NULL);
      node->ret1 = rc;
      int ret = packets_mgr.addWaitNode(node, bucket_number, packet_id);
      if (ret != 0) {
          return ret;
      }   

      for (size_t i = 0; i < copy_count; ++i) {
          request_hmset_dup* packet = new request_hmset_dup(*request);
          packet->packet_id = packet_id;
          packet->bucket_id = bucket_number;
          uint64_t des_svr = slaves[i];
          std::string ip_str = tbsys::CNetUtil::addrToString(des_svr);
          log_debug("duplicate packet %u to %s", packet_id, ip_str.c_str());
          if (conn_mgr->sendPacket(des_svr, packet, NULL, (void*)((long)packet_id), true) == false) {
              log_error("duplicate packet %u to %s busy", packet_id, ip_str.c_str());
              delete packet;
              packets_mgr.clear_waitnode(packet_id);
              ret = TAIR_RETURN_DUPLICATE_BUSY;
              return ret;
          }   
      }   

      return TAIR_DUP_WAIT_RSP;
  }

  int dup_sync_sender_manager::duplicate_data(request_hset *request, int rc, int version) {
      log_debug("dup_sync_send_manager::duplicate_data");
      int bucket_number = tair_mgr->get_bucket_number(request->key);
      vector<uint64_t> slaves;
      tair_mgr->get_slaves(request->key.server_flag, bucket_number, slaves);
      size_t copy_count = slaves.size();
      uint32_t packet_id = atomic_add_return(copy_count, &packet_id_creater);
      if (packet_id == 0) {
          atomic_add_return(copy_count, &packet_id_creater);
      }   

      //int ret = packets_mgr.addWaitNode(0, NULL, NULL, bucket_number, slaves, request, packet_id, version);
      CPacket_wait_Nodes* node = new CPacket_wait_Nodes(bucket_number, request, slaves, version, NULL);
      node->ret1 = rc;
      int ret = packets_mgr.addWaitNode(node, bucket_number, packet_id);
      if (ret != 0) {
          return ret;
      }   

      for (size_t i = 0; i < copy_count; ++i) {
          request_hset_dup* packet = new request_hset_dup(*request);
          packet->packet_id = packet_id;
          packet->bucket_id = bucket_number;
          uint64_t des_svr = slaves[i];
          std::string ip_str = tbsys::CNetUtil::addrToString(des_svr);
          log_debug("duplicate packet %u to %s", packet_id, ip_str.c_str());
          if (conn_mgr->sendPacket(des_svr, packet, NULL, (void*)((long)packet_id), true) == false) {
              log_error("duplicate packet %u to %s busy", packet_id, ip_str.c_str());
              delete packet;
              packets_mgr.clear_waitnode(packet_id);
              ret = TAIR_RETURN_DUPLICATE_BUSY;
              return ret;
          }   
      }   

      return TAIR_DUP_WAIT_RSP;
  }

  int dup_sync_sender_manager::duplicate_data(request_hsetnx *request, int version) {
      log_debug("dup_sync_send_manager::duplicate_data");
      int bucket_number = tair_mgr->get_bucket_number(request->key);
      vector<uint64_t> slaves;
      tair_mgr->get_slaves(request->key.server_flag, bucket_number, slaves);
      size_t copy_count = slaves.size();
      uint32_t packet_id = atomic_add_return(copy_count, &packet_id_creater);
      if (packet_id == 0) {
          atomic_add_return(copy_count, &packet_id_creater);
      }   

      int ret = packets_mgr.addWaitNode(0, NULL, NULL, bucket_number, slaves, request, packet_id, version);
      if (ret != 0) {
          return ret;
      }   

      for (size_t i = 0; i < copy_count; ++i) {
          request_hsetnx_dup* packet = new request_hsetnx_dup(*request);
          packet->packet_id = packet_id;
          packet->bucket_id = bucket_number;
          uint64_t des_svr = slaves[i];
          std::string ip_str = tbsys::CNetUtil::addrToString(des_svr);
          log_debug("duplicate packet %u to %s", packet_id, ip_str.c_str());
          if (conn_mgr->sendPacket(des_svr, packet, NULL, (void*)((long)packet_id), true) == false) {
              log_error("duplicate packet %u to %s busy", packet_id, ip_str.c_str());
              delete packet;
              packets_mgr.clear_waitnode(packet_id);
              ret = TAIR_RETURN_DUPLICATE_BUSY;
              return ret;
          }   
      }   

      return TAIR_DUP_WAIT_RSP;
  }

  int dup_sync_sender_manager::duplicate_data(request_hdel *request, int version) {
      log_debug("dup_sync_send_manager::duplicate_data");
      int bucket_number = tair_mgr->get_bucket_number(request->key);
      vector<uint64_t> slaves;
      tair_mgr->get_slaves(request->key.server_flag, bucket_number, slaves);
      size_t copy_count = slaves.size();
      uint32_t packet_id = atomic_add_return(copy_count, &packet_id_creater);
      if (packet_id == 0) {
          atomic_add_return(copy_count, &packet_id_creater);
      }   

      int ret = packets_mgr.addWaitNode(0, NULL, NULL, bucket_number, slaves, request, packet_id, version);
      if (ret != 0) {
          return ret;
      }   

      for (size_t i = 0; i < copy_count; ++i) {
          request_hdel_dup* packet = new request_hdel_dup(*request);
          packet->packet_id = packet_id;
          packet->bucket_id = bucket_number;
          uint64_t des_svr = slaves[i];
          std::string ip_str = tbsys::CNetUtil::addrToString(des_svr);
          log_debug("duplicate packet %u to %s", packet_id, ip_str.c_str());
          if (conn_mgr->sendPacket(des_svr, packet, NULL, (void*)((long)packet_id), true) == false) {
              log_error("duplicate packet %u to %s busy", packet_id, ip_str.c_str());
              delete packet;
              packets_mgr.clear_waitnode(packet_id);
              ret = TAIR_RETURN_DUPLICATE_BUSY;
              return ret;
          }   
      }   

      return TAIR_DUP_WAIT_RSP;
  }

int dup_sync_sender_manager::duplicate_data(request_spop *request, int version) {
      log_debug("dup_sync_send_manager::duplicate_data");
      int bucket_number = tair_mgr->get_bucket_number(request->key);
      vector<uint64_t> slaves;
      tair_mgr->get_slaves(request->key.server_flag, bucket_number, slaves);
      size_t copy_count = slaves.size();
      uint32_t packet_id = atomic_add_return(copy_count, &packet_id_creater);
      if (packet_id == 0) {
          atomic_add_return(copy_count, &packet_id_creater);
      }   

      int ret = packets_mgr.addWaitNode(0, NULL, NULL, bucket_number, slaves, request, packet_id, version);
      if (ret != 0) {
          return ret;
      }   

      for (size_t i = 0; i < copy_count; ++i) {
          request_spop_dup* packet = new request_spop_dup(*request);
          packet->packet_id = packet_id;
          packet->bucket_id = bucket_number;
          uint64_t des_svr = slaves[i];
          std::string ip_str = tbsys::CNetUtil::addrToString(des_svr);
          log_debug("duplicate packet %u to %s", packet_id, ip_str.c_str());
          if (conn_mgr->sendPacket(des_svr, packet, NULL, (void*)((long)packet_id), true) == false) {
              log_error("duplicate packet %u to %s busy", packet_id, ip_str.c_str());
              delete packet;
              packets_mgr.clear_waitnode(packet_id);
              ret = TAIR_RETURN_DUPLICATE_BUSY;
              return ret;
          }   
      }   

      return TAIR_DUP_WAIT_RSP;
  }

  int dup_sync_sender_manager::duplicate_data(request_srem *request, int version) {
      log_debug("dup_sync_send_manager::request_srem");
      int bucket_number = tair_mgr->get_bucket_number(request->key);
      vector<uint64_t> slaves;
      tair_mgr->get_slaves(request->key.server_flag, bucket_number, slaves);
      size_t copy_count = slaves.size();
      uint32_t packet_id = atomic_add_return(copy_count, &packet_id_creater);
      if (packet_id == 0) {
          atomic_add_return(copy_count, &packet_id_creater);
      }   

      int ret = packets_mgr.addWaitNode(0, NULL, NULL, bucket_number, slaves, request, packet_id, version);
      if (ret != 0) {
          return ret;
      }   

      for (size_t i = 0; i < copy_count; ++i) {
          request_srem_dup* packet = new request_srem_dup(*request);
          packet->packet_id = packet_id;
          packet->bucket_id = bucket_number;
          uint64_t des_svr = slaves[i];
          std::string ip_str = tbsys::CNetUtil::addrToString(des_svr);
          log_debug("duplicate packet %u to %s", packet_id, ip_str.c_str());
          if (conn_mgr->sendPacket(des_svr, packet, NULL, (void*)((long)packet_id), true) == false) {
              log_error("duplicate packet %u to %s busy", packet_id, ip_str.c_str());
              delete packet;
              packets_mgr.clear_waitnode(packet_id);
              ret = TAIR_RETURN_DUPLICATE_BUSY;
              return ret;
          }   
      }   

      return TAIR_DUP_WAIT_RSP;
  }

  int dup_sync_sender_manager::duplicate_data(request_sadd *request, int rc, int version) {      
      log_debug("dup_sync_send_manager::duplicate_data");
      int bucket_number = tair_mgr->get_bucket_number(request->key);
      vector<uint64_t> slaves;
      tair_mgr->get_slaves(request->key.server_flag, bucket_number, slaves);
      size_t copy_count = slaves.size();
      uint32_t packet_id = atomic_add_return(copy_count, &packet_id_creater);      
      if (packet_id == 0) { 
          atomic_add_return(copy_count, &packet_id_creater);
      }    

      //int ret = packets_mgr.addWaitNode(0, NULL, NULL, bucket_number, slaves, request, packet_id, version);
      CPacket_wait_Nodes* node = new CPacket_wait_Nodes(bucket_number, request, slaves, version, NULL);
      node->ret1 = rc;
      int ret = packets_mgr.addWaitNode(node, bucket_number, packet_id);
      if (ret != 0) {           
          return ret; 
      }    

      for (size_t i = 0; i < copy_count; ++i) {
          request_sadd_dup* packet = new request_sadd_dup(*request);
          packet->packet_id = packet_id;          
          packet->bucket_id = bucket_number;
          uint64_t des_svr = slaves[i];
          std::string ip_str = tbsys::CNetUtil::addrToString(des_svr);
          log_debug("duplicate packet %u to %s", packet_id, ip_str.c_str());
          if (conn_mgr->sendPacket(des_svr, packet, NULL, (void*)((long)packet_id), true) == false) {
              log_error("duplicate packet %u to %s busy", packet_id, ip_str.c_str());
              delete packet;
              packets_mgr.clear_waitnode(packet_id);
              ret = TAIR_RETURN_DUPLICATE_BUSY;
              return ret; 
          }    
      }    

      return TAIR_DUP_WAIT_RSP;
  }

  int dup_sync_sender_manager::duplicate_data(request_sadd_multi *request, int rc, int version) {
      log_debug("dup_sync_send_manager::duplicate_data");
      map<data_entry*, vector<data_entry *>*> &mapper = request->keys_values_map;
      map<data_entry*, vector<data_entry *>*>::iterator iter = mapper.begin();
      if (iter == mapper.end()) {
          return TAIR_RETURN_FAILED;
      }
      data_entry* key = iter->first;
      int bucket_number = tair_mgr->get_bucket_number(*key);
      vector<uint64_t> slaves;
      tair_mgr->get_slaves(key->server_flag, bucket_number, slaves);
      size_t copy_count = slaves.size();
      uint32_t packet_id = atomic_add_return(copy_count, &packet_id_creater);
      if (packet_id == 0) {
          atomic_add_return(copy_count, &packet_id_creater);
      }   

      //int ret = packets_mgr.addWaitNode(0, NULL, NULL, bucket_number, slaves, request, packet_id, version);
      CPacket_wait_Nodes* node = new CPacket_wait_Nodes(bucket_number, request, slaves, version, NULL);
      node->ret1 = rc;
      int ret = packets_mgr.addWaitNode(node, bucket_number, packet_id);
      if (ret != 0) {
          return ret;
      }   

      for (size_t i = 0; i < copy_count; ++i) {
          request_sadd_multi_dup* packet = new request_sadd_multi_dup(*request);
          packet->packet_id = packet_id;
          packet->bucket_id = bucket_number;
          uint64_t des_svr = slaves[i];
          std::string ip_str = tbsys::CNetUtil::addrToString(des_svr);
          log_debug("duplicate packet %u to %s", packet_id, ip_str.c_str());
          if (conn_mgr->sendPacket(des_svr, packet, NULL, (void*)((long)packet_id), true) == false) {
              log_error("duplicate packet %u to %s busy", packet_id, ip_str.c_str());
              delete packet;
              packets_mgr.clear_waitnode(packet_id);
              ret = TAIR_RETURN_DUPLICATE_BUSY;
              return ret;
          }   
      }   

      return TAIR_DUP_WAIT_RSP;
  }

  int dup_sync_sender_manager::duplicate_data(request_zadd *request, int rc, int version) {
      log_debug("dup_sync_send_manager::duplicate_data");
      int bucket_number = tair_mgr->get_bucket_number(request->key);
      vector<uint64_t> slaves;
      tair_mgr->get_slaves(request->key.server_flag, bucket_number, slaves);
      size_t copy_count = slaves.size();
      uint32_t packet_id = atomic_add_return(copy_count, &packet_id_creater);
      if (packet_id == 0) {
          atomic_add_return(copy_count, &packet_id_creater);
      }   

      //int ret = packets_mgr.addWaitNode(0, NULL, NULL, bucket_number, slaves, request, packet_id, version);
      CPacket_wait_Nodes* node = new CPacket_wait_Nodes(bucket_number, request, slaves, version, NULL);
      node->ret1 = rc;
      int ret = packets_mgr.addWaitNode(node, bucket_number, packet_id);
      if (ret != 0) {
          return ret;
      }   

      for (size_t i = 0; i < copy_count; ++i) {
          request_zadd_dup* packet = new request_zadd_dup(*request);
          packet->packet_id = packet_id;
          packet->bucket_id = bucket_number;
          uint64_t des_svr = slaves[i];
          std::string ip_str = tbsys::CNetUtil::addrToString(des_svr);
          log_debug("duplicate packet %u to %s", packet_id, ip_str.c_str());
          if (conn_mgr->sendPacket(des_svr, packet, NULL, (void*)((long)packet_id), true) == false) {
              log_error("duplicate packet %u to %s busy", packet_id, ip_str.c_str());
              delete packet;
              packets_mgr.clear_waitnode(packet_id);
              ret = TAIR_RETURN_DUPLICATE_BUSY;
              return ret;
          }   
      }   

      return TAIR_DUP_WAIT_RSP;
  }

  int dup_sync_sender_manager::duplicate_data(request_zrem *request, int version) {
      log_debug("dup_sync_send_manager::duplicate_data");
      int bucket_number = tair_mgr->get_bucket_number(request->key);
      vector<uint64_t> slaves;
      tair_mgr->get_slaves(request->key.server_flag, bucket_number, slaves);
      size_t copy_count = slaves.size();
      uint32_t packet_id = atomic_add_return(copy_count, &packet_id_creater);
      if (packet_id == 0) {
          atomic_add_return(copy_count, &packet_id_creater);
      }   

      int ret = packets_mgr.addWaitNode(0, NULL, NULL, bucket_number, slaves, request, packet_id, version);
      if (ret != 0) {
          return ret;
      }   

      for (size_t i = 0; i < copy_count; ++i) {
          request_zrem_dup* packet = new request_zrem_dup(*request);
          packet->packet_id = packet_id;
          packet->bucket_id = bucket_number;
          uint64_t des_svr = slaves[i];
          std::string ip_str = tbsys::CNetUtil::addrToString(des_svr);
          log_debug("duplicate packet %u to %s", packet_id, ip_str.c_str());
          if (conn_mgr->sendPacket(des_svr, packet, NULL, (void*)((long)packet_id), true) == false) {
              log_error("duplicate packet %u to %s busy", packet_id, ip_str.c_str());
              delete packet;
              packets_mgr.clear_waitnode(packet_id);
              ret = TAIR_RETURN_DUPLICATE_BUSY;
              return ret;
          }   
      }   

      return TAIR_DUP_WAIT_RSP;
  }

  int dup_sync_sender_manager::duplicate_data(request_zremrangebyrank *request, long long resp_retnum, int version) {
      log_debug("dup_sync_send_manager::duplicate_data");
      int bucket_number = tair_mgr->get_bucket_number(request->key);
      vector<uint64_t> slaves;
      tair_mgr->get_slaves(request->key.server_flag, bucket_number, slaves);
      size_t copy_count = slaves.size();
      uint32_t packet_id = atomic_add_return(copy_count, &packet_id_creater);
      if (packet_id == 0) {
          atomic_add_return(copy_count, &packet_id_creater);
      }   

      //int ret = packets_mgr.addWaitNode(0, NULL, NULL, bucket_number, slaves, request, packet_id, version);
      CPacket_wait_Nodes* node = new CPacket_wait_Nodes(bucket_number, request, slaves, version, NULL);
      node->ret1 = resp_retnum;
      int ret = packets_mgr.addWaitNode(node, bucket_number, packet_id);
      if (ret != 0) {
          return ret;
      }   

      for (size_t i = 0; i < copy_count; ++i) {
          request_zremrangebyrank_dup* packet = new request_zremrangebyrank_dup(*request);
          packet->packet_id = packet_id;
          packet->bucket_id = bucket_number;
          uint64_t des_svr = slaves[i];
          std::string ip_str = tbsys::CNetUtil::addrToString(des_svr);
          log_debug("duplicate packet %u to %s", packet_id, ip_str.c_str());
          if (conn_mgr->sendPacket(des_svr, packet, NULL, (void*)((long)packet_id), true) == false) {
              log_error("duplicate packet %u to %s busy", packet_id, ip_str.c_str());
              delete packet;
              packets_mgr.clear_waitnode(packet_id);
              ret = TAIR_RETURN_DUPLICATE_BUSY;
              return ret;
          }   
      }   

      return TAIR_DUP_WAIT_RSP;
  }

  int dup_sync_sender_manager::duplicate_data(request_zremrangebyscore *request, long long resp_retnum, int version) {
      log_debug("dup_sync_send_manager::duplicate_data");
      int bucket_number = tair_mgr->get_bucket_number(request->key);
      vector<uint64_t> slaves;
      tair_mgr->get_slaves(request->key.server_flag, bucket_number, slaves);
      size_t copy_count = slaves.size();
      uint32_t packet_id = atomic_add_return(copy_count, &packet_id_creater);
      if (packet_id == 0) {
          atomic_add_return(copy_count, &packet_id_creater);
      }   

      //int ret = packets_mgr.addWaitNode(0, NULL, NULL, bucket_number, slaves, request, packet_id, version);
      CPacket_wait_Nodes* node = new CPacket_wait_Nodes(bucket_number, request, slaves, version, NULL);
      node->ret1 = resp_retnum;
      int ret = packets_mgr.addWaitNode(node, bucket_number, packet_id);
      if (ret != 0) {
          return ret;
      }   

      for (size_t i = 0; i < copy_count; ++i) {
          request_zremrangebyscore_dup* packet = new request_zremrangebyscore_dup(*request);
          packet->packet_id = packet_id;
          packet->bucket_id = bucket_number;
          uint64_t des_svr = slaves[i];
          std::string ip_str = tbsys::CNetUtil::addrToString(des_svr);
          log_debug("duplicate packet %u to %s", packet_id, ip_str.c_str());
          if (conn_mgr->sendPacket(des_svr, packet, NULL, (void*)((long)packet_id), true) == false) {
              log_error("duplicate packet %u to %s busy", packet_id, ip_str.c_str());
              delete packet;
              packets_mgr.clear_waitnode(packet_id);
              ret = TAIR_RETURN_DUPLICATE_BUSY;
              return ret;
          }   
      }   

      return TAIR_DUP_WAIT_RSP;
  }

  int dup_sync_sender_manager::duplicate_data(request_lrpush *request, int version, 
          int resp_pushed_num, int resp_list_len, int rc) {
      log_debug("dup_sync_send_manager::duplicate_data");
      int bucket_number = tair_mgr->get_bucket_number(request->key);
      vector<uint64_t> slaves;
      tair_mgr->get_slaves(request->key.server_flag, bucket_number, slaves);
      size_t copy_count = slaves.size();
      uint32_t packet_id = atomic_add_return(copy_count, &packet_id_creater);
      if (packet_id == 0) {
          atomic_add_return(copy_count, &packet_id_creater);
      }   

      //int ret = packets_mgr.addWaitNode(0, NULL, NULL, bucket_number, slaves, request, packet_id, version);
      CPacket_wait_Nodes* node = new CPacket_wait_Nodes(bucket_number, request, slaves, version, NULL);
      node->ret1 = resp_pushed_num;
      node->ret2 = resp_list_len;

      // use this to store rc of master, ugly!!!!
      node->inc_value_result = rc;
      int ret = packets_mgr.addWaitNode(node, bucket_number, packet_id);
      if (ret != 0) {
          return ret;
      }   

      for (size_t i = 0; i < copy_count; ++i) {
          request_lrpush_dup* packet = new request_lrpush_dup(*request);
          packet->packet_id = packet_id;
          packet->bucket_id = bucket_number;
          uint64_t des_svr = slaves[i];
          std::string ip_str = tbsys::CNetUtil::addrToString(des_svr);
          log_debug("duplicate packet %u to %s", packet_id, ip_str.c_str());
          if (conn_mgr->sendPacket(des_svr, packet, NULL, (void*)((long)packet_id), true) == false) {
              log_error("duplicate packet %u to %s busy", packet_id, ip_str.c_str());
              delete packet;
              packets_mgr.clear_waitnode(packet_id);
              ret = TAIR_RETURN_DUPLICATE_BUSY;
              return ret;
          }   
      }   

      return TAIR_DUP_WAIT_RSP;
  }

  int dup_sync_sender_manager::duplicate_data(request_lrpush_limit *request, int version) {
      log_debug("dup_sync_send_manager::duplicate_data");
      int bucket_number = tair_mgr->get_bucket_number(request->key);
      vector<uint64_t> slaves;
      tair_mgr->get_slaves(request->key.server_flag, bucket_number, slaves);
      size_t copy_count = slaves.size();
      uint32_t packet_id = atomic_add_return(copy_count, &packet_id_creater);
      if (packet_id == 0) {
          atomic_add_return(copy_count, &packet_id_creater);
      }   

      //int ret = packets_mgr.addWaitNode(0, NULL, NULL, bucket_number, slaves, request, packet_id, version);
      CPacket_wait_Nodes* node = new CPacket_wait_Nodes(bucket_number, request, slaves, version, NULL);
      int ret = packets_mgr.addWaitNode(node, bucket_number, packet_id);
      if (ret != 0) {
          return ret;
      }   

      for (size_t i = 0; i < copy_count; ++i) {
          request_lrpush_limit_dup* packet = new request_lrpush_limit_dup(*request);
          packet->packet_id = packet_id;
          packet->bucket_id = bucket_number;
          uint64_t des_svr = slaves[i];
          std::string ip_str = tbsys::CNetUtil::addrToString(des_svr);
          log_debug("duplicate packet %u to %s", packet_id, ip_str.c_str());
          if (conn_mgr->sendPacket(des_svr, packet, NULL, (void*)((long)packet_id), true) == false) {
              log_error("duplicate packet %u to %s busy", packet_id, ip_str.c_str());
              delete packet;
              packets_mgr.clear_waitnode(packet_id);
              ret = TAIR_RETURN_DUPLICATE_BUSY;
              return ret;
          }   
      }   

      return TAIR_DUP_WAIT_RSP;
  }

  int dup_sync_sender_manager::duplicate_data(request_lrpop *request, int version, 
          int master_ret_code, const vector<data_entry*>& master_ret_values) {
      log_debug("dup_sync_send_manager::duplicate_data");
      int bucket_number = tair_mgr->get_bucket_number(request->key);
      vector<uint64_t> slaves;
      tair_mgr->get_slaves(request->key.server_flag, bucket_number, slaves);
      size_t copy_count = slaves.size();
      uint32_t packet_id = atomic_add_return(copy_count, &packet_id_creater);
      if (packet_id == 0) {
          atomic_add_return(copy_count, &packet_id_creater);
      }   

      CPacket_wait_Nodes* node = new CPacket_wait_Nodes(bucket_number, request, slaves, version, NULL);
      node->ret1 = master_ret_code;
      node->ret_values = master_ret_values;
      int ret = packets_mgr.addWaitNode(node, bucket_number, packet_id);
      if (ret != 0) {
          return ret;
      }   

      for (size_t i = 0; i < copy_count; ++i) {
          request_lrpop_dup* packet = new request_lrpop_dup(*request);
          packet->packet_id = packet_id;
          packet->bucket_id = bucket_number;
          uint64_t des_svr = slaves[i];
          std::string ip_str = tbsys::CNetUtil::addrToString(des_svr);
          log_debug("duplicate packet %u to %s", packet_id, ip_str.c_str());
          if (conn_mgr->sendPacket(des_svr, packet, NULL, (void*)((long)packet_id), true) == false) {
              log_error("duplicate packet %u to %s busy", packet_id, ip_str.c_str());
              delete packet;
              packets_mgr.clear_waitnode(packet_id);
              ret = TAIR_RETURN_DUPLICATE_BUSY;
              return ret;
          }   
      }   

      return TAIR_DUP_WAIT_RSP;
  }

  int dup_sync_sender_manager::duplicate_data(request_remove *request, int version, int master_rc) {
      log_debug("dup_sync_send_manager::duplicate_data");
      int bucket_number ;
      int server_flag;
      if (NULL != request->key_list) {
           set<data_entry*, data_entry_comparator>::iterator it = request->key_list->begin();
           if (it != request->key_list->end()) { 
               bucket_number = tair_mgr->get_bucket_number(**it);
               server_flag = (*it)->server_flag;
           }
      }
      else if (NULL != request->key) {
          bucket_number = tair_mgr->get_bucket_number(*(request->key));
          server_flag = request->key->server_flag;
      }
      else {
          return TAIR_RETURN_SUCCESS;
      }
      vector<uint64_t> slaves;
      tair_mgr->get_slaves(server_flag, bucket_number, slaves);
      size_t copy_count = slaves.size();
      uint32_t packet_id = atomic_add_return(copy_count, &packet_id_creater);
      if (packet_id == 0) {
          atomic_add_return(copy_count, &packet_id_creater);
      }   

      CPacket_wait_Nodes* node = new CPacket_wait_Nodes(bucket_number, request, slaves, version, NULL);
      node->ret1 = master_rc;
      int ret = packets_mgr.addWaitNode(node, bucket_number, packet_id);
      if (ret != TAIR_RETURN_SUCCESS) {
          return ret;
      }

      for (size_t i = 0; i < copy_count; ++i) {
          request_remove_dup* packet = new request_remove_dup(*request);
          packet->packet_id = packet_id;
          packet->bucket_id = bucket_number;
          uint64_t des_svr = slaves[i];
          std::string ip_str = tbsys::CNetUtil::addrToString(des_svr);
          log_debug("duplicate packet %u to %s", packet_id, ip_str.c_str());
          if (conn_mgr->sendPacket(des_svr, packet, NULL, (void*)((long)packet_id), true) == false) {
              log_info("duplicate packet %u to %s busy", packet_id, ip_str.c_str());
              delete packet;
              packets_mgr.clear_waitnode(packet_id);
              ret = TAIR_RETURN_DUPLICATE_BUSY;
              return ret;
          }  
      }   

      return TAIR_DUP_WAIT_RSP;
  }


}
