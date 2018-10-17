void decision_error( double *error , double *ok_error){
	for( int run = 0 ; run < 6 ; run++){
		if( fabs(error[run]) < ok_error[run]) error[run] = 0 ;
	}
}
