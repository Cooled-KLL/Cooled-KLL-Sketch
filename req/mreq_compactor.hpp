#pragma once
#include "../common/common/sketch_utils.hpp"

namespace sketch {
    class mReqCmtor {
    public:
        /// @brief Constructor.
        /// @param lg_w_ log2 of weight of the compactor.
        /// @param cap_ Capacity of the compactor.
        mReqCmtor(u32 lg_w_, u32 cap_);
        
        /// @brief Deleted default constructor.
        mReqCmtor() = delete;

        /// @brief Return if the compactor is full.
        inline bool full() const;
        /// @brief Return size of the compactor.
        inline u32 size() const;
        /// @brief Return capacity of the compactor.
        inline u32 capacity() const;
        /// @brief Return log2 of weight of the compactor.
        inline u32 lgWeight() const;
        /// @brief Return weight of the compactor.
        inline u32 weight() const;
        /// @brief Return number of bytes the compactor uses.
        inline u32 memory() const;

        /// @brief Return begin iterator.
        inline elem_t_const_iter begin() const;
        /// @brief Return end iterator.
        inline elem_t_const_iter end() const;
        /// @brief Return begin iterator.
        inline elem_t_iter begin();
        /// @brief Return end iterator.
        inline elem_t_iter end();

        /// @brief Append a given item into the compactor.
        /// @param item Item to be appended.
        inline void append(elem_t item);

        /// @brief Compact the compactor into another compactor.
        /// @param next The next compactor which receives those
        ///             compacted items.
        inline void compact(mReqCmtor& next);

        /// @brief Estimate absolute rank of a given item.
        /// @param item Item to be ranked.
        /// @param inclusive If the given item is included in the rank.
        inline u32 rank(elem_t item, bool inclusive = true) const;

        /// @brief Estimate weighted rank of a given item.
        /// @param item Item to be ranked.
        /// @param inclusive If the given item is included in the rank.
        inline u32 weightedRank(elem_t item, bool inclusive = true) const;

        void dump_weighted_items(vector<weighted_item> &list);


    private:
        u32     lg_w;      ///< Log2 of the weight of the compactor.
        u32     cap;       ///< Capacity of the compactor.
        vec_elem_t items;     ///< Items in the compactor.
    };
} // namespace sketch

#include "mreq_compactor_impl.hpp"