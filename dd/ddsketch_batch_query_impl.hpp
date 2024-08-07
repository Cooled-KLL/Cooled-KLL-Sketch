#pragma once
#include "../common/common.h"
#include "ddsketch.hpp"
#include <cmath>
#include <cassert>

namespace sketch {
    void DDSketch::dump_weighted_items(vector<weighted_item> &list){
        for (u32 i = 0; i < counters.size(); ++i) {
            list.push_back((weighted_item){
                (elem_t)lrint(i == 0 ? 1 : 2 * std::pow(gamma, i) / (gamma + 1)), (int)counters[i]
            });
        }
    }

    void DDSketch::multi_query_find_e_by_w(vector<elem_t> &result, vector<double> &w){
        vector<weighted_item> all_items;
        all_items.clear();
        dump_weighted_items(all_items);
        sort(all_items.begin(), all_items.end());
        int total_insert = DDSketch::size();

        for (int i = 0, j = 0, current_rank = 0; i < w.size(); i++){
            int target_rank = (total_insert - 1) * w[i];
            while ((j != all_items.size() - 1) && (target_rank > current_rank + all_items[j].w)){
                current_rank += all_items[j].w;
                j++;
            }
            result.push_back(all_items[j].e);
        }
    }

    void DDSketch::multi_query_find_w_by_e(vector<double> &result, vector<elem_t> &e){
        vector<weighted_item> all_items;
        all_items.clear();
        dump_weighted_items(all_items);
        sort(all_items.begin(), all_items.end());
        int total_insert = DDSketch::size();

        for (int i = 0, j = 0, current_rank = 0; i < e.size(); i++){
            while ((j != all_items.size()) && (e[i] >= all_items[j].e)){
                current_rank += all_items[j].w;
                j++;
            }
            result.push_back((double)current_rank / total_insert);
        }
    }
}  // namespace sketch