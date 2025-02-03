#include <RcppArmadillo.h>
#include <RcppParallel.h>
#if RCPP_PARALLEL_USE_TBB
#include <tbb/task_arena.h>
#endif
// [[Rcpp::depends(RcppArmadillo, RcppParallel)]]

// Serial case

double fast_auc_code(const arma::vec& probs, SEXP actualSEXP) {
  
  std::size_t n_size = probs.n_elem;
  arma::vec r(n_size);
  arma::uvec w = arma::sort_index(probs);
  double s1 = 0.0;
  std::size_t n1 = 0;
  
  for (std::size_t i = 0, n; i < n_size; i += n) {
    n = 1;
    while (i + n < n_size && probs[w[i]] == probs[w[i + n]]) ++n;
    double i_p_np1_half = i + (n + 1) * 0.5;
    for (std::size_t k = 0; k < n; ++k) {
      r[w[i + k]] = i_p_np1_half; // average rank of n tied values
    }
  }
  
  // Check the type of actual and dispatch accordingly
  if (Rf_isInteger(actualSEXP)) {
    Rcpp::IntegerVector actual(actualSEXP);
    for (std::size_t i = 0; i < n_size; ++i) {
      if (actual[i]) {
        s1 += r[i];
        ++n1;
      }
    }
  } else if (Rf_isLogical(actualSEXP)) {
    Rcpp::LogicalVector actual(actualSEXP);
    for (std::size_t i = 0; i < n_size; ++i) {
      if (actual[i]) {
        s1 += r[i];
        ++n1;
      }
    }
  } else if (Rf_isNumeric(actualSEXP)) {
    Rcpp::NumericVector actual(actualSEXP);
    for (std::size_t i = 0; i < n_size; ++i) {
      if (actual[i]) {
        s1 += r[i];
        ++n1;
      }
    }
  } else {
    Rcpp::stop("Unsupported type for 'actual'.");
    return NA_REAL; // In case of unsupported type
  }
  
  return (s1 - n1 * (n1 + 1) * 0.5) /\
    (n1 * (n_size - n1));
  
}

// Parallel case

struct ComputeRanks : public RcppParallel::Worker {
  const arma::vec& probs;
  const arma::uvec& w;
  arma::vec& r;
  
  ComputeRanks(const arma::vec& probs,
               const arma::uvec& w,
               arma::vec& r)
    : probs(probs), w(w), r(r) {}
  
  void operator()(std::size_t begin, std::size_t end) {
    for (std::size_t i = begin, n; i < end; i += n) {
      n = 1;
      while (i + n < end && probs[w[i]] == probs[w[i + n]]) ++n;
      double i_p_np1_half = i + (n + 1) * 0.5;
      for (std::size_t k = 0; k < n; ++k) {
        r[w[i + k]] = i_p_np1_half; // average rank of n tied values
      }
    }
  }
};

struct FastAUC_int : public RcppParallel::Worker {
  const arma::vec& r;
  const RcppParallel::RVector<int>& actual;
  double s1;
  std::size_t n1;
  
  FastAUC_int(const arma::vec& r, const RcppParallel::RVector<int>& actual)
    : r(r), actual(actual), s1(0.0), n1(0) {}
  
  FastAUC_int(const FastAUC_int& other, RcppParallel::Split)
    : r(other.r), actual(other.actual), s1(0.0), n1(0) {}
  
  void operator()(std::size_t begin, std::size_t end) {
    for (std::size_t i = begin; i < end; ++i) {
      if (actual[i]) {
        s1 += r[i];
        n1++;
      }
    }
  }
  
  void join(const FastAUC_int& other) {
    s1 += other.s1;
    n1 += other.n1;
  }
};

struct FastAUC_double : public RcppParallel::Worker {
  const arma::vec& r;
  const RcppParallel::RVector<double>& actual;
  double s1;
  std::size_t n1;
  
