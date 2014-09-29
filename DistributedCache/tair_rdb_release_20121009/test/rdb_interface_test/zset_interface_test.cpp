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
    // get one value
    std::vector<std::pair<data_entry*, double> > valuescores;
    int resultcode = tairClient.zrange(10, key, 0, 0, true, valuescores);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    ASSERT_EQ(1, valuescores.size());

    for (size_t i = 0; i < valuescores.size(); i++) {
      ASSERT_EQ(1, data_entry_cmp((*vde)[i], valuescores[i].first));
      ASSERT_EQ((*dvect)[i], valuescores[i].second);
    }

    for (size_t i = 0; i < valuescores.size(); i++) {
      delete valuescores[i].first;
    }
  }

  {
    std::vector<std::pair<data_entry*, double> > valuescores;
    int resultcode = tairClient.zrange(10, key, 0, -1, true, valuescores);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    ASSERT_EQ(vde->size(), valuescores.size());

    for (size_t i = 0; i < valuescores.size(); i++) {
      ASSERT_EQ(1, data_entry_cmp((*vde)[i], valuescores[i].first));
      ASSERT_EQ((*dvect)[i], valuescores[i].second);
    }

    for (size_t i = 0; i < valuescores.size(); i++) {
      delete valuescores[i].first;
    }
  }

  {
    // get one value
    std::vector<std::pair<data_entry*, double> > valuescores;
    int resultcode = tairClient.zrange(10, key, 0, 0, false, valuescores);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    ASSERT_EQ(1, valuescores.size());

    for (size_t i = 0; i < valuescores.size(); i++) {
      ASSERT_EQ(1, data_entry_cmp((*vde)[i], valuescores[i].first));
    }

    for (size_t i = 0; i < valuescores.size(); i++) {
      delete valuescores[i].first;
    }
  }

  {
    std::vector<std::pair<data_entry*, double> > valuescores;
    int resultcode = tairClient.zrange(10, key, 0, -1, false, valuescores);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    ASSERT_EQ(vde->size(), valuescores.size());

    for (size_t i = 0; i < vde->size(); i++) {
      ASSERT_EQ(1, data_entry_cmp((*vde)[i], valuescores[i].first));
    }

    for (size_t i = 0; i < valuescores.size(); i++) {
      delete valuescores[i].first;
    }
  }

  {
    std::vector<std::pair<data_entry*, double> > valuescores;
    int resultcode = tairClient.zrangebyscore(10, key, (*dvect)[0], (*dvect)[0], true, 10, true, valuescores);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    ASSERT_EQ(1, valuescores.size());

    for (size_t i = 0; i < valuescores.size(); i++) {
      ASSERT_EQ(1, data_entry_cmp((*vde)[i], valuescores[i].first));
      ASSERT_EQ((*dvect)[i], valuescores[i].second);
    }

    for (size_t i = 0; i < valuescores.size(); i++) {
      delete valuescores[i].first;
    }
  }

  {
    std::vector<std::pair<data_entry*, double> > valuescores;
    int resultcode = tairClient.zrangebyscore(10, key, (*dvect)[0], (*dvect)[dvect->size() - 1], true, 10, true, valuescores);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    ASSERT_EQ(10, valuescores.size());

    for (size_t i = 0; i < valuescores.size(); i++) {
      ASSERT_EQ(1, data_entry_cmp((*vde)[i], valuescores[i].first));
      ASSERT_EQ((*dvect)[i], valuescores[i].second);
    }

    for (size_t i = 0; i < valuescores.size(); i++) {
      delete valuescores[i].first;
    }
  }

  {
    // limit
    std::vector<std::pair<data_entry*, double> > valuescores;
    int resultcode = tairClient.zrangebyscore(10, key, G(dvect)[0], G(dvect)[dvect->size() - 1], true, 2, true, valuescores);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    ASSERT_EQ(2, valuescores.size());

    for (size_t i = 0; i < valuescores.size(); i++) {
      ASSERT_EQ(1, data_entry_cmp(G(vde)[i], valuescores[i].first));
      ASSERT_EQ(G(dvect)[i], valuescores[i].second);
    }

    for (size_t i = 0; i < valuescores.size(); i++) {
      delete valuescores[i].first;
    }
  }

  {
    // delete G(vde)[5]
    int resultcode = tairClient.zrem(10, key, G(G(vde)[5]), NOT_CARE_EXPIRE, (const int)NOT_CARE_VERSION);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    data_entry_free(*(vde->begin() + 5), true);
    vde->erase(vde->begin() + 5);
    dvect->erase(dvect->begin() + 5);
  }

  {
    // check delete G(vde)[5] success
    std::vector<std::pair<data_entry*, double> > valuescores;
    int resultcode = tairClient.zrange(10, key, 0, -1, true, valuescores);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    ASSERT_EQ(vde->size(), valuescores.size());

    for (size_t i = 0; i < valuescores.size(); i++) {
      ASSERT_EQ(1, data_entry_cmp((*vde)[i], valuescores[i].first));
      ASSERT_EQ((*dvect)[i], valuescores[i].second);
    }

    for (size_t i = 0; i < valuescores.size(); i++) {
      delete valuescores[i].first;
    }
  }

  {
    // delete range  2 3 4
    long long retnum = 0;
    std::vector<std::pair<data_entry*, double> > valuescores;
    int resultcode = tairClient.zremrangebyrank(10, key, 2, 4, NOT_CARE_EXPIRE, NOT_CARE_VERSION, &retnum);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    ASSERT_EQ(3, retnum);

    data_entry_free(*(vde->begin() + 2), true);
    data_entry_free(*(vde->begin() + 3), true);
    data_entry_free(*(vde->begin() + 4), true);
    vde->erase(vde->begin() + 2, vde->begin() + 5);
    dvect->erase(dvect->begin() + 2, dvect->begin() + 5);
  }

  {
    // check delete range 2 3 4 success
    std::vector<std::pair<data_entry*, double> > valuescores;
    int resultcode = tairClient.zrange(10, key, 0, -1, true, valuescores);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    ASSERT_EQ(vde->size(), valuescores.size());

    for (size_t i = 0; i < valuescores.size(); i++) {
      ASSERT_EQ(1, data_entry_cmp((*vde)[i], valuescores[i].first));
      ASSERT_EQ((*dvect)[i], valuescores[i].second);
    }

    for (size_t i = 0; i < valuescores.size(); i++) {
      delete valuescores[i].first;
    }
  }

  {
    long long retnum = 0;
    std::vector<std::pair<data_entry*, double> > valuescores;
    int resultcode = tairClient.zremrangebyscore(10, key, G(dvect)[2], G(dvect)[4], NOT_CARE_EXPIRE, NOT_CARE_VERSION, &retnum);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    ASSERT_EQ(3, retnum);

    data_entry_free(*(vde->begin() + 2), true);
    data_entry_free(*(vde->begin() + 3), true);
    data_entry_free(*(vde->begin() + 4), true);
    vde->erase(vde->begin() + 2, vde->begin() + 5);
    dvect->erase(dvect->begin() + 2, dvect->begin() + 5);
  }

  {
    // check delete range 2 3 4 success
    std::vector<std::pair<data_entry*, double> > valuescores;
    int resultcode = tairClient.zrange(10, key, 0, -1, true, valuescores);
    ASSERT_EQ(TAIR_RETURN_SUCCESS, resultcode);
    ASSERT_EQ(vde->size(), valuescores.size());

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
  VDE* vde = vector_data_entry_numeric_rand(10);
  DVECT* dvect = vector_double_rand(10);

  data_entry key("simple_zadd_case");

  test_simple_zadd(tairClient, key, vde, dvect);

  vector_data_entry_free(vde, true);
  delete dvect;
}

