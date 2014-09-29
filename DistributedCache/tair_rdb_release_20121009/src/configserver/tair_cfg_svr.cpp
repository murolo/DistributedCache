/*
 * (C) 2007-2010 Alibaba Group Holding Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * tair_cfg_svr.cpp is the main func for config server
 *
 * Version: $Id: tair_cfg_svr.cpp 28 2010-09-19 05:18:09Z ruohai@taobao.com $
 *
 * Authors:
 *   Daoan <daoan@taobao.com>
 *
 */
#include "tair_cfg_svr.hpp"
#include "util.hpp"
#include "agile_admin_packet.hpp"
#include "tair_config.hpp"
#include "boost/algorithm/string.hpp"
#include <tbsys.h>

namespace tair {
  namespace config_server {
    bool semicolon(const char& ch) {
        return ';' == ch;
    }

    tair_config_server::tair_config_server()
    {
      stop_flag = 0;
    }

    tair_config_server::~tair_config_server()
    {
    }

    void tair_config_server::start()
    {
      if(initialize()) {
        return;
      }

      //process thread
      task_queue_thread.start();

      // ransport
      char spec[32];
      bool ret = true;
      if(ret) {
        int port =
          TBSYS_CONFIG.getInt(CONFSERVER_SECTION, TAIR_PORT,
                              TAIR_CONFIG_SERVER_DEFAULT_PORT);
        port++;                        // port of heart beat will be plus one
        sprintf(spec, "tcp::%d", port);
        if(heartbeat_transport.listen(spec, &packet_streamer, this) == NULL) {
          log_error("listen port %d error", port);
          ret = false;
        }
        else {
          log_info("open tcp port %d", port);
        }
      }
      // tcp
      if(ret) {
        int port =
          TBSYS_CONFIG.getInt(CONFSERVER_SECTION, TAIR_PORT,
                              TAIR_CONFIG_SERVER_DEFAULT_PORT);
        sprintf(spec, "tcp::%d", port);
        if(packet_transport.listen(spec, &packet_streamer, this) == NULL) {
          log_error("listen port %d error", port);
          ret = false;
        }
        else {
          log_info("listen tcp port: %d", port);
        }
      }

      if(ret) {
        log_info("---- program stated PID: %d ----", getpid());
        packet_transport.start();
        heartbeat_transport.start();
        my_server_conf_thread.start();
      }
      else {
        stop();
      }

      task_queue_thread.wait();
      my_server_conf_thread.wait();
      packet_transport.wait();
      heartbeat_transport.wait();

      destroy();
    }

    void tair_config_server::stop()
    {
      if(stop_flag == 0) {
        stop_flag = 1;
        my_server_conf_thread.stop();
        packet_transport.stop();
        heartbeat_transport.stop();
        task_queue_thread.stop();
      }
    }

    int tair_config_server::initialize()
    {
      const char *dev_name =
        TBSYS_CONFIG.getString(CONFSERVER_SECTION, TAIR_DEV_NAME, NULL);
      uint32_t local_ip = tbsys::CNetUtil::getLocalAddr(dev_name);
      int port =
        TBSYS_CONFIG.getInt(CONFSERVER_SECTION, TAIR_PORT,
                            TAIR_CONFIG_SERVER_DEFAULT_PORT);
      util::local_server_ip::ip = tbsys::CNetUtil::ipToAddr(local_ip, port);

      vector<const char *>str_list =
        TBSYS_CONFIG.getStringList(TAIRPUBLIC_SECTION, TAIR_CONFIG_SERVER);
      uint64_t id;
      int server_index = 0;
      for(uint32_t i = 0; i < str_list.size(); i++) {
        id = tbsys::CNetUtil::strToAddr(str_list[i], port);
        if(id == 0)
          continue;
        if(id == util::local_server_ip::ip) {
          server_index = 0;
          break;
        }
        server_index++;
        if(server_index >= 2)
          break;
      }
      if(server_index != 0) {
        log_error
          ("my Ip %s is not in the list of config_server check it out.",
           tbsys::CNetUtil::addrToString(util::local_server_ip::ip).c_str());
        return EXIT_FAILURE;
      }
      else {
        log_info("my IP is: %s",
                 tbsys::CNetUtil::addrToString(util::local_server_ip::ip).
                 c_str());
      }

      // packet_streamer
      packet_streamer.setPacketFactory(&packet_factory);
      //m_sdbmStreamer.setPacketFactory(&m_sdbmFactory);

      int thread_count =
        TBSYS_CONFIG.getInt(CONFSERVER_SECTION, TAIR_PROCESS_THREAD_COUNT, 4);
      task_queue_thread.setThreadParameter(thread_count, this, NULL);

      // my_server_conf_thread
      my_server_conf_thread.set_thread_parameter(&packet_transport,
                                                 &heartbeat_transport,
                                                 &packet_streamer);

      return EXIT_SUCCESS;
    }

