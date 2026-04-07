
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

#include "math.h"

#include "gigatraj/MetGridLatLonData.hh"

using namespace gigatraj;


// default constructor
MetGridLatLonData::MetGridLatLonData() : MetGridData() 
{

      delete us;
      delete vs;
      delete ws;
      
      us = new MetCache3D(wind_ew_name, maxsnaps);
      vs = new MetCache3D(wind_ns_name, maxsnaps);
      ws = new MetCache3D(wind_vert_name, maxsnaps);

      field3Ds.clear();
      field2Ds.clear();
          
     //x3D = new GridLatLonField3D();
     //xSfc = new GridLatLonFieldSfc();

}



// full constructor
MetGridLatLonData::MetGridLatLonData(
        const std::string quantity, const std::string units
      , const std::vector<real>& xzs
      , const std::vector<real>& xlons
      , const std::vector<real>& xlats
      ) : MetGridData()  
{
      real val;
     
      // todo: check zs for monotonicity
      nzs = xzs.size();
      zs = xzs;
     
      vquant = quantity;
      vuu = units;
     
      delete us;
      delete vs;
      delete ws;
      
      us = new MetCache3D(wind_ew_name, maxsnaps);
      vs = new MetCache3D(wind_ns_name, maxsnaps);
      ws = new MetCache3D(wind_vert_name, maxsnaps);

      field3Ds.clear();
      field2Ds.clear();

     //x3D = new GridLatLonField3D();
     //x3D->setPgroup( my_pgroup, my_metproc );

     //xSfc = new GridLatLonFieldSfc();
     //xSfc->setPgroup( my_pgroup, my_metproc );
};

// destructor
MetGridLatLonData::~MetGridLatLonData() 
{
   //delete x3D;
   //delete xSfc;
}

// copy constructor
MetGridLatLonData::MetGridLatLonData( const MetGridLatLonData&  src) : MetGridData(src)
{
    nlons = src.nlons;
    nlats = src.nlats;
    nzs   = src.nzs;    
    lons  = src.lons;
    lats  = src.lats;
    zs    = src.zs;

}    

void MetGridLatLonData::assign(const MetGridLatLonData& src)
{
    MetGridData::assign(src);
    
    nlons = src.nlons;
    nlats = src.nlats;
    nzs   = src.nzs;    
    lons  = src.lons;
    lats  = src.lats;
    zs    = src.zs;

}    

MetGridData& MetGridLatLonData::operator=(const MetGridData& src)
{
    const MetGridLatLonData* ssrc;
    
    ssrc = dynamic_cast<const MetGridLatLonData*>(&src);
    // handle assignment to self
    if ( this == ssrc ) {
       return *this;
    }
    
    assign(*ssrc);

    return *this;
}


void MetGridLatLonData::setOption( const std::string &name, const std::string &value )
{    
     MetGridData::setOption( name, value ); 
}
void MetGridLatLonData::setOption( const std::string &name, int value )
{
     MetGridData::setOption( name, value ); 
}

void MetGridLatLonData::setOption( const std::string &name, float value )
{
     MetGridData::setOption( name, value ); 
}

void MetGridLatLonData::setOption( const std::string &name, double value )
{
     MetGridData::setOption( name, value ); 
}

bool MetGridLatLonData::getOption( const std::string &name, std::string &value )
{
    bool result;
    
    result = MetGridData::getOption( name, value ); 
    
    return result;
}

bool MetGridLatLonData::getOption( const std::string &name, int &value )
{
    bool result;
    
    result = MetGridData::getOption( name, value ); 

    return result;
}


bool MetGridLatLonData::getOption( const std::string &name, float &value )
{
    bool result;
    
    result = MetGridData::getOption( name, value ); 

    return result;
}


bool MetGridLatLonData::getOption( const std::string &name, double &value )
{
    bool result;
    
    result = MetGridData::getOption( name, value ); 

    return result;
}





real MetGridLatLonData::get_u( double time, real lon, real lat, real z)
{
    return getData(wind_ew_name, time, lon, lat, z, METDATA_MKS);
}

real MetGridLatLonData::get_v( double time, real lon, real lat, real z)
{
    return getData(wind_ns_name, time, lon, lat, z, METDATA_MKS);
}

real MetGridLatLonData::get_w( double time, real lon, real lat, real z)
{
    if ( isoVertical() ) {
       return 0.0;
    } else {   
       return wfctr*getData(wind_vert_name, time, lon, lat, z, METDATA_MKS);
    }
}


void MetGridLatLonData::get_uvw( double time, real lon, real lat, real z, real *u, real *v, real *w)
{
    real uu, vv;
    //*u = getData(wind_ew_name, time, lon, lat, z, METDATA_MKS | METDATA_THROWBAD );
    //*v = getData(wind_ns_name, time, lon, lat, z, METDATA_MKS | METDATA_THROWBAD );
    //
    getVectorData( wind_ew_name, wind_ns_name, uu, vv, time, lon, lat, z, METDATA_MKS | METDATA_THROWBAD );
    *u = uu;
    *v = vv;

    if ( isoVertical() ) {
       *w = 0;
    } else {   
       *w = wfctr*getData(wind_vert_name, time, lon, lat, z, METDATA_MKS | METDATA_THROWBAD );
    }
    if ( dbug >= 100 ) {
       std::cerr << "   @@@  u(" << lon << "," << lat << "," << z << "," << time2Cal(time) << ") = " << *u << std::endl;
       std::cerr << "   @@@  v(" << lon << "," << lat << "," << z << "," << time2Cal(time) << ") = " << *v << std::endl;
       std::cerr << "   @@@  w(" << lon << "," << lat << "," << z << "," << time2Cal(time) << ") = " << *w << std::endl;       
    }
}

void  MetGridLatLonData::get_uvw( double time, int n, real* lons, real* lats, real* zs, real *u, real *v, real *w)
{
    real uu, vv;
    real lat,lon,z;
    int flag;

    getVectorData( n, wind_ew_name, wind_ns_name, u, v, time, lons, lats, zs, METDATA_MKS | METDATA_NANBAD );
    //getData( wind_ew_name, time, n, lons, lats, zs, u, METDATA_MKS | METDATA_NANBAD );
    //getData( wind_ns_name, time, n, lons, lats, zs, v, METDATA_MKS | METDATA_NANBAD );
    if ( isoVertical() ) {
       for ( int i=0; i < n; i++ ) {
           w[i] = 0;
       }
    } else {   
       getData( wind_vert_name, time, n, lons, lats, zs, w, METDATA_MKS | METDATA_NANBAD );
       for ( int i=0; i < n; i++ ) {
           if ( FINITE(w[i]) != 0 ) {
              w[i] = wfctr*w[i];
           }
       }
    }

}

int MetGridLatLonData::setup(  const std::string quantity, const double time )
{
    int ndims = 3;
    
    // logic goes here for inspecting the quantity and
    // setting ndims =2 if necessary.
    
    return ndims;
}    

int MetGridLatLonData::setup(  const std::string quantity, const std::string &time )
{
    int ndims = 3;

    // logic goes here for inspecting the quantity and
    // setting ndims =2 if necessary.
    
    return ndims;
}    


