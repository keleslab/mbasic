#include "loglik.h"

SEXP loglik(SEXP _W, SEXP _P, SEXP _V, SEXP _zeta, SEXP _probz, SEXP _PDF, SEXP _factor, SEXP _statemap) {
	NumericMatrix PDF(_PDF); // MN by I
        NumericMatrix W(_W); // KS by J
	NumericMatrix P(_P); // I by S
	NumericMatrix V(_V); // N by M
	NumericVector factor(_factor); //length N
	NumericVector statemap(_statemap); // length M
	double zeta = as<double>(_zeta);
	NumericVector probz(_probz);//length J

	// extract the dimensions
	int S = P.ncol();
	int I = PDF.ncol();
	int M = V.ncol();
	int N = V.nrow();
	int K = W.nrow() / S;
	int J = probz.size();

	double loglik = 0;
	int i, j, k, s, n, m;

	double _LOW = 1e-10;

	//compute joint PDF
	NumericMatrix jointPDF(S * K, I);
	for(i = 0; i < I; i ++) {
		for(k = 0; k < K; k ++) {
			for(s = 0; s < S; s ++) {
				jointPDF(k + K * s, i) = 0;
				for(n = 0; n < N; n ++) {
					if(factor[n] == k) {
						int m1 = 0;
						while(statemap[m1] != s) {
							m1 ++;
						}
						double maxexp = PDF(n + N * m1, i);
						for(m = m1; m < M; m ++) {
							if(statemap[m] == s && PDF(n + N * m, i) > maxexp) {
								maxexp = PDF(n + N * m, i);
							}
						}
						double tmp = 0;
						for(m = 0; m < M; m ++) {
							if(statemap[m] == s) {
								tmp += V(n, m) * exp(PDF(n + N * m, i) - maxexp);
							}
						}
						if(tmp < _LOW) {
							tmp = _LOW;
						}
						jointPDF(k + K * s, i) += log(tmp) + maxexp;
					}
				}
			}
		}
	}

	for(i = 0; i < I; i ++){
		double exp1 = log(zeta);
		for(k = 0; k < K; k ++){ 
			double maxexp = jointPDF(k, i);
			for(s = 0; s < S; s ++){
				if(maxexp < jointPDF(k + K * s, i))
					maxexp = jointPDF(k + K * s, i);
			}
			double tmp = 0; 
			for(s = 0; s < S; s ++)
				tmp += P(i, s) * exp(jointPDF(k + K * s, i) - maxexp);
			exp1 += maxexp + log(tmp);
		}

		double tmp_exp[J];
		for(j = 0; j < J; j ++){
			tmp_exp[j] = log(probz[j]);// bug found to cause decreasing likelihood 
			for(k = 0; k < K; k ++){
				double maxexp = jointPDF(k, i);
				for(s = 0; s < S; s ++){
					if(maxexp < jointPDF(k + K * s, i))
						maxexp = jointPDF(k + K * s, i);
				}
				double tmp = 0; 
				for(s = 0; s < S; s ++)
					tmp += W(k + K * s, j) * exp(jointPDF(k + K * s, i) - maxexp);
				tmp_exp[ j ] += maxexp + log(tmp);
			}
		}
		double max_exp = tmp_exp[0];
		for(j = 1; j < J; j ++)
			if(tmp_exp[j] > max_exp)
				max_exp = tmp_exp[j];

		double exp2 = 0;
		for(j = 0; j < J; j ++)
			exp2 += exp(tmp_exp[j] - max_exp);

		exp2 = log(exp2) + max_exp;
		exp2 += log(1 - zeta);
		
		if(exp1 > exp2)
			loglik += exp1 + log(1 + exp(exp2 - exp1));
		else
			loglik += exp2 + log(1 + exp(exp1 - exp2));
	}

	return(wrap(loglik));
}
