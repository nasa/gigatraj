/******************************************************************************* 
***  Written by: 
***     L. R. Lait (NASA Ames Research Center, Code SG) 
***     Code 614 
***     NASA Goddard Space Flight Center 
***     Greenbelt, MD 20771 
*** 
***  Copyright (c) 2023 United States Government as represented by the Administrator of the National Aeronautics and Space Administration.  All Rights Reserved. 
*** 
*** Disclaimer:
*** No Warranty: THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY KIND, EITHER EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL CONFORM TO SPECIFICATIONS, ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM INFRINGEMENT, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL BE ERROR FREE, OR ANY WARRANTY THAT DOCUMENTATION, IF PROVIDED, WILL CONFORM TO THE SUBJECT SOFTWARE. THIS AGREEMENT DOES NOT, IN ANY MANNER, CONSTITUTE AN ENDORSEMENT BY GOVERNMENT AGENCY OR ANY PRIOR RECIPIENT OF ANY RESULTS, RESULTING DESIGNS, HARDWARE, SOFTWARE PRODUCTS OR ANY OTHER APPLICATIONS RESULTING FROM USE OF THE SUBJECT SOFTWARE.  FURTHER, GOVERNMENT AGENCY DISCLAIMS ALL WARRANTIES AND LIABILITIES REGARDING THIRD-PARTY SOFTWARE, IF PRESENT IN THE ORIGINAL SOFTWARE, AND DISTRIBUTES IT "AS IS." 
*** Waiver and Indemnity:  RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT.  IF RECIPIENT'S USE OF THE SUBJECT SOFTWARE RESULTS IN ANY LIABILITIES, DEMANDS, DAMAGES, EXPENSES OR LOSSES ARISING FROM SUCH USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING FROM, RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD HARMLESS THE UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT, TO THE EXTENT PERMITTED BY LAW.  RECIPIENT'S SOLE REMEDY FOR ANY SUCH MATTER SHALL BE THE IMMEDIATE, UNILATERAL TERMINATION OF THIS AGREEMENT. 
***  (Please see the NOSA_19110.pdf file for more information.) 
*** 
********************************************************************************/


// Unfinished

/*!
     Test utilities for gigatraj
*/

#include <stdlib.h>
#include "test_utils.hh"

using namespace gigatraj;

std::string datadir( std::string dir )
{
    std::string result;
    char *envdir;

    result = "";
    if ( dir != "" ) {
       envdir = getenv( dir.c_str() );
       if ( envdir != NULL ) {
          result = std::string( envdir ) + "/";
       }
    }

    return result;
}

   
int mismatch( gigatraj::real a, gigatraj::real b) 
{

    gigatraj::real diff;

    if ( ! (ISOK(a) && ISOK(b))  ) {
       return 1;
    }   
    
    diff = a - b;
    if (diff < 0.0 ) 
    {
       diff = - diff;
    }   

    if ( b < 0.0 )
    {
       b = - b;
    }
    if ( b > 0.0 ) 
    {   
       if ( diff/b*100.0 > 0.001 ) {
          return 1;
       } 
    } else {
       if ( diff > 0.0001 ) {
          return 1;
       } 
    }
   

    return 0;   

}

int mismatch( gigatraj::real a, gigatraj::real b, gigatraj::real tol ) 
{

    gigatraj::real diff;
    
    if ( ! (ISOK(a) && ISOK(b))  ) {
       return 1;
    }   
    
    diff = a - b;
    if (diff < 0.0 ) 
    {
       diff = - diff;
    }   

    if ( diff > tol ) {
       return 1;
    } 
    
    return 0;   

}

int mismatchdbl( double a, double b, double tol ) 
{

    double diff;
    
    if ( ! (ISOK(a) && ISOK(b))  ) {
       return 1;
    }   
    
    diff = a - b;
    if (diff < 0.0 ) 
    {
       diff = - diff;
    }   

    if ( diff > tol ) {
       return 1;
    } 
    
    return 0;   

}


void cmp_pos(std::vector<gigatraj::Parcel> &vflock0, std::vector<gigatraj::Parcel> &vflock
     , gigatraj::real* max_d, gigatraj::real* min_d, gigatraj::real *avg_d, gigatraj::real *med_d, gigatraj::real *sprd_d )
{

   real minx,maxx,tot,var;
   real median;
   int n = vflock0.size();

   // first, compare the sizes of the two vectors
   if ( n != vflock.size() ) {
      std::cerr << "Mismatch on vector sizes!" << std::endl;
      exit(1);
   } 
   

   minx =  1.0e15;
   maxx = -1.0e15;
   tot =  0.0;
   var = 0.0;
   median = 0.0;
   
   std::vector<Parcel>::iterator vit = vflock.begin();
   std::vector<Parcel>::iterator vit0 = vflock0.begin();
   
   // get the planet
   PlanetNav *e = vit0->getNav();
   
   // set up a vector for the differences
   std::list<real> difs;
     
   while ( vit0 != vflock0.end() ) {
      real lat,lon,lat0,lon0;
      
      vit0->getPos(&lon0,&lat0);
      vit->getPos(&lon,&lat);
   
      real dst = e->distance(lon0,lat0, lon,lat);
      
      if ( maxx < dst ) {
         maxx = dst;
      }   
      if ( minx > dst ) {
         minx = dst;
      }   
   
      tot = tot + dst;

      difs.push_back(dst);
   
      vit0++;
      vit++;
   }
   
   difs.sort();
   
   tot = tot / n;
   
   std::list<real>::iterator lit = difs.begin();
   int i=0;
   while ( lit != difs.end() ) {
   
      if ( i == (n/2) ) {
         median = *lit;
      }
      if ( ((n % 2) == 1) & ( i == (n/2+1) ) ) {
         median = ( median + *lit )/2.0;
      }       
   
      var = var + ( *lit - tot )*( *lit - tot );
   
      lit++;
      i++;
   }
   
   *max_d = maxx;
   *min_d = minx;
   *avg_d = tot;
   *med_d = median;
   *sprd_d = SQRT( var / (n-1) );

} 

