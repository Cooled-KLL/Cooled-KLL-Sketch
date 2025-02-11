#pragma once
#include <unordered_map>
#include "sketch_utils.hpp"

namespace sketch {
    class real_dist {
    public:
        /// @brief Append a given item.
        /// @param id Item ID.
        /// @param value Item value.
        inline void append(u32 id, u32 value);

        /// @brief Return absolute rank of a given item.
        /// @param id Item ID.
        /// @param value Item value.
        /// @param inclusive If the item is included in the rank.
        inline u32 rank(u32 id, u32 value, bool inclusive = true) const;

        /// @brief Return normalized rank of a given item.
        /// @param id Item ID.
        /// @param value Item value.
        /// @param inclusive If the item is included in the rank.
        inline f64 nomRank(u32 id, u32 value, bool inclusive = true) const;

        /// @brief Return the quantile value of a given normalized rank.
        /// @param id Item ID.
        /// @param nom_rank Normalized rank.
        /// @param inclusive If the item is included in the rank.
        inline u32 quantile(u32 id, f64 nom_rank, bool inclusive = true) const;

        /// @brief Return the size of a given flow.
        /// @param id Flow ID.
        inline u32 size(u32 id) const;

        /// @brief Return the type of a given flow.
        /// @param id Flow ID.
        inline FlowType type(u32 id) const;

    private:
        std::unordered_map<u32, vec_u32> container;  ///< Container.
    };
}   // namespace sketch

#include "real_dist_impl.hpp"