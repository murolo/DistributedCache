#ifndef _agile_c_agile_c_admin_h_
#define _agile_c_agile_c_admin_h_

#include "agile_c.hpp"
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////
//
// agile c admin declare
//

typedef struct agile_server_t
{
    /// 服务器主机，可以是域名可以是IP，但暂只支持IPV4  
    char                    host[ 128 ];

    /// 服务器端口  
    unsigned short          port;

} agile_server_t;

typedef struct agile_area_info_t
{
    /// 分区配额（M字节为单位）  
    unsigned int                limit_m;

} agile_area_info_t;

typedef std::vector< agile_server_t >      server_array_t;
typedef std::vector< agile_area_info_t >   area_array_t;
typedef std::vector< std::string >         name_array_t;


/**
 * @brief 打开管理接口  
 * @param[in] self : agile c 模块实例  
 * @param[in] cert_server_1_addr     : 配置服务器1 地址，格式：IP:PORT，不支持域名。  
 * @param[in] cert_server_1_addr_len : cert_server_1_addr 长度，不包括\0  
 * @param[in] cert_server_2_addr     : 配置服务器2 地址，格式：IP:PORT，不支持域名。  
 * @param[in] cert_server_2_addr_len : cert_server_2_addr 长度，不包括\0  
 * @return int: 0=ok, error otherwise
 */
static
int
agile_c_admin_open(
    agile_c_t *     self,
    const char *    cert_server_1_addr,
    int             cert_server_1_addr_len,
    const char *    cert_server_2_addr,
    int             cert_server_2_addr_len
);

/**
 * @brief 取得组名称列表  
 * @param[in] self : agile c 模块实例  
 * @param[out]    group_names     : 名称列表  
 * @return int: 0=ok, error otherwise
 */
static
int
agile_c_admin_get_group_names(
    agile_c_t *                 self,
    name_array_t &              groups
);

/**
 * @brief 根据组名称查询组配置  
 * @param[in] self : agile c 模块实例  
 * @param[in] group_name     : 组名  
 * @param[in] group_name_len : 组名称长度（不包括\0）  
 * @param[out]group_limit_m  : 组总配额（以M字节为单位）  
 * @param[out]servers        : 组内数据服务器列表  
 * @param[out]areas          : 组内分区配置列表  
 * @return int: 0=ok, error otherwise
 */
static
int
agile_c_admin_get_group_info(
    agile_c_t *                 self,
    const char *                group_name,
    int                         group_name_len,
    unsigned int &              group_limit_m,
    server_array_t &            servers,
    area_array_t &              areas
);

/**
 * @brief 根据组名称设置组配置  
 * @param[in] self : agile c 模块实例  
 * @param[in] group_name     : 组名  
 * @param[in] group_name_len : 组名称长度（不包括\0）  
  * @param[in] group_limit_m  : 组总配额（以M字节为单位）  
 * @param[in] servers        : 组内数据服务器列表  
 * @param[in] areas          : 组内分区配置列表  
 * @return int: 0=ok, error otherwise
 */
static
int
agile_c_admin_set_group_info(
    agile_c_t *                 self,
    const char *                group_name,
    int                         group_name_len,
    unsigned int                group_limit_m,
    const server_array_t &      servers,
    const area_array_t &        areas
);

/**
 * @brief 删除组   
 * @param[in] self : agile c 模块实例  
 * @param[in] group_name     : 组名称  
 * @param[in] group_name_len : 组名称长度（不包括\0）   
 * @return int: 0=ok, error otherwise
 */
static
int
agile_c_admin_del_group(
    agile_c_t *                 self,
    const char *                group_name,
    int                         group_name_len
);

///////////////////////////////////////////////////////////////////////
//
// agile c admin interface implementation
//

typedef enum agile_c_admin_command_t
{
#define ADGILE_ADMIN_COMMAND_BEGIN  10000
    ANGILE_ADMIN_get_group_names = ADGILE_ADMIN_COMMAND_BEGIN,
    ANGILE_ADMIN_get_group_info,
    ANGILE_ADMIN_set_group_info,
    ANGILE_ADMIN_del_group,
    ANGILE_ADMIN_open
} agile_c_admin_command_t;

static inline
int
agile_c_admin_open(
    agile_c_t *     self,
    const char *    cert_server_1_addr,
    int             cert_server_1_addr_len,
    const char *    cert_server_2_addr,
    int             cert_server_2_addr_len
)
{
    if ( self ) {
        void * params[ 5 ];
        params[ 0 ] = self;
        params[ 1 ] = (void *)cert_server_1_addr;
        params[ 2 ] = & cert_server_1_addr_len;
        params[ 3 ] = (void *)cert_server_2_addr;
        params[ 4 ] = & cert_server_2_addr_len;
        return self->func( ANGILE_ADMIN_open, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

static inline
int
agile_c_admin_get_group_names(
    agile_c_t *                 self,
    name_array_t &              groups
)
{
    if ( self ) {
        void * params[ 2 ];
        params[ 0 ] = self;
        params[ 2 ] = & groups;
        return self->func( ANGILE_ADMIN_get_group_names, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

static inline
int
agile_c_admin_get_group_info(
    agile_c_t *                 self,
    const char *                group_name,
    int                         group_name_len,
    unsigned int &              group_limit_m,
    server_array_t &            servers,
    area_array_t &              areas
)
{
    if ( self ) {
        void * params[ 6 ];
        params[ 0 ] = self;
        params[ 1 ] = (void *)group_name;
        params[ 2 ] = & group_name_len;
        params[ 3 ] = & group_limit_m;
        params[ 4 ] = & servers;
        params[ 5 ] = & areas;
        return self->func( ANGILE_ADMIN_get_group_info, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

static inline
int
agile_c_admin_set_group_info(
    agile_c_t *                 self,
    const char *                group_name,
    int                         group_name_len,
    unsigned int                group_limit_m,
    const server_array_t &      servers,
    const area_array_t &        areas
)
{
    if ( self ) {
        void * params[ 6 ];
        params[ 0 ] = self;
        params[ 1 ] = (void *)group_name;
        params[ 2 ] = & group_name_len;
        params[ 3 ] = & group_limit_m;
        params[ 4 ] = (void *)& servers;
        params[ 5 ] = (void *)& areas;
        return self->func( ANGILE_ADMIN_set_group_info, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

static inline
int
agile_c_admin_del_group(
    agile_c_t *                 self,
    const char *                group_name,
    int                         group_name_len
)
{
    if ( self ) {
        void * params[ 3 ];
        params[ 0 ] = self;
        params[ 1 ] = (void *)group_name;
        params[ 2 ] = & group_name_len;
        return self->func( ANGILE_ADMIN_del_group, (void **)& params, sizeof(params)/sizeof(params[0]) );
    }

    return - EINVAL;
}

#ifdef __cplusplus
}
#endif

#endif // #ifndef _agile_c_agile_c_admin_h_
