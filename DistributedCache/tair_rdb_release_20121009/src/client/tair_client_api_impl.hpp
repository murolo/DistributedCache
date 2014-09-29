/*
 * (C) 2007-2010 Alibaba Group Holding Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 * Version: $Id: tair_client_api_impl.hpp 562 2012-02-08 09:44:00Z choutian.xmm@taobao.com $
 *
 * Authors:
 *   MaoQi <maoqi@taobao.com>
 *
 */
#ifndef __TAIR_CLIENT_IMPL_H
#define __TAIR_CLIENT_IMPL_H

#include <string>
#include <vector>
#include <map>
#include <ext/hash_map>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <dirent.h>

#include <tbsys.h>
#include <tbnet.h>

#include "define.hpp"
#include "tair_client_api.hpp"
#include "packet_factory.hpp"
#include "wait_object.hpp"
#include "response_return_packet.hpp"
#include "util.hpp"
#include "dump_data_info.hpp"
#include "remove_packet.hpp"
#include "op_cmd_packet.hpp"

#include "lrpush_packet.hpp"
#include "lrpop_packet.hpp"
#include "lindex_packet.hpp"
#include "lrange_packet.hpp"
#include "llen_packet.hpp"
#include "ltrim_packet.hpp"
#include "lset_packet.hpp"

#include "sadd_packet.hpp"
#include "smembers_packet.hpp"
#include "srem_packet.hpp"

#include "hset_packet.hpp"
#include "hget_packet.hpp"
#include "hmset_packet.hpp"
#include "hgetall_packet.hpp"
#include "hmget_packet.hpp"
#include "hdel_packet.hpp"

#include "zadd_packet.hpp"
#include "zrem_packet.hpp"
#include "zcard_packet.hpp"
#include "zrange_packet.hpp"
#include "generic_zrangebyscore_packet.hpp"
#include "zremrangebyscore_packet.hpp"
#include "zremrangebyrank_packet.hpp"

//patch:zouyueming: 2014-05-12 16:33
#ifdef _ENABLE_AGILE
#include "../../../../../tair_patch/agile_packet_factory.h"
#endif // #ifdef _ENABLE_AGILE

namespace tair {


  using namespace std;
  using namespace __gnu_cxx;
  using namespace tair::common;

  const int UPDATE_SERVER_TABLE_INTERVAL = 50;

  const static short NOT_CARE_VERSION = 0;
  const static int NOT_CARE_EXPIRE = -1;
  const static int CANCEL_EXPIRE = 0;

  typedef map<uint64_t , request_get *> request_get_map;
  typedef map<uint64_t , request_remove *> request_remove_map;

  class tair_client_impl : public tbsys::Runnable, public tbnet::IPacketHandler {
    public:
      tair_client_impl();

      ~tair_client_impl();

      bool startup(const char *master_addr,const char *slave_addr,const char *group_name);
      bool directup(const char *server_addr);
      bool startup(uint64_t data_server);

      void close();

    public:

      int send_packet(uint32_t hashcode, base_packet* spacket);
      int send_packet(const data_entry &key, base_packet* spacket);

      int put(int area,
          const data_entry &key,
          const data_entry &data,
          int expire,
          int version);

      //the caller will release the memory
      int get(int area,
          const data_entry &key,
          data_entry*& data);

      int mget(int area,
          vector<data_entry *> &keys,
          tair_keyvalue_map &data);

      int remove(int area,
          const data_entry &key);

      int mdelete(int area,
          vector<data_entry*> &keys);

