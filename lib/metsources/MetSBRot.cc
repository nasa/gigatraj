
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

#include <sstream>
#include <cmath>

#include "gigatraj/MetSBRot.hh"

using namespace gigatraj;

const std::string MetSBRot::iam = "MetSBRot";


MetSBRot::MetSBRot() : MetData() 
{
   ws = 40.0;
   aa = 0.0;
   bb = 0.0;
   gg = 0.0;
   vs = 0.0;
   fr = 0.0;
   pr = 0.0;
   wfctr = 0.001; // convert m/s -> km/s, since alt is in km
   
   calsys = 0;
   basetime = 0.0;
   
   testInt = 123;
   testFloat = 456.7;
   testDouble = 981.0;
   testBool = false;
   testString = "test string";

   wmo_trop.setTropName("trop");
   wmo_trop.setTemperatureName("t");
   wmo_trop.setPressureName("p");
   wmo_trop.setAltitudeName("PAlt");
   wmo_trop.setPotentialTemperatureName("Theta");
   wmo_trop.setDensityName("rho");

};

MetSBRot::MetSBRot(real windspeed, ProcessGrp* pg, int met) : MetData() 
{
   ws = windspeed;
   aa = 0.0;
   bb = 0.0;
   gg = 0.0;
   vs = 0.0;
   fr = 0.0;
   pr = 0.0;
   wfctr = 0.001; // convert m/s -> km/s, since alt is in km

   calsys = 0;
   basetime = 0.0;

   my_pgroup = pg;
   my_metproc = met;

   
   testInt = 123;
   testFloat = 456.7;
   testDouble = 981.0;
   testBool = false;
   testString = "test string";

   wmo_trop.setTropName("trop");
   wmo_trop.setTemperatureName("t");
   wmo_trop.setPressureName("p");
   wmo_trop.setAltitudeName("PAlt");
   wmo_trop.setPotentialTemperatureName("Theta");
   wmo_trop.setDensityName("rho");

};

MetSBRot::MetSBRot(real windspeed, real tilt, ProcessGrp* pg, int met) : MetData() 
{
   ws = windspeed;
   aa = 0.0;
   bb = tilt;
   gg = 0.0;
   vs = 0.0;
   fr = 0.0;
   pr = 0.0;
   wfctr = 0.001; // convert m/s -> km/s, since alt is in km

   calsys = 0;
   basetime = 0.0;

   //std::cerr << "windspeed=" << ws << ", tilt=" << bb << std::endl;

   my_pgroup = pg;
   my_metproc = met;

   
   testInt = 123;
   testFloat = 456.7;
   testDouble = 981.0;
   testBool = false;
   testString = "test string";

   wmo_trop.setTropName("trop");
   wmo_trop.setTemperatureName("t");
   wmo_trop.setPressureName("p");
   wmo_trop.setAltitudeName("PAlt");
   wmo_trop.setPotentialTemperatureName("Theta");
   wmo_trop.setDensityName("rho");

};

MetSBRot::MetSBRot(real windspeed, real alpha, real beta, real gamma, ProcessGrp* pg, int met) : MetData() 
{
   ws = windspeed;
   aa = alpha;
   bb = beta;
   gg = gamma;
   vs = 0.0;
   fr = 0.0;
   pr = 0.0;
   wfctr = 0.001; // convert m/s -> km/s, since alt is in km

   calsys = 0;
   basetime = 0.0;

   my_pgroup = pg;
   my_metproc = met;
   
   testInt = 123;
   testFloat = 456.7;
   testDouble = 981.0;
   testBool = false;
   testString = "test string";

   wmo_trop.setTropName("trop");
   wmo_trop.setTemperatureName("t");
   wmo_trop.setPressureName("p");
   wmo_trop.setAltitudeName("PAlt");
   wmo_trop.setPotentialTemperatureName("Theta");
   wmo_trop.setDensityName("rho");

};


MetSBRot::~MetSBRot() 
{
   int a;
   
   a = 12.0;      
};


