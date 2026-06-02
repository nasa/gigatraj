
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

#include "gigatraj/MetGridSBRot.hh"

using namespace gigatraj;

const std::string MetGridSBRot::iam = "MetGridSBRot";


// default constructor
MetGridSBRot::MetGridSBRot( real lonspc, real latspc
      , real windspeed, real tilt
      ) : MetGridLatLonData() 
{

     setup_vars();

     metfcn = new MetSBRot(windspeed, tilt);

     // quantity names
     pressure_name = "p";
     altitude_name = "alt";
     palt_name = "alt";
     pottemp_name = "theta";
     pressureDot_name = "omega";
     altDot_name = "w";
     paltDot_name = "PAltDot";
     thetaDot_name = "ThetaDot";

     setgrid( lonspc, latspc );
     setup_vGrid();
     
      native_vquant = altitude_name;
     
     set_vertical( "alt", "km" );
     
     //vquant = "alt";
     //vuu = "km";
     //wfctr = 0.001; // converts m/s -> km/s to match w with alt.
      
          
}


MetGridSBRot::MetGridSBRot( real lonspc, real latspc
      , real windspeed
      , real alpha, real beta, real gamma
      ) : MetGridLatLonData()
{
    
     setup_vars();

     metfcn = new MetSBRot(windspeed, alpha, beta, gamma);

     setgrid( lonspc, latspc );
     setup_vGrid();

      native_vquant = altitude_name;
     
     set_vertical( altitude_name, "km" );
     
     //vquant = "alt";
     //vuu = "km";
     //wfctr = 0.001; // converts m/s -> km/s to match w with alt.
     

}

// full constructor
MetGridSBRot::MetGridSBRot(
        const std::string quantity, const std::string units
      , const std::vector<real>& xzs
      , const std::vector<real>& xlons
      , const std::vector<real>& xlats
      , real windspeed, real tilt  
      ) : MetGridLatLonData(quantity,units,xzs,xlons,xlats)  
{
     
      setup_vars();

      metfcn = new MetSBRot(windspeed, tilt);

      native_vquant = altitude_name;

      setgrid( xlons, xlats );

     set_vertical( altitude_name, "km" );
     
     //vquant = "alt";
     //vuu = "km";
     //wfctr = 0.001; // converts m/s -> km/s to match w with alt.
     
          
};

// destructor
MetGridSBRot::~MetGridSBRot()
{
   delete metfcn;
}

// copy constructor
MetGridSBRot::MetGridSBRot( const MetGridSBRot&  src) : MetGridLatLonData(src)
{
    tropgen = src.tropgen;
}    

void MetGridSBRot::assign(const MetGridSBRot& src)
{
    MetGridLatLonData::assign(src);

    tropgen = src.tropgen;
}    

MetGridData* MetGridSBRot::MetGridCopy()
{
     MetGridSBRot* newthing;
     
     newthing = new MetGridSBRot();
     newthing->assign( *this );
     
     return newthing;
}

void MetGridSBRot::setup_vars()
{
    vWindStuff vw;

    altitude_name = "alt";
    pressure_name = "p";
    palt_name = "PAlt";
    pottemp_name = "theta";

      vw.quantity = "w";
      vw.units = "km/s";
      vw.MKSscale = 1000.0;
      vw.MKSoffset = 0.0;
      vertwind_quants[ altitude_name ] = vw;
    
      vw.quantity = "omega";
      vw.units = "Pa/s";
      vw.MKSscale = 1.0;
      vw.MKSoffset = 0.0;
      vertwind_quants[ pressure_name ] = vw;
    
      vw.quantity = "heating rate";
      vw.units = "K/s";
      vw.MKSscale = 1.0;
      vw.MKSoffset = 0.0;
      vertwind_quants[ pottemp_name ] = vw;
    
    wind_ew_name   = "u";
    wind_ns_name   = "v";
    wind_vert_name = (vertwind_quants[ vquant ]).quantity;

    getpalt.set_quantity(palt_name);
    getpalt.setPressureName( pressure_name );

    getpaltdot.set_quantity((vertwind_quants[ palt_name ]).quantity);
    getpaltdot.setPressureAltitudeName(palt_name);
    getpaltdot.setPressureName(pressure_name);
    getpaltdot.setPressureDotName((vertwind_quants[ pressure_name ]).quantity);
      
     tropgen.setTropName("trop");
     tropgen.setTemperatureName("t");
     tropgen.setPressureName(pressure_name);
     tropgen.setAltitudeName(altitude_name);
     tropgen.setPotentialTemperatureName(pottemp_name);
     tropgen.setDensityName("rho");
    

}


