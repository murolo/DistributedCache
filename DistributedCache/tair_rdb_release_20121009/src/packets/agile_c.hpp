#ifndef _agile_c_agile_c_h_
#define _agile_c_agile_c_h_

//#define DEBUG_PRINTF()    printf( ">line = %d<\n", __LINE__ )
#define DEBUG_PRINTF()

#if defined( WIN32 ) || defined( WIN64 )
    #include <winsock2.h>
    #include <Windows.h>
#else
    #include <dlfcn.h>
#endif
#include <stdlib.h> // for calloc
#include <stddef.h> // for size_t
#include <stdio.h>  // printf
#include <errno.h>
#ifdef __cplusplus
    #include <string>
    #include <vector>
    #include <map>
    
    typedef std::map< std::string, std::string >    data_map_t;
    typedef std::vector< std::string >              data_list_t;
    typedef std::map< std::string, double >         score_map_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/// ANGILE_C_VERSION 宏用于指定当前agile_c的调用接口版本号，  
/// 只有在新增了接口或接口有变动时才有必要递增此值。  
#define ANGILE_C_VERSION        1
/// 当前可以支持的调用方最低版本号，如果调用方版本号小于此值，模块将初始化失败  
#define ANGILE_C_LOW_VERSION    ANGILE_C_VERSION

        struct agile_c_t;
typedef struct agile_c_t   agile_c_t;

// ENOENT           = not found
// EVERSION         = version error
// ECONNREFUSED     = connect failed
// EINVAL           = invalid params
// ETIMEDOUT        = timeout
// EPARTIAL         = partial success
// EFAULT           = unknown error
// ENOBUFS          = buffer max length limit
#define EVERSION        10000
#define EEXPIRED        10001
#define EPARTIAL        10002

///////////////////////////////////////////////////////////////////////
//
// agile c declare
//

typedef struct agile_data_t
{
    void *  data;
    int     len;

} agile_data_t;

/**
 * @brief 取错误码描述，由于有一些自定义的错误码，所以需要此函数，使用方法和strerror一样  
 * @param[in] errid : 错误码，正数负数都支持。  
 * @return const char *: 针对错误码的描述信息，调用方不需要释放  
 */
static
const char *
agile_c_strerror(
    agile_c_t *     self,
    int             r
);

/**
 * @brief 创建agile_c 模块的实例  
 * @param[in] agile_c_version : 必须填 ANGILE_C_VERSION  
 * @param[in] agile_so_path   : agile_c 的 so 路径，必须\0结束  
 * @return agile_c_t *: 如果成功，返回有效指针，否则返回空指针。  
 */
static
agile_c_t *
agile_c_new(
    int             agile_c_version,
    const char *    agile_so_path
);

/**
 * @brief 删除agile_c 模块的实例  
 * @param[in] self : agile c 模块实例，删除实例前会自动调用 agile_c_close 函数  
 */
static
void
agile_c_delete(
    agile_c_t *    self
);

/**
 * @brief 打开数据接口  
 * @param[in] self : agile c 模块实例  
 * @param[in] cert_server_1_addr     : 配置服务器1 地址，格式：IP:PORT，不支持域名。  
 * @param[in] cert_server_1_addr_len : cert_server_1_addr 长度，不包括\0  
 * @param[in] cert_server_2_addr     : 配置服务器2 地址，格式：IP:PORT，不支持域名。  
 * @param[in] cert_server_2_addr_len : cert_server_2_addr 长度，不包括\0  
 × @param[in] group_name             : 组名字，不允许空  
 * @param[in] group_name_len         : 组名字长度，不包括 \0  
 * @return int: 0=ok, error otherwise
 */
static
int
agile_c_open(
    agile_c_t *    self,
    const char *    cert_server_1_addr,
    int             cert_server_1_addr_len,
    const char *    cert_server_2_addr,
    int             cert_server_2_addr_len,
    const char *    group_name,
    int             group_name_len
);

/**
 * @brief 关闭数据或管理接口，以便再次调 agile_c_open 或 agile_c_admin_open。
 * @param[in] self : agile c 模块实例  
 */