    int tair_config_server::destroy()
    {
      return EXIT_SUCCESS;
    }

    tbnet::IPacketHandler::HPRetCode tair_config_server::handlePacket(tbnet::
                                                                      Connection
                                                                      *
                                                                      connection,
                                                                      tbnet::
                                                                      Packet *
                                                                      packet)
    {
      if(!packet->isRegularPacket()) {
        log_error("ControlPacket, cmd:%d",
                  ((tbnet::ControlPacket *) packet)->getCommand());
        return tbnet::IPacketHandler::FREE_CHANNEL;
      }

      base_packet *bp = (base_packet *) packet;
      bp->set_connection(connection);
      bp->set_direction(DIRECTION_RECEIVE);
      task_queue_thread.push(bp);

      return tbnet::IPacketHandler::KEEP_CHANNEL;
    }

    bool tair_config_server::handleGetGroupName(base_packet* packet, agile::admin_packet_t* rsp) {
        rsp->set("type", TAIR_AGILE_REQ_GET_GROUP_NAMES );
        const char* group_file_name = 
            TBSYS_CONFIG.getString(CONFSERVER_SECTION, TAIR_GROUP_FILE, NULL);

        log_error("%s", group_file_name);
        Configer config;
        if (EXIT_FAILURE == config.load(group_file_name)) {
            log_error("config load file failed");
            return false;
        }

        log_error("get group names: \n %s", config.toString().c_str());

        string group_str;
        vector<string> section_list;
        config.getSectionName(section_list);
        vector<string>::iterator it = section_list.begin();
        for(int i = 0; it != section_list.end(); ++it, ++i){
            if (i > 0) {
                group_str += ";";
            }
            group_str += *it;
        }

        rsp->set(AGILE_KEY_GROUP_NAMES, group_str);
        rsp->output_data();
        return true;
    }


    bool tair_config_server::handleGetGroupInfo(base_packet* packet, agile::admin_packet_t* rsp) {
        rsp->set("type", TAIR_AGILE_REQ_GET_GROUP_INFO);
        const char* group_file_name = 
            TBSYS_CONFIG.getString(CONFSERVER_SECTION, TAIR_GROUP_FILE, NULL);
        Configer config;
        if (EXIT_FAILURE == config.load(group_file_name)) {
            log_error("===> config load file failed");
            return false;
        }
        
        log_error("====> config file: \n %s", config.toString().c_str());
        agile::admin_packet_t* req = (agile::admin_packet_t*)packet;
        agile::admin_packet_t::map_t::iterator it = req->find("group_name");
        if (req->end() == it) {
            return false;
        }
        string group_name = it->second;

        string key_server_list = "_server_list";
        vector<const char*> server_list = 
            config.getStringList(group_name.data(), key_server_list);
        string server_str;
        vector<const char*>::iterator sit = server_list.begin();
        for(int i = 0; sit != server_list.end(); ++sit, ++i) {
            if (i > 0) {
                server_str += ";";
            }
            log_error("_server_list => %s", *sit);
            server_str += *sit;
        }

        log_error("server_str => %s", server_str.c_str());
        rsp->set("servers", server_str);

        string key_area_capacity = "_areaCapacity_list";
        vector<const char*> area_cap_list = 
            config.getStringList(group_name.data(), key_area_capacity);
       string area_str;
       vector<const char*>::const_iterator ait = area_cap_list.begin();
       for(int i = 0; ait != area_cap_list.end(); ++ait, ++i) {
           if (i > 0) {
                area_str += ";";
           }
           area_str += *ait;
       }
       log_error("area_str => %s", area_str.c_str());
       rsp->set("areas", area_str);

       string key_copy_count = "_copy_count";
       const char* str_copy_count = config.getString(group_name.data(), key_copy_count);
       if (NULL == str_copy_count) {
           return false;
       }
       string scc(str_copy_count);
       rsp->set("copy_count", scc);

       rsp->output_data();
       return true;
    }

