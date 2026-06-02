
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

#include "gigatraj/IntegRK4a.hh"

using namespace gigatraj;

const std::string IntegRK4a::id = "IntegRK4";

const real gigatraj::IntegRK4a::NEARPOLE = 88.0;

IntegRK4a :: IntegRK4a()
{
    // the default is to apply no conformal adjustment
    confml = 0;
}

void IntegRK4a :: go( real &lon, real &lat, real &z, double &t, MetData *metsrc, PlanetNav *nav, double dt0 )
{
    // coefficients for the Runga-Kutte integration
    real k1u,k1v,k1w, k2u,k2v,k2w, k3u,k3v,k3w, k4u,k4v,k4w;
    // changes in longitude, latitude, and vertical cooridates, respectively
    real dx,dy,dz;
    // intermediate longotude and latitude variables
    real tmplon,tmplat;
    // sine and cosine of the longitude, for tilting the winds near the poles 
    real cdlon,sdlon;
    // adjusted/"rotate" wind components
    real up,vp;
    // time in seconds
    real dt;
    // delta-lons and delta-lats, delta-zs
    real dlon1, dlon2, dlon3, dlon4;
    real dlat1, dlat2, dlat3, dlat4;
    real dz1, dz2, dz3, dz4;
    std::string dyt;
    real r;
    real mg1, mg2;
    int debug;
    
    debug = metsrc->dbug;
    //debug = 101;

    // get the planetary radius
    r = 6371.0;
    
    // convert days to seconds
    dt = dt0 * 86400.0;

    //std::cerr << "IntegRK4 BEGIN: time0=" << t << ", dt0=" << dt0 << std::endl;
    // note: in general, z will need to be translated to the met levels
    // but this is handled by the met source, internally
    metsrc->get_uvw(t, lon, lat, z,  &k1u, &k1v, &k1w);
    if ( debug >= 100 ) {
       dyt = metsrc->time2Cal(t, 3);
       std::cerr << "     IntegRK4a @ (" << t << "/" << dyt << ", " << lon << "," << lat << ", " << z << ", " << t << "): u1=" << k1u << ", v1=" << k1v << std::endl;
    }
    // convert m/s to km/s 
    k1u = k1u/1000.0;
    k1v = k1v/1000.0;

    
    // advance the parcel halfway through the time delta, using the winds
    dlat1 = dt*k1v/r/RCONV;
    tmplon = lon;
    tmplat = lat;
    nav->deltapos( &tmplon, &tmplat, 0.0, dlat1/2.0 );
    if ( ABS(tmplat) != 90.0 ) {
       dlon1 = dt*k1u/r/RCONV/COS(tmplat*RCONV);
    } else {
       dlon1 = 0.0;
    }
    dz1 = dt*k1w;
    if ( debug >= 100 ) {
       std::cerr << "     IntegRK4a: dlon1, dlat1 = " << dlon1 << ", " << dlat1 << std::endl;
    }
    tmplon = lon;
    tmplat = lat;
    nav->deltapos( &tmplon, &tmplat, dlon1/2, dlat1/2 );          
    // get the winds at this intermediate point
    //std::cerr << "IntegRK4: time1=" << t + dt0/2.0 << std::endl;
    metsrc->get_uvw(t + dt0/2.0, tmplon, tmplat, z + dz1/2.0 ,  &k2u, &k2v, &k2w );
    if ( debug >= 100 ) {
       dyt = metsrc->time2Cal(t + dt0/2.0, 3);
       std::cerr << "     IntegRK4a @ (" << t << "/" << dyt << ", "  << tmplon << "," << tmplat << ", " << z+dz1/2.0 << ", " << t+dt0/2.0 << "): u2=" << k2u << ", v2=" << k2v << std::endl;
    }
    k2u = k2u/1000.0;
    k2v = k2v/1000.0;


    // repeat the procedure using the winds at the intermediate point
    dlat2 = dt*k2v/r/RCONV;
    tmplon = lon;
    tmplat = lat;
    nav->deltapos( &tmplon, &tmplat, 0.0, dlat2/2.0 );
    if ( ABS(tmplat) != 90.0 ) {
       dlon2 = dt*k2u/r/RCONV/COS(tmplat*RCONV);
    } else {
       dlon2 = 0.0;
    }
    dz2 = dt*k2w;
    if ( debug >= 100 ) {
       std::cerr << "     IntegRK4a: dlon2, dlat2 = " << dlon2 << ", " << dlat2 << std::endl;
    }
    tmplon = lon;
    tmplat = lat;
    nav->deltapos( &tmplon, &tmplat, dlon2/2.0, dlat2/2.0 );
    //std::cerr << "IntegRK4: time2=" << t + dt0/2.0 << std::endl;
    metsrc->get_uvw(t + dt0/2.0, tmplon, tmplat, z + dz2/2.0, &k3u, &k3v, &k3w );
    if ( debug >= 100 ) {
       dyt = metsrc->time2Cal(t + dt0/2.0, 3);
       std::cerr << "     IntegRK4a @ (" << t << "/" << dyt << ", "  << tmplon << "," << tmplat << ", " << z+dz2/2.0 << ", " << t+dt0/2.0 << "): u3=" << k3u << ", v3=" << k3v << std::endl;
    }
    k3u = k3u/1000.0;
    k3v = k3v/1000.0;


    // and once again. using the winds from the previous step 
    dlat3 = dt*k3v/r/RCONV;
    tmplon = lon;
    tmplat = lat;
    nav->deltapos( &tmplon, &tmplat, 0.0, dlat3/2.0 ); // (note the 1/2.0 !)
    if ( ABS(tmplat) != 90.0 ) {
       dlon3 = dt*k3u/r/RCONV/COS(tmplat*RCONV);
    } else {
       dlon3 = 0.0;
    }
    dz3 = dt*k3w;
    if ( debug >= 100 ) {
       std::cerr << "     IntegRK4a: dlon3, dlat3 = " << dlon3 << ", " << dlat3 << std::endl;
    }
    tmplon = lon;
    tmplat = lat;
    nav->deltapos( &tmplon, &tmplat, dlon3, dlat3 );
   //std::cerr << "IntegRK4: time3=" << t + dt0 << std::endl;
    metsrc->get_uvw(t + dt0, tmplon, tmplat, z + dz3,   &k4u, &k4v, &k4w );
    if ( debug >= 100 ) {
       dyt = metsrc->time2Cal(t + dt0, 3);
       std::cerr << "     IntegRK4a @ (" << t << "/" << dyt << ", "  << tmplon << "," << tmplat << ", " << z+dt*k3w/2.0 << ", " << t+dt0 << "): u4=" << k4u << ", v4=" << k4v << std::endl;
    }
    k4u = k4u/1000.0;
    k4v = k4v/1000.0;

    // final deltas
    dlat4 = dt*k4v/r/RCONV;
    tmplon = lon;
    tmplat = lat;
    nav->deltapos( &tmplon, &tmplat, 0.0, dlat4/2.0 );
    if ( ABS(tmplat) != 90.0 ) {
       dlon4 = dt*k4u/r/RCONV/COS(tmplat*RCONV);
    } else {
       dlon4 = 0.0;
    }
    dz4 = dt*k4w;
    if ( debug >= 100 ) {
       std::cerr << "     IntegRK4a: dlon4, dlat4 = " << dlon4 << ", " << dlat4 << std::endl;
    }
    
    
    // apply the Runga-Kutte method to get the
    // changes in lon,lat, and z
    dx = ( dlon1 + 2.0*dlon2 + 2.0*dlon3 + dlon4 )/6.0;
    dy = ( dlat1 + 2.0*dlat2 + 2.0*dlat3 + dlat4 )/6.0;
    dz = (   dz1 + 2.0*dz2   + 2.0*dz3   + dz4)/6.0;
    
    //dx =  dt/6.0*(k1u + 2.0*k2u + 2.0*k3u + k4u);
    //dy =  dt/6.0*(k1v + 2.0*k2v + 2.0*k3v + k4v);
    //dz =  dt/6.0*(k1w + 2.0*k2w + 2.0*k3w + k4w);
    
    // update the longitude and latitude
    nav->deltapos( &lon, &lat, dx, dy );
    if ( debug >= 100 ) {
       std::cerr << "     IntegRK4a: dx, dy, lat, lon = " << dx << ", " << dy << ", " << lat << ", " << lon << std::endl;
    }

    // update the vertical coordinate
    z += dz;
    // advance the time
    t += dt0;
    //std::cerr << "IntegRK4 END: time=" << t << ", dt0=" << dt0 << std::endl;


}

