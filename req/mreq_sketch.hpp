#pragma once
#include "../common/common.h"
#include "mreq_compactor.hpp"
#include "../common/common/sorted_view.hpp"
#include "../common/common/histogram.hpp"

namespace sketch {
    class mReqSketch {
        using vec_cmtor = vector<mReqCmtor>;
    public:
        /// @brief Constructor.
        /// @param sketch_cap_ Capacity of the sketch. 
        /// @param cmtor_cap_ Capacity of each compactor.
        mReqSketch(u32 sketch_cap_, u32 cmtor_cap_);

        /// @brief Destructor.
        ~mReqSketch() = default;

        /// @brief Default constructor.
        /// @warning Members are potential uninitialized after construction.
        ///          Make sure you know what you are doing.
        mReqSketch() = default;
        
        /// @brief Return size of the sketch.
        inline u32 size() const;
        /// @brief Return if the sketch is empty.
        inline bool empty() const;
        /// @brief Return if the sketch is full.
        inline bool full() const;
        /// @brief Return number of bytes the sketch uses.
        inline u32 memory() const;

        /// @brief Append a given item into the sketch.
        /// @param item Appended item.
        inline void append(elem_t item);

        /// @brief Estimate absolute rank of a given item.
        /// @param item Item to be ranked.
        /// @param inclusive If the item is included in the rank.
        inline u32 rank(u32 item, bool inclusive) const;
        
        /// @brief Estimate normalized rank of a given item.
        /// @param item Item to be ranked.
        /// @param inclusive If the item is included in the rank.
        inline f64 nomRank(u32 item, bool inclusive) const;

        /// @brief Estimate the quantile value of a given normalized rank.
        /// @param nom_rank Normalized rank.
        /// @param inclusive If the given rank is included.
        inline u32 quantile(f64 nom_rank, bool inclusive = true) const;

        /// @brief Convert the sketch into a histogram.
        inline operator Histogram() const;

        void dump_weighted_items(vector<weighted_item> &list);
        void multi_query_find_e_by_w(vector<elem_t> &result, vector<double> &w);
        void multi_query_find_w_by_e(vector<double> &result, vector<elem_t> &e);

    private:
        u32 itemNum;               ///< Number of items in the sketch.
        u32 sketchCap;             ///< Capacity of the sketch.
        vec_cmtor cmtors;          ///< Compactors.
        elem_t minItem = UINT32_MAX;  ///< Minimum item in the sketch.
        elem_t maxItem = 0;           ///< Maximum item in the sketch.
        
        inline SortedView setupSortedView() const;
    };
} // namespace sketch

#include "mreq_sketch_impl.hpp"
#include "req_sketch_batch_query_impl.hpp"