void MetSBRot::assign(const MetSBRot& src)
{
     MetData::assign(src);
     
     ws = src.ws;
     aa = src.aa;
     bb = src.bb;
     gg = src.gg;
     vs = src.vs;
     fr = src.fr;
     pr = src.pr;
     
     calsys = src.calsys;
     basetime = src.basetime;
     
     testInt = src.testInt;
     testFloat = src.testFloat;
     testDouble = src.testDouble;
     testBool = src.testBool;
     testString = src.testString;

     wmo_trop = src.wmo_trop;
     
}


MetData *MetSBRot::genericCopy()
{
    MetSBRot *result;
    
    result = new MetSBRot;
    result->assign(*this);
    
    return dynamic_cast<MetData*>( result );
}


void MetSBRot::set(real windspeed) 
{
   ws = windspeed;

};

void MetSBRot::set(real windspeed, real tilt) 
{
   ws = windspeed;
   aa = 0.0;
   bb = tilt;
   gg = 0.0;

};

void MetSBRot::set(real windspeed, real alpha, real beta, real gamma) 
{
   ws = windspeed;
   aa = alpha;
   bb = beta;
   gg = gamma;
   
};

void MetSBRot::setvert(real amp, real period) 
{
    vs = amp;
    if ( period > 0.0 ) {
       // change period to days, then get the frequency
       fr = 2.0*PI / ( period * 3600.0*24.0 );
    } else {
       fr = 0.0;
    }

};

void MetSBRot::set_period(real period)
{
    if ( period > 0.0 ) {
       // get the frequency
       pr = 2.0*PI / ( period * 3600.0*24.0 );
    } else {
       pr = 0.0;
    }
} 


void MetSBRot::set_cal( int cal )
{
     calsys = cal;
     switch (calsys) {
     case 0:
          basetime = 0.0;
          break;
     case 1:
          basetime =  25568.0;
          break;
     }
}

int MetSBRot::get_cal() const
{
     return calsys;
} 

void MetSBRot::setOption( const std::string &name, const std::string &value )
{
     int item;
     float fitem;
     double ditem;
     
     if ( name == "test" ) {
        testString = value;
     } else if ( name == "tilt" || name == "maxwind") {
        if ( str2float( value, &fitem ) ) {
           setOption(name, fitem );
        }
     } else {
        MetData::setOption( name, value );
     }
}

void MetSBRot::setOption( const std::string &name, int value )
{
     if ( name == "test" ) {
        testInt = value;
     } else {
        MetData::setOption( name, value );
     }
}

void MetSBRot::setOption( const std::string &name, float value )
{
     if ( name == "test" ) {
        testFloat = value;
     } else if ( name == "tilt" ) {
        set( ws, value );
     } else if ( name == "maxwind" ) {
        set( value );
     } else {
        MetData::setOption( name, value );
     }
}

void MetSBRot::setOption( const std::string &name, double value )
{
     if ( name == "test" ) {
        testDouble = value;
     } else {
        MetData::setOption( name, value );
     }
}


bool MetSBRot::getOption( const std::string &name, std::string &value )
{
     bool result;
     float fitem;
     
     if ( name == "test" ) {
        value = testString;
        result = true;
     } else if ( name == "tilt" || name == "maxwind") {
        if ( getOption( name, fitem ) ) {
           result = float2str( fitem, value );
        }
     } else {
        result = MetData::getOption( name, value );
     }
     
     return result;
}


bool MetSBRot::getOption( const std::string &name, int &value )
{
     bool result;
     
     if ( name == "test" ) {
        value = testInt;
        result = true;
     } else {
        result = MetData::getOption( name, value );
     }
     
     return result;
}


bool MetSBRot::getOption( const std::string &name, float &value )
{
     bool result;
     
     if ( name == "test" ) {
        value = testFloat;
        result = true;
     } else if ( name == "tilt" ) {
        value = bb;
     } else if ( name == "maxwind" ) {
        value = ws;
     } else {
        result = MetData::getOption( name, value );
     }
     
     return result;
}