    bool tair_config_server::handleSetGroupInfo(base_packet* packet, agile::admin_packet_t* rsp) {
        if (!my_server_conf_thread.is_master()) {
            return false;
        }
        rsp->set("type", TAIR_AGILE_REQ_SET_GROUP_INFO);
        agile::admin_packet_t* req = (agile::admin_packet_t*)packet;
        agile::admin_packet_t::map_t::iterator it = req->find("group_name");
        if (it == req->end()) {
            return false;
        }
        req->output_data();
        string group_name = it->second;

        Configer config;
        const char* file_name = 
          TBSYS_CONFIG.getString(CONFSERVER_SECTION, TAIR_GROUP_FILE, NULL);
        if (EXIT_FAILURE == config.load(file_name)) {
            return false;
        }

        config.removeSection(group_name.c_str());

        it = req->find("copy_count");
        if (it == req->end()) {
            return false;
        }
        int copy_count = atoi(it->second.c_str());
        if (0 > copy_count || copy_count > 3) {
            return false;
        }
        
        config.removeSectionKey(group_name.c_str(), "_copy_count");
        config.setString(group_name.c_str(), "_copy_count", it->second.c_str());

         it = req->find("servers");
         vector<string> servers_str_vec;
         if (it != req->end()) {
             boost::split(servers_str_vec, it->second, boost::is_any_of(";"), boost::token_compress_on);
         }

         vector<string> areas_str_vec;
         it = req->find("areas");
         if (it != req->end()) {
             boost::split(areas_str_vec, it->second, boost::is_any_of(";"), boost::token_compress_on);
         }

         vector<string>::iterator sit = servers_str_vec.begin();
         for(; sit != servers_str_vec.end(); ++sit) {
             if (!sit->empty()) {
                 config.setString(group_name.c_str(), "_server_list", sit->c_str());
             }
         }

         it = req->find("areas");
         if (it != req->end()) {
            vector<string> area_str_vec;
            boost::split(area_str_vec, it->second, boost::is_any_of(";"), boost::token_compress_on);

             vector<string>::iterator ait = area_str_vec.begin();
             for(int i = 0; ait != area_str_vec.end(); ++ait, ++i) {
                 if (!ait->empty()) {
                     char buf[128] = {0};
                     sprintf(buf, "%d,%s", i, ait->c_str());
                   config.setString(group_name.c_str(), "_areaCapacity_list", buf);
                 }
             }
         }
         string data = config.toString();
         log_error("set config data => %s", data.c_str());
         bool ret = config.backup_and_write_file(file_name, data.c_str(), data.size(), time(NULL));
         return ret;
    }

    bool tair_config_server::handleDelGroup(agile::admin_packet_t* req, agile::admin_packet_t* rsp) {
        if(!my_server_conf_thread.is_master()){
            return false;
        }
        log_error("process del group");
        rsp->set("type", "del_group");
        agile::admin_packet_t::map_t::iterator it = req->find("group_name");
        if (req->end() == it) {
            return false;
        }
        req->output_data();
        string group_name = it->second;
        Configer config;
        const char* file_name = 
          TBSYS_CONFIG.getString(CONFSERVER_SECTION, TAIR_GROUP_FILE, NULL);
        if (EXIT_FAILURE == config.load(file_name)) {
            log_error("del group load file failed");
            return false;
        }   
        
        config.removeSection(group_name.c_str());
        string data = config.toString();
        log_error("after del group: %s", data.c_str());
        bool ret = config.backup_and_write_file(file_name, data.c_str(), data.size(), time(NULL));
        return ret;
    }