  FastAUC_double(const arma::vec& r,
                 const RcppParallel::RVector<double>& actual)
    : r(r), actual(actual), s1(0.0), n1(0) {}
  
  FastAUC_double(const FastAUC_double& other, RcppParallel::Split)
    : r(other.r), actual(other.actual), s1(0.0), n1(0) {}
  
  void operator()(std::size_t begin, std::size_t end) {
    for (std::size_t i = begin; i < end; ++i) {
      if (actual[i]) {
        s1 += r[i];
        n1++;
      }
    }
  }
  
  void join(const FastAUC_double& other) {
    s1 += other.s1;
    n1 += other.n1;
  }
};

double fast_auc_code_par(const arma::vec& probs,
                         const SEXP actualSEXP,
                         const int numThreads) {
  
  std::size_t n_size = probs.n_elem;
  arma::vec r(n_size);
#if RCPP_PARALLEL_USE_TBB
  arma::uvec w = arma::regspace<arma::uvec>(0, n_size - 1);
  // tbb::task_arena arena(numThreads);
  // tbb::task_scheduler_init init(numThreads);
  tbb::global_control c(tbb::global_control::max_allowed_parallelism,
                        numThreads);
  tbb::parallel_sort(std::begin(w), std::end(w), 
                     [&probs](std::size_t i, std::size_t j) { return probs[i] < probs[j]; });
#else
  arma::uvec w = arma::sort_index(probs);
#endif
  
  // // Parallelize the rank computation
  ComputeRanks computeRanks(probs, w, r);
  parallelFor(0, n_size, computeRanks, numThreads);
  
  // Check the type of actual and dispatch accordingly
  if (Rf_isInteger(actualSEXP)) {
    Rcpp::IntegerVector actual(actualSEXP);
    RcppParallel::RVector<int> actual_vector(actual);
    FastAUC_int fastAUC_int(r, actual_vector);
    parallelReduce(0, n_size, fastAUC_int);
    return (fastAUC_int.s1 - fastAUC_int.n1 * (fastAUC_int.n1 + 1) * 0.5) / \
      (fastAUC_int.n1 * (n_size - fastAUC_int.n1));
  } else if (Rf_isLogical(actualSEXP)) {
    Rcpp::LogicalVector actual(actualSEXP);
    // Convert the Rcpp::LogicalVector to RVector<int> which is thread-safe
    // RcppParallel::RVector<bool> can not be handled...
    RcppParallel::RVector<int> actual_vector(actual);
    FastAUC_int fastAUC_int(r, actual_vector);
    parallelReduce(0, n_size, fastAUC_int, numThreads);
    return (fastAUC_int.s1 - fastAUC_int.n1 * (fastAUC_int.n1 + 1) * 0.5) / \
      (fastAUC_int.n1 * (n_size - fastAUC_int.n1));
  } else if (Rf_isNumeric(actualSEXP)) {
    Rcpp::NumericVector actual(actualSEXP);
    RcppParallel::RVector<double> actual_vector(actual);
    FastAUC_double fastAUC_double(r, actual_vector);
    parallelReduce(0, n_size, fastAUC_double);
    return (fastAUC_double.s1 - fastAUC_double.n1 * (fastAUC_double.n1 + 1) * \
            0.5) / (fastAUC_double.n1 * (n_size - fastAUC_double.n1));
  } else {
    Rcpp::stop("Unsupported type for 'actual'.");
  }
  
  return NA_REAL; // In case of unsupported type
}


// [[Rcpp::export]]
double lightAUC(const arma::vec& probs,
                const SEXP actuals,
                const bool parallel = false,
                const int cores = 2){
  if (parallel){
    if (cores <= 0){
      Rcpp::stop("If parallel=TRUE, then number of threads must positive.");
      return NA_REAL; // In case of unsupported type
    }
    return fast_auc_code_par(probs, actuals, cores);
  } else {
    return fast_auc_code(probs, actuals);
  }
}
