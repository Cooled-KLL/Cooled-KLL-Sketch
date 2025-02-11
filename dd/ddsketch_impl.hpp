#pragma once
#include "ddsketch.hpp"
#include <stdexcept>
#include <cmath>
#include <algorithm>

namespace sketch {
    #ifdef DICT_COLLAPSE
    DDSketch::DDSketch(u32 cap_, f64 alpha_, elem_t range, u32 counter_number)
    #else
    DDSketch::DDSketch(u32 cap_, f64 alpha_, elem_t range = 1e9)
    #endif
        : cap(cap_), alpha(alpha_),
          gamma((1.0 + alpha) / (1.0 - alpha)) {
        if (alpha <= 0.0 || alpha >= 1.0) {
            throw std::invalid_argument("alpha must be in (0, 1)");
        }

        u32 num = std::ceil(std::log2(range) / std::log2(gamma)) + 1;
    #ifdef DICT_COLLAPSE
        // counter_dict = dict();
        counter_limit = counter_number;
    #else
        counters = vec_u32(num, 0);
    #endif
    }

    u32 DDSketch::size() const {
        return totalSize;
    }

    u32 DDSketch::capacity() const {
        return cap;
    }

    bool DDSketch::empty() const {
        return size() == 0;
    }

    bool DDSketch::full() const {
        return maxCnt >= cap;
    }

    u32 DDSketch::memory() const {
    #ifdef DICT_COLLAPSE
        return counter_dict.size();
    #else
        // u32 counter_bits = std::ceil(std::log2(static_cast<f64>(cap) + 1));
        u32 counter_bits = 32;
        return (counter_bits * counters.size() + 7) / 8;
    #endif
    }

    u32 DDSketch::pos(elem_t item) const {
        return std::ceil(std::log2(item) / std::log2(gamma));
    }

    void DDSketch::append(elem_t item) {
        u32 idx = pos(item);
    #ifdef DICT_COLLAPSE
        // do nothing
    #else
        if (counters[idx] >= cap) {
            throw std::runtime_error("append to a full DDSketch");
        }
    #endif
        append(item, idx);
    }

    void DDSketch::append(elem_t item, u32 pos) {
    #ifdef DICT_COLLAPSE
        counter_dict.insert(pos);
        assert(counter_limit > 2);
        while (counter_dict.total_items() > counter_limit)
            counter_dict.mergeMinToSecondMin();
    #else
        ++counters[pos];
    #endif
        ++totalSize;
    #ifdef DICT_COLLAPSE
        maxCnt = std::max(maxCnt, counter_dict.count(pos));
    #else
        maxCnt = std::max(maxCnt, counters[pos]);
    #endif
    }

    u32 DDSketch::quantile(f64 nom_rank) const {
    #ifdef DICT_COLLAPSE
        if (nom_rank < 0.0 || nom_rank > 1.0) {
            throw std::invalid_argument("normalized rank out of range");
        }

        u32 rank = nom_rank * (totalSize - 1);
        u32 idx = 0;

        // for (u32 sum = counters[0]; sum <= rank; sum += counters[++idx]);
        auto iter = counter_dict.begin();
        for (u32 sum = iter->second; sum <= rank; sum += (++iter)->second);

        f64 res = idx == 0 ? 1 : 2 * std::pow(gamma, idx) / (gamma + 1);
        return std::lrint(res);
    #else
        if (nom_rank < 0.0 || nom_rank > 1.0) {
            throw std::invalid_argument("normalized rank out of range");
        }

        u32 rank = nom_rank * (totalSize - 1);
        u32 idx = 0;

        for (u32 sum = counters[0]; sum <= rank; sum += counters[++idx]);

        f64 res = idx == 0 ? 1 : 2 * std::pow(gamma, idx) / (gamma + 1);
        return std::lrint(res);
    #endif
    }

    DDSketch::operator Histogram() const {
    #ifdef DICT_COLLAPSE
        // u32 sz = counters.size();
    #else
        u32 sz = counters.size();
        vec_f64 split = vec_f64(sz + 1, 0);
        vec_u32 height = vec_u32(sz, 0);

        for (u32 i = 0; i < counters.size(); ++i) {
            split[i + 1] = std::pow(gamma, i);
            height[i] = counters[i];
        }
        return Histogram(split, height);
    #endif
    }

    
    UDDSketch::UDDSketch(u32 cap_, f64 alpha_, elem_t range, u32 counter_number)
    :DDSketch(cap_, alpha_, range, counter_number){
        // nothing
    }
    
    void UDDSketch::append(elem_t item, u32 pos) {
        // printf("override OK\n");
        counter_dict.insert(pos);
        assert(counter_limit > 2);
        while (counter_dict.total_items() > counter_limit){
            counter_dict.uniformCollapse();
            double new_alpha = (2 * alpha) / (1 + alpha * alpha);
            double new_gamma = (1.0 + new_alpha) / (1.0 - new_alpha);
            alpha = new_alpha;
            gamma = new_gamma;
        }
        ++totalSize;
        maxCnt = std::max(maxCnt, counter_dict.count(pos));
    }
}   // namespace sketch
