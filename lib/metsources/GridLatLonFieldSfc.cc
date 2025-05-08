
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

#include <math.h>
#include <stdlib.h>

#include "gigatraj/GridLatLonFieldSfc.hh"

using namespace gigatraj;


const std::string GridLatLonFieldSfc::iam = "GridLatLonFieldSfc";


// Default constructor
GridLatLonFieldSfc::GridLatLonFieldSfc() : GridFieldSfc()
{   
   lons.wrapping( 360.0 );
   
   use_array = 1;
   nd = 0;
   dater = NULLPTR;
   
}

// Default destructor
GridLatLonFieldSfc::~GridLatLonFieldSfc()
{
};

// copy constructor
GridLatLonFieldSfc::GridLatLonFieldSfc(const GridLatLonFieldSfc& src) : GridFieldSfc(src)
{

        lons = src.lons;
        lats = src.lats;

        lons.setPgroup( pgroup, metproc );
        lats.setPgroup( pgroup, metproc );

}


// copy assignment
GridLatLonFieldSfc& GridLatLonFieldSfc::operator=(const GridLatLonFieldSfc& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }

    // copy the parent-class elements
    GridLatLonFieldSfc::assign(src);
    
    // copy the elements unique to this class
    lons = src.lons;
    lats = src.lats;
    lons.setPgroup( pgroup, metproc );
    lats.setPgroup( pgroup, metproc );

    return *this;
}      


void GridLatLonFieldSfc::clear() {
   
   lons.clear();
   lats.clear();
   
   GridFieldSfc::clear();
   
}

int GridLatLonFieldSfc::status() const
{
    int result = 0;
    
    result = GridFieldSfc::status();
    if ( lons.size() <= 0 || lats.size() <= 0 ) {
       result = result | GFS_NODIMS;
    }
    if ( nd != lons.size()*lats.size() ) {
       result = result | GFS_GRIDERR;
    }
    
    return result;

}

void GridLatLonFieldSfc::setPgroup( ProcessGrp* pg, int met)
{
       GridField::setPgroup( pg, met );
       lons.setPgroup( pg, met );
       lats.setPgroup( pg, met );

}

void GridLatLonFieldSfc::dims( int* nlon, int* nlat)  const
{
   *nlon = lons.size();
   *nlat = lats.size();
};


real GridLatLonFieldSfc::value( int i, int j )  const
{
    int indx;

    if ( ! hasdata() ) {
        throw (baddatareq());
    }    
    
    i = lons.iwrap(i);
    
    // ensure that the latitude index is legal
    if ( j < 0 || j >= lats.size() ) {
        throw (baddatareq());
    }    
    
    // turn the two indices into a single index into the data array
    indx = j*lons.size() + i;
    
    return dater[indx];
};


real& GridLatLonFieldSfc::valueref( int i, int j ) 
{
   real* result;
   int indx;

   if ( ! hasdata() ) {
       throw (baddatareq());
   }    
     
   // ensure that the longitude index is legal
   i = lons.iwrap( i );
   // ensure that the latitude index is legal
   if ( j < 0 || j >= lats.size() ) {
       throw (baddatareq());
   }    

   // turn the two indices into a single index into the data array
   indx = j*lons.size() + i;
     
   result = &(dater[indx]);
   return *result;
}        

real GridLatLonFieldSfc::value( int indx ) const
{
     real result;
   
     if ( ! hasdata() ) {
         std::cerr << "GridLatLonFieldSfc: has no data in value() call" << std::endl;
         throw (baddatareq());
     }    
     
     result = dater[indx];

     return result;

}

real* GridLatLonFieldSfc::values( int n, real* vals, int *indices ) const
{
     int indx;
     int ref;
     int i,j;

     if ( ! hasdata() ) {
         std::cerr << "GridLatLonFieldSfc: has no data in values() call" << std::endl;
         throw (baddatareq());
     }    
     
     if ( n > 0 ) {
     
        if ( vals == NULLPTR ) {
           vals = new real[n];
        }
   
        for ( indx=0; indx < n; indx++ ) {
            vals[indx] = dater[indices[indx]];
	    }
     }
        
     return vals;
};


