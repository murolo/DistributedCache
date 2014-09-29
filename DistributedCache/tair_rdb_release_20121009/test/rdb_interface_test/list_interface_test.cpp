#include "base_test.hpp"

TEST_F(TairClientRDBTest, test_numeric_lpush_rpop_llen_no_expire_no_version) {
  VDE* vde = vector_data_entry_numeric_rand(10);
  data_entry key("numeric_lpush_rpop_llen_no_expire_no_version");

  tairClient.remove(10, key);

  int length = 0;
  int resultcode = tairClient.llen(10, key, length);
  ASSERT_EQ(TAIR_RETURN_DATA_NOT_EXIST, resultcode);
  ASSERT_EQ(0, length);

  long successlen = 0;
  resultcode = tairClient.lpush(10, key, *vde, NOT_CARE_EXPIRE, (const int)NOT_CARE_VERSION, successlen);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
  ASSERT_EQ(10, successlen);

  VDE* vde1 = vector_data_entry_numeric_rand(8182);
  for (size_t i = 0; i < vde1->size(); i++) {
    data_entry* de = (*vde1)[i];
    resultcode = tairClient.lpush(10, key, de, NOT_CARE_EXPIRE, (int)NOT_CARE_VERSION);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);

    resultcode = tairClient.llen(10, key, length);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    ASSERT_EQ(i + 11, length);
  }

  data_entry* dentry = data_entry_numeric_rand();
  resultcode = tairClient.lpush(10, key, dentry, NOT_CARE_EXPIRE, NOT_CARE_VERSION);
  ASSERT_EQ(TAIR_RETURN_DATA_LEN_LIMITED, resultcode);
  data_entry_free(dentry);

  resultcode = tairClient.llen(10, key, length);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
  ASSERT_EQ(8192, length);

  std::vector<data_entry *> ret_values;
  resultcode = tairClient.rpop(10, key, 10, ret_values, NOT_CARE_EXPIRE, NOT_CARE_VERSION);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);

  ASSERT_EQ(1, vector_data_entry_cmp(vde, &ret_values));

  for (size_t i = 0; i < vde1->size(); i++) {
    data_entry* value = NULL;
    resultcode = tairClient.rpop(10, key, &value, NOT_CARE_EXPIRE, NOT_CARE_VERSION);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    ASSERT_EQ(1, data_entry_cmp((*vde1)[i], value));
    data_entry_free(value);
  }

  data_entry* value = NULL;
  resultcode = tairClient.rpop(10, key, &value, NOT_CARE_EXPIRE, NOT_CARE_VERSION);
  ASSERT_EQ(TAIR_RETURN_DATA_NOT_EXIST, resultcode);
  ASSERT_EQ(NULL, value);

  vector_data_entry_free(vde);
  vector_data_entry_free(vde1);

  resultcode = tairClient.remove(10, key);
  ASSERT_EQ(TAIR_RETURN_DATA_NOT_EXIST, resultcode);

}

