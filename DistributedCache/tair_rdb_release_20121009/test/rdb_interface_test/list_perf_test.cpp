#include "base_test.hpp"
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

tair_client_impl tairClient;
size_t success = 0;
size_t failed = 0;

long long diff_time(const struct timeval &begin_time, const struct timeval &end_time) {
    return  (end_time.tv_sec - begin_time.tv_sec)*1000000 + end_time.tv_usec - begin_time.tv_usec;
}

void push_data(int index_thread, int task_number) {
    char buf[128] = {0};
    sprintf(buf, "numeric_lpush_rpop_llen_no_expire_no_version_%d", task_number);
    data_entry key(buf);
    VDE* vde1 = vector_data_entry_numeric_rand(task_number);
    for (size_t i = 0; i < vde1->size(); i++) {
        data_entry* de = (*vde1)[i];
        int resultcode = tairClient.lpush(10, key, de, NOT_CARE_EXPIRE, (int)NOT_CARE_VERSION);
        if (0 == resultcode) {
            success ++;
        }
        else {
            failed ++;
            log_error("key:[%s] value:[%s] rc:[%d]", key.get_data(), de->get_data(),  resultcode);
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
    if (!done) {
        log_error("start up faield");
        return EXIT_FAILURE;
    }

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
        boost::thread* thrd = new boost::thread(boost::bind(&push_data, i, task_per_thread));
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