std::vector<real> GridLatLonFieldSfc::longitudes()  const
{ 
   return lons.dimension();
};

std::vector<real> GridLatLonFieldSfc::latitudes()  const
{ 
   return lats.dimension();
};


real GridLatLonFieldSfc::longitude( int i )  const
{
   i = lons.iwrap(i);

   return lons(i);
};
   
real GridLatLonFieldSfc::latitude( const int j ) const
{
   if ( j < 0 || j >= lats.size() ) {
       throw (baddatareq());
   }    
   return lats(j);
};   
 

real GridLatLonFieldSfc::wrap( real lon ) const
{

     return lons.wrap( lon );

};

int GridLatLonFieldSfc::iwrap( int i ) const
{
     return lons.iwrap( i );
};

void GridLatLonFieldSfc::lonindex( real lon, int* i1, int* i2 ) const
{

     lons.index( lon, i1, i2 );

};

void GridLatLonFieldSfc::latindex( real lat, int* i1, int* i2 ) const
{

     lats.index( lat, i1, i2 );

};




void GridLatLonFieldSfc::setWraps( const int loadFlags )
{

   lons.setWraps( loadFlags );

}





void GridLatLonFieldSfc::load( const realvec& inlons, const realvec& inlats, const realvec& indata, const int loadFlags )
{
   int nlons, nlats;
   
   lons.load( inlons, loadFlags );
   lats.load( inlats, loadFlags );
   
   nlons = lons.size();
   nlats = lats.size();
   
   clearData();
   nd = nlons*nlats;
   dater = new real[nd];   

   // copy the data   
   for (int indx=0; indx < nd; indx++ ) {
       dater[indx] = indata[indx];
   }
   clear_nodata();
   
}

void GridLatLonFieldSfc::load( const realvec& inlons, const realvec& inlats, const int loadFlags )
{
   
   lons.load( inlons, loadFlags );
   lats.load( inlats , loadFlags);
   
   // Don't try to copy any data 
   clearData();  

   if ( loadFlags & GFL_PREFILL ) {
      nd = lons.size()*lats.size();
      dater = new real[nd];
      for ( int i=0; i<nd; i++ ) {
         dater[i] = fill_value;
      }
      clear_nodata();   
   }
   
}

void GridLatLonFieldSfc::load( const realvec& indata, const int loadFlags )
{
   int indx = 0;
   int i;
   int nlons, nlats;
   
   nlons = lons.size();
   nlats = lats.size();
   
   if ( nlons*nlats != indata.size() ) {
      throw(badincompatcoords());
   }      
   
   clearData();
   nd = nlons*nlats;
   dater = new real[nd];
   for ( int i=0; i<nd; i++ ) {
      dater[i] = indata[i];
   }
   
   clear_nodata();

   /// note: since the longitudes and latitudes are unchanged,

}



bool GridLatLonFieldSfc::compatible( const GridFieldSfc& obj, int compatFlags ) const  
{
    int result = true;
    std::vector<real> dim;
    const GridLatLonFieldSfc *trueobj;
    
    if ( obj.id() == id() ) {
    
       result = GridFieldSfc::compatible( obj, compatFlags );
       
       trueobj = dynamic_cast<const GridLatLonFieldSfc*>(&obj);
    
       if ( compatFlags & METCOMPAT_HORIZ ) {
          // check that the longitudes match in number and values
          if (  ! lons.compatible( trueobj->lons ) ) {
             result = false;
          }   
          if (  ! lats.compatible( trueobj->lats ) ) {
             result = false;
          }   
       }

    } else {
       // different grid types are by definition incompatible
       result = false;
    }   
    
    return result;      
       
}