TEST_F(TairClientRDBTest, test_lpush_rpop_llen_no_expire_no_version) {
  VDE* vde = vector_data_entry_rand(100, 10);
  data_entry key("lpush_rpop_llen_no_expire_no_version");

  tairClient.remove(10, key);

  int length = 0;
  int resultcode = tairClient.llen(10, key, length);
  ASSERT_EQ(TAIR_RETURN_DATA_NOT_EXIST, resultcode);
  ASSERT_EQ(0, length);

  long successlen = 0;
  resultcode = tairClient.lpush(10, key, *vde, NOT_CARE_EXPIRE, (const int)NOT_CARE_VERSION, successlen);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
  ASSERT_EQ(10, successlen);

  VDE* vde1 = vector_data_entry_rand(10, 8182);
  for (size_t i = 0; i < vde1->size(); i++) {
    data_entry* de = (*vde1)[i];
    resultcode = tairClient.lpush(10, key, de, NOT_CARE_EXPIRE, (int)NOT_CARE_VERSION);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);

    resultcode = tairClient.llen(10, key, length);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    ASSERT_EQ(i + 11, length);
  }

  data_entry* dentry = data_entry_rand(100);
  resultcode = tairClient.lpush(10, key, dentry, NOT_CARE_EXPIRE, NOT_CARE_VERSION);
  ASSERT_EQ(TAIR_RETURN_DATA_LEN_LIMITED, resultcode);
  data_entry_free(dentry);

  resultcode = tairClient.llen(10, key, length);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
  ASSERT_EQ(8192, length);

  std::vector<data_entry *> ret_values;
  resultcode = tairClient.rpop(10, key, 10, ret_values, NOT_CARE_EXPIRE, NOT_CARE_VERSION);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);

  ASSERT_EQ(1, vector_data_entry_cmp(vde, &ret_values));

  for (size_t i = 0; i < vde1->size(); i++) {
    data_entry* value = NULL;
    resultcode = tairClient.rpop(10, key, &value, NOT_CARE_EXPIRE, NOT_CARE_VERSION);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    ASSERT_EQ(1, data_entry_cmp((*vde1)[i], value));
    data_entry_free(value);
  }

  data_entry* value = NULL;
  resultcode = tairClient.rpop(10, key, &value, NOT_CARE_EXPIRE, NOT_CARE_VERSION);
  ASSERT_EQ(TAIR_RETURN_DATA_NOT_EXIST, resultcode);
  ASSERT_EQ(NULL, value);

  vector_data_entry_free(vde);
  vector_data_entry_free(vde1);

  resultcode = tairClient.remove(10, key);
  ASSERT_EQ(TAIR_RETURN_DATA_NOT_EXIST, resultcode);
}

TEST_F(TairClientRDBTest, test_numeric_rpush_lpop_llen_no_expire_no_version) {
  VDE* vde = vector_data_entry_numeric_rand(10);
  data_entry key("numeric_rpush_lpop_no_expire_no_version");

  tairClient.remove(10, key);

  int length = 0;
  int resultcode = tairClient.llen(10, key, length);
  ASSERT_EQ(TAIR_RETURN_DATA_NOT_EXIST, resultcode);
  ASSERT_EQ(0, length);

  long successlen = 0;
  resultcode = tairClient.rpush(10, key, *vde, NOT_CARE_EXPIRE, (const int)NOT_CARE_VERSION, successlen);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
  ASSERT_EQ(10, successlen);

  VDE* vde1 = vector_data_entry_numeric_rand(8182);
  for (size_t i = 0; i < vde1->size(); i++) {
    data_entry* de = (*vde1)[i];
    resultcode = tairClient.rpush(10, key, de, NOT_CARE_EXPIRE, NOT_CARE_VERSION);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);

    resultcode = tairClient.llen(10, key, length);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    ASSERT_EQ(i + 11, length);
  }

  data_entry* dentry = data_entry_numeric_rand();
  resultcode = tairClient.rpush(10, key, dentry, NOT_CARE_EXPIRE, NOT_CARE_VERSION);
  ASSERT_EQ(TAIR_RETURN_DATA_LEN_LIMITED, resultcode);
  data_entry_free(dentry);

  resultcode = tairClient.llen(10, key, length);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
  ASSERT_EQ(8192, length);

  std::vector<data_entry *> ret_values;
  resultcode = tairClient.lpop(10, key, 10, ret_values, NOT_CARE_EXPIRE, NOT_CARE_VERSION);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);

  ASSERT_EQ(1, vector_data_entry_cmp(vde, &ret_values));

  for (size_t i = 0; i < vde1->size(); i++) {
    data_entry* value = NULL;
    resultcode = tairClient.lpop(10, key, &value, NOT_CARE_EXPIRE, NOT_CARE_VERSION);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    ASSERT_EQ(1, data_entry_cmp((*vde1)[i], value));
    data_entry_free(value);
  }

  data_entry* value = NULL;
  resultcode = tairClient.lpop(10, key, &value, NOT_CARE_EXPIRE, NOT_CARE_VERSION);
  ASSERT_EQ(TAIR_RETURN_DATA_NOT_EXIST, resultcode);
  ASSERT_EQ(NULL, value);

  vector_data_entry_free(vde);
  vector_data_entry_free(vde1);

  resultcode = tairClient.remove(10, key);
  ASSERT_EQ(TAIR_RETURN_DATA_NOT_EXIST, resultcode);
}

