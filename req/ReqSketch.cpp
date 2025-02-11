#pragma once
#include "../common/common.h"
#include "../common/sketch.h"
#include "mreq_sketch.hpp"

class ReqSketch : public quantile_sketch
{
public:
    int tot_memory_in_bytes;
    sketch::mReqSketch *req_sketch;


    ReqSketch(int _tot_memory_in_bytes)
    :tot_memory_in_bytes(_tot_memory_in_bytes){
        int sketch_cap_ = TOTAL_ELEMENTS * 1.5;
        int item_number = tot_memory_in_bytes / sizeof(elem_t);
        int layer_number = 1;
        while (1){
            int cmtor_cap_ = item_number / layer_number;
            if (cmtor_cap_ * ((1 << layer_number) - 1) > sketch_cap_) break;
            ++layer_number;
        }
        req_sketch = new sketch::mReqSketch(sketch_cap_, item_number / layer_number);
    }

    ~ReqSketch() override {
        delete req_sketch;
    }

    void insert(elem_t e) override {
        req_sketch->append(e);
    }

    void multi_query_find_e_by_w(vector<elem_t> &result, vector<double> &w) override {
        result.clear();
        req_sketch->multi_query_find_e_by_w(result, w);
    }

    void multi_query_find_w_by_e(vector<double> &result, vector<elem_t> &e) override {
        result.clear();
        req_sketch->multi_query_find_w_by_e(result, e);
    }

    int memory() override {
        return req_sketch->memory();
    }
};