void MetGridSBRot::setup_vGrid()
{

     native_nzs = 18;
     native_zs.reserve(native_nzs);
     native_zs.push_back( 1000.0);
     native_zs.push_back(  850.0);
     native_zs.push_back(  700.0);
     native_zs.push_back(  500.0);
     native_zs.push_back(  400.0);
     native_zs.push_back(  300.0);
     native_zs.push_back(  250.0);
     native_zs.push_back(  200.0);
     native_zs.push_back(  150.0);
     native_zs.push_back(  100.0);
     native_zs.push_back(   70.0);
     native_zs.push_back(   50.0);
     native_zs.push_back(   30.0);
     native_zs.push_back(   10.0);
     native_zs.push_back(    5.0);
     native_zs.push_back(    2.0);
     native_zs.push_back(    1.0);
     native_zs.push_back(    0.4);   
     // now convert these pressures to log-p altitudes
     for ( int i=0; i<native_nzs; i++ ) {
        native_zs[i] = 7.0 * LOG( 1000.0/native_zs[i] );
     }     

}


MetData *MetGridSBRot::genericCopy()
{
    MetGridSBRot *result;
    
    result = new MetGridSBRot;
    result->assign(*this);
    
    return dynamic_cast<MetData*>( result );
}


std::vector<real> *MetGridSBRot::vcoords( const std::string *coordSys ) const
{
    std::vector<real> *result;
    const std::string *vc;

    vc = &vquant;
    if ( coordSys != NULLPTR ) {
       vc = coordSys;
    }

    result = new std::vector<real>;

    if ( *vc == pressure_name || *vc == "air_pressure" ) {

       result->push_back( 1013.25 );     
       result->push_back( 1000.  );     
       result->push_back(  850.  );     
       result->push_back(  700.  );     
       result->push_back(  500.  );     
       result->push_back(  400.  );     
       result->push_back(  300.  );     
       result->push_back(  250.  );     
       result->push_back(  200.  );     
       result->push_back(  150.  );     
       result->push_back(  100.  );     
       result->push_back(   70.  );     
       result->push_back(   50.  );     
       result->push_back(   30.  );     
       result->push_back(   10.  );     
       result->push_back(    5.  );     
       result->push_back(    2.  );     
       result->push_back(    1.  );     
       result->push_back(    0.4 );     
    
    } else if ( *vc == pottemp_name || *vc == "air_potential_temperature" ) {

       result->push_back(   287.0 );  
       result->push_back(   287.5 );  
       result->push_back(   292.0 );   
       result->push_back(   297.0 );    
       result->push_back(   307.0 );   
       result->push_back(   314.0 );
       result->push_back(   322.0 );   
       result->push_back(   328.0 );  
       result->push_back(   343.0 );  
       result->push_back(   373.0 );   
       result->push_back(   418.0 );
       result->push_back(   463.0 );   
       result->push_back(   511.0 );   
       result->push_back(   601.0 );  
       result->push_back(   849.0 ); 
       result->push_back(  1087.0 );
       result->push_back(  1522.0 );   
       result->push_back(  1948.0 );   
       result->push_back(  2426.0 );
           
    } else if ( *vc == palt_name || *vc == "pressure_altitude" ) {

       result->push_back( 0.0      );    
       result->push_back( 0.109767 );    
       result->push_back( 1.45441  );                        
       result->push_back( 3.01204  );                       
       result->push_back( 5.57129  );                       
       result->push_back( 7.18340  );                 
       result->push_back( 9.16200  );    
       result->push_back( 10.3595  );     
       result->push_back( 11.7840  );     
       result->push_back( 13.6084  );     
       result->push_back( 16.1797  );
       result->push_back( 18.4416  );     
       result->push_back( 20.5767  );     
       result->push_back( 23.8489  );     
       result->push_back( 31.0547  );     
       result->push_back( 35.7776  );
       result->push_back( 42.4412  );     
       result->push_back( 47.8201  );     
       result->push_back( 54.9943  );
        
    } else if ( *vc == altitude_name || *vc == "altitude" ) {

       result->push_back( 0.0      );    
       result->push_back( 0.109767 );    
       result->push_back( 1.45441  );                        
       result->push_back( 3.01204  );                       
       result->push_back( 5.57129  );                       
       result->push_back( 7.18340  );                 
       result->push_back( 9.16200  );    
       result->push_back( 10.3595  );     
       result->push_back( 11.7840  );     
       result->push_back( 13.6084  );     
       result->push_back( 16.1797  );
       result->push_back( 18.4416  );     
       result->push_back( 20.5767  );     
       result->push_back( 23.8489  );     
       result->push_back( 31.0547  );     
       result->push_back( 35.7776  );
       result->push_back( 42.4412  );     
       result->push_back( 47.8201  );     
       result->push_back( 54.9943  );
    
    } else {
   
       throw(badVerticalCoord());
    
    }


    return result;
}