bool MetSBRot::getOption( const std::string &name, double &value )
{
     bool result;
     
     if ( name == "test" ) {
        value = testDouble;
        result = true;
     } else {
        result = MetData::getOption( name, value );
     }
     
     return result;
}


string MetSBRot::time2Cal( const double time, int format )
{
   std::ostringstream otmp;
   static const gigatraj::CalGregorian cal;
   
   switch (calsys) {
   case 0:
        otmp << time + basetime;
        return otmp.str();   
        break;
   case 1:
        return cal.date1900( basetime + time, format );
        break;
   }
   
   return "invalid";
   
};


double MetSBRot::cal2Time( const std::string cal )
{
   double time;
   std::istringstream itmp;
   static const gigatraj::CalGregorian calx;
   double result;
   
   switch (calsys) {
   case 0:
        itmp.str(cal);
        itmp >> time;
        result = (time - basetime);
        break;
   case 1:
        result = (calx.day1900( cal ) - basetime);
        break;
   }
   
   return result;
};

void MetSBRot::defineCal( string caldate, double time )
{
     // clear out the current basetime
     basetime = 0.0;
     // convert cal to a time and set basetime to the offset between that and time
     basetime = ( cal2Time( caldate ) - time );

}


std::string MetSBRot::u_wind( std::string *units ) const
{
    if ( units != NULL ) {
       *units = "m/s";
    }
    return "u";   
}

std::string MetSBRot::v_wind( std::string *units ) const
{
    if ( units != NULL ) {
       *units = "m/s";
    }
    return "v";   
}

std::string MetSBRot::w_wind( std::string *units ) const
{
    if ( units != NULL ) {
       *units = "m/s";
    }
    return "w";   
}

std::string MetSBRot::vertical( std::string *units ) const
{
    if ( units != NULL ) {
       *units = "km";
    }
    return "alt";   
}

std::string MetSBRot::vunits() const
{
    return "km";   
}

int MetSBRot::vIncrease() const
{
     return 1;
}

