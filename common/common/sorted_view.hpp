#pragma once
#include "sketch_utils.hpp"
#include "histogram.hpp"

namespace sketch {
    class SortedView {
    public:
        /// @brief Weighted item.
        struct witem {
            u32 value;      ///< Value.
            u32 weight;     ///< Weight.

            /// @brief Compare by value.
            static bool value_less(const witem& a, const witem& b) {
                return a.value < b.value;
            }

            /// @brief Compare by weight.
            static bool weight_less(const witem& a, const witem& b) {
                return a.weight < b.weight;
            }
        };

        using witem_iter = vector<witem>::iterator;
        using witem_const_iter = vector<witem>::const_iterator;
        using vec_witem = vector<witem>;

    public:
        /// @brief Constructor.
        /// @param num Number of items in the sketch.
        SortedView(u32 num);

        /// @brief Deleted default constructor.
        SortedView() = delete;

        /// @brief Insert items in [first, last) into the sorted view.
        /// @param first First iterator.
        /// @param last Last iterator.
        /// @param weight Weight of each inserted item.
        inline void insert(u32_const_iter first, u32_const_iter last, 
                           u32 weight);

        /// @brief Insert items in [first, last) into the sorted view.
        /// @param first First iterator.
        /// @param last Last iterator.       
        inline void insert(witem_const_iter first, witem_const_iter last);

        /// @brief Insert a single item into the sorted view.
        /// @param value Item value.
        /// @param weight Item weight.
        inline void insert(u32 value, u32 weight);

        /// @brief Convert the sorted view to a cumulative view.
        inline void convertToCumulative();

        /// @brief Estimate absolute rank of a given item.
        /// @param item Item to be ranked. 
        /// @param inclusive If the item is included in the rank.
        inline u32 rank(u32 item, bool inclusive = true) const;
        
        /// @brief Estimate normalized rank of a given item.
        /// @param item Item to be ranked.
        /// @param inclusive If the item is included in the rank.
        inline f64 nomRank(u32 item, bool inclusive = true) const;

        /// @brief Estimate the quantile value of a given normalized rank.
        /// @param nom_rank Normalized rank.
        /// @param inclusive If the given rank is included.
        inline u32 quantile(f64 nom_rank, bool inclusive = true) const;

        /// @brief Convert the sorted view to a histogram.
        inline operator Histogram() const;

    private:
        vector<witem> view;     ///< Sorted view.
        u32 totalWeight;       ///< Total weight.
    };
} // namespace sketch

#include "sorted_view_impl.hpp"