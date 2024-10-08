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


/*!
     Test program for the Earth class
*/

#include <stdlib.h>     
#include <iostream>
#include <math.h>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/Earth.hh"

#include "test_utils.hh"

using namespace gigatraj;
using std::cerr;
using std::endl;

int main() 
{

    Earth e;
    real lon, lonw;
    real lat;
    real dist;
    int status;
    int n;
    real *lats, *lats2;
    real *lons, *lons2;
    real *targetlats, *targetlats_exact, *targetlats_apprx, *targetlats_crude;
    real *targetlons, *targetlons_exact, *targetlons_apprx, *targetlons_crude;
    real *dists, *targetdists;
    real *dlons, *dlats;
    real *dxs, *dys;
    real *bearings;
    real *vxIn, *vxOut, *vxTarget;
    real *vyIn, *vyOut, *vyTarget;
    real vx, vy;
    real lonlimit;
    int quality, qual;
    
    
    if ( e.id != string("Earth") )
    {
       cerr << "Bad ID" << endl;
       exit(1);
    }   
    
    // =========================  method wrap
    
    // simple case--should not wrap
    lonw = e.wrap(40.0);
    if ( mismatch(  lonw, 40.0)   ) 
    {
       cerr << "Bad wrap 40->" << lonw << endl;
       exit(1);
    }   
    // also should not wrap
    lonw = e.wrap(90.0);
    if ( mismatch(  lonw, 90.0)   ) 
    {
       cerr << "Bad wrap 90->" << lonw << endl;
       exit(1);
    }   
    // should wrap to 0
    lonw = e.wrap(360.0);
    if ( mismatch( lonw, 0.0) ) 
    {
       cerr << "Bad wrap 360.0 -> " << lonw << endl;
       exit(1);
    }   
    
    e.wrappingLongitude( -77.7 );
    lonlimit = e.wrappingLongitude();
    if ( mismatch( lonlimit, -77.7 ) ) { 
       cerr << "Failed to set wrappinglongitude to -77.7. Value is " << lonlimit << endl;
       exit(1);
    }   

    e.wrappingLongitude( -180.0 );
    
    // should not wrap
    lonw = e.wrap(-40.0);
    if ( mismatch( lonw, -40.0)  ) 
    {
       cerr << "Bad wrap -40 -> " << lonw << endl;
       exit(1);
    }   
    // edge case; should not wrap
    lonw = e.wrap(-180.0);
    if ( mismatch( lonw, -180.0)  ) 
    {
       cerr << "Bad wrap -180 -> " << lonw << endl;
       exit(1);
    }   
    // should wrap
    lonw = e.wrap(270.0);
    if ( mismatch( lonw, -90.0)  ) 
    {
       cerr << "Bad wrap 270 -> " << lonw << endl;
       exit(1);
    } 
    // edge case; should wrap  
    lonw = e.wrap(180.0);
    if ( mismatch( lonw, -180.0)  ) 
    {
       cerr << "Bad wrap 180 -> " << lonw << endl;
       exit(1);
    }   
    // not-quite-edge case; should not wrap
    lonw = e.wrap(179.999);
    if ( mismatch( lonw, 179.999) ) 
    {
       cerr << "Bad wrap 179.999 -> " << lonw << endl;
       exit(1);
    }   
    
    e.wrappingLongitude( 0.0 );

    // should wrap
    lonw = e.wrap(-40.0);
    if ( mismatch( lonw, 320.0)  ) 
    {
       cerr << "Bad wrap -40 -> " << lonw << endl;
       exit(1);
    }   
    // should wrap
    lonw = e.wrap(-180.0);
    if ( mismatch( lonw, 180.0)  ) 
    {
       cerr << "Bad wrap -180 -> " << lonw << endl;
       exit(1);
    }   
    // should not wrap
    lonw = e.wrap(270.0);
    if ( mismatch( lonw, 270.0)  ) 
    {
       cerr << "Bad wrap 270 -> " << lonw << endl;
       exit(1);
    }   
    // should not wrap
    lonw = e.wrap(180.0);
    if ( mismatch( lonw, 180.0)  ) 
    {
       cerr << "Bad wrap 180 -> " << lonw << endl;
       exit(1);
    }   
    // not-quite-edge case; should not wrap
    lonw = e.wrap(359.999);
    if ( mismatch( lonw, 359.999) ) 
    {
       cerr << "Bad wrap 359.999 -> " << lonw << endl;
       exit(1);
    }   



    // ===========================  method checkpos
    // position OK
    try {
       e.checkpos( 30.0, 40.0 );
    } catch ( Earth::badlocation err) {
       cerr << "Bad location check (30.0, 40.0) " << endl;
       exit(1);  
    }   
    // edge case; position OK
    try {
       e.checkpos( 30.0, 90.0 );
    } catch ( Earth::badlocation err) {
       cerr << "Bad location check (30.0, 90.0) " << endl;
       exit(1);  
    }   
    // edge case; position OK
    try {
       e.checkpos( 30.0, -90.0 );
    } catch ( Earth::badlocation err) {
       cerr << "Bad location check (30.0, -90.0) " << endl;
       exit(1);  
    }   
    // lots of wrapping; postion OK
    try {
       e.checkpos( 800.0, -90.0 );
    } catch ( Earth::badlocation err) {
       cerr << "Bad location check (800.0, -90.0) " << endl;
       exit(1);  
    }   
    // lots of wrapping; position OK
    try {
       e.checkpos( -800.0, -90.0 );
    } catch ( Earth::badlocation err) {
       cerr << "Bad location check (-800.0, -90.0) " << endl;
       exit(1);  
    } 
    // No reason this position should be considered bad, but you never know...
    try {
       e.checkpos( 0.0, 0.0 );
    } catch ( Earth::badlocation err) {
       cerr << "Bad location check (0.0, 0.0) " << endl;
       exit(1);  
    } 


    // position validity tests using NaNs 
#ifdef USE_DOUBLE    
    lon = nan("bang");
    lat = nan("boom");
#else
    lon = nanf("bang");
    lat = nanf("boom");
#endif
    // longitude is a NaN
    status = 1;
    try {
       e.checkpos( lon, -30.0 );
    } catch ( Earth::badlocation err) {
       // we should catch the exception, if all works well
       status = 0;
    } 
    if ( status ) {
       cerr << "Bad location check (NaN, -30.0) " << endl;
       exit(1);  
    }
    // latitude is a NaN
    status = 1;
    try {
       e.checkpos( 20.0, lat );
    } catch ( Earth::badlocation err) {
       // we should ctach the exception, if all works well
       status = 0;
    } 
    if ( status ) {
       cerr << "Bad location check (20.0, NaN) " << endl;
       exit(1);  
    }
    // both longitude and latitude are NaNs
    status = 1;
    try {
       e.checkpos( lon, lat );
    } catch ( Earth::badlocation err) {
       // we should ctach the exception, if all works well
       status = 0;
    } 
    if ( status ) {
       cerr << "Bad location check (NaN, NaN) " << endl;
       exit(1);  
    }
    // latitude is out of range
    status = 1;
    try {
       e.checkpos( 30.0, 95.0 );
    } catch ( Earth::badlocation err) {
       // we should ctach the exception, if all works well
       status = 0;
    } 
    if ( status ) {
       cerr << "Bad location check (30.0,95.0) " << endl;
       exit(1);  
    }
    // latitude is out of range
    status = 1;
    try {
       e.checkpos( 30.0, -95.0 );
    } catch ( Earth::badlocation err) {
       // we should ctach the exception, if all works well
       status = 0;
    } 
    if ( status ) {
       cerr << "Bad location check (30.0,-95.0) " << endl;
       exit(1);  
    }


    // =============================   make sure that conformal adjusts are off (for now)
    e.conformal( 0 );
    if ( e.conformal() != 0 ) {
       cerr << "Unable to disable conformal adjustments " << endl;
       exit(1);  
    }

    // =============================   method deltapos

    n = 4;
    lons = new real[n];
    lats = new real[n];
    targetlons = new real[n];
    targetlats = new real[n];
    dlons = new real[n];
    dlats = new real[n];
    
    // simple case
    lons[0] = 10.0;
    lats[0] = 20.0;
    dlons[0] = 5.0;
    dlats[0] = 10.0;
    targetlons[0] = 15.0;
    targetlats[0] = 30.0;
    
    e.wrappingLongitude( -180.0 );
    
    // increment across longitude wrapping point
    lons[1] = 178.0;
    lats[1] = 20.0;
    dlons[1] = 5.0;
    dlats[1] = 10.0;
    targetlons[1] = -177.0;
    targetlats[1] = 30.0;
    // increment across North pole
    lons[2] = 45.0;
    lats[2] = 89.0;
    dlons[2] = 5.0;
    dlats[2] = 3.0;
    targetlons[2] = -130.0;
    targetlats[2] = 88.0;
    // increment across South pole
    lons[3] = 45.0;
    lats[3] = -89.0;
    dlons[3] = 5.0;
    dlats[3] = -3.0;
    targetlons[3] = -130.0;
    targetlats[3] = -88.0;

    e.wrappingLongitude( 0.0 );

    // increment across longitude wrapping point
    lons[1] = 358.0;
    lats[1] = 20.0;
    dlons[1] = 5.0;
    dlats[1] = 10.0;
    targetlons[1] = 3.0;
    targetlats[1] = 30.0;
    // increment across North pole
    lons[2] = 45.0;
    lats[2] = 89.0;
    dlons[2] = 5.0;
    dlats[2] = 3.0;
    targetlons[2] = 230.0;
    targetlats[2] = 88.0;
    // increment across South pole
    lons[3] = 45.0;
    lats[3] = -89.0;
    dlons[3] = 5.0;
    dlats[3] = -3.0;
    targetlons[3] = 230.0;
    targetlats[3] = -88.0;

    
    for ( int i=0; i<n; i++ ) {
        try {
           lon = lons[i];
           lat = lats[i];
           e.deltapos( &lon, &lat, dlons[i], dlats[i] );
        } catch ( Earth::badincrement err) {
           cerr << "Bad deltapos() increment failed at i=" << i << " ("
           << "expecting ( " << targetlons[i] << ", " << targetlats[i] << ")" << endl;
           exit(1);  
        }
        if ( mismatch(lon, targetlons[i]) || mismatch(lat,targetlats[i]) ) {
           cerr << "Bad deltapos() increment check at i=" << i << " ("
           << "expecting ( " << targetlons[i] << ", " << targetlats[i] << "), got ("
           << lon << ", " << lat << ")" << endl;
           exit(1);  
        } 
    }
     
    // test the array version
    try {
       e.deltapos( n, lons, lats, dlons, dlats );
    } catch ( Earth::badincrement err) {
       cerr << "array: Bad deltapos() increment failed " << endl;
       exit(1);  
    }
    for ( int i=0; i<n; i++ ) {
       if ( mismatch(lons[i], targetlons[i]) || mismatch(lats[i],targetlats[i]) ) {
          cerr << "array: Bad deltapos() increment check at i=" << i << " ("
          << "expecting ( " << targetlons[i] << ", " << targetlats[i] << "), got ("
          << lons[i] << ", " << lats[i] << ")" << endl;
          exit(1);  
       } 
    }
    delete[] dlats;
    delete[] dlons;
    delete[] targetlats;
    delete[] targetlons;
    delete[] lats;
    delete[] lons;


    // =============================   method deltaxy
    n = 4;
    lons = new real[n];
    lats = new real[n];
    targetlons = new real[n];
    targetlats = new real[n];
    dxs = new real[n];
    dys = new real[n];

    // simple equatorial case, along longitude
    lons[0] = 10.0;
    lats[0] =  0.0;
    dxs[0] = 100.0;
    dys[0] =   0.0;
    targetlons[0] = 10.8993;
    targetlats[0] = 0.0;
    // simple equatorial case, along latitude
    lons[1] = 10.0;
    lats[1] =  20.0;
    dxs[1] = 0.0;
    dys[1] =  100.0;
    targetlons[1] = 10.0;
    targetlats[1] = 20.8993;
    // simple non-equatorial case, along longitude
    lons[2] = 40.0;
    lats[2] =  -20.0;
    dxs[2] = 100.0;
    dys[2] =  0.0;
    //targetlons[2] = 40.9572;
    //targetlats[2] = -20.0;
    // note: following great circle path, not the latitude circle
    targetlons[2] = 40.9571;
    targetlats[2] = -19.9977;
    // typical arbitrary mid-latitude case
    lons[3] = 97.0;
    lats[3] =  -43.4;
    dxs[3] = 56.0;
    dys[3] =  -12.3;
    targetlons[3] = 97.6931;
    targetlats[3] = -43.5106;


    for ( int i=0; i<n; i++ ) {
        lon = lons[i];
        lat = lats[i];
        try {
           e.deltaxy( &lon, &lat, dxs[i], dys[i] );
        } catch ( Earth::badincrement err) {
           cerr << "Bad xy deltaxy() increment at i=" << i 
           << ": expected (" << targetlons[i] << ", " << targetlats[i] << ")"  << endl;
           exit(1);  
        }
        if ( mismatch(lon, targetlons[i]) || mismatch(lat,targetlats[i]) ) {
           cerr << "Bad deltaxy() increment check at i=" << i << " ("
           << "expecting ( " << targetlons[i] << ", " << targetlats[i] << "), got ("
           << lon << ", " << lat << ")" << endl;
           exit(1);  
        } 
        
    }

    // try the array version
    try {
       e.deltaxy( n, lons, lats, dxs, dys );
    } catch ( Earth::badincrement err) {
       cerr << "array: Bad xy increment check " << endl;
       exit(1);  
    }
    for ( int i=0; i<n; i++ ) {
        if ( mismatch(lons[i], targetlons[i]) || mismatch(lats[i],targetlats[i]) ) {
           cerr << "array: Bad xy increment check at i=" << i << " ("
           << "expecting ( " << targetlons[i] << ", " << targetlats[i] << "), got ("
           << lons[i] << ", " << lats[i] << ")" << endl;
           exit(1);  
        }
    }
    
    delete[] dys;
    delete[] dxs;
    delete[] targetlats;
    delete[] targetlons;
    delete[] lats;
    delete[] lons;


    // =============================   method distance
    n = 3;
    lons = new real[n];
    lats = new real[n];
    lons2 = new real[n];
    lats2 = new real[n];
    dists = new real[n];
    targetdists = new real[n];

    lons[0] = 40.0;
    lats[0] = 23.4;
    lons2[0] = 57.0;
    lats2[0] = 34.9;
    targetdists[0] = 2082.82;
    
    lons[1] = 0.0;
    lats[1] = 0.0;
    lons2[1] = 180.0;
    lats2[1] =  0.0;
    targetdists[1] = 20015.0;

    lons[2] = 40.0;
    lats[2] = 90.0;
    lons2[2] = 40.0;
    lats2[2] =  -90.0;
    targetdists[2] = 20015.0;

    for ( int i=0; i<n; i++ ) {
        try {
           dist = e.distance(lons[i], lats[i], lons2[i], lats2[i]);
        } catch (...) {
           cerr << "Bad distance check " << endl;
           exit(1);  
        }
        if ( mismatch(  dist, targetdists[i] )   ) 
        {
           cerr << "Bad distance at i=" << i << ": got " << dist << " instead of " << targetdists[i] << endl;
           exit(1);
        }   
    }
    
    // now the array version
    try {
       e.distance( n, lons, lats, lons2, lats2, dists );
    } catch (...) { 
        cerr << "array: Bad distance check " << endl;
        exit(1);  
    }
    for ( int i=0; i<n; i++ ) {
        if ( mismatch(  dists[i], targetdists[i] )   ) 
        {
           cerr << "array: Bad distance at i=" << i << ": got " << dists[i] << " instead of " << targetdists[i] << endl;
           exit(1);
        }    
    }
    
    delete[] targetdists;
    delete[] dists;
    delete[] lats2;
    delete[] lons2;
    delete[] lats;
    delete[] lons;


    // =============================   method radius

    dist = e.radius();
    if ( mismatch(  dist, 6371.0 )   ) 
    {
       cerr << "Bad earth radius 6371.0 ->" << dist << endl;
       exit(1);
    }   


    // =============================   method displace
    n = 12;
    lons = new real[n];
    lats = new real[n];
    dists = new real[n];
    bearings = new real[n];
    targetlons = new real[n];
    targetlats = new real[n];
    lons2 = new real[n];
    lats2 = new real[n];
    
    // from equator, in all four directions
    lons[0] = 45.0;
    lats[0] = 0.0;
    dists[0] = 111.0;
    bearings[0] = 0.0;
    targetlons[0] = 45.000;
    targetlats[0] = 0.998245;

    lons[1] = 45.0;
    lats[1] = 0.0;
    dists[1] = 111.0;
    bearings[1] = 90.0;
    targetlons[1] = 45.9982;
    targetlats[1] = 0.0;

    lons[2] = 45.0;
    lats[2] = 0.0;
    dists[2] = 111.0;
    bearings[2] = 180.0;
    targetlons[2] = 45.000;
    targetlats[2] = -0.998245;
    
    lons[3] = 45.0;
    lats[3] = 0.0;
    dists[3] = 111.0;
    bearings[3] = -90.0;
    targetlons[3] = 44.0018;
    targetlats[3] = 0.0;
    
    // from north pole, in all four directions
    lons[4] = 45.0;
    lats[4] = 90.0;
    dists[4] = 111.0;
    bearings[4] = 0.0;
    if ( e.wrappingLongitude() != 0.0 ) {
        targetlons[4] = -135.0;
    } else {
        targetlons[4] = 225.0;
    }
    targetlats[4] = 89.0018;

    lons[5] = 45.0;
    lats[5] = 90.0;
    dists[5] = 111.0;
    bearings[5] = 90.0;
    targetlons[5] = 135.0;
    targetlats[5] = 89.0018;

    lons[6] = 45.0;
    lats[6] = 90.0;
    dists[6] = 111.0;
    bearings[6] = 180.0;
    targetlons[6] = 45.000;
    targetlats[6] = 89.0018;
    
    lons[7] = 45.0;
    lats[7] = 90.0;
    dists[7] = 111.0;
    bearings[7] = -90.0;
    if ( e.wrappingLongitude() != 0.0 ) {
        targetlons[7] = -45.0;
    } else {
        targetlons[7] = 315.0;
    }
    targetlats[7] = 89.0018;
    
    // from south pole, in all four directions
    lons[8] = 45.0;
    lats[8] = -90.0;
    dists[8] = 111.0;
    bearings[8] = 0.0;
    targetlons[8] = 45.0;
    targetlats[8] = -89.0018;

    lons[9] = 45.0;
    lats[9] = -90.0;
    dists[9] = 111.0;
    bearings[9] = 90.0;
    targetlons[9] = 135.0;
    targetlats[9] = -89.0018;

    lons[10] = 45.0;
    lats[10] = -90.0;
    dists[10] = 111.0;
    bearings[10] = 180.0;
    if ( e.wrappingLongitude() != 0.0 ) {
        targetlons[10] = -135.0;
    } else {
        targetlons[10] = 225.000;
    }
    targetlats[10] = -89.0018;
    
    lons[11] = 45.0;
    lats[11] = -90.0;
    dists[11] = 111.0;
    bearings[11] = -90.0;
    if ( e.wrappingLongitude() != 0.0 ) {
        targetlons[11] = -45.0;
    } else {
        targetlons[11] = 315.0;
    }
    targetlats[11] = -89.0018;

    for ( int i=0; i<n; i++ ) {
        try {
            e.displace( lons[i], lats[i], dists[i], bearings[i], lon, lat );
        } catch (...) {
           cerr << "Bad displace check " << endl;
           exit(1);  
        }
        if ( mismatch( lon, targetlons[i] ) || mismatch( lat, targetlats[i] )  ) {
           cerr << "Bad displace() at i=" << i << ": got (" <<  lon << ", " << lat << ") "
           << " instead of (" << targetlons[i] << ", " << targetlats[i] << ")" << endl;
           exit(1);
        }   
    }
    // array version
    try {
        e.displace( n, lons, lats, dists, bearings, lons2, lats2 );
    } catch (...) {
       cerr << "array: Bad displace check " << endl;
       exit(1);  
    }
    for ( int i=0; i<n; i++ ) {
        if ( mismatch( lons2[i], targetlons[i] ) || mismatch( lats2[i], targetlats[i] )  ) 
        {
           cerr << "array: Bad displace() at i=" << i << ": got (" <<  lons2[i] << ", " << lats2[i] << ") "
           << " instead of (" << targetlons[i] << ", " << targetlats[i] << ")" << endl;
           exit(1);
        }   
    }
    
    delete[] lats2;
    delete[] lons2;
    delete[] targetlats;
    delete[] targetlons;
    delete[] bearings;
    delete[] dists;
    delete[] lats;
    delete[] lons;
    

    // =============================   vector relocations
    n = 12;
    lons = new real[n];
    lats = new real[n];
    lons2 = new real[n];
    lats2 = new real[n];
    vxIn = new real[n];
    vxOut = new real[n];
    vxTarget = new real[n];
    vyIn = new real[n];
    vyOut = new real[n];
    vyTarget = new real[n];

    // vector at equator; four directions
    lons[0] = 45.0;
    lats[0] = 0.0;
    vxIn[0] = 0.0;
    vyIn[0] = 10.0;
    lons2[0] = 105.0;
    lats2[0] = lats[0];
    vxTarget[0] = 0.0;
    vyTarget[0] = 10.0;

    lons[1] = 45.0;
    lats[1] = 0.0;
    vxIn[1] = 10.0;
    vyIn[1] = 0.0;
    lons2[1] = 105.0;
    lats2[1] = lats[1];
    vxTarget[1] = 10.0;
    vyTarget[1] =  0.0;

    lons[2] = 45.0;
    lats[2] = 0.0;
    vxIn[2] = 0.0;
    vyIn[2] = -10.0;
    lons2[2] = 105.0;
    lats2[2] = lats[2];
    vxTarget[2] = 0.0;
    vyTarget[2] = -10.0;

    lons[3] = 45.0;
    lats[3] = 0.0;
    vxIn[3] = -10.0;
    vyIn[3] =  0.0;
    lons2[3] = 105.0;
    lats2[3] = lats[3];
    vxTarget[3] = -10.0;
    vyTarget[3] =  0.0;

    // vector near north pole; four directions
    lons[4] = 0.0;
    lats[4] = 89.99;
    vxIn[4] = 0.0;
    vyIn[4] = 10.0;
    lons2[4] =  30.0;
    lats2[4] = lats[4];
    vxTarget[4] = -5.0;
    vyTarget[4] =  8.66025;

    lons[5] = 0.0;
    lats[5] = 89.99;
    vxIn[5] = 10.0;
    vyIn[5] = 0.0;
    lons2[5] = 30.0;
    lats2[5] = lats[5];
    vxTarget[5] = 8.66025;
    vyTarget[5] =  5.0;

    lons[6] = 0.0;
    lats[6] = 89.99;
    vxIn[6] = 0.0;
    vyIn[6] = -10.0;
    lons2[6] = 30.0;
    lats2[6] = lats[6];
    vxTarget[6] = 5.0;
    vyTarget[6] = -8.66025;

    lons[7] = 0.0;
    lats[7] = 89.99;
    vxIn[7] = -10.0;
    vyIn[7] =  0.0;
    lons2[7] = 30.0;
    lats2[7] = lats[7];
    vxTarget[7] = -8.66025;
    vyTarget[7] =  -5.0;

    // vector near south pole; four directions
    lons[8] = 0.0;
    lats[8] = -89.99;
    vxIn[8] = 0.0;
    vyIn[8] = 10.0;
    lons2[8] =  30.0;
    lats2[8] = lats[8];
    vxTarget[8] =  5.0;
    vyTarget[8] =  8.66025;

    lons[9] = 0.0;
    lats[9] = -89.99;
    vxIn[9] = 10.0;
    vyIn[9] = 0.0;
    lons2[9] = 30.0;
    lats2[9] = lats[9];
    vxTarget[9] = 8.66025;
    vyTarget[9] =  -5.0;

    lons[10] = 0.0;
    lats[10] = -89.99;
    vxIn[10] = 0.0;
    vyIn[10] = -10.0;
    lons2[10] = 30.0;
    lats2[10] = lats[10];
    vxTarget[10] =  -5.0;
    vyTarget[10] = -8.66025;

    lons[11] = 0.0;
    lats[11] = -89.99;
    vxIn[11] = -10.0;
    vyIn[11] =  0.0;
    lons2[11] = 30.0;
    lats2[11] = lats[11];
    vxTarget[11] = -8.66025;
    vyTarget[11] =  5.0;

    e.conformal( 0 );
    if ( e.conformal() != 0 ) {
       cerr << "Unable to disable conformal adjustments by rotation" << endl;
       exit(1);  
    }
    for ( int i=0; i<n; i++ ) {
        try {
            vx = vxIn[i];
            vy = vyIn[i];
            e.vRelocate( lons2[i], lats2[i], lons[i], lats[i], &vx, &vy );
        } catch (...) {
           cerr << "Bad vRelocate check w/o adjustment " << endl;
           exit(1);  
        }
        if ( mismatch( vx, vxIn[i] ) || mismatch( vy, vyIn[i] )  ) {
           cerr << "Bad vRelocate() w/o adjustment at i=" << i << ": got (" <<  vx << ", " << vy << ") "
           << " instead of (" << vxIn[i] << ", " << vyIn[i] << ")" << endl;
           exit(1);
        }   
    }
    // array version
    try {
        for ( int i=0; i<n; i++ ) {
            vxOut[i] = vxIn[i];
            vyOut[i] = vyIn[i];
        }
        e.vRelocate( n, lons2, lats2, lons, lats, vxOut, vyOut );
    } catch (...) {
       cerr << "array: Bad vRelocate check w/o adjustment " << endl;
       exit(1);  
    }
    for ( int i=0; i<n; i++ ) {
        if ( mismatch( vxOut[i], vxIn[i] ) || mismatch( vyOut[i], vyIn[i] )  ) {
           cerr << "array: Bad vRelocate() w/o adjustment at i=" << i << ": got (" 
           <<  vxOut[i] << ", " << vyOut[i] << ") "
           << " instead of (" << vxIn[i] << ", " << vyIn[i] << ")" << endl;
           exit(1);
        }   
    }
    
   //----------------------------------------------------------------------
   
   // Now check the conformal vector adjustments
   
    // =============================   make sure that conformal adjusts are set to 1
    e.conformal( 1 );
    if ( e.conformal() != 1 ) {
       cerr << "Unable to enable conformal adjustments by rotation" << endl;
       exit(1);  
    }

    // =============================   vector relocations
    // re-using the arrays created for the last vRelocate test
    for ( int i=0; i<n; i++ ) {
        try {
            vx = vxIn[i];
            vy = vyIn[i];
            e.vRelocate( lons2[i], lats2[i], lons[i], lats[i], &vx, &vy );
        } catch (...) {
           cerr << "Bad vRelocate check " << endl;
           exit(1);  
        }
        if ( mismatch( vx, vxTarget[i] ) || mismatch( vy, vyTarget[i] )  ) {
           cerr << "Bad vRelocate() at i=" << i << ": got (" <<  vx << ", " << vy << ") "
           << " instead of (" << vxTarget[i] << ", " << vyTarget[i] << ")" << endl;
           exit(1);
        }   
    }
    // array version
    try {
        for ( int i=0; i<n; i++ ) {
            vxOut[i] = vxIn[i];
            vyOut[i] = vyIn[i];
        }
        e.vRelocate( n, lons2, lats2, lons, lats, vxOut, vyOut );
    } catch (...) {
       cerr << "array: Bad vRelocate check " << endl;
       exit(1);  
    }
    for ( int i=0; i<n; i++ ) {
        if ( mismatch( vxOut[i], vxTarget[i] ) || mismatch( vyOut[i], vyTarget[i] )  ) {
           cerr << "array: Bad vRelocate() at i=" << i << ": got (" 
           <<  vxOut[i] << ", " << vyOut[i] << ") "
           << " instead of (" << vxTarget[i] << ", " << vyTarget[i] << ")" << endl;
           exit(1);
        }   
    }

    delete[] vyTarget;
    delete[] vyOut;
    delete[] vyIn;
    delete[] vxTarget;
    delete[] vxOut;
    delete[] vxIn;
    delete[] lats2;
    delete[] lons2;
    delete[] lats;
    delete[] lons; 

    // =============================   setting and querying the approximation/quality attribute
    qual = e.approximate();
    if ( qual != 0 ) {
       cerr << "approximate: default value is not 0, but " << qual << endl;
       exit(1);
    }
    e.approximate(-1);
    qual = e.approximate();
    if ( qual != -1 ) {
       cerr << "approximate: approximate() set to -1 did not stick: " << qual << endl;
       exit(1);
    }
    
    // =============================   method deltaxy (again, this time w/ conformal adjustment)
    n = 4;
    lons = new real[n];
    lats = new real[n];
    targetlons_exact = new real[n];
    targetlats_exact = new real[n];
    targetlons_apprx = new real[n];
    targetlats_apprx = new real[n];
    targetlons_crude = new real[n];
    targetlats_crude = new real[n];
    lons2 = new real[n];
    lats2 = new real[n];
    dxs = new real[n];
    dys = new real[n];

    // simple equatorial case, along longitude
    lons[0] = 10.0;
    lats[0] =  0.0;
    dxs[0] = 100.0;
    dys[0] =   0.0;
    targetlons_exact[0] = 10.8993;
    targetlats_exact[0] = 0.0;
    targetlons_apprx[0] = 10.8993;
    targetlats_apprx[0] = 0.0;
    targetlons_crude[0] = 10.8993;
    targetlats_crude[0] = 0.0;
    // simple equatorial case, along latitude
    lons[1] = 10.0;
    lats[1] =  20.0;
    dxs[1] = 0.0;
    dys[1] =  100.0;
    targetlons_exact[1] = 10.0;
    targetlats_exact[1] = 20.8993;
    targetlons_apprx[1] = 10.0;
    targetlats_apprx[1] = 20.8993;
    targetlons_crude[1] = 10.0;
    targetlats_crude[1] = 20.8993;
    // simple non-equatorial case, along longitude
    lons[2] = 40.0;
    lats[2] =  -20.0;
    dxs[2] = 100.0;
    dys[2] =  0.0;
    targetlons_exact[2] = 40.957;
    targetlats_exact[2] = -19.9974;
    targetlons_apprx[2] = 40.9571;
    targetlats_apprx[2] = -19.9977;
    targetlons_crude[2] = 40.9572;
    targetlats_crude[2] = -20.0;
    // typical arbitrary mid-latitude case
    // (this one is a little different from last time)
    lons[3] = 97.0;
    lats[3] =  -43.4;
    dxs[3] = 56.0;
    dys[3] =  -12.3;
    targetlons_exact[3] = 97.6944;
    targetlats_exact[3] = -43.5085;
    targetlons_apprx[3] = 97.6931;
    targetlats_apprx[3] = -43.5106;
    targetlons_crude[3] = 97.6938;
    targetlats_crude[3] = -43.5106;

    // remember: approcimate() was set to -1, exact, above
    for ( int i=0; i<n; i++ ) {
        lon = lons[i];
        lat = lats[i];
        lons2[i] = lon;
        lats2[i] = lat;
        try {
           e.deltaxy( &lon, &lat, dxs[i], dys[i] );
        } catch ( Earth::badincrement err) {
           cerr << "Bad xy deltaxy() <exact> increment at i=" << i 
           << ": expected (" << targetlons_exact[i] << ", " << targetlats_exact[i] << ")"  << endl;
           exit(1);  
        }
        if ( mismatch(lon, targetlons_exact[i]) || mismatch(lat,targetlats_exact[i]) ) {
           cerr << "Bad deltaxy() <exact> increment check at i=" << i << " ("
           << "expecting ( " << targetlons_exact[i] << ", " << targetlats_exact[i] << "), got ("
           << lon << ", " << lat << ")" << endl;
           exit(1);  
        } 
        
    }

    // try the array version
    try {
       e.deltaxy( n, lons2, lats2, dxs, dys );
    } catch ( Earth::badincrement err) {
       cerr << "array: Bad xy <exact> array increment check " << endl;
       exit(1);  
    }
    for ( int i=0; i<n; i++ ) {
        if ( mismatch(lons2[i], targetlons_exact[i]) || mismatch(lats2[i],targetlats_exact[i]) ) {
           cerr << "array: Bad xy <exact> array increment check at i=" << i << " ("
           << "expecting ( " << targetlons_exact[i] << ", " << targetlats_exact[i] << "), got ("
           << lons2[i] << ", " << lats2[i] << ")" << endl;
           exit(1);  
        }
    }


    // now try it again, with the approx set to 0
    e.approximate(0);
    // remember: approcimate() was set to -1, apprx, above
    for ( int i=0; i<n; i++ ) {
        lon = lons[i];
        lat = lats[i];
        lons2[i] = lon;
        lats2[i] = lat;
        try {
           e.deltaxy( &lon, &lat, dxs[i], dys[i] );
        } catch ( Earth::badincrement err) {
           cerr << "Bad xy deltaxy() <apprx> increment at i=" << i 
           << ": expected (" << targetlons_apprx[i] << ", " << targetlats_apprx[i] << ")"  << endl;
           exit(1);  
        }
        if ( mismatch(lon, targetlons_apprx[i]) || mismatch(lat,targetlats_apprx[i]) ) {
           cerr << "Bad deltaxy() <apprx> increment check at i=" << i << " ("
           << "expecting ( " << targetlons_apprx[i] << ", " << targetlats_apprx[i] << "), got ("
           << lon << ", " << lat << ")" << endl;
           exit(1);  
        } 
        
    }

    // try the array version
    try {
       e.deltaxy( n, lons2, lats2, dxs, dys );
    } catch ( Earth::badincrement err) {
       cerr << "array: Bad xy <apprx> array increment check " << endl;
       exit(1);  
    }
    for ( int i=0; i<n; i++ ) {
        if ( mismatch(lons2[i], targetlons_apprx[i]) || mismatch(lats2[i],targetlats_apprx[i]) ) {
           cerr << "array: Bad xy <apprx> array increment check at i=" << i << " ("
           << "expecting ( " << targetlons_apprx[i] << ", " << targetlats_apprx[i] << "), got ("
           << lons2[i] << ", " << lats2[i] << ")" << endl;
           exit(1);  
        }
    }


    // now try it again, with the approx set to 1
    e.approximate(1);
    for ( int i=0; i<n; i++ ) {
        lon = lons[i];
        lat = lats[i];
        lons2[i] = lon;
        lats2[i] = lat;
        try {
           e.deltaxy( &lon, &lat, dxs[i], dys[i] );
        } catch ( Earth::badincrement err) {
           cerr << "Bad xy deltaxy() <crude> increment at i=" << i 
           << ": expected (" << targetlons_crude[i] << ", " << targetlats_crude[i] << ")"  << endl;
           exit(1);  
        }
        if ( mismatch(lon, targetlons_crude[i]) || mismatch(lat,targetlats_crude[i]) ) {
           cerr << "Bad deltaxy() <crude> increment check at i=" << i << " ("
           << "expecting ( " << targetlons_crude[i] << ", " << targetlats_crude[i] << "), got ("
           << lon << ", " << lat << ")" << endl;
           exit(1);  
        } 
        
    }

    // try the array version
    try {
       e.deltaxy( n, lons2, lats2, dxs, dys );
    } catch ( Earth::badincrement err) {
       cerr << "array: Bad xy <crude> array increment check " << endl;
       exit(1);  
    }
    for ( int i=0; i<n; i++ ) {
        if ( mismatch(lons2[i], targetlons_crude[i]) || mismatch(lats2[i],targetlats_crude[i]) ) {
           cerr << "array: Bad xy <crude> array increment check at i=" << i << " ("
           << "expecting ( " << targetlons_crude[i] << ", " << targetlats_crude[i] << "), got ("
           << lons2[i] << ", " << lats2[i] << ")" << endl;
           exit(1);  
        }
    }
    
    delete[] dys;
    delete[] dxs;
    delete[] lats2;
    delete[] lons2;
    delete[] targetlats_crude;
    delete[] targetlons_crude;
    delete[] targetlats_apprx;
    delete[] targetlons_apprx;
    delete[] targetlats_exact;
    delete[] targetlons_exact;
    delete[] lats;
    delete[] lons;


    // if we got this far, everything is OK
    exit(0);

}
    
