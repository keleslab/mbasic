% Generated by roxygen2 (4.1.0): do not edit by hand
% Please edit documentation in R/MBASIC_state.R
\name{MBASIC.state}
\alias{MBASIC.state}
\title{Bayesian clustering model for a state-space matrix.}
\usage{
MBASIC.state(Theta, J, struct = NULL, method = "SE-MC", zeta = 0.1,
  maxitr = 100, tol = 1e-10, tol.par = 0.001, para = NULL, out = NULL,
  W.init = NULL, Z.init = NULL, P.init = NULL, b.init = NULL)
}
\arguments{
\item{Theta}{A K by I matrix. The (k,i)-th entry is the state of the i-th unit under condition k. Notice that the sorted distinct values of entries in this matrix must be 1,2,...,S, where S is the total number of states.}

\item{J}{The number of clusters to be identified.}

\item{struct}{A K by J matrix indicating the structures of each cluster.}

\item{method}{A string for the fitting method, 'SE-HC' or 'SE-MC'(default).}

\item{zeta}{The initial value for the proportion of units that are not clustered. Default: 0.1.}

\item{maxitr}{The maximum number of iterations in the E-M algorithm. Default: 100.}

\item{tol}{Tolerance for error in checking the E-M algorithm's convergence. Default: 1e-04.}

\item{para}{A list object that contains the true model parameters. Default: NULL. See details for more information.}

\item{out}{The file directory for writing fitting information in each E-M iteration. Default: NULL (no information is outputted).}
}
\value{
An object of class \linkS4class{MBASICFit}.
}
\description{
This function clusters a state-space matrix.
}
\details{
The \code{method} argument determines what fitting method will be used. The default is 'SE-MC', where an E-M algorithm is used for clustering. If 'SE-HC', then hierarchical clustering is used.\cr
The \code{para} argument takes a list object that is supposed to include the following fields:
\tabular{ll}{
W \tab A K by (J*S) matrix. The (k,J*(s-1)+j)-th entry is the probability that the units in cluster j has state s in the k-th experiment.\cr
Z \tab An I by J matrix. The (i,j)-th entry is the indicator whether the i-th unit belongs to cluster j.\cr
non.id \tab A binary vector of length I. The i-th entry is the indicator whether the i-th unit does not belong to any cluster.
}
This argument is intended to carry the true parameters in simulation studies. If it is not null, then the model also computes a number of metrics that describes the error in model fitting. Users should be cautious that the order of the rows and columns of matrices in the fields of para should match the Y matrix.
}
\examples{
state.sim <- MBASIC.sim.state(I = 1000, K = 10, J = 4, S = 3, zeta = 0.1)
state.sim.fit <- MBASIC.state(Theta = state.sim$Theta, J = 4, method = "SE-MC", zeta = 0.1, maxitr = 100, tol = 1e-6)
}
\author{
Chandler Zuo \email{zuo@stat.wisc.edu}
}