real MetGridLatLonData::getData( string quantity, double time, real lon, real lat, real z, int flags)
{
     GridLatLonField3D  *g1, *g2;
     GridLatLonFieldSfc *s1, *s2;
     double t1, t2;
     std:string ct1, ct2;
     double tt1,tt2;
     real val1, val2;
     real result;
     int ndims;
     std::string sfcQuant;
     std::string sfcType;
     size_t pos;
     std::string cxtime;
     bool is_valid;
     real badval;
     const char *nanstr = "";

     //- std::cerr << "====MetGridLatLonData::getData Entry" << std::endl;  

     // handle the special case of the quantity being a simple function of the vertical coordinate
     if ( quantity == vquant ) {
        // the simplest function of all: identity
        return z;
     }
     if ( quantity == palt_name && vquant == pressure_name ) {
        // given pressure (transformed to Pa), find palt (in km)
        val1 = (z*vMKSscale + vMKSoffset);
        val2 = getpalt.calc( val1 );
        return val2;
     }
     if ( quantity == pressure_name && vquant == palt_name ) {
        // given Palt (transformed to km), find pressure (transformed to hPa)
        val1 = (z*vMKSscale + vMKSoffset)/1000.0;
        val2 = getpalt.clac( val1 )/100.0;
        return val2;
     }


     // do what we need to do before we start readinf the data
     // (e.g. open URL, verify that the quantity is legal, etc.)      
     ndims = this->setup(quantity, time);
          
     // A quantity of the form "quant1@quant2) is quant1 on Surface quant2.
     if (  (pos = quantity.find("@")) != std::string::npos ) {
        // it's a surface being requested
        ndims = 2;
     }
     
     //std::cerr << "==== 1: in getdata for " << ndims << " dims " << std::endl;        
     
     // change internal model time to met source calendar time string   
     cxtime = time2Cal( time, 4 ); 
     
     // send the numeric time to bracket, so we can use the full numeric precision
     bracket( quantity, time, &tt1, &tt2);
     ct1 = time2Cal(tt1);
     ct2 = time2Cal(tt2);
     
     if ( dbug > 2 ) {
        std::cerr << "MetGridLatLonData::getData: bracketing source data time " << time2Cal( time ) << "(" << time << ") between " << ct1 << " and " << ct2 << std::endl;        
     }
     //- std::cerr << "bracket times " << time << " = " << ct1 << " vs " << ct2 << std::endl;
     //- std::cerr << "==== 2: in getdata for " << ndims << " dims " << std::endl;        
     

     // start off assuming the data are valid
     is_valid = true;
     
     if ( ndims == 3 ) {
     
        //- std::cerr << ".... TIME(getData) = " << time << " (" << cxtime << ")" << std::endl;
        
        // ========= first, get the 3D grid at time ct1
        
        /* The downcasting is necessary because the horizontal interpolation
           routines need the child class, not the abstract parent class.
           We use our abstract parent's new_mgmtGrid3D() instead of our own new_directGrid3D()
           because the former handles caching.
           Note there is no real danger here: new_mgmtGrid3D() calls our own
           new_directGrid3D() to fill its caches, and our new_directGrid3D() always 
           returns a GridLatLonField3D.
        */   
        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getData: ==== about to fetch 3D field" << std::endl;        
        }
        g1 = dynamic_cast<GridLatLonField3D*>(new_mgmtGrid3D( quantity, ct1 ));
        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getData: ==== about to request 3D field" << std::endl;        
        }
        
        // if we are a met client, then alert the met server that we are about to request data gridpoints
        request_data3D(quantity,ct1);
        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getData: ==== about to spatially interpolate the 3D field" << std::endl;   
        }        
        
        badval = g1->fillval();
        try {
           if ( receive_svr_status() == PGR_STATUS_OK ) {
              val1 = hin->vinterp( lon, lat, z, *g1, *vin );
           } else {
              throw (badmetfailure());
           }    
        } catch (...) {
           val1 = badval;
        }
        g1->svr_done();
        is_valid = ( val1 != badval );
        //- std::cerr << " MetGridLatLonData::getData:: is_valid=" << is_valid << std::endl;        
        if ( is_valid && (flags & METDATA_MKS) ) {
           val1 = val1 * g1->mksScale + g1->mksOffset;
        }     
        t1 = g1->time();
        remove(g1);

        if ( is_valid && (ct1 != ct2) ) {
           if ( dbug > 2 ) {
              std::cerr << "MetGridLatLonData::getData: ==== about to get data for the second of bracketed times for 3D field" << std::endl; 
           }          
           g2 = dynamic_cast<GridLatLonField3D*>(new_mgmtGrid3D( quantity, ct2 ));     
           request_data3D(quantity,ct2);
           try {
              if ( receive_svr_status() == PGR_STATUS_OK ) {
                 val2 = hin->vinterp( lon, lat, z, *g2, *vin );
              } else {
                 throw (badmetfailure());
              }
           } catch (...) {
              val2 = g2->fillval();
           }
           g2->svr_done();
           is_valid = ( val2 != g2->fillval() );
           if ( is_valid && (flags & METDATA_MKS) ) {
              val2 = val2 * g2->mksScale + g2->mksOffset;
           }     
           t2 = g2->time();
           remove(g2);
        } else {
           val2 = 0.0;
           t2 = t1 + 1.0;
        } 
        
     } else {
        // surface data
        if ( dbug> 2 ) {
            std::cerr << "MetGridLatLonData::getData: ==== getting a 2d field" << std::endl;        
        }

        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getData: ==== about to fetch Sfc field" << std::endl;        
        }
        s1 = dynamic_cast<GridLatLonFieldSfc*>(new_mgmtGridSfc( quantity, ct1 ));
        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getData: ==== about to request Sfc field" << std::endl;        
        }
        request_dataSfc(quantity,ct1);
        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getData: ==== about to spatially interpolate the Sfc field" << std::endl;   
        }        
        badval = s1->fillval();
        try {
           if ( receive_svr_status() == PGR_STATUS_OK ) {
              val1 = hin->calc( lon, lat, *s1 ); 
           } else {
              throw (badmetfailure());
           }
        } catch (...) {
           val1 = badval;
        }
        s1->svr_done();
        is_valid = ( val1 != badval );
        if ( is_valid && (flags & METDATA_MKS) ) {
           val1 = val1 * s1->mksScale + s1->mksOffset;
        }     
        t1 = s1->time();
        remove(s1);

        if ( is_valid && ( ct1 != ct2 ) ) {
           if ( dbug > 2 ) {
              std::cerr << "MetGridLatLonData::getData: ==== about to get data for the second of bracketed times for Sfc field" << std::endl; 
           }          
           s2 = dynamic_cast<GridLatLonFieldSfc*>(new_mgmtGridSfc( quantity, ct2 ));
           request_dataSfc(quantity,ct2);
           try {
              if ( receive_svr_status() == PGR_STATUS_OK ) { 
                 val2 = hin->calc( lon, lat, *s2 );          
              } else {                                       
                 throw (badmetfailure());                    
              }                                              
           } catch (...) {
              val2 = s2->fillval();
           }
           s2->svr_done();
           is_valid = ( val2 != s2->fillval() );
           if ( is_valid && (flags & METDATA_MKS) ) {
              val2 = val2 * s2->mksScale + s2->mksOffset;
           }     
           t2 = s2->time();
           remove(s2);
        } else {
           val2 = 0.0;
           t2 = t1 + 1.0;
        }
        
     } 
  
     if ( dbug > 3 ) {   
        std::cerr << "MetGridLatLonData::getData: ==== about to time interpolate between " << val1 << " and " << val2 << std::endl; 
        //std::cerr << "val1=" << val1 << ", t1=" << t1 << std::endl;     
        //std::cerr << "val2=" << val2 << ", t2=" << t2 << std::endl;
        //std::cerr << "time = " << time << std::endl;       
     }
     if ( is_valid ) {
        result = val1*(t2-time)/(t2-t1) + val2*(time-t1)/(t2-t1);
     } else {
        result = badval;
     }
     if ( dbug >= 30 ) {
        std::cerr << "MetGridLatLonData::getData:___ quantity, lon, lat, z= " << quantity << ", " << lon << ", " << lat << ", " << z << std::endl;
        std::cerr << "MetGridLatLonData::getData:___ t-interp " << ct1 << "(" << t1 << ")" << ", " << val1 << std::endl;
        std::cerr << "MetGridLatLonData::getData:___          " << ct2 << "(" << t2 << ")" << ", " << val2 << std::endl;
        std::cerr << "MetGridLatLonData::getData:___        = " << time << ", " << result << std::endl;
     }
     
     if ( dbug > 3 ) {   
        std::cerr << "MetGridLatLonData::getData: ==== returning " << result << std::endl; 
     } 

     if ( ! is_valid ) {
        if ( flags & METDATA_KEEPBAD ) {
           // do nothing;
        } else if ( flags & METDATA_INFBAD ) {
           result = INFINITY;
        } else if ( flags & METDATA_THROWBAD ) {
           throw (badmetdata());
        } else if ( flags & METDATA_NANBAD ) {
           result = RNAN(nanstr);
        } else {
           result = RNAN(nanstr);
        }
     }

     return result;

}

