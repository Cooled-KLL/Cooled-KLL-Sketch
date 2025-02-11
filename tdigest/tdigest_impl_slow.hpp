#pragma once
#include "tdigest.hpp"
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <iomanip>
#include <cassert>
#include "../common/common/vec_ops.hpp"

namespace sketch{
    TDigest::TDigest(u32 cap_, u32 delta_)
        : totalWeight(0), cap(cap_), DELTA(delta_) {}

    u32 TDigest::size() const {
        return totalWeight;
    }

    bool TDigest::empty() const {
        return size() == 0;
    }

    bool TDigest::full() const {
        return max_weight >= cap;
    }

    u32 TDigest::memory() const {
        const u32 counter_bits = std::ceil(std::log2(static_cast<f64>(cap) + 1));
        const u32 centroid_bits = counter_bits + 32;
        return (centroid_bits * DELTA + 7) / 8;
    }

    f64 TDigest::scale(f64 p) const {
        const f64 PI = acos(-1);
        return asin(2 * p - 1) / (2 * PI) * DELTA;
    }

    std::pair<f64, f64> TDigest::qBound(const Centroid& c) const {
        f64 q_left = 0.0, q_right = 0.0;
        for (const auto& centroid : centroids) {
            if (Centroid::addr_equal(centroid, c)) {
                q_right = q_left + centroid.weight();
                break;
            }
            q_left += centroid.weight();
        }
        if (q_right == 0.0) {
            throw std::invalid_argument("centroid not found");
        }
        q_left /= totalWeight;
        q_right /= totalWeight;
        return {q_left, q_right};
    }

    f64 TDigest::kSize(const Centroid& c) const {
        auto [q_left, q_right] = qBound(c);
        return scale(q_right) - scale(q_left);
    }

    void TDigest::append(elem_t item) {
        if (full()) {
            throw std::logic_error("append to a full t-digest");
        }

        if (centroids.size() < DELTA) {
            vec_insert_ordered(centroids, Centroid(item, 1),
                               Centroid::mean_less);
            ++totalWeight;
            min_item = std::min(min_item, item);
            max_item = std::max(max_item, item);
            return;
        }

        Centroid* pos = findAppendPos(item);
        if (pos == nullptr) {
            vec_insert_ordered(centroids, Centroid(item, 1),
                               Centroid::mean_less);
            max_weight = std::max(max_weight, 1u);
        } else {
            pos->append(item);
            max_weight = std::max(max_weight, pos->weight());
        }

        ++totalWeight;
        min_item = std::min(min_item, item);
        max_item = std::max(max_item, item);

        std::sort(centroids.begin(), centroids.end(), Centroid::mean_less);
        if (centroids.size() > DELTA) {
            compressNearest();
        }
    }

    Centroid* TDigest::findAppendPos(elem_t item) {
        Centroid* pos = nullptr;
        f64 min_dist = UINT32_MAX;
        u32 max_weight = 0;

        const f64 eps = 1e-5;
        for (auto& centroid : centroids) {
            f64 dist = std::fabs(centroid.mean() - item);
            if (dist > min_dist + eps) {
                continue;
            }
            if (dist < min_dist - eps) {
                min_dist = dist;
                pos = nullptr;
                max_weight = 0;
            }
            if (appendable(centroid) && centroid.weight() > max_weight) {
                pos = &centroid;
                max_weight = centroid.weight();
            }
        }

        return pos;
    }

    bool TDigest::appendable(const Centroid& c) const {
        Centroid& c_ref = const_cast<Centroid&>(c);
        c_ref.m_weight += 1;
        f64 size = kSize(c_ref);
        c_ref.m_weight -= 1;
        return size <= 1;
    }

    void TDigest::compressNearest() {
        if (centroids.size() <= 1) {
            return;
        }

        f64 min_size = UINT32_MAX;
        auto pos = centroids.begin();
        auto i = pos, j = pos + 1;
        f64 q_left = 0.0, q_right = i->weight();

        while (j != centroids.end()) {
            q_right += j->weight();
            f64 size = scale(q_right / totalWeight)
                     - scale(q_left / totalWeight);
            if (size < min_size) {
                min_size = size;
                pos = i;
            }
            q_left += i->weight();
            ++i, ++j;
        }

        assert(min_size <= 1);
        pos->merge(*(pos + 1));
        centroids.erase(pos + 1);
        max_weight = std::max(max_weight, pos->weight());
    }

    u32 TDigest::quantile(f64 nom_rank) const {
        if (empty()) {
            throw std::logic_error("get quantile on empty t-digest");
        }
        return static_cast<Histogram>(*this).quantile(nom_rank);
    }

    TDigest::operator Histogram() const {
        const auto& c = centroids;
        assert(c.size() > 0);
        vec_f64 split(c.size() + 2, 0);
        vec_u32 height(c.size() + 1, 0);
        
        split.front() = min_item - f64_equal(min_item, c.front().mean());
        for (u32 i = 0; i < c.size(); ++i) {
            split[i + 1] = c[i].mean();
        }
        split.back() = max_item + f64_equal(max_item, c.back().mean());

        height.front() = c.front().weight() / 2;
        for (u32 i = 0; i < c.size(); ++i) {
            height[i + 1] += (c[i].weight() + 1) / 2;
            height[i + 1] += c[i + 1].weight() / 2;
        }
        height.back() = (c.back().weight() + 1) / 2;

        return Histogram(split, height);
    }
}   // namespace sketch
