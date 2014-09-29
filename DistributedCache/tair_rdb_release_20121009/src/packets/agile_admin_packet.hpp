#ifndef _agile_admin_packet_h_
#define _agile_admin_packet_h_

#include "base_packet.hpp"
//#include "tair_packet_factory.h"
#include "tbnet.h"
#include "databuffer.h"
#include "agile_c_admin.hpp"
#include <map>
#include <string>

namespace agile
{

#ifndef _ENABLE_AGILE
#define TAIR_REQ_AGILE_ADMIN_PACKET      3000 
#define TAIR_RESP_AGILE_ADMIN_PACKET     3001
#endif

#define TAIR_AGILE_REQUEST_TYPE  "type"
#define TAIR_AGILE_REQ_GET_GROUP_NAMES  "get_group_names"
#define TAIR_AGILE_REQ_GET_GROUP_INFO  "get_group_info"
#define TAIR_AGILE_REQ_SET_GROUP_INFO  "set_group_info"

#define AGILE_KEY_GROUP_NAMES    "group_names"
#define AGILE_KEY_ERROR         "error"
#define AGILE_KEY_SERVERS       "servers"
#define AGILE_KEY_AREAS         "areas"

#define KEY_TYPE                "type"
#define TYPE_GET_SERVER_COUNT   "get_server_count"

class admin_packet_t : public tair::base_packet
{
public:
    enum { MAX_COUNT = 50 };
    typedef std::map< std::string, std::string >    map_t;
    typedef map_t::iterator                         iterator;

    admin_packet_t( int pcode );
    bool is_req() const;
    bool encode( tbnet::DataBuffer * output );
    bool decode( tbnet::DataBuffer * input, tbnet::PacketHeader * header);

    //iterator begin();
    map_t::iterator end();
    map_t::iterator find( const std::string & key );
    //void erase( iterator i );
    //void clear();
    void set( const std::string & key, const std::string & val );

    bool build_req_get_group_names();
    bool build_req_get_group_info( const char * group_name, int group_name_len);
    bool build_req_set_group_info(
        const char *    group_name,
        int             group_name_len,
        unsigned int    group_limit_m,
        const server_array_t * servers,
        const area_array_t *   areas,
        int& copy_count
    );
    bool build_req_del_group( const char * group_name, int group_name_len );

    bool rsp_get_group_names( int & error, std::vector< std::string > & group_names );
    bool rsp_get_group_info( int & error,
                             unsigned int & group_limit_m,
                             server_array_t & servers,
                             area_array_t &   areas,
                             int& copy_count
    );
    bool rsp_set_group_info( int & error );
    bool rsp_del_group( int & error );

    void output_data();
    std::string agile_server_to_str();

private:

    bool write_bytes( tbnet::DataBuffer *  output,
                        const char *  s,
                        int           sl );
    bool read_bytes( tbnet::DataBuffer *   input,
                        std::string &  buf );

private:
    map_t       m_data;
private:
    admin_packet_t();
    admin_packet_t(const admin_packet_t &);
    const admin_packet_t & operator = (const admin_packet_t &);
};

} // namespace agile

#endif // #ifndef _agile_admin_packet_h_