void MetGridLatLonData::getVectorData( string lonquantity, string latquantity, real &lonval, real &latval, double time, real lon, real lat, real z, int flags)
{
     GridLatLonField3D  *gx1, *gy1, *gx2, *gy2;
     GridLatLonFieldSfc *sx1, *sy1, *sx2, *sy2;
     double t1, t2;
     std:string ct1, ct2;
     double tt1,tt2;
     real lonval1, lonval2;
     real latval1, latval2;
     real result;
     int ndims;
     std::string sfcQuant;
     std::string sfcType;
     size_t pos, posy;
     std::string cxtime;
     bool is_valid;
     real xbadval, ybadval;
     const char *nanstr = "";

     // Note: this call to setup **should** suffice for both component quantities,
     // but there are no guarantees. (sigh)
     ndims = this->setup(lonquantity, time);

     
     if (  (pos = lonquantity.find("@")) != std::string::npos ) {
        // it's a surface being requested
        ndims = 2;
     }
     posy = latquantity.find("@");
     if ( posy != pos ) {
        throw (badIncompatibleVectors());
     }
     // should check here that the surfaces are the same
     
     
     cxtime = time2Cal( time, 4 ); 
     
     // send the numeric time to bracket, so we can use the full numeric precision
     bracket( lonquantity, time, &tt1, &tt2);
     ct1 = time2Cal(tt1);
     ct2 = time2Cal(tt2);
     
     if ( dbug > 2 ) {
        std::cerr << "tt1=" << tt1 << ", ct1=" << ct1 << std::endl;
        std::cerr << "tt2=" << tt2 << ", ct2=" << ct2 << std::endl;
        std::cerr << "MetGridLatLonData::getVectorData: bracketing source data time " << time2Cal( time ) << "(" << time << ") between " << ct1 << " and " << ct2 << std::endl;        
     }
     
     // start off assuming the data are valid
     is_valid = true;
     
     if ( ndims == 3 ) {
     
        /* The downcasting is necessary because the horizontal interpolation
           routines need the child class, not the abstract parent class.
           We use our abstract parent's new_mgmtGrid3D() instead of our own new_directGrid3D()
           because the former handles caching.
           Note there is no real danger here: new_mgmtGrid3D() calls our own
           new_directGrid3D() to fill its caches, and our new_directGrid3D() always 
           returns a GridLatLonField3D.
        */   
        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getVectorData: ==== about to fetch lon 3D field" << std::endl;        
        }
        gx1 = dynamic_cast<GridLatLonField3D*>(new_mgmtGrid3D( lonquantity, ct1 ));
        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getVectorData: ==== about to request lon 3D field" << std::endl;        
        }

        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getVectorData: ==== about to fetch lat 3D field" << std::endl;        
        }
        gy1 = dynamic_cast<GridLatLonField3D*>(new_mgmtGrid3D( latquantity, ct1 ));
        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getVectorData: ==== about to request lat 3D field" << std::endl;        
        }

        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getVectorData: ==== about to spatially interpolate the 3D field" << std::endl;   
        }        
        xbadval = gx1->fillval();
        ybadval = gy1->fillval();
        try {
           //- std::cerr << "calling request_data3D /w result ";
           request_data3D(lonquantity,latquantity, ct1);
           if ( receive_svr_status() == PGR_STATUS_OK ) {
              //- std::cerr << " OK ";
              hin->vinterpVector( lon, lat, z, lonval1, latval1, *gx1, *gy1, *vin );
              //- std::cerr << " and vinterp ";
           } else {
              //- std::cerr << " not OK ";
              throw (badmetfailure());
           }    
        } catch (...) {
           //- std::cerr << " error ";
           lonval1 = xbadval;
           latval1 = ybadval;
        }
        //- std::cerr << std::endl;
        // note: each grid's svr_done() call is done by its gridpoints() call inside vinterp() 
        is_valid = ( (lonval1 != xbadval) && (latval1 != ybadval) );
        if ( is_valid && (flags & METDATA_MKS) ) {
           lonval1 = lonval1 * gx1->mksScale + gx1->mksOffset;
           latval1 = latval1 * gy1->mksScale + gy1->mksOffset;
        }     
        t1 = gx1->time();
        if ( t1 != gy1->time() ) {
           throw (badIncompatibleVectors());
        }
        remove(gy1);
        remove(gx1);

        if ( is_valid && (ct1 != ct2) ) {
           if ( dbug > 2 ) {
              std::cerr << "MetGridLatLonData::getVectorData: ==== about to get lon data for the second of bracketed times for 3D field" << std::endl; 
           }          
           gx2 = dynamic_cast<GridLatLonField3D*>(new_mgmtGrid3D( lonquantity, ct2 ));     
           if ( dbug > 2 ) {
              std::cerr << "MetGridLatLonData::getVectorData: ==== about to get lat data for the second of bracketed times for 3D field" << std::endl; 
           }          
           gy2 = dynamic_cast<GridLatLonField3D*>(new_mgmtGrid3D( latquantity, ct2 ));     
           
           try {
              request_data3D(lonquantity,latquantity,ct2);
              if ( receive_svr_status() == PGR_STATUS_OK ) {
                 hin->vinterpVector( lon, lat, z, lonval2, latval2, *gx2, *gy2, *vin );
              } else {
                 throw (badmetfailure());
              }    
           } catch (...) {
              lonval2 = gx2->fillval();
              latval2 = gy2->fillval();
           }
           is_valid = ( (lonval2 != gx2->fillval()) && (latval2 != gy2->fillval()) );
           if ( is_valid && (flags & METDATA_MKS) ) {
              lonval2 = lonval2 * gx2->mksScale + gx2->mksOffset;
              latval2 = latval2 * gy2->mksScale + gy2->mksOffset;
           }     
           t2 = gx2->time();
           if ( t2 != gy2->time() ) {
              throw (badIncompatibleVectors());
           }
           remove(gy2);
           remove(gx2);
        } else {
           lonval2 = 0.0;
           latval2 = 0.0;
           t2 = t1 + 1.0;
        } 
        
     } else {
        // surface data
        if ( dbug ) {
            std::cerr << "MetGridLatLonData::getVectorData: ==== getting a 2d field" << std::endl;        
        }

        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getVectorData: ==== about to fetch lon Sfc field" << std::endl;        
        }
        sx1 = dynamic_cast<GridLatLonFieldSfc*>(new_mgmtGridSfc( lonquantity, ct1 ));
        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getVectorData: ==== about to request lon Sfc field" << std::endl;        
        }
        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getVectorData: ==== about to fetch lat Sfc field" << std::endl;        
        }
        sy1 = dynamic_cast<GridLatLonFieldSfc*>(new_mgmtGridSfc( latquantity, ct1 ));
        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getVectorData: ==== about to request lat Sfc field" << std::endl;        
        }


        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getVectorData: ==== about to spatially interpolate the Sfc field" << std::endl;   
        }        
        xbadval = sx1->fillval();
        ybadval = sy1->fillval();
        try {
           request_dataSfc(lonquantity,latquantity,ct1);
           if ( receive_svr_status() == PGR_STATUS_OK ) {
              hin->calc(lon, lat, lonval1, latval1, *sx1, *sy1 );
           } else {
              throw (badmetfailure());
           }
        } catch (...) {
           lonval1 = xbadval;
           latval1 = ybadval;
        }
        is_valid = ( (lonval1 != xbadval) && ( latval1 != ybadval) );
        if ( is_valid && (flags & METDATA_MKS) ) {
           lonval1 = lonval1 * sx1->mksScale + sx1->mksOffset;
           latval1 = latval1 * sy1->mksScale + sy1->mksOffset;
        }     
        t1 = sx1->time();
        if ( t1 != sy1->time() ) {
           throw (badIncompatibleVectors());
        }
        remove(sy1);
        remove(sx1);

        if ( is_valid && ( ct1 != ct2 ) ) {
           if ( dbug > 2 ) {
              std::cerr << "MetGridLatLonData::getVectorData: ==== about to get data for the second of bracketed times for lon Sfc field" << std::endl; 
           }          
           sx2 = dynamic_cast<GridLatLonFieldSfc*>(new_mgmtGridSfc( lonquantity, ct2 ));
           if ( dbug > 2 ) {
              std::cerr << "MetGridLatLonData::getVectorData: ==== about to get data for the second of bracketed times for lat Sfc field" << std::endl; 
           }          
           sy2 = dynamic_cast<GridLatLonFieldSfc*>(new_mgmtGridSfc( latquantity, ct2 ));
           try {
              request_dataSfc(lonquantity,latquantity,ct2);
              if ( receive_svr_status() == PGR_STATUS_OK ) { 
                 hin->calc(lon, lat, lonval2, latval2, *sx2, *sy2 );
              } else {                                       
                 throw (badmetfailure());                    
              }                                              
           } catch (...) {
              lonval2 = sx2->fillval();
              latval2 = sy2->fillval();
           }
           is_valid = ( ( lonval2 != sx2->fillval() ) && ( latval2 != sy2->fillval() ) );
           if ( is_valid && (flags & METDATA_MKS) ) {
              lonval2 = lonval2 * sx2->mksScale + sx2->mksOffset;
              latval2 = latval2 * sy2->mksScale + sy2->mksOffset;
           }
           t2 = sx2->time();
           if ( t2 != sy2->time() ) {
              throw (badIncompatibleVectors());
           }
           remove(sy2);
           remove(sx2);
        } else {
           lonval2 = 0.0;
           latval2 = 0.0;
           t2 = t1 + 1.0;
        }
        
     } 
  
     if ( dbug > 3 ) {   
        std::cerr << "MetGridLatLonData::getVectorData: ==== about to time interpolate between " << lonval1 << " and " << lonval2 << std::endl; 
        std::cerr << "MetGridLatLonData::getVectorData: ==== about to time interpolate between " << latval1 << " and " << latval2 << std::endl; 
     }
     if ( is_valid ) {
        lonval = lonval1*(t2-time)/(t2-t1) + lonval2*(time-t1)/(t2-t1);
        latval = latval1*(t2-time)/(t2-t1) + latval2*(time-t1)/(t2-t1);
     } else {
        lonval = xbadval;
        latval = ybadval;
     }
     if ( dbug >= 100 ) {
        std::cerr << "___x t-interp " << ct1 << "(" << t1 << ")" << ", " << lonval1 << std::endl;
        std::cerr << "___x          " << ct2 << "(" << t2 << ")" << ", " << lonval2 << std::endl;
        std::cerr << "___x        = " << time << ", " << lonval << std::endl;
        std::cerr << "___y t-interp " << ct1 << "(" << t1 << ")" << ", " << latval1 << std::endl;
        std::cerr << "___y          " << ct2 << "(" << t2 << ")" << ", " << latval2 << std::endl;
        std::cerr << "___y        = " << time << ", " << latval << std::endl;
     }
     
     if ( dbug > 3 ) {   
        std::cerr << "MetGridLatLonData::getVectorData: ==== returning " << result << std::endl; 
     } 

     if ( ! is_valid ) {
        if ( flags & METDATA_KEEPBAD ) {
           // do nothing;
        } else if ( flags & METDATA_INFBAD ) {
           lonval = INFINITY;
           latval = INFINITY;
        } else if ( flags & METDATA_THROWBAD ) {
           throw (badmetdata());
        } else if ( flags & METDATA_NANBAD ) {
           lonval = RNAN(nanstr);
           latval = RNAN(nanstr);
        } else {
           lonval = RNAN(nanstr);
           latval = RNAN(nanstr);
        }
     }

}


