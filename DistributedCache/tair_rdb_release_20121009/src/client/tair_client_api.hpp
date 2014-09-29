/*
 * (C) 2007-2010 Alibaba Group Holding Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 * Version: $Id: tair_client_api.hpp 562 2012-02-08 09:44:00Z choutian.xmm@taobao.com $
 *
 * Authors:
 *   MaoQi <maoqi@taobao.com>
 *
 */
#ifndef TAIR_CLIENT_API
#define TAIR_CLIENT_API

#include <vector>
#include <map>
#include "data_entry.hpp"
#include "util.hpp"

namespace tair {

  class tair_client_impl;
  using namespace std;
  using namespace tair::common;

  class tair_client_api
  {
    public:
      tair_client_api();
      ~tair_client_api();

      static const int MAX_ITEMS = 65535;
      static const int ALL_ITEMS = MAX_ITEMS + 1;

      /**
       * @brief connect to tair cluster
       *
       * @param master_addr     master configserver addr [ip:port],default port is 5198
       * @param slave_addr      slave configserver addr [ip:port]
       * @param group_name      group_name
       *
       * @return true -- success,false -- fail
       */
      bool startup(const char *master_addr,const char *slave_addr,const char *group_name);

      /**
       * @brief close tairclient,release resource
       */
      void close();

      /**
       * @brief put data to tair
       *
       * @param area     namespace
       * @param key      key
       * @param data     data
       * @param expire   expire time,realtive time
       * @param version  version,if you don't care the version,set it to 0
       *
       * @return 0 -- success, otherwise fail,you can use get_error_msg(ret) to get more information.
       */
      int put(int area, const data_entry &key, const data_entry &data, int expire, int version);

      /**
       * @brief get data from tair cluster
       *
       * @param area    namespace
       * @param key     key
       * @param data    data,a pointer,the caller must release the memory.
       *
       * @return 0 -- success, otherwise fail.
       */
      int get(int area, const data_entry &key, data_entry*& data);

      /**
       * @brief get multi  data from tair cluster
       *
       * @param area    namespace
       * @param keys    keys list, vector type
       * @param data    data,data hash map,the caller must release the memory.
       *
       * @return 0 -- success, TAIR_RETURN_PARTIAL_SUCCESS -- partly success, otherwise fail.
       */
      int mget(int area, std::vector<data_entry *> &keys, tair_keyvalue_map& data);

      /**
       * @brief  remove data from tair cluster
       *
       * @param area    namespace
       * @param key     key
       *
       * @return  0 -- success, otherwise fail.
       */
      int remove(int area, const data_entry &key);

      /**
       * @brief  delete all data in the namespace
       *
       * @param area    namespace
       *
       * @return 0 -- success, otherwise fail
       */
      int remove_area(int area);

      /**
       * @brief remove multi  data from tair cluster
       *
       * @param area    namespace
       * @param keys    key list, vector type
       *
       * @return  0 -- success, TAIR_RETURN_PARTIAL_SUCCESS partly success, otherwise fail.
       */
      int mdelete(int area, vector<data_entry*> &key);
      /**
       * @brief invalid multi  data from tair cluster
       *
       * @param area    namespace
       * @param keys    key list, vector type
       *
       * @return  0 -- success, TAIR_RETURN_PARTIAL_SUCCESS partly success, otherwise fail.
       */
      int minvalid(int area, std::vector<data_entry*> &key);

      /**
       * @brief push value insert list on the left
       *
       * @param area    namespace
       * @param key     name of list
       * @param value   the data which you want push
       * @param expire  expire time about this list, `grep "FOR CLIENT"`
       * @param version version about this list, `grep "FOR CLIENT"`
       *
       * @return  0 -- success
       */
      int lpush(int area, data_entry &key, data_entry *value, int expire, int version);

      /**
       * @brief push value insert list on the left
       *
       * @param area        namespace
       * @param key         name of list
       * @param values      the data list which you want push
       * @param expire      expire time about this list, `grep "FOR CLIENT"`
       * @param version     version about this list, `grep "FOR CLIENT"`
       * @param successlen  if resultcode is not success, it tells you how many values are pushed into list from values' head
       *
       * @return  0 -- success
       */
      int lpush(int area, data_entry &key, vector<data_entry*> &values,
              int expire, int version, long &successlen);
      //rpush
      int rpush(int area, data_entry &key, data_entry *value, int expire, int version);
      int rpush(int area, data_entry &key, vector<data_entry*> &values,
              int expire, int version, long &successlen);

