#pragma once
#include <random>
#include <stdexcept>
#include <cstdio>
#include <fstream>
#include <climits>
#include <cmath>
#include <numeric>
#include <unordered_map>
#include "sketch_defs.hpp"
#include "file_path.hpp"

namespace sketch {
    // Random number generation.

    /// @brief Generate a random bit.
    /// @warning This function is not thread-safe.
    inline bool rand_bit() {
        static std::random_device rd;
        static std::default_random_engine gen(rd());
        static std::bernoulli_distribution dis(0.5);
        return dis(gen);
    }

    /// @brief Random u32 generator.
    struct rand_u32_generator {
        /// @brief Construct a generator with given seed and max value.
        rand_u32_generator(u32 seed = 0, u32 max = UINT32_MAX)
            : gen(seed), dis(0, max) { }

        /// @brief Generate a random u32 in [0, max].
        u32 operator()() {
            return dis(gen);
        }

    private:
        std::default_random_engine gen;
        std::uniform_int_distribution<u32> dis;
    };

    // Others.

    bool f64_equal(f64 a, f64 b) {
        return std::fabs(a - b) < 1e-5;
    }


    vector<FlowItem> load_dataset(const string& dataset) {
        const char* filename;
        if(dataset=="caida") {
            filename = caida_path;
        } else if (dataset=="imc") {
            filename = imc_path;
        } else if (dataset=="MAWI") {
            filename = mawi_path;
        } else {
            throw std::invalid_argument("unknown dataset");
        }    

        FILE* pf = fopen(filename, "rb");
        if(!pf){
            throw std::runtime_error("cannot open file");
        }

        vector<FlowItem> vec;
        double ftime = -1;
        long long fime = -1;
        char trace[30];

        if(filename == caida_path){
            while(fread(trace, 1, 21, pf)){
                uint32_t tkey = *(uint32_t*) (trace);
                double ttime = *(double*) (trace+13);
                if(ftime<0)ftime=ttime;
                vec.emplace_back((FlowItem){tkey, u32((ttime-ftime)*10000000)+1});
            }
        }else if(filename == imc_path){
            while(fread(trace, 1, 26, pf)){
                uint32_t tkey = *(uint32_t*) (trace);
                long long ttimee = *(long long*) (trace+18);
                if(fime<0)fime=ttimee;
                vec.emplace_back((FlowItem){tkey, u32((ttimee-fime)/100)+1});
            }
        }else{
            while(fread(trace, 1, 21, pf)){
                uint32_t tkey = *(uint32_t*) (trace);
                long long ttime = *(long long*) (trace+13);
                if(fime<0)fime=ttime;
                vec.emplace_back((FlowItem){tkey, u32((ttime-fime)*100000)+1});
            }
        }
        fclose(pf);

        std::unordered_map<u32, u32> mp;
        for (auto& [key, value]: vec) {
            u32 temp = value;
            value = std::max(1u, value - mp[key]);
            mp[key] = temp;
        }

        return vec;
    }

} // namespace sketch