void MetGridLatLonData::getData( string quantity, double time, int n, real* lons, real* lats, real* zs, real* values, int flags )
{
     GridLatLonField3D  *g1, *g2;
     GridLatLonFieldSfc *s1, *s2;
     double t1, t2;
     std:string ct1, ct2;
     double tt1,tt2;
     real val1, val2;
     real result;
     int ndims;
     std::string sfcQuant;
     std::string sfcType;
     size_t pos;
     std::string cxtime;
     bool is_valid;
     real badval, badval2;
     const char *nanstr = "";
     real lat,lon,z;


     real* const vals1 = new real[n];
     real* const vals2 = new real[n];
  
     // handle the special case of the quantity being a simple function of the vertical coordinate
     if ( quantity == palt_name && vquant == pressure_name ) {
        // given pressure (transformed to Pa), find palt (in km)
        for ( int i=0; i < n; i++ ) {
           val1 = (zs[i]*vMKSscale + vMKSoffset);
           values[i] = getpalt.calc( val1 );
        }   
        return;
     }
     if ( quantity == pressure_name && vquant == palt_name ) {
        // given Palt (transformed to km), find pressure (transformed to hPa)
        for ( int i=0; i < n; i++ ) {
           val1 = (zs[i]*vMKSscale + vMKSoffset)/1000.0;
           values[i]= getpalt.clac( val1 )/100.0;
        }
        return;
     }

     // do what we need to do before we start readinf the data
     // (e.g. open URL, verify that the quantity is legal, etc.)      
     ndims = this->setup(quantity, time);
          
     // A quantity of the form "quant1@quant2) is quant1 on Surface quant2.
     if (  (pos = quantity.find("@")) != std::string::npos ) {
        // it's a surface being requested
        ndims = 2;
     }
  
     
     //std::cerr << "==== 1: in getdata for " << ndims << " dims " << std::endl;        
     
     // change internalmodel time to met source calendar time string   
     cxtime = time2Cal( time, 4 ); 
     
     // send the numeric time to bracket, so we can use the full numeric precision
     bracket( quantity, time, &tt1, &tt2);
     ct1 = time2Cal(tt1);
     ct2 = time2Cal(tt2);
     
     if ( dbug > 2 ) {
        std::cerr << "MetGridLatLonData::getData: bracketing source data time " << time2Cal( time ) 
                  << "(" << time << ") between " << ct1 << " and " << ct2 << std::endl;        
     }
     //- std::cerr << "bracket times " << time << " = " << ct1 << " vs " << ct2 << std::endl;
     //- std::cerr << "==== 2: in getdata for " << ndims << " dims " << std::endl;        
     

     // start off assuming the data are valid
     is_valid = true;
     
     if ( ndims == 3 ) {
     
        //- std::cerr << ".... TIME(getData) = " << time << " (" << cxtime << ")" << std::endl;
        
        // ========= first, get the 3D grid at time ct1
        
        /* The downcasting is necessary because the horizontal interpolation
           routines need the child class, not the abstract parent class.
           We use our abstract parent's new_mgmtGrid3D() instead of our own new_directGrid3D()
           because the former handles caching.
           Note there is no real danger here: new_mgmtGrid3D() calls our own
           new_directGrid3D() to fill its caches, and our new_directGrid3D() always 
           returns a GridLatLonField3D.
        */   
        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getData: ==== about to fetch 3D field" << std::endl;        
        }
        g1 = dynamic_cast<GridLatLonField3D*>(new_mgmtGrid3D( quantity, ct1 ));
        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getData: ==== about to request 3D field" << std::endl;        
        }
        
        // if we are a met client, then alert the met server that we are about to request data gridpoints
        request_data3D(quantity,ct1);
        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getData: ==== about to spatially interpolate the 3D field" << std::endl;   
        }        
        
        badval = g1->fillval();
        try {
           if ( receive_svr_status() == PGR_STATUS_OK ) {
              hin->vinterp( n, lons, lats, zs, vals1, *g1, *vin  );
           } else {
              throw (badmetfailure());
           }    
        } catch (...) {
           for ( int i=0; i<n; i++ ) {
              vals1[i] = badval;
           }   
        }
        g1->svr_done();
        
        for ( int i=0; i<n; i++ ) {
            is_valid = ( vals1[i] != badval ) && ( FINITE(vals1[i]) );
            if ( is_valid ) {
               if (flags & METDATA_MKS) {
                  vals1[i] = vals1[i] * g1->mksScale + g1->mksOffset;
               }
            } else {
               vals1[i] = badval;
            }
        }    
        t1 = g1->time();
        remove(g1);

        if ( is_valid && (ct1 != ct2) ) {
           if ( dbug > 2 ) {
              std::cerr << "MetGridLatLonData::getData: ==== about to get data for the second of bracketed times for 3D field" << std::endl; 
           }          
           g2 = dynamic_cast<GridLatLonField3D*>(new_mgmtGrid3D( quantity, ct2 ));     
           request_data3D(quantity,ct2);
           try {
              if ( receive_svr_status() == PGR_STATUS_OK ) {
                 hin->vinterp( n, lons, lats, zs, vals2, *g2, *vin );
              } else {
                 throw (badmetfailure());
              }
           } catch (...) {
              for ( int i=0; i<n; i++ ) {
                  vals2[i] = g2->fillval();
              }
           }
           g2->svr_done();
           badval2 = g2->fillval();

           for ( int i=0; i<n; i++ ) {
              is_valid = ( vals2[i] != badval2 ) && ( FINITE(vals2[i]) );
              if ( is_valid ) {
                 if (flags & METDATA_MKS) {
                    vals2[i] = vals2[i] * g2->mksScale + g2->mksOffset;
                 }
              } else {
                 vals2[i] = badval;
              }
           }     
           t2 = g2->time();
           remove(g2);
        } else {
           for ( int i=0; i<n; i++ ) {
               vals2[i] = 0.0;
           }    
           t2 = t1 + 1.0;
        } 
        
     } else {
        // surface data
        if ( dbug> 2 ) {
            std::cerr << "MetGridLatLonData::getData: ==== getting a 2d field" << std::endl;        
        }

        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getData: ==== about to fetch Sfc field" << std::endl;        
        }
        s1 = dynamic_cast<GridLatLonFieldSfc*>(new_mgmtGridSfc( quantity, ct1 ));
        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getData: ==== about to request Sfc field" << std::endl;        
        }
        request_dataSfc(quantity,ct1);
        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getData: ==== about to spatially interpolate the Sfc field" << std::endl;   
        }        
        badval = s1->fillval();
        try {
           if ( receive_svr_status() == PGR_STATUS_OK ) {
              hin->calc( n, lons, lats, vals1, *s1 ); 
           } else {
              throw (badmetfailure());
           }
        } catch (...) {
           for ( int i=0; i<n; i++ ) {
              vals1[i] = badval;
           }
        }
        s1->svr_done();
       
        for ( int i=0; i<n; i++ ) {
            is_valid = ( vals1[i] != badval ) && ( FINITE(vals1[i]) ) ;
            if ( is_valid ) {
               if (flags & METDATA_MKS) {
                  vals1[i] = vals1[i] * s1->mksScale + s1->mksOffset;
               }
            } else {
               vals1[i] = badval;
            }
        }     
        t1 = s1->time();
        remove(s1);

        if ( ct1 != ct2 ) {
           if ( dbug > 2 ) {
              std::cerr << "MetGridLatLonData::getData: ==== about to get data for the second of bracketed times for Sfc field" << std::endl; 
           }          
           s2 = dynamic_cast<GridLatLonFieldSfc*>(new_mgmtGridSfc( quantity, ct2 ));
           request_dataSfc(quantity,ct2);
           try {
              if ( receive_svr_status() == PGR_STATUS_OK ) { 
                 hin->calc( n, lons, lats, vals2, *s2 ); 
              } else {                                       
                 throw (badmetfailure());                    
              }                                              
           } catch (...) {
              for ( int i=0; i<n; i++ ) {
                 vals2[i] = s2->fillval();
              }
           }
           s2->svr_done();
           badval2 = s2->fillval();
           
           for ( int i=0; i<n; i++ ) {
              is_valid = ( vals2[i] != badval2 ) && ( FINITE(vals2[i]) );
              if ( is_valid ) {
                 if (flags & METDATA_MKS) {
                    vals2[i] = vals2[i] * s2->mksScale + s2->mksOffset;
                 }
             } else {
                 vals2[i] = badval;
              }
           }     
           t2 = s2->time();
           remove(s2);
        } else {
           for ( int i=0; i<n; i++ ) {
              vals2[i] = 0.0;
           }
           t2 = t1 + 1.0;
        }
        
     } 
  
     if ( dbug > 3 ) {   
        std::cerr << "MetGridLatLonData::getData: ==== about to time interpolate between " << vals1[0] << " and " << vals2[0] << std::endl; 
        //std::cerr << "val1=" << val1 << ", t1=" << t1 << std::endl;     
        //std::cerr << "val2=" << val2 << ", t2=" << t2 << std::endl;
        //std::cerr << "time = " << time << std::endl;       
     }
     
     for ( int i=0; i<n; i++ ) {
        is_valid = ( vals1[i] != badval ) && ( vals2[i] != badval );
        if ( is_valid ) {
           result = vals1[i]*(t2-time)/(t2-t1) + vals2[i]*(time-t1)/(t2-t1);
        } else {
           if ( flags & METDATA_KEEPBAD ) {
              // do nothing;
           } else if ( flags & METDATA_INFBAD ) {
              result = INFINITY;
           } else if ( flags & METDATA_THROWBAD ) {
              throw (badmetdata());
           } else if ( flags & METDATA_NANBAD ) {
              result = RNAN(nanstr);
           } else {
              result = RNAN(nanstr);
           }
        }
        values[i] = result;
     }   
     if ( dbug >= 30 ) {
        std::cerr << "MetGridLatLonData::getData:___ quantity, lon, lat, z= " << quantity << ", " << lons[0] << ", " << lats[0] << ", " << zs[0] << std::endl;
        std::cerr << "MetGridLatLonData::getData:___ t-interp " << ct1 << "(" << t1 << ")" << ", " << vals1[0] << std::endl;
        std::cerr << "MetGridLatLonData::getData:___          " << ct2 << "(" << t2 << ")" << ", " << vals2[0] << std::endl;
        std::cerr << "MetGridLatLonData::getData:___        = " << time << ", " << values[0] << std::endl;
     }
     
     delete[] vals2;
     delete[] vals1;
     
     if ( dbug > 3 ) {   
        std::cerr << "MetGridLatLonData::getData: ==== returning " << result << std::endl; 
     } 

}


