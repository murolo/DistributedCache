#include "base_test.hpp"
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <vector>

//global
int success = 0;
int failed = 0;

long long diff_time(const struct timeval &begin_time, const struct timeval &end_time) {
    return  (end_time.tv_sec - begin_time.tv_sec)*1000000 + end_time.tv_usec - begin_time.tv_usec;
}

void put_data(int index_thread, int task_number) {
    tair_client_impl tairClient;
    bool done = tairClient.startup("10.138.87.53", "10.138.87.54", "group_1");
    int resultcode;

    MMDE* mmde = mmap_data_entry_rand(32, 54, 256, task_number/10, 10);
    int count = 0;
    MMDE::iterator iter;
    for(iter = mmde->begin(); iter != mmde->end(); iter++) {
        if (count % 5000 == 0) {
            log_debug("pid:%d index:%d count => %d", pthread_self(),  index_thread, count);
        }
        char buf[128] = {0};
        data_entry* key = iter->first;
        sprintf(buf, "%s_%d", key->get_data(), count);
        data_entry newkey(buf);
        MDE* mde = iter->second;
        MDE::iterator iter2;
        for(iter2 = mde->begin(); iter2 != mde->end(); iter2++) {
            data_entry* field = iter2->first;
            data_entry* value = iter2->second;
            char buff[128] = {0};
            sprintf(buff, "%s_%d", field->get_data(), count);
            data_entry newfield(buff);
            resultcode = tairClient.hset(10, newkey, newfield, *value, NOT_CARE_EXPIRE, NOT_CARE_VERSION);
            count ++;
            if (0 == resultcode) {
                success ++;
            }
            else {
                failed ++;
                log_error("key:[%s] field:[%s] value:[%s] rc:[%d]", 
                        newkey.get_data(), newfield.get_data(), value->get_data(), resultcode);
            }
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
    tair_client_impl tairClient;
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
    printf("used time:%ld success:%d failed:%d\n", diff_time(begin_time, end_time), success, failed);
    return 0;
}

