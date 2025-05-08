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
            
#include "config.h"

#include "gigatraj/SZAOTF.hh"

using namespace gigatraj;

SZAOTF::SZAOTF()
{
   quant = "solar_zenith_angle";
   uu = "degrees";

}

SZAOTF::~SZAOTF()
{

}

real SZAOTF::calc( real lon, real lat, std::string& time ) const
{
     real result;
     real lons[1];
     real lats[1];
     real* results;
     
     lons[0] = lon;
     lats[0] = lat;
     
     results = calc( lons, lats, 1, time );
     result = results[0];
     
     delete[] results;
     
     return result;
}

real SZAOTF::calc( real lon, real lat, double jday ) const
{
     real result;
     real lons[1];
     real lats[1];
     real* results;
     
     lons[0] = lon;
     lats[0] = lat;
     
     results = calc( lons, lats, 1, jday );
     result = results[0];
     
     delete[] results;
     
     return result;
}

double SZAOTF::modulus( double a, double b ) const
{
     double result;
     int n;
     
     b = fabs( b );
     
     n = a/b;
     
     result = a - n*b;
     
     return result;
}         

void SZAOTF::date_to_parts( std::string& time, int& year, int& month, int& dom, int& hr, int& min, double& sec) const
{
     int len;
     
     len = time.size();
     year = 0;
     month = 0;
     dom = 0;
     hr = 0;
     min = 0;
     sec = 0;
     
     if ( len >= 10 ) {
         // decode time
         for ( int i=0; i<4; i++ ) {
             year = year*10 + ( time[i] - '0');
         }
         for ( int i=5; i<7; i++ ) {
             month = month*10 + ( time[i] - '0');
         }
         for ( int i=8; i<10; i++ ) {
             dom = dom*10 + ( time[i] - '0');
         }
         if ( len > 11 ) {
            for ( int i=11; i<13; i++ ) {
                hr = hr*10 + ( time[i] - '0');
            }
            if ( len > 14 ) {
               for ( int i=14; i<16; i++ ) {
                   min = min*10 + ( time[i] - '0');
               }
               if ( len > 17 ) {
                  for ( int i=17; i<=18; i++ ) {
                      sec = sec*10 + ( time[i] - '0');
                  }
               }
            }
         }
     }
} 

double SZAOTF::julday( int year, int month, int dom, double hours ) const
{
     double result;
     int jy;
     int jm;
     int greg;
     int jul;
  
/*     
     //  Gregorian Calander was adopted on Oct. 15, 1582
     greg = 15 + 31 * (10 + 12 * 1582);
     
     
     if ( year < 0 ) {
        year++;
     }
     if ( month > 2 ) {
        jy = year;
        jm = month + 1;
     } else {
        jy = year - 1;
        jm = month + 13;
     }   
     
     jul = trunc(365.25 * jy) + trunc(30.6001 * jm) + dom + 1720995;
     if ( (dom + 31*(month + 12*year)) >= greg) {
        double ja = trunc( 0.01*jy );
	    jul = jul + 2 - ja + trunc( 0.25*ja );
	 }
     
*/
     
    ; https://en.wikipedia.org/wiki/Julian_day
    jul = ( 1461*( year + 4800 + (month - 14)/12))/4 
           + (367*(month - 2 - 12*((month - 14)/12)))/12 
       - (3*( ( year + 4900 + (month - 14)/12)/100 ))/4 
       + dom 
       - 32075;
 
     
     result = jul - 0.5000 + hours/24.0;
     
     return result;
     
}

double SZAOTF::julday( std::string& time ) const
{
     int year, month, day, hour, minute;
     double seconds;
     double result;
     double hrs;

     date_to_parts( time, year, month, day, hour, minute, seconds );
    
     hrs = hour + ( minute + seconds/60.0)/60.0;
 
     result = julday( year, month, day, hrs );
     
     return result;
}