bool GridLatLonFieldSfc::match( const GridLatLonFieldSfc& obj ) const  
{
    int result = true;
    std::vector<real> dim;
    
    if ( ! this->compatible(obj) ) {
       result = false;
    }
    
    if ( obj.quantity() != quant 
    ||   obj.units() != uu  
    ||   obj.fillval() != fill_value ) {
       result = false;
    }   

    return result;      
       
}

int GridLatLonFieldSfc::doeswrap() const
{
   int result = 0;
   if ( lons.wrapping() ) {
      result = 1;
   }   
   return result;
}   

real GridLatLonFieldSfc::area(int i, int j) const
{
    real dlon, dlat;
    real lat1, lat2, lon1, lon2;
    real ar;
    int nlons, nlats;
    
    nlons = lons.size();
    nlats = lats.size();

    if ( j > 0 ) {
       lat1 = (lats(j-1) + lats(j))/2.0;
    } else {
       lat1 = lats(j);
    }      
    if ( j < (nlats-1) ) {
       lat2 = (lats(j) + lats(j+1))/2.0;
    } else {
       lat2 = lats(j);
    }
    dlat = ABS( SIN(lat2/180.0*PI) - SIN(lat1/180.0*PI) );

    if ( i > 0 ) {
       lon1 = ( lons(i-1) + lons(i) )/2.0;
    } else {
       // i == 0 
       if ( lons.wrapping() ) {
          if ( lons(nlons-1) > lons(0) ) {
             lon1 = (lons(0) +  (lons(nlons-1) - 360.0) )/2.0;
          } else {
             lon1 = (lons(0) +  (lons(nlons-1) + 360.0) )/2.0;
          }
       } else {
          lon1 = lons(0);
       }
    }   
    if ( i < (nlons-1) ) {
       lon2 = ( lons(i) + lons(i+1) )/2.0;
    } else {
       if ( lons.wrapping() ) {
          if ( lons(0) < lons(nlons-2) ) {
             lon2 = ((lons(0) + 360.0) + lons(nlons-1))/2.0;
          } else {
             lon2 = ((lons(0) - 360.0) + lons(nlons-1))/2.0;
          }       
       } else {
          lon2 = lons(i);
       }
    }
    
    dlon = ABS( lon2 - lon1 );
    while ( dlon > 360.0 ) {
       dlon = dlon - 360.0;
    }   

    ar = dlon/180.0*PI * dlat;


    return ar;
}

real GridLatLonFieldSfc::area(int i) const
{
    int ii,jj;
    
    ii = i % lons.size();
    jj = i / lons.size();
    
    return area(ii,jj);
    
}

GridFieldSfc* GridLatLonFieldSfc::areas() const
{
    GridLatLonFieldSfc* result;
    real* newdata;
    real ar;
    real dlon, dlat;
    real lat1, lat2, lon1, lon2;
    int maxlon;
    int nlons, nlats;
    real* newlats;
    real* newlons;
    
    nlons = lons.size();
    nlats = lats.size();
    
    result = dynamic_cast<GridLatLonFieldSfc*>(this->duplicate());

    result->set_quantity( "normalized_area" );
    result->set_units( "" );
    result->set_fillval( -999.0 );
    result->set_surface( "" );
    
    newdata = new real[nlons*nlats];
    newlons = new real[nlons];
    newlats = new real[nlats]; 
    for ( int j=0; j<nlats; j++ ) {
        newlats[j] = lats(j);
        if ( j > 0 ) {
           lat1 = (lats(j-1) + lats(j))/2.0;
        } else {
           lat1 = lats(j);
        }      
        if ( j < (nlats-1) ) {
           lat2 = (lats(j) + lats(j+1))/2.0;
        } else {
           lat2 = lats(j);
        }
        dlat = ABS( SIN(lat2/180.0*PI) - SIN(lat1/180.0*PI) );
        for ( int i=0; i<nlons; i++ ) {
           
           if ( i > 0 ) {
              lon1 = ( lons(i-1) + lons(i) )/2.0;
           } else {
              // i == 0 
              if ( lons.wrapping() ) {
                 if ( lons(nlons-1) > lons(0) ) {
                    lon1 = (lons(0) +  (lons(nlons-1) - 360.0) )/2.0;
                 } else {
                    lon1 = (lons(0) +  (lons(nlons-1) + 360.0) )/2.0;
                 }
              } else {
                 lon1 = lons(0);
              }
           }   
           if ( i < (nlons-1) ) {
              lon2 = ( lons(i) + lons(i+1) )/2.0;
           } else {
              if ( lons.wrapping() ) {
                 if ( lons(0) < lons(nlons-2) ) {
                    lon2 = ((lons(0) + 360.0) + lons(nlons-1))/2.0;
                 } else {
                    lon2 = ((lons(0) - 360.0) + lons(nlons-1))/2.0;
                 }       
              } else {
                 lon2 = lons(i);
              }
           }
           
           dlon = ABS( lon2 - lon1 );
           while ( dlon > 360.0 ) {
              dlon = dlon - 360.0;
           }   
           
           ar = dlon/180.0*PI * dlat;
           
           newdata[ j*nlons + i ] = ar;

           if ( j == 0 ) {
              newlons[i] = lons(i);
           }
       }
    }
    result->absorb( nlons, nlats, newdata, newlons, newlats );    

    return result;


}

