
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
#include "gigatraj/BilinearHinterp.hh"
#include "gigatraj/LinearVinterp.hh"
#include "gigatraj/LogLinearVinterp.hh"

using namespace gigatraj;

// constructor
MetGridData::MetGridData() : MetData() 
{
      vWindStuff vw;
            
      vquant = "none";
      vuu = "N/A";
      vMKSscale = 1.0;
      vMKSoffset = 0.0;      
      vin = new LinearVinterp();
      myVin = true;
      hin = new BilinearHinterp();
      myHin = true;
      maxsnaps = 3;
      
      // use CF conventions by default
      //  zonal wind
      wind_ew_name = "eastward_wind";
      //  meridional wind
      wind_ns_name = "northward_wind";
      //  vertical wind
      wind_vert_name = "upward_air_velocity";
      // other quantities
      pressure_name = "p";
      altitude_name = "alt";
      palt_name = "PAlt";
      pottemp_name = "Theta";
      pressureDot_name = "omega";
      altDot_name = "w";
      paltDot_name = "PAltDot";
      thetaDot_name = "ThetaDot";
      
      us = new MetCache3D(wind_vert_name, maxsnaps);
      vs = new MetCache3D(wind_ew_name, maxsnaps);
      ws = new MetCache3D(wind_ns_name, maxsnaps);

      xSfc = NULLPTR;
      x3D = NULLPTR;

      field3Ds.clear();
      field2Ds.clear();
      
      diskcachedir = NULLPTR;
      diskcaching = false;
      
      override_tbase = -1;
      override_tspace = -1;
    
      getpalt.set_quantity(palt_name);
      getpalt.setPressureName( pressure_name );

      vw.quantity = "PAltDot";
      vw.units = "km/s";
      vw.MKSscale = 1000.0;
      vw.MKSoffset = 0.0;
      vertwind_quants[ palt_name ] = vw;

      getpaltdot.set_quantity((vertwind_quants[ palt_name ]).quantity);
      getpaltdot.setPressureAltitudeName(palt_name);
      getpaltdot.setPressureName(pressure_name);
      getpaltdot.setPressureDotName("unknown");

}

// destructor
MetGridData::~MetGridData()
{

     // get rid of the interpolators
     if ( myVin ) {
        delete vin;
     }
     if ( myHin ) {
        delete hin;
     }  

     flush_cache();

     // get rid of the (empty) wind caches
     delete us;
     delete vs;
     delete ws;

     if ( diskcachedir != NULLPTR ) {
        delete diskcachedir;
     }   

}

// copy constructor
MetGridData::MetGridData( const MetGridData& src ) : MetData(src)
{
    // note: this calls the MetData copy constructor, then the MetGridData::assign() method
    // The  MetGridData::assign method calls the MetData::assign() method
    // which is a redundant. Not very efficient, but should cause no harm.
    // the benefit is that we only have to maintain the assign() method below.
    assign(src);
}


// copy into self
void MetGridData::assign( const MetGridData& src )
{

      MetData::assign(src);

      altitude_name = src.altitude_name ;
      pressure_name = src.pressure_name;
      palt_name = src.palt_name;
      pottemp_name = src.pottemp_name;
      
      vquant   = src.vquant;
      vuu      = src.vuu;
      vMKSscale = src.vMKSscale;
      vMKSoffset = src.vMKSoffset;      
      if ( myVin ) {
         delete vin;
      }   
      if ( src.myVin ) {
         vin = src.vin->copy();
         // this is just a copy, so we should delete it when we are done
         myVin = true;
      } else {
         // the vin in the source is not native
         // to that source, so we just copy the pointer
         set_vinterp( src.vin );
      }
      if ( myHin ) {
         delete hin;
      }   
      if ( src.myHin ) {
         hin = src.hin->copy();
         myHin = true;
      } else {
         set_hinterp( src.hin );
      }

      maxsnaps = src.maxsnaps;

      wind_ew_name = src.wind_ew_name ;
      wind_ns_name = src.wind_ns_name ;
      wind_vert_name = src.wind_vert_name ;

      flush_cache();
      
      us = new MetCache3D(wind_ew_name, maxsnaps);
      vs = new MetCache3D(wind_ns_name, maxsnaps);
      ws = new MetCache3D(wind_vert_name, maxsnaps);
      
      if ( src.diskcachedir != NULLPTR ) {
         if ( diskcachedir == NULLPTR ) {
            diskcachedir = new FilePath();
         } 
         *diskcachedir = *src.diskcachedir;
      } else {
         delete diskcachedir;
         diskcachedir = NULLPTR;
      }      
      diskcaching = src.diskcaching;      

      override_tbase = src.override_tbase;
      override_tspace = src.override_tspace;

      // Note: we do not copy the actual cached met data. 
      // Those data will be loaded afresh into this object
      // as requests are made to it.    


}


void MetGridData::setOption( const std::string &name, const std::string &value )
{    
    int tmp;
    int ival;
    int fval;
    int dval;
    int bval;
    
    if ( name == "HorizontalGridThinning" ) {
        if ( str2int( value, &ival ) ) {
           set_thinning(ival);
        }   
    } else if ( name == "HorizontalGridOffset" ) {
        tmp = thinning();
        if ( str2int( value, &ival ) ) {
           set_thinning( tmp, ival );
        }   
    } else if ( name == "PressureName" ) {
        pressure_name = value;
    } else if ( name == "AltitudeName" ) {
        altitude_name = value;
    } else if ( name == "PressureAltitudeName" ) {
        palt_name = value;
    } else if ( name == "PotentialTemperatureName" ) {
        pottemp_name = value;
    } else if ( name == "PressureDotName" ) {
        pressureDot_name = value;
    } else if ( name == "AltitudeDotName" ) {
        altDot_name = value;
    } else if ( name == "PressureAltitudeDotName" ) {
        paltDot_name = value;
    } else if ( name == "PotentialTemperatureDotName" ) {
        thetaDot_name = value;
    } else {
        MetData::setOption( name, value ); 
    }
}

void MetGridData::setOption( const std::string &name, int value )
{
    int tmp;
    
    if ( name == "HorizontalGridThinning" ) {
        set_thinning(value);
    } else if ( name == "HorizontalGridOffset" ) {
        tmp = thinning();
        set_thinning( tmp, value );
    } else {
        MetData::setOption( name, value ); 
    }
}

void MetGridData::setOption( const std::string &name, float value )
{
     MetData::setOption( name, value ); 
}

void MetGridData::setOption( const std::string &name, double value )
{
     MetData::setOption( name, value ); 
}

bool MetGridData::getOption( const std::string &name, std::string &value )
{
    bool result = false;
    int ival;
    
    if ( name == "HorizontalGridThinning" ) {
        ival = thinning();
        result = int2str( ival, value );
    } else if ( name == "HorizontalGridOffset" ) {
        (void) thinning(&ival);
        result = int2str( ival, value );
    } else if ( name == "PressureName" ) {
        value = pressure_name;
        result = true;
    } else if ( name == "AltitudeName" ) {
        value = altitude_name;
        result = true;
    } else if ( name == "PressureAltitudeName" ) {
        value = palt_name;
        result = true;
    } else if ( name == "PotentialTemperatureName" ) {
        value = pottemp_name;
        result = true;
    } else if ( name == "PressureDotName" ) {
        value = pressureDot_name;
        result = true;
    } else if ( name == "AltitudeDotName" ) {
        value = altDot_name;
        result = true;
    } else if ( name == "PressureAltitudeDotName" ) {
        value = paltDot_name;
        result = true;
    } else if ( name == "PotentialTemperatureDotName" ) {
        value = thetaDot_name;
        result = true;
    } else {
        result = MetData::getOption( name, value ); 
    }
     
    return result;
}