real* SZAOTF::calc( real* lons, real* lats, int n, std::string& time ) const
{
    real* result;
    double jday;
    
    //date_to_parts( time, year, month, day, hour, minute, seconds );
    jday  = julday( time );
    
    result = calc( lons, lats, n, jday );
    
    return result;
}

real* SZAOTF::calc( real* lons, real* lats, int n, double jday ) const
{
    real* result;
    double dpi = 3.1415926535897931159979634685441851615905761718750;
    double rc = dpi/180.0;
    
    result = NULL;
    if ( n > 0 ) {
       result = new real[n];
       
       double jul2000 = jday - 2451545.0;
       double jcent = ( jul2000 )/36525.0;

       // mean solar longoutde
       double mnsollon = 280.46646 + jcent*( 36000.76983 + jcent*0.0003032 );
       mnsollon = modulus( mnsollon,  360.0 );

       // solar mean anomaly
       double solmnanom = 357.52911 + jcent*( 35999.05029 - 0.0001537*jcent );
       solmnanom = (modulus( solmnanom , 360.0))*rc;

       // obliquity of the ecliptic
       double MeanobliqEcliptic  = 23.0 + ( 26.0 + (( 21.448 - jcent*( 46.815 + jcent*( 0.00059 - jcent*0.001813 ) ) ))/60.0 )/60.0;
       double obliq  = ( MeanobliqEcliptic + 0.00256*cos( ( 125.04 - 1934.136*jcent)*rc ) )*rc;
       
       // eqn of center
       double cnt =  sin( solmnanom   )*( 1.914602 - jcent*( 0.004817 + 0.000014*jcent ) ) 
                   + sin( 2*solmnanom )*( 0.019993 - 0.000101*jcent ) 
                   + sin( 3*solmnanom )*0.000289;
       
       //  ecliptic longitude  
       double ecllon = (mnsollon + cnt - 0.00569 - 0.00478*sin( rc*( 125.04 - 1934.136*jcent ) ) )*rc;
       mnsollon = mnsollon*rc;
       
       // solar right ascension
       double rtasc = ( atan2( cos(obliq)*sin(ecllon), cos(ecllon) ) )/rc;

       // solar declination
       double soldecl = ( asin( sin(obliq)*sin(ecllon) ) )/rc;

       double eccentorbit =  0.016708634 - jcent*( 0.000042037 + 0.0000001267*jcent );

       double stanom = solmnanom + cnt*rc;

       double sunradv = (1.000001018*( 1 - eccentorbit*eccentorbit) )/( 1.0 + eccentorbit*cos( stanom ) );

       double vary = tan( obliq/2.0);
       vary = vary*vary;

       double eqtime =  4.0/rc*( vary*sin(2.0*mnsollon) 
                                    - 2.0*eccentorbit*sin(solmnanom) 
                                    + 4.0*eccentorbit*vary*sin(solmnanom)*cos(2.0*mnsollon) 
                                    - 0.5*vary*vary*sin(4.0*mnsollon) 
                                    - 1.25*eccentorbit*eccentorbit*sin(2.0*solmnanom) );
       
       double dayfrac = jday - ( floor( jday - 0.5 ) + 0.5 );
       
       double soltime;
       double hrang;
       double sza;
       
       // for each lon and lat...
       for ( int i=0 ; i < n; i++ ) {
           
           soltime = dayfrac*1440.0 + eqtime + 4.0*lons[i];
           soltime = modulus( soltime, 1440.0 );
           if ( soltime < 0 ) { 
               soltime = soltime + 1440.0;
           }
           hrang = soltime/4.0 - 180.0;
           if ( soltime < 0 ) {
              hrang = soltime/4.0 + 180.0;
           }
           
           sza = acos( sin(lats[i]*rc)*sin(soldecl*rc) 
                     + cos(lats[i]*rc)*cos(soldecl*rc)*cos(hrang*rc) 
                     )/rc;
           
           result[i] = sza;
       }
    }

    return result;

}


