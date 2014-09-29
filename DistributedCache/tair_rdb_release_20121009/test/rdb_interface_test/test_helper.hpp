#ifndef __TEST_HELPER__
#define __TEST_HELPER__

#include <vector>
#include <map>

#include "data_entry.hpp"

using namespace std;
using namespace tair::common;

#define G(x)    (*(x))
#define g(x)    (&(x))

typedef vector<double> DVECT;
typedef vector<data_entry *> VDE;
typedef map<data_entry*, vector<data_entry *>* > MVDE;
typedef map<data_entry*, data_entry*> MDE;
typedef map<data_entry*, map<data_entry*, data_entry*>* > MMDE;

int staticData(char** buffer, int size);
int staticNumericData(char** buffer);
int randData(char** buffer, int size);
int randNumericData(char** buffer);

DVECT* vector_double_rand(int size, bool isrand = true);
data_entry* data_entry_rand(int size, int isrand = 1);
data_entry* data_entry_numeric_rand(bool isrand = true);
VDE* vector_data_entry_rand(int len, int size, int isrand = 1);
VDE* vector_data_entry_numeric_rand(int size, bool isrand = true);
MDE* map_data_entry_rand(int k_len, int v_len, int size, int isrand = 1);
MDE* map_data_entry_numeric_rand(int size, bool isrand = true);
MVDE* mapv_data_entry_rand(int k_len, int f_len, int size, int ssize, int isrand = 1);
MVDE* mapv_data_entry_numeric_rand(int size, int ssize, bool isrand = true);
MMDE* mmap_data_entry_rand(int k_len, int f_len, int v_len,
        int size, int ssize, int isrand = 1);
MMDE* mmap_data_entry_numeric_rand(int size, int ssize, bool isrand = true);

MDE* map_get_n_data_entry(MDE* mde, int n);
VDE* vector_get_n_data_entry(VDE* vde, int n);
VDE* vector_key_from_map(MDE* mde);
VDE* vector_value_from_map(MDE* mde);

void data_entry_free(data_entry* de, int isfree = 0);
void vector_data_entry_free(VDE* vde, int isfree = 0);
void vector_double_free(DVECT* dvect);
void map_data_entry_free(MDE* mde, int isfree = 0);
void mapv_data_entry_free(MVDE* mvde, int isfree = 0);
void mmap_data_entry_free(MMDE* mmde, int isfree = 0);

int data_entry_cmp(data_entry* de1, data_entry* de2);
int vector_data_entry_cmp(VDE* vde1, VDE* vde2);
int map_data_entry_cmp(MDE* mde1, MDE* mde2);
int mmap_data_entry_cmp(MMDE* mmde1, MMDE* mmde2);
int mapv_data_entry_cmp(MVDE* mvde1, MVDE* mvde2);
int set_data_entry_cmp(VDE* vde1, VDE* vde2);

void print_hex_string(char* buffer, int size);

#endif
