#include "base_test.hpp"

// zadd
// zrem
// zcard
// zrange
// zrangbyscore
// zremrangebyscore
// zremrangebyrank

#define test_swap(a, b, T) do {\
  T t = a;  \
  a   = b;  \
  b   = t;  \
}while(0)

static void sort_vde_by_dvect(VDE* vde, DVECT* dvect) {
  for (size_t i = 0; i < vde->size(); i++) {
    for (size_t j = i + 1; j < vde->size(); j++) {
      if ((*dvect)[j] < (*dvect)[i]) {
        test_swap((*vde)[i], (*vde)[j], data_entry*);
        test_swap((*dvect)[i], (*dvect)[j], double);
      }
    }
  }
}

static void test_simple_zadd(tair_client_impl &tairClient, data_entry &key, VDE* vde, DVECT *dvect) {
  tairClient.remove(10, key);

  for (size_t i = 0; i < vde->size(); i++) {
    int resultcode = tairClient.zadd(10, key, G(G(vde)[i]), G(dvect)[i], NOT_CARE_EXPIRE, (const int)NOT_CARE_VERSION);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    long long retnum = 0;
    resultcode = tairClient.zcard(10, key, &retnum);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    ASSERT_EQ(i + 1, retnum);
  }

  sort_vde_by_dvect(vde, dvect);


  {
    std::vector<std::pair<data_entry*, double> > valuescores;
    int resultcode = tairClient.zrangebyscore(10, key, (*dvect)[0], (*dvect)[dvect->size() - 1], true, 50000 , true, valuescores);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    ASSERT_EQ(50000, valuescores.size());

    for (size_t i = 0; i < valuescores.size(); i++) {
      ASSERT_EQ(1, data_entry_cmp((*vde)[i], valuescores[i].first));
      ASSERT_EQ((*dvect)[i], valuescores[i].second);
    }

    for (size_t i = 0; i < valuescores.size(); i++) {
      delete valuescores[i].first;
    }
  }

  tairClient.remove(10, key);
}


TEST_F(TairClientRDBTest, test_simple_zadd) {
  VDE* vde = vector_data_entry_numeric_rand(50000);
  DVECT* dvect = vector_double_rand(50000);

  data_entry key("simple_zadd_case");

  test_simple_zadd(tairClient, key, vde, dvect);

  vector_data_entry_free(vde, true);
  delete dvect;
}

