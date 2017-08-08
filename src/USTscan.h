#ifndef USTSCAN_H
#define USTSCAN_H

#include <cmath>
#include "probability_functions.h"
#include "RcppArmadillo.h"
// [[depends(RcppArmadillo)]]

// Univariate Spate-Time scan statistic (base class) ---------------------------
template <class T, class t>
class USTscan {

public:
  USTscan(const T& counts,
          const arma::uvec& m_zones,
          const arma::uvec& m_zone_lengths,
          const bool store_everything,
          const arma::uword num_mcsim);
  void run_scan();
  virtual void run_mcsim();

  virtual Rcpp::DataFrame get_scan() = 0;
  virtual Rcpp::DataFrame get_mcsim() = 0;

protected:
  arma::uword        m_num_locs;
  arma::uword        m_num_zones;
  arma::uword        m_max_dur;
  arma::uword        m_num_mcsim;
  bool       m_store_everything;
  arma::uword        m_mcsim_index;
  arma::uword        m_out_length;
  T          m_counts;
  arma::uvec m_zones;
  arma::uvec m_zone_lengths;

  // Values calculated on observed data
  arma::uvec m_zone_numbers;
  arma::uvec m_durations;
  arma::vec  m_scores;

  // Values calculated on simulated data
  arma::uvec sim_zone_numbers;
  arma::uvec sim_durations;
  arma::vec  sim_scores;

  virtual void calculate(const arma::uword storage_index,
                         const arma::uword zone_nr,
                         const arma::uword duration,
                         const arma::uvec& current_zone,
                         const arma::uvec& current_rows) = 0;

  virtual void simulate_counts();
  virtual t draw_sample(arma::uword row, arma::uword col) = 0;
  virtual void set_sim_store_fun() = 0;

  arma::uvec zone_complement(const arma::uvec& zone);
  arma::uvec duration_complement(const arma::uword d);

};

// Implementations -------------------------------------------------------------

template <class T, class t>
inline USTscan<T, t>::USTscan(const T& counts,
                              const arma::uvec& zones,
                              const arma::uvec& zone_lengths,
                              const bool store_everything,
                              const arma::uword num_mcsim)
  : m_counts(counts),
    m_num_locs(counts.n_cols),
    m_num_zones(zone_lengths.n_elem),
    m_max_dur(counts.n_rows),
    m_zones(zones),
    m_zone_lengths(zone_lengths),
    m_store_everything(store_everything),
    m_num_mcsim(num_mcsim),
    m_mcsim_index(0) {

  m_out_length = (store_everything ? m_num_zones * m_max_dur : 1);

  // Reserve sizes for values calculated on observed data
  m_zone_numbers.set_size(m_out_length);
  m_durations.set_size(m_out_length);
  m_scores.set_size(m_out_length);

  // Reserve sizes for values calculated on simulated
  sim_zone_numbers.set_size(m_num_mcsim);
  sim_durations.set_size(m_num_mcsim);
  sim_scores.set_size(m_num_mcsim);

  if (!store_everything) {
    m_scores[0] = R_NegInf;
  }
}

template <class T, class t>
inline void USTscan<T, t>::run_scan() {
  arma::uword i = 0; // Storage index
  for (arma::uword d = 0; d < m_max_dur; ++d) {

    arma::uvec row_idx (d + 1); // d+1 latest time periods
    for (arma::uword k = 0; k <= d; ++k) row_idx[k] = k;

    // Indices for extracting the current zone
    arma::uword zone_start = 0;
    arma::uword zone_end = 0;

    for (arma::uword z = 0; z < m_num_zones; ++z) {

      // Extract zone
      zone_end = zone_start + m_zone_lengths[z] - 1;
      arma::uvec current_zone = m_zones(arma::span(zone_start, zone_end));

      // Calculate scan statistic + related quantities for current ST-window
      calculate(i, z, d, current_zone, row_idx);

      if (i % 500 == 0) Rcpp::checkUserInterrupt();

      zone_start = zone_end + 1;
      ++i;
    }
  }
}

template <class T, class t>
inline void USTscan<T, t>::run_mcsim() {
  set_sim_store_fun();
  while (m_mcsim_index < m_num_mcsim) {
    sim_scores[m_mcsim_index] = R_NegInf;
    simulate_counts();
    run_scan();
    ++m_mcsim_index;
  }
}

template <class T, class t>
inline void USTscan<T, t>::simulate_counts() {
  for (arma::uword j = 0; j < m_counts.n_cols; ++j) {
    for (arma::uword i = 0; i < m_counts.n_rows; ++i) {
      m_counts.at(i, j) = draw_sample(i, j);
    }
  }
}

#endif