void MetSBRot::get_uvw( double time, real lon, real lat, real z
, real *u, real *v, real *w)
{
   int cmd;
   real coords[3];
   real vals[3];
   double xtime;
   
   // Note: we do not need to convert internal model time here
   
   if ( ! isMetClient() ) {
      /* sines and cosines */
      real ca, sa, cb, sb, cg, sg;
      real clat,slat,clon,slon;
      real cxlat,sxlat,cxlon,sxlon;
      /* rectangular coordinates */ 
      real xx,yy,zz;
      /* tilted rectangular coordinates */
      real xt, yt, zt, ht;
      /* wind components in UN-tilted rectangular coords */
      real wxu, wyu, wzu;
      /* time-varying wind speed */
      real wst;
   
      time = time * 24.0*3600.0; // to make the times come out as they did when we were using seconds for internal model time
   
      /* get sines and cosines */
      ca = COS(aa*RCONV);
      sa = SIN(aa*RCONV);
      cb = COS(bb*RCONV);
      sb = SIN(bb*RCONV);
      cg = COS(gg*RCONV);
      sg = SIN(gg*RCONV);
   
      clat = COS(lat * RCONV);
      slat = SIN(lat * RCONV);
      clon = COS(lon * RCONV);
      slon = SIN(lon * RCONV);

      /* convert location from spherical to rectangular coords */
      xx = clat*clon;
      yy = clat*slon;
      zz = slat;

      /* convert location to tilted coordinates */
      xt = (ca*cg-sa*cb*sg)*xx + (-sa*cg-ca*cb*sg)*yy +  (sb*sg)*zz;
      yt = (ca*sg+sa*cb*cg)*xx + (-sa*sg+ca*cb*cg)*yy + (-sb*cg)*zz;
      zt =          (sa*sb)*xx +           (ca*sb)*yy +     (cb)*zz;
   
      /* get the radial distance from the axis of rotation */
      ht = SQRT(xt*xt+yt*yt);
      /* get the sines and cosines of this pseudo-longitude and pseudo-latitude
         in the titled cooridnate system */
      sxlat = zt;  /* Note: we are using the unit sphere, so the denominator is 1.0 */
      cxlat = ht;
      if (ht > 0.0) 
      {
         /* the the sine and cosine of the pseudo-longitude on the rotated sphere */
         sxlon = yt/ht;
         cxlon = xt/ht;
      } else { 
         /* the position on the rotated spheres is one of the poles. Cheat. */
         sxlon = 0.0;
         cxlon = 0.0;
      }   

      /* the wind speed components on the rotated sphere are: 
            u = ws*cxlon
            v = 0.0
            w = 0.0
         convert these to rectangular coordinates,   
         then them to UN-rotated rectangular coords */
      wst = ws * COS( pr*time );
      wxu = (ca*((-sxlon*cxlat*wst*cg) + (sg*cxlon*cxlat*wst)) 
            + sa*( cb*((cg*cxlon*cxlat*wst) 
            - (-sxlon*cxlat*wst*sg)) ));
      wyu = (ca*( cb*((cg*cxlon*cxlat*wst) - (-sg*sxlon*cxlat*wst))) 
            -  sa*((sg*cxlon*cxlat*wst) 
            + (-cg*sxlon*cxlat*wst)));
      wzu = ( + sb*((-sg*sxlon*cxlat*wst) 
            - (cxlon*cxlat*wst*cg)));

      /* convert to spherical coordinates */
      *u =      -slon*wxu +      clon*wyu +  0.0*wzu;
      *v = -slat*clon*wxu - slat*slon*wyu + clat*wzu;
      *w =  clat*clon*wxu + clat*slon*wyu + slat*wzu;
   
      /* do the vertical wind */
      *w = vs * COS( fr*time );
   } else {
      /* get values from a dedicated met processor*/
      cmd = PGR_CMD_UVW;
      my_pgroup->send_ints( my_metproc, 1, &cmd, PGR_TAG_REQ );
      my_pgroup->send_doubles( my_metproc, 1, &time, PGR_TAG_TIME);
      coords[0] = lon;
      coords[1] = lat;
      coords[2] = z;
      my_pgroup->send_reals( my_metproc, 3, coords, PGR_TAG_COORDS);
      my_pgroup->receive_reals( my_metproc, 3, vals, PGR_TAG_RECV);
      
      *u = vals[0];
      *v = vals[1];
      *w = vals[2]*wfctr;
   
   }
   
   if ( isoVertical() ) {
      *w = 0.0;
   }

};

void MetSBRot::get_uvw( double time, int n, real* lons, real* lats, real* zs
, real *u, real *v, real *w)
{
     for (int i=0; i<n; i++ ) {
         
          get_uvw( time, lons[i], lats[i], zs[i], &(u[i]), &(v[i]), &(w[i]) );
      
     } 
}

real MetSBRot::get_u( double time, real lon, real lat, real z)
{
     real u,v,w;
     real zz;

     //if ( z == 0.0 ) {   
     //   zz = 7.0*LOG(1000.0/z);
     //} else {
     //   zz = 1000.0;
     //}

     MetSBRot::get_uvw( time, lon, lat, zz, &u, &v, &w);

     return u;
}

real MetSBRot::get_v( double time, real lon, real lat, real z)
{
     real u,v,w;
     real zz;

     //if ( z == 0.0 ) {   
     //   zz = 7.0*LOG(1000.0/z);
     //} else {
     //   zz = 1000.0;
     //}

     MetSBRot::get_uvw( time, lon, lat, zz, &u, &v, &w);

     return v;
}

