#pragma once
#include "tiny_counter.hpp"
#include <algorithm>

namespace sketch {
    TinyCnter::TinyCnter()
        : cnt0(0), cnt1(0), cnt2(0), cnt3(0), maxItem(0) { }

    bool TinyCnter::full(u32 idx) const {
        return count(idx) == MAX_CNT;
    }

    bool TinyCnter::empty(u32 idx) const {
        return count(idx) == 0;
    }

    void TinyCnter::append(u32 item, u32 idx) {
        checkIdx(idx);
        if (full(idx)) {
            throw std::logic_error("append to a full tiny counter");
        }
        switch (idx) {
            case 0: ++cnt0; break;
            case 1: ++cnt1; break;
            case 2: ++cnt2; break;
            case 3: ++cnt3; break;
        }
        maxItem = std::max(maxItem, item);
    }

    u32 TinyCnter::count(u32 idx) const {
        checkIdx(idx);
        switch (idx) {
            case 0: return cnt0;
            case 1: return cnt1;
            case 2: return cnt2;
            case 3: return cnt3;
        }

        throw std::runtime_error("shouldn't reach here");
    }

    u32 TinyCnter::value() const {
        return maxItem;
    }

    u32 TinyCnter::memory() const {
        return 5;
    }

    void TinyCnter::checkIdx(u32 idx) const {
        if (idx > MAX_CNT) {
            throw std::invalid_argument("tiny counter index out of range");
        }
    }
}   // namespace sketch