bool MetGridData::getOption( const std::string &name, int &value )
{
    bool result;
    int tmp;
    
    if ( name == "HorizontalGridThinning" ) {
        value = thinning();
        result = true;
    } else if ( name == "HorizontalGridOffset" ) {
        (void) thinning(&tmp);
        value = tmp;
        result = true;
    } else {
       result = MetData::getOption( name, value ); 
    }

    return result;
}


bool MetGridData::getOption( const std::string &name, float &value )
{
    bool result;
    
    result = MetData::getOption( name, value ); 

    return result;
}


bool MetGridData::getOption( const std::string &name, double &value )
{
    bool result;
    
    result = MetData::getOption( name, value ); 

    return result;
}


void MetGridData::set_vinterp( Vinterp* vinterp, bool okToDelete  )
{
     if ( myVin ) {
        delete vin;
     }
     vin = vinterp;
     myVin = okToDelete;
}

void MetGridData::set_hinterp( HLatLonInterp* hinterp, bool okToDelete )
{
     if ( myHin ) {
        delete hin;
     }
     hin = hinterp;
     myHin = okToDelete;
}

int MetGridData::thinning( int *offset )
{
    if ( offset != NULL ) {
       *offset = skoff;
    }
    
    return skip;
}

void MetGridData::set_thinning( int thin, int offset )
{
    if ( thin > 1 ) {
       skip = thin;
    } else {
       skip = 1;
    } 
    if ( offset >= 0 ) {
       skoff = offset;
    } else {
       skoff = 0;
    }
    
}


void MetGridData::flush_cache() 
{
     std::map< std::string, MetCache3D* >::iterator i;
     std::map< std::string, MetCacheSfc* >::iterator j;

     // get rid of the winds
     delete us;
     delete vs;
     delete ws;
     
     // get rid of the other met fields
     for ( i = field3Ds.begin(); i != field3Ds.end(); i++ ) {
        delete (*i).second;
     }    
     for ( j = field2Ds.begin(); j != field2Ds.end(); j++ ) {
        delete (*j).second;
     }    
     
     field3Ds.clear();
     field2Ds.clear();

     us = new MetCache3D(wind_ew_name, maxsnaps);
     vs = new MetCache3D(wind_ns_name, maxsnaps);
     ws = new MetCache3D(wind_vert_name, maxsnaps);

}


std::string MetGridData::u_wind( std::string *units ) const
{
    if ( units != NULLPTR ) {
       *units = "m/s";
    }
    return wind_ew_name;   
}

std::string MetGridData::v_wind( std::string *units ) const
{
    if ( units != NULLPTR ) {
       *units = "m/s";
    }
    return wind_ns_name;   
}

std::string MetGridData::w_wind( std::string *units ) const
{
    if ( units != NULLPTR ) {
       *units = "";
    }
    return wind_vert_name;   
}

std::string MetGridData::vertical( std::string *units ) const
{
    if ( units != NULLPTR ) {
       *units = vuu;
    }
    return vquant;   
}

std::string MetGridData::vunits() const
{
    return vuu;   
}



void MetGridData::set_verticalBase( const std::string quantity, const std::string units, const std::vector<real>* levels, real scale, real offset )
{

    if ( legalQuantity(quantity) ) {

       vquant = quantity;
       vuu = units;
       vMKSscale = scale;
       vMKSoffset = offset; 
       
       /* removed: too much overhead for too little payoff
       if ( myVin ) {
          delete vin;
          if ( vquant == pressure_name || vquant == pottemp_name ) {
             vin = new LogLinearVinterp();
          } else {
             vin = new LinearVinterp();
          }         
       } 
       */ 
    
       zs.clear();
       if ( levels != NULLPTR ) {
          zs = *levels;
       } 

       flush_cache();
    } else {
       throw (badquantity());
    } 

}

void MetGridData::set_verticalBase( GridField3D *grid )
{
    real new_vscale;
    real new_voffset;
    std:string new_vuu = grid->vunits( &new_vscale, &new_voffset );
    std::string vname = grid->vertical();
    set_verticalBase( vname, new_vuu, NULLPTR, new_vscale, new_voffset );

}    

void MetGridData::remove( GridField3D* field )
{
    std::map< std::string, MetCache3D* >::iterator i;
    bool keepit = false;
    
    if ( field == NULLPTR ) {
       keepit = true;
    }
    
    if ( ! keepit ) {
       // have we cached this in any of our wind fields?
       if ( us->has(field) || vs->has(field) || ws->has(field) ) {
          keepit = true;
       }
    }

    // is it cached elsewhere?
    if ( ! keepit ) {
       for ( i = field3Ds.begin(); i != field3Ds.end(); i++ ) {
           if ( (*i).second->has(field) ) {
              keepit = true;
              break;
           }
       }
    }    

    if ( ! keepit ) {
       delete field;
    }

}

void MetGridData::remove( GridFieldSfc* field )
{
    std::map< std::string, MetCacheSfc* >::iterator j;
    bool keepit = false;
    
    if ( field == NULLPTR ) {
       keepit = true;
    }
    
    // is it cached somewhere?
    if ( ! keepit ) {
       for ( j = field2Ds.begin(); j != field2Ds.end(); j++ ) {
          if ( (*j).second->has(field) ) {
             keepit = true;
             break;
          }
       }    
    }    

    if ( ! keepit ) {
       delete field;
    }

}

void MetGridData::impose_times( double otbase, double otspace )
{
       if ( otbase < 24.0 ) {
          override_tbase = otbase;
       } else {
          throw(badtimespec());
       }
       
       override_tspace = otspace;
}

double MetGridData::get_imposed_timebase()
{
    return override_tbase;
}

double MetGridData::get_imposed_timedelta()
{
    return override_tspace;
}

void MetGridData::vMKS( const std::string quantity, const std::string units, real* scale, real* offset) const 
{

      *scale = 1.0;
      *offset = 0.0;
      
      if ( quantity == altitude_name || quantity == "altitude"  
       ||  quantity == palt_name     || quantity == "pressure_altitude"  ) {
         if ( units == "km" ) {
            *scale = 1000.0;
         } else if ( units == "m" ) {
            *scale = 1.0;
         } else {
            throw (badvunits());
         }
      } else if ( quantity == pressure_name || quantity == "air_pressure"  ) {
         if ( units == "hPa" || units == "mb" ) {
            *scale = 100.0;
         } else {
            throw (badvunits());
         }
      } else if ( quantity == pottemp_name || quantity == "air_potential_temperature" ) {
         if ( units == "K" ) {
            *scale = 1.0;
         } else {
            throw (badvunits());
         }
      } else {
         throw (badvunits());
      }

}

bool MetGridData::vConvert( GridField3D *input, std::string quant, std::string units, real scale, real offset )
{
    return false;
}


