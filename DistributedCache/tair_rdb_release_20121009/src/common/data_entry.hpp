/*
 * (C) 2007-2010 Alibaba Group Holding Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * data_entry wrap item of tair, it can be key or value
 *
 * Version: $Id: data_entry.hpp 242 2011-09-14 14:18:23Z yexiang.ych@taobao.com $
 *
 * Authors:
 *   ruohai <ruohai@taobao.com>
 *     - initial release
 *
 */
#ifndef TAIR_DATA_ENTRY_HPP
#define TAIR_DATA_ENTRY_HPP
#include <set>
#include "tbsys.h"
#include "tbnet.h"
#include "util.hpp"
#include "item_data_info.hpp"

namespace tair
{
   namespace common {
      using namespace tair::util;
      class data_entry {
      public:
         data_entry()
          {
            init();
         }
         data_entry(const data_entry &entry)
          {
            alloc = false;
            data = NULL;
            m_true_data = NULL;

            set_data(entry.data, entry.size, entry.alloc);
            has_merged = entry.has_merged;
            has_meta_merged = entry.has_meta_merged;
            area = entry.area;
            hashcode = 0;
            server_flag = entry.server_flag;
            data_meta = entry.data_meta;
         }

         data_entry& operator=(const data_entry &entry)
          {
            if (this == &entry)
               return *this;
            set_data(entry.data, entry.size, entry.alloc);
            has_merged = entry.has_merged;
            has_meta_merged = entry.has_meta_merged;
            area = entry.area;
            hashcode = 0;
            server_flag = entry.server_flag;
            data_meta = entry.data_meta;

            return *this;
         }

         data_entry& clone(const data_entry &entry)
          {
            assert(this != &entry);

            set_data(entry.data, entry.size, true);
            has_merged = entry.has_merged;
            has_meta_merged = entry.has_meta_merged;
            area = entry.area;
            hashcode = 0;
            server_flag = entry.server_flag;
            data_meta = entry.data_meta;
            return *this;
         }


         bool operator<(const data_entry &entry) const
         {
            if(size == 0 || entry.size == 0 || data == NULL || entry.data == NULL) return false;

            int min_size = (size>entry.size?entry.size:size);
            int r = memcmp(data, entry.data, min_size);
            if (r<0)
               return true;
            else if (r>0)
               return false;
            else
               return (size<entry.size);
         }

        bool operator==(const data_entry &entry) const
        {
          if (entry.size != size)
            return false;
          if (has_merged != entry.has_merged)
            return false;
          if (!has_merged && area != entry.area)
            return false;

          if (data == entry.data)
            return true;
          if (data == NULL || entry.data == NULL)
            return false;

          return memcmp(data, entry.data, size) == 0;
        }


         void print_out()
         {
            log_debug("size: %d", size);
            if(size > 0) {
               char debug_data[4*size];
               char *d = (char *)data;
               for(int j=0; j<size; j++) {
                  sprintf(debug_data+3*j, "%02X ", (d[j] & 0xFF));
               }
               log_debug( "TairDataEntry(%d) - %s", size, debug_data);
            }
         }

         data_entry(const char *str, bool alloc = true)
          {
            init();
            set_data(str, strlen(str), alloc);
         }

         data_entry(const char *data, int size, bool alloc = true)
          {
            init();
            set_data(data, size, alloc);
         }

         ~data_entry()
          {
            free_data();
         }

         void merge_area(int area)
          {
            if(has_merged) {
               return;
            }
            if(size > 0) {
               char new_data[size + 2];
               memset(new_data, 0, size+2);
               new_data[0] = (area & 0xFF);
               new_data[1] = ((area >> 8) & 0xFF);
               memcpy(new_data+2, data, size);
               set_data(new_data, size+2);
               this->area = area;
               has_merged = true;
            }
         }

         int decode_area()
          {
            int target_area = area;
            if(has_merged) {
               assert(size > 2);
               target_area = data[1];
               target_area <<= 8;
               target_area |= data[0];
               char new_data[size -2];
               memset(new_data, 0, size-2);
               memcpy(new_data, data+2, size-2);
               set_data(new_data, size -2);
               area = target_area;
               has_merged =false;
            }
            return target_area;
         }

         void merge_meta()
          {
            if (has_meta_merged) return;

            if (size > 0) {
               int new_size = size + sizeof(item_meta_info);
               char *new_data = (char *)malloc(new_size);
               assert (new_data != NULL);

               char *p = new_data;
               memcpy(p, &data_meta, sizeof(item_meta_info));
               p += sizeof(item_meta_info);
               memcpy(p, data, size);

               //log_debug("meta merged, newsize: %d", new_size);
               set_alloced_data(new_data, new_size);
               has_meta_merged = true;
            }

         }

         void decode_meta(bool force_decode = false)
          {
            if (has_meta_merged || force_decode) {
               assert(size > (int)sizeof(item_meta_info));
               int new_size = size - sizeof(item_meta_info);
               char *new_data = (char *)malloc(new_size);
               assert (new_data != NULL);

               char *p = data;
               memcpy(&data_meta, data, sizeof(item_meta_info));
               p += sizeof(item_meta_info);
               memcpy(new_data, p, new_size);

               set_alloced_data(new_data, new_size);
               has_meta_merged = false;
            }
         }

         void set_alloced_data(const char *data, int size)
         {
            free_data();
            this->data = (char *) data;
            this->size = size;
            alloc = true;
         }