TEST_F(TairClientRDBTest, test_rpush_lpop_llen_no_expire_no_version) {
  VDE* vde = vector_data_entry_rand(100, 10);
  data_entry key("rpush_lpop_no_expire_no_version");

  tairClient.remove(10, key);

  int length = 0;
  int resultcode = tairClient.llen(10, key, length);
  ASSERT_EQ(TAIR_RETURN_DATA_NOT_EXIST, resultcode);
  ASSERT_EQ(0, length);

  long successlen = 0;
  resultcode = tairClient.rpush(10, key, *vde, NOT_CARE_EXPIRE, (const int)NOT_CARE_VERSION, successlen);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
  ASSERT_EQ(10, successlen);

  VDE* vde1 = vector_data_entry_rand(10, 8182);
  for (size_t i = 0; i < vde1->size(); i++) {
    data_entry* de = (*vde1)[i];
    resultcode = tairClient.rpush(10, key, de, NOT_CARE_EXPIRE, NOT_CARE_VERSION);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);

    resultcode = tairClient.llen(10, key, length);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    ASSERT_EQ(i + 11, length);
  }

  data_entry* dentry = data_entry_rand(100);
  resultcode = tairClient.rpush(10, key, dentry, NOT_CARE_EXPIRE, NOT_CARE_VERSION);
  ASSERT_EQ(TAIR_RETURN_DATA_LEN_LIMITED, resultcode);
  data_entry_free(dentry);

  resultcode = tairClient.llen(10, key, length);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
  ASSERT_EQ(8192, length);

  std::vector<data_entry *> ret_values;
  resultcode = tairClient.lpop(10, key, 10, ret_values, NOT_CARE_EXPIRE, NOT_CARE_VERSION);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);

  ASSERT_EQ(1, vector_data_entry_cmp(vde, &ret_values));

  for (size_t i = 0; i < vde1->size(); i++) {
    data_entry* value = NULL;
    resultcode = tairClient.lpop(10, key, &value, NOT_CARE_EXPIRE, NOT_CARE_VERSION);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    ASSERT_EQ(1, data_entry_cmp((*vde1)[i], value));
    data_entry_free(value);
  }

  data_entry* value = NULL;
  resultcode = tairClient.lpop(10, key, &value, NOT_CARE_EXPIRE, NOT_CARE_VERSION);
  ASSERT_EQ(TAIR_RETURN_DATA_NOT_EXIST, resultcode);
  ASSERT_EQ(NULL, value);

  vector_data_entry_free(vde);
  vector_data_entry_free(vde1);

  resultcode = tairClient.remove(10, key);
  ASSERT_EQ(TAIR_RETURN_DATA_NOT_EXIST, resultcode);
}

TEST_F(TairClientRDBTest, test_numeric_lindex) {
  VDE* vde = vector_data_entry_numeric_rand(10);
  data_entry key("numeric_lindex_normal");

  tairClient.remove(10, key);

  long successlen = 0;
  int resultcode = tairClient.rpush(10, key, *vde, NOT_CARE_EXPIRE, NOT_CARE_VERSION, successlen);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
  ASSERT_EQ(10, successlen);

  for (size_t i = 0; i < vde->size(); i++) {
    data_entry* de = (*vde)[i];
    data_entry value;
    resultcode = tairClient.lindex(10, key, i, value);
    ASSERT_EQ(resultcode, TAIR_RETURN_SUCCESS);
    ASSERT_EQ(1, data_entry_cmp(de, &value));
  }

  for (size_t i = 0; i < vde->size(); i++) {
    data_entry* de = (*vde)[vde->size() - i - 1];
    data_entry value;
    resultcode = tairClient.lindex(10, key, 0 - i - 1, value);
    ASSERT_EQ(resultcode, TAIR_RETURN_SUCCESS);
    ASSERT_EQ(1, data_entry_cmp(de, &value));
  }

  data_entry value;
  resultcode = tairClient.lindex(10, key, vde->size(), value);
  ASSERT_EQ(TAIR_RETURN_OUT_OF_RANGE, resultcode);
  resultcode = tairClient.lindex(10, key, 0 - vde->size() - 1, value);
  ASSERT_EQ(TAIR_RETURN_OUT_OF_RANGE, resultcode);

  vector_data_entry_free(vde);
}

