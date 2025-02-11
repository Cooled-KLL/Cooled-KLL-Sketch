#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <cstdint>

namespace sketch {
    using std::cout, std::cerr, std::endl;
    using std::vector, std::string;

    using namespace std::chrono;

    using u8 = uint8_t;
    using i32 = int32_t;
    using u32 = uint32_t;
    using u64 = uint64_t;
    using f64 = double;
    using vec_f64 = vector<f64>;
    using vec_u32 = vector<u32>;
    using u32_const_iter = vec_u32::const_iterator;
    using u32_iter = vec_u32::iterator;

#define UNUSED(x) (void(x))

    enum FlowType {
        TINY = 1,
        MID = 2,
        HUGE = 4,
    };


    struct FlowItem {
        u32 id;
        u32 value;
    };

    // SketchModel is defined as enum not enum class
    // to allow for implicit conversion from/to u32.
    enum SketchModel {
        M4MODEL,
        STRAW,
        NUM_MODELS,
    };

    enum MetaModel {
        DD,
        MREQ,
        TD,
        NUM_METAS,
    };
}   // namespace sketch