static
void
agile_c_close(
    agile_c_t *    self
);

/**
 * @brief put  
 * @param[in]  self    : agile c 模块实例  
 * @params[in] area    : 分区编号, base 0
 * @params[in] key     : key，二进制数据  
 * @params[in] key_len : key 字节数  
 * @params[in] data    : 数据，二进制数据  
 * @params[in] data_len: 数据字节数  
 * @params[in] expire  : 过期时间毫秒数  
 * @params[in] version : 版本号，如果不使用，可以填0，但下次取出来时是1  
 * @return int: 0=ok, error otherwise，如果数据已经存在，则直接覆盖并返回0
 */
static
int
agile_c_put(
    agile_c_t *     self,
    int             area,
    const void *    key,
    int             key_len,
    const void *    data,
    int             data_len,
    int             expire,
    int             version
);

/**
 * @brief get  
 * @param[in]  self    : agile c 模块实例  
 * @params[in] area    : 分区编号, base 0
 * @params[in] key     : key，二进制数据  
 * @params[in] key_len : key 字节数  
 * @params[out]data    : 返回数据，二进制数据  
 * @params[in/out]data_len: 传入数据缓冲字节数，传出实际写入字节数  
 * @params[out]version : 记录版本号  
 * @return int: 0=ok, error otherwise
 *              没找到返回 - ENOENT
 */
static
int
agile_c_get(
    agile_c_t *     self,
    int             area,
    const void *    key,
    int             key_len,
    void *          data,
    int *           data_len,
    int *           version
);

#ifdef __cplusplus

/**
 * @brief 批量get  
 * @param[in]  self       : agile c 模块实例  
 * @params[in] area       : 分区编号, base 0  
 * @params[in] keys       : key列表，二进制数据  
 * @params[in] keys_count : key列表数量  
 * @params[out]data       : 返回数据，是个map，first即是keys中的key，second 是数据  
 * @return int: 0=ok, error otherwise
 *              没一个找到返回 - ENOENT
 */
static
int
agile_c_mget(
    agile_c_t *             self,
    int                     area,
    const agile_data_t *    keys,
    int                     keys_count,
    data_map_t &            data
);

#endif // #ifdef __cplusplus

/**
 * @brief 删除一条数据  
 * @param[in]  self       : agile c 模块实例  
 * @params[in] area       : 分区编号, base 0  
 * @params[in] key        : key，二进制数据  
 * @params[in] key_len    : key字节数  
 * @return int: 0=ok, error otherwise
 *              没找到返回 - ENOENT
 */
static
int
agile_c_del(
    agile_c_t *     self,
    int             area,
    const void *    key,
    int             key_len
);

#ifdef __cplusplus

static
int
agile_c_mdel(
    agile_c_t *             self,
    int                     area,
    const agile_data_t *    keys,
    int                     keys_count
);

#endif // #ifdef __cplusplus

/**
 * @brief 清除一个area的所有数据，但我觉得有问题，连续调用两次，第二次就超时，不建议使用！  
 * @param[in]  self       : agile c 模块实例  
 * @params[in] area       : 分区编号, base 0  
 * @return int: 0=ok, error otherwise
 */
static
int
agile_c_clear(
    agile_c_t *     self,
    int             area
);

static
int
agile_c_lpush(
    agile_c_t *     self,
    int             area,
    const void *    key,
    int             key_len,
    const void *    data,
    int             data_len,
    int             expire = 0
);

static
int
agile_c_rpush(
    agile_c_t *     self,
    int             area,
    const void *    key,
    int             key_len,
    const void *    data,
    int             data_len,
    int             expire = 0
);

static
int
agile_c_lpop(
    agile_c_t *     self,
    int             area,
    const void *    key,
    int             key_len,
    void *          data,
    int *           data_len,
    int             expire = 0
);

static
int
agile_c_rpop(
    agile_c_t *     self,
    int             area,
    const void *    key,
    int             key_len,
    void *          data,
    int *           data_len,
    int             expire = 0
);

