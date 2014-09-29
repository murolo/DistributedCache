#ifndef __BASE_TEST__
#define __BASE_TEST__

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <gtest/gtest.h>

#include "test_helper.hpp"

#include "tair_client_api_impl.hpp"

using namespace tair;

//const static short NOT_CARE_VERSION = 0;
//const static int NOT_CARE_EXPIRE = -1;
//const static int CANCEL_EXPIRE = 0;


class TairClientRDBTest:public testing::Test {
protected:
    virtual void SetUp() {
        //init
        TBSYS_LOGGER.setLogLevel("ERROR");
        ReadTestConfig();
        bool done = tairClient.startup(server_addr, slave_addr, group_name);
        ASSERT_EQ(done, true);
        printf("---------------------start up----------------------\n");
    }

    virtual void TearDown() {
        //finit
        tairClient.close();
        printf("---------------------close-------------------------\n");
    }
private:
    void ReadTestConfig() {
      FILE* fp = fopen("test.conf", "r");
      ASSERT_NE(true, fp == NULL);
      char buffer[4096];
      while(fgets(buffer, 4096, fp) != NULL) {
        ParseLine(buffer, strlen(buffer));
      }
      fclose(fp);
    }

    inline bool IsWhiteSpace(char c) {
      return c == ' ' || c == '\t' || c == '\r' || c == '\n';
    }

    void ParseLine(const char* buffer, const int length) {
      int start = 0, end = length - 1;
      for (; start <= end && IsWhiteSpace(buffer[start]); start++);
      for (; end >= 0 && IsWhiteSpace(buffer[end]); end--);
      int equals_flag = start;
      for (; equals_flag <= end && buffer[equals_flag] != '='; equals_flag++);
      if (equals_flag > end) return;
      int left_end = equals_flag - 1;
      for (; left_end >= start && IsWhiteSpace(buffer[left_end]); left_end--);
      if (left_end < start) return;
      int right_start = equals_flag + 1;
      for (; right_start <= end && IsWhiteSpace(buffer[right_start]); right_start++);
      if (right_start > end) return;

      char* key = (char*)malloc(sizeof(char) * (left_end - start + 1 + 1));
      memcpy(key, buffer + start, left_end - start + 1);
      key[left_end - start + 1] = '\0';
      char* value = (char*)malloc(sizeof(char) * (end - right_start + 1 + 1));
      memcpy(value, buffer + right_start, end - right_start + 1);
      value[end - right_start + 1] = '\0';

      if (strcmp("server_addr", key) == 0) {
        server_addr = value;
      } else if (strcmp("slave_addr", key) == 0) {
        slave_addr = value;
      } else if (strcmp("group_name", key) == 0) {
        group_name = value;
      } else if (strcmp("default_namespace", key) == 0) {
        default_namespace = atoi(value);
      } else if (strcmp("max_namespace", key) == 0) {
        max_namespace = atoi(value);
      } else {
        free(value);
      }
      free(key);
    }
public:
    tair_client_impl tairClient;
public:
    static char* server_addr;
    static char* slave_addr;
    static char* group_name;

    static int default_namespace;
    static int max_namespace;
};

char* TairClientRDBTest::server_addr = NULL;
char* TairClientRDBTest::slave_addr  = NULL;
char* TairClientRDBTest::group_name  = NULL;

int TairClientRDBTest::default_namespace = 0;
int TairClientRDBTest::max_namespace     = 0;

#endif