void GridLatLonFieldSfc::absorb( int nlons, int nlats, real* vals , real* lonvals, real* latvals)
{
      if ( lonvals != NULLPTR ) {
         lons.absorb( nlons, lonvals );
      }
      if ( latvals != NULLPTR ) {
         lats.absorb( nlats, latvals );
      }
      
      clearData();
      nd = nlons*nlats;
      dater = vals;
      clear_nodata();
      
}

void GridLatLonFieldSfc::absorbLons( int n, real* lonvals )
{
      if ( lonvals != NULLPTR ) {
         lons.absorb( n, lonvals );
      }

}

void GridLatLonFieldSfc::absorbLats( int n, real* latvals )
{
      if ( latvals != NULLPTR ) {
         lats.absorb( n, latvals );
      }
}


GridFieldDim GridLatLonFieldSfc::getLatitudes() const
{
     return lats;
} 

GridFieldDimLon GridLatLonFieldSfc::getLongitudes() const
{
     return lons;
} 

GridFieldSfc* GridLatLonFieldSfc::duplicate() const
{
    GridLatLonFieldSfc* newgrid;
   
    newgrid = new GridLatLonFieldSfc();
    
    *newgrid = *this;

    return newgrid;

}

int GridLatLonFieldSfc::dataSize() const
{
    if ( lons.size() == 0 || lats.size() == 0 ) {
       throw (badnodims());
    }
    
    return lons.size()*lats.size();   

}

void GridLatLonFieldSfc::gridcoords( int n, int* is, int* js, real* olons, real* olats, int flags ) const
{

     lons.values( n, olons, is );
     lats.values( n, olats, js );

}



void GridLatLonFieldSfc::gridpoints( int n, int* is, int* js, real* vals, int flags) const
{
     int* coords;
     int cmd;
     int local;
     int done;
     
     local = flags & 0x01;
     done = flags * 0x02;

     // get the data from a central met processors    
     try {
        coords = new int[n];
     } catch(...) {
        throw (badmemreq());
     }
     
     joinIndex( n, coords, is, js );

     if ( pgroup == NULLPTR || metproc < 0 || local != 0 ) {
         // serial processing.  Access the data locally
         this->values( n, vals, coords );
         //for ( int i=0; i<n; i++ ) {
         //    vals[i] = this->value(is[i],js[i]);
         //}
     } else {
     
         // Never ask a dedicated met processor to fetch gridpoint data for itself
         if ( metproc == pgroup->id() ) {
            throw (badProcReq());
         }
     
         // send data request to central met reader process
         //cmd = PGR_CMD_GDATA;
         //pgroup->send_ints( metproc, 1, &cmd, PGR_TAG_GREQ );
         // send request for n points 
         pgroup->send_ints( metproc, 1, &n, PGR_TAG_GNUM );
         // send the coordinates
         pgroup->send_ints( metproc, n, coords, PGR_TAG_GCOORDS );
         // receive the values
         pgroup->receive_reals( metproc, n, vals, PGR_TAG_GVALS );

         if ( done ) {
            svr_done();
         }
     
     }

     delete[] coords;
}