void IntegRK4a :: go( int n, real *lons, real *lats, real *zs, int *flags, double &t, MetData *metsrc, PlanetNav *nav, double dt0 )
{
    real lon, lat, z;
    // coefficients for the Runga-Kutte integration
    real k1u,k1v,k1w, k2u,k2v,k2w, k3u,k3v,k3w, k4u,k4v,k4w;
    // changes in longitude, latitude, and vertical cooridates, respectively
    real dx,dy,dz;
    // intermediate longitude and latitude variables
    real tmplon,tmplat,tmpz;
    // sine and cosine of the longitude, for tilting the winds near the poles 
    real cdlon,sdlon;
    // adjusted/"rotate" wind components
    real up,vp;
    // time in seconds
    real dt;
    // delta-lons and delta-lats, delta-zs
    real dlon1, dlon2, dlon3, dlon4;
    real dlat1, dlat2, dlat3, dlat4;
    real dz1, dz2, dz3, dz4;
    std::string dyt;
    real r;
    real mg1, mg2;
    int debug;
    int i;
    int ii;
    int flag;
    const char *nanstr = "";
    int nuse;
    real thslon, thslat, nxtlon, nxtlat;
    
    debug = metsrc->dbug;
    //debug = 101;

    // get the planetary radius
    r = 6371.0;
    
    // convert days to seconds
    dt = dt0 * 86400.0;
    
    int*  const iused = new int[n];

    real* const plons = new real[n];
    real* const plats = new real[n];
    real* const pzs   = new real[n];

    real* const kus = new real[n];
    real* const kvs = new real[n];
    real* const kws = new real[n];

    real* const tmplons = new real[n];
    real* const tmplats = new real[n];
    real* const tmpzs = new real[n];
    
    real* const xhold = new real[n];
    real* const yhold = new real[n];
    real* const zhold = new real[n];

    real* const dlons = new real[n];
    real* const dlats = new real[n];
    real* const dzs   = new real[n];
    
    // mark which parcels we are going to trace
    nuse = 0;
    ii = -1;
    for ( i=0; i<n; i++ ) {
        if ( flags[i] == 0 ) {

           nuse++;
           ii++;

           iused[ii] = i;

           plons[ii] = lons[i];
           plats[ii] = lats[i];
           pzs[ii]   = zs[i];
        }    
    }
    
    // get the winds at those parcels' locations
    metsrc->get_uvw(t, nuse, plons, plats, pzs, kus, kvs, kws);
    if ( debug >= 100 ) {
       dyt = metsrc->time2Cal(t, 3);
       std::cerr << "     IntegRK4a-vector @ (" << t << "/" << dyt << ", " << plons[0] << "," << plats[0] 
       << ", " << pzs[0] << ", " << t << "): u1=" << kus[0] << ", v1=" << kvs[0] << std::endl;
    }

    // now calculate the the stage-1 displacement for each parcel
    for ( i=0; i<nuse; i++ ) {
    
        // this point's current longitude and latitude
        thslon = plons[i];
        thslat = plats[i];

        if ( FINITE(kus[i]) && FINITE(kvs[i]) && FINITE(kws[i]) ) {
    
           // convert m/s to km/s 
           k1u = kus[i]/1000.0;
           k1v = kvs[i]/1000.0;
    
           // advance the parcel halfway through the time delta, using the winds
           dlat1 = dt*k1v/r/RCONV;
           tmplon = thslon;
           tmplat = thslat;
           nav->deltapos( &tmplon, &tmplat, 0.0, dlat1/2.0 );
           if ( ABS(tmplat) != 90.0 ) {
              dlon1 = dt*k1u/r/COS(tmplat*RCONV)/RCONV;
           } else {
              dlon1 = 0.0;
           }

           // advance the vertical position here
           // (but hold off on advancing the horizontal position until
           // the deltapos() call below.) 
           dz1 = dt*kws[i];
           tmpz =  pzs[i] + dz1/2.0;

        } else {
           // bad data at this point, so no advancement
           dlon1 = RNAN(nanstr);
           dlat1 = RNAN(nanstr);
           dz1   = RNAN(nanstr);
           // and the parcel stays where it is
           tmpz = pzs[i];
        }
        
        // remember these for the next stage
        dlons[i] = dlon1;
        dlats[i] = dlat1;

        xhold[i] = dlon1;
        yhold[i] = dlat1;
        zhold[i] = dz1;
        
        // prep for the deltapos() call below
        tmplons[i] = thslon;
        tmplats[i] = thslat;
        
        // remember the stage-1 vertical position
        tmpzs[i] = tmpz;
    }
    nav->deltapos( nuse, tmplons, tmplats, dlons, dlats, 1.0/2.0 );
    if ( debug >= 100 ) {
       std::cerr << "     IntegRK4a-vector: dlons1, dlats1 = " << dlons[0] << ", " << dlats[0] << std::endl;
       //std::cerr << "IntegRK4-vector: time1=" << t + dt0/2.0 << std::endl;
    }

    // now get the winds at the first-stage intermediate points (tmplons,tmplats)
    metsrc->get_uvw(t + dt0/2.0, nuse, tmplons, tmplats, tmpzs, kus, kvs, kws );
    if ( debug >= 100 ) {
       dyt = metsrc->time2Cal(t + dt0/2.0, 3);
       std::cerr << "     IntegRK4a-vector @ (" << t << "/" << dyt << ", "  << tmplons[0] << "," << tmplats[0] 
                 << ", " << tmpzs[0] << ", " << t+dt0/2.0 << "): u2=" << kus[0] << ", v2=" << kvs[0] << std::endl;
    }

    // advance each point to the second stage
    for ( i=0; i<nuse; i++ ) {    
    
        ii = iused[i];
    
        thslon = plons[i];
        thslat = plats[i];
    
        if ( FINITE(kus[i]) && FINITE(kvs[i]) && FINITE(kws[i]) 
          && FINITE(xhold[i]) && FINITE(yhold[i]) && FINITE(zhold[i]) ) {

           k2u = kus[i]/1000.0;
           k2v = kvs[i]/1000.0;

           // repeat the procedure using the winds at the intermediate point
           dlat2 = dt*k2v/r/RCONV;
           tmplon = thslon;
           tmplat = thslat;
           nav->deltapos( &tmplon, &tmplat, 0.0, dlat2/2.0 );
           if ( ABS(tmplat) != 90.0 ) {
              dlon2 = dt*k2u/r/RCONV/COS(tmplat*RCONV);
           } else {
              dlon2 = 0.0;
           }
           dz2 = dt*kws[i];
           
           xhold[i] = xhold[i] + 2.0*dlon2;
           yhold[i] = yhold[i] + 2.0*dlat2;
           zhold[i] = zhold[i] + 2.0*dz2;
           
           //tmpz =  zs[ii] + dz2/2.0;
           tmpz =  pzs[i] + dz2/2.0;
        } else {
           dlon2 = RNAN(nanstr);
           dlat2 = RNAN(nanstr);
           dz2   = RNAN(nanstr);
           
           xhold[i] = dlon2;
           yhold[i] = dlat2;
           zhold[i] = dz2;

           tmpz = pzs[i];
        }

        dlons[i] = dlon2;
        dlats[i] = dlat2;
        
        tmplons[i] = thslon;
        tmplats[i] = thslat;

        tmpzs[i] =  tmpz;
    } 
    nav->deltapos( nuse, tmplons, tmplats, dlons, dlats, 1.0/2.0 );
    if ( debug >= 100 ) {
       std::cerr << "     IntegRK4a-vector: dlons2, dlats2 = " << dlons[0] << ", " << dlats[0] << std::endl;
    }

    // get the winds data at the second-stage intermediate points
    //std::cerr << "IntegRK4-vector: time2=" << t + dt0/2.0 << std::endl;
    metsrc->get_uvw(t + dt0/2.0, nuse, tmplons, tmplats, tmpzs, kus, kvs, kws );
    if ( debug >= 100 ) {
       dyt = metsrc->time2Cal(t + dt0/2.0, 3);
       std::cerr << "     IntegRK4a-vector @ (" << t << "/" << dyt << ", "  << tmplons[0] << "," << tmplats[0] 
                 << ", " << tmpzs[0] << ", " << t+dt0/2.0 << "): u3=" << kus[0] << ", v3=" << kvs[0] << std::endl;
    }

    // advance each point to the third stage
    for ( i=0; i<nuse; i++ ) {    
    
        ii = iused[i];
    
        thslon = plons[i];
        thslat = plats[i];

        if ( FINITE(kus[i]) && FINITE(kvs[i]) && FINITE(kws[i]) 
          && FINITE(xhold[i]) && FINITE(yhold[i]) && FINITE(zhold[i]) ) {
    
           k3u = kus[i]/1000.0;
           k3v = kvs[i]/1000.0;
           
           dlat3 = dt*k3v/r/RCONV;
           tmplon = thslon;
           tmplat = thslat;
           nav->deltapos( &tmplon, &tmplat, 0.0, dlat3/2.0 );
           if ( ABS(tmplat) != 90.0 ) {
              dlon3 = dt*k3u/r/RCONV/COS(tmplat*RCONV);
           } else {
              dlon3 = 0.0;
           }
  
           dz3 = dt*kws[i];

           xhold[i] = xhold[i] + 2.0*dlon3;
           yhold[i] = yhold[i] + 2.0*dlat3;
           zhold[i] = zhold[i] + 2.0*dz3;

           tmpz =  pzs[i] + dz3;
        } else {
           dlon3 = RNAN(nanstr);
           dlat3 = RNAN(nanstr);
           dz3   = RNAN(nanstr);

           xhold[i] = dlon3;
           yhold[i] = dlat3;
           zhold[i] = dz3;

           tmpz   = pzs[i];
        }
        
        dlons[i] = dlon3;
        dlats[i] = dlat3;

        tmplons[i] = thslon;
        tmplats[i] = thslat;

        tmpzs[i] =  tmpz;
    } 
    nav->deltapos( nuse, tmplons, tmplats, dlons, dlats );
    if ( debug >= 100 ) {
       std::cerr << "     IntegRK4a-vector: dlons3, dlats3 = " << dlons[0] << ", " << dlats[0] << std::endl;
    }

    /// get the met data at the stage-3 intermediate points
    metsrc->get_uvw(t + dt0, nuse, tmplons, tmplats, tmpzs,  kus, kvs, kws );
    if ( debug >= 100 ) {
       //std::cerr << "IntegRK4-vector: time3=" << t + dt0 << std::endl;
       dyt = metsrc->time2Cal(t + dt0, 3);
       std::cerr << "     IntegRK4a-vector @ (" << t << "/" << dyt << ", "  << tmplons[0] << "," << tmplats[0] 
                 << ", " << tmpzs[0] << ", " << t+dt0 << "): u4=" << kus[0] << ", v4=" << kvs[0] << std::endl;
    }

    // advance each point
    for ( i=0; i<nuse; i++ ) {    
    
        ii = iused[i];
    
        thslon = plons[i];
        thslat = plats[i];

        if ( FINITE(kus[i]) && FINITE(kvs[i]) && FINITE(kws[i]) 
          && FINITE(xhold[i]) && FINITE(yhold[i]) && FINITE(zhold[i]) ) {
    
           k4u = kus[i]/1000.0;
           k4v = kvs[i]/1000.0;

           // final deltas
           dlat4 = dt*k4v/r/RCONV;
           tmplon = thslon;
           tmplat = thslat;
           nav->deltapos( &tmplon, &tmplat, 0.0, dlat4/2.0 );
           if ( ABS(tmplat) != 90.0 ) {
              dlon4 = dt*k4u/r/RCONV/COS(tmplat*RCONV);
           } else {
              dlon4 = 0.0;
           }
           if ( debug >= 100 && i == 0 ) {
              std::cerr << "     IntegRK4a: dlon4, dlat4 = " << dlon4 << ", " << dlat4 << std::endl;
           }

           dz4 = dt*kws[i];
            
           // apply the Runga-Kutte method to get the
           // changes in lon,lat, and z
           // (using the sums we have been accumulating so far)
           dx = ( xhold[i] + dlon4 )/6.0;
           dy = ( yhold[i] + dlat4 )/6.0;
           dz = ( zhold[i] + dz4)/6.0;
    
           // update the longitude and latitude
           nav->deltapos( &(lons[ii]), &(lats[ii]), dx, dy );
           if ( debug >= 100 ) {
              std::cerr << "     IntegRK4a: dx, dy, lat, lon = " << dx << ", " << dy << ", " << lat << ", " << lon << std::endl;
           }
           // update the vertical coordinate
           zs[ii] += dz;
    
        } else {
        
           // disable tracing this parcel 
           flags[ii] = 1;
        
        }

    }
    
    delete []dzs;
    delete []dlats;
    delete []dlons;
    delete []zhold;
    delete []yhold;
    delete []xhold;
    delete []tmpzs;
    delete []tmplats;
    delete []tmplons;
    delete []kws;
    delete []kvs;
    delete []kus;
    delete []pzs;
    delete []plats;
    delete []plons;
    delete []iused;


    // advance the time
    t += dt0;
    //std::cerr << "IntegRK4 END: time=" << t << ", dt0=" << dt0 << std::endl;


}
