/*
 * (C) 2007-2010 Alibaba Group Holding Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * llen packet
 *
 * Version: $Id: llen_packet.hpp 28 2010-09-19 05:18:09Z ruohai@taobao.com $
 *
 * Authors:
 *   ruohai <ruohai@taobao.com>
 *     - initial release
 *
 */
#ifndef TAIR_PACKET_LLEN_PACKET_H
#define TAIR_PACKET_LLEN_PACKET_H
#include "base_packet.hpp"
namespace tair
{
  class request_llen:public base_packet
  {
  public:
    request_llen()
    {
      setPCode(TAIR_REQ_LLEN_PACKET);
      server_flag = 0;
      area        = 0;
    }

    request_llen(request_llen & packet)
    {
      setPCode(packet.getPCode());
      server_flag = packet.server_flag;
      area        = packet.area;

      key.clone(packet.key);
    }

    bool encode(tbnet::DataBuffer * output)
    {
      output->writeInt8(server_flag);
      output->writeInt16(area);

      output->writeInt32(key.get_size());
      output->writeBytes(key.get_data(), key.get_size());
      return true;
    }

    bool decode(tbnet::DataBuffer * input, tbnet::PacketHeader * header)
    {

      HEADER_VERIFY;

      GETKEY_FROM_DATAENTRY(input, key);

      return true;
    }

  public:
    uint16_t area;
    data_entry key;
  };

  class response_llen:public base_packet
  {
  public:

    response_llen()
    {
      setPCode(TAIR_RESP_LLEN_PACKET);
      config_version = 0;
      code           = 0;
    }

    bool encode(tbnet::DataBuffer * output)
    {
      output->writeInt32(config_version);
      output->writeInt32(code);
      output->writeInt64(retnum);

      return true;
    }

    bool decode(tbnet::DataBuffer * input, tbnet::PacketHeader * header)
    {
      GETKEY_FROM_INT32(input, config_version);
      GETKEY_FROM_INT32(input, code);
      GETKEY_FROM_INT64(input, retnum);

      return true;
    }

    void set_meta(uint32_t config_version, uint32_t code)
    {
      this->config_version = config_version;
      //this->version           = version;
      this->code = code;
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
    long long retnum;
  private:
    int32_t code;
    response_llen (const response_llen &);
  };
}
#endif
