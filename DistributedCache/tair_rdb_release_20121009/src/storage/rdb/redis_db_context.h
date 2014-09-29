#ifndef REDIS_DB_CONTEXT_H
#define REDIS_DB_CONTEXT_H

#include "redis/redislib.h"
#include <tbsys.h>
#include <Mutex.h>

#include "redis_define.h"

BEGIN_NS

class redis_server_cron;


class redis_db_context
{
public:
    redis_db_context();
    redis_db_context(const redisConfig &config);
	  virtual ~redis_db_context();

    void start();
    void stop();

    void set_db_maxmemory(int dbid, uint64_t quota);
    uint64_t get_maxmemory();
    static uint64_t get_db_used_maxmemory(uint32_t db);

    size_t evict_count(uint32_t dbnum) const;
    void reset_evict_count(uint32_t dbnum);

    size_t expired_count(uint32_t dbnum) const;
    void reset_expired_count(uint32_t dbnum);

    size_t item_count(uint32_t dbnum) const;

    int get_read_count(uint32_t dbnum) const;
    void reset_read_count(uint32_t dbnum);

    int get_write_count(uint32_t dbnum) const;
    void reset_write_count(uint32_t dbnum);

    int get_hit_count(uint32_t dbnum) const;
    void reset_hit_count(uint32_t dbnum);

    int get_remove_count(uint32_t dbnum) const;
    void reset_remove_count(uint32_t dbnum);
public:
    int get_unit_num() {return unit_num;}
    int get_area_num() {return area_group_num;}
public:
    void set_list_max_size(int size);
    void set_hash_max_size(int size);
    void set_zset_max_size(int size);
    void set_set_max_size(int size);
    void set_max_evict_each_time(int size);
    void set_timed_task_time_range(int start, int end);
    void set_timed_task_clean_times(int times);

    int get_list_max_size() const;
    int get_hash_max_size() const;
    int get_zset_max_size() const;
    int get_set_max_size() const;
    int get_max_evict_each_time() const;
    void get_timed_task_time_range(int* start, int* end) const;
    int get_timed_task_clean_times() const;

    int get_area_detail_info(int index,
        long long* keys,
        long long* vkeys,
        long long* slots,
        size_t* usedmemory,
        size_t* maxmemory);
private:
    redisServer *create_context();
    redisServer *create_context(const redisConfig &config);
    void destroy_context(redisServer *server);
private:
    redis_server_cron *server_cron;
    redisServer *server;
    int area_group_num;
    int unit_num;
    //int mutex_num;
    friend class redis_db_session;
    friend class redis_server_cron;
private:
    static bool inited;
    static void initialize_shared_data();
};

END_NS

#endif
