/*
 * (C) 2007-2010 Alibaba Group Holding Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * zrangebyscore packet
 *
 * Version: $Id: zrangebyscore_packet.hpp 28 2010-09-19 05:18:09Z ruohai@taobao.com $
 *
 * Authors:
 *   ruohai <ruohai@taobao.com>
 *     - initial release
 *
 */
#ifndef TAIR_PACKET_GENERIC_ZRANGEBYSCORE_PACKET_H
#define TAIR_PACKET_GENERIC_ZRANGEBYSCORE_PACKET_H
#include "base_packet.hpp"
#include <stdint.h>

#include "storage_manager.hpp"

namespace tair
{
  class request_generic_zrangebyscore:public base_packet
  {
  public:
    request_generic_zrangebyscore()
    {
      setPCode(TAIR_REQ_GENERIC_ZRANGEBYSCORE_PACKET);

      server_flag = 0;
      area        = 0;
      start       = 0.0;
      end         = 0.0;
      limit       = -1;  //mean not set limit
      withscore   = 0;
    }

    request_generic_zrangebyscore(request_generic_zrangebyscore & packet)
    {
      setPCode(packet.getPCode());
      server_flag = packet.server_flag;
      area        = packet.area;
      start       = packet.start;
      end         = packet.end;
      limit       = packet.limit;
      withscore   = packet.withscore;
      key.clone(packet.key);
    }

    bool encode(tbnet::DataBuffer *output)
    {
      CREATE_HEADER;

      PUT_DOUBLE_TO_BUFFER(output, start);
      PUT_DOUBLE_TO_BUFFER(output, end);
      PUT_DATAENTRY_TO_BUFFER(output, key);
      PUT_INT32_TO_BUFFER(output, reverse);
      PUT_INT32_TO_BUFFER(output, limit);
      PUT_INT32_TO_BUFFER(output, withscore);

      return true;
    }

    bool decode(tbnet::DataBuffer * input, tbnet::PacketHeader * header)
    {
      HEADER_VERIFY;

      GETKEY_FROM_DOUBLE(input, start);
      GETKEY_FROM_DOUBLE(input, end);
      GETKEY_FROM_DATAENTRY(input, key);
      GETKEY_FROM_INT32(input, reverse);
      GETKEY_FROM_INT32(input, limit);
      GETKEY_FROM_INT32(input, withscore);

      return true;
    }

  public:
    uint16_t    area;
    data_entry  key;
    double      start;
    double      end;
    int         reverse;
    int         limit;
    int         withscore;
  };

  class response_generic_zrangebyscore:public base_packet
  {
#define RESPONSE_VALUES_MAXSIZE 32767
  public:
    response_generic_zrangebyscore()
    {
      setPCode(TAIR_RESP_GENERIC_ZRANGEBYSCORE_PACKET);

      config_version = 0;
      needfree       = true;

      values.clear();
    }

    ~response_generic_zrangebyscore()
    {
      if (needfree) {
        for (size_t i = 0; i < values.size (); ++i)
        {
          data_entry *entry = values[i];
          if (entry != NULL)
            delete (entry);
        }
      }
      values.clear();
    }

    bool encode(tbnet::DataBuffer *output)
    {
      if (values.size() > RESPONSE_VALUES_MAXSIZE) return false;

      PUT_INT32_TO_BUFFER(output, config_version);
      PUT_INT16_TO_BUFFER(output, version);
      PUT_INT32_TO_BUFFER(output, code);
      PUT_INT32_TO_BUFFER(output, scores.size()); // to flag packet if this not zero, mean have scores

      if (scores.size() == 0) {
        PUT_DATAVECTOR_TO_BUFFER(output, values);
      } else {
        assert(scores.size() == values.size());
        PUT_INT32_TO_BUFFER(output, values.size());
        for (size_t i = 0; i < values.size(); i++)
        {
          PUT_PDATAENTRY_TO_BUFFER(output, values[i]);
          PUT_DOUBLE_TO_BUFFER(output, scores[i]);
        }
      }

      return true;
    }

    bool decode(tbnet::DataBuffer *input, tbnet::PacketHeader *header)
    {
      size_t scores_size = 0;

      GETKEY_FROM_INT32(input, config_version);
      GETKEY_FROM_INT16(input, version);
      GETKEY_FROM_INT32(input, code);
      GETKEY_FROM_INT32(input, scores_size);

      if (scores_size == 0) {
        GETKEY_FROM_DATAVECTOR(input, values);
      } else {
        double       score = 0.0;
        data_entry* value  = NULL;
        size_t values_size = 0;
        GETKEY_FROM_INT32(input, values_size);
        for (size_t i = 0; i < values_size; i++)
        {
          GETKEY_FROM_PDATAENTRY(input, value);
          values.push_back(value);
          GETKEY_FROM_DOUBLE(input, score);
          scores.push_back(score);
        }
      }

      return true;
    }

    void set_meta(uint32_t config_version, uint32_t code)
    {
      this->config_version = config_version;
      this->code           = code;
    }

    void set_version(uint16_t version)
    {
      this->version = version;
    }

    void add_data(data_entry *data)
    {
      if (data == NULL) return;
      values.push_back(data);
    }

    int get_code()
    {
      return code;
    }

    void set_need_free(bool free)
    {
      this->needfree = free;
    }
  public:
    uint32_t config_version;
    uint16_t version;
    int32_t  code;
    vector<data_entry *> values;
    vector<double>       scores;
  private:
    bool needfree;
    response_generic_zrangebyscore(const response_generic_zrangebyscore &packet);
  };
}				// end namespace
#endif
