// #include <cmath>
// #include "probability_functions.h"
// 
// // ZIP EM algorithm ------------------------------------------------------------
// 
// //' Calculate the conditional expectation of the structural zero indicator.
// //' 
// //' Calculate the conditional expectation of the structural zero indicator for 
// //' the zero-inflated Poisson distribution in the population-based ZIP scan 
// //' statistic model.
// //' @param mu The expected values of the count (which is zero).
// //' @param p The structural zero probability.
// //' @param pop An integer; the population at risk.
// //' @return A scalar between 0 and 1.
// //' @keywords internal
// // [[Rcpp::export]]
// double est_pb_zip_zeroindic(const double theta, 
//                             const double p, 
//                             const int pop) {
//   return p / (p + (1 - p) * exp(-pop * mu));
// }
// 
// //' Estimate the relative risk for the ZIP distribution.
// //' 
// //' Estimate the relative risk for the ZIP distribution in the population-based
// //' ZIP scan statistic model.
// //' @param y_sum A non-negative integer; the sum of the observed counts.
// //' @param mu A vector of positive integers; the populations at risk.
// //' @param p A vector of scalars between 0 and 1; the structural zero
// //'    probabilities.
// //' @param d_hat A vector of estimates of the structural zero indicators.
// //' @return A scalar; the relative risk.
// //' @keywords internal
// // [[Rcpp::export]]
// double est_pb_zip_relrisk(const int y_sum,
//                           const arma::uvec& pop,
//                           const arma::vec& p,
//                           const arma::vec& d_hat) {
//   double denominator = 0;
//   for (int i = 0; i < pop.n_elem; ++i) {
//     denominator += pop[i] * (1.0 - d_hat[i]);
//   }
//   return std::max(1.0, y_sum / denominator);
// }
// 
// 
// //' Estimate parameters for the ZIP 
// Rcpp::List pb_zip_em_null(const arma::vec y, const arma::uvec pop) {
//   
//   int y_sum = arma::sum(y);
//   
// }
// 
// 
// //' Calculate the ZIP loglihood ratio statistic and the relative risk.
// //' 
// //' Calculate the ZIP loglihood ratio statistic and the relative risk for the 
// //' population-based ZIP scan statistic model.
// //' @param y A non-negative integer vector; the observed counts.
// //' @param mu A vector of positive scalars; the expected values of the counts.
// //' @param p A vector of scalars between 0 and 1; the structural zero
// //'    probabilities.
// //' @param d_hat A vector of estimates of the structural zero indicators.
// //' @param rel_tol The absolute convergence criterion.
// //' @return A list with four elements:
// //'    \enumerate{
// //'      \item The loglihood ratio statistic.
// //'      \item The estimated relative risk inside the cluster.
// //'      \item The estimated relative risk outside the cluster.
// //'      \item The number of iterations of the EM algorithm performed.
// //'    } 
// //' @keywords internal
// // [[Rcpp::export]]
// Rcpp::List score_pb_zip(const arma::uvec& y_in,
//                         const arma::uvec& y_out,
//                         const arma::uvec& pop_in,
//                         const arma::uvec& pop_out,
//                         const double rel_tol = 1e-3) {
//   Rcpp::List score_q_niter (4);
//   
//   arma::vec y
//   
//   // Structural zero estimates
//   arma::vec d_null = arma::zeros(y_in.n_elem + ); 
//   arma::vec d_in = arma::zeros(y.n_elem); 
//   
//   double loglik_null = zip_loglihood(y, mu, p, 1.0);
//   double loglik_old = loglik_null;
//   double loglik_new;
//   
//   // Store indices of zero counts; only loop through these when estimating 
//   // structural zero indicators. Also compute the sum of all counts.
//   std::vector<int> zero_idx;
//   int y_sum = 0;
//   for (int i = 0; i < y.n_elem; ++i) {
//     y_sum += y[i];
//     if (y[i] == 0) zero_idx.push_back(i);
//   }
//   
//   
//   
//   // Run EM algorithm
//   double diff = 2.0 * rel_tol;
//   int n_iterations = 0;
//   while (diff > rel_tol) {
//     n_iterations += 1;
//     
//     // Expectation-step
//     for (const int& i : zero_idx) {
//       d_hat[i] = est_eb_zip_zeroindic(mu[i], p[i], q_hat);
//     }
//     // Maximization-step
//     q_hat = est_eb_zip_relrisk(y_sum, mu, p, d_hat);
//     
//     // Update likelihood
//     loglik_new = zip_loglihood(y, mu, p, q_hat);
//     diff = abs(exp(loglik_new - loglik_old) - 1);
//     loglik_old = loglik_new;
//   }
//   
//   score_q_niter[0] = loglik_new - loglik_null;
//   score_q_niter[1] = q_hat;
//   score_q_niter[2] = n_iterations;
//   
//   return score_q_niter;
// }
// 
// // EB ZIP scan statistic -------------------------------------------------------
// 
// //' Calculate the EB ZIP loglihood ratio statistic for all clusters.
// //' 
// //' Calculate the expectation-based ZIP loglihood ratio statistic for each zone 
// //' and duration. The estimate of the relative risk is also calculated, along 
// //' with the number of iterations the EM algorithm performed for each zone and 
// //' duration.
// //' @param counts A matrix of non-negative integers; the observed counts. Rows
// //'    indicate time, ordered from most recent (row 1) to least recent. Columns
// //'    indicate locations; the locations are numbered from 1 and up.
// //' @param baselines A matrix of positive scalars; the expected values of the 
// //'    counts. Of the same dimensions as \code{counts}.
// //' @param probs A matrix of scalars between 0 and 1; the structural zero
// //'    probabilities. Of the same dimensions as \code{counts}.
// //' @param zones An integer vector containing the zones, stored one after 
// //'    another. Each zone is found using the elements of the parameter 
// //'    \code{zone_lengths}. For example, if the first element of 
// //'    \code{zone_lengths} is 5, then the first 5 elements of \code{zones}
// //'    make up the first zone. If the second element of \code{zone_lengths} is
// //'    2, then elements 6 and 7 of \code{zones} make up the second zone, and so
// //'    on. Note that the zones are numbered from 0 and up in the input, but
// //'    from 1 and up in the output.
// //' @param zone_lengths An integer vector holding the number of locations in 
// //'    each zone.
// //' @param rel_tol A positive scalar. If the relative change in the incomplete
// //'    information likelihood is less than this value, then the EM algorithm is
// //'    deemed to have converged.
// //' @return A data frame with five columns:
// //'    \describe{
// //'      \item{zone}{The (number of the) zone.}
// //'      \item{duration}{The duration.}
// //'      \item{score}{The value of the loglihood ratio statistic.}
// //'      \item{relrisk}{The estimated relative risk.}
// //'      \item{n_iter}{The number of iterations performed by the EM algorithm.}
// //'    } 
// //' @keywords internal
// // [[Rcpp::export]]
// Rcpp::DataFrame scan_eb_zip_cpp(const arma::umat& counts,
//                                 const arma::mat&  baselines,
//                                 const arma::mat&  probs,
//                                 const arma::uvec& zones,
//                                 const arma::uvec& zone_lengths,
//                                 const double rel_tol = 1e-3) {
//   int max_duration = counts.n_rows;
//   int n_zones = zone_lengths.n_elem;
//   
//   // Components of returned list
//   arma::uvec zone_numbers (n_zones * max_duration);
//   arma::uvec durations    (n_zones * max_duration);
//   arma::vec  scores       (n_zones * max_duration);
//   arma::vec  relrisks     (n_zones * max_duration);
//   arma::uvec iterations   (n_zones * max_duration);
//   
//   int i = 0; // Storage index
//   Rcpp::List score_q_niter (3); // Output from score_eb_zip
//   
//   for (int d = 0; d < max_duration; ++d) {
//     
//     // Vector for extracting (d + 1) latest time periods
//     arma::uvec row_idx (d + 1);
//     for (int k = 0; k <= d; ++k) row_idx[k] = k;
//     
//     // Indices for extracting the current zone
//     int zone_start = 0;
//     int zone_end = 0;
//     
//     for (int z = 0; z < n_zones; ++z) {
//       // Extract zone
//       zone_end = zone_start + zone_lengths[z] - 1;
//       arma::uvec current_zone = zones(arma::span(zone_start, zone_end));
//       
//       score_q_niter = score_eb_zip(
//         arma::vectorise(counts.submat(row_idx, current_zone)),
//         arma::vectorise(baselines.submat(row_idx, current_zone)),
//         arma::vectorise(probs.submat(row_idx, current_zone)),
//         rel_tol);
//       
//       // Store results
//       scores[i]       = score_q_niter[0];
//       relrisks[i]     = score_q_niter[1];
//       iterations[i]   = score_q_niter[2];
//       zone_numbers[i] = z + 1;
//       durations[i]    = d + 1;
//       
//       // Update indices
//       zone_start = zone_end + 1;
//       ++i;
//     }
//   }
//   return Rcpp::DataFrame::create(Rcpp::Named("zone")     = zone_numbers,
//                                  Rcpp::Named("duration") = durations,
//                                  Rcpp::Named("score")    = scores,
//                                  Rcpp::Named("relrisk")  = relrisks,
//                                  Rcpp::Named("n_iter")   = iterations);
// }
// 
// //' Calculate the highest-value EB ZIP loglihood ratio statistic.
// //' 
// //' Calculate the expectation-based ZIP loglihood ratio statistic for each zone 
// //' and duration, but only keep the zone and duration with the highest value 
// //' (the MLC). The estimate of the relative risk is also calculated, along with 
// //' the number of iterations the EM algorithm performed.
// //' @inheritParams scan_eb_zip_cpp
// //' @return A data frame with five columns:
// //'    \describe{
// //'      \item{zone}{The top-scoring zone (spatial component of MLC).}
// //'      \item{duration}{The corresponding duration (time-length of MLC).}
// //'      \item{score}{The value of the loglihood ratio statistic (the scan
// //'                   statistic).}
// //'      \item{relrisk}{The estimated relative risk.}
// //'      \item{n_iter}{The number of iterations performed by the EM algorithm.}
// //'    } 
// //' @keywords internal
// // [[Rcpp::export]]
// Rcpp::DataFrame scan_eb_zip_cpp_max(const arma::umat& counts,
//                                     const arma::mat&  baselines,
//                                     const arma::mat&  probs,
//                                     const arma::uvec& zones,
//                                     const arma::uvec& zone_lengths,
//                                     const double rel_tol = 1e-3) {
//   int max_duration = counts.n_rows;
//   int n_zones = zone_lengths.n_elem;
//   
//   // Return values
//   int    mlc_zone       = -1;
//   int    mlc_duration   = -1;
//   double mlc_score      = -1.0;
//   double mlc_relrisk    = -1.0;
//   int    mlc_iterations = -1;
//   
//   Rcpp::List score_q_niter (3); // Output from score_eb_zip
//   
//   for (int d = 0; d < max_duration; ++d) {
//     
//     // Vector for extracting (d + 1) latest time periods
//     arma::uvec row_idx (d + 1);
//     for (int k = 0; k <= d; ++k) row_idx[k] = k;
//     
//     // Indices for extracting the current zone
//     int zone_start = 0;
//     int zone_end = 0;
//     
//     for (int z = 0; z < n_zones; ++z) {
//       
//       // Extract zone
//       zone_end = zone_start + zone_lengths[z] - 1;
//       arma::uvec current_zone = zones(arma::span(zone_start, zone_end));
//       
//       score_q_niter = score_eb_zip(
//         arma::vectorise(counts.submat(row_idx, current_zone)),
//         arma::vectorise(baselines.submat(row_idx, current_zone)),
//         arma::vectorise(probs.submat(row_idx, current_zone)),
//         rel_tol);
//       
//       // Update return values if new score is highest so far
//       if (score_q_niter[0] > mlc_score) {
//         mlc_zone       = z + 1;
//         mlc_duration   = d + 1;
//         mlc_score      = score_q_niter[0];
//         mlc_relrisk    = score_q_niter[1];
//         mlc_iterations = score_q_niter[2];
//       }
//       
//       zone_start = zone_end + 1;
//     }
//   }
//   return Rcpp::DataFrame::create(Rcpp::Named("zone")     = mlc_zone,
//                                  Rcpp::Named("duration") = mlc_duration,
//                                  Rcpp::Named("score")    = mlc_score,
//                                  Rcpp::Named("relrisk")  = mlc_relrisk,
//                                  Rcpp::Named("n_iter")   = mlc_iterations);
// }
