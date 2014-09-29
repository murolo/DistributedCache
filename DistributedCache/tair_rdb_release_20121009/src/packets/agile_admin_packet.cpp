#include "agile_admin_packet.hpp"
#include "boost/algorithm/string.hpp"


namespace agile
{

    bool semicolon(const char& ch) {
        return ';' == ch;
    }

admin_packet_t::admin_packet_t( int pcode )
    : m_data()
{
    setPCode( pcode );
}

bool admin_packet_t::is_req() const
{
    switch( getPCode() ) {
    case TAIR_REQ_AGILE_ADMIN_PACKET:
        return true;
    default:
        return false;
    }
}

bool admin_packet_t::encode( tbnet::DataBuffer * output )
{
    if ( m_data.empty() || m_data.size()>MAX_COUNT ) {
        return false;
    }
    output->writeInt32( (int)m_data.size() );
    map_t::iterator i;
    for ( i= m_data.begin(); i != m_data.end(); ++i ) {
        const std::string & key = (*i).first;
        const std::string & val = (*i).second;
        if ( key.empty() ) {
            return false;
        }
        if ( ! write_bytes( output, key.c_str(), key.size() ) ) {
            return false;
        }
        if ( ! write_bytes( output, val.c_str(), val.size() ) ) {
            return false;
        }
    }
    return true;
}

bool admin_packet_t::decode( tbnet::DataBuffer * input, tbnet::PacketHeader * header)
{
    m_data.clear();
    unsigned int count = input->readInt32();
    if ( 0 == count || count > MAX_COUNT ) {
        return false;
    }
    std::string key, val;
    for ( unsigned int i = 0; i < count; ++ i ) {
        if ( ! read_bytes( input, key ) ) {
            return false;
        }
        if ( key.empty() ) {
            return false;
        }
        if ( ! read_bytes( input, val ) ) {
            return false;
        }
        m_data[ key ] = val;
    }
    return true;
}

bool admin_packet_t::write_bytes(
    tbnet::DataBuffer *  output,
    const char *  s,
    int           sl )
{
    if ( sl < 0 ) {
        return false;
    }
    output->writeInt32( sl );
    output->writeBytes( s, sl );
    return true;
}

bool admin_packet_t::read_bytes(
    tbnet::DataBuffer *   input,
    std::string &  buf )
{
    uint32_t len = input->readInt32();
    buf.resize( 0 );
    try {
        buf.resize( len );
    } catch ( ... ) {
        return false;
    }
    if ( len > 0 ) {
        if (!input->readBytes( & buf[ 0 ], (int)len)) {
            buf.resize( 0 );
            return false;
        }
    }
    return true;
}

bool admin_packet_t::build_req_get_group_names()
{
    m_data[TAIR_AGILE_REQUEST_TYPE] = TAIR_AGILE_REQ_GET_GROUP_NAMES;
    return true;
}

bool admin_packet_t::build_req_get_group_info( const char * group_name, int group_name_len )
{
    m_data[TAIR_AGILE_REQUEST_TYPE] = TAIR_AGILE_REQ_GET_GROUP_INFO;
    m_data["group_name"] = string(group_name, group_name_len);
    return true;
}


bool admin_packet_t::build_req_set_group_info(
    const char *    group_name,
    int             group_name_len,
    unsigned int    group_limit_m,
    const server_array_t * servers,
    const area_array_t *   areas,
    int&            copy_count
)
{
    m_data.clear();
    if (0 < copy_count || copy_count > 3) {
        return false;
    }
    char buf[128] = {0};
    sprintf(buf, "%d", copy_count);
    m_data["copy_count"] =  buf;
    m_data[TAIR_AGILE_REQUEST_TYPE] = TAIR_AGILE_REQ_SET_GROUP_INFO;

    string groupname = string(group_name, group_name_len);
    m_data["group_name"] = groupname;

    char buffer[128] = {0};
    sprintf(buffer, "%d", group_limit_m);
    m_data["group_limit_m"] = buffer;

    string server_str;
    server_array_t::const_iterator it = servers->begin();
    for(int i = 0; it != servers->end(); ++it, ++i) {
        if (i > 0) {
            server_str += ";";
        }
        char buff[128] = {0};
        sprintf(buff, "%s:%d", it->host, it->port);
        server_str += buff;
    }
    m_data["servers"] =  server_str;

    string area_str;
    area_array_t::const_iterator ait = areas->begin();
    for(int i = 0; ait != areas->end(); ++ait, ++i) {
        if (i > 0) {
            area_str += ";";
        }
        char buffer[128] = {0};
        sprintf(buffer, "%d,%d", i, ait->limit_m);
        area_str += buffer;
    }
    string trim_area_str = boost::algorithm::trim_right_copy_if(area_str, semicolon);
    m_data["areas"] = area_str;
    return true;
}

bool admin_packet_t::build_req_del_group( const char * group_name, int group_name_len )
{
    m_data.clear();
    m_data["type"] = "del_group";
    m_data["group_name"] = string(group_name, group_name_len);
    return false;
}

bool admin_packet_t::rsp_get_group_names( int & error, std::vector< std::string > & group_names )
{
    map_t::iterator it = m_data.find(TAIR_AGILE_REQUEST_TYPE);
    if (it == m_data.end()) {
        return false;
    }

    if (it->second != TAIR_AGILE_REQ_GET_GROUP_NAMES) {
        return false;
    }

    it = m_data.find(AGILE_KEY_GROUP_NAMES);
    string group_str;
    if (it == m_data.end()) {
        return false;
    }
    group_str = it->second;
    boost::split(group_names, group_str, boost::is_any_of(";"), boost::token_compress_on);

    it = m_data.find("error");
    
    if (it == m_data.end()) {
        return false;
    } 

    error = atoi(it->second.c_str());
    return true;
}

bool admin_packet_t::rsp_get_group_info(
    int & error,
    unsigned int & group_limit_m,
    server_array_t & servers,
    area_array_t &   areas,
    int&            copy_count
)
{
    map_t::iterator it = m_data.find(TAIR_AGILE_REQUEST_TYPE);
    if (it == m_data.end()) {
        return false;
    }

    if (it->second != TAIR_AGILE_REQ_GET_GROUP_INFO) {
        return false;
    }

    it = m_data.find("error");
    if (it == m_data.end()) {
        return false;
    }

    error = atoi(it->second.c_str());
    if (error != 0) {
        return false;
    }

    it = m_data.find("copy_count");
    if (it == m_data.end()) {
        return false;
    }

    copy_count = atoi(it->second.c_str());
    if (copy_count < 0 || copy_count > 3) {
        return false;
    }

    it = m_data.find("servers");
    if (it != m_data.end()) {
        vector<string> server_id_vec;
        boost::split(server_id_vec, it->second, boost::is_any_of(";"),  boost::token_compress_on);
        vector<string>::iterator sit = server_id_vec.begin();
        for(; sit != server_id_vec.end(); ++sit) {
            if (!sit->empty()) {
                agile_server_t ast;
                sscanf(sit->c_str(), "%[^:]:%d", ast.host, &ast.port);
                servers.push_back(ast);
            }
        }
    }

    it = m_data.find("areas");
    if (it != m_data.end()) {
        vector<string> area_str_vec;
        boost::split(area_str_vec, it->second, boost::is_any_of(";"),  boost::token_compress_on);
        vector<string>::iterator vit = area_str_vec.begin();
        group_limit_m = 0;
        for(; vit != area_str_vec.end(); ++vit) {
            if (!vit->empty()) {
                int area, capacity;
                sscanf(vit->c_str(), "%d,%d", &area, &capacity);
                agile_area_info_t info;
                info.limit_m = capacity;
                areas.push_back(info);
                group_limit_m += capacity;
            }
        }
    }

    return true;
}

bool admin_packet_t::rsp_set_group_info( int & error )
{
    map_t::iterator it = m_data.find("error");
    if (it == m_data.end()) {
        error = -1;
        return false;
    }
    error = atoi(it->second.c_str());
    return true;
}

bool admin_packet_t::rsp_del_group( int & error )
{
    map_t::iterator it = m_data.find("error");
    if (it == m_data.end()) {
        error = -1;
        return false;
    }
    error = atoi(it->second.c_str());
    return true;
}

void admin_packet_t::output_data() {
    map_t::iterator it = m_data.begin();
    for(; it != m_data.end(); ++it){
        log_error("%s=>%s\n", it->first.c_str(), it->second.c_str());
    }
}

void admin_packet_t::set(const string& key, const string& value) {
    m_data[key] = value;
}

agile::admin_packet_t::map_t::iterator admin_packet_t::find( const std::string & key ){
    map_t::iterator it = m_data.find(key);
    return it;
}

agile::admin_packet_t::map_t::iterator admin_packet_t::end() {
    return m_data.end();
}

} // namespace agile
