#pragma once
#include "sketch_utils.hpp"
#include <algorithm>

namespace sketch {
    /// @brief Insert an item to an ordered vector while maintaining the order.
    /// @tparam T Vector element type.
    /// @param vec An ordered vector.
    /// @param item The item to be inserted.
    template<typename T>
    inline void vec_insert_ordered(vector<T>& vec, const T& item) {
        auto it = vec.begin();
        for (; it != vec.end() && *it < item; ++it);
        vec.insert(it, item);
    }

    /// @brief Insert an item to an ordered vector while maintaining the order.
    /// @tparam T Vector element type.
    /// @tparam Comp Comparator type.
    /// @param vec An ordered vector.
    /// @param item The item to be inserted.
    /// @param cmp Comparator.
    template<typename T, typename Comp>
    inline void vec_insert_ordered(vector<T>& vec, const T& item,
                                   Comp cmp) {
        auto it = vec.begin();
        for (; it != vec.end() && cmp(*it, item); ++it);
        vec.insert(it, item);
    }  

    /// @brief Insert an item to an unordered vector.
    /// @tparam T Vector element type.
    /// @tparam Comp Comparator type.
    /// @param vec An unordered vector.
    /// @param item The item to be inserted.
    /// @param cmp Comparator.
    template<typename T, typename Comp>
    inline void vec_insert_unordered(vector<T>& vec, const T& item,
                                   Comp cmp) {
        // auto it = vec.begin();
        // for (; it != vec.end() && cmp(*it, item); ++it);
        // vec.insert(it, item);
        vec.push_back(item);
    }  

    /// @brief Return the rank of an item in an ordered vector.
    /// @tparam T Vector element type.
    /// @param vec An ordered vector.
    /// @param item The item to be ranked.
    /// @param inclusive If the item is included in the rank.
    /// @details An item's rank is defined as follows:
    ///         - If @c inclusive, the number of items in the vector that are
    ///           less than or equal to the item.
    ///         - If not @c inclusive, the number of items in the vector that
    ///           are less than the item.
    template <typename T>
    inline u32 vec_rank(const vector<T>& vec, const T& item,
                        bool inclusive) {
        auto stop = [&item, inclusive](const T& t) {
            return t > item || (!inclusive && t == item);
        };
        u32 i = 0;
        for (; i < vec.size() && !stop(vec[i]); ++i);
        return i;
    }

    /// @brief Return the rank of an item in an ordered vector.
    /// @tparam T Vector element type.
    /// @tparam Comp Comparator type.
    /// @param vec An ordered vector.
    /// @param item The item to be ranked.
    /// @param inclusive If the item is included in the rank.
    /// @param cmp Comparator.
    /// @details An item's rank is defined as follows:
    ///         - If @c inclusive, the number of items in the vector that are
    ///           less than or equal to the item.
    ///         - If not @c inclusive, the number of items in the vector that
    ///           are less than the item.
    template <typename T, typename Comp>
    inline u32 vec_rank(const vector<T>& vec, const T& item,
                        bool inclusive, Comp cmp) {
        auto stop = [&item, inclusive, &cmp](const T& t) {
            return cmp(item, t) || (!inclusive && !cmp(t, item));
        };
        u32 i = 0;
        for (; i < vec.size() && !stop(vec[i]); ++i);
        return i;
    }

    /// @brief Return the union of two ordered vectors.
    /// @tparam T Vector element type.
    /// @param v1 The first ordered input vector.
    /// @param v2 The second ordered input vector.
    template <typename T>
    inline vector<T> vec_union(const vector<T>& v1, const vector<T>& v2) {
        vector<T> res;
        res.reserve(v1.size() + v2.size());
        std::set_union(v1.begin(), v1.end(), v2.begin(), v2.end(),
                       std::back_inserter(res));
        return res;
    }
}   // namespace sketch