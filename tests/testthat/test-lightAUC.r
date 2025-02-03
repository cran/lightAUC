test_that("lightAUC works", {
  library(lightAUC)
  probs   <- c(1, 0.4, 0.8)
  actuals <- c(0L, 0L, 1L)
  expect_equal(lightAUC(probs, actuals), 0.5)
  expect_equal(lightAUC(probs, actuals, parallel = TRUE, cores = 2L), 0.5)

  actuals <- c(FALSE, FALSE, TRUE)
  expect_equal(lightAUC(probs, actuals), 0.5)
  expect_equal(lightAUC(probs, actuals, parallel = TRUE, cores = 2L), 0.5)

  actuals <- c(0.0, 0.0, 1.0)
  expect_equal(lightAUC(probs, actuals), 0.5)
  expect_equal(lightAUC(probs, actuals, parallel = TRUE, cores = 2L), 0.5)

  tryerror <- try(lightAUC(probs, actuals, parallel = TRUE, cores = 0L),
                  silent = TRUE)

  expect_true(class(tryerror) == "try-error")

  expect_equal(
    tryerror[[1]],
    paste0("Error : If parallel=TRUE, then number of threads must positive.\n")
  )
  
  tryerror <- try(lightAUC(probs, as.character(actuals)),
                  silent = TRUE)

  expect_true(class(tryerror) == "try-error")

  expect_equal(
    tryerror[[1]],
    paste0("Error : Unsupported type for 'actual'.\n")
  )

  tryerror <- try(lightAUC(probs, as.character(actuals), parallel=TRUE),
                  silent = TRUE)
  
  expect_true(class(tryerror) == "try-error")
  
  expect_equal(
    tryerror[[1]],
    paste0("Error : Unsupported type for 'actual'.\n")
  )
  
  Sys.setenv(RCPP_PARALLEL_BACKEND = "tinythread")
  actuals <- c(0.0, 0.0, 1.0)
  expect_equal(lightAUC(probs, actuals, parallel = TRUE, cores = 2L), 0.5)
  
  actuals <- c(0L, 0L, 1L)
  expect_equal(lightAUC(probs, actuals, parallel = TRUE, cores = 2L), 0.5)
})