GridField3D* MetGridData::new_mgmtGrid3D( const std::string& quantity, const std::string& time )
{
    GridField3D* grid;
    GridField3D* vgrid;
    GridField3D* newgrid;
    MetCache3D *cache;
    std::map< std::string, MetCache3D* >::iterator qm;
    size_t pos;
    int cmd;
    std::vector<real> xtst;
    real xval1,xval2;
    int i1,i2, j1, j2;

    //- std::cerr << "MetGridData::new_mgmtGrid3D: Hello. I am a met ";
    //- if ( isMetClient() ) {
    //-    std::cerr << " client " << std::endl;
    //- } else {
    //-    std::cerr << " **SERVER** " << std::endl;     
    //- }

    //- std::cerr << "MetGridData::new_mgmtGrid3D: vquant is '" << vquant << "'" << std::endl;
 
    // we set out local caching object "cache" to 
    // whatever source we are trying to read from.
    // U, V, and W winds have their own dedicated caches.
    if ( quantity == wind_ew_name ) {
       if ( dbug > 2 ) {
           std::cerr << "MetGridData::new_mgmtGrid3D: using US for memory cache" << std::endl;
       }
       cache = us;
    } else if ( quantity == wind_ns_name ) {
       if ( dbug > 2 ) {
           std::cerr << "MetGridData::new_mgmtGrid3D: using VS for memory cache" << std::endl;
       }
       cache = vs;
    } else if ( quantity ==  wind_vert_name ) {
       if ( dbug > 2 ) {
           std::cerr << "MetGridData::new_mgmtGrid3D: using WS for memory cache" << std::endl;
       }
       cache = ws;
    } else {
       // not U, V, or W cached wind fields.
       
       // is this quantity something we are caching already?
       if ( (qm = field3Ds.find(quantity)) != field3Ds.end() ) {
          // yes. Use that.
          cache = (*qm).second;
       } else {
          // No, not something we are caching already.
          // Create a new cache object and add it to the collection.
          cache = new MetCache3D(quantity, maxsnaps);
          field3Ds[quantity] = cache;
       }    

    }        

    if ( dbug > 0 ) {
       std::cerr << "MetGridData::new_mgmtGrid3D: Want " << quantity << " on " << vquant << " @ " << time << std::endl;    
    }
    
    // try to get the cached field valid for our desired time
    grid = cache->query(time);
    if ( grid == NULLPTR ) {
       // no cached data for the desired time
        
       // are we getting met data from a dedicated met processor?
       if ( ! isMetClient() ) {
          // No?   Ok, we should be reading met data ourselves.
          // Do it.
          
          // try the disk cache
          grid = readCache3D(quantity, time);
          if ( grid == NULLPTR ) {
     
             // data not in cache.  we have to go get it.

             if ( dbug >= 1 ) {
                std::cerr << "MetGridData::new_mgmtGrid3D:  requesting to read " << quantity << std::endl;    
             }

             // read in the data from the actual source
             grid = new_directGrid3D( quantity, time );
             if ( grid != NULLPTR ) {
             
                if ( dbug >= 1 ) {
                   std::cerr << "MetGridData::new_mgmtGrid3D:     got " << grid->quantity() << " on " << grid->vertical() << std::endl;   
                }
                
                // We have read in the raw data from its source, and grid now contains that.
                // But the data might be on a different vertical grid than we want.
                
       
                // if the grid's vertical coordinates units don't match, it's an error
                // (new_directGrid3D(), which read the data in, should have ensured that
                // the units match.)
                if ( grid->vertical() == vquant && grid->vunits() != vuu ) {
                   throw (badvunits());
                }
                
                // Else if the data are read in on a different vertical coordinate
                // than we are using...
                if ( grid->vertical() != vquant ) {
                   if ( dbug >= 1 ) {
                      std::cerr << "MetGridData::new_mgmtGrid3D:  have " << grid->quantity() << " data on <<" << grid->vertical() 
                      << ">>  but need it on <<" << vquant << ">>"  << std::endl;
                   }
                   if ( grid->quantity() == grid->vertical() ) {
                      // This is the trivial case of the data grid we
                      // just read being constant surfaces of the vertical coord values.
                   
                      //  The data source uses grid->quantity as its native vertical coordinate
                      //  So we want to read in our preferred vertical coordinate instead
                      //  and invert the resulting grid.
                      vgrid = new_directGrid3D( vquant, time );
                      if ( vgrid != NULLPTR && vgrid->vertical() == grid->vertical() 
                        && vgrid->vunits() == grid->vunits() ) {
                         // throw away the useless old grid.
                         // (note that new_directGrid3D() returns deletable grid objects.
                         // unlike new_manageGrid3D().) 
                         delete grid;
                         // invert the data just read to obtain the grid we want.
                         grid = vin->invert( zs, *vgrid );
                      } else {
                         throw (baddataload());
                      }      
                      delete vgrid;
                      if ( dbug >= 1 ) {
                         std::cerr << "MetGridData::new_mgmtGrid3D:              re-read & inverted to get " << grid->quantity() 
                         << " on " << grid->vertical() << std::endl;
                      }
                   } else {
                      // The raw data we just read uses some other quantity as its vertical coordinate.
                      
                      // if the preferred vcoordinate is an analytical function of the current one,
                      // then we convert it now
                      if ( ! vConvert( grid, vquant, vuu ) ) {
                         // ok, not an analytical function.
                         // so we have to read in the preferred quantity and interpolate
                      
                         // Ask for that quantity on our preferred vertical coordinate.
                      
                         try {
                            // We call ourselves, so that "vgrid" can be cached and will
                            // not have to be read the next time we need it.
                            vgrid = new_mgmtGrid3D( grid->vertical(), time );
                         } catch (...) {
                            // we don't have the native-coord on desired-coord
                            if ( dbug >= 1 ) {
                               std::cerr << "MetGridData::new_mgmtGrid3D:              cannot find " << grid->vertical() 
                               << " on " << vquant << " for " << grid->quantity() << std::endl;
                            }
                            vgrid = NULLPTR;
                         }
                         if ( vgrid == NULLPTR ) {
                            // we could not get the actual vcoord on the desired surface,
                            // so try to get the desired vcoord on the actual vcoord surfaces,
                            // then invert that grid.
           //                 try {

                               std::vector<real>* target_vs = vcoords( NULLPTR );

                               MetGridData* newsrc = MetGridCopy();
                               // change over to the read-data's actual vcoord                               
                               newsrc->set_verticalBase( grid );

                               // Read the desired vcoord on the read-data actual coord surfaces
                               newgrid = newsrc->new_mgmtGrid3D( vquant, time );
                               if ( newgrid != NULLPTR ) {
                                  // fiddle with the units of the?
                                  if ( newgrid->units() != vuu ) {
                                     newgrid->transform( vuu, vMKSscale, vMKSoffset );
                                  }
                                  // now invert the coords
                                  
                               }
                               
                               // invert the grid we just read in
                               vgrid = vin->invert( *target_vs, *newgrid );
                               
                               //delete newgrid;
                               delete newsrc;
                               
             //               } catch (...) {
             //                  // we don't have the native-coord on desired-coord
             //                  if ( dbug >= 1 ) {
             //                     std::cerr << "MetGridData::new_mgmtGrid3D:              cannot find " << grid->vertical() 
             //                     << " on " << vquant << " for " << grid->quantity() << std::endl;
             //                  }
             //                  vgrid = NULLPTR;
             //               }
                         }
                         if ( vgrid == NULLPTR ) {
                            throw (baddataload());
                         }
                         //  A(C) & C(B) -> A(B)
                         newgrid = vin->reProfile( *grid, *vgrid );
                         delete grid;
                         // Note: we do NOT delete vgrid, since it is now in the cache (i.e., a pointer
                         // in the cache points to that memory)!
                         remove( vgrid );
                         // our newgrid is now our output grid
                         grid = newgrid;             
                         if ( dbug >= 1 ) {
                            std::cerr << "MetGridData::new_mgmtGrid3D:              reProfiled to get " << grid->quantity() 
                            << " on " << grid->vertical() << std::endl;
                         }
                      }
                   }
                      
                }

                // This new data grid is not yet set up for multiprocessing,
                // so do that now.
                grid->setPgroup( my_pgroup, my_metproc ); 
             
                // add it to the disk cache
                if ( dbug >= 2 ) {
                   std::cerr << "MetGridData::new_mgmtGrid3D:  writing " << grid->quantity() << " @ " << grid->met_time() << " to disk cache" << std::endl;
                }    
                writeCache(grid);

             } else {
                if ( dbug > 0 ) {
                   std::cerr << "MetGridData::new_mgmtGrid3D:  failed to read met data from source" << std::endl;            
                }
             }
             
          } else {
             if ( dbug >= 1 ) {
                std::cerr << "MetGridData::new_mgmtGrid3D:  request fulfilled from disk cache" << std::endl;
             }  
             
             // need to set the PGrp stuff before we use this grid.
             grid->setPgroup( my_pgroup, my_metproc );  
             
          }

          // either through reading from the data source or from disk cache
          if ( grid != NULLPTR ) {
             // add it to the in-memory cache
             if ( dbug >= 1 ) {
               std::cerr << "MetGridData::new_mgmtGrid3D:  adding data to memory cache" << std::endl;
             }
             cache->add(grid);
          }   
    
          if ( dbug >= 2 ) {
             std::cerr << "MetGridData::new_mgmtGrid3D:  cache report:" << std::endl;
             report();
          }                                                                              
        
       } else {
       
          // ok, we need to be reading the data from our met processor
          
          // basically, we ask the processor to fetch data, and we receive
          // a pointer to a grid object which has only metadata in it.
          // This grid object may then be cached in memory and pulled up
          // as needed.  Attempts to interpolate data from such an object
          // will talk to the met processor object instead.
          if ( dbug >= 1 ) {
             std::cerr << "MetGridData::new_mgmtGrid3D:  (met client) asking met processor " << my_metproc << " for metadata" << std::endl;
          } 
          
          grid = new_clientGrid3D( quantity, time );   
       
          if ( dbug >= 1 ) {
            std::cerr << "MetGridData::new_mgmtGrid3D:  (met client) grid created and received metadata from met processor" << std::endl;
          }    
          
          // ok, we now have a cliebt grid, which has metadata but no 
          // actual data. The data will be obtained from the met swrver process.
          
          // but we do want to save this in memeory cache
          if ( grid != NULLPTR ) {
             // add it to the in-memory cache
             if ( dbug >= 1 ) {
               std::cerr << "MetGridData::new_mgmtGrid3D:  adding client grid to memory cache" << std::endl;
             }
             cache->add(grid);
          }   
         
         
       }


    } else {
       if ( dbug >= 1 ) {
          std::cerr << "MetGridData::new_mgmtGrid3D:  request fullfilled from memory cache" << std::endl;
       } 
       // note: since the grid was retrieved from cache, its
       // group stuff is already in place.
    }

    if ( dbug > 0 ) {
       std::cerr << "MetGridData::new_mgmtGrid3D:  returning " << quantity << " on " << vquant << " @ " << time << std::endl;
    }


    return grid;
}