      //caller need release values
      //lpush
      int lpush(int area, data_entry &key, data_entry *value, int expire, int version);
      int lpush(int area, data_entry &key, vector<data_entry*> &values, int expire, int version, long &successlen);
      //rpush
      int rpush(int area, data_entry &key, data_entry *value, int expire, int version);
      int rpush(int area, data_entry &key, vector<data_entry*> &values, int expire, int version, long &successlen);
      //lpop
      int lpop(int area, data_entry &key, data_entry** value, int expire, int version);
      int lpop(int area, data_entry &key, int count, vector<data_entry*> &values, int expire, int version);
      //rpop
      int rpop(int area, data_entry &key, data_entry** value, int expire, int version);
      int rpop(int area, data_entry &key, int count, vector<data_entry*> &values, int expire, int version);
      //lindex
      int lindex(int area, data_entry &key, int index, data_entry &value);
      //lrange
      int lrange(int area, data_entry &key, int start, int end, vector<data_entry*> &values);
      //lset
      int lset(int area, data_entry &key, int index, data_entry &value, int expire, int version);
      //llen
      int llen(int area, data_entry &key, int &length);
      //ltrim
      int ltrim(int area, data_entry &key, int start, int end, int expire, int version);


      //sadd
      int sadd(const int area, const data_entry &key, const data_entry &value,
              const int expire, const int version);
      //smembers
      int smembers(const int area, const data_entry &key, vector<data_entry*> &values);
      //srem
      int srem(const int area, const data_entry &key, const data_entry &value,
              const int expire, const int version);

      //hset
      int hset(const int area, const data_entry &key, const data_entry &field, const data_entry &value,
              const int expire, const int version);
      //hget
      int hget(const int area, const data_entry &key, const data_entry &field, data_entry &value);
      //hmset
      int hmset(const int area, const data_entry &key, const map<data_entry*, data_entry*> &field_values,
        map<data_entry*, data_entry*> &key_values_success,
        const int expire, const int version);
      //hgetall
      int hgetall(const int area, const data_entry &key, map<data_entry*, data_entry*> &field_values);
      //hmget
      int hmget(const int area, const data_entry &key, const vector<data_entry*> &fields, vector<data_entry*> &values);
      //hdel
      int hdel(const int area, const data_entry &key, const data_entry &field, const int expire, const int version);


      // zadd
      int zadd(const int area, const data_entry &key, const data_entry &value,
          const double score, const int expire, const int version);
      // zcard
      int zcard(const int area, const data_entry &key, long long *retnum);
      // zrem
      int zrem(const int area, const data_entry &key, const data_entry &value, const int expire, const int version);
      // zrange
      int zrange(const int area, const data_entry &key, const int start, const int end,
          const bool withscore, std::vector<std::pair<data_entry*, double> > &valuescores);
      // zrangebyscore
      int zrangebyscore(const int area, const data_entry &key, const double start, const double end,
          const bool reverse, const int limit, const bool withscore, std::vector<std::pair<data_entry*, double> > &valuescores);
      // zremrangebyscore
      int zremrangebyscore(const int area, const data_entry &key, const double start, const double end,
          const int expire, const int version, long long *retnum);
      // zremrangebyrank
      int zremrangebyrank(const int area, const data_entry &key, const int start, const int end,
          const int expire, const int version, long long *retnum);


      int add_count(int area,
          const data_entry &key,
          int count,
          int *retCount,
          int init_value = 0,
          int expire_time = 0);

      // cmd operated directly to configserver
      int op_cmd_to_cs(ServerCmdType cmd, std::vector<std::string>* params, std::vector<std::string>* ret_values);
      int op_cmd_to_ds(ServerCmdType cmd, std::vector<std::string>* params, std::vector<std::string>* infos,
          const char* dest_server_addr = NULL);

      void set_timeout(int this_timeout);

      uint32_t get_bucket_count() const { return bucket_count;}
      uint32_t get_copy_count() const { return copy_count;}

      template<typename Type>
        int find_elements_type(Type element);

      static const std::map<int,string> m_errmsg;
      static std::map<int,string> init_errmsg();
      const char *get_error_msg(int ret);
      void get_server_with_key(const data_entry& key,std::vector<std::string>& servers);

      //@override IPacketHandler
      tbnet::IPacketHandler::HPRetCode handlePacket(tbnet::Packet *packet, void *args);

      //@override Runnable
      void run(tbsys::CThread *thread, void *arg);

      int remove_area(int area, bool lazy = true);

      int dump_area(std::set<dump_meta_info>& info);

