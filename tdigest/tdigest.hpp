#pragma once
#include "centroid.hpp"
#include <utility>
#include "../common/common/histogram.hpp"

namespace sketch {
    class TDigest {
    public:
        /// @brief Default constructor.
        /// @warning Members are potential uninitialized after construction.
        ///          Make sure you know what you are doing.
        TDigest() = default;

        /// @brief Constructor.
        /// @param cap_ Capacity, i.e. maximum number of items that
        ///             can be held in the t-digest.
        /// @param delta_ Argument for compression.
        TDigest(u32 cap_, u32 delta_);

        /// @brief Destructor.
        ~TDigest() = default;

        /// @brief Return the number of items in the t-digest.
        inline u32 size() const;

        /// @brief Return whether the t-digest is empty.
        inline bool empty() const;

        /// @brief Return whether the t-digest is full.
        inline bool full() const;

        /// @brief Return the number of bytes the t-digest uses.
        inline u32 memory() const;

        /// @brief Append an item to the t-digest.
        /// @param item The item to append.
        inline void append(elem_t item);

        /// @brief Estimate the quantile value of a normalized rank.
        /// @param nom_rank Normalized rank.
        inline u32 quantile(f64 nom_rank) const;

        /// @brief Convert the t-digest to a histogram.
        inline operator Histogram() const;
        
    private:
        vector<Centroid> centroids; ///< Centroids in the t-digest.
        u32 totalWeight;            ///< Total weight.
        u32 cap;                    ///< Capacity.
        u32 DELTA;                  ///< Argument delta, logically const.

        elem_t min_item = UINT32_MAX;  ///< Minimum item value.
        elem_t max_item = 0;           ///< Maximum item value.
        u32 max_weight = 0;

        /// @brief Calculate scale function of a percentage.
        /// @param p Percentage.
        inline f64 scale(f64 p) const;

        /// @brief Calculate the quantile bounds of a centroid.
        /// @param c The centroid, must be an element in @c centroids.
        /// @return A @c std::pair<f64, f64>, whose first key is the
        ///         centroid's q_{left} and second key is its q_{right}.
        inline std::pair<f64, f64> qBound(const Centroid& c) const;

        /// @brief Calculate the k-size of a centroid.
        /// @param c The centroid, must be an element, in @c centroids.
        inline f64 kSize(const Centroid& c) const;
    
        inline void compressNearest();

        /// @brief Find the appending position of a new item.
        /// @param item The item to append.
        /// @return Pointer to a centroid if found appropriate position,
        ///         @c nullptr otherwise.
        inline Centroid* findAppendPos(elem_t item);

        /// @brief Check whether a centroid can be appended to, i.e.
        ///        its k-size still no more than 1 after the appending.
        /// @param c The centroid to be appended to, 
        ///          must be an element in @c centroids.
        inline bool appendable(const Centroid& c) const;

        void dump_weighted_items(vector<weighted_item> &list);
public:
        void multi_query_find_e_by_w(vector<elem_t> &result, vector<double> &w);
        void multi_query_find_w_by_e(vector<double> &result, vector<elem_t> &e);
    };
}   // namespace sketch

#include "tdigest_impl.hpp"
#include "tdigest_batch_query_impl.hpp"