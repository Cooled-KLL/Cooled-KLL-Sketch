#pragma once
#include "../common/common/sketch_defs.hpp"
#include "../common/common/histogram.hpp"

namespace sketch {
        template <typename META>
        class M4;
    class DDSketch {
        friend class M4<DDSketch>;
    public:
        /// @brief Default constructor.
        /// @warning Members are potential uninitialized after construction.
        ///          Make sure you know what you are doing.
        DDSketch() = default;

        /// @brief Constructor.
        /// @param cap_ Capacity, i.e. maximum number of items that
        ///             can be held in the DDSketch.
        /// @param alpha_ Argument for interval division.
        DDSketch(u32 cap_, f64 alpha_, elem_t range);

        /// @brief Destructor.
        ~DDSketch() = default;

        /// @brief Return the number of items in the DDSketch.
        inline u32 size() const;
        /// @brief Return the capacity of the DDSketch.
        inline u32 capacity() const;

        /// @brief Return whether the DDSketch is empty.
        inline bool empty() const;
        /// @brief Return whether the DDSketch is full.
        inline bool full() const;

        /// @brief Return the number of bytes the DDSketch uses.
        inline u32 memory() const;

        /// @brief Append an item to the DDSketch.
        /// @param item The item to append.
        inline void append(elem_t item);

        /// @brief Estimate the quantile value of a given normalized rank.
        inline u32 quantile(f64 nom_rank) const;

        /// @brief Convert the DDSketch to a histogram.
        inline operator Histogram() const;

        void dump_weighted_items(vector<weighted_item> &list);
        void multi_query_find_e_by_w(vector<elem_t> &result, vector<double> &w);
        void multi_query_find_w_by_e(vector<double> &result, vector<elem_t> &e);

    private:
        vec_u32 counters;       ///< Counters.
        u32 totalSize = 0;      ///< Total number of items.
        u32 maxCnt = 0;         ///< Maximum counter.
        u32 cap;                ///< Capacity.
        f64 alpha;              ///< Argument for interval division.
        f64 gamma;              ///< gamma = (1 + alpha) / (1 - alpha).

        /// @brief Return the index of an item in @c counters.
        u32 pos(elem_t item) const;

        /// @brief Append an item to given position.
        void append(elem_t item, u32 pos);
    };
}   // namespace sketch

#include "ddsketch_impl.hpp"
#include "ddsketch_batch_query_impl.hpp"