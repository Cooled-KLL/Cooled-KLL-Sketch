#pragma once
#include "../common/common/sketch_defs.hpp"

#define private public

namespace sketch {
    class Centroid {
        friend class TDigest;
        friend class TDigestDiff;
    public:
        /// @brief Construct a centroid with mean 0.0 and weight 0.
        Centroid() : Centroid(0, 0) {}

        /// @brief Construct a centroid with given mean and weight.
        /// @param mean Mean of the centroid.
        /// @param weight Weight of the centroid.
        Centroid(f64 mean, u32 weight) : m_mean(mean), m_weight(weight) {}

        /// @brief Return the mean of the centroid.
        f64 mean() const { return m_mean; }
        /// @brief Return the weight of the centroid.
        u32 weight() const { return m_weight; }

        /// @brief Merge another centroid into this one.
        /// @param other The centroid to be merged.
        void merge(const Centroid& other) {
            f64 sum = m_mean * m_weight + other.m_mean * other.m_weight;
            m_weight += other.m_weight;
            m_mean = sum / m_weight;
        }

        /// @brief Append an item into the centroid.
        /// @param item The item to append. 
        void append(f64 item) {
            merge(Centroid(item, 1));
        }

        /// @brief Check if two centroids are the same object
        ///        by comparing their address.
        static bool addr_equal(const Centroid& a, const Centroid& b) {
            return &a == &b;
        }

        /// @brief Compare two centroids by their mean.
        static bool mean_less(const Centroid& a, const Centroid& b) {
            return a.mean() < b.mean();
        }

        /// @brief Compare two centroids by their weight.
        static bool weight_less(const Centroid& a, const Centroid& b) {
            return a.weight() < b.weight();
        }

    private:
        f64 m_mean;     ///< Mean of the centroid.
        u32 m_weight;   ///< Weight of the centroid.
    };
}   // namespace sketch

#undef private