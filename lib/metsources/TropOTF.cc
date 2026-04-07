
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

#include "gigatraj/MetGridData.hh"

using namespace gigatraj;

#include "gigatraj/TropOTF.hh"

TropOTF::TropOTF() 
{
   sfc = "tropopause";
   quant = "tropopause_air_pressure";
   tropkind = 0;
   uu = "mb";
   pname = "air_pressure";
   tname = "air_temperature";
   aname = "altitude";
   hname = "air_potential_temperature";
   dname = "air_density";
   
   palt.setPressureName(pname);
   palt.set_quantity(aname);

}
TropOTF::TropOTF(std::string trop, std::string temperature, std::string pressure, std::string altitude, std::string theta, std::string density, int tkind )
{
   // the quantity and units are ignored in the calculation methods.
   quant = "tropopause_air_pressure";
   uu = "mb";
   tropkind = tkind;
   
   sfc = trop;
   pname = pressure;
   tname = temperature;
   aname = altitude;
   hname = theta;
   dname = density;

   palt.setPressureName(pname);
   palt.set_quantity(aname);
}

TropOTF::~TropOTF() 
{
}

TropOTF::TropOTF(const TropOTF& src) : MetOnTheFly(src)
{
   tropkind = src.tropkind;   
   sfc = src.sfc;
   pname = src.pname;
   tname = src.tname;
   aname = src.aname;
   hname = src.hname;
   dname = src.dname;
   palt = src.palt;

   palt.setPressureName(pname);
   palt.set_quantity(aname);
}

TropOTF& TropOTF::operator=(const TropOTF& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }
    
    this->assign( src ) ;
    
    return *this;
}

void TropOTF::assign( const TropOTF& src)
{
    MetOnTheFly::assign( src );

    tropkind = src.tropkind;   
    sfc = src.sfc;
    pname = src.pname;
    tname = src.tname;
    aname = src.aname;
    hname = src.hname;
    dname = src.dname;
    palt = src.palt;

    palt.setPressureName(pname);
    palt.set_quantity(aname);
}