TEST_F(TairClientRDBTest, test_lindex) {
  VDE* vde = vector_data_entry_rand(100, 10);
  data_entry key("lindex_normal");

  tairClient.remove(10, key);

  long successlen = 0;
  int resultcode = tairClient.rpush(10, key, *vde, NOT_CARE_EXPIRE, NOT_CARE_VERSION, successlen);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
  ASSERT_EQ(10, successlen);

  for (size_t i = 0; i < vde->size(); i++) {
    data_entry* de = (*vde)[i];
    data_entry value;
    resultcode = tairClient.lindex(10, key, i, value);
    ASSERT_EQ(resultcode, TAIR_RETURN_SUCCESS);
    ASSERT_EQ(1, data_entry_cmp(de, &value));
  }

  for (size_t i = 0; i < vde->size(); i++) {
    data_entry* de = (*vde)[vde->size() - i - 1];
    data_entry value;
    resultcode = tairClient.lindex(10, key, 0 - i - 1, value);
    ASSERT_EQ(resultcode, TAIR_RETURN_SUCCESS);
    ASSERT_EQ(1, data_entry_cmp(de, &value));
  }

  data_entry value;
  resultcode = tairClient.lindex(10, key, vde->size(), value);
  ASSERT_EQ(TAIR_RETURN_OUT_OF_RANGE, resultcode);
  resultcode = tairClient.lindex(10, key, 0 - vde->size() - 1, value);
  ASSERT_EQ(TAIR_RETURN_OUT_OF_RANGE, resultcode);

  vector_data_entry_free(vde);
}

TEST_F(TairClientRDBTest, test_numeric_lrange) {
  VDE* vde = vector_data_entry_numeric_rand(10);
  data_entry key("numeric_lindex_normal");

  tairClient.remove(10, key);

  long successlen = 0;
  int resultcode = tairClient.rpush(10, key, *vde, NOT_CARE_EXPIRE, NOT_CARE_VERSION, successlen);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
  ASSERT_EQ(10, successlen);

  for (size_t i = 0; i < vde->size(); i++) {
    std::vector<data_entry *> v;
    resultcode = tairClient.lrange(10, key, i, vde->size() - 1, v);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    ASSERT_EQ(vde->size() - i, v.size());
    for (size_t j = i; j < vde->size(); j++) {
      ASSERT_EQ(1, data_entry_cmp((*vde)[j], v[j - i]));
    }
  }

  std::vector<data_entry *> v;
  resultcode = tairClient.lrange(10, key, -1000, 1000, v);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
  ASSERT_EQ(vde->size(), v.size());
  ASSERT_EQ(1, vector_data_entry_cmp(vde, &v));
  for (size_t i = 0; i < v.size(); i++) {
    data_entry_free(v[i]);
  }

  v.clear();
  resultcode = tairClient.lrange(10, key, 1000, 2000, v);
  ASSERT_EQ(TAIR_RETURN_OUT_OF_RANGE, resultcode);
  ASSERT_EQ(v.size(), 0);

  v.clear();
  resultcode = tairClient.lrange(10, key, -1000, -2000, v);
  ASSERT_EQ(TAIR_RETURN_OUT_OF_RANGE, resultcode);
  ASSERT_EQ(v.size(), 0);

  vector_data_entry_free(vde);
}