real MetSBRot::get_w( double time, real lon, real lat, real z)
{
     real u,v,w;
     real zz;

     //if ( z == 0.0 ) {   
     //   zz = 7.0*LOG(1000.0/z);
     //} else {
     //   zz = 1000.0;
     //}

   
     if ( isoVertical() ) {
        w = 0.0;
     } else {
        MetSBRot::get_uvw( time, lon, lat, zz, &u, &v, &w);
     }

     return w;
}

real MetSBRot::getData( string quantity, double time, real lon, real lat, real z, int flags)
{
   real dat;
   real u, v, w;
   real zz;
   int cmd;
   real coords[3];

   if ( quantity == "u" ) 
   {         
      
     MetSBRot::get_uvw( time, lon, lat, z, &dat, &v, &w);
              
   } else if ( quantity == "v" )
   {         
      
     MetSBRot::get_uvw( time, lon, lat, z, &u, &dat, &w);
              
   } else if ( quantity == "w" || quantity == "heating rate" || quantity == "omega" )
   {         
     /* vertical velocity is going to be 0.0 */ 
     dat = 0.0;
              
   } else if ( quantity == "p" )
   {  
      if ( ! isMetClient() ) {
         /* interpolate pressure from us std atmos, and possible convert to MKS */
         dat = usta76( quantity, "alt", z );  
      } else {
         /* get values from a dedicated met processor*/
         cmd = PGR_CMD_DATA;
         my_pgroup->send_ints( my_metproc, 1, &cmd, PGR_TAG_REQ );
         my_pgroup->send_string( my_metproc, quantity, PGR_TAG_QUANT);
         my_pgroup->send_doubles( my_metproc, 1, &time, PGR_TAG_TIME);
         coords[0] = lon;
         coords[1] = lat;
         coords[2] = z;
         my_pgroup->send_reals( my_metproc, 3, coords, PGR_TAG_COORDS);
         my_pgroup->receive_reals( my_metproc, 1, &dat, PGR_TAG_RECV);
         
      }
      if ( flags & METDATA_MKS ) {
         dat = dat * 0.10;
      }   
   } else if ( quantity == "tropz" )
   {         
        std::vector<real> z_profile;
        std::vector<real> t_profile;
        real val;
        
        real z_std[] = {
           0.00000, 1.00000, 2.00000, 3.00000, 4.00000, 5.00000
         , 6.00000, 7.00000, 8.00000, 9.00000, 10.0000, 11.0000
         , 12.0000, 13.0000, 14.0000, 15.0000, 16.0000, 17.0000
         , 18.0000, 19.0000, 20.0000, 21.0000, 22.0000, 23.0000 };
        
        real t_std[] = {
           288.150, 281.650, 275.150, 268.650, 262.150, 255.650
         , 249.150, 242.650, 236.150, 229.650, 223.150, 216.650
         , 216.650, 216.650, 216.650, 216.650, 216.650, 216.650
         , 216.650, 216.650, 216.650, 217.650, 218.650, 219.650 };
         
         for ( int i=0; i<24; i++ ) {
             z_profile.push_back( z_std[i]*1000.0 );
             t_profile.push_back( t_std[i] );
         }
         
         // tropopause height is returned in m
         // The vertical coordinate is always alt in km. 
         dat = wmo_trop.wmo( t_profile, z_profile )/1000.0;
                       
   } else if ( quantity == "alt" )
   {         
      
      dat = z;
              
   } else {
      if ( ! isMetClient() ) {
         /* interpolate quantities from us std atmos */
         dat = usta76( quantity, "alt", z );  
      } else {
         /* get values from a dedicated met processor*/
         cmd = PGR_CMD_DATA;
         my_pgroup->send_ints( my_metproc, 1, &cmd, PGR_TAG_REQ );
         my_pgroup->send_string( my_metproc, quantity, PGR_TAG_QUANT);
         my_pgroup->send_doubles( my_metproc, 1, &time, PGR_TAG_TIME);
         coords[0] = lon;
         coords[1] = lat;
         coords[2] = z;
         my_pgroup->send_reals( my_metproc, 3, coords, PGR_TAG_COORDS);
         my_pgroup->receive_reals( my_metproc, 1, &dat, PGR_TAG_RECV);
         
      }
   }
      
   return dat;
};