void GridLatLonFieldSfc::gridpoints( int n, int* indices, real* vals, int flags) const
{
     int cmd;
     int local;
     int done;
     
     local = flags & 0x01;
     done = flags * 0x02;


     if ( metproc < 0 || local != 0 ) {
         // serial processing.  Access the data locally
         //for ( int i=0; i<n; i++ ) {
         //    vals[i] = this->value(is[i],js[i]);
         //}
         this->values( n, vals, indices );
     } else {
     
         // Never ask a dedicated met processor to fetch gridpoint data for itself
         if ( metproc == pgroup->id() ) {
            throw (badProcReq());
         }
     
         // send data request to central met reader process
         //cmd = PGR_CMD_GDATA;
         //pgroup->send_ints( metproc, 1, &cmd, PGR_TAG_GREQ );
         // send request for n points 
         pgroup->send_ints( metproc, 1, &n, PGR_TAG_GNUM );
         // send the coordinates
         pgroup->send_ints( metproc, n, indices, PGR_TAG_GCOORDS );
         // receive the values
         pgroup->receive_reals( metproc, n, vals, PGR_TAG_GVALS );

         if ( done ) {
            svr_done();
         }
     
     }

}

void GridLatLonFieldSfc::splitIndex( int index, int* i, int* j ) const
{
    *i = index % lons.size();
    *j = index / lons.size();

}

void GridLatLonFieldSfc::splitIndex( int n, int *index, int* i, int* j ) const
{
    int m;
    
    if ( n > 0 ) {
    
       for ( m=0; m < n; m++ ) {
          // now get the indices in that level
          i[m] = index[m] % lons.size();
          j[m] = index[m] / lons.size();
       }
    }

}

int GridLatLonFieldSfc::joinIndex( int i, int j ) const
{
     int result;
     
     // ensure that the longitude index is legal
     i = lons.iwrap(i);
     // ensure that the latitude index is legal
     if ( j < 0 || j >= lats.size() ) {
         std::cerr << "GridLatLonFieldSfc: out-of-range latitude or vertical index "  
         << j << " of " << lats.size() << " in joinIndex() call" << std::endl;
         throw (baddatareq());
     }    

     result = j*lons.size() + i;

     return result;


}

int* GridLatLonFieldSfc::joinIndex( int n, int *index, int* i, int* j ) const
{
     
     if ( n > 0 ) {
     
        if ( index == NULLPTR ) {
           index = new int[n];
        }
     
        for ( int m=0; m<n; m++ ) {           // ensure that the longitude index is legal
            i[m] = lons.iwrap( i[m] );
            // ensure that the latitude index is legal                                                   
            if ( j[m] < 0 || j[m] >= lats.size() ) {                                                                                   
                std::cerr << "GridLatLonField3D:  out-of-range latitude or vertical index "  << j[m]   
                << " of " << lats.size() << " in joinIndex() call" << std::endl;                       
                throw (baddatareq());                                                                                    
            }
                                                                                                                 
            index[m] = j[m]*lons.size() + i[m];
        }
     }
     
     return index;
}