/// assignment operator
MetGridSBRot& MetGridSBRot::operator=(const MetGridSBRot& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }
    
    assign(src);
    
    return *this;

}
MetGridLatLonData& MetGridSBRot::operator=(const MetGridLatLonData& src)
{
    const MetGridSBRot* ssrc;
    
    ssrc = dynamic_cast<const MetGridSBRot*>(&src);
    // handle assignment to self
    if ( this == ssrc ) {
       return *this;
    }
    
    assign(*ssrc);

    return *this;
}

void MetGridSBRot::set_cal( int cal )
{
     metfcn->set_cal(cal);
}

int MetGridSBRot::get_cal() const
{
     return metfcn->get_cal();;
} 

void MetGridSBRot::setOption( const std::string &name, const std::string &value )
{
     if ( name == "test" ) {
        metfcn->setOption( name, value);
     } else if ( name == "tilt" || name == "maxwind" ) {
        metfcn->setOption( name, value);
     } else {
        MetGridLatLonData::setOption( name, value );
     }
}

void MetGridSBRot::setOption( const std::string &name, int value )
{
     if ( name == "test" ) {
        metfcn->setOption( name, value);
     } else {
        MetGridLatLonData::setOption( name, value );
     }
}

void MetGridSBRot::setOption( const std::string &name, float value )
{
     if ( name == "test" ) {
        metfcn->setOption( name, value);
     } else {
        MetGridLatLonData::setOption( name, value );
     }
}

void MetGridSBRot::setOption( const std::string &name, double value )
{
     if ( name == "test" ) {
        metfcn->setOption( name, value);
     } else {
        MetGridLatLonData::setOption( name, value );
     }
}

bool MetGridSBRot::getOption( const std::string &name, std::string &value )
{
     bool result;
     
     if ( name == "test" ) {
        result = metfcn->getOption( name, value );
     } else {
        result = MetGridLatLonData::getOption( name, value );
     }
     
     return result;
}


bool MetGridSBRot::getOption( const std::string &name, int &value )
{
     bool result;
     
     if ( name == "test" ) {
        result = metfcn->getOption( name, value );
     } else {
        result = MetGridLatLonData::getOption( name, value );
     }
     
     return result;
}


bool MetGridSBRot::getOption( const std::string &name, float &value )
{
     bool result;
     
     if ( name == "test" ) {
        result = metfcn->getOption( name, value );
     } else {
        result = MetGridLatLonData::getOption( name, value );
     }
     
     return result;
}


bool MetGridSBRot::getOption( const std::string &name, double &value )
{
     bool result;
     
     if ( name == "test" ) {
        result = metfcn->getOption( name, value );
     } else {
        result = MetGridLatLonData::getOption( name, value );
     }
     
     return result;
}




