/*
 * (C) 2007-2010 Alibaba Group Holding Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * duplicate packet
 *
 * Version: $Id: duplicate_packet.hpp 28 2010-09-19 05:18:09Z ruohai@taobao.com $
 *
 * Authors:
 *   ruohai <ruohai@taobao.com>
 *     - initial release
 *
 */
#ifndef TAIR_PACKET_DUPLICATE_PUT_PACKET_H
#define TAIR_PACKET_DUPLICATE_PUT_PACKET_H
#include "put_packet.hpp"
namespace tair {
    class request_put_dup : public request_put {
        public:

            request_put_dup()
            {
                setPCode(TAIR_REQ_PUT_DUPLICATE_PACKET);
                packet_id = 0;
                bucket_id = -1;
            }

            request_put_dup(request_put& packet):request_put(packet)
            {
                setPCode(TAIR_REQ_PUT_DUPLICATE_PACKET);
                packet_id = 0;
                bucket_id = -1;
            }

            ~request_put_dup()
            {
            }


            bool encode(tbnet::DataBuffer *output)
            {
                request_put::encode(output);
                output->writeInt32(packet_id);
                output->writeInt32(bucket_id);
                return true;
            }


            bool decode(tbnet::DataBuffer *input, tbnet::PacketHeader *header) 
            {
                if (!request_put::decode(input, header)) {
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

    class request_putnx_dup : public request_putnx {
        public:

            request_putnx_dup()
            {   
                setPCode(TAIR_REQ_PUTNX_DUPLICATE_PACKET);
                bucket_id = -1; 
                packet_id = 0;
            }   

            request_putnx_dup(request_putnx& packet):request_putnx(packet)
            {   
                setPCode(TAIR_REQ_PUTNX_DUPLICATE_PACKET);
                packet_id = 0;
                bucket_id = -1; 
            }   

            ~request_putnx_dup()
            {   
            }   


            bool encode(tbnet::DataBuffer *output)
            {   
                request_put::encode(output);
                output->writeInt32(packet_id);
                output->writeInt32(bucket_id);
                return true;
            }   


            bool decode(tbnet::DataBuffer *input, tbnet::PacketHeader *header) 
            {   
                if (!request_put::decode(input, header)) {
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
}
#endif
