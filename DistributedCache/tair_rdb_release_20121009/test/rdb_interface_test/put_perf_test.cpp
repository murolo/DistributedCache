#include "base_test.hpp"
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <vector>

//global
tair_client_impl tairClient;
int success = 0;
int failed = 0;

long long diff_time(const struct timeval &begin_time, const struct timeval &end_time) {
    return  (end_time.tv_sec - begin_time.tv_sec)*1000000 + end_time.tv_usec - begin_time.tv_usec;
}

void put_data(int index_thread, int task_number) {
    int resultcode;

    MDE* mde = map_data_entry_rand(32, 256, task_number);
    //MDE* mde = map_data_entry_numeric_rand(10);
    int count = 0;
    MDE::iterator iter;
    for(iter = mde->begin(); iter != mde->end(); iter++, count++) {
        if (count % 1000 == 0) {
            log_info("pid:%d index:%d count => %d", pthread_self(),  index_thread, count);
        }
        data_entry* key = iter->first;
        data_entry* value = iter->second;
        //log_info("area:%d key:%s value:%s", i, key->get_data(), value->get_data());
        //resultcode = tairClient.put(i, *key, *value, NOT_CARE_EXPIRE, NOT_CARE_VERSION);
        resultcode = tairClient.put(10, *key, *value, 0, NOT_CARE_VERSION);
        if (0 == resultcode) {
            success ++;
        }
        else {
            log_info("error:%d", resultcode);
            failed ++;
        }
    }
}

int main(int argc, const char *argv[])
{
    if (argc < 3) {
        log_error("Usage %s thread_number task_per_number", argv[0]);
        return EXIT_FAILURE;
    }   
    int thread_number = atoi(argv[1]);
    int task_per_thread = atoi(argv[2]);

    TBSYS_LOGGER.setLogLevel("INFO");
    bool done = tairClient.startup("10.138.87.53", "10.138.87.54", "group_1");


    int rc = tairClient.remove_area(10);
    if (0 != rc) {
        log_error("remove area 10 failed");
        return EXIT_FAILURE;
    }

    struct timeval begin_time;
    struct timeval end_time;
    gettimeofday(&begin_time, NULL);

    std::vector<boost::thread*> thread_vec;
    for (int i = 0; i < thread_number; i++) {
        boost::thread* thrd = new boost::thread(boost::bind(&put_data, i, task_per_thread));
        thread_vec.push_back(thrd);
    }

    std::vector<boost::thread*>::iterator it = thread_vec.begin();
    for(; it != thread_vec.end(); ++it) {
        (*it)->join();
    }

    gettimeofday(&end_time, NULL);
    log_info("used time:%ld success:%d failed:%d\n", diff_time(begin_time, end_time), success, failed);
    return 0;
}
