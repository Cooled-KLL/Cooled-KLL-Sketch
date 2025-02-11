// #define DEBUG_FLAG
#include <bits/stdc++.h>
#include "Mmap.h"
#include <time.h>

#include "common/common.h"
#include "common/sketch.h"
#include "elastickll/ElasticKLL.cpp"
#include "req/ReqSketch.cpp"
#include "tdigest/Tdigest.cpp"
#include "kll/KLL.cpp"
#include "dd/DD.cpp"
#include "gk/GK.cpp"

using namespace std;

int main(){
    int sketch_memory;
    vector<quantile_sketch*> sketch_list;
    for (sketch_memory = 16 * 1024; sketch_memory <= 512 * 1024; sketch_memory *= 2){
        auto current_sketch = new ElasticKLL(sketch_memory);
        cout << sketch_memory / 1024 
            << ",\t" << config.c
            << ",\t" << config.top_level_length 
            << ",\t" << current_sketch->sketch_list[0]->get_max_possible_level()
            << ",\t" << current_sketch->sketch_list[0]->get_max_array_length_total()
            << ",\t" << current_sketch->sketch_list[0]->get_bucket_number() << endl;
    }
    return 0;
}