static
int
agile_c_lindex(
    agile_c_t *     self,
    int             area,
    const void *    key,
    int             key_len,
    int             index,
    void *          data,
    int *           data_len
);

#ifdef __cplusplus

static
int
agile_c_lrange(
    agile_c_t *     self,
    int             area,
    const void *    key,
    int             key_len,
    int             start,
    int             stop,
    data_list_t &   data
);

#endif // #ifdef __cplusplus

static
int
agile_c_lset(
    agile_c_t *     self,
    int             area,
    const void *    key,
    int             key_len,
    int             index,
    const void *    data,
    int             data_len,
    int             expire = 0
);

static
int
agile_c_llen(
    agile_c_t *     self,
    int             area,
    const void *    key,
    int             key_len,
    int *           length
);

static
int
agile_c_ltrim(
    agile_c_t *     self,
    int             area,
    const void *    key,
    int             key_len,
    int             start,
    int             end,
    int             expire = 0
);

static
int
agile_c_sadd(
    agile_c_t *     self,
    int             area,
    const void *    key,
    int             key_len,
    const void *    data,
    int             data_len,
    int             expire = 0
);

#ifdef __cplusplus

static
int
agile_c_smembers(
    agile_c_t *     self,
    int             area,
    const void *    key,
    int             key_len,
    data_list_t &   data
);

#endif // #ifdef __cplusplus

static
int
agile_c_srem(
    agile_c_t *     self,
    int             area,
    const void *    key,
    int             key_len,
    const void *    data,
    int             data_len,
    int             expire = 0
);

static
int
agile_c_hset(
    agile_c_t *     self,
    int             area,
    const void *    key,
    int             key_len,
    const void *    field,
    int             field_len,
    const void *    data,
    int             data_len,
    int             expire = 0
);

static
int
agile_c_hget(
    agile_c_t *     self,
    int             area,
    const void *    key,
    int             key_len,
    const void *    field,
    int             field_len,
    void *          data,
    int *           data_len
);

#ifdef __cplusplus

static
int
agile_c_hmget(
    agile_c_t *             self,
    int                     area,
    const void *            key,
    int                     key_len,
    data_map_t &            data
);

static
int
agile_c_hmset(
    agile_c_t *             self,
    int                     area,
    const void *            key,
    int                     key_len,
    const data_map_t &      data,
    int                     expire = 0
);

static
int
agile_c_hgetall(
    agile_c_t *             self,
    int                     area,
    const void *            key,
    int                     key_len,
    const data_map_t &      data
);

#endif // #ifdef __cplusplus

#define agile_c_hrem    agile_c_hdel

static
int
agile_c_hdel(
    agile_c_t *     self,
    int             area,
    const void *    key,
    int             key_len,
    const void *    field,
    int             field_len,
    int             expire = 0
);

static
int
agile_c_zadd(
    agile_c_t *     self,
    int             area,
    const void *    key,
    int             key_len,
    const void *    data,
    int             data_len,
    double          score,
    int             expire = 0
);

static
int
agile_c_zcard(
    agile_c_t *     self,
    int             area,
    const void *    key,
    int             key_len,
    long long *     retnum
);

static
int
agile_c_zrem(
    agile_c_t *     self,
    int             area,
    const void *    key,
    int             key_len,
    const void *    data,
    int             data_len,
    int             expire = 0
);

#ifdef __cplusplus

static
int
agile_c_zrange(
    agile_c_t *     self,
    int             area,
    const void *    key,
    int             key_len,
    int             start,
    int             end,
    bool            withscore,
    score_map_t &   valuescores
);

static
int
agile_c_zrangebyscore(
    agile_c_t *     self,
    int             area,
    const void *    key,
    int             key_len,
    double          start,
    double          end,
    bool            reverse,
    int             limit,
    bool            withscore,
    score_map_t &   valuescores    
);

#endif // #ifdef __cplusplus

static
int
agile_c_zremrangebyscore(
    agile_c_t *     self,
    int             area,
    const void *    key,
    int             key_len,
    double          start,
    double          end,
    long long *     retnum,
    int             expire = 0
);