TEST_F(TairClientRDBTest, test_lrange) {
  VDE* vde = vector_data_entry_rand(100, 10);
  data_entry key("lindex_normal");

  tairClient.remove(10, key);

  long successlen = 0;
  int resultcode = tairClient.rpush(10, key, *vde, NOT_CARE_EXPIRE, NOT_CARE_VERSION, successlen);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
  ASSERT_EQ(10, successlen);

  for (size_t i = 0; i < vde->size(); i++) {
    std::vector<data_entry *> v;
    resultcode = tairClient.lrange(10, key, i, vde->size() - 1, v);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    ASSERT_EQ(vde->size() - i, v.size());
    for (size_t j = i; j < vde->size(); j++) {
      ASSERT_EQ(1, data_entry_cmp((*vde)[j], v[j - i]));
    }
  }

  std::vector<data_entry *> v;
  resultcode = tairClient.lrange(10, key, -1000, 1000, v);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
  ASSERT_EQ(vde->size(), v.size());
  ASSERT_EQ(1, vector_data_entry_cmp(vde, &v));
  for (size_t i = 0; i < v.size(); i++) {
    data_entry_free(v[i]);
  }

  v.clear();
  resultcode = tairClient.lrange(10, key, 1000, 2000, v);
  ASSERT_EQ(TAIR_RETURN_OUT_OF_RANGE, resultcode);
  ASSERT_EQ(v.size(), 0);

  v.clear();
  resultcode = tairClient.lrange(10, key, -1000, -2000, v);
  ASSERT_EQ(TAIR_RETURN_OUT_OF_RANGE, resultcode);
  ASSERT_EQ(v.size(), 0);

  vector_data_entry_free(vde);
}

TEST_F(TairClientRDBTest, test_numeric_lset) {
  VDE* vde = vector_data_entry_numeric_rand(10);
  VDE* vde2 = vector_data_entry_numeric_rand(10);
  data_entry key("lset_normal");

  tairClient.remove(10, key);

  long successlen = 0;
  int resultcode = tairClient.rpush(10, key, *vde, NOT_CARE_EXPIRE, NOT_CARE_VERSION, successlen);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
  ASSERT_EQ(10, successlen);

  for (size_t i = 0; i < vde->size(); i++) {
    data_entry value;
    resultcode = tairClient.lindex(10, key, i, value);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    ASSERT_EQ(1, data_entry_cmp((*vde)[i], &value));
  }

  for (size_t i = 0; i < vde->size(); i++) {
    resultcode = tairClient.lset(10, key, i, *(*vde2)[i],  NOT_CARE_EXPIRE, NOT_CARE_VERSION);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
  }

  for (size_t i = 0; i < vde->size(); i++) {
    data_entry value;
    resultcode = tairClient.lindex(10, key, i, value);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    ASSERT_EQ(1, data_entry_cmp((*vde2)[i], &value));
  }

  vector_data_entry_free(vde);
  vector_data_entry_free(vde2);
}

TEST_F(TairClientRDBTest, test_lset) {
  VDE* vde = vector_data_entry_rand(100, 10);
  VDE* vde2 = vector_data_entry_rand(100, 10);
  data_entry key("lset_normal");

  tairClient.remove(10, key);

  long successlen = 0;
  int resultcode = tairClient.rpush(10, key, *vde, NOT_CARE_EXPIRE, NOT_CARE_VERSION, successlen);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
  ASSERT_EQ(10, successlen);

  for (size_t i = 0; i < vde->size(); i++) {
    data_entry value;
    resultcode = tairClient.lindex(10, key, i, value);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    ASSERT_EQ(1, data_entry_cmp((*vde)[i], &value));
  }

  for (size_t i = 0; i < vde->size(); i++) {
    resultcode = tairClient.lset(10, key, i, *(*vde2)[i],  NOT_CARE_EXPIRE, NOT_CARE_VERSION);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
  }

  for (size_t i = 0; i < vde->size(); i++) {
    data_entry value;
    resultcode = tairClient.lindex(10, key, i, value);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    ASSERT_EQ(1, data_entry_cmp((*vde2)[i], &value));
  }

  vector_data_entry_free(vde);
  vector_data_entry_free(vde2);
}