void MetGridLatLonData::getVectorData( int n, string lonquantity, string latquantity, real* lonvals, real *latvals, double time, real* lons, real* lats, real* zs, int flags)
{
     GridLatLonField3D  *gx1, *gy1, *gx2, *gy2;
     GridLatLonFieldSfc *sx1, *sy1, *sx2, *sy2;
     double t1, t2;
     std:string ct1, ct2;
     double tt1,tt2;
     real lonval1, lonval2;
     real latval1, latval2;
     real result;
     int ndims;
     std::string sfcQuant;
     std::string sfcType;
     size_t pos, posy;
     std::string cxtime;
     bool is_valid;
     real xbadval, ybadval;
     real xbadval2, ybadval2;
     const char *nanstr = "";
     real lat,lon,z;
     real lonval, latval;


     // Note: this call to setup **should** suffice for both component quantities,
     // but there are no guarantees. (sigh)
     ndims = this->setup(lonquantity, time);

     
     if (  (pos = lonquantity.find("@")) != std::string::npos ) {
        // it's a surface being requested
        ndims = 2;
     }
     posy = latquantity.find("@");
     if ( posy != pos ) {
        throw (badIncompatibleVectors());
     }
     // should check here that the surfaces are the same
     
     
     cxtime = time2Cal( time, 4 ); 
     
     // send the numeric time to bracket, so we can use the full numeric precision
     bracket( lonquantity, time, &tt1, &tt2);
     ct1 = time2Cal(tt1);
     ct2 = time2Cal(tt2);
     
     if ( dbug > 2 ) {
        std::cerr << "tt1=" << tt1 << ", ct1=" << ct1 << std::endl;
        std::cerr << "tt2=" << tt2 << ", ct2=" << ct2 << std::endl;
        std::cerr << "MetGridLatLonData::getVectorData: bracketing source data time " << time2Cal( time ) << "(" << time << ") between " << ct1 << " and " << ct2 << std::endl;        
     }
     
     // start off assuming the data are valid
     is_valid = true;
     
     
     real* const lonvals1 = new real[n];
     real* const lonvals2 = new real[n];
     real* const latvals1 = new real[n];
     real* const latvals2 = new real[n];
  
     if ( ndims == 3 ) {
     
        /* The downcasting is necessary because the horizontal interpolation
           routines need the child class, not the abstract parent class.
           We use our abstract parent's new_mgmtGrid3D() instead of our own new_directGrid3D()
           because the former handles caching.
           Note there is no real danger here: new_mgmtGrid3D() calls our own
           new_directGrid3D() to fill its caches, and our new_directGrid3D() always 
           returns a GridLatLonField3D.
        */   
        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getVectorData-vector: ==== about to fetch lon 3D field" << std::endl;        
        }
        gx1 = dynamic_cast<GridLatLonField3D*>(new_mgmtGrid3D( lonquantity, ct1 ));
        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getVectorData-vector: ==== about to request lon 3D field" << std::endl;        
        }

        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getVectorData-vector: ==== about to fetch lat 3D field" << std::endl;        
        }
        gy1 = dynamic_cast<GridLatLonField3D*>(new_mgmtGrid3D( latquantity, ct1 ));
        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getVectorData-vector: ==== about to request lat 3D field" << std::endl;        
        }

        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getVectorData-vector: ==== about to spatially interpolate the 3D field" << std::endl;   
        }        
        xbadval = gx1->fillval();
        ybadval = gy1->fillval();
        try {
           request_data3D(lonquantity,latquantity, ct1);
           if ( receive_svr_status() == PGR_STATUS_OK ) {
              hin->vinterpVector( n, lons, lats, zs, lonvals1, latvals1, *gx1, *gy1, *vin );
           } else {
              throw (badmetfailure());
           }    
        } catch (...) {
           for ( int i=0; i<n; i++ ) {
              lonvals1[i] = xbadval;
              latvals1[i] = ybadval;
           }
        }
        // note: each grid's svr_done() call is done by its gridpoints() call inside vinterpVector() 
        for ( int i=0; i<n; i++ ) {
           is_valid = ( (lonvals1[i] != xbadval) && FINITE(lonvals1[i]) 
                     && (latvals1[i] != ybadval) && FINITE(latvals1[i]) );
           if ( is_valid ) {
              if (flags & METDATA_MKS) {
                 lonvals1[i] = lonvals1[i] * gx1->mksScale + gx1->mksOffset;
                 latvals1[i] = latvals1[i] * gy1->mksScale + gy1->mksOffset;
              }
           } else {
               lonvals1[i] = xbadval;
               latvals1[i] = ybadval;
           }
        }     
        t1 = gx1->time();
        if ( t1 != gy1->time() ) {
           throw (badIncompatibleVectors());
        }
        remove(gy1);
        remove(gx1);

        if ( ct1 != ct2 ) {
           if ( dbug > 2 ) {
              std::cerr << "MetGridLatLonData::getVectorData-vector: ==== about to get lon data for the second of bracketed times for 3D field" 
                        << std::endl; 
           }          
           gx2 = dynamic_cast<GridLatLonField3D*>(new_mgmtGrid3D( lonquantity, ct2 ));     
           if ( dbug > 2 ) {
              std::cerr << "MetGridLatLonData::getVectorData-vector: ==== about to get lat data for the second of bracketed times for 3D field" 
                        << std::endl; 
           }          
           gy2 = dynamic_cast<GridLatLonField3D*>(new_mgmtGrid3D( latquantity, ct2 ));     
           xbadval = gx2->fillval();
           ybadval = gy2->fillval();
           
           try {
              request_data3D(lonquantity,latquantity,ct2);
              if ( receive_svr_status() == PGR_STATUS_OK ) {
                 hin->vinterpVector( n, lons, lats, zs, lonvals2, latvals2, *gx2, *gy2, *vin );
              } else {
                 throw (badmetfailure());
              }    
           } catch (...) {
              for ( int i=0; i<n; i++ ) {
                 lonvals2[i] = gx2->fillval();
                 latvals2[i] = gy2->fillval();
              }
           }
           for ( int i=0; i<n; i++ ) {
              is_valid = ( (lonvals2[i] != gx2->fillval()) && FINITE(lonvals2[i]) 
                        && (latvals2[i] != gy2->fillval()) && FINITE(latvals2[i]) );
              if ( is_valid ) {
                 if ( flags & METDATA_MKS ) {
                    lonvals2[i] = lonvals2[i] * gx2->mksScale + gx2->mksOffset;
                    latvals2[i] = latvals2[i] * gy2->mksScale + gy2->mksOffset;
                 }
              } else {
                  lonvals2[i] = xbadval;
                  latvals2[i] = ybadval;
              }
           }     
           t2 = gx2->time();
           if ( t2 != gy2->time() ) {
              throw (badIncompatibleVectors());
           }
           remove(gy2);
           remove(gx2);
        } else {
           for ( int i=0; i<n; i++ ) {
               lonvals2[i] = 0.0;
               latvals2[i] = 0.0;
           }
           t2 = t1 + 1.0;
        } 
        
     } else {

        // surface data

        if ( dbug ) {
            std::cerr << "MetGridLatLonData::getVectorData-vector: ==== getting a 2d field" << std::endl;        
        }

        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getVectorData-vector: ==== about to fetch lon Sfc field" << std::endl;        
        }
        sx1 = dynamic_cast<GridLatLonFieldSfc*>(new_mgmtGridSfc( lonquantity, ct1 ));
        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getVectorData-vector: ==== about to request lon Sfc field" << std::endl;        
        }
        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getVectorData-vector: ==== about to fetch lat Sfc field" << std::endl;        
        }
        sy1 = dynamic_cast<GridLatLonFieldSfc*>(new_mgmtGridSfc( latquantity, ct1 ));
        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getVectorData-vector: ==== about to request lat Sfc field" << std::endl;        
        }


        if ( dbug > 2 ) {
           std::cerr << "MetGridLatLonData::getVectorData-vector: ==== about to spatially interpolate the Sfc field" << std::endl;   
        }        
        xbadval = sx1->fillval();
        ybadval = sy1->fillval();
        try {
           request_dataSfc(lonquantity,latquantity,ct1);
           if ( receive_svr_status() == PGR_STATUS_OK ) {
              hin->calc(n, lons, lats, lonvals1, latvals1, *sx1, *sy1 );
           } else {
              throw (badmetfailure());
           }
        } catch (...) {
           for ( int i=0; i<n; i++ ) {
               lonvals1[i] = xbadval;
               latvals1[i] = ybadval;
           }
        }
        for ( int i=0; i<n; i++ ) {
            is_valid = ( (lonvals1[i] != xbadval) && FINITE(lonvals1[i]) 
                      && (latvals1[i] != ybadval) && FINITE(latvals1[i]) );
            if ( is_valid ) {
               if (flags & METDATA_MKS) {
                  lonvals1[i] = lonvals1[i] * sx1->mksScale + sx1->mksOffset;
                  latvals1[i] = latvals1[i] * sy1->mksScale + sy1->mksOffset;
               }
            } else {
               lonvals1[i] = xbadval;
               latvals1[i] = ybadval;
            }
        }     
        t1 = sx1->time();
        if ( t1 != sy1->time() ) {
           throw (badIncompatibleVectors());
        }
        remove(sy1);
        remove(sx1);

        if ( ct1 != ct2 ) {
           if ( dbug > 2 ) {
              std::cerr << "MetGridLatLonData::getVectorData-vector: ==== about to get data for the "
                        << "second of bracketed times for lon Sfc field" 
                        << std::endl; 
           }          
           sx2 = dynamic_cast<GridLatLonFieldSfc*>(new_mgmtGridSfc( lonquantity, ct2 ));
           if ( dbug > 2 ) {
              std::cerr << "MetGridLatLonData::getVectorData-vector: ==== about to get data for the second of bracketed times for lat Sfc field" 
                        << std::endl; 
           }          
           sy2 = dynamic_cast<GridLatLonFieldSfc*>(new_mgmtGridSfc( latquantity, ct2 ));
           try {
              request_dataSfc(lonquantity,latquantity,ct2);
              if ( receive_svr_status() == PGR_STATUS_OK ) { 
                 hin->calc(n, lons, lats, lonvals2, latvals2, *sx2, *sy2 );
              } else {                                       
                 throw (badmetfailure());                    
              }                                              
           } catch (...) {
              for (int i=0; i<n; i++ ) {
                  lonvals2[i] = sx2->fillval();
                  latvals2[i] = sy2->fillval();
              }
           }
           for ( int i=0; i<n; i++ ) {
              is_valid = ( (lonvals2[i] != sx2->fillval()) && FINITE(lonvals2[i]) 
                        && (latvals2[i] != sy2->fillval()) && FINITE(latvals2[i]) );
              if ( is_valid ) {
                 if (flags & METDATA_MKS) {
                    lonvals2[i] = lonvals2[i] * sx2->mksScale + sx2->mksOffset;
                    latvals2[i] = latvals2[i] * sy2->mksScale + sy2->mksOffset;
                 }
              }
           }
           t2 = sx2->time();
           if ( t2 != sy2->time() ) {
              throw (badIncompatibleVectors());
           }
           remove(sy2);
           remove(sx2);
        } else {
           lonval2 = 0.0;
           latval2 = 0.0;
           t2 = t1 + 1.0;
        }
        
     } 
  
     if ( dbug > 3 ) {   
        std::cerr << "MetGridLatLonData::getVectorData-vector: ==== about to time interpolate between " 
                  << lonvals1[0] << " and " << lonvals2[0] << std::endl; 
        std::cerr << "MetGridLatLonData::getVectorData-vector: ==== about to time interpolate between " 
                  << latvals1[0] << " and " << latvals2[0] << std::endl; 
     }
     
     for ( int i=0; i<n; i++  ) {
         is_valid = (lonvals1[i] != xbadval) && (lonvals2[i] != xbadval)
                 && (latvals1[i] != ybadval) && (latvals2[i] != ybadval);
         if ( is_valid ) {
            lonvals[i] = lonvals1[i]*(t2-time)/(t2-t1) + lonvals2[i]*(time-t1)/(t2-t1);
            latvals[i] = latvals1[i]*(t2-time)/(t2-t1) + latvals2[i]*(time-t1)/(t2-t1);
         } else {
            lonvals[i] = xbadval;
            latvals[i] = ybadval;
         }

         if ( dbug >= 100 && i == 0 ) {
            std::cerr << "@___@x t-interp " << ct1 << "(" << t1 << ")" << ", " << lonvals1[i] << std::endl;
            std::cerr << "@___@x          " << ct2 << "(" << t2 << ")" << ", " << lonvals2[i] << std::endl;
            std::cerr << "@___@x        = " << time << ", " << lonvals[i] << std::endl;
            std::cerr << "@___@y t-interp " << ct1 << "(" << t1 << ")" << ", " << latvals1[i] << std::endl;
            std::cerr << "@___@y          " << ct2 << "(" << t2 << ")" << ", " << latvals2[i] << std::endl;
            std::cerr << "@___@y        = " << time << ", " << latvals[i] << std::endl;
         }
     
         if ( dbug > 3 ) {   
            std::cerr << "MetGridLatLonData::getVectorData: ==== returning " << result << std::endl; 
         } 

         if ( ! is_valid ) {
            if ( flags & METDATA_KEEPBAD ) {
               // do nothing;
            } else if ( flags & METDATA_INFBAD ) {
               lonvals[i] = INFINITY;
               latvals[i] = INFINITY;
            } else if ( flags & METDATA_THROWBAD ) {
               throw (badmetdata());
            } else if ( flags & METDATA_NANBAD ) {
               lonvals[i] = RNAN(nanstr);
               latvals[i] = RNAN(nanstr);
            } else {
               lonvals[i] = RNAN(nanstr);
               latvals[i] = RNAN(nanstr);
            }
         }
     }
     
     delete[] latvals2;
     delete[] latvals1;
     delete[] lonvals2;
     delete[] lonvals1;
     
}