void MetGridData::request_meta3D( const std::string& quantity, double time ) 
{
     int cmd;
     std::string ctime;
     
     if ( isMetClient() ) {
        // send this request to the met server processor's Met Source object    
        // send request for metadata
        cmd = PGR_CMD_M3M;
        my_pgroup->send_ints( my_metproc, 1, &cmd, PGR_TAG_REQ );
        //- std::cerr << "   MetGridData::request_meta3D:  (met client) sent cmd " << std::endl;
        // send string for quantity
        my_pgroup->send_string( my_metproc, quantity, PGR_TAG_QUANT ); // quantity
        //- std::cerr << "   MetGridData::request_meta3D:  (met client) sent quantity " << std::endl;
        // send string for vertical coord
        ctime = time2Cal( time );
        my_pgroup->send_string( my_metproc, ctime, PGR_TAG_TIME ); // time
        //- std::cerr << "   MetGridData::request_meta3D:  (met client) sent time " << std::endl;
     }

}


void MetGridData::request_data3D( std::string& quantity, std::string& time )
{
     int cmd;
     
     if ( isMetClient() ) {
         //- std::cerr << "MetGridData::request_data3D: (client) sending SCALAR request" << std::endl;
         // send "need data" status to central met reader process
         cmd = PGR_CMD_M3D;
         // send request for data
         my_pgroup->send_ints( my_metproc, 1, &cmd, PGR_TAG_REQ );
         // send the desired quantity to the server
         my_pgroup->send_string( my_metproc, quantity, PGR_TAG_QUANT );
         // send the desired timestamp to the server
         my_pgroup->send_string( my_metproc, time, PGR_TAG_TIME );
         //std::cerr << "MetGridData::request_data3D: (client) sent request" << std::endl;
     }
}

void MetGridData::request_data3D( std::string& xquantity, std::string& yquantity, std::string& time )
{
     int cmd;
     
     if ( isMetClient() ) {
         //- std::cerr << "MetGridData::request_data3D: (client) sending VECTOR request" << std::endl;
         // send "need data" status to central met reader process
         cmd = PGR_CMD_M3DV;
         // send request for data
         my_pgroup->send_ints( my_metproc, 1, &cmd, PGR_TAG_REQ );
         // send the desired vector component quantities to the server
         my_pgroup->send_string( my_metproc, xquantity, PGR_TAG_QUANT );
         my_pgroup->send_string( my_metproc, yquantity, PGR_TAG_QUANT );
         // send the desired timestamp to the server
         my_pgroup->send_string( my_metproc, time, PGR_TAG_TIME );
         //std::cerr << "MetGridData::request_data3D: (client) sent request" << std::endl;
     }
}