real MetSBRot::usta76( string quantity, string vertical, real z )
{
     real g0 =    9.80665;
     real r0 = 6356.766;
     real Rs =    8.31432e3;
     real M0 =   28.9644;
     real NA = 6.022169e26;
     real result;
     real zz;
     real* x;
     real* y;
     int n_std;
     int i;
     static real z_std[] = {
           0.00000, 1.00000, 2.00000, 3.00000, 4.00000, 5.00000
         , 6.00000, 7.00000, 8.00000, 9.00000, 10.0000, 11.0000
         , 12.0000, 13.0000, 14.0000, 15.0000, 16.0000, 17.0000
         , 18.0000, 19.0000, 20.0000, 21.0000, 22.0000, 23.0000
         , 24.0000, 25.0000, 26.0000, 27.0000, 28.0000, 29.0000
         , 30.0000, 31.0000, 32.0000, 33.0000, 34.0000, 35.0000
         , 36.0000, 37.0000, 38.0000, 39.0000, 40.0000, 41.0000
         , 42.0000, 43.0000, 44.0000, 45.0000, 46.0000, 47.0000
         , 48.0000, 49.0000, 50.0000, 51.0000, 52.0000, 53.0000
         , 54.0000, 55.0000, 56.0000, 57.0000, 58.0000, 59.0000
         , 60.0000, 61.0000, 62.0000, 63.0000, 64.0000, 65.0000
         , 66.0000, 67.0000, 68.0000, 69.0000, 70.0000, 71.0000
         , 72.0000, 73.0000, 74.0000, 75.0000, 76.0000, 77.0000
         , 78.0000, 79.0000, 80.0000, 81.0000, 82.0000, 83.0000
         , 84.0000, 85.0000
     };
     static real t_std[] = {
           288.150, 281.650, 275.150, 268.650, 262.150, 255.650
         , 249.150, 242.650, 236.150, 229.650, 223.150, 216.650
         , 216.650, 216.650, 216.650, 216.650, 216.650, 216.650
         , 216.650, 216.650, 216.650, 217.650, 218.650, 219.650
         , 220.650, 221.650, 222.650, 223.650, 224.650, 225.650
         , 226.650, 227.650, 228.650, 231.450, 234.250, 237.050
         , 239.850, 242.650, 245.450, 248.250, 251.050, 253.850
         , 256.650, 259.450, 262.250, 265.050, 267.850, 270.650
         , 270.650, 270.650, 270.650, 270.650, 267.850, 265.050
         , 262.250, 259.450, 256.650, 253.850, 251.050, 248.250
         , 245.450, 242.650, 239.850, 237.050, 234.250, 231.450
         , 228.650, 225.850, 223.050, 220.250, 217.450, 214.650
         , 212.650, 210.650, 208.650, 206.650, 204.650, 202.650
         , 200.650, 198.650, 196.650, 194.650, 192.650, 190.650
         , 188.650, 186.867
     };
     static real p_std[] = {
           1013.25, 898.746, 794.952, 701.086, 616.402, 540.199
         , 471.810, 410.607, 355.998, 307.425, 264.363, 226.321
         , 193.304, 165.104, 141.018, 120.446, 102.875, 87.8668
         , 75.0484, 64.1001, 54.7489, 46.7789, 39.9979, 34.2243
         , 29.3049, 25.1102, 21.5309, 18.4746, 15.8629, 13.6296
         , 11.7187, 10.0823, 8.68019, 7.48228, 6.46122, 5.58924
         , 4.84317, 4.20367, 3.65455, 3.18221, 2.77522, 2.42396
         , 2.12030, 1.85738, 1.62937, 1.43135, 1.25910, 1.10906
         , 0.977546, 0.861624, 0.759448, 0.669389, 0.589622, 0.518669
         , 0.455632, 0.399700, 0.350137, 0.306274, 0.267509, 0.233296
         , 0.203143, 0.176606, 0.153287, 0.132826, 0.114900, 0.0992204
         , 0.0855276, 0.0735896, 0.0631992, 0.0541718, 0.0463423, 0.0395643
         , 0.0337177, 0.0286917, 0.0243773, 0.0206792, 0.0175141, 0.0148092
         , 0.0125013, 0.0105351, 0.00886281, 0.00744281, 0.00623906, 0.00522038
         , 0.00435982, 0.00363421
     };
     real h_std[] = {
           287.068, 290.373, 293.794, 297.339, 301.015, 304.831
         , 308.795, 312.917, 317.209, 321.682, 326.349, 331.225
         , 346.489, 362.457, 379.161, 396.634, 414.912, 434.033
         , 454.035, 474.959, 496.847, 522.089, 548.488, 576.093
         , 604.952, 635.116, 666.639, 699.573, 733.978, 769.910
         , 807.430, 846.602, 887.489, 937.294, 989.245, 1043.41
         , 1099.84, 1158.63, 1219.82, 1283.50, 1349.73, 1418.59
         , 1490.14, 1564.47, 1641.65, 1721.76, 1804.87, 1891.06
         , 1960.51, 2032.50, 2107.14, 2184.52, 2241.74, 2301.07
         , 2362.64, 2426.54, 2492.88, 2561.80, 2633.42, 2707.88
         , 2785.33, 2865.92, 2949.82, 3037.20, 3128.26, 3223.19
         , 3322.21, 3425.55, 3533.45, 3646.18, 3764.02, 3887.27
         , 4031.07, 4181.63, 4339.33, 4504.58, 4677.83, 4859.54
         , 5050.21, 5250.39, 5460.66, 5681.62, 5913.95, 6158.36
         , 6415.60, 6694.25
     };
     real d_std[] = {
           1.22500, 1.11164, 1.00649, 0.909122, 0.819129, 0.736115
         , 0.659697, 0.589501, 0.525167, 0.466348, 0.412706, 0.363918
         , 0.310828, 0.265483, 0.226753, 0.193674, 0.165420, 0.141288
         , 0.120676, 0.103071, 0.0880348, 0.0748737, 0.0637273, 0.0542802
         , 0.0462673, 0.0394658, 0.0336882, 0.0287769, 0.0245988, 0.0210420
         , 0.0180119, 0.0154287, 0.0132250, 0.0112620, 0.00960888, 0.00821392
         , 0.00703441, 0.00603513, 0.00518691, 0.00446557, 0.00385101, 0.00332648
         , 0.00287802, 0.00249393, 0.00216443, 0.00188129, 0.00163760, 0.00142753
         , 0.00125825, 0.00110904, 0.000977525, 0.000861606, 0.000766867, 0.000681710
         , 0.000605253, 0.000536684, 0.000475264, 0.000420311, 0.000371207, 0.000327383
         , 0.000288321, 0.000253550, 0.000222640, 0.000195200, 0.000170875, 0.000149342
         , 0.000130308, 0.000113510, 9.87069e-05, 8.56831e-05, 7.42430e-05, 6.42111e-05
         , 5.52370e-05, 4.74496e-05, 4.07010e-05, 3.48607e-05, 2.98135e-05, 2.54579e-05
         , 2.17046e-05, 1.84751e-05, 1.57006e-05, 1.33205e-05, 1.12820e-05, 9.53900e-06
         , 8.05099e-06, 6.77508e-06
     };

     n_std = sizeof(z_std)/sizeof(*z_std);


     /* set up the independent variable, based on what z is */
     if ( vertical == "alt" ) {
        x = z_std;
     } else if ( vertical == "p" ) {
        x = p_std;
     } else if ( vertical == "theta" ) {
        x = h_std;
     } else if ( vertical == "rho" ) {
        x = d_std;
     } else {
        x = NULLPTR;
     }
     
     /* set up the dependent variable, based on the desired quantity */
     
     result = 1.0;
     if ( quantity == "alt" ) {
        y = z_std;
     } else if ( quantity == "p" ) {
        y = p_std;
     } else if ( quantity == "theta" ) {
        y = h_std;
     } else if ( quantity == "t" ) {
        y = t_std;
     } else if ( quantity == "rho" ) {
        y = d_std;
     } else {
        y = NULLPTR;
     }
     
     result = 1.0;
     
     if ( x != NULLPTR && y != NULLPTR ) {     
         // linearly interpolate
         for ( i=0; i<(n_std-1); i++ ) {
             // find a point that straddles the desired level
             if ( ( (x[i] - z ) * (x[i+1] - z) ) <= 0.0 ) { 
                result = y[i+1] - ( y[i+1]-y[i] )/( x[i+1]-x[i] )*(x[i+1]-z);                
                break;
             }
         }
     }
     
     return result;
     
}