GridField3D* MetGridLatLonData::new_clientGrid3D( const std::string& quantity, const std::string& time )
{
   int cmd;
   int status;
   GridLatLonField3D* result;
   double ttime;
   
   //- std::cerr << "MetLatLonGridData::new_clientGrid3D:  (met client) entry " << std::endl;

   ttime = cal2Time(time);
   // send a request to the met processor's met source object
   request_meta3D( quantity, ttime );
   //- std::cerr << "MetLatLonGridData::new_clientGrid3D:  (met client) send request_meta3D " << std::endl;

   // while the server is getting the data ready for us,
   // set up a grid to use in receiving data
   
   // create the new grid
   result = new GridLatLonField3D();
   result->clear();
   result->set_quantity( quantity );
   result->set_time( ttime, time ) ;
   result->setPgroup( my_pgroup, my_metproc );    
   //- std::cerr << "MetLatLonGridData::new_clientGrid3D:  (met client) set grid pgroup " << std::endl;

   // was the met processor able to come up with the data?
   status = receive_svr_status();
   if ( status == PGR_STATUS_OK ) {
       //- std::cerr << "MetLatLonGridData::new_clientGrid3D:  (met client) status is OK " << std::endl;
   
       // ask for grid metadata
       result->ask_for_meta();
       //- std::cerr << "MetLatLonGridData::new_clientGrid3D:  (met client) asked for metadata " << std::endl;
   
       // receive grid metadata
       result->receive_meta();
       if ( dbug >= 1 ) {
          std::cerr << "MetLatLonGridData::new_clientGrid3D:  (met client) received metadata from met processor" << std::endl;
       }    

       result->svr_done();

   } else {
      // the met processors could not get its data grid,
      // so we fail, too.
      //- std::cerr << "MetLatLonGridData::new_clientGrid3D:  (met client) status is FAILURE " << std::endl;
      delete result;
      result = NULLPTR;
   }
   
   return result;
}
          
