#include "real_dist.hpp"
#include <numeric>
#include <cmath>
#include <algorithm>
#include "vec_ops.hpp"

namespace sketch {
    void real_dist::append(u32 id, u32 value) {
        container[id].push_back(value);
    }

    u32 real_dist::rank(u32 id, u32 value, bool inclusive) const {
        return vec_rank(container.at(id), value, inclusive);
    }

    f64 real_dist::nomRank(u32 id, u32 value, bool inclusive) const {
        return rank(id, value, inclusive) / static_cast<f64>(size(id));
    }

    u32 real_dist::quantile(u32 id, f64 nom_rank, bool inclusive) const {
        if (nom_rank < 0.0 || nom_rank > 1.0) {
            throw std::invalid_argument("normalized rank out of range");
        }

        u64 tmp = nom_rank * size(id) - inclusive;
        u32 idx = inclusive ? std::ceil(tmp) : tmp;

        auto& vec = const_cast<real_dist*>(this)->container.at(id);
        std::nth_element(vec.begin(), vec.begin() + idx, vec.end());
        return vec[idx];
    }

    u32 real_dist::size(u32 id) const {
        return container.at(id).size();
    }

    FlowType real_dist::type(u32 id) const {
        u32 sz = size(id);
        if (sz <= 3) return TINY;
        else if (sz <= 255) return MID;
        else return HUGE;
    }
}   // namespace sketch