GridFieldSfc* MetGridData::new_mgmtGridSfc( const std::string& quantity, const std::string& time )
{
    GridFieldSfc* grid;
    GridFieldSfc* vgrid;
    GridFieldSfc* newgrid;
    MetCacheSfc *cache;
    std::map< std::string, MetCacheSfc* >::iterator qm;
    std::string sfcname;
    std::string quantname;
    std::string fullqname;
    size_t pos;
    int cmd;

    //- std::cerr << "MetGridData::new_mgmtGridSfc: Hello. I am a met ";
    //- if ( isMetClient() ) {
    //-    std::cerr << " client " << std::endl;
    //- } else {
    //-    std::cerr << " **SERVER** " << std::endl;     
    //- }
 

    // split the quantity name into quantity and surface
    pos = quantity.find("@");
    if ( pos != string::npos ) {
       quantname = quantity.substr(0, pos+1);
       sfcname = quantity.substr(pos+1);
       // strip off any blanks from end (so "p @ trop" will work
       pos = quantname.find_last_not_of(" ");
       if ( pos != string::npos ) {
          quantname = quantname.substr(0,pos);
       }
       // likewise, strip off blanks from the beginning
       pos = sfcname.find_first_not_of(" ");
       if ( pos != string::npos ) {
          sfcname = sfcname.substr(pos);
       }
    } else {
       // No surface spec in quantity name
       
       //todo:  put in switch block here
       quantname = quantity;
       sfcname = "";  
    }
    
    // this will serve as the canonical name of the quantity
    // for caching purposes.
    fullqname = quantname;
    if ( sfcname != "" ) {
       fullqname = fullqname + "@" + sfcname;
    }   


    // is there a memory cachefor this field?
    if ( (qm=field2Ds.find(fullqname)) != field2Ds.end() ) {
       cache = (*qm).second;
    } else {
       cache = new MetCacheSfc(fullqname, maxsnaps);
       field2Ds[fullqname] = cache;
    }    


    if ( dbug > 0 ) {
       std::cerr << "MetGridData::new_mgmtGridSfc: Want " << quantname << " on Sfc " << sfcname << " @ " << time << std::endl;    
    }
    
    // try to get the cached field valid for our desired time
    grid = cache->query(time);
    if ( grid == NULLPTR ) {
     
       // are we getting met data from a dedicated met processor?
       if ( ! isMetClient() ) {
          // No? we should be reading met data ourselves.
          // Do it.
          
          // try the disk cache
          grid = readCacheSfc( fullqname, time );
          if ( grid == NULLPTR ) {

             // data not in cache.  we have to go get it.

             if ( dbug >= 1 ) {
                std::cerr << "MetGridData::new_mgmtGridSfc:  requesting to read " << quantity << std::endl;    
             }

             // read in the data form the actual source
             grid = new_directGridSfc( fullqname, time );
             if ( grid != NULLPTR ) {
       
                if ( dbug >= 1 ) {
                   std::cerr << "MetGridData::new_mgmtGridSfc:       got " << grid->quantity() << " on Sfc " << grid->surface() << std::endl;
                }

                // This new data grid is not yet set up for multiprocessing,
                // so do that now.
                grid->setPgroup( my_pgroup, my_metproc ); 
             
                // add it to the disk cache
                if ( dbug >= 2 ) {
                   std::cerr << "MetGridData::new_mgmtGridSfc:  writing " << grid->quantity() << " on Sfc " << grid->surface() << " @ " << grid->met_time() << " to disk cache" << std::endl;
                }    
                writeCache(grid);
             } else {
                if ( dbug > 0 ) {
                   std::cerr << "MetGridData::new_mgmtGridSfc:  failed to read met data from source" << std::endl;            
                }
             }   
          } else {
             if ( dbug >= 1 ) {
                std::cerr << "MetGridData::new_mgmtGrid3D:  request fulfilled from disk cache" << std::endl;
             }  
             
             // need to set the PGrp stuff befiore we use this grid.
             grid->setPgroup( my_pgroup, my_metproc );  
             
          }

          // either through reading from the data source or from disk cache
          if ( grid == NULLPTR ) {
             // add it to the in-memory cache
             if ( dbug > 1 ) {
               std::cerr << "MetGridData::new_mgmtGridSfc:  adding data to memory cache" << std::endl;
             }
             cache->add(grid);
          }
          
       } else {
       
          // ok, we need to be reading the data from our met processor
          
          // basically, we ask the processor to fetch data, and we receive
          // a pointer to a grid object which has only metadata in it.
          // This grid object may then be cached in memory and pulled up
          // as needed.  Attempts to interpolate data from such an object
          // will talk to the met processor object instead.
          if ( dbug >= 1 ) {
             std::cerr << "MetGridData::new_mgmtGridSfc:  (met client) asking met processor for metadata" << std::endl;
          }  
          
          grid = new_clientGridSfc( quantity, time );  
          if ( dbug >= 1 ) {
            std::cerr << "MetGridData::new_mgmtGridSfc:  (met client) grid created and received metadata from met processor" << std::endl;
          }    
          // but we do want to save this in memeory cache
          if ( grid != NULLPTR ) {
             // add it to the in-memory cache
             if ( dbug >= 1 ) {
               std::cerr << "MetGridData::new_mgmtGridSfc:  adding client grid to memory cache" << std::endl;
             }
             cache->add(grid);
          }   
       
       }
    } else {
       if ( dbug >= 1 ) {
          std::cerr << "MetGridData::new_mgmtGridSfc:  request fullfilled from memory cache" << std::endl;
       }    
       // note: since the grid was retrieved from cache, its
       // group stuff is already in place.
    }

    if ( dbug > 0 ) {
       std::cerr << "MetGridData::new_mgmtGridSfc:  returning " << quantname << " on Sfc " << sfcname << " @ " << time  << std::endl;
    }

    return grid;
}

void MetGridData::request_metaSfc( const std::string& quantity, double time ) 
{
     int cmd;
     std::string ctime;
     
     if ( isMetClient() ) {
        // send this request to the met server processor's Met Source object    
        // send request for metadata
        cmd = PGR_CMD_M2M;
        my_pgroup->send_ints( my_metproc, 1, &cmd, PGR_TAG_REQ );
        //- std::cerr << "   MetGridData::request_metaSfc:  (met client) sent cmd " << std::endl;
        // send string for quantity
        my_pgroup->send_string( my_metproc, quantity, PGR_TAG_QUANT ); // quantity
        //- std::cerr << "   MetGridData::request_metaSfc:  (met client) sent quantity " << std::endl;
        // send string for vertical coord
        ctime = time2Cal( time );
        my_pgroup->send_string( my_metproc, ctime, PGR_TAG_TIME ); // time
        //- std::cerr << "   MetGridData::request_metaSfc:  (met client) sent time " << std::endl;
     }

}


void MetGridData::request_dataSfc( std::string& quantity, std::string& time )
{
     int cmd;
     
     if ( isMetClient() ) {
         //- std::cerr << "MetGridData::request_dataSfc: (client) sending request" << std::endl;
         // send "need data" status to central met reader process
         cmd = PGR_CMD_M2D;
         // send request for data
         my_pgroup->send_ints( my_metproc, 1, &cmd, PGR_TAG_REQ );
         // send the desired quantity to the server
         my_pgroup->send_string( my_metproc, quantity, PGR_TAG_QUANT );
         // send the desired timestamp to the server
         my_pgroup->send_string( my_metproc, time, PGR_TAG_TIME );
         //- std::cerr << "MetGridData::request_dataSfc: (client) sent request" << std::endl;
     }
}

void MetGridData::request_dataSfc( std::string& xquantity, std::string& yquantity, std::string& time )
{
     int cmd;
     
     if ( isMetClient() ) {
         //- std::cerr << "MetGridData::request_dataSfc: (client) sending VECTOR request" << std::endl;
         // send "need data" status to central met reader process
         cmd = PGR_CMD_M2DV;
         // send request for data
         my_pgroup->send_ints( my_metproc, 1, &cmd, PGR_TAG_REQ );
         // send the desired vector component quantities to the server
         my_pgroup->send_string( my_metproc, xquantity, PGR_TAG_QUANT );
         my_pgroup->send_string( my_metproc, yquantity, PGR_TAG_QUANT );
         // send the desired timestamp to the server
         my_pgroup->send_string( my_metproc, time, PGR_TAG_TIME );
         //std::cerr << "MetGridData::request_dataSfc: (client) sent request" << std::endl;
     }

}


void MetGridData::setCacheDir( std::string path )
{
    if ( diskcachedir != NULLPTR ) {
       delete diskcachedir;
    }
    
    if ( path != "" ) {
       diskcachedir = new FilePath(path);
       diskcaching = true;
    } else {
       diskcachedir = NULLPTR;
       diskcaching = false;
    }   
}