GridFieldSfc* MetGridLatLonData::new_clientGridSfc( const std::string& quantity, const std::string& time )
{
   int cmd;
   int status;
   GridLatLonFieldSfc* result;
   double ttime;
   
   ttime = cal2Time(time);
   //- std::cerr << "MetLatLonGridData::new_clientGridSfc:  (met client) entry " << std::endl;

   // send a request to the met processor's met source object
   request_metaSfc( quantity, ttime );
   //- std::cerr << "MetLatLonGridData::new_clientGrid3D:  (met client) send request_metaSfc " << std::endl;

   // while the server is getting the data ready for us,
   // set up a grid to use in receiving data
   
   // create the new grid
   result = new GridLatLonFieldSfc();
   result->clear();
   result->set_quantity( quantity );
   result->set_time( ttime, time ) ;
   result->setPgroup( my_pgroup, my_metproc );    
   //- std::cerr << "MetGridData::new_clientGridSfc:  (met client) set grid pgroup " << std::endl;

   // was the met processor able to come up with the data?
   status = receive_svr_status();
   if ( status == PGR_STATUS_OK ) {
       //- std::cerr << "MetLatLonGridData::new_clientGridSfc:  (met client) status is OK " << std::endl;
   
       // ask for grid metadata
       result->ask_for_meta();
       //- std::cerr << "MetLatLonGridData::new_clientGridSfc:  (met client) asked for metadata " << std::endl;
   
       // receive grid metadata
       result->receive_meta();
       if ( dbug >= 1 ) {
          std::cerr << "MetLatLonGridData::new_clientGridSfc:  (met client) received metadata from met processor" << std::endl;
       }    

       result->svr_done();

   } else {
       // the met processors could not get its data grid,
       // so we fail, too.
       //- std::cerr << "MetLatLonGridData::new_clientGridSfc:  (met client) status is FAILURE " << std::endl;
       delete result;
       result = NULLPTR;
   }
   
   return result;

}


double MetGridLatLonData::get_default_timedelta( const std::string &quantity, const std::string &caltime )
{
     double result;
     double time;
     // bracketing calendar data times around our test time.
     double dbeg, dend;
     
     
     time = cal2Time( caltime );
      
     bracket( quantity, time, &dbeg, &dend );
     if ( abs( dend - dbeg ) < 1e-8 ) {
        // oop's the test time lies on the boundary between data time ticks
        // shift it by about 10 minutes
        time = time + 10.0/60.0/24.0;
        bracket( quantity, time, &dbeg, &dend );
     }   
     //- std::cerr << "MetGridLatLonData::get_default_timedelta:  beg, end = " << dbeg << ", " << dend << std::endl;
     result =  ( dend - dbeg )*24.0;
     if ( result == 0.0 ) {
        // failsafe so that the time spacing returned is guaranteed never to be zero
        result = 5.0/60.0/24.0;
     }   
     // round to the nearest minute
     result = round(result*60.0)/60.0;
     
     return result;
}    




GridLatLonField3D* MetGridLatLonData::reconcile( const GridLatLonField3D  &dat1, const GridLatLonField3D  &dat2 )
{
    GridLatLonField3D* result;
    
    // are we using the same vertical levels (that makes things easier)
    if ( dat2.compatible( dat1 , METCOMPAT_VERT) ) {
       if ( dat2.compatible( dat1, METCOMPAT_HORIZ ) ) {
          // nothing to reconcile. simply copy
          result = dynamic_cast<GridLatLonField3D*>(dat1.duplicate());
       } else {   
          // only the horizontal grid is different
          result = hin->regrid( dat1, dat2.longitudes(), dat2.latitudes() );
       }
    } else {
       // nope. different vertical grids
       result = hin->regrid( dat1, dat2.longitudes(), dat2.latitudes(), dat2.levels(), *vin );
    }
        
    
    return result;
}           

GridLatLonFieldSfc* MetGridLatLonData::reconcile( const GridLatLonFieldSfc  &dat1, const GridLatLonFieldSfc  &dat2 )
{
    GridLatLonFieldSfc* result;
    
    if ( dat2.compatible( dat1, METCOMPAT_HORIZ ) ) {
       // nothing to reconcile. simply copy
       result = dynamic_cast<GridLatLonFieldSfc*>(dat1.duplicate());
    } else {   
       // only the horizontal grid is different
       result = hin->regrid( dat1, dat2.longitudes(), dat2.latitudes() );
    }
    
    return result;
}           

void MetGridLatLonData::writeCache( const GridField3D* item ) const
{
     FilePath* cachepath;
     FileLock* cachelock;
     std::ofstream* outcache;
     const GridLatLonField3D* actualItem;
    
     actualItem = dynamic_cast<const GridLatLonField3D*>(item);
    
     if ( diskcaching && item->cacheable() ) {
         cachepath = cachefile( actualItem );
         if ( cachepath != NULLPTR ) {
            try {
               // try to create the directory
               cachepath->makedir();
               try {
                  // try to open the file with locking
                  cachelock = new FileLock;
                  if ( dbug > 2 ) {
                     std::cerr << "MetGridLatLonData::writeCache: (3D) *** opening cache file for writing: " << cachepath->fullpath() << std::endl;
                  }
                  outcache = cachelock->openw( cachepath->fullpath(), 0 );
                  // write the data out
                  if ( dbug  > 2 ) {
                     std::cerr << "MetGridLatLonData::writeCache: (3D)   writing cached to from " << actualItem->id() << std::endl;
                  }
                  *outcache << *actualItem;
                  // close the file and release the lock
                  if ( dbug  > 2 ) {
                     std::cerr << "MetGridLatLonData::writeCache:  (3D)  closing written cache file" << std::endl;
                  }
                  cachelock->closer(outcache);
                  delete cachelock;
                  delete cachepath;
               } catch (...) {
               }
            } catch (...) {
            }
         }
     }
}