real TropOTF::wmo( const std::vector<real>&t, const std::vector<real>&alt, int flags ) const
{
     // the length of the input vectors
     unsigned len;
     // the profile of calculated lapse rates
     std::vector<real> lapse_rate;
     // the altitude of the tropopause
     real trop_alt;
     // the index in the profile that marks the tropopause
     unsigned pick;
     // whether the element was found
     bool picked;
     // the target lapse rate
     real target_lapse;
     // debugging
     int debug = 0;
     // vector index
     unsigned int i;
     // altitudes of three points
     real x0,x1,x2;
     // temperatures of three points
     real y0,y1,y2;
     // fir parameters of y = aa + bb*x + cc*x*x;
     real bb,cc;

     debug = ( flags & 0x1) != 0;

     len = t.size();
     if ( len != alt.size() ) {
        std::cerr << "TropOTF::wmo: size mismatch: " << len << " vs " << alt.size() << std::endl; 
        throw (badprofile());
     }   

     // calculate the lapse rates along the profile  
     // We do this by fitting every three points to y = a + b*x + c*x*x.
     // Then dy/dx = b + 2*c*x
     
     // first point
     i = 0;
     x0 = alt[i];
     x1 = alt[i+1];
     x2 = alt[i+2];
     y0 = t[i];
     y1 = t[i+1];
     y2 = t[i+2];
     bb = (((x0*x0)*(y2 - y1)) + ((x1*x1)*(y0 - y2)) + ((x2*x2)*(y1 - y0)))/((x1 - x0)*(x2 - x0)*(x2 - x1));
     cc = (((y0 - y1)/(x1 - x0)) + ((y2 - y1)/(x2 - x1)))/(x2 - x0);
     // Note the negative:  lapse rate = - dt/dz
     lapse_rate.push_back( - bb - 2.0*cc*x0 );
     
     // middle points   
     for ( unsigned i=1; i<(len-1); i++ ) {
         if ( alt[i] == alt[i+1] ) {
             std::cerr << "TropOTF::wmo: duplicate vertical level at " << i << " and " << i + 1 << " : " << alt[i] << std::endl; 
             throw (badprofile());
         }    
         x0 = alt[i-1];
         x1 = alt[i];
         x2 = alt[i+1];
         y0 = t[i-1];
         y1 = t[i];
         y2 = t[i+1];
         bb = (((x0*x0)*(y2 - y1)) + ((x1*x1)*(y0 - y2)) + ((x2*x2)*(y1 - y0)))/((x1 - x0)*(x2 - x0)*(x2 - x1));
         cc = (((y0 - y1)/(x1 - x0)) + ((y2 - y1)/(x2 - x1)))/(x2 - x0);
         lapse_rate.push_back( - bb - 2.0*cc*x1 );
     }
     
     
     // last point
     i = len - 1;
     x0 = alt[i-2];
     x1 = alt[i-1];
     x2 = alt[i];
     y0 = t[i-2];
     y1 = t[i-1];
     y2 = t[i];
     bb = (((x0*x0)*(y2 - y1)) + ((x1*x1)*(y0 - y2)) + ((x2*x2)*(y1 - y0)))/((x1 - x0)*(x2 - x0)*(x2 - x1));
     cc = (((y0 - y1)/(x1 - x0)) + ((y2 - y1)/(x2 - x1)))/(x2 - x0);
     lapse_rate.push_back( - bb - 2.0*cc*x2 );
     
     if ( debug ) {
        std::cerr << " i   z,   t,   dtdz " << len << std::endl;
        for ( int ix=0; ix < lapse_rate.size(); ix++ ) {
            std::cerr << " [" << ix << "] " 
            << alt[ix]/1000.0 << "  " << t[ix]
            << " " << lapse_rate[ix]*1000.0 
            << std::endl;
            
        }
     }
     
     // now find the lowest-in-altitude lapse rate that lies below 0.002 K/m
     // with no level within 2 km above it having a lapse rate greater 
     // than 0.002 K/m.
     target_lapse = 0.002;
     picked = false;
     for ( unsigned i=0; 
           (i < lapse_rate.size()) && ( ! picked ) ; 
           i++ ) {
         
         if ( lapse_rate[i] < target_lapse ) {
            // tentatively assume that this is a right level
            pick = i;
            picked = true;
            if ( debug ) {
               std::cerr << "  tentative level: " << alt[i]/1000.0 << std::endl;
            }   
            for ( unsigned j=i+1; 
                 (j < lapse_rate.size()) && ((alt[j] - alt[i]) < 2000.0) ; 
                 j++ ) {
                if ( lapse_rate[j] > target_lapse ) {
                   // within 2 km but lapse rate > 0.002
                   // reset
                   picked = false;
                   if ( debug ) {
                      std::cerr << "  reset by " << alt[j] << std::endl;
                   }
                }   
            }
         }
     }
     if ( debug ) {
        std::cerr << "debug message here" << std::endl;
     }
     
     if ( picked && (pick > 0 || alt[pick] > 17210.4 ) ) {
        // found a point.
        // find the altitude of the trop
        trop_alt = ( target_lapse - lapse_rate[pick-1] )
                  / ( lapse_rate[pick] - lapse_rate[pick-1]  )
                  * ( alt[pick] - alt[pick-1] ) + alt[pick-1];
        if ( debug ) {
           std::cerr << "found our trop alt: " << trop_alt << std::endl;
        }
     } else {
        trop_alt = 17210.4;
     }
     
     // note: we ignore any OTF_MKS flag at this point.
       
     return trop_alt;
}


