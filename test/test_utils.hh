#include <stdlib.h>     
#include <iostream>
#include <math.h>
#include <vector>
#include <list>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/Earth.hh"
#include "gigatraj/Parcel.hh"

std::string datadir( std::string dir = "" );

int mismatch( gigatraj::real a, gigatraj::real b);
int mismatch( gigatraj::real a, gigatraj::real b, gigatraj::real tol );
int mismatchdbl( double a, double b, double tol );


void cmp_pos(std::vector<gigatraj::Parcel> &vflock0
     , std::vector<gigatraj::Parcel> &vflock
     , gigatraj::real* max_d
     , gigatraj::real* min_d
     , gigatraj::real *avg_d
     , gigatraj::real *med_d
     , gigatraj::real *sprd_d );
 