static
int
agile_c_zremrangebyrank(
    agile_c_t *     self,
    int             area,
    const void *    key,
    int             key_len,
    int             start,
    int             end,
    long long *     retnum,
    int             expire = 0
);

///////////////////////////////////////////////////////////////////////
//
// agile c interface implementation
//

typedef int ( * func_agile_impl_t )( int command, void ** params, int params_count );

typedef enum agile_c_command_t
{
      ANGILE_new
    , ANGILE_delete
    , ANGILE_open
    , ANGILE_close
    , ANGILE_put
    , ANGILE_get
    , ANGILE_mget
    , ANGILE_del
    , ANGILE_mdel
    , ANGILE_clear
    , ANGILE_lpush
    , ANGILE_rpush
    , ANGILE_lpop
    , ANGILE_rpop
    , ANGILE_lindex
    , ANGILE_lrange
    , ANGILE_lset
    , ANGILE_llen
    , ANGILE_ltrim
    , ANGILE_sadd
    , ANGILE_smembers
    , ANGILE_srem
    , ANGILE_hset
    , ANGILE_hget
    , ANGILE_hmget
    , ANGILE_hmset
    , ANGILE_hgetall
    , ANGILE_hdel
    , ANGILE_zadd
    , ANGILE_zcard
    , ANGILE_zrem
    , ANGILE_zrange
    , ANGILE_zrangebyscore
    , ANGILE_zremrangebyscore
    , ANGILE_zremrangebyrank
    , ANGILE_strerror

} agile_c_command_t;

typedef struct agile_c_t
{
    // caller ANGILE_C_VERSION
    int                 caller_version;

    // callee ANGILE_C_VERSION
    int                 callee_version;

    // callee ANGILE_C_LOW_VERSION
    int                 low_version;

    char                _reserved[ 4 ];

    // agile so
    void *              so;
    
    // agile implementation
    void *              impl;

    // export function
    func_agile_impl_t  func;

} agile_c_t;

#define ANGILE_C_FUNC_NAME  "agile_c"

static inline
agile_c_t *
agile_c_new(
    int             agile_c_version,
    const char *    agile_so_path
)
{
    agile_c_t * result = NULL;

#if defined( WIN32 ) || defined( WIN64 )
    HINSTANCE dll = LoadLibraryA( agile_so_path );
    if ( NULL == dll ) {
        return NULL;
    }

    void * p = GetProcAddress( dll, ANGILE_C_FUNC_NAME );
    if ( NULL == p ) {
        FreeLibrary( dll );
        return NULL;
    }
#else
    int flags = RTLD_NOW;
    #ifdef _AIX
    flags |= RTLD_MEMBER;
    #endif

    DEBUG_PRINTF();
    void * dll = dlopen( agile_so_path, flags );
    DEBUG_PRINTF();
    if ( NULL == dll ) {
        printf( "dlopen failed: %s\n", dlerror() );
        DEBUG_PRINTF();
        return NULL;
    }

    DEBUG_PRINTF();
    void * p = dlsym( dll, ANGILE_C_FUNC_NAME );
    DEBUG_PRINTF();
    if ( NULL == p ) {
        DEBUG_PRINTF();
        dlclose( dll );
        return NULL;
    }
#endif

    int r = -999;

    do {
        DEBUG_PRINTF();
        result = (agile_c_t *)calloc( 1, sizeof( agile_c_t ) );
        DEBUG_PRINTF();
        if ( NULL == result ) {
            DEBUG_PRINTF();
            r = -998;
            break;
        }

        result->so              = dll;
        result->func            = (func_agile_impl_t)p;
        result->caller_version  = ANGILE_C_VERSION;

        void * params[ 1 ];
        params[ 0 ] = result;
        DEBUG_PRINTF();
        r = result->func( ANGILE_new, (void **)& params, sizeof(params)/sizeof(params[0]) );
        if ( 0 != r ) {
            DEBUG_PRINTF();
            break;
        }
   
        DEBUG_PRINTF();
        r = 0;
    } while ( 0 );

    if ( 0 != r ) {
        DEBUG_PRINTF();
        if ( result ) {
            DEBUG_PRINTF();
            free( result );
        }
#if defined( WIN32 ) || defined( WIN64 )
        FreeLibrary( dll );
#else
        DEBUG_PRINTF();
        dlclose( dll );
#endif
        DEBUG_PRINTF();
        return NULL;
    }

    DEBUG_PRINTF();
    return result;
}