GridFieldSfc* TropOTF::wmo( const GridField3D& t, int flags ) const
{
    // the output tropopause field
    GridFieldSfc* tropsfc;
    // iterator over the output grid points
    GridFieldSfc::iterator pnt;
    // iterator over the input grid vertical profiles
    GridField3D::const_profileIterator prof;
    // vector of vertical coordinates from the input grid
    std::vector<real> rawvrt;
    // vector of vertical coordinates for usable points from the profile
    std::vector<real> vrt;
    // length of the input vertical coordinates
    int n;
    // vertical profile of temperatures, as extracted from the input grid, 
    // potentially with bad points.
    std::vector<real>* tp;
    // vertical profile of temperature, w/o bad points
    std::vector<real> dat;
    // vertical profile of altitude values, corresponding to dat
    std::vector<real> alts;
    // temporary variable
    real value;
    // temperature at a given vertical level
    real tval;
    // vertical coordinate at a given level
    real vval;
    // bad-or-missing fill value
    real tbad;
    // indicates what kind of vertical coordinates we are using
    int vcoord;
    // debugging messages flag
    int debug = 0;
    // MKS vertical units
    std::string vu;
    // vertical coordinate of the input quantity
    std::string vcoordname;


    // the input quantity must be temperature
    if ( t.quantity() != tname ) {
       throw (badprofile());
    }   
    tbad = t.fillval(); 
    vcoordname = t.vertical();

    // get the vertical coordinate, and scale it to SI units
    rawvrt = t.levels();
    // note its length as well
    n = rawvrt.size();
    // convert to SI units
    for ( int i=0; i<n; i++ ) {
        rawvrt[i] = rawvrt[i]*t.mksVScale + t.mksVOffset;
    }    

    // we can calculate the trop only for these vertical coords
    if ( vcoordname == aname ) {
       // altitude
       vcoord = 0;
    } else if ( vcoordname == pname ) {
       // pressure
       vcoord = 1;
    } else if ( vcoordname == hname ) {
       // potential temperature
       vcoord = 2;
    } else if ( vcoordname == dname ) {
       // density
       vcoord = 3;
    } else {
        throw (badprofile());
    } 

    // Extract a surface from the input field.
    // This ensures that the output has the same horizontal grid as the input.
    tropsfc = t.extractSurface(0);
    // Set the metadata.
    if ( tropkind == 0 ) {
       // We return the tropopause in terms of the vertical coordinate of t, in the same units
       tropsfc->set_quantity(t.vertical());
       tropsfc->set_units(t.vunits());
       tropsfc->mksScale=t.mksVScale;
       tropsfc->mksOffset=t.mksVOffset;
       tropsfc->set_surface( "trop" );
    } else {
       // We return the tropopause as calculated, in altitude [km]
       tropsfc->set_quantity("alt");
       tropsfc->set_units("km");
       tropsfc->mksScale=100.0;
       tropsfc->mksOffset=0.0;
       tropsfc->set_surface( "trop" );    
    }
    
    


    // for each horizontal gridpoint...
    for ( prof  = t.profileBegin(), pnt = tropsfc->begin(); 
          prof != t.profileEnd(); 
          prof++, pnt++ ) {

        // reset our altitude and temperature variables
        alts.clear();
        alts.reserve(n);
        dat.clear();
        dat.reserve(n);
        vrt.clear();
        vrt.reserve(n);
         
        // extract the temperature profile
        tp = *prof;
        // assemble vectors of T and alt with no bad points
        // 'i' is the index into the source vertical profile
        // 'j' is the index into the dat/alts profile
        int i, j;
        for ( i=0, j=0; i<n; i++, j++ ) {
            // we skip past bad-data points
            if ( (*tp)[i] != tbad ) {
              
               // get the temperature in Kelvin
               tval = ((*tp)[i]*t.mksScale+t.mksOffset); 
               // and whatever vertical coordinate we are using
               vval = rawvrt[i];
               // save it
               dat.push_back( tval );
               vrt.push_back( vval );
               
               switch (vcoord) {
               case 0: // altitude
                  value = vval; 
                  break;
               case 1: // pressure
                  // we integrate the temperature and pressure to get geopotential height
                  if ( j > 0 ) {   
                     real dlogp = LOG(vval) - LOG(vrt[j - 1]);
                     real tavg = (tval + dat[ j - 1])/2.0;
                     value = alts[j - 1] - 287.04/9.81*tavg*dlogp;
                  } else {
                     value = 0.0;
                  }
                  break;
               case 2: // theta   
                  // we integrate the temperature and theta to get geopotential height
                  if ( j > 0 ) {   
                     real dlogtheta = LOG(vval) - LOG(vrt[j - 1]);
                     real dlogt = LOG(tval) - LOG(dat[j - 1]);
                     real tavg = (tval + dat[ j - 1])/2.0;
                     value = alts[j - 1] - 287.04/9.81/(2./7.)*tavg*(dlogt - dlogtheta);
                  } else {
                     value = 0.0;
                  }
                  break;
               case 3: // density   
                  // we integrate the density to get geopotential height
                  if ( j > 0 ) {   
                     real dlogrho = LOG(vval) - LOG(vrt[j - 1]);
                     real dlogt = LOG(dat[j]) - LOG(dat[j - 1]);
                     real tavg = (tval + dat[ j - 1])/2.0;
                     value = alts[j - 1] - 287.04/9.81*tavg*(dlogrho + dlogt);
                  } else {
                     value = 0.0;
                  }
                  break;
               } 
               
               alts.push_back( value ); 
              
            }    
        }

        // find the altitude of the tropopause
        value = wmo( dat, alts, debug ); 

        if ( tropkind == 0 ) {
           // convert this trop alt back to the original vertical coordinates
           // but note that altitude is in m right now, so we may
           // want to convert it
           if ( vcoord != 0 ) {
              // not an altitude
              
              // find the input altitudes that straddle the tropopause
              real vc = NAN;
              for ( int i=1; i < vrt.size(); i++ ) {
                  if ( ((alts[i-1] <= value) && (alts[i]   >= value) )
                    || ((alts[i]   <= value) && (alts[i-1] >= value)) ) {
                     // got it
                     // now interpolate in log coordinates
                     vc = ( value - alts[i-1] )/(alts[i] - alts[i-1])*( LOG(vrt[i]) - LOG(vrt[i-1]) ) + LOG(vrt[i-1]);
                     break;
                  }
              }
              if ( FINITE(vc) ) {
                 value = EXP(vc);
              } else {
                 value = vc;
              }   
           }              
           
           if ( FINITE(value) ) {
              // convert back to caller's units, before storing it
              value =  ( value - tropsfc->mksOffset )/tropsfc->mksScale;
           }
           *pnt = value;
        } else {
           // keep it in m
           *pnt = value;
        }
        
        // get ready for the next vertical profile
        delete tp;

    }
    

    if ( flags & OTF_MKS ) {
       switch (vcoord) {
       case 0: // altitude
          vu = "m";
          break;
       case 1: // pressure   
          vu = "Pa";
          break;
       case 2: // theta   
          vu = "K";
          break;
       case 3: // density   
          // "value" is in altitude.  find the temperature at this altitude
          vu = "kg/m^3";
          break;        
       }
       tropsfc->transform(vu, 1.0/tropsfc->mksScale, - tropsfc->mksOffset );
    
    }    

    return tropsfc;
}




