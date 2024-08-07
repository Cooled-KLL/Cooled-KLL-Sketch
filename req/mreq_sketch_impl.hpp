#pragma once
#include "mreq_sketch.hpp"
#include <cmath>
#include <cassert>

namespace sketch {
    mReqSketch::mReqSketch(u32 sketch_cap_, u32 cmtor_cap_)
        : itemNum(0), sketchCap(sketch_cap_) {
        // to satisfy the requirement that
        // cmtor_cap_ * (1 + 2 + ... + 2 ^ (cmtor_num - 1)) >= sketch_cap
        u32 cmtor_num = std::ceil(
            std::log2(static_cast<f64>(sketchCap) / cmtor_cap_ + 1));

        cmtors.reserve(cmtor_num);
        for (u32 i = 0; i < cmtor_num; ++i) {
            cmtors.emplace_back(i, cmtor_cap_);
        }
    }

    u32 mReqSketch::size() const {
        return itemNum;
    }

    bool mReqSketch::empty() const {
        return size() == 0;
    }

    bool mReqSketch::full() const {
        return size() >= sketchCap;
    }

    u32 mReqSketch::memory() const {
        return cmtors.capacity() * cmtors.front().memory();
    }

    void mReqSketch::append(elem_t item) {
        if (full()) {
            throw std::logic_error("append to a full mreq sketch");
        }

        // append to the first compactor
        ++itemNum;
        minItem = std::min(minItem, item);
        maxItem = std::max(maxItem, item);
        cmtors.front().append(item);

        // compact if needed
        for (u32 i = 0; cmtors[i].full(); ++i) {
            assert (i + 1 < cmtors.size());
            cmtors[i].compact(cmtors[i + 1]);
        }
    }

    u32 mReqSketch::rank(u32 item, bool inclusive) const {
        if (empty()) {
            throw std::runtime_error("rank on empty mreq sketch");
        }

        u32 rank = 0;
        // sum up ranks of all compactors
        for (const auto& cmtor : cmtors) {
            rank += cmtor.weightedRank(item, inclusive);
        }
        return rank;
    }

    f64 mReqSketch::nomRank(u32 item, bool inclusive) const {
        if (empty()) {
            throw std::runtime_error("rank on empty mreq sketch");
        }
        f64 rk = rank(item, inclusive);
        return rk / size(); // normalize
    }

    u32 mReqSketch::quantile(f64 nom_rank, bool inclusive) const {
        if (empty()) {
            throw std::runtime_error("get quantile on empty mreq sketch");
        }
        if (nom_rank < 0.0 || nom_rank > 1.0) {
            cerr << "normalized rank: " << nom_rank << endl;
            throw std::invalid_argument("normalized rank out of range");
        }

        auto view = setupSortedView();
        return view.quantile(nom_rank, inclusive);
    }

    SortedView mReqSketch::setupSortedView() const {
        auto view = SortedView(size());

        for (const auto& cmtor : cmtors) {
            assert(false); // not implemented
            // view.insert(cmtor.begin(), cmtor.end(), cmtor.weight());
        }
        view.insert(minItem, 0);
        view.insert(maxItem, 0);

        view.convertToCumulative();

        return view;
    }

    mReqSketch::operator sketch::Histogram() const {
        if (empty()) {
            throw std::runtime_error("convert an empty mreq sketch to histogram");
        }

        auto view = setupSortedView();
        return static_cast<sketch::Histogram>(view);
    }
}  // namespace sketch