    bool tair_config_server::handlePacketQueue(tbnet::Packet * apacket,
                                               void *args)
    {
      base_packet *packet = (base_packet *) apacket;
      int pcode = apacket->getPCode();


      bool send_ret = true;
      int ret = EXIT_SUCCESS;
      const char *msg = "";
      switch (pcode) {
          case 3000: { 
              log_error("process agile admin");
              agile::admin_packet_t* req = (agile::admin_packet_t*) packet;
              agile::admin_packet_t::map_t::iterator it = req->find("type");
              if (it != req->end()) {
                  agile::admin_packet_t* rsp = 
                      new agile::admin_packet_t(3001);
        
                  int ret = true;
                  log_error("type => %s", it->second.c_str());
                  if ("get_group_names" == it->second) {
                      ret = handleGetGroupName(packet, rsp);
                  }
                  else if ("get_group_info" == it->second)
                  {
                      log_error("====> process get group info");
                      ret = handleGetGroupInfo(packet, rsp);
                  }
                  else if ("set_group_info" == it->second)
                  {
                      ret = handleSetGroupInfo(packet, rsp);
                      if (!ret) {
                          rsp->setPCode(3000);
                      }
                  }
                  else if ("del_group" == it->second) {
                      ret = handleDelGroup(req, rsp);
                      if (!ret) {
                          rsp->setPCode(3000);
                      }
                  }
                  else {
                      log_error("====> error: type = %s", it->second.c_str());
                      ret = false;
                  }

                  if (ret) {
                      rsp->set("error", "0");
                  }
                  else {
                      log_error("handle error");
                      rsp->set("error", "-1");
                  }

                  log_error("chid %d", packet->getChannelId());
                  rsp->setChannelId(packet->getChannelId());
                  log_error("rspchid %d", rsp->getChannelId());
                  if (!packet->get_connection()->postPacket(rsp)) {
                      delete rsp;
                  }
              }
              send_ret = false;
          }
          break;
     
      case TAIR_REQ_GET_GROUP_PACKET:{
          request_get_group *req = (request_get_group *) packet;
          response_get_group *resp = new response_get_group();

          my_server_conf_thread.find_group_host(req, resp);

          resp->setChannelId(packet->getChannelId());
          if(packet->get_connection()->postPacket(resp) == false) {
            delete resp;
          }
          send_ret = false;
        }
        break;
      case TAIR_REQ_HEARTBEAT_PACKET:{
          request_heartbeat *req = (request_heartbeat *) packet;
          response_heartbeat *resp = new response_heartbeat();

          my_server_conf_thread.do_heartbeat_packet(req, resp);
          resp->setChannelId(packet->getChannelId());
          if(packet->get_connection()->postPacket(resp) == false) {
            log_error("send req heartbeat packet error");
            delete resp;
          }
          send_ret = false;
        }
        break;
      case TAIR_REQ_QUERY_INFO_PACKET:{
          log_debug("request_query_info");
          request_query_info *req = (request_query_info *) packet;
          response_query_info *resp = new response_query_info();

          my_server_conf_thread.do_query_info_packet(req, resp);
          resp->setChannelId(packet->getChannelId());
          if(packet->get_connection()->postPacket(resp) == false) {
            log_error("send req query info packet error");
            delete resp;
          }
          send_ret = false;
        }
        break;
      case TAIR_REQ_CONFHB_PACKET:{
          my_server_conf_thread.
            do_conf_heartbeat_packet((request_conf_heartbeart *) packet);
        }
        break;
      case TAIR_REQ_SETMASTER_PACKET:{
          if(my_server_conf_thread.
             do_set_master_packet((request_set_master *) packet) == false) {
            ret = EXIT_FAILURE;
          }
        }
        break;
      case TAIR_REQ_GET_SVRTAB_PACKET:{
          request_get_server_table *req = (request_get_server_table *) packet;
          response_get_server_table *resp = new response_get_server_table();

          my_server_conf_thread.do_get_server_table_packet(req, resp);
          resp->setChannelId(packet->getChannelId());
          if(packet->get_connection()->postPacket(resp) == false) {
            delete resp;
          }
          send_ret = false;
        }
        break;
      case TAIR_RESP_GET_SVRTAB_PACKET:{
          if(my_server_conf_thread.
             do_set_server_table_packet((response_get_server_table *) packet)
             == false) {
            ret = EXIT_FAILURE;
          }
        }
        break;
      case TAIR_REQ_MIG_FINISH_PACKET:{
          ret =
            my_server_conf_thread.
            do_finish_migrate_packet((request_migrate_finish *) packet);
          if(ret == 1) {
            send_ret = false;
          }
          if(ret == -1) {
            ret = EXIT_FAILURE;
          }
          else {
            ret = EXIT_SUCCESS;
          }
        }
        break;

      case TAIR_REQ_GROUP_NAMES_PACKET:{
          response_group_names *resp = new response_group_names();
          my_server_conf_thread.do_group_names_packet(resp);
          resp->setChannelId(packet->getChannelId());
          if(packet->get_connection()->postPacket(resp) == false) {
            delete resp;
          }
          send_ret = false;
        }
        break;
      case TAIR_REQ_DATASERVER_CTRL_PACKET:
        // force down or foece up some dataserver;
        my_server_conf_thread.
          force_change_server_status((request_data_server_ctrl *) packet);
        break;
      case TAIR_REQ_GET_MIGRATE_MACHINE_PACKET:
        {
          response_get_migrate_machine *resp =
            new response_get_migrate_machine();
          request_get_migrate_machine *req =
            (request_get_migrate_machine *) packet;

          my_server_conf_thread.get_migrating_machines(req, resp);

          resp->setChannelId(packet->getChannelId());
          if(packet->get_connection()->postPacket(resp) == false) {
            delete resp;
          }
          send_ret = false;
        }
        break;
      case TAIR_REQ_OP_CMD_PACKET:
        {
          request_op_cmd *req = (request_op_cmd*) packet;
          my_server_conf_thread.do_op_cmd(req);
          send_ret = false;
        }
        break;
      default:
        log_error("unknow packet pcode: %d", pcode);
      }
      if(send_ret && packet->get_direction() == DIRECTION_RECEIVE) {
        tair_packet_factory::set_return_packet(packet, ret, msg, 0);
      }
      return true;
    }
  }
}                                // namespace end