void MetSBRot::serveMet()
{
   int done_count = 0;
   int done_goal;
   int tracer_request;
   int src;
   int n;
   float lat,lon,z;
   real val;
   std::string quant;
   double time;
   real coords[3];
   real valset[3];
   real u,v,w;
   
   if ( isMetServer() ) {
      // how many processors do we need to tell us we are done?
      done_goal = my_pgroup->size() - 1;
//std::cerr << "serveMet: [" << my_pgroup->id() << "] entering, looking for " << done_goal << std::endl;
      
      while ( done_count < done_goal ) {
//std::cerr << "serveMet: [" << my_pgroup->id() << "] begin loop " << done_count << " of " << done_goal << std::endl;
      
          // receive a signal from any processor in this group
          my_pgroup->receive_ints( -1, 1, &tracer_request, PGR_TAG_REQ, &src );
//std::cerr << "serveMet: [" << my_pgroup->id() << "] got " << tracer_request << " req from " <<  src << std::endl;
          switch (tracer_request) {
          case PGR_CMD_DONE:
             // that processor is done
             done_count++;
//std::cerr << "serveMet: [" << my_pgroup->id() << "] done_count = " << done_count << " of " << done_goal << std::endl;
             break;
          case PGR_CMD_DATA:
             // send data values to a client
//std::cerr << "serveMet: [" << my_pgroup->id() << "] sending misc data " << std::endl;
             
             // get the quantity
             my_pgroup->receive_string( src, &quant, PGR_TAG_QUANT );
             // get the time
             my_pgroup->receive_doubles( src, 1, &time, PGR_TAG_TIME );
             // get the coordinates
             my_pgroup->receive_reals( src, 3, coords, PGR_TAG_COORDS );
             
             // obtain the data value
             val = getData( quant, time, coords[0], coords[1], coords[2] );
             
             // send the data value
             my_pgroup->send_reals( src, 1, &val, PGR_TAG_RECV );
                          
             break;
          case PGR_CMD_UVW:
             // send wind data values to a client
//std::cerr << "serveMet: [" << my_pgroup->id() << "] sending uvw data " << std::endl;
             
             // get the time
             my_pgroup->receive_doubles( src, 1, &time, PGR_TAG_TIME );
             // get the coordinates
             my_pgroup->receive_reals( src, 3, coords, PGR_TAG_COORDS );
             
             // obtain the data values
             get_uvw( time, coords[0], coords[1], coords[2], &u, &v, &w);
             valset[0] = u;
             valset[1] = v;
             valset[2] = w;
             
             // send the data value
             my_pgroup->send_reals( src, 3, valset, PGR_TAG_RECV );
                          
             break;
          }

         
//std::cerr << "serveMet: [" << my_pgroup->id() << "] looping" << std::endl;
      
      }
      
      // sync with the clients;
      my_pgroup->sync();
   }
//std::cerr << "serveMet: [" << my_pgroup->id() << "] exit" << std::endl;

};