void MetGridSBRot::setgrid( real lonspc, real latspc ) 
{
     real val;
     
     if ( lonspc == 0.0 || latspc == 0.0 ) {
        throw (badgridspec());
     }
         
     nlons = static_cast<int>( 360.0/ABS(lonspc) + 0.5 );
     lons.reserve( nlons );
     if ( lonspc > 0.0 ) {
        for (val = 0.0; val < 360.0; val += lonspc ) {
            lons.push_back(val);
        }
     } else {
        for (val = 0.0; val > -360.0; val += lonspc ) {
            lons.push_back(val);
        }
     }

     nlats = static_cast<int>( 180.0/ABS(latspc) + 1.5 );
     lats.reserve( nlats );
     if ( latspc > 0.0 ) {
        for (val = -90.0; val <= 90.0; val += latspc ) {
            lats.push_back(val);
        }
     } else {
        for (val = 90.0; val >= -90.0; val += latspc ) {
            lats.push_back(val);
        }
     }
   
}

void MetGridSBRot::setgrid( const std::vector<real>&xlons, const std::vector<real>& xlats ) 
{
     // todo: check for monotonicity

     nlons = xlons.size();
     nlats = xlats.size();
     if ( nlons <2 | nlats < 2 ) {
        throw (badgridspec());
     }   

     lons = xlons;
     lats = xlats;

}

std::string MetGridSBRot::time2Cal( double time, int format ) 
{
    return metfcn->time2Cal(time, format);
}


double MetGridSBRot::cal2Time( std::string cal )
{
    return metfcn->cal2Time(cal);
}

void MetGridSBRot::defineCal( string caldate, double time )
{
    metfcn->defineCal( caldate, time );
}

void MetGridSBRot::set_vertical( const std::string quantity, const std::string units )
{
     set_vertical( quantity, units, NULLPTR );
}

void MetGridSBRot::set_vertical( const std::string quantity, const std::string units, const std::vector<real>* levels )
{
     std::string myunits;
     std::string my_quant;
     const std::vector<real>* levs;
     real myScale, myOffset;
     
     my_quant = "alt";
     if ( quantity != "" ) {
        my_quant = quantity;
     }  
      
    
     myunits = units;
     if ( units == "" ) {
        if ( my_quant == altitude_name ) {
           myunits = "km";
        } else if ( my_quant == pressure_name ) {
           myunits = "hPa";
        } else if ( my_quant == pottemp_name ) {
           myunits = "K";
        } else {
           // should we throw an exception here?
           myunits = "m";
        }         
     }
     
     vMKS( my_quant, myunits, &myScale, &myOffset );
     
     
     if ( levels == NULLPTR ) {
        levs = vcoords( &my_quant );
     } else {
        levs = levels;     
     }


     MetGridData::set_verticalBase( my_quant, myunits, levs, myScale, myOffset );

     if ( levels == NULLPTR ) {
        delete levs;
     }

     wind_vert_name = (vertwind_quants[ vquant ]).quantity;
     wfctr = (vertwind_quants[ vquant ]).MKSscale/vMKSscale; // convert to vert-coord-units/s
         
}

int MetGridSBRot::vIncrease() const
{
    int result;
    
    if ( vquant == altitude_name ) {
       result = 1;
    } else if ( vquant == pressure_name ) {
       result = -1;
    } else if ( vquant == pottemp_name ) {
       result = 1;
    } else {
       // should we throw an exception here?
       result = 1;
    }         
    
    return result;
}

bool MetGridSBRot::legalQuantity( const std::string quantity )
{
    // any quantity is egal
    return true;
}


int MetGridSBRot::setup(  const std::string quantity, const double time )
{
    int ndims = 3;
    
    if ( quantity == "trop" 
    || quantity == "tropz" 
    || quantity == "tropp" ) {
       ndims = 2;
    }
    
    return ndims;
}    

int MetGridSBRot::setup(  const std::string quantity, const std::string &time )
{
    int ndims = 3;

    if ( quantity == "trop" 
    || quantity == "tropz" 
    || quantity == "tropp" ) {
       ndims = 2;
    }
    
    return ndims;
}    