void MetGridData::setCacheDir( FilePath *path )
{
    if ( diskcachedir != NULLPTR ) {
       delete diskcachedir;
    }
    
    if ( path != NULLPTR ) {
       diskcachedir = new FilePath();
       *diskcachedir = *(path);
       diskcaching = true;
    } else {
       diskcachedir = NULLPTR;
       diskcaching = false;
    }   
}

void MetGridData::report() const
{
    std::map< std::string, MetCache3D* >::const_iterator i;
    std::map< std::string, MetCacheSfc* >::const_iterator j;
  
    std::cerr << "++++++ MetGridData report::" << std::endl;
    std::cerr << "*** U-wind cache:" << std::endl;
    us->report();
    std::cerr << "*** V-wind cache:" << std::endl;
    vs->report();
    std::cerr << "*** W-wind cache:" << std::endl;
    ws->report();
    std::cerr << "*** Other 3D:" << std::endl;
    for ( i = field3Ds.begin(); i != field3Ds.end(); i++ ) {   
       std::cerr << "   ... "  <<  (*i).first << " cache:" << std::endl;
       (*i).second->report();
    }    
    std::cerr << "*** Other Sfc:" << std::endl;
    for ( j = field2Ds.begin(); j != field2Ds.end(); j++ ) {   
       std::cerr << "   ... "  <<  (*j).first << " cache:" << std::endl;
       (*j).second->report();
    }    
    std::cerr << "++++++ end MetGridData report::" << std::endl;

}


void MetGridData::serveMet()
{
    int done_goal;
    int done_count;
    GridField3D*  grid3D;
    GridField3D*  grid3D2;
    GridFieldSfc* gridSfc;
    GridFieldSfc* gridSfc2;
    int client_cmd;
    int src;
    int status;
    std::string quantity;
    std::string quantity2;
    std::string time;
    
    //std::cerr << "in serveMet" << std::endl;
    if ( isMetServer() ) {

       // This is the central met processor
       // listen to and satisfy data requests from other processors
       
       // how many processors do we need to tell us we are done?
       // (all except this one)
       done_goal = my_pgroup->size() - 1;
       done_count = 0;

       //- std::cerr << "MetGridData::serveMet: I am a met server. done_count is " << done_count << " of " << done_goal << std::endl;      
       while ( done_count < done_goal ) {
             //- std::cerr << "MetGridData::serveMet: STARTING loop with done_count " << done_count << " of " << done_goal << std::endl;      
          // receive a signal from any processor in this group
          my_pgroup->receive_ints( -1, 1, &client_cmd, PGR_TAG_REQ, &src );
          //- std::cerr << "serveMet: " << " got cmd " << client_cmd << " from proc " << src  << std::endl; 
          switch (client_cmd) {
          case PGR_CMD_DONE: // that client processor is finished making requests
             //- std::cerr << "MetGridData::serveMet: got @@@@ PGR_CMD_DONE " << std::endl;      
             done_count++;
             //- std::cerr << "svr_listen [" << my_pgroup->id() << "]" << " proc " << src << " is done" << std::endl; 
             break;
          case PGR_CMD_M3M: // that client processor is making a 3D metadata request
             //- std::cerr << "MetGridData::serveMet: got @@@@ PGR_CMD_M3M " << std::endl;      
             // get the desired quantity from the client
             my_pgroup->receive_string( src, &quantity, PGR_TAG_QUANT );
             // get the desired timestamp from the client
             my_pgroup->receive_string( src, &time, PGR_TAG_TIME );

             // fetch the desired data
             grid3D = new_mgmtGrid3D( quantity, time );
             if ( grid3D != NULLPTR ) {
          
                // ok, the data grid has been obtained
                send_svr_status( PGR_STATUS_OK, src );
                
                //- std::cerr << "svr_listen [" << my_pgroup->id() << "]" << " about to send metadata to " << src  << std::endl; 
                // and send the metadata to the client
                grid3D->svr_listen(src);
                //grid3D->svr_send_meta(src);
                //- std::cerr << "svr_listen [" << my_pgroup->id() << "]" << " sent metadata to " << src  << std::endl; 
             
               remove(grid3D);
             } else {
                // "Sorry, I was not able to obtain the data grid requested"
                send_svr_status( PGR_STATUS_FAILED, src );
                //- std::cerr << "svr_listen [" << my_pgroup->id() << "]" << " metadata failed for " << src  << std::endl; 
             }
             break;
          case PGR_CMD_M3D: // that client processor is making a 3D data request
             //- std::cerr << "MetGridData::serveMet: got @@@@ PGR_CMD_M3D " << std::endl;      
             //- std::cerr << "serveMet: handshaking with " << src << " begins " << std::endl;             
             // get the desired quantity from the client
             my_pgroup->receive_string( src, &quantity, PGR_TAG_QUANT );
             // get the desired timestamp from the client
             my_pgroup->receive_string( src, &time, PGR_TAG_TIME );
             
             //- std::cerr << "serveMet: handshaking with " << src << " done " << std::endl;             
             // fetch the desired data
             grid3D = new_mgmtGrid3D( quantity, time );
             if ( grid3D != NULLPTR ) {
          
                // ok, the data grid has been obtained
                send_svr_status( PGR_STATUS_OK, src );

                // and send the data values to the client
                grid3D->svr_listen(src);
                //- std::cerr << "xxxxxxxxxxxxxxxx serveMet: data sent to " << src << std::endl;             
                //- std::cerr << "svr_listen [" << my_pgroup->id() << "]" << " sent values to " << src  << std::endl; 
             
                //- std::cerr << "serveMet: 3D field of " << quantity << "( or " << grid3D->quantity() << ") gotten  for " << src << std::endl;             
               remove(grid3D);
             } else {
                // "Sorry, I was not able to obtain the data grid requested"
                send_svr_status( PGR_STATUS_FAILED, src );
                //- std::cerr << "serveMet: 3D field of " << quantity << "( or " << grid3D->quantity() << ") failed  for " << src << std::endl;             
             }
          
             break;
          case PGR_CMD_M3DV: // that client processor is making a 3D data request for vector data
             //- std::cerr << "MetGridData::serveMet: got @@@@ PGR_CMD_M3DV " << std::endl;      
             //- std::cerr << "serveMet: handshaking with " << src << " begins " << std::endl;             
             // get the desired quantity from the client
             my_pgroup->receive_string( src, &quantity, PGR_TAG_QUANT );
             my_pgroup->receive_string( src, &quantity2, PGR_TAG_QUANT );
             // get the desired timestamp from the client
             my_pgroup->receive_string( src, &time, PGR_TAG_TIME );
             
             //- std::cerr << "serveMet: handshaking with " << src << " done " << std::endl;             
             // fetch the desired data
             grid3D = new_mgmtGrid3D( quantity, time );
             grid3D2 = new_mgmtGrid3D( quantity2, time );
             if ( grid3D != NULLPTR && grid3D2 != NULLPTR ) {
          
                // ok, the data grid has been obtained
                send_svr_status( PGR_STATUS_OK, src );

                grid3D->svr_listen(src);
                grid3D2->svr_listen(src);
                //- std::cerr << "xxxxxxxxxxxxxxxx serveMet: vector data sent to " << src << std::endl;             
                //- std::cerr << "svr_listen [" << my_pgroup->id() << "]" << " sent values to " << src  << std::endl; 

                //- std::cerr << "serveMet: 3D vector field of " << quantity << "( or " << grid3D->quantity() << ") ";           
                //- std::cerr << "serveMet:      and  " << quantity2 << "( or " << grid3D2->quantity() << ") gotten  for " << src << std::endl;             
             } else {
                // "Sorry, I was not able to obtain the data grid requested"
                send_svr_status( PGR_STATUS_FAILED, src );
                //- std::cerr << "serveMet: 3D field of " << quantity << "( or " << grid3D->quantity() << ") failed  for " << src << std::endl;             
             }
             
             if ( grid3D2 != NULLPTR ) {
                remove(grid3D2);
             }
             if ( grid3D != NULLPTR ) {
                remove(grid3D);
             }
          
             break;
          case PGR_CMD_M2M: // that client processor is making a 2D metadata request
             //- std::cerr << "MetGridData::serveMet: got @@@@ PGR_CMD_M2M " << std::endl;      
             // get the desired quantity from the client
             my_pgroup->receive_string( src, &quantity, PGR_TAG_QUANT );
             // get the desired timestamp from the client
             my_pgroup->receive_string( src, &time, PGR_TAG_TIME );
             
             // fetch the desired data
             gridSfc = new_mgmtGridSfc( quantity, time );
             if ( gridSfc != NULLPTR ) {
          
                // ok, the data grid has been obtained
                send_svr_status( PGR_STATUS_OK, src );
                
                //- std::cerr << "svr_listen [" << my_pgroup->id() << "]" << " about to send metadata to " << src  << std::endl; 
                // and send the metadata to the client
                gridSfc->svr_listen(src);
                //- std::cerr << "svr_listen [" << my_pgroup->id() << "]" << " sent metadata to " << src  << std::endl; 
             
                remove(gridSfc);
             } else {
                // "Sorry, I was not able to obtain the data grid requested"
                send_svr_status( PGR_STATUS_FAILED, src );
             }
          
             break;
          case PGR_CMD_M2D: // that client processor is making a 2D data request
             //- std::cerr << "MetGridData::serveMet: @@@@ got PGR_CMD_M2D " << std::endl;      
             // get the desired quantity from the client
             my_pgroup->receive_string( src, &quantity, PGR_TAG_QUANT );
             // get the desired timestamp from the client
             my_pgroup->receive_string( src, &time, PGR_TAG_TIME );
             
             // fetch the desired data
             gridSfc = new_mgmtGridSfc( quantity, time );
             if ( gridSfc != NULLPTR ) {
          
                // ok, the data grid has been obtained
                send_svr_status( PGR_STATUS_OK, src );
                
                //- std::cerr << "svr_listen [" << my_pgroup->id() << "]" << " about to send metadata to " << src  << std::endl; 
                // and send the metadata to the client
                gridSfc->svr_listen(src);
                //- std::cerr << "svr_listen [" << my_pgroup->id() << "]" << " sent metadata to " << src  << std::endl; 
             
                remove(gridSfc);
             
             } else {
                // "Sorry, I was not able to obtain the data grid requested"
                send_svr_status( PGR_STATUS_FAILED, src );
             }
          
             break;
          case PGR_CMD_M2DV: // that client processor is making a Sfc data request for vector data
             //- std::cerr << "MetGridData::serveMet: got @@@@ PGR_CMD_M2DV " << std::endl;      
             //- std::cerr << "serveMet: handshaking with " << src << " begins " << std::endl;             
             // get the desired quantity from the client
             my_pgroup->receive_string( src, &quantity, PGR_TAG_QUANT );
             my_pgroup->receive_string( src, &quantity2, PGR_TAG_QUANT );
             // get the desired timestamp from the client
             my_pgroup->receive_string( src, &time, PGR_TAG_TIME );
             
             //- std::cerr << "serveMet: handshaking with " << src << " done " << std::endl;             
             // fetch the desired data
             gridSfc = new_mgmtGridSfc( quantity, time );
             gridSfc2 = new_mgmtGridSfc( quantity2, time );
             if ( gridSfc != NULLPTR && gridSfc2 != NULLPTR ) {
          
                // ok, the data grid has been obtained
                send_svr_status( PGR_STATUS_OK, src );

                //- std::cerr << "serveMet: Sfc vector field of " << quantity << "( or " << grid3D->quantity() << ") ";           
                //- std::cerr << "serveMet:      and  " << quantity2 << "( or " << grid3D2->quantity() << ") gotten  for " << src << std::endl;             
          
                // and send the data values to the client
                gridSfc->svr_listen(src);
                gridSfc2->svr_listen(src);
                //- std::cerr << "xxxxxxxxxxxxxxxx serveMet: vector data sent to " << src << std::endl;             
                //- std::cerr << "svr_listen [" << my_pgroup->id() << "]" << " sent values to " << src  << std::endl; 
             } else {
                // "Sorry, I was not able to obtain the data grid requested"
                send_svr_status( PGR_STATUS_FAILED, src );
                //- std::cerr << "serveMet: 3D field of " << quantity << "( or " << grid3D->quantity() << ") failed  for " << src << std::endl;             
             }
             
             if ( gridSfc2 != NULLPTR ) {
                remove(gridSfc2);
             }
             if ( gridSfc != NULLPTR ) {
                remove(gridSfc);
             }

             break;
          }
          //- std::cerr << "MetGridData::serveMet: ENDING loop with done_count " << done_count << " of " << done_goal << std::endl;      

       } 
    }  
    
    //- std::cerr << "met server exit-syncing with the group" << std::endl;
    //my_pgroup->sync("Met server exit");
    my_pgroup->sync();
    //- std::cerr << "leaving serveMet" << std::endl;
    
    return; 

}



