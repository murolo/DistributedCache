/*
 * (C) 2007-2010 Alibaba Group Holding Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * remove packet
 *
 * Version: $Id: remove_packet.hpp 389 2011-12-01 09:31:22Z choutian.xmm@taobao.com $
 *
 * Authors:
 *   ruohai <ruohai@taobao.com>
 *     - initial release
 *
 */
#ifndef TAIR_PACKET_REMOVE_PACKET_H
#define TAIR_PACKET_REMOVE_PACKET_H
#include "base_packet.hpp"
#include "get_packet.hpp"
namespace tair
{
  class request_remove:public request_get
  {
  public:

    request_remove ():request_get ()
    {
      setPCode (TAIR_REQ_REMOVE_PACKET);
    }

    request_remove (uint16_t area, data_entry* key) :
        request_get(area, key)
    {
      setPCode (TAIR_REQ_REMOVE_PACKET);
    }

    request_remove (request_remove & packet):request_get (packet)
    {
      setPCode (TAIR_REQ_REMOVE_PACKET);
    }


    bool encode (tbnet::DataBuffer * output)
    {
      return request_get::encode (output);
    }
  };

  class request_remove_dup:public request_remove
  {
  public:

    request_remove_dup ():request_remove ()
    {   
      setPCode (TAIR_REQ_REMOVE_DUPLICATE_PACKET);
      packet_id = 0;
      bucket_id = -1; 
    }   

    request_remove_dup (request_remove_dup& packet) : request_remove(packet)
    {   
      setPCode (TAIR_REQ_REMOVE_DUPLICATE_PACKET);
      packet_id = 0;
      bucket_id = -1; 
    }   
    request_remove_dup (request_remove& packet) : request_remove(packet)
    {   
      setPCode (TAIR_REQ_REMOVE_DUPLICATE_PACKET);
      packet_id = 0;
      bucket_id = -1; 
    }   

    bool encode (tbnet::DataBuffer * output)
    {   
      bool ret = request_remove::encode (output);
      if (!ret) {
          return false;
      }   
      output->writeInt32(packet_id);
      output->writeInt32(bucket_id);
      return true;
    }   

    bool decode (tbnet::DataBuffer *input, tbnet::PacketHeader *header) {
        if (!request_remove::decode(input, header)) {
            return false;
        }   
        packet_id = input->readInt32();
        bucket_id = input->readInt32();
        return true;
    }   

    uint32_t get_packet_id() {
        return packet_id;
    }   

    void set_packet_id(uint32_t id) {
        packet_id = id; 
    }   

    void output_data () {
        log_info("pcode => %d", getPCode());
        log_info("key_count => %d", key_count);
        log_info("area => %d", key_count);
        log_info("server_flag => %d", server_flag);
        log_info("datalen => %d", _packetHeader._dataLen);
    }

    uint32_t packet_id;
    int32_t bucket_id;
  };  
}
#endif