TEST_F(TairClientRDBTest, test_numeric_ltrim) {
  VDE* vde = vector_data_entry_numeric_rand(10);
  data_entry key("lset_normal");

  tairClient.remove(10, key);

  long successlen = 0;
  int resultcode = tairClient.rpush(10, key, *vde, NOT_CARE_EXPIRE, NOT_CARE_VERSION, successlen);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
  ASSERT_EQ(10, successlen);

  resultcode = tairClient.ltrim(10, key, 1000, 2000, NOT_CARE_EXPIRE, NOT_CARE_VERSION);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);

  int length = 0;
  resultcode = tairClient.llen(10, key, length);
  ASSERT_EQ(TAIR_RETURN_DATA_NOT_EXIST, resultcode);
  ASSERT_EQ(0, length);

  resultcode = tairClient.rpush(10, key, *vde, NOT_CARE_EXPIRE, NOT_CARE_VERSION, successlen);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
  ASSERT_EQ(10, successlen);

  resultcode = tairClient.ltrim(10, key, -1000, -2000, NOT_CARE_EXPIRE, NOT_CARE_VERSION);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);

  resultcode = tairClient.llen(10, key, length);
  ASSERT_EQ(TAIR_RETURN_DATA_NOT_EXIST, resultcode);
  ASSERT_EQ(0, length);

  resultcode = tairClient.rpush(10, key, *vde, NOT_CARE_EXPIRE, NOT_CARE_VERSION, successlen);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
  ASSERT_EQ(10, successlen);

  resultcode = tairClient.ltrim(10, key, 1, 3, NOT_CARE_EXPIRE, NOT_CARE_VERSION);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);

  resultcode = tairClient.llen(10, key, length);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, 0);
  ASSERT_EQ(3, length);

  resultcode = tairClient.ltrim(10, key, 1, 0, NOT_CARE_EXPIRE, NOT_CARE_VERSION);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);

  resultcode = tairClient.llen(10, key, length);
  ASSERT_EQ(TAIR_RETURN_DATA_NOT_EXIST, resultcode);
  ASSERT_EQ(0, length);

  vector_data_entry_free(vde);
}

TEST_F(TairClientRDBTest, test_ltrim) {
  VDE* vde = vector_data_entry_rand(100, 10);
  data_entry key("lset_normal");

  tairClient.remove(10, key);

  long successlen = 0;
  int resultcode = tairClient.rpush(10, key, *vde, NOT_CARE_EXPIRE, NOT_CARE_VERSION, successlen);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
  ASSERT_EQ(10, successlen);

  resultcode = tairClient.ltrim(10, key, 1000, 2000, NOT_CARE_EXPIRE, NOT_CARE_VERSION);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);

  int length = 0;
  resultcode = tairClient.llen(10, key, length);
  ASSERT_EQ(TAIR_RETURN_DATA_NOT_EXIST, resultcode);
  ASSERT_EQ(0, length);

  resultcode = tairClient.rpush(10, key, *vde, NOT_CARE_EXPIRE, NOT_CARE_VERSION, successlen);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
  ASSERT_EQ(10, successlen);

  resultcode = tairClient.ltrim(10, key, -1000, -2000, NOT_CARE_EXPIRE, NOT_CARE_VERSION);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);

  resultcode = tairClient.llen(10, key, length);
  ASSERT_EQ(TAIR_RETURN_DATA_NOT_EXIST, resultcode);
  ASSERT_EQ(0, length);

  resultcode = tairClient.rpush(10, key, *vde, NOT_CARE_EXPIRE, NOT_CARE_VERSION, successlen);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
  ASSERT_EQ(10, successlen);

  resultcode = tairClient.ltrim(10, key, 1, 3, NOT_CARE_EXPIRE, NOT_CARE_VERSION);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);

  resultcode = tairClient.llen(10, key, length);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, 0);
  ASSERT_EQ(3, length);

  resultcode = tairClient.ltrim(10, key, 1, 0, NOT_CARE_EXPIRE, NOT_CARE_VERSION);
  ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);

  resultcode = tairClient.llen(10, key, length);
  ASSERT_EQ(TAIR_RETURN_DATA_NOT_EXIST, resultcode);
  ASSERT_EQ(0, length);

  vector_data_entry_free(vde);
}