//////////////////////////  MetCache3D


// constructor
MetGridData::MetCache3D::MetCache3D( std::string quantity, int size )
{
    quant = quantity;
    max = 3;
    if (size > 0 ) {
       max = size;
    }   

}

// destructor
MetGridData::MetCache3D::~MetCache3D()
{
     std::deque<GridField3D*>::iterator i;

     // get rid of all of the GridField3D objects we are holding
     for ( i=data.begin(); i != data.end(); i++ ) {
        delete *i;
     }   

}

// copy constructor
MetGridData::MetCache3D::MetCache3D( const MetCache3D& src )
{
     std::deque<GridField3D*>::const_iterator i;

     quant = src.quant;
     max = src.max;
     
     // copy the held data 
     for ( i=src.data.begin(); i != src.data.end(); i++ ) {
        data.push_back( *i );
     }   
    

}

void MetGridData::MetCache3D::setSize(int n) 
{
   if ( n > 0 ) {
      max = n;
   }
}


GridField3D* MetGridData::MetCache3D::query( double time, int flag ) 
{
     std::deque<GridField3D*>::iterator i;
     int gotit = 0;
     GridField3D *result;

     // Start by assuming we won't find anything
     result = NULLPTR;

     // look to see if we already have this snapshot
     for ( i=data.begin(); (i != data.end()) && (gotit == 0) ; i++ ) {
        result = *i;
        if ( result->time() == time ) {
           gotit = 1;
        }
     }
     
     if ( gotit == 1 ) {
        // found it.  

        // move this snapshot to the front?
        // only if it is not already there
        if ( flag && ( *i != data.front() ) ) {
        
           data.erase(i);
           data.push_front(result);
        }

     } else {
        result = NULLPTR;
     }
     
     return result;   
}

