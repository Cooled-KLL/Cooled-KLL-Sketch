#pragma once
#include "mreq_compactor.hpp"
#include <algorithm>
#include "../common/common/vec_ops.hpp"

namespace sketch {
    mReqCmtor::mReqCmtor(u32 lg_w_, u32 cap_)
        : lg_w(lg_w_), cap(cap_) {
        items.reserve(cap);
    }

    bool mReqCmtor::full() const {
        return size() >= cap;
    }

    u32 mReqCmtor::size() const {
        return items.size();
    }

    u32 mReqCmtor::capacity() const {
        return cap;
    }

    u32 mReqCmtor::lgWeight() const {
        return lg_w;
    }

    u32 mReqCmtor::weight() const {
        return 1U << lgWeight();
    }

    u32 mReqCmtor::memory() const {
        return sizeof(elem_t) * capacity();
    }

    elem_t_const_iter mReqCmtor::begin() const {
        return items.begin();
    }

    elem_t_const_iter mReqCmtor::end() const {
        return items.end();
    }

    elem_t_iter mReqCmtor::begin() {
        return items.begin();
    }

    elem_t_iter mReqCmtor::end() {
        return items.end();
    }

    void mReqCmtor::append(elem_t item) {
        if (full()) {
            throw std::logic_error("append to a full compactor");
        }
        vec_insert_ordered(items, item);
    }

    void mReqCmtor::compact(mReqCmtor& next) {
        if (!full()) {
            throw std::logic_error("compact a non-full compactor");
        }

        // output coin, coin+2, coin+4, ... into next compactor
        bool coin = rand_bit();
        for (u32 i = coin; i < size(); i += 2) {
            vec_insert_ordered(next.items, items[i]);
        }

        // clear this compactor
        items.clear();
    }

    u32 mReqCmtor::rank(elem_t item, bool inclusive) const {
        return vec_rank(items, item, inclusive);
    }

    u32 mReqCmtor::weightedRank(elem_t item, bool inclusive) const {
        return rank(item, inclusive) * weight();
    }

    void mReqCmtor::dump_weighted_items(vector<weighted_item> &list){
        for (u32 i = 0; i < items.size(); ++i) {
            list.push_back((weighted_item){
                items[i], (int)weight()
            });
        }
    }
} // namespace sketch