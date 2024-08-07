#pragma once
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cassert>

#include "config.h"

using namespace std;

#if defined(USE_CAIDA) || defined(USE_KAGGLE_CUSTOM)
typedef uint32_t elem_t;
#elif defined(USE_KAGGLE_VOLTAGE) || defined(USE_KAGGLE_PRICE) || defined(USE_KAGGLE_DELAY) || defined(USE_KAGGLE_ECOM)
typedef double elem_t;
#endif


typedef vector<elem_t> vec_elem_t;
using elem_t_const_iter = vec_elem_t::const_iterator;
using elem_t_iter = vec_elem_t::iterator;

#define TOTAL_ELEMENTS 30000000

struct weighted_item{
    elem_t e;
    int w;
    bool operator <(weighted_item &t) const {
        return e < t.e;
    }
};