GridFieldSfc* TropOTF::wmo( const GridField3D& t, const GridField3D& alt, int flags) const
{
    // the output tropopause field
    GridFieldSfc* tropsfc;
    // the altitudes
    const GridField3D* alts3d;
    // flag for what the "alts" parameter is
    int alts_arg_is;
    // temporary holding surface
    GridFieldSfc* tmp2d;
    // iterator over the output grid points
    GridFieldSfc::iterator pnt;
    // iterators over the input grid vertical profiles
    GridField3D::const_profileIterator prof, aprof;
    // vector of vertical coordinates from the input grid
    std::vector<real> rawalts;
    // length of the input vertical coordinates
    int n;
    // vertical profile of temperatures, as extracted from the first input grid, 
    // potentially with bad points.
    std::vector<real>* tp;
    // vertical profile of altitudes, as extracted from the second input grid, 
    // potentially with bad points.
    std::vector<real>* ap;
    // vertical profile of temperature, w/o bad points
    std::vector<real> dat;
    // vertical profile of altitude-like quantity, w/o bad points
    std::vector<real> alts;
    // temporary variable
    real value;
    // temperature at a given vertical level
    real tval;
    // altitude-like value at a given vertical level
    real aval;
    // bad-or-missing fill value
    real tbad;
    // bad-or-missing fill value
    real abad;
    // indicates what kind of vertical coordinates we are using
    int vcoord;
    // debugging messages flag
    int debug = 0;
    // MKS vertical units
    std::string vu;

    if ( flags & OTF_DEBUG ) {
       debug = 1;
    }

    alts_arg_is = -1; 
    if ( alt.quantity() == aname ) {
       alts_arg_is = 0; // it's altitude
    } else if ( alt.quantity() == pname ) {
       alts_arg_is = 1; // it's pressure
   
    }   

    // the input quantities must be correct
    if ( t.quantity() != tname || alts_arg_is == -1 ) {
       throw (badprofile());
    }  
    // and the two input grids must be compatible
    if ( ! t.compatible(alt) ) {
       throw (badprofile());
    }  
    
    if ( alts_arg_is == 0 ) {
       alts3d = &alt;
    } else {
       alts3d = palt.calc( alt );
    }   

    // Extract a surface from the input field.
    // This ensures that the output has the same horizontal grid as the input.
    tropsfc = t.extractSurface(0);
    // We return the tropopause in terms of the alt variable, in the same units
    tropsfc->set_quantity(alt.quantity());
    tropsfc->set_units(alt.units());
    tropsfc->mksScale=alt.mksScale;
    tropsfc->mksOffset=alt.mksOffset;
    tropsfc->set_surface( sfc );

    tbad = t.fillval(); 
    abad = alt.fillval();

    // for each horizontal gridpoint...
    for (prof=t.profileBegin(), aprof=alt.profileBegin(), pnt=tropsfc->begin(); 
         prof!=t.profileEnd(); 
         prof++, aprof++, pnt++ ) {

        // extract the temperature and altitude profiles
        tp = *prof;
        ap = *aprof;
        n = (*ap).size();

        // reset our altitude and temperature variables
        alts.clear();
        alts.reserve(n);
        dat.clear();
        dat.reserve(n);

        // convert t to SI units, assembling vectors of t and alt with no bad points
        for ( int i=0; i<n; i++ ) {
            tval = (*tp)[i];
            aval = (*ap)[i];
            if ( tval != tbad  && aval != abad ) {
               tval = (tval*t.mksScale+t.mksOffset); 
               aval = (aval*alt.mksScale+alt.mksOffset); 
               
               dat.push_back( tval );
               alts.push_back( aval );
            }    
        }

        // find the trop
        value = wmo( dat, alts, debug ); 
        
        // convert back to caller's units, before storing it
        *pnt =  ( value - tropsfc->mksOffset )/tropsfc->mksScale;
        
        delete tp;
        delete ap;

    }
    if ( alts_arg_is == 1 ) {
       delete alts3d;
       tmp2d = tropsfc;
       tropsfc = palt.clac( *tmp2d );
       delete tmp2d;
    }   

    if ( flags & OTF_MKS ) {
       switch (vcoord) {
       case 0: // altitude
          vu = "m";
          break;
       case 1: // pressure   
          vu = "Pa";
          break;
       case 2: // theta   
          vu = "K";
          break;
       case 3: // density   
          // "value" is in altitude.  find the temperature at this altitude
          vu = "kg/m^3";
          break;        
       }
       tropsfc->transform(vu, 1.0/tropsfc->mksScale, - tropsfc->mksOffset );
    
    }

    return tropsfc;
}

 
 
real TropOTF::intrp( real xi, std::vector<real>&x, std::vector<real>&y ) const
{
      // length of input vectors
      int len;
      // output interpolated result
      real value = 0.0;
      
      // make sure the two input vectors are the same size
      len = x.size();
      if ( len != y.size() || len < 2 ) {
         throw (badprofile());
      }

      // work out way through the profile
      for ( int i=0; i<(len-1); i++ ) {      
      
           if ( (x[i+1]-xi)*(x[i]-xi) <= 0.0 ) {
              // found the two gridpoints that are straddled by our desired point
              
              // interpolate linearly
              value = ( y[i+1]-y[i] )/( x[i+1] - x[i] )*( xi - x[i] ) + y[i];
              
              // leave the loop; we are done
              break;
           }
      
      }
            
      return value;
 
 }
 
