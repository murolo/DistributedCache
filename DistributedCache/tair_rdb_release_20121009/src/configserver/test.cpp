#include "agile_admin_packet.hpp"
#include <iostream>

using namespace tair;
using namespace config_server;

int main(int argc, const char *argv[])
{
    agile::admin_packet_t* packet = new agile::admin_packet_t(3000);
    packet->set("error", "0");
    packet->set("group_name","group_1;group_2;group_3");
    
    int error;
    vector<string> group_name_vec;
    packet->rsp_get_group_names(error, group_name_vec);
    cout << "error => " << error << endl;

    vector<string>::iterator it = group_name_vec.begin();
    for(; it != group_name_vec.end(); ++it) {
        cout << *it << " " ;
    }
    cout << endl;

    return 0;
}
