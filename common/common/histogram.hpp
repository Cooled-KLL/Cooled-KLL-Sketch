#pragma once
#include "sketch_utils.hpp"

namespace sketch {
    class Histogram {
    public:
        // Constructors and destructors.

        Histogram() = default;

        Histogram(const vec_f64& split, const vec_u32& height) :
            m_splitPoints(split), m_heights(height) {}

        ~Histogram() = default;

        // Getters.

        vec_f64 splitPoints() const { return m_splitPoints; }
        vec_u32 heights() const { return m_heights; }

        // Operations.

        /// @brief Perform 'minimum' operation on two histograms.
        friend Histogram operator&(const Histogram& a, const Histogram& b);
        /// @brief Perform 'sum' operation on two histograms.
        friend Histogram operator|(const Histogram& a, const Histogram& b);

        /// @brief Estimate the quantile value of a given normalized rank.
        inline u32 quantile(f64 nom_rank) const;

    private:
        // It must be satisfied that m_splitPoints and m_heights are sorted
        // and that m_splitPoints.size() == m_heights.size() + 1.
        vec_f64 m_splitPoints;  ///< Split points.
        vec_u32 m_heights;      ///< Interval heights.

        /// @brief Split the histogram into intervals defined by split_points.
        inline Histogram split(const vec_f64& split_points) const;

        /// @brief Perform 'minimum' operation on two aligned histograms.
        inline static Histogram minAligned(const Histogram& h1,
                                           const Histogram& h2);
        /// @brief Perform 'sum' operation on two aligned histograms.
        inline static Histogram sumAligned(const Histogram& h1,
                                          const Histogram& h2);
    };
}   // namespace sketch

#include "histogram_impl.hpp"