GridLatLonField3D* MetGridSBRot::new_directGrid3D( const std::string quantity, const std::string time ) 
{
    int i,j,k;
    real lon, lat, z;
    GridLatLonField3D* grid3d;
    double mtime;
    real val;
    std::vector<real> data;
    std::vector<real>* levs;
    int nzs;

    if ( quantity == "nodataload" ) {
       throw (MetGridData::baddataload());
    }   
    
    if ( dbug > 2 ) {
       std::cerr << "MetGridSBRot::new_directGrid3D: reading " << quantity << " @ " << time << std::endl;
    }

    // translate the time    
    mtime = metfcn->cal2Time(time);

    /// 3D gridded field
    
    grid3d = new GridLatLonField3D();
    grid3d->set_time( mtime, time );
    grid3d->set_fillval( -9999.0 );
    grid3d->set_vertical(altitude_name);
    grid3d->set_vunits("km");
    grid3d->mksVScale = 1000.0;
    grid3d->mksVOffset = 0.0;
    grid3d->set_quantity(quantity);
    if ( quantity == "u" || quantity == "v" || quantity == "w" ) {
        grid3d->set_units("m/s");
    } else if ( quantity == "t" or quantity == pottemp_name ) {
        grid3d->set_units("K");
    } else if ( quantity == "z" or quantity == altitude_name ) {
        grid3d->set_units("km");
    } else if ( quantity == pressure_name ) {
        grid3d->set_units("mb");
        grid3d->mksOffset = 0.0;
        grid3d->mksScale = 100.0; // (mb->Pa)
    } else {     
        grid3d->set_units("");
    }

    grid3d->setPgroup( my_pgroup, my_metproc );

    levs = vcoords( &altitude_name );
    nzs = levs->size();

    data.reserve(nlons*nlats*nzs);
    for (k=0; k < nzs; k++) {
        //z = native_zs[k];
        z = (*levs)[k];
        for (j=0; j<nlats; j++ ) {
            lat = lats[j];
            for (i=0; i<nlons; i++ ) {
                lon = lons[i];
                val = metfcn->getData( quantity, mtime, lon, lat, z );
                data.push_back( val );
            }
        }
    }

    //- std::cerr << "Loading lons, lats, zs into grid" << std::endl;
    grid3d->load( lons, lats, *levs, data );
    
    delete levs;

    return grid3d;
    

}

GridLatLonFieldSfc* MetGridSBRot::new_directGridSfc( const std::string quantity, const std::string time ) 
{
    int i,j;
    real lon, lat;
    GridLatLonFieldSfc* gridsfc;
    GridLatLonFieldSfc* desiredsfc;
    GridLatLonField3D* grid3D;
    GridLatLonField3D* desired3D;
    double mtime;
    real val;
    std::vector<real> data;
    std::string sfcname;
    std::string quantname;
    size_t pos;

    if ( quantity == "nodataload" ) {
       throw (MetGridData::baddataload());
    }   

    // split the quantity name into quantity and surface
    pos = quantity.find("@");
    if ( pos != string::npos ) {
       quantname = quantity.substr(0, pos);
       sfcname = quantity.substr(pos+1);
    } else {
       
       if ( quantity == "tropz" ) {
          quantname = "alt";
          sfcname = "trop";
       } else if ( quantity == "tropp" ) {
          quantname = "p";
          sfcname = "trop";       
       } else if ( quantity == "trop" ) {
          quantname = vquant;
          sfcname = "trop";       
       } else {   
          quantname = quantity;
          sfcname = "sfc";   // note where we are evaluating the quantity, below
       }
       
    }

    if ( dbug > 2 ) {
       std::cerr << "MetGridSBRot::new_directGridSfc: reading " << quantity << " (" << quantname << " on " << sfcname << ") @ " << time << std::endl;
    }


    // translate the time    
    mtime = metfcn->cal2Time(time);

    // create the output gridded surface
    gridsfc = new GridLatLonFieldSfc();
    gridsfc->set_quantity(quantity);
    gridsfc->set_units("");
    gridsfc->set_time( mtime, time );
    gridsfc->set_fillval( -9999.0 );
    gridsfc->set_surface(sfcname);
    gridsfc->setPgroup( my_pgroup, my_metproc );
    

    if ( sfcname == "trop" ) {
       // The surface is the tropopause

       // get the 3D temperatures on our desired vertical coordinate
       grid3D = new_directGrid3D("t", time); // get temperature on altitude
       // compute tropopause from temp on the desired vertical coordinate
       desiredsfc = dynamic_cast<GridLatLonFieldSfc*>(tropgen.wmo( *grid3D ));
       if ( desiredsfc->quantity() != quantname ) {
          // get the desired quantity on destied vertical coord
          desired3D = new_directGrid3D(quantname, time); 
          // interpolate the desired quantity onto the desired surface
          remove( gridsfc );
          gridsfc = dynamic_cast<GridLatLonFieldSfc*>(vin->surface( *desired3D, *desiredsfc ));
          remove( desired3D );
          remove( desiredsfc );
       } else {
          delete gridsfc;
          gridsfc = desiredsfc;
          gridsfc->set_surface(sfcname);
          gridsfc->setPgroup( my_pgroup, my_metproc );
       }
       remove( grid3D );
    } else if ( sfcname == "sfc" ) { 
      data.reserve(nlons*nlats);
      for (j=0; j<nlats; j++ ) {
          lat = lats[j];
          for (i=0; i<nlons; i++ ) {
              lon = lons[i];
              val = metfcn->getData( quantity, mtime, lon, lat, 0.0 );
              data.push_back( val );
          }
      }

      gridsfc->load( lons, lats, data, 0);

    } else {
      remove(gridsfc);
      throw (badsurface());
    } 

    return gridsfc;
    

}