GridField3D* SZAOTF::calc( const GridField3D& input, int flags ) const
{
   // the output SZA
   GridField3D *result;
   // indices into the data array
   int* is;
   int* js;
   int* ks;
   // dimensions of the data
   int ni, nj, nk;
   // gridpoint longitudes and latitudes
   real* lons;
   real* lats;
   real* levs;
   // julian day
   double jday;
   // calendar time
   std::string ctime;
   // the computed SZAs
   real* szas;
   

   // use the input quantity as a base for the result
   result = input.duplicate();

   // change the quantity and units to SZA
   result->set_quantity(quant);
   result->set_units(uu);
   
   ctime =  input.met_time();
   
   jday = julday( ctime );

   input.dims( &ni, &nj, &nk );
   
   /* We'll do one 1st-dim row at a time.
      This seems ot be a good compromise between
      doing all the horizontal gridpoints at cone 
      (which might use a lot of memmory),
      and computing each gridpont
      individually (which might be too slow).
    */ 
   
   is = new int[ni];
   for ( int i=0; i < ni; i++ ) {
      is[i] = i;
   }
   
   js = new int[ni];

   ks = new int[ni];
   for ( int i=0; i < ni; i++ ) {
      ks[i] = 0;
   }
   
   lons = new real[ni];
   lats = new real[ni];
   levs = new real[ni];
   
   // for each of the second dimension...
   for ( int j=0; j < nj; j++ ) {
       for ( int i=0; i < ni; i++ ) {
           js[i] = j;
       }
   
       // now turn these indices into lats and lons into 
       input.gridcoords( ni, is, js, ks, lons, lats, levs );
   
       // compute
       szas = calc( lons, lats, ni, jday );     
   
       // store
       for ( int i=0; i < ni ; i++ ) {
           for ( int k=0; k < nk; k++ ) {
               (*result)( i, j, k ) = szas[i];
           }
       }
       
       delete[] szas;
   }
   
   delete[] levs;
   delete[] lats;
   delete[] lons;
   delete[] ks;
   delete[] js;
   delete[] is;

   return result;

}


GridFieldSfc* SZAOTF::calc( const GridFieldSfc& input, int flags) const
{
   // the output SZA
   GridFieldSfc *result;
   // indices into the data array
   int* is;
   int* js;
   // dimensions of the data
   int ni, nj;
   // gridpoint longitudes and latitudes
   real* lons;
   real* lats;
   // julian day
   double jday;
   // calendar time
   std::string ctime;
   // the computed SZAs
   real* szas;
   

   // use the input quantity as a base for the result
   result = input.duplicate();

   // change the quantity and units to SZA
   result->set_quantity(quant);
   result->set_units(uu);
   
   ctime =  input.met_time();
   
   jday = julday( ctime );

   input.dims( &ni, &nj );
   
   /* We'll do one 1st-dim row at a time.
      This seems ot be a good compromise between
      doing all the horizontal gridpoints at cone 
      (which might use a lot of memmory),
      and computing each gridpont
      individually (which might be too slow).
    */ 
   
   is = new int[ni];
   for ( int i=0; i < ni; i++ ) {
      is[i] = i;
   }
   
   js = new int[ni];

   lons = new real[ni];
   lats = new real[ni];
   
   // for each of the second dimension...
   for ( int j=0; j < nj; j++ ) {
       for ( int i=0; i < ni; i++ ) {
           js[i] = j;
       }
   
       // now turn these indices into lats and lons into 
       input.gridcoords( ni, is, js, lons, lats );
   
       // compute
       szas = calc( lons, lats, ni, jday );     
   
       // store
       for ( int i=0; i < ni ; i++ ) {
           (*result)( i, j ) = szas[i];
       }
       
       delete[] szas;
   }
   
   delete[] lats;
   delete[] lons;
   delete[] js;
   delete[] is;

   return result;
}