////////////////////////////////////////////////////////////////////////////////////////////////
// Main
////////////////////////////////////////////////////////////////////////////////////////////////
tair::config_server::tair_config_server * tair_cfg_svr = NULL;
tbsys::CThreadMutex gmutex;

void
sign_handler(int sig)
{
  switch (sig) {
  case SIGTERM:
  case SIGINT:
    gmutex.lock();
    if(tair_cfg_svr != NULL) {
      tair_cfg_svr->stop();
    }
    gmutex.unlock();
    break;
  case 40:
    TBSYS_LOGGER.checkFile();
    break;
  case 41:
  case 42:
    if(sig == 41) {
      TBSYS_LOGGER._level++;
    }
    else {
      TBSYS_LOGGER._level--;
    }
    log_error("TBSYS_LOGGER._level: %d", TBSYS_LOGGER._level);
    break;
  }
}

void
print_usage(char *prog_name)
{
  fprintf(stderr, "%s -f config_file\n"
          "    -f, --config_file  config file\n"
          "    -h, --help         this help\n"
          "    -V, --version      version\n\n", prog_name);
}

char *
parse_cmd_line(int argc, char *const argv[])
{
  int
    opt;
  const char *
    opt_string = "hVf:";
  struct option
    longopts[] = {
    {"config_file", 1, NULL, 'f'},
    {"help", 0, NULL, 'h'},
    {"version", 0, NULL, 'V'},
    {0, 0, 0, 0}
  };

  char *
    config_file = NULL;
  while((opt = getopt_long(argc, argv, opt_string, longopts, NULL)) != -1) {
    switch (opt) {
    case 'f':
      config_file = optarg;
      break;
    case 'V':
      fprintf(stderr, "BUILD_TIME: %s %s\n\n", __DATE__, __TIME__);
      exit(1);
    case 'h':
      print_usage(argv[0]);
      exit(1);
    }
  }
  return config_file;
}

ssize_t get_exe_path( char * path, size_t  path_len)
{   
    ssize_t r;
    assert( path && path_len );
    r = readlink( "/proc/self/exe", path, path_len );
    if ( r <= 0 ) {   
        log_error( "readlink failed %d,%s\n", errno, strerror(errno) );
        path[ 0 ] = 0;  
        return 0;
    }    
    if ( (unsigned long)r >= path_len ) {   
        log_error( "invalid params\n" );
        path[ 0 ] = 0;  
        return 0;
    }    
    path[ r ] = '\0';
    return r;
}   