static inline
void
agile_c_delete(
    agile_c_t *    self
)
{
    if ( self ) {
#if defined( WIN32 ) || defined( WIN64 )
        HINSTANCE dll = (HINSTANCE)self->so;
#else
        void * dll = self->so;
#endif

        void * params[ 1 ];
        params[ 0 ] = self;
        self->func( ANGILE_delete, (void **)& params, sizeof(params)/sizeof(params[0]) );

        free( self );

#if defined( WIN32 ) || defined( WIN64 )
        FreeLibrary( dll );
#else
        dlclose( dll );
#endif
    }
}

static inline
int
agile_c_open(
    agile_c_t *    self,
    const char *    cert_server_1_addr,
    int             cert_server_1_addr_len,
    const char *    cert_server_2_addr,
    int             cert_server_2_addr_len,
    const char *    group_name,
    int             group_name_len
)
{
    if ( self ) {
        void * params[ 7 ];
        params[ 0 ] = self;
        params[ 1 ] = (void*)cert_server_1_addr;
        params[ 2 ] = & cert_server_1_addr_len;
        params[ 3 ] = (void*)cert_server_2_addr;
        params[ 4 ] = & cert_server_2_addr_len;
        params[ 5 ] = (void*)group_name;
        params[ 6 ] = & group_name_len;

        return self->func( ANGILE_open, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

static inline
void
agile_c_close(
    agile_c_t *    self
)
{
    if ( self ) {
        void * params[ 1 ];
        params[ 0 ] = self;
        self->func( ANGILE_close, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }
}

static inline
int
agile_c_put(
    agile_c_t *    self,
    int             area,
    const void *    key,
    int             key_len,
    const void *    data,
    int             data_len,
    int             expire,
    int             version
)
{
    if ( self ) {
        void * params[ 8 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)key;
        params[ 3 ] = & key_len;
        params[ 4 ] = (void *)data;
        params[ 5 ] = & data_len;
        params[ 6 ] = & expire;
        params[ 7 ] = & version;
        return self->func( ANGILE_put, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

static inline
int
agile_c_get(
    agile_c_t *    self,
    int             area,
    const void *    key,
    int             key_len,
    void *          data,
    int *           data_len,
    int *           version
)
{
    if ( self ) {
        void * params[ 7 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)key;
        params[ 3 ] = & key_len;
        params[ 4 ] = (void *)data;
        params[ 5 ] = data_len;
        params[ 6 ] = version;
        return self->func( ANGILE_get, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

#ifdef __cplusplus

static inline
int
agile_c_mget(
    agile_c_t *            self,
    int                     area,
    const agile_data_t *   keys,
    int                     keys_count,
    data_map_t &            data
)
{
    if ( self ) {
        void * params[ 5 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)keys;
        params[ 3 ] = & keys_count;
        params[ 4 ] = & data;
        return self->func( ANGILE_mget, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

#endif // #ifdef __cplusplus

static inline
int
agile_c_del(
    agile_c_t *    self,
    int             area,
    const void *    key,
    int             key_len
)
{
    if ( self ) {
        void * params[ 4 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)key;
        params[ 3 ] = & key_len;
        return self->func( ANGILE_del, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

#ifdef __cplusplus

static inline
int
agile_c_mdel(
    agile_c_t *             self,
    int                     area,
    const agile_data_t *    keys,
    int                     keys_count
)
{
    if ( self ) {
        void * params[ 4 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)keys;
        params[ 3 ] = & keys_count;
        return self->func( ANGILE_mdel, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

#endif // #ifdef __cplusplus

static inline
int
agile_c_clear(
    agile_c_t *    self,
    int             area
)
{
    if ( self ) {
        void * params[ 2 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        return self->func( ANGILE_clear, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

static inline
int
agile_c_lpush(
    agile_c_t *    self,
    int             area,
    const void *    key,
    int             key_len,
    const void *    data,
    int             data_len,
    int             expire
)
{
    if ( self ) {
        void * params[ 7 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)key;
        params[ 3 ] = & key_len;
        params[ 4 ] = (void *)data;
        params[ 5 ] = & data_len;
        params[ 6 ] = & expire;
        return self->func( ANGILE_lpush, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

static inline
int
agile_c_rpush(
    agile_c_t *    self,
    int             area,
    const void *    key,
    int             key_len,
    const void *    data,
    int             data_len,
    int             expire
)
{
    if ( self ) {
        void * params[ 7 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)key;
        params[ 3 ] = & key_len;
        params[ 4 ] = (void *)data;
        params[ 5 ] = & data_len;
        params[ 6 ] = & expire;
        return self->func( ANGILE_rpush, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

static inline
int
agile_c_lpop(
    agile_c_t *    self,
    int             area,
    const void *    key,
    int             key_len,
    void *          data,
    int *           data_len,
    int             expire
)
{
    if ( self ) {
        void * params[ 7 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)key;
        params[ 3 ] = & key_len;
        params[ 4 ] = (void *)data;
        params[ 5 ] = data_len;
        params[ 6 ] = & expire;
        return self->func( ANGILE_lpop, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

static inline
int
agile_c_rpop(
    agile_c_t *    self,
    int             area,
    const void *    key,
    int             key_len,
    void *          data,
    int *           data_len,
    int             expire
)
{
    if ( self ) {
        void * params[ 7 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)key;
        params[ 3 ] = & key_len;
        params[ 4 ] = (void *)data;
        params[ 5 ] = data_len;
        params[ 6 ] = & expire;
        return self->func( ANGILE_rpop, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

static inline
int
agile_c_lindex(
    agile_c_t *    self,
    int             area,
    const void *    key,
    int             key_len,
    int             index,
    void *          data,
    int *           data_len
)
{
    if ( self ) {
        void * params[ 7 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)key;
        params[ 3 ] = & key_len;
        params[ 4 ] = & index;
        params[ 5 ] = (void *)data;
        params[ 6 ] = data_len;
        return self->func( ANGILE_lindex, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

#ifdef __cplusplus

static inline
int
agile_c_lrange(
    agile_c_t *    self,
    int             area,
    const void *    key,
    int             key_len,
    int             start,
    int             stop,
    data_list_t &   data
)
{
    if ( self ) {
        void * params[ 7 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)key;
        params[ 3 ] = & key_len;
        params[ 4 ] = & start;
        params[ 5 ] = & stop;
        params[ 6 ] = & data;
        return self->func( ANGILE_lrange, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

#endif // #ifdef __cplusplus

static inline
int
agile_c_lset(
    agile_c_t *    self,
    int             area,
    const void *    key,
    int             key_len,
    int             index,
    const void *    data,
    int             data_len,
    int             expire
)
{
    if ( self ) {
        void * params[ 8 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)key;
        params[ 3 ] = & key_len;
        params[ 4 ] = & index;
        params[ 5 ] = (void *)data;
        params[ 6 ] = & data_len;
        params[ 7 ] = & expire;
        return self->func( ANGILE_lset, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

static inline
int
agile_c_llen(
    agile_c_t *    self,
    int             area,
    const void *    key,
    int             key_len,
    int *           length
)
{
    if ( self ) {
        void * params[ 5 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)key;
        params[ 3 ] = & key_len;
        params[ 4 ] = length;
        return self->func( ANGILE_llen, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

static inline
int
agile_c_ltrim(
    agile_c_t *    self,
    int             area,
    const void *    key,
    int             key_len,
    int             start,
    int             end,
    int             expire
)
{
    if ( self ) {
        void * params[ 7 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)key;
        params[ 3 ] = & key_len;
        params[ 4 ] = & start;
        params[ 5 ] = & end;
        params[ 6 ] = & expire;
        return self->func( ANGILE_ltrim, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

static inline
int
agile_c_sadd(
    agile_c_t *    self,
    int             area,
    const void *    key,
    int             key_len,
    const void *    data,
    int             data_len,
    int             expire
)
{
    if ( self ) {
        void * params[ 7 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)key;
        params[ 3 ] = & key_len;
        params[ 4 ] = (void *)data;
        params[ 5 ] = & data_len;
        params[ 6 ] = & expire;
        return self->func( ANGILE_sadd, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

#ifdef __cplusplus

static inline
int
agile_c_smembers(
    agile_c_t *    self,
    int             area,
    const void *    key,
    int             key_len,
    data_list_t &   data
)
{
    if ( self ) {
        void * params[ 5 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)key;
        params[ 3 ] = & key_len;
        params[ 4 ] = & data;
        return self->func( ANGILE_smembers, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

#endif // #ifdef __cplusplus

static inline
int
agile_c_srem(
    agile_c_t *    self,
    int             area,
    const void *    key,
    int             key_len,
    const void *    data,
    int             data_len,
    int             expire
)
{
    if ( self ) {
        void * params[ 7 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)key;
        params[ 3 ] = & key_len;
        params[ 4 ] = (void *)data;
        params[ 5 ] = & data_len;
        params[ 6 ] = & expire;
        return self->func( ANGILE_srem, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

static inline
int
agile_c_hset(
    agile_c_t *    self,
    int             area,
    const void *    key,
    int             key_len,
    const void *    field,
    int             field_len,
    const void *    data,
    int             data_len,
    int             expire
)
{
    if ( self ) {
        void * params[ 9 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)key;
        params[ 3 ] = & key_len;
        params[ 4 ] = (void *)field;
        params[ 5 ] = & field_len;
        params[ 6 ] = (void *)data;
        params[ 7 ] = & data_len;
        params[ 8 ] = & expire;
        return self->func( ANGILE_hset, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

static inline
int
agile_c_hget(
    agile_c_t *    self,
    int             area,
    const void *    key,
    int             key_len,
    const void *    field,
    int             field_len,
    void *          data,
    int *           data_len
)
{
    if ( self ) {
        void * params[ 8 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)key;
        params[ 3 ] = & key_len;
        params[ 4 ] = (void *)field;
        params[ 5 ] = & field_len;
        params[ 6 ] = (void *)data;
        params[ 7 ] = data_len;
        return self->func( ANGILE_hget, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

#ifdef __cplusplus

static inline
int
agile_c_hmget(
    agile_c_t *            self,
    int                     area,
    const void *            key,
    int                     key_len,
    data_map_t &            data
)
{
    if ( self ) {
        void * params[ 5 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)key;
        params[ 3 ] = & key_len;
        params[ 4 ] = & data;
        return self->func( ANGILE_hmget, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

static inline
int
agile_c_hmset(
    agile_c_t *            self,
    int                     area,
    const void *            key,
    int                     key_len,
    const data_map_t &      data,
    int                     expire
)
{
    if ( self ) {
        void * params[ 6 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)key;
        params[ 3 ] = & key_len;
        params[ 4 ] = (void *)& data;
        params[ 5 ] = & expire;
        return self->func( ANGILE_hmset, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

static inline
int
agile_c_hgetall(
    agile_c_t *             self,
    int                     area,
    const void *            key,
    int                     key_len,
    const data_map_t &      data
)
{
    if ( self ) {
        void * params[ 5 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)key;
        params[ 3 ] = & key_len;
        params[ 4 ] = (void *)& data;
        return self->func( ANGILE_hgetall, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

#endif // #ifdef __cplusplus

static inline
int
agile_c_hdel(
    agile_c_t *     self,
    int             area,
    const void *    key,
    int             key_len,
    const void *    field,
    int             field_len,
    int             expire
)
{
    if ( self ) {
        void * params[ 7 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)key;
        params[ 3 ] = & key_len;
        params[ 4 ] = (void *)field;
        params[ 5 ] = & field_len;
        params[ 6 ] = & expire;
        return self->func( ANGILE_hdel, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

static inline
int
agile_c_zadd(
    agile_c_t *    self,
    int             area,
    const void *    key,
    int             key_len,
    const void *    data,
    int             data_len,
    double          score,
    int             expire
)
{
    if ( self ) {
        void * params[ 8 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)key;
        params[ 3 ] = & key_len;
        params[ 4 ] = (void *)data;
        params[ 5 ] = & data_len;
        params[ 6 ] = & score;
        params[ 7 ] = & expire;
        return self->func( ANGILE_zadd, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

static inline
int
agile_c_zcard(
    agile_c_t *    self,
    int             area,
    const void *    key,
    int             key_len,
    long long *     retnum
)
{
    if ( self ) {
        void * params[ 5 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)key;
        params[ 3 ] = & key_len;
        params[ 4 ] = retnum;
        return self->func( ANGILE_zcard, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

static inline
int
agile_c_zrem(
    agile_c_t *    self,
    int             area,
    const void *    key,
    int             key_len,
    const void *    data,
    int             data_len,
    int             expire
)
{
    if ( self ) {
        void * params[ 7 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)key;
        params[ 3 ] = & key_len;
        params[ 4 ] = (void *)data;
        params[ 5 ] = & data_len;
        params[ 6 ] = & expire;
        return self->func( ANGILE_zrem, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

#ifdef __cplusplus

static inline
int
agile_c_zrange(
    agile_c_t *    self,
    int             area,
    const void *    key,
    int             key_len,
    int             start,
    int             end,
    bool            withscore,
    score_map_t &   valuescores
)
{
    if ( self ) {
        void * params[ 8 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)key;
        params[ 3 ] = & key_len;
        params[ 4 ] = & start;
        params[ 5 ] = & end;
        params[ 6 ] = & withscore;
        params[ 7 ] = & valuescores;
        return self->func( ANGILE_zrange, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

static inline
int
agile_c_zrangebyscore(
    agile_c_t *    self,
    int             area,
    const void *    key,
    int             key_len,
    double          start,
    double          end,
    bool            reverse,
    int             limit,
    bool            withscore,
    score_map_t &   valuescores
)
{
    if ( self ) {
        void * params[ 10 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)key;
        params[ 3 ] = & key_len;
        params[ 4 ] = & start;
        params[ 5 ] = & end;
        params[ 6 ] = & reverse;
        params[ 7 ] = & limit;
        params[ 8 ] = & withscore;
        params[ 9 ] = & valuescores;
        return self->func( ANGILE_zrangebyscore, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

#endif // #ifdef __cplusplus

static inline
int
agile_c_zremrangebyscore(
    agile_c_t *    self,
    int             area,
    const void *    key,
    int             key_len,
    double          start,
    double          end,
    long long *     retnum,
    int             expire
)
{
    if ( self ) {
        void * params[ 8 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)key;
        params[ 3 ] = & key_len;
        params[ 4 ] = & start;
        params[ 5 ] = & end;
        params[ 6 ] = & expire;
        params[ 7 ] = retnum;
        return self->func( ANGILE_zremrangebyscore, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

static inline
int
agile_c_zremrangebyrank(
    agile_c_t *    self,
    int             area,
    const void *    key,
    int             key_len,
    int             start,
    int             end,
    long long *     retnum,
    int             expire
)
{
    if ( self ) {
        void * params[ 8 ];
        params[ 0 ] = self;
        params[ 1 ] = & area;
        params[ 2 ] = (void *)key;
        params[ 3 ] = & key_len;
        params[ 4 ] = & start;
        params[ 5 ] = & end;
        params[ 6 ] = & expire;
        params[ 7 ] = retnum;
        return self->func( ANGILE_zremrangebyrank, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

static inline
const char *
agile_c_strerror(
    agile_c_t *     self,
    int             r
)
{
    const char * ret = "(unknown)";
    if ( self ) {
        void * params[ 2 ];
        params[ 0 ] = & r;
        params[ 1 ] = (void *)& ret;
        self->func( ANGILE_strerror, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return ret;
}

#ifdef __cplusplus
}
#endif

#endif // #ifndef _agile_c_agile_c_h_