GridField3D* MetGridData::MetCache3D::query( const std::string time, int flag ) 
{
     std::deque<GridField3D*>::iterator i;
     int gotit = 0;
     GridField3D *result;

     // Start by assuming we won't find anything
     result = NULLPTR;

     // look to see if we already have this snapshot
     if ( data.size() > 0 ) {
        for ( i=data.begin(); (i != data.end()) && (gotit == 0) ; i++ ) {
           result = *i;
           if ( result != NULLPTR ) {
              if ( result->met_time() == time ) {
                 gotit = 1;
                 break;
              }
           }
        }
     }
     

     //- std::cerr << "    queried " <<   time <<  ": " << gotit << std::endl;
     if ( gotit == 1 ) {
        // found it.  
        
        // move this snapshot to the front?
        if ( flag && ( *i != data.front() ) ) {
           //  need to decrement if we do not break out of the loop above
           //i--;
           data.erase(i);
           data.push_front(result);
        }
        
     } else {
        result = NULLPTR;
     }
     
     
     return result;   
}

bool MetGridData::MetCache3D::has( GridField3D* field ) 
{
    bool result;
    std::deque<GridField3D*>::const_iterator i;
    GridField3D *mine;
  
     result = false;
     
     // does the given pointer correspond to anything that we have?
     for ( i=data.begin(); i != data.end(); i++ ) {
         mine = *i;
         if ( field == mine ) {
            result = true;
            break;
         }   
     }   
    
     return result;
}

void MetGridData::MetCache3D::add( GridField3D* field )
{
   int i;
   
   // do we have too many snapshots to hold another?
   while ( data.size() >= max ) {
      // yes. Drop the one with the least priority

      // std::cerr << "  cache dropping " << grid->quantity() << " @ " << grid->met_time() << std::endl;
      
      delete data.back();
      data.pop_back(); 
   }
   
   // add this snapshot
   //- std::cerr << "  cache adding " << field->quantity() << " @ " << field->met_time() << ": " << field << std::endl;
   data.push_front(field);

   //- std::cerr << "report: " << std::endl;
   //- report();

}

void MetGridData::MetCache3D::report() const
{
     std::deque<GridField3D*>::const_iterator i;
     GridField3D *grid;
     int cnt = 0;
     
     std::cerr << " 3D Cache report:" << std::endl;
     std::cerr << "    quantity=" << quant << std::endl;
     std::cerr << "    state: " << data.size() << " elements of " << max << " max " << std::endl;
     for ( i=data.begin(); i != data.end(); i++ ) {
         grid = *i;
         std::cerr << "       " << cnt << ": " << grid->quantity() << " at " << grid->met_time() << ": " << grid << std::endl;
         cnt++;
     }

}


//////////////////////////  MetCacheSfc


// constructor
MetGridData::MetCacheSfc::MetCacheSfc( std::string quantity, int size )
{
    quant = quantity;
    max = 3;
    if (size > 0 ) {
       max = size;
    }   

}

// destructor
MetGridData::MetCacheSfc::~MetCacheSfc()
{
     std::deque<GridFieldSfc*>::iterator i;

     // get rid of all of the GridFieldSfcobjects we are holding
     for ( i=data.begin(); i != data.end(); i++ ) {
        delete *i;
     }   

}

// copy constructor
MetGridData::MetCacheSfc::MetCacheSfc( const MetCacheSfc& src )
{
     std::deque<GridFieldSfc*>::const_iterator i;

     quant = src.quant;
     max = src.max;
     
     // copy the held data 
     for ( i=src.data.begin(); i != src.data.end(); i++ ) {
        data.push_back( *i );
     }   
    

}

void MetGridData::MetCacheSfc::setSize(int n) 
{
   if ( n > 0 ) {
      max = n;
   }
}


GridFieldSfc* MetGridData::MetCacheSfc::query( double time, int flag ) 
{
     std::deque<GridFieldSfc*>::iterator i;
     int gotit = 0;
     GridFieldSfc *result;
     
     // Start by assuming we won't find anything
     result = NULLPTR;

     // look to see if we already have this snapshot
     for ( i=data.begin(); (i != data.end()) && (gotit == 0) ; i++ ) {
        result = *i;
        if ( result->time() == time ) {
           gotit = 1;
        }
     }
     
     if ( gotit == 1 ) {
        // found it.  

        //- std::cerr << "found in cache: " << result->quantity() << " for " << time << std::endl;
        // move this snapshot to the front?
        if ( flag && (*i != data.front() ) ) {
        
           data.erase(i);
           data.push_front(result);
        }

     } else {
        // not found
        result = NULLPTR;
        
     }
     
     return result;   
}

GridFieldSfc* MetGridData::MetCacheSfc::query( const std::string time, int flag ) 
{
     std::deque<GridFieldSfc*>::iterator i;
     int gotit = 0;
     GridFieldSfc *result;

     // Start by assuming we won't find anything
     result = NULLPTR;

     // look to see if we already have this snapshot
     if ( data.size() > 0 ) {
        for ( i=data.begin(); (i != data.end()) && (gotit == 0) ; i++ ) {
           result = *i;
           if ( result != NULLPTR ) {
              if ( result->met_time() == time ) {
                 gotit = 1;
                 break;
              }
           }
        }
     }
     

     //- std::cerr << "    queried " <<   time <<  ": " << gotit << std::endl;
     if ( gotit == 1 ) {
        // found it.  
        
        // move this snapshot to the front?
        if ( flag && (*i != data.front() ) ) {
           //  need to decrement if we do not break out of the loop above
           //i--;
           data.erase(i);
           data.push_front(result);
        }

     } else {
        // not found
        result = NULLPTR;

     }
     
     return result;   
}

bool MetGridData::MetCacheSfc::has( GridFieldSfc* field ) 
{
    bool result;
    std::deque<GridFieldSfc*>::const_iterator i;
    GridFieldSfc *mine;
  
     result = false;
     
     // does the given pointer correspond to anything that we have?
     for ( i=data.begin(); i != data.end(); i++ ) {
         mine = *i;
         if ( field == mine ) {
            result = true;
            break;
         }   
     }   
    
     return result;
}

void MetGridData::MetCacheSfc::add( GridFieldSfc* field )
{
   int i;
   
   // do we have too many snapshots to hold another?
   while ( data.size() >= max ) {
      // yes. Drop the one with the least priority
      
      // std::cerr << "  cache dropping " << grid->quantity() << " @ " << grid->met_time() << std::endl;
      
      delete data.back();
      data.pop_back(); 
   }
   
   // add this snapshot
   // std::cerr << "  cache adding " << field->quantity() << " @ " << field->met_time() << std::endl;
   data.push_front(field);

   //- std::cerr << "report: " << std::endl;
   //- report();

}

void MetGridData::MetCacheSfc::report() const
{
     std::deque<GridFieldSfc*>::const_iterator i;
     GridFieldSfc *grid;
     int cnt = 0;
     
     std::cerr << " Sfc Cache report:" << std::endl;
     std::cerr << "    quantity=" << quant << std::endl;
     std::cerr << "    state: " << data.size() << " elements of " << max << " max " << std::endl;
     for ( i=data.begin(); i != data.end(); i++ ) {
         grid = *i;
         std::cerr << "       " << cnt << ": " << grid->quantity() << " on " << grid->surface() 
                   << " at " << grid->met_time() << std::endl;
         cnt++;
     }

}


