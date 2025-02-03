\name{lightAUC}

\alias{lightAUC}

\title{Fast AUC computation}

\description{
Fast and memory efficient AUC computation.
}

\usage{
lightAUC(probs, actuals, parallel = FALSE, cores = 2)
}

\arguments{
\item{probs}{
\code{numeric} vector containing probability from the model, where closer to 1
is for the positive class and closer to 0 is for the negative class.
}
\item{actuals}{
\code{integer}, \code{numeric} or \code{logical} vector with the actual data,
where is 1 for the positive class and 0 for the negative class.
}
\item{parallel}{
\code{logical} indicating if multithreading should be used. The default is no
multithreading (\code{parallel = FALSE}).
}
\item{cores}{
\code{integer} indicating the number of threads to be used when
\code{parallel = TRUE}. The default is \code{cores=2}, meaning that 2 cores are
used.
}
}

\details{
Binary AUC computation according to Fawcett (2006)
\doi{10.1016/j.patrec.2005.10.010}.
}

\value{
\code{numeric} value representing the AUC metric.
}

\references{
Fawcett, T. (2006). An introduction to ROC analysis. \emph{Pattern Recognition
Letters}, \bold{27}(8), 861–874. \doi{10.1016/j.patrec.2005.10.010}
}

\examples{
probs   <- c(1, 0.4, 0.8)
actuals <- c(0, 0, 1)
lightAUC(probs, actuals)

probs   <- c(1, 0.4, 0.8)
actuals <- c(FALSE, FALSE, TRUE)
lightAUC(probs, actuals)

probs   <- c(1, 0.4, 0.8)
actuals <- c(0, 0, 1)
lightAUC(probs, actuals, parallel = TRUE, cores = 2L)
}
