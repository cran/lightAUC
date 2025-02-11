<!-- badges: start -->
[![CRAN status](https://www.r-pkg.org/badges/version/lightAUC)](https://CRAN.R-project.org/package=lightAUC)
[![Developmental version](https://img.shields.io/badge/devel%20version-0.1.2-blue.svg)](https://github.com/cadam00/lightAUC)
[![R-CMD-check](https://github.com/cadam00/lightAUC/actions/workflows/R-CMD-check.yaml/badge.svg)](https://github.com/cadam00/lightAUC/actions/workflows/R-CMD-check.yaml)
[![Codecov test coverage](https://codecov.io/gh/cadam00/lightAUC/graph/badge.svg)](https://app.codecov.io/gh/cadam00/lightAUC)
<!-- badges: end -->

# **Fast AUC computation in R**

Fast and lightweight computation of AUC metric for the binary case (1 positive
and 0 negative) is offered by <b>lightAUC</b> package. The algorithm used is a
fast implementation from  algorithm of Fawcett ([2006](#ref-fawcett2006)).

## **Install**

The official [(CRAN)](https://cran.r-project.org/) version of the package can be
installed using
``` r
install.packages("lightAUC")
```

Alternatively, the development version of the package can be installed via
``` r
if (!require(remotes)) install.packages("remotes")
remotes::install_github("cadam00/lightAUC")
```


## **Citation**

To cite the official [(CRAN)](https://cran.r-project.org/) version of the
package, please use

<blockquote>
<p>Adam, C. (2025). lightAUC: Fast AUC Computation. R package version 0.1.2.
doi:<a href="https://doi.org/10.32614/CRAN.package.lightAUC"
class="uri">10.32614/CRAN.package.lightAUC</a></p>
</blockquote>


## **Example**

```r
# Create some data
probs   <- c(1, 0.4, 0.8)
actuals <- c(0, 0, 1)
lightAUC(probs, actuals)
```
```
## 0.5
```

For parallel calculations use:
```r
# E.g. 2 cores (you can use cores = parallel::detectCores() for your case)
probs   <- c(1, 0.4, 0.8)
actuals <- c(0, 0, 1)
lightAUC(probs, actuals, parallel = TRUE, cores = 2)
```
```
## 0.5
```


## **References**

<span class="nocase" id="ref-fawcett2006">
Fawcett, T. (2006). An introduction to ROC analysis. <emph>Pattern Recognition
Letters</emph>, <b>27</b>(8), 861–874. doi:<a href=
"https://doi.org/10.1016/j.patrec.2005.10.010">10.1016/j.patrec.2005.10.010</a>