void MetGridLatLonData::writeCache( const GridFieldSfc* item ) const
{
     FilePath* cachepath;
     FileLock* cachelock;
     std::ofstream* outcache;
     const GridLatLonFieldSfc* actualItem;
     bool done;
     std::string dirname;
     int dirlevel;
     int err;
    
     actualItem = dynamic_cast<const GridLatLonFieldSfc*>(item);
    
     if ( diskcaching && item->cacheable() ) {
         cachepath = cachefile( actualItem );
         if ( cachepath != NULLPTR ) {
            try {
               // try to create the directory
               cachepath->makedir();
               try {
                  // try to open the file with locking
                  cachelock = new FileLock;
                  if ( dbug > 2 ) {
                     std::cerr << "MetGridLatLonData::writeCache*: (Sfc) ** opening cache file for writing: " << cachepath->fullpath() << std::endl;
                  }
                  outcache = cachelock->openw( cachepath->fullpath(), 0 );
                  // write the data out
                  if ( dbug > 2 ) {
                     std::cerr << "MetGridLatLonData::writeCache: (Sfc)   writing cached to from " << actualItem->id() << std::endl;
                  }
                  *outcache << *actualItem;
                  // close the file and release the lock
                  if ( dbug > 2 ) {
                     std::cerr << "MetGridLatLonData::writeCache:  (Sfc)  closing written cache file" << std::endl;
                  }
                  cachelock->closer(outcache);
                  delete cachelock;
                  delete cachepath;
               } catch (...) {
               }
            } catch (...) {
            }
         }     
     }

}


GridField3D* MetGridLatLonData::readCache3D( const std::string quantity, const std::string time )
{
    GridLatLonField3D* grid3d;
    FilePath* cachepath;
    FileLock* cachelock;
    std::ifstream* incache;
    bool usingCache;
    double xtime;
    time_t expt;


    grid3d = NULLPTR;

    if ( diskcaching ) {

       // create a grid and set up quantities that
       // are needed for creating the cache file name
       grid3d = new GridLatLonField3D;
       grid3d->set_quantity(quantity);
       grid3d->set_vertical(vquant);
       xtime = cal2Time( time);
       grid3d->set_time( xtime, time );
       grid3d->setPgroup( my_pgroup, my_metproc );
                       
       
       // get the cache file name
       cachepath = cachefile( grid3d );
       
       if ( cachepath != NULLPTR ) {
          cachelock = new FileLock;
          //cachelock->dbug = 1;
          try {
             if ( dbug >= 2 ) {
                std::cerr << "MetGridLatLonData::readCache3D: +++ opening cache file " << cachepath->fullpath() << std::endl;
             }
             int num_procs = 1;
             if ( my_pgroup != NULLPTR ) {
                num_procs = my_pgroup->numberOfProcessors();
             }
             incache = cachelock->openr( cachepath->fullpath(), - num_procs*2 );
             try {
                if ( dbug >= 2 ) {
                   std::cerr << "MetGridLatLonData::readCache3D:   reading cached data from " << grid3d->id() << std::endl;
                }
                *incache >> *grid3d;
                
                // check that the data in this cache have not expired
                expt = grid3d->expires();
                if ( expt == 0 || expt <= now ) {
                   // not expired

                   // hold on, though: the copy read from cache might not not have
                   // the same base time as we do. So check the calendar time
                   // to make sure it is right, and then set the model time
                   // to match ours.
                   if ( time != grid3d->met_time() ) {
                      std::cerr << "MetGridLatLonData::readCache3D: *******  mismatch of cache and model met times: " << time << " vs " << grid3d->met_time() << " *********" << std::endl;
                   }
                   grid3d->set_time(xtime, time);
                   
                   usingCache = true;
                } else {
                   // the data have expired                   
                   if ( dbug >= 2 ) {
                       std::cerr << "MetGridLatLonData::readCache3D :  data read from cache have expired: " << expt << " vs " << now << std::endl;
                   }
                   usingCache = false;
                }
             } catch (...) {
                if ( dbug >= 2 ) {
                   std::cerr << "MetGridLatLonData::readCache3D :  error reading cache" << std::endl;
                }
                usingCache= false;     
             }
             if ( dbug >= 2 ) {
                std::cerr << "MetGridLatLonData::readCache3D:   closing read cache file" << std::endl;
             }
             cachelock->closer(incache);
          } catch (...) {
             if ( dbug >= 2 ) {
                std::cerr << "MetGridLatLonData::readCache3D:   no cache to read" << std::endl;
             }
             usingCache= false;     
          }
          
          delete cachelock;
          delete cachepath;
          
       } else {
          usingCache= false;     
       }
       
       // did something go wrong?
       if ( ! usingCache ) {
          remove(grid3d);
          grid3d = NULLPTR;
       }   

    }

    return grid3d;
}
     
GridFieldSfc* MetGridLatLonData::readCacheSfc( const std::string quantity, const std::string time )
{
    GridLatLonFieldSfc* gridsfc;
    FilePath* cachepath;
    FileLock* cachelock;
    std::ifstream* incache;
    bool usingCache;
    double xtime;
    std::string sfcname;
    std::string quantname;
    size_t pos;
    time_t expt;
    
    gridsfc = NULLPTR;

    if ( diskcaching ) {

       // split the quantity name into quantity and surface
       pos = quantity.find("@");
       if ( pos != string::npos ) {
          quantname = quantity.substr(0, pos);
          sfcname = quantity.substr(pos+1);
       } else {
          //todo:  put in switch block here
          quantname = quantity;
          //sfcname = "sfc";
          sfcname = quantity;
       }
    
       // create a grid and set up quantities that
       // are needed for creating the cache file name
       gridsfc = new GridLatLonFieldSfc;
       gridsfc->set_quantity(quantname);
       gridsfc->set_surface(sfcname);
       xtime = cal2Time( time);
       gridsfc->set_time( xtime, time );
       gridsfc->setPgroup( my_pgroup, my_metproc );
       
       cachepath = cachefile( gridsfc );
       
       if ( cachepath != NULLPTR ) {
          cachelock = new FileLock;
          //cachelock->dbug = 1;
          try {
             if ( dbug >= 2 ) {
                std::cerr << "MetGridLatLonData::readCacheSfc: +++ opening cache file " << cachepath->fullpath() << std::endl;
             }
             int num_procs = 1;
             if ( my_pgroup != NULLPTR ) {
                num_procs = my_pgroup->numberOfProcessors();
             }
             incache = cachelock->openr( cachepath->fullpath(), - num_procs*2 );
             try {
             
                if ( dbug >= 2 ) {                                                                                                                                                            
                   std::cerr << "MetGridLatLonData::readCacheSfc:   reading cached data from " << gridsfc->id() << std::endl;                                                                 
                }                                                                                                                                                                             
                *incache >> *gridsfc;                                                                                                                                                         
             
                // check that the data in this cache have not expired                                                                                                                         
                expt = gridsfc->expires();                                                                                                                                                    
                if ( expt == 0 || expt <= now ) {                                                                                                                                             
                   // not expired                                                                                                                                                             

                   // hold on, though: the copy read from cache might not not have                                                                                                            
                   // the same base time as we do. So check the calendar time                                                                                                                 
                   // to make sure it is right, and then set the model time                                                                                                                   
                   // to match ours.                                                                                                                                                          
                   if ( time != gridsfc->met_time() ) {                                                                                                                                       
                      std::cerr << "MetGridLatLonData::readCacheSfc: *******  mismatch of cache and model met times: " << time << " vs " << gridsfc->met_time() << " *********" << std::endl; 
                   }                                                                                                                                                                          
                   gridsfc->set_time(xtime, time);                                                                                                                                            

                   usingCache = true;                                                                                                                                                         
                } else {                                                                                                                                                                      
                   // the data have expired                                                                                                                                                   
                   if ( dbug >= 2 ) {                                                                                                                                                         
                       std::cerr << "MetGridLatLonData::readCacheSfc :  data read from cache have expired: " << expt << " vs " << now << std::endl;                                           
                   }                                                                                                                                                                          
                   usingCache = false;                                                                                                                                                        
                }                                                                                                                                                                             
             } catch (...) {
                if ( dbug >= 2 ) {
                   std::cerr << "MetGridLatLonData::readCache3D :  error reading cache" << std::endl;
                }
                usingCache= false;     
             }
             if ( dbug >= 2 ) {
                std::cerr << "MetGridLatLonData::readCacheSfc:   closing read cache file" << std::endl;
             }
             cachelock->closer(incache);
          } catch (...) {
             if ( dbug >= 2 ) {
                std::cerr << "MetGridLatLonData::readCacheSfc:   no cache to read" << std::endl;
             }
             usingCache= false;     
          }
          
          delete cachelock;
          delete cachepath;
       
       } else {
          usingCache= false;
       } 
       
       // did something go wrong?
       if ( ! usingCache ) {
          remove( gridsfc );
          gridsfc = NULLPTR;
       }   

    }

    return gridsfc;

}