int write_maintain_pid_file()
{   
    int pid = getpid();
    int ppid = getppid();

    char exe_path[ 256 ] = "";  
    get_exe_path( exe_path, sizeof( exe_path ) );  
    if ( '\0' == exe_path ) {   
        log_error( "[maintain][pid=%d][ppid=%d] get_exe_path failed: %d,%s",
                pid, ppid, errno, strerror(errno) );
        return -1;; 
    }    

    const char * p = strrchr( exe_path, '/' );
    if ( NULL == p ) {   
        log_error( "[maintain][pid=%d][ppid=%d] %s not found in %s",
                pid, ppid, "/", exe_path );
        return -1; 
    }    
    char exe_dir[ 256 ];
    size_t len = p - (const char *)exe_path;
    memcpy( exe_dir, exe_path, len );
    exe_dir[ len ] = '\0';

    std::string cmdline = "LD_LIBRARY_PATH=/usr/lib:/usr/lib64:.:";
    cmdline += exe_dir;
    cmdline += "; cd ";
    cmdline += exe_dir;
    cmdline += "/../; sh tair.sh start_ds &";
    log_debug("cmdline:%s", cmdline.c_str());

    if (!tbsys::CFileUtil::mkdirs("/home/s/var/proc/")) {
        log_error("create /home/s/var/proc failed:%d,%s", errno, strerror(errno));
        return -1;
    }
    char path[ 256 ];
    sprintf( path, "/home/s/var/proc/agile_configserver.%d.pid", pid );

    FILE * fp = fopen( path, "wb" );
    if ( NULL == fp ) { 
        log_error( "[maintain][pid=%d][ppid=%d][exe=%s][path=%s] write %s to %s failed: %d,%s\n",
                pid, ppid, exe_path, path, cmdline.c_str(), path, errno, strerror(errno) );
        return -1; 
    }   

    if ( cmdline.size() != fwrite( cmdline.c_str(), 1, cmdline.size(), fp ) ) { 
        log_error( "[maintain][pid=%d][ppid=%d][exe=%s][path=%s] write %s to %s failed: %d,%s\n",
                pid, ppid, exe_path, path, cmdline.c_str(), path, errno, strerror(errno) );
        fclose( fp );
        return -1; 
    }   

    fclose( fp );

    log_error( "[maintain][pid=%d][ppid=%d]write %s to %s file OK\n",
            pid, ppid, cmdline.c_str(), path );

    return 0;
}   

int
main(int argc, char *argv[])
{
  // parse cmd
  char *
    config_file = parse_cmd_line(argc, argv);
  if(config_file == NULL) {
    print_usage(argv[0]);
    return EXIT_FAILURE;
  }

  if(TBSYS_CONFIG.load(config_file)) {
    fprintf(stderr, "load file %s error\n", config_file);
    return EXIT_FAILURE;
  }

  const char *
    sz_pid_file =
    TBSYS_CONFIG.getString(CONFSERVER_SECTION, TAIR_PID_FILE, "cfgsvr.pid");
  const char *
    sz_log_file =
    TBSYS_CONFIG.getString(CONFSERVER_SECTION, TAIR_LOG_FILE, "cfgsvr.log");
  if(1) {
    char *
      p,
      dir_path[256];
    sprintf(dir_path, "%s",
            TBSYS_CONFIG.getString(CONFSERVER_SECTION, TAIR_DATA_DIR,
                                   TAIR_DEFAULT_DATA_DIR));
    if(!tbsys::CFileUtil::mkdirs(dir_path)) {
      fprintf(stderr, "create dir %s error\n", dir_path);
      return EXIT_FAILURE;
    }
    sprintf(dir_path, "%s", sz_pid_file);
    p = strrchr(dir_path, '/');
    if(p != NULL)
      *p = '\0';
    if(p != NULL && !tbsys::CFileUtil::mkdirs(dir_path)) {
      fprintf(stderr, "create dir %s error\n", dir_path);
      return EXIT_FAILURE;
    }
    sprintf(dir_path, "%s", sz_log_file);
    p = strrchr(dir_path, '/');
    if(p != NULL)
      *p = '\0';
    if(p != NULL && !tbsys::CFileUtil::mkdirs(dir_path)) {
      fprintf(stderr, "create dir %s error\n", dir_path);
      return EXIT_FAILURE;
    }
  }

  int
    pid;
  if((pid = tbsys::CProcess::existPid(sz_pid_file))) {
    fprintf(stderr, "program has been exist: pid=%d\n", pid);
    return EXIT_FAILURE;
  }

  const char *
    sz_log_level =
    TBSYS_CONFIG.getString(CONFSERVER_SECTION, TAIR_LOG_LEVEL, "info");
  TBSYS_LOGGER.setLogLevel(sz_log_level);

  if(tbsys::CProcess::startDaemon(sz_pid_file, sz_log_file) == 0) {
      if (write_maintain_pid_file()) {
          return EXIT_FAILURE;
      }
    signal(SIGPIPE, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGINT, sign_handler);
    signal(SIGTERM, sign_handler);
    signal(40, sign_handler);
    signal(41, sign_handler);
    signal(42, sign_handler);

    tair_cfg_svr = new tair::config_server::tair_config_server();
    tair_cfg_svr->start();
    gmutex.lock();
    delete
      tair_cfg_svr;
    tair_cfg_svr = NULL;
    gmutex.unlock();

    log_info("exit program.");
  }

  return EXIT_SUCCESS;
}

////////////////////////////////END
