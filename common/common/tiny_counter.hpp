#pragma once
#include "sketch_utils.hpp"

namespace sketch {
    class TinyCnter {
    public:
        /// @brief Default constructor.
        TinyCnter();

        /// @brief Return if the counter is full.
        /// @param idx Counter index, must be 0-3.
        inline bool full(u32 idx) const;

        /// @brief Return if the counter is empty.
        /// @param idx Counter index, must be 0-3.
        inline bool empty(u32 idx) const;
        
        /// @brief Append a given item into the counter.
        /// @param item Item to be appended.
        /// @param idx Counter index, must be 0-3.
        inline void append(u32 item, u32 idx);

        /// @brief Count number of items in the counter.
        /// @param idx Counter index, must be 0-3.
        inline u32 count(u32 idx) const;

        /// @brief Return an approximate value of items in the counter.
        inline u32 value() const;

        /// @brief Return number of bytes the counter uses.
        inline u32 memory() const;

    private:
        u8 cnt0: 2;     ///< Counter 0.
        u8 cnt1: 2;     ///< Counter 1.
        u8 cnt2: 2;     ///< Counter 2.
        u8 cnt3: 2;     ///< Counter 3.
        u32 maxItem;    ///< Maximum item in four counters.
        static constexpr u32 MAX_CNT = 3;   ///< Maximum count value.

        /// @brief Check if a given index falls in 0-3.
        /// @param idx Index to be checked.
        inline void checkIdx(u32 idx) const;
    };
}   // namespace sketch

#include "tiny_counter_impl.hpp"