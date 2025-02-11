#pragma once
#include "histogram.hpp"
#include <algorithm>
#include <cassert>
#include <stdexcept>
#include "vec_ops.hpp"

namespace sketch {
    Histogram operator&(const Histogram& a, const Histogram& b) {
        vec_f64 split_points = vec_union(a.m_splitPoints, b.m_splitPoints);
        return Histogram::minAligned(a.split(split_points), b.split(split_points));
    }

    Histogram operator|(const Histogram& a, const Histogram& b) {
        vec_f64 split_points = vec_union(a.m_splitPoints, b.m_splitPoints);
        return Histogram::sumAligned(a.split(split_points), b.split(split_points));
    }

    Histogram Histogram::split(const vec_f64& split_points) const {
        const auto& s = split_points;
        assert(s.size() > 0);

        Histogram res = {s, vec_u32(s.size() - 1, 0)};

        constexpr f64 eps = 1e-6;
        u32 split_idx = 0;
        for (; s[split_idx] + eps < m_splitPoints.front(); ++split_idx);

        u32 l = 0;
        while (l < m_splitPoints.size() - 1 && split_idx < res.m_heights.size()) {
            f64 p = s[split_idx + 1] - s[split_idx];
            p /= m_splitPoints[l + 1] - m_splitPoints[l];

            assert(split_idx < res.m_heights.size());
            res.m_heights[split_idx] = std::rint(m_heights[l] * p);
            if (res.m_heights[split_idx] == 0 && m_heights[l] > 0) {
                res.m_heights[split_idx] = 1;
            }

            ++split_idx;
            if (m_splitPoints[l + 1] <= s[split_idx] + eps) {
                ++l;
            }
        }

        return res;
    }

    Histogram Histogram::minAligned(const Histogram& h1, const Histogram& h2) {
        Histogram res = h1;
        for (u32 i = 0; i < res.m_heights.size(); ++i) {
            res.m_heights[i] = std::min(res.m_heights[i], h2.m_heights[i]);
        }
        return res;
    }

    Histogram Histogram::sumAligned(const Histogram& h1, const Histogram& h2) {
        Histogram res = h1;
        for (u32 i = 0; i < res.m_heights.size(); ++i) {
            res.m_heights[i] += h2.m_heights[i];
        }
        return res;
    }

    u32 Histogram::quantile(f64 nom_rank) const {
        if (nom_rank < 0.0 || nom_rank > 1.0) {
            throw std::invalid_argument("normalized rank out of range");
        }

        u32 total_height = 0;
        for (u32 h : m_heights) {
            total_height += h;
        }
        if (total_height == 0) {
            return 0;
        }
        u32 rk = nom_rank * total_height;

        u32 l = 0, l_rk = 0;
        for (; l_rk == 0 || l_rk < rk; l_rk += m_heights[l++]);

        u32 r = l;
        u32 r_rk = l_rk;
        l_rk -= m_heights[--l];

        f64 p = static_cast<f64>(rk - l_rk) / (r_rk - l_rk);
        f64 interval = m_splitPoints[r] - m_splitPoints[l];
        return m_splitPoints[l] + p * interval;
    }
}   // namespace sketch