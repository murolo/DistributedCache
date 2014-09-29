#ifndef TAIR_PACKET_LSET_PACKET_H
#define TAIR_PACKET_LSET_PACKET_H

#include "hset_packet.hpp"

namespace tair {
  class request_lset : public base_packet
  {
  public:
    request_lset()
    {
      setPCode(TAIR_REQ_LSET_PACKET);
      server_flag = 0;
      area        = 0;
      index       = 0;
      version     = 0;
      expire      = 0;
    }

    request_lset(request_lset & packet)
    {
      setPCode (TAIR_REQ_LSET_PACKET);
      server_flag = packet.server_flag;
      version     = packet.version;
      expire      = packet.expire;
      area        = packet.area;
      index       = packet.index;
      key.clone(packet.key);
      value.clone(packet.value);
    }

    bool encode(tbnet::DataBuffer* output)
    {
      CREATE_HEADER;

      PUT_INT16_TO_BUFFER(output, version);
      PUT_INT32_TO_BUFFER(output, expire);
      PUT_INT32_TO_BUFFER(output, index);

      PUT_DATAENTRY_TO_BUFFER(output, key);
      PUT_DATAENTRY_TO_BUFFER(output, value);

      return true;
    }

    bool decode(tbnet::DataBuffer* input, tbnet::PacketHeader* header)
    {
      HEADER_VERIFY;

      GETKEY_FROM_INT16(input, version);
      GETKEY_FROM_INT32(input, expire);
      GETKEY_FROM_INT32(input, index);
      GETKEY_FROM_DATAENTRY(input, key);
      GETKEY_FROM_DATAENTRY(input, value);

      key.set_version(version);

      return true;
    }

  public:
    uint16_t   area;
    int32_t    index;
    uint16_t   version;
    int        expire;
    data_entry key;
    data_entry value;
  };

  class request_lset_dup : public request_lset {
      public:
          request_lset_dup() : request_lset(){
              setPCode(TAIR_REQ_LSET_DUPLICATE_PACKET);
              packet_id = 0;
              bucket_id = -1; 
          }   

          request_lset_dup(request_lset& packet) : request_lset(packet) {
              setPCode(TAIR_REQ_LSET_DUPLICATE_PACKET);
              packet_id = 0;
              bucket_id = -1; 
          }   

          ~request_lset_dup() {
          }   

          bool encode(tbnet::DataBuffer* output) {
              request_lset::encode(output);
              output->writeInt32(packet_id);
              output->writeInt32(bucket_id);
              return true;
          }   

          bool decode(tbnet::DataBuffer* input, tbnet::PacketHeader* header) {
              if (!request_lset::decode(input, header)) {
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


  class response_lset:public response_hset
  {
  public:
    response_lset() : response_hset()
    {
      setPCode(TAIR_RESP_LSET_PACKET);
    }
  };
}

#endif