void GridLatLonFieldSfc::receive_meta()
{
     real* dimvals;
     int cmd;
     int i;
     int nlons;
     int nlats;

     if (  pgroup == NULLPTR || metproc < 0 ) {
         // serial processing.  Load nothing, but
         // check that the object has valid metadata
         if ( ! this->hasdata() ) {
            throw (baddataload());
         }
         
     } else {
     
         // Never ask a dedicated met processor to fetch metadata for itself
         if ( metproc == pgroup->id() ) {
            throw (badProcReq());
         }
     
         // receive theta metadata
         //- std::cerr << "   GridLatLonField3D::receive_meta: r-100 with " << metproc << std::endl;
         pgroup->receive_string( metproc, &quant, PGR_TAG_GMETA ); // quantity
         //- std::cerr << "   GridLatLonFieldSfc::receive_meta: r-110 from " << metproc  << std::endl;
         pgroup->receive_string( metproc, &uu, PGR_TAG_GMETA );  // units
         //- std::cerr << "   GridLatLonFieldSfc::receive_meta: r-120 from " << metproc  << std::endl;
         pgroup->receive_string( metproc, &ctime, PGR_TAG_GMETA );  // met time stamp
         //- std::cerr << "   GridLatLonFieldSfc::receive_meta: r-130 from " << metproc  << std::endl;
         pgroup->receive_doubles( metproc, 1, &mtime, PGR_TAG_GMETA );  // met time 
         //- std::cerr << "   GridLatLonFieldSfc::receive_meta: r-140 from " << metproc  << std::endl;
         pgroup->receive_reals( metproc, 1, &fill_value, PGR_TAG_GMETA );  // fill value
         //- std::cerr << "   GridLatLonFieldSfc::receive_meta: r-150 from " << metproc  << std::endl;
         pgroup->receive_string( metproc, &sfc, PGR_TAG_GMETA ); // quantity
         //- std::cerr << "   GridLatLonFieldSfc::receive_meta: r-200 from " << metproc  << std::endl;

         lons.receive_meta();
         lats.receive_meta();

         clearData();
   
         metaID = 0;
   
     }

}

void GridLatLonFieldSfc::svr_send_meta(int id) const
{
     real* dimvals;
     int cmd;
     int i;

     if ( metproc < 0 ) {
         // serial processing.  Send nothing, but
         // check that the object has valid metadata
         if ( this->status() & (~0x10) ) {
            throw (baddataload());
         }
         
     } else {
     
         // Ask only a dedicated met processor to send metadata
         if ( metproc != pgroup->id() ) {
            throw (badProcReq());
         }

         // by the time we get here, the receiving processor
         // has issued its request for metadata, and
         // the calling routine has already received that request
         // and called this method.

         // receive the metadata
         //- std::cerr << "   GridLatLonFieldSfc::svr_send_meta: s-100 with " << id << std::endl;
         pgroup->send_string( id, quant, PGR_TAG_GMETA ); // quantity
         //- std::cerr << "   GridLatLonFieldSfc::svr_send_meta: s-110 to " << id << std::endl;
         pgroup->send_string( id, uu, PGR_TAG_GMETA );  // units
         //- std::cerr << "   GridLatLonFieldSfc::svr_send_meta: s-120 to " << id << std::endl;
         pgroup->send_string( id, ctime, PGR_TAG_GMETA );  // met time stamp
         //- std::cerr << "   GridLatLonFieldSfc::svr_send_meta: s-130 to " << id << std::endl;
         pgroup->send_doubles( id, 1, &mtime, PGR_TAG_GMETA );  // met time 
         //- std::cerr << "   GridLatLonFieldSfc::svr_send_meta: s-140 to " << id << std::endl;
         pgroup->send_reals( id, 1, &fill_value, PGR_TAG_GMETA );  // fill value
         //- std::cerr << "   GridLatLonFieldSfc::svr_send_meta: s-150 to " << id << std::endl;
         pgroup->send_string( id, sfc, PGR_TAG_GMETA ); // surface quantity
         //- std::cerr << "   GridLatLonFieldSfc::svr_send_meta: s-200 to " << id << std::endl;

         lons.svr_send_meta(id);
         lats.svr_send_meta(id);

     }

}