         void set_data(const char *new_data, int new_size, bool alloc = true)
          {
            free_data();
            if(alloc) {
               if(new_size > 0) {
                  data = (char *)malloc(new_size + 1);
                  assert(data != NULL);
                  this->alloc = true;
                  size = new_size;
                  if(new_data) {
                     memcpy(data, new_data, size);
                  }else{
                     memset(data, 0, size);
                  }
                  *(data + size) = '\0';
               }
            } else {
               data = (char *) new_data;
               size = new_size;
            }
         }

         char *get_data() const
          {
            return data;
         }


         int get_size() const
          {
            return size;
         }

         void set_version(uint16_t version)
         {
            data_meta.version = version;
         }

         uint16_t get_version() const
         {
            return data_meta.version;
         }

         uint64_t get_hashcode()
         {
            if(hashcode == 0 && size > 0 && data != NULL) {
               hashcode = hash_util::mhash1(data, size);
               hashcode <<= 32;
               hashcode |= hash_util::mhash2(data, size);
            }

            return hashcode;
         }

         uint32_t get_cdate() const
          {
            return data_meta.cdate;
         }

         void  set_cdate(uint32_t cdate)
          {
            data_meta.cdate = cdate;
         }
         bool is_alloc() const {
            return alloc;
         }

         size_t encoded_size() {
            return 40 + get_size();
         }

         void encode(tbnet::DataBuffer *output) const
         {
            output->writeInt8(has_merged);
            output->writeInt32(area);
            output->writeInt16(server_flag);
            data_meta.encode(output);

            output->writeInt32(get_size());
            if (get_size() > 0) {
               output->writeBytes(get_data(), get_size());
            }
         }
         void decode(tbnet::DataBuffer *input)
          {
            free_data();
            uint8_t temp_merged = input->readInt8();
            int area = input->readInt32();
            uint16_t flag = input->readInt16();
            data_meta.decode(input);

            int size = input->readInt32();
            if (size > 0) {
               set_data(NULL, size);
               input->readBytes(get_data(), size);
            }
            has_merged = temp_merged;
            area = area;
            server_flag = flag;
         }

      private:
         void init()
          {
            alloc = false;
            size = 0;
            data = NULL;

            has_merged = false;
            has_meta_merged = false;
            area = 0;
            hashcode = 0;
            server_flag = 0;
            memset(&data_meta, 0, sizeof(item_meta_info));
         }

         inline void free_data()
          {
            if (data && alloc) {
               free(data);
            }
            data = NULL;
            alloc = false;
            size = 0;
            hashcode = 0;
            has_merged = false;
            area = 0;
         }

      private:
         int size;
         char *data;
         bool alloc;
         uint64_t hashcode;

         int m_true_size;  //if has_merged ,or m_true_size=size+2
         char *m_true_data; //if has_merged arae,same as data,or data=m_true_data+2;
      public:
         bool has_merged;
         bool has_meta_merged;
         uint32_t area;
         uint16_t server_flag;
         item_meta_info data_meta;
      };

      class data_entry_comparator {
      public:
         bool operator() (const data_entry *a, const data_entry *b)
          {
            return ((*a) < (*b));
         }
      };

      struct data_entry_hash {
         size_t operator()(const data_entry *a) const
          {
            if(a == NULL || a->get_data() == NULL || a->get_size() == 0) return 0;

            return string_util::mur_mur_hash(a->get_data(), a->get_size());
         }
      };

      struct data_entry_equal_to {
       bool operator()(const data_entry *a, const data_entry *b) const
       {
         return *a == *b;
       }
     };

      typedef std::vector<data_entry *> tair_dataentry_vector;
      typedef std::set<data_entry*, data_entry_comparator> tair_dataentry_set;
      typedef __gnu_cxx::hash_map<data_entry*, data_entry*, data_entry_hash> tair_keyvalue_map;
      typedef __gnu_cxx::hash_map<data_entry*, int, data_entry_hash, data_entry_equal_to> key_code_map_t;


     class value_entry {
     public:
       value_entry() : version(0), expire(0)
       {
       }

       value_entry(const value_entry &entry)
       {
         d_entry = entry.d_entry;
         version = entry.version;
         expire = entry.expire;
       }

       value_entry& clone(const value_entry &entry)
       {
         assert(this != &entry);
         d_entry = entry.d_entry;
         version = entry.version;
         expire = entry.expire;

         return *this;
       }

       void set_d_entry(const data_entry& in_d_entry)
       {
         d_entry = in_d_entry;
       }

       data_entry& get_d_entry()
       {
         return d_entry;
       }

       void set_expire(int32_t expire_time)
       {
         expire = expire_time;
       }

       int32_t get_expire() const
       {
         return expire;
       }

       void set_version(uint16_t kv_version)
       {
         version = kv_version;
       }

       uint16_t get_version() const
       {
         return version;
       }

       void encode(tbnet::DataBuffer *output) const
       {
         d_entry.encode(output);
         output->writeInt16(version);
         output->writeInt32(expire);
       }

       void decode(tbnet::DataBuffer *input)
       {
         d_entry.decode(input);
         version = input->readInt16();
         expire = input->readInt32();
       }

       int get_size() const
       {
         return d_entry.get_size()+ 2 + 4;
       }

     private:
       data_entry d_entry;
       uint16_t version;
       int32_t expire;
     };

     class mput_record {
     public:
       mput_record()
       {
         key = NULL;
         value = NULL;
       }

       mput_record(mput_record &rec)
       {
         key = new data_entry(*(rec.key));
         value = new value_entry(*(rec.value));
       }

       ~mput_record()
       {
         if (key != NULL ) {
           delete key;
           key = NULL;
         }
         if (value != NULL) {
           delete value;
           value = NULL;
         }
       }

     public:
       data_entry* key;
       value_entry* value;
     };

     typedef std::vector<mput_record*> mput_record_vec;

   }
}

#endif
