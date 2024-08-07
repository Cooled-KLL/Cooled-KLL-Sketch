#pragma once
#include "param.h"
#include "../common/common.h"
#include "../common/sketch.h"
#include "SingleKLL.h"

class KLL : public quantile_sketch
{
public:
    int tot_memory_in_bytes;
    SingleKLL *kll_sketch;


    KLL(int _tot_memory_in_bytes)
    :tot_memory_in_bytes(_tot_memory_in_bytes){
        kll_sketch = new SingleKLL(_tot_memory_in_bytes);
    }

    ~KLL() override {
        delete kll_sketch;
    }

    void insert(elem_t e) override {
        kll_sketch->insert(e);
    }

    void multi_query_find_e_by_w(vector<elem_t> &result, vector<double> &w) override {
        result.clear();
        kll_sketch->multi_query_find_e_by_w(result, w);
    }

    void multi_query_find_w_by_e(vector<double> &result, vector<elem_t> &e) override {
        result.clear();
        kll_sketch->multi_query_find_w_by_e(result, e);
    }

    int memory() override {
        return kll_sketch->get_memory_usage();
    }
};