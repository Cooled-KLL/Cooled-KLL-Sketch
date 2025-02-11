#pragma once
#include "common.h"

class quantile_sketch {
public:
    quantile_sketch(){}

    virtual ~quantile_sketch(){}

    virtual void insert(elem_t e){}

    virtual void multi_query_find_e_by_w(vector<elem_t> &result, vector<double> &w){}

    virtual void multi_query_find_w_by_e(vector<double> &result, vector<elem_t> &e){}

    virtual int memory(){}

    virtual void debug_aee(vector<elem_t> &groundtruth){ printf("debug_aee undefine\n"); }
};