bool MetGridSBRot::bracket( const std::string &quantity, const std::string &time, std::string *t1, std::string *t2)
{
    double wanted, prev, next;

    wanted = metfcn->cal2Time(time);
    
    bracket( quantity, wanted, &prev, &next );
    
    *t1 = metfcn->time2Cal(prev);
    *t2 = metfcn->time2Cal(next);

    return (prev == next);
}

bool MetGridSBRot::bracket( const std::string &quantity, double time, double *t1, double *t2)
{
    
    *t1 = floor(time);
    *t2 = ceil(time);
   
    return (*t1 == *t2);
}




FilePath* MetGridSBRot::cachefile( const GridFieldSfc* item ) const
{
    FilePath* filepath;
    std::string fname;
    std::string date;
    std::string type;
    const GridLatLonFieldSfc* actualItem;
    
    actualItem = dynamic_cast<const GridLatLonFieldSfc*>(item);

    filepath = NULLPTR;

    if ( item->cacheable() ) {

       filepath = new FilePath;
       *filepath = *diskcachedir;
       
       fname = "MetGridSBRot_"
               + actualItem->quantity()
               + "_" + actualItem->met_time()
               + "_Sfc" + actualItem->surface()
               + ".cache";
       
       // in other data sources, we might want to 
       // extract the year and month from the date and
       // make some subdirectories here
       
       // put the file name at the end of the path
       filepath->append( fname );
       
    }
    
    return filepath;  
    
}


FilePath* MetGridSBRot::cachefile( const GridField3D* item ) const
{
    FilePath* filepath;
    std::string fname;
    std::string date;
    std::string type;
    std::string month, year, day;
    const GridLatLonField3D* actualItem;
    
    actualItem = dynamic_cast<const GridLatLonField3D*>(item);

    filepath = NULLPTR;

    if ( item->cacheable() ) {

       filepath = new FilePath;
       *filepath = *diskcachedir;
       
       date = actualItem->met_time();
       
       fname = "MetGridSBRot_"
               + actualItem->quantity()
               + "_" + date
               + "_3D_" + actualItem->vertical()
               + ".cache";

       if ( metfcn->get_cal() == 1 ) {       
          year = date.substr(0,4);
          month = date.substr(5,2);
          day = date.substr(8,2);
          filepath->append( "Y"+year  );
          filepath->append( "M"+month );
          filepath->append( "D"+day   );
       }
       
       // put the file name at the end of the path
       filepath->append( fname );
       
    }
    
    return filepath;    
    
}