      void force_change_dataserver_status(uint64_t server_id, int cmd);
      void get_migrate_status(uint64_t server_id,vector<pair<uint64_t,uint32_t> >& result);
      void query_from_configserver(uint32_t query_type, const string group_name,
              map<string, string>&, uint64_t server_id = 0);

    //patch:zouyueming: 2014-05-12 agile version enable caller direct access member.
    public:

      int lrpush(int pcode, int area, data_entry &key, vector<data_entry*> &values,
              int expire, int version, long &successlen);
      int lrpop(int pcode, int area, data_entry &key, int count, vector<data_entry*> &values, int expire, int version);

      bool startup(uint64_t master_cfgsvr, uint64_t slave_cfgsvr, const char *group_name);
      bool directup(uint64_t data_server);

      bool initialize();

      bool retrieve_server_addr();

      void start_tbnet();

      void stop_tbnet();

      void wait_tbnet();

      void reset(); //reset enviroment

      bool get_server_id(const uint32_t hash, vector<uint64_t>& server);
      bool get_server_id(const data_entry &key, vector<uint64_t>& server);

      int send_request(uint64_t serverId,base_packet *packet,int waitId);

      //int send_request(vector<uint64_t>& server,TAIRPacket *packet,int waitId);

      int get_response(wait_object *cwo,int waitCount,base_packet*& tpacket);
      int get_response(wait_object *cwo, int wait_count, std::vector<base_packet*>& tpacket);


      bool data_entry_check(const data_entry& data);
      bool key_entry_check(const data_entry& data);
      bool map_data_check(const map<data_entry*, data_entry*> &md);
      bool vector_data_check(const vector<data_entry*> &vd);
      bool vector_key_check(const vector<data_entry*> &vk);

      int mget_impl(int area,
          vector<data_entry *> &keys,
          tair_keyvalue_map &data ,
          int server_select = 0);

      int init_request_map(int area,
          vector<data_entry *>& keys,
          request_get_map &request_gets ,
          int server_select = 0);

      int init_request_map(int area,
          vector<data_entry *>& keys,
          request_remove_map &request_removes);

      static bool vector_to_map(const std::vector<data_entry*> &_vd, std::map<data_entry*, data_entry*> &_md) {
        size_t _vd_len = _vd.size();
        if (_vd_len & 1) {
            return false;
        }
        for(size_t i = 0; i < _vd_len; i+=2) {
            _md.insert(make_pair(_vd[i], _vd[i+1]));
        }
        return true;
      }

      static bool map_to_vector(const std::map<data_entry*, data_entry*> &_md, std::vector<data_entry*> &_vd) {
        std::map<data_entry*, data_entry*>::const_iterator _iter;
        for(_iter = _md.begin(); _iter != _md.end(); _iter++) {
            _vd.push_back(_iter->first);
            _vd.push_back(_iter->second);
        }
        return true;
      }

    public:
      bool isinited() {return inited;}

    //patch:zouyueming: 2014-05-12 agile version enable caller direct access member.
    public:
      bool inited;
      bool is_stop;
      tbsys::CThread thread;

      bool direct;
      uint64_t data_server;


      //tair_packet_factory *packet_factory;
      //patch:zouyueming: 2014-05-12 18:02
#ifdef _ENABLE_AGILE
      agile::packet_factory_t *packet_factory;
#else
      tair_packet_factory *packet_factory;
#endif

      tbnet::DefaultPacketStreamer *streamer;
      tbnet::Transport *transport;
      tbnet::ConnectionManager *connmgr;

      int timeout; // ms

      vector<uint64_t> ds_server_list;   //ds bucket table
      vector<uint64_t> config_server_list;
      std::string group_name;
      uint32_t config_version;
      uint32_t new_config_version;
      int send_fail_count;
      //CWaitObjectManager wait_object_manager;
      wait_object_manager *this_wait_object_manager;
      uint32_t bucket_count;
      uint32_t copy_count;
    //private:
      pthread_rwlock_t m_init_mutex;
  };

} /* tair */
#endif //__TAIR_CLIENT_IMPL_H
