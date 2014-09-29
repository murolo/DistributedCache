/*
 * (C) 2007-2010 Alibaba Group Holding Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * zadd packet
 *
 * Version: $Id: zadd_packet.hpp 28 2010-09-19 05:18:09Z ruohai@taobao.com $
 *
 * Authors:
 *   ruohai <ruohai@taobao.com>
 *     - initial release
 *
 */
#ifndef TAIR_PACKET_ZADD_PACKET_H
#define TAIR_PACKET_ZADD_PACKET_H
#include "base_packet.hpp"
namespace tair
{
  class request_zadd:public base_packet
  {
  public:
    request_zadd()
    {
      setPCode(TAIR_REQ_ZADD_PACKET);
      server_flag = 0;
      area        = 0;
      version     = 0;
      expire      = 0;
    }

    request_zadd(request_zadd &packet)
    {
      setPCode(packet.getPCode());
      server_flag = packet.server_flag;
      area        = packet.area;
      score       = packet.score;
      version     = packet.version;
      expire      = packet.expire;
      key.clone(packet.key);
      value.clone(packet.value);
    }

    bool encode(tbnet::DataBuffer *output)
    {
      CREATE_HEADER;

      PUT_INT16_TO_BUFFER(output, version);
      PUT_INT32_TO_BUFFER(output, expire);
      PUT_DOUBLE_TO_BUFFER(output, score);
      PUT_DATAENTRY_TO_BUFFER(output, key);
      PUT_DATAENTRY_TO_BUFFER(output, value);

      return true;
    }

    bool decode(tbnet::DataBuffer *input, tbnet::PacketHeader *header)
    {

      HEADER_VERIFY;

      GETKEY_FROM_INT16(input, version);
      GETKEY_FROM_INT32(input, expire);
      GETKEY_FROM_DOUBLE(input, score);
      GETKEY_FROM_DATAENTRY(input, key);
      GETKEY_FROM_DATAENTRY(input, value);

      key.set_version(version);

      return true;
    }

  public:
    uint16_t   area;
    uint16_t   version;
    int32_t    expire;
    double     score;
    data_entry key;
    data_entry value;
  };

class request_zadd_dup : public request_zadd {
      public:
          request_zadd_dup() : request_zadd(){
              setPCode(TAIR_REQ_ZADD_DUPLICATE_PACKET);
              packet_id = 0;
              bucket_id = -1; 
          }   

          request_zadd_dup(request_zadd& packet) : request_zadd(packet) {
              setPCode(TAIR_REQ_ZADD_DUPLICATE_PACKET);
              packet_id = 0;
              bucket_id = -1; 
          }   

          ~request_zadd_dup() {
          }   

          bool encode(tbnet::DataBuffer* output) {
              request_zadd::encode(output);
              output->writeInt32(packet_id);
              output->writeInt32(bucket_id);
              return true;
          }   

          bool decode(tbnet::DataBuffer* input, tbnet::PacketHeader* header) {
              if (!request_zadd::decode(input, header)) {
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

        public:
            uint32_t      packet_id;
            int32_t       bucket_id;
  };  

  class response_zadd:public base_packet
  {
  public:

    response_zadd()
    {
      setPCode(TAIR_RESP_ZADD_PACKET);
      config_version = 0;
      code           = 0;
    }

    bool encode(tbnet::DataBuffer *output)
    {
      PUT_INT32_TO_BUFFER(output, config_version);
      PUT_INT32_TO_BUFFER(output, code);

      return true;
    }

    bool decode(tbnet::DataBuffer *input, tbnet::PacketHeader *header)
    {
      GETKEY_FROM_INT32(input, config_version);
      GETKEY_FROM_INT32(input, code);

      return true;
    }

    void set_meta(uint32_t config_version, uint32_t code)
    {
      this->config_version = config_version;
      this->code           = code;
    }

    void set_code(int code)
    {
      this->code = code;
    }

    int get_code()
    {
      return code;
    }

    //not used
    void set_version(uint16_t version) {}
  public:
    uint32_t config_version;
  private:
    int32_t code;
    response_zadd(const response_zadd &);
  };
}
#endif
