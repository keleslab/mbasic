#include "e_step.h"

SEXP e_step1(SEXP _W, SEXP _P, SEXP _probz, SEXP _PDF) {

	// the following parameters are inputs and are not updated
	NumericMatrix PDF(_PDF);

	// the following parameters serve both as input, but will be updated in M step as output
        NumericMatrix W(_W);
	NumericMatrix P(_P);
	NumericVector probz(_probz);
	
	// extract the dimensions
	int I = P.nrow();
	int S = P.ncol();
	int K = W.nrow();
	int J = probz.size();

	double _LOW = 1e-10;

	// the following quantities are outputs
	NumericMatrix Theta_mean(K, I * S);
	NumericVector b_mean(I);
	NumericMatrix Theta_b(I, S);
	NumericVector Z_mean(J);
	NumericMatrix W_max(K, J*S);
	NumericMatrix predZ(I, J);

	// iterators
	int i, j, k, s;//, likid;

	// Intermediate matrices
	NumericMatrix FP(K, I);
	NumericMatrix FW(K, I * J);
	NumericMatrix Zcond(I, J);
	
	double tmpexp[S], maxexp;
	for(k = 0; k < K; k ++){
		for(i = 0; i < I; i ++){
			for(j = 0; j < J; j ++){
				FW(k, i + j * I) = 0;
				for(s = 0; s < S; s ++){
					if(W(k, j + J * s) < _LOW)
						W(k, j + J * s) = _LOW;
					tmpexp[s] = PDF(k, i + I * s) + log(W(k, j + J * s));
				}
				maxexp = tmpexp[0];
				for(s = 1; s < S; s ++)
					if(maxexp < tmpexp[s])
						maxexp = tmpexp[s];
				for(s = 0; s < S; s ++)
					FW(k, i + j * I) += exp(tmpexp[s] - maxexp);
				FW(k, i + j * I) = log(FW(k, i + j * I)) + maxexp;
			}
		}
	}
	
	for(j = 0; j < J; j ++)
		Z_mean(j) = 0;

	// compute b, Z
	for(i = 0; i < I; i ++){

		double exp1,exp0;
		// numerator

		// denominator
		double tmpexp[J];
		for(j = 0; j < J; j ++){
			tmpexp[j] = log(probz(j));
			for(k = 0; k < K; k ++)
				tmpexp[j] += FW(k, i + I * j);
		}
		double maxexp = tmpexp[0];
		for(j = 1; j < J; j ++)
			if(tmpexp[j] > maxexp)
				maxexp = tmpexp[j];
		exp0 = 0;
		for(j = 0; j < J; j ++)
			exp0 += exp(tmpexp[j] - maxexp);
		exp0 = log(exp0);
		exp0 += maxexp;

		for(j = 0; j < J; j ++){
			predZ(i, j) = exp(tmpexp[j] - exp0);
			Z_mean(j) += predZ(i, j);
		}

	}


	for(i = 0; i < I; i ++) {
		b_mean(i) = 0;
		for(s = 0; s < S; s ++)
			Theta_b(i, s) = 0;
	}

	for(k = 0; k < K; k ++)
		for(j = 0; j < J; j ++)
			for(s = 0; s < S; s ++)
				W_max(k, j + J * s) = 0;

	for(i = 0; i < I; i ++){
		for(k = 0; k < K; k ++){
			for(s = 0; s < S; s ++){
				for(j = 0; j < J; j ++){
					double tmp2 = predZ(i, j) * exp(PDF(k, i + I * s)  - FW(k, i + I * j)) *  W(k, j + J * s);
					Theta_mean(k, i + I * s) += tmp2;
					W_max(k, j + J * s) += tmp2;
				}
				
				if(Theta_mean(k, i + I * s) < _LOW)
					Theta_mean(k, i + I * s) = _LOW;
				else if(Theta_mean(k, i + I * s) > 1 - _LOW)
					Theta_mean(k, i + I * s) = 1 - _LOW;
				
			}
		}
	}

	for(j = 0; j < J; j ++){
		Z_mean(j) /= I;
		if(Z_mean(j) < _LOW)
			Z_mean(j) = _LOW;
		else if(Z_mean(j) > 1 - _LOW)
			Z_mean(j) = 1 - _LOW;
	}

	Rcpp::List ret = Rcpp::List::create(
					    Rcpp::Named("P") = Theta_b,
					    Rcpp::Named("zeta") = 0,
					    Rcpp::Named("probz") = Z_mean,
					    Rcpp::Named("W") = W_max,
					    Rcpp::Named("Theta_mean") = Theta_mean,
					    Rcpp::Named("b_prob") = b_mean,
					    Rcpp::Named("predZ") = predZ
					    //,Rcpp::Named("oldlik") = oldlik
					   );
	
	return(ret);
	
}