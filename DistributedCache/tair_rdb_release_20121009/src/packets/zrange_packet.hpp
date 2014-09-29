/*
 * (C) 2007-2010 Alibaba Group Holding Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * zrange packet
 *
 * Version: $Id: zrange_packet.hpp 28 2010-09-19 05:18:09Z ruohai@taobao.com $
 *
 * Authors:
 *   ruohai <ruohai@taobao.com>
 *     - initial release
 *
 */
#ifndef TAIR_PACKET_ZRANGE_PACKET_H
#define TAIR_PACKET_ZRANGE_PACKET_H
#include "base_packet.hpp"
#include <stdint.h>

#include "storage_manager.hpp"

namespace tair
{
  class request_zrange:public base_packet
  {
  public:
    request_zrange()
    {
      setPCode(TAIR_REQ_ZRANGE_PACKET);

      server_flag = 0;
      area        = 0;
      start       = 0;
      end         = 0;
      withscore   = 0;
    }

    request_zrange(const request_zrange &packet)
    {
      setPCode(packet.getPCode());
      server_flag = packet.server_flag;
      area        = packet.area;
      start       = packet.start;
      end         = packet.end;
      withscore   = packet.withscore;
      key.clone(packet.key);
    }

    bool encode(tbnet::DataBuffer *output)
    {
      CREATE_HEADER;

      PUT_INT32_TO_BUFFER(output, start);
      PUT_INT32_TO_BUFFER(output, end);
      PUT_INT32_TO_BUFFER(output, withscore);
      PUT_DATAENTRY_TO_BUFFER(output, key);

      return true;
    }

    bool decode(tbnet::DataBuffer *input, tbnet::PacketHeader *header)
    {
      HEADER_VERIFY;

      GETKEY_FROM_INT32(input, start);
      GETKEY_FROM_INT32(input, end);
      GETKEY_FROM_INT32(input, withscore);
      GETKEY_FROM_DATAENTRY(input, key);

      return true;
    }

  public:
    uint16_t   area;
    data_entry key;
    int32_t    start;
    int32_t    end;
    int32_t    withscore;
  };

  class response_zrange:public base_packet
  {
#define RESPONSE_VALUES_MAXSIZE 32767
  public:
    response_zrange()
    {
      setPCode(TAIR_RESP_ZRANGE_PACKET);

      needfree       = true;
      config_version = 0;

      values.clear();
    }

    ~response_zrange() {
      if (needfree) {
        for (size_t i = 0; i < values.size (); ++i)
        {
          data_entry *entry = values[i];
          if (entry != NULL) {
            delete (entry);
          }
        }
      }
      values.clear();
    }

    bool encode(tbnet::DataBuffer *output)
    {
      if (values.size() > RESPONSE_VALUES_MAXSIZE)
      {
        log_warn("zrange values_size = %zu, larger than RESPONSE_VALUES_MAXSZIE", values.size());
        return false;
      }

      PUT_INT32_TO_BUFFER(output, config_version);
      PUT_INT16_TO_BUFFER(output, version);
      PUT_INT32_TO_BUFFER(output, code);
      PUT_DATAVECTOR_TO_BUFFER(output, values);

      return true;
    }

    bool decode(tbnet::DataBuffer *input, tbnet::PacketHeader *header)
    {
      GETKEY_FROM_INT32(input, config_version);
      GETKEY_FROM_INT16(input, version);
      GETKEY_FROM_INT32(input, code);
      GETKEY_FROM_DATAVECTOR(input, values);

      return true;
    }

    void set_meta (uint32_t config_version, uint32_t code)
    {
      this->config_version = config_version;
      this->code           = code;
    }

    void set_version(uint16_t version)
    {
      this->version = version;
    }

    void add_data(data_entry * data)
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
    uint32_t code;
    vector<data_entry *> values;
    vector<double>       scores;
  private:
    bool needfree;
    response_zrange(const response_zrange &packet);
  };

  class response_zrangewithscore : public base_packet
  {
#define RESPONSE_VALUES_MAXSIZE 32767
  public:
    response_zrangewithscore()
    {
      setPCode(TAIR_RESP_ZRANGEWITHSCORE_PACKET);

      config_version = 0;
      needfree       = true;
      values.clear();
      scores.clear();
    }

    ~response_zrangewithscore() {
      if (needfree) {
        for (size_t i = 0; i < values.size(); ++i)
        {
          data_entry *entry = values[i];
          delete (entry);
        }
      }
      values.clear();
      scores.clear();
    }

    bool encode(tbnet::DataBuffer *output)
    {
      if (values.size() > RESPONSE_VALUES_MAXSIZE)
      {
        log_warn("zrange withscore values_size = %zu, larger than RESPONSE_VALUES_MAXSZIE", values.size());
        return false;
      }

      PUT_INT32_TO_BUFFER(output, config_version);
      PUT_INT16_TO_BUFFER(output, version);
      PUT_INT32_TO_BUFFER(output, code);
      PUT_INT32_TO_BUFFER(output, values.size());

      double score = 0.0;
      for (size_t i = 0; i < values.size(); i++)
      {
        PUT_PDATAENTRY_TO_BUFFER(output, values[i]);

        score = 0;
        if (i < scores.size()) {
          score = scores[i];
        }

        PUT_DOUBLE_TO_BUFFER(output, score);
      }

      return true;
    }

    bool decode(tbnet::DataBuffer *input, tbnet::PacketHeader *header)
    {
      GETKEY_FROM_INT32(input, config_version);
      GETKEY_FROM_INT16(input, version);
      GETKEY_FROM_INT32(input, code);

      size_t value_size = 0;
      GETKEY_FROM_INT32(input, value_size);

      double score      = 0;
      data_entry *value = NULL;
      for (size_t i = 0; i < value_size; i++)
      {
        GETKEY_FROM_PDATAENTRY(input, value);
        values.push_back(value);
        GETKEY_FROM_DOUBLE(input, score);
        scores.push_back(score);
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

    void add_data(data_entry *data, double score)
    {
      if (data == NULL) return;
      values.push_back(data);
      scores.push_back(score);
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
    response_zrangewithscore(const response_zrange &packet);
  };
}				// end namespace
#endif