void GridLatLonFieldSfc::serialize(std::ostream& os) const
{
  int prec;
  string str;
  int len;
  int i, j;
  double t;
  real val;
  int ival;
  const char *cstr;
  int version = 2;

  
  try {
  
      GridFieldSfc::serialize(os);
      
      // output the version
      os.write( reinterpret_cast<char *>( &version), static_cast<std::streamsize>( sizeof(int)));

      switch (version) {
      case 1: 
          // output the number of longitudes
          ival = lons.size();
          os.write( reinterpret_cast<char *>( &ival), static_cast<std::streamsize>( sizeof(int)));
          // output the number of latitudes
          ival = lats.size();
          os.write( reinterpret_cast<char *>( &ival), static_cast<std::streamsize>( sizeof(int)));
  
          // output the longitudes
          for ( i=0; i<lons.size(); i++ ) {
              val = lons(i);
              os.write( reinterpret_cast<char *>( &val), static_cast<std::streamsize>( sizeof(real)));
          }    
          // output the latitudes
          for ( i=0; i<lats.size(); i++ ) {
              val = lats(i);
              os.write( reinterpret_cast<char *>( &val), static_cast<std::streamsize>( sizeof(real)));
          }  
          break;
      case 2: 
          lons.serialize(os);
          lats.serialize(os);
          break; 
      }
       
      // output the data
      for ( j=0; j<lats.size(); j++ ) {
      for ( i=0; i<lons.size(); i++ ) {
          val = value(i,j);
          os.write( reinterpret_cast<char *>( &val), static_cast<std::streamsize>( sizeof(real)));
      }
      }
    

  } catch (...) {
      throw;
  }    

}

void GridLatLonFieldSfc::deserialize(std::istream& is)
{
   string str;
   int len;
   char cc;
   int i;
   double t;
   real val;
   int ival;
   int nxlons, nxlats;
   real* xlons;
   real *xlats;
   real *xdata;
   int version;
   
   clear();

   try {
   
       GridFieldSfc::deserialize(is);
      
       // read the version
       is.read(reinterpret_cast<char *>( &version), static_cast<std::streamsize>( sizeof(int)));
       
      switch (version) {
      case 1: 
       
           // read the number of longitudes
           is.read( reinterpret_cast<char *>(&nxlons), static_cast<std::streamsize>( sizeof(int)) );
           // read the number of latitudes
           is.read( reinterpret_cast<char *>(&nxlats), static_cast<std::streamsize>( sizeof(int)) );
       
           // read the longitudes
           xlons = new real[nxlons];
           for ( i=0; i<nxlons; i++ ) {
               is.read( reinterpret_cast<char *>(&val), static_cast<std::streamsize>( sizeof(real)) );
               xlons[i] = val;
           }    
           // read the latitudes
           xlats = new real[nxlats];
           for ( i=0; i<nxlats; i++ ) {
               is.read( reinterpret_cast<char *>(&val), static_cast<std::streamsize>( sizeof(real)) );
               xlats[i] = val;
           }    
           lons.absorb( nxlons, xlons );
           lats.absorb( nxlats, xlats );           
           
           break;
      case 2: 
           lons.deserialize(is);
           lats.deserialize(is);
           
           nxlons = lons.size();
           nxlats = lats.size();
           
           break;
      } 
      
      // read the data
      xdata = new real[nxlats*nxlons];
      for ( i=0; i<nxlats*nxlons; i++ ) {
          is.read( reinterpret_cast<char *>(&val), static_cast<std::streamsize>( sizeof(real) ));
          xdata[i] = val;
      }    
      absorb( nxlons, nxlats, xdata );          

   } catch (...) {
       throw;
   }    

}

void GridLatLonFieldSfc::getindices( int index, int* i, int*j ) const
{
    *i = index % lons.size();
    *j = index / lons.size();

}


//.......................................................................................



namespace gigatraj {

// outputs the Met Field 
std::ostream& operator<<( std::ostream& os, const GridLatLonFieldSfc& p)
{
  
  try {
  
      p.serialize(os);
  

  } catch (...) {
      throw;
  }    
  
  return os;

};

// inputs the Met field
std::istream& operator>>( std::istream& is, GridLatLonFieldSfc& p)
{

   try {
   
       p.deserialize(is);

   } catch (...) {
       throw;
   }    
   
   return is;

};

}