      //lpop
      int lpop(int area, data_entry &key, data_entry **value, int expire, int version);
      int lpop(int area, data_entry &key, int count, vector<data_entry*> &values,
              int expire, int version);
      //rpop
      int rpop(int area, data_entry &key, data_entry **value, int expire, int version);
      int rpop(int area, data_entry &key, int count, vector<data_entry*> &values,
              int expire, int version);
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
      int hset(const int area, const data_entry &key, const data_entry &field,
              const data_entry &value, const int expire, const int version);
      //hget
      int hget(const int area, const data_entry &key, const data_entry &field, data_entry &value);
      //hmset
      int hmset(const int area, const data_entry &key, const map<data_entry*, data_entry*> &field_values,
        map<data_entry*, data_entry*> &field_values_success, const int expire, const int version);
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


      /**
       * @brief incr
       *
       * @param area            namespace
       * @param key             key
       * @param count           add (count) to original value.
       * @param retCount                the latest value
       * @param expire          expire time
       * @param initValue               initialize value
       *
       * @return 0 -- success, otherwise fail.
       */
      int incr(int area,
          const data_entry& key,
          int count,
          int *ret_count,
          int init_value = 0,
          int expire = 0);
      // opposite to incr
      int decr(int area,
          const data_entry& key,
          int count,
          int *ret_count,
          int init_value = 0,
          int expire = 0);

      /**
       *
       * ******this method is deprecated,don't use******
       *
       * @brief   add count
       *
       * @param area            namespace
       * @param key             key
       * @param count           add (count) to original value.
       * @param retCount                the latest value
       * @param initValue               the initialize value
       *
       * @return  0 -- success ,otherwise fail.
       */
      int add_count(int area,
          const data_entry &key,
          int count,
          int *ret_count,
          int init_value = 0);

      /**
       *
       * items :  just support four types: int32_t int64_t double string
       *
       * @brief  add items
       *
       * @param area            namespace
       * @param key             key
       * @param data            data,vector<type>
       * @param version         version,the same as put
       * @param expired         expire time ,the same as put
       * @param max_count               how many items do you want to save
       *
       * @return  0 -- success, otherwise fail
       */
#define ADD_ITEMS(type)                                                 \
      int add_items(int area,const data_entry &key,type data,int version,int expired,int max_count=500)

      ADD_ITEMS(std::vector<int>&);
      ADD_ITEMS(std::vector<int64_t>&);
      ADD_ITEMS(std::vector<double>&);
      ADD_ITEMS(std::vector<std::string>&);


      /**
       * @brief get items
       *
       * @param area            namespace
       * @param key             key
       * @param offset          item's offset
       * @param count           the item's count you want to get,must > 0,
       *                                iff you want to get all items,set count to ALL_ITEMS;
       * @param std::vector       result,you should know the item's type
       *
       * @return  0 -- success, otherwise fail
       */
#define GET_ITEMS_FUNC(FuncName,type)                                   \
      int FuncName(int area, const data_entry &key, int offset, int count, type result)

      GET_ITEMS_FUNC(get_items,std::vector<int>&);
      GET_ITEMS_FUNC(get_items,std::vector<int64_t>&);
      GET_ITEMS_FUNC(get_items,std::vector<double>&);
      GET_ITEMS_FUNC(get_items,std::vector<std::string>&);

      /**
       * @brief get and remove items
       *
       * @param area            namespace
       * @param key             key
       * @param offset          item's offset
       * @param count           the item's count you want to get
       *                                if count >= really items count,this key will be deleted.
       * @param std::vector       result,you should know the item's type
       *
       * @return  0 -- success, otherwise fail
       */

      GET_ITEMS_FUNC(get_and_remove,std::vector<int>&);
      GET_ITEMS_FUNC(get_and_remove,std::vector<int64_t>&);
      GET_ITEMS_FUNC(get_and_remove,std::vector<double>&);
      GET_ITEMS_FUNC(get_and_remove,std::vector<std::string>&);

      /**
       * @brief remove items
       *
       * @param area            namespace
       * @param key             key
       * @param offset          the offset of item
       * @param count           the count of item that you want to remove
       *
       * @return  0 -- success, otherwise -- fail
       */
      int remove_items(int area,
          const data_entry &key,
          int offset,
          int count);

      /**
       * @brief get item count
       *
       * @param area            namespace
       * @param key             key
       *
       * @return  if ret > 0,ret is item's count,else failed.
       */
      int get_items_count(int area,const data_entry& key);

      /**
       * @brief set timout of each method
       *
       * @param timeout
       */
      void set_timeout(int timeout);

      /**
       * @brief get bucket count of tair cluster
       *
       * @return  bucket count
       */
      uint32_t get_bucket_count() const;

      /**
       * @brief get copy count of tair cluster
       *
       * @return  copy count
       */
      uint32_t get_copy_count() const;

      const char *get_error_msg(int ret);

      void get_server_with_key(const data_entry& key,std::vector<std::string>& servers) const;

    //patch:zouyueming: 2014-05-12 agile version enable caller direct access impl.
    public:

      tair_client_impl *impl;

  };
}
#endif
