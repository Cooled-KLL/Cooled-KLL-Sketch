#pragma once
#include "../common/common.h"
#include "../common/sketch.h"
#include "gk_impl.hpp"

class GK : public quantile_sketch
{
public:
    gk *gk_sketch;


    /*
    GK(double epsilon){
        // int items = tot_memory_in_bytes / sizeof(gk::tuple);
        // double epsilon = 1.0 / (2 * items);
        gk_sketch = new gk(epsilon);
    }
    */
    GK(int tot_memory_in_bytes){
#ifdef USE_CAIDA
        if (tot_memory_in_bytes == 16 * 1024)
            gk_sketch = new gk(0.0012795);
        else if (tot_memory_in_bytes == 32 * 1024)
            gk_sketch = new gk(0.0006418);
        else if (tot_memory_in_bytes == 64 * 1024)
            gk_sketch = new gk(0.0003238);
        else if (tot_memory_in_bytes == 128 * 1024)
            gk_sketch = new gk(0.0001706);
        else if (tot_memory_in_bytes == 256 * 1024)
            gk_sketch = new gk(0.0000863);
        else if (tot_memory_in_bytes == 512 * 1024)
            gk_sketch = new gk(0.0000447);
        else assert(false);
#endif
#ifdef USE_KAGGLE_VOLTAGE
        if (tot_memory_in_bytes == 16 * 1024)
            gk_sketch = new gk(0.0010269);
        else if (tot_memory_in_bytes == 32 * 1024)
            gk_sketch = new gk(0.0005282);
        else if (tot_memory_in_bytes == 64 * 1024)
            gk_sketch = new gk(0.0003626);
        else if (tot_memory_in_bytes == 128 * 1024)
            gk_sketch = new gk(0.0002586);
        else if (tot_memory_in_bytes == 256 * 1024)
            gk_sketch = new gk(0.0001706);
        else if (tot_memory_in_bytes == 512 * 1024)
            gk_sketch = new gk(0.0000977);
        else assert(false);
#endif
#ifdef USE_KAGGLE_PRICE
        if (tot_memory_in_bytes == 16 * 1024)
            gk_sketch = new gk(0.0014896);
        else if (tot_memory_in_bytes == 32 * 1024)
            gk_sketch = new gk(0.0008774);
        else if (tot_memory_in_bytes == 64 * 1024)
            gk_sketch = new gk(0.0005746);
        else if (tot_memory_in_bytes == 128 * 1024)
            gk_sketch = new gk(0.0003522);
        else if (tot_memory_in_bytes == 256 * 1024)
            gk_sketch = new gk(0.0002065);
        else if (tot_memory_in_bytes == 512 * 1024)
            gk_sketch = new gk(0.0001214);
        else assert(false);
#endif
#ifdef USE_KAGGLE_CUSTOM
        if (tot_memory_in_bytes == 16 * 1024)
            gk_sketch = new gk(0.0007373);
        else if (tot_memory_in_bytes == 32 * 1024)
            gk_sketch = new gk(0.0004194);
        else if (tot_memory_in_bytes == 64 * 1024)
            gk_sketch = new gk(0.0002794);
        else if (tot_memory_in_bytes == 128 * 1024)
            gk_sketch = new gk(0.0001715);
        else if (tot_memory_in_bytes == 256 * 1024)
            gk_sketch = new gk(0.0001081);
        else if (tot_memory_in_bytes == 512 * 1024)
            gk_sketch = new gk(0.0000655);
        else assert(false);
#endif
#ifdef USE_KAGGLE_DELAY
        if (tot_memory_in_bytes == 16 * 1024)
            gk_sketch = new gk(0.0039053);
        else if (tot_memory_in_bytes == 32 * 1024)
            gk_sketch = new gk(0.0029723);
        else if (tot_memory_in_bytes == 64 * 1024)
            gk_sketch = new gk(0.0015691);
        else if (tot_memory_in_bytes == 128 * 1024)
            gk_sketch = new gk(0.0007818);
        else if (tot_memory_in_bytes == 256 * 1024)
            gk_sketch = new gk(0.0003759);
        else if (tot_memory_in_bytes == 512 * 1024)
            gk_sketch = new gk(0.0001734);
        else assert(false);
#endif
#ifdef USE_KAGGLE_ECOM
        if (tot_memory_in_bytes == 16 * 1024)
            gk_sketch = new gk(0.0029846);
        else if (tot_memory_in_bytes == 32 * 1024)
            gk_sketch = new gk(0.0019116);
        else if (tot_memory_in_bytes == 64 * 1024)
            gk_sketch = new gk(0.0010591);
        else if (tot_memory_in_bytes == 128 * 1024)
            gk_sketch = new gk(0.0006361);
        else if (tot_memory_in_bytes == 256 * 1024)
            gk_sketch = new gk(0.0003030);
        else if (tot_memory_in_bytes == 512 * 1024)
            gk_sketch = new gk(0.0001516);
        else assert(false);
#endif
    }

    ~GK() override {
        delete gk_sketch;
    }

    void insert(elem_t e) override {
        gk_sketch->insert(e);
    }

    void multi_query_find_e_by_w(vector<elem_t> &result, vector<double> &w) override {
        result.clear();
        gk_sketch->multi_query_find_e_by_w(result, w);
    }

    void multi_query_find_w_by_e(vector<double> &result, vector<elem_t> &e) override {
        result.clear();
        gk_sketch->multi_query_find_w_by_e(result, e);
    }

    int memory() override {
        return gk_sketch->memory();
    }
};