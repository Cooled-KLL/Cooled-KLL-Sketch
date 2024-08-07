#pragma once
#include "sorted_view.hpp"
#include <algorithm>
#include <cmath>
#include "vec_ops.hpp"

namespace sketch {
    SortedView::SortedView(u32 num) : totalWeight(0) {
        view.reserve(num);
    }

    void SortedView::insert(u32_const_iter first, u32_const_iter last,
                             u32 weight) {
        for (auto it = first; it != last; ++it) {
            vec_insert_ordered(view, {*it, weight}, witem::value_less);
        }
    }

    void SortedView::insert(witem_const_iter first, witem_const_iter last) {
        for (auto it = first; it != last; ++it) {
            vec_insert_ordered(view, *it, witem::value_less);
        }
    }

    void SortedView::insert(u32 value, u32 weight) {
        vec_insert_ordered(view, {value, weight}, witem::value_less);
    }

    void SortedView::convertToCumulative() {
        // Merge items with same value.
        for (u32 i = 0; i < view.size(); ++i) {
            while (i + 1 < view.size() && view[i].value == view[i + 1].value) {
                view[i].weight += view[i + 1].weight;
                view.erase(view.begin() + i + 1);
            }
        }

        // Convert to cumulative view.
        totalWeight = 0;
        for (auto& entry : view) {
            totalWeight += entry.weight;
            entry.weight = totalWeight;
        }
    }

    u32 SortedView::rank(u32 item, bool inclusive) const {
        if (view.empty()) {
            throw std::runtime_error("rank on empty view");
        }

        u32 rk = vec_rank(view, {item, 0}, inclusive, witem::value_less);
        return rk == 0 ? 0 : view[rk - 1].weight;
    }

    f64 SortedView::nomRank(u32 item, bool inclusive) const {
        f64 rk = rank(item, inclusive);
        return rk / totalWeight;   // normalize
    }

    u32 SortedView::quantile(f64 nom_rank, bool inclusive) const {
        if (view.empty()) {
            throw std::runtime_error("get quantile on empty view");
        }

        f64 tmp = nom_rank * totalWeight;
        u32 weight = inclusive ? std::ceil(tmp) : tmp;

        u32 rk = vec_rank(view, {0, weight}, !inclusive, witem::weight_less);
        return rk == view.size() ? view.back().value : view[rk].value;
    }

    SortedView::operator sketch::Histogram() const {
        if (view.empty()) {
            throw std::runtime_error("convert an empty view to histogram");
        }

        if (view.size() == 1) {
            vec_f64 split = {(f64)view[0].value - 1, (f64)view[0].value + 1};
            vec_u32 height = {view[0].weight};
            return Histogram(split, height);
        }

        vec_witem vec = view;
        for (u32 i = 1; i < vec.size(); ++i) {
            vec[i].weight -= vec[i - 1].weight;
        }

        vec_f64 split(vec.size(), 0);
        vec_u32 height(vec.size() - 1, 0);
        
        for (u32 i = 0; i < vec.size(); ++i) {
            split[i] = vec[i].value;
        }

        for (u32 i = 0; i + 1 < vec.size(); ++i) {
            height[i] += (vec[i].weight + vec[i + 1].weight + 1) / 2;
        }
        height.front() += (vec.front().weight) / 2;
        height.back() += (vec.back().weight + 1) / 2;

        return Histogram(split, height);
    }
} // namespace sketch