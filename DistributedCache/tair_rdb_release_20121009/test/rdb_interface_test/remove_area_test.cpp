#include "base_test.hpp"

//this file test hash's all interface common test

TEST_F(TairClientRDBTest, remove_area_test) {
    int resultcode;
    MDE* mde = map_data_entry_numeric_rand(10, 10);
    MDE::iterator iter;
    for(iter = mde->begin(); iter != mde->end(); iter++) {
        data_entry* key = iter->first;
        data_entry* value = iter->second;
        printf("key:%s value:%s\n", key->get_data(), value->get_data());
        resultcode = tairClient.put(11, *key, *value, 0, NOT_CARE_VERSION);
        ASSERT_EQ(resultcode, TAIR_RETURN_SUCCESS);

        data_entry* ret_val = new data_entry();
        resultcode = tairClient.get(11, *key, ret_val);
        printf("key:%s ret_val:%s\n", key->get_data(), ret_val->get_data());
        ASSERT_EQ(resultcode, TAIR_RETURN_SUCCESS);
        if (data_entry_cmp(value, ret_val)) {
            ASSERT_TRUE(1);
        } else {
            ASSERT_TRUE(0);
        }

        data_entry_free(ret_val);
    }


    resultcode = tairClient.remove_area(11, 1);
    ASSERT_EQ(resultcode, TAIR_RETURN_SUCCESS);

    printf("=========> remove area once \n");


    /*
    for(iter = mde->begin(); iter != mde->end(); iter++) {
        data_entry* key = iter->first;
        data_entry* value = iter->second;
        data_entry* ret_val = new data_entry();
        resultcode = tairClient.get(11, *key, ret_val);

        printf("key:%s value:%s\n", key->get_data(), value->get_data());
        printf("key:%s ret_val:%s\n", key->get_data(), ret_val->get_data());
        ASSERT_EQ(resultcode, TAIR_RETURN_SUCCESS);
        if (data_entry_cmp(value, ret_val)) {
            ASSERT_TRUE(1);
        } else {
            ASSERT_TRUE(0);
        }

        data_entry_free(ret_val);
    }
    */

    sleep(5);

    resultcode = tairClient.remove_area(11, 1);
    ASSERT_EQ(resultcode, TAIR_RETURN_SUCCESS);

}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

