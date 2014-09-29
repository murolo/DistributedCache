/*
 * (C) 2007-2010 Alibaba Group Holding Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 * Version: $Id: tair_cfg_svr.hpp 28 2010-09-19 05:18:09Z ruohai@taobao.com $
 *
 * Authors:
 *   Daoan <daoan@taobao.com>
 *
 */
#ifndef TAIR_CONFIG_SERVER_H
#define TAIR_CONFIG_SERVER_H
#include <string>
#include <ext/hash_map>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <tbsys.h>
#include <tbnet.h>
#include "server_conf_thread.hpp"
#include "packet_factory.hpp"
#include "agile_admin_packet.hpp"

//patch:zouyueming: 2014-05-14 15:01
#ifdef _ENABLE_AGILE
#include "../../../../../tair_patch/agile_packet_factory.h"
#endif // #ifdef _ENABLE_AGILE

using namespace std;
using namespace __gnu_cxx;

namespace tair {
  namespace config_server {
    class tair_config_server:public tbnet::IServerAdapter,
      public tbnet::IPacketQueueHandler {
    public:
      tair_config_server();
      ~tair_config_server();
      void start();
      void stop();
      // iserveradapter interface
        tbnet::IPacketHandler::HPRetCode handlePacket(tbnet::Connection *
                                                      connection,
                                                      tbnet::Packet * packet);
      // IPacketQueueHandler interface
      bool handlePacketQueue(tbnet::Packet * packet, void *args);

    private:
    bool handleSetGroupInfo(base_packet* packet, agile::admin_packet_t* rsp);
    bool handleGetGroupInfo(base_packet* packet, agile::admin_packet_t* rsp);
    bool handleGetGroupName(base_packet* packet, agile::admin_packet_t* rsp);
    bool handleDelGroup(agile::admin_packet_t* req, agile::admin_packet_t* rsp);
      int stop_flag;
      
//patch:zouyueming: 2014-05-14 15:01
#ifdef _ENABLE_AGILE
    agile::packet_factory_t packet_factory;
#else
      tair_packet_factory packet_factory;
#endif

        tbnet::DefaultPacketStreamer packet_streamer;
        tbnet::Transport packet_transport;
        tbnet::Transport heartbeat_transport;        //

        tbnet::PacketQueueThread task_queue_thread;
      server_conf_thread my_server_conf_thread;

    private:
        inline int initialize();
      inline int destroy();
    };
  }
}
#endif
///////////////////////END