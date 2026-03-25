
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

#include "gigatraj/GridLatLonField3D.hh"

using namespace gigatraj;


const std::string GridLatLonField3D::iam = "GridLatLonField3D";


// Default constructor
GridLatLonField3D::GridLatLonField3D(): GridField3D() 
{
   lons.wrapping( 1 );
}

// Default destructor
GridLatLonField3D::~GridLatLonField3D()
{
};

// copy constructor
GridLatLonField3D::GridLatLonField3D(const GridLatLonField3D& src): GridField3D(src)
{


        lons = src.lons;
        lats = src.lats;

        lons.setPgroup( pgroup, metproc );
        lats.setPgroup( pgroup, metproc );

}

GridLatLonField3D& GridLatLonField3D::operator=(const GridLatLonField3D& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }
    
    // copy the parent-class elements
    GridLatLonField3D::assign(src);
    
    // copy the elements unique to this class
    lons = src.lons;
    lats = src.lats;

    lons.setPgroup( pgroup, metproc );
    lats.setPgroup( pgroup, metproc );

    return *this;

}      


void GridLatLonField3D::clear() {
   
   lons.clear();
   lats.clear();

   GridField3D::clear();
   
}

int GridLatLonField3D::status() const
{
    int result = 0;

    result = GridField3D::status();
    
    if ( lons.size() <= 0 || lats.size() <= 0 ) {
       result = result | GFS_NODIMS;
    }
    if ( nd != lons.size()*lats.size()*zs.size() ) {
       result = result | GFS_GRIDERR;
    }
    
    return result;

}


void GridLatLonField3D::setPgroup( ProcessGrp* pg, int met)
{
       GridField3D::setPgroup( pg, met );
       lons.setPgroup( pg, met );
       lats.setPgroup( pg, met );

}

void GridLatLonField3D::dims( int* nlon, int* nlat, int* nv)  const
{
   *nlon = lons.size();
   *nlat = lats.size();
   *nv = zs.size();
};

real GridLatLonField3D::value( int i, int j, int k )  const
{
     int indx;
     real result;
   
     if ( ! hasdata() ) {
         std::cerr << "GridLatLonField3D: has no data in value() call" << std::endl;
         throw (baddatareq());
     }    

     // ensure that the longitude index is legal
     i = lons.iwrap(i);
     
    // ensure that the latitude and vertical indices are legal
     if ( j < 0 || j >= lats.size() 
       || k < 0 || k >= zs.size() ) {
         std::cerr << "GridLatLonField3D: Quant=" << quant << " out-of-range latitude or vertical index "  
         << j << ", " << k << " of " << lats.size() << ", " << zs.size() << " in value() call" << std::endl;
         throw (baddatareq());
     }    
     
     // turn the three indices into a single index into the data array
     indx = (k*lats.size() + j)*lons.size() + i;

     result = dater[indx];

     return result;
};

real GridLatLonField3D::value( int indx )  const
{
     real result;
   
     if ( ! hasdata() ) {
         std::cerr << "GridLatLonField3D: has no data in value() call" << std::endl;
         throw (baddatareq());
     }    
     
     result = dater[indx];

     return result;
};

real* GridLatLonField3D::values( int n, real* vals, int *indices ) const
{
     int indx;
     int ref;

     if ( ! hasdata() ) {
         std::cerr << "GridLatLonField3D: has no data in values() call" << std::endl;
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

real& GridLatLonField3D::valueref( int i, int j, int k )  
{
     real *result;
     int indx;

     if ( ! hasdata() ) {
         std::cerr << "GridLatLonField3D: has no data in valueref() call" << std::endl;
         throw (baddatareq());
     }    
     
     // ensure that the longitude index is legal
     i = lons.iwrap( i );
     // ensure that the latitude nad vertical indices are legal
     if ( j < 0 || j >= lats.size() 
       || k < 0 || k >= zs.size() ) {
         std::cerr << "GridLatLonField3D: out-of-range latitude or vertical index " << j << ", " << k << " in valueref() call" << std::endl;
         throw (baddatareq());
     }    
     
     // turn the three indices into a single index into the data array
     indx = (k*lats.size() + j)*lons.size() + i;
     
     result = &(dater[indx]);
     return *result;
}        



std::vector<real> GridLatLonField3D::longitudes()  const
{ 
   return lons.dimension();
};

std::vector<real> GridLatLonField3D::latitudes()  const
{ 
   return lats.dimension();
};


real GridLatLonField3D::longitude( int i )  const
{
   i = lons.iwrap(i);

   return lons(i);
};
   
real GridLatLonField3D::latitude( int j ) const
{
   return lats(j);
};   

real GridLatLonField3D::wrap( real lon ) const
{
     return lons.wrap( lon );
};

int GridLatLonField3D::iwrap( int i ) const
{
     return lons.iwrap( i );
};

void GridLatLonField3D::lonindex( real lon, int* i1, int* i2 ) const
{
     lons.index( lon, i1, i2 );
};

void GridLatLonField3D::latindex( real lat, int* i1, int* i2 ) const
{
     lats.index( lat, i1, i2 );
};

void GridLatLonField3D::setWraps( const int loadFlags )
{
   lons.setWraps( loadFlags );
}



void GridLatLonField3D::load( const realvec& inlons, const realvec& inlats
     , const realvec& inlevels
     , const realvec& indata, const int loadFlags )
{
   int nlons, nlats, nzs;
   
   lons.load( inlons, loadFlags );
   lats.load( inlats, loadFlags );
   zs.load( inlevels, loadFlags );


   nlons = lons.size();
   nlats = lats.size();
   nzs = zs.size();
   
   clearData();
   nd = nlons*nlats*nzs;
   dater = new real[nd];   

   // copy the data   
   for (int indx=0; indx < nd; indx++ ) {
       dater[indx] = indata[indx];
   }
   clear_nodata();


}


void GridLatLonField3D::load( const realvec& inlons, const realvec& inlats
     , const realvec& inlevels, const int loadFlags )
{
   int indx = 0;
   int i;
   int nlons, nlats, nzs;
   
   lons.load( inlons, loadFlags );
   lats.load( inlats, loadFlags );
   zs.load( inlevels, loadFlags );


   nlons = lons.size();
   nlats = lats.size();
   nzs = zs.size();
   
   clearData();

   if ( loadFlags & GFL_PREFILL ) {
      nd = nlons*nlats*nzs;
      dater = new real[nd];   
      for ( int i=0; i<nd; i++ ) {
         dater[i] = fill_value;
      }
      clear_nodata();   
   }
   
}


void GridLatLonField3D::load( const realvec& indata, const int loadFlags )
{
   
   if ( lons.size()*lats.size()*zs.size() != indata.size() ) {
      throw(badincompatcoords());
   }
   clearData();      
   nd = lons.size()*lats.size()*zs.size();
   dater = new real[nd];   
   for ( int i=0; i<nd; i++ ) {
      dater[i] = indata[i];
   }
   clear_nodata();


}


bool GridLatLonField3D::compatible( const GridField3D& obj, int compatFlags ) const
{
    int result = true;
    std::vector<real> dim;
    const GridLatLonField3D *trueobj;
    
    if ( obj.id() == this->id() ) {
    
       result = GridField3D::compatible( obj, compatFlags );
    
       trueobj = dynamic_cast<const GridLatLonField3D*>( &obj );
       
       if ( compatFlags & METCOMPAT_HORIZ ) {
          // check that the longitudes match in number and values
          
          if ( ! lons.compatible( trueobj->lons ) ) {
             result = false;
          }   
          if ( ! lats.compatible( trueobj->lats ) ) {
             result = false;
          }   
       }

    } else {
       // objects of different grid types are by definition
       // incompatible
       result = false;
    }   


    return result;      
       
}


bool GridLatLonField3D::compatible( const GridFieldSfc& obj, int compatFlags ) const
{
    int result = true;
    std::vector<real> dim;
    
    const GridLatLonFieldSfc *trueobj;
    
    // we can only compare surface grids of this type
    if ( obj.id() == "GridLatLonFieldSfc" ) {
    
       result = GridField3D::compatible( obj, compatFlags );
       //if ( ! result ) {
       //    std::cerr << " GridField3D returns incompat!" << std::endl;
       //}
    
       trueobj = dynamic_cast<const GridLatLonFieldSfc*>( &obj );
       
       if ( compatFlags & METCOMPAT_HORIZ ) {
          
          if ( ! lons.compatible( trueobj->getLongitudes() ) ) {
             result = false;
          }   
          if ( ! lats.compatible( trueobj->getLatitudes() ) ) {
             result = false;
          }   
    
       }
    } else {
       // objects of different grid types are by definition
       // incompatible
       //std::cerr << " incompatible grid: type " << obj.id() << std::endl;
       result = false;
    }   

    return result;      
       
}

bool GridLatLonField3D::match( const GridLatLonField3D& obj ) const  
{
    int result = true;
    std::vector<real> dim;
    
    if ( ! this->compatible(obj) ) {
       result = false;
    }
    
    if ( obj.quantity() != quant 
    ||   obj.units() != uu  
    ||   obj.vertical() != vquant
    ||   obj.vunits() != vuu
    ||   obj.fillval() != fill_value ) {
       result = false;
    }   
    
    return result;      
       
}

int GridLatLonField3D::doeswrap() const
{
   return lons.wrapping();
}   


GridLatLonFieldSfc* GridLatLonField3D::extractSfc( int k ) const
{
    GridLatLonFieldSfc* result;
    real* newdata;
    std::string name;
    int indx;
    int ndx;
    real* dimvals;
    int nx;
    
    
    if ( k < 0 || k >= zs.size() ) {
       std::cerr << "bad vertical index " << k << " in extractSfc() call" << std::endl;
       throw (baddatareq());
    }
    
    result = new GridLatLonFieldSfc;
    result->set_quantity( quant );
    result->set_units( uu );
    result->mksScale = mksScale;
    result->mksOffset = mksOffset;
    result->set_fillval( fill_value );
    result->set_time( mtime, ctime );
    
    dimvals = lons.extract( &nx );
    result->absorbLons( nx, dimvals );
    dimvals = lats.extract( &nx );
    result->absorbLats( nx, dimvals );
    
    name = zs(k); 
    name = name + " " + vuu + " " + vquant + " surface";
    result->set_surface( name );
    
    ndx = lons.size()*lats.size();
    newdata = new real[ndx];
    for ( int indx=0; indx < ndx; indx++ ) {
        newdata[indx] = dater[indx + k*ndx];
    }    
    result->absorb( lons.size(), lats.size(), newdata );    

    return result;

}

GridFieldSfc* GridLatLonField3D::extractSurface( int k ) const
{
   return extractSfc(k);
}

void GridLatLonField3D::replaceLevel( GridFieldSfc& sfc, int k)
{
    real val;
    int nlats, nlons;
    
    if ( this->compatible(sfc) ) {
    
      nlons = lons.size();
      nlats = lats.size();
    
      for ( int j=0; j<nlats; j++ ) {
         for ( int i=0; i<nlons; i++ ) {
             // scale the source data to MKS
             val = sfc(i,j) * sfc.mksScale + sfc.mksOffset;
             // scale the MKS data to our units
             this->valueref(i,j,k) = (val - this->mksOffset )/this->mksScale;
         }
      }

    } else {
       throw (badgrid());
    }

}


real GridLatLonField3D::area(int i, int j) const
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

real GridLatLonField3D::area(int i) const
{
    int ii,jj;
    
    ii = i % lons.size();
    jj = i / lons.size();
    
    return area(ii,jj);
    
}




GridFieldSfc* GridLatLonField3D::areas() const
{
    GridLatLonFieldSfc* result;
    GridLatLonFieldSfc* slice;
    std::vector<real> newdata;
    real ar;
    real dlon, dlat;
    real lat1, lat2, lon1, lon2;
    
    slice = this->extractSfc(0);
    result = dynamic_cast<GridLatLonFieldSfc*>(slice->areas());

    delete slice;
    
    return result;


}

void GridLatLonField3D::absorb( int nlons, int nlats, int nzs, real* vals , real* lonvals, real* latvals, real *zvals)
{
      if ( lonvals != NULLPTR ) {
         lons.absorb( nlons, lonvals );
      }
      if ( latvals != NULLPTR ) {
         lats.absorb( nlats, latvals );
      }
      if ( zvals != NULLPTR ) {
         zs.absorb( nzs, zvals );
      }
      
      clearData();
      nd = nlons*nlats*nzs;
      dater = vals;
      clear_nodata();
      
}

void GridLatLonField3D::absorbLons( int n, real* lonvals )
{
      if ( lonvals != NULLPTR ) {
         lons.absorb( n, lonvals );
      }

}

void GridLatLonField3D::absorbLats( int n, real* latvals )
{
      if ( latvals != NULLPTR ) {
         lats.absorb( n, latvals );
      }
}

GridField3D* GridLatLonField3D::duplicate() const
{
    GridLatLonField3D* newgrid;
   
    newgrid = new GridLatLonField3D();
    
    *newgrid = *this;

    return newgrid;

}

int GridLatLonField3D::dataSize() const
{
    int nlons, nlats, nzs;
    
    nlons = lons.size();
    nlats = lats.size();
    nzs = zs.size();
    
    if ( nzs == 0 || nlons == 0 || nlats == 0 ) {
       throw (badnodims());
    }
    
    return nlons*nlats*nzs;   

}

void GridLatLonField3D::gridcoords( int n, int* is, int* js, int* ks, real* olons, real* olats, real* olevs, int flags) const
{

     lons.values( n, olons, is );
     lats.values( n, olats, js );
     zs.values( n, olevs, ks );

}


void GridLatLonField3D::gridpoints( int n, int* is, int* js, int* ks, real* vals, int flags) const
{
     int* coords;
     int cmd;
     int local;
     int done;
     
     local = flags & 0x01;
     done = flags * 0x02;

     //- std::cerr << " parallel gridpoints" << std::endl;
     // get the data from a central met processors    
     try {
        coords = new int[n];
     } catch(...) {
        throw (badmemreq());
     }
     //- std::cerr << "--- tracproc starts receiving values" << std::endl;    
     
     joinIndex( n, coords, is, js, ks );
     
     if ( pgroup == NULLPTR || metproc < 0 || local != 0 ) {
         //- std::cerr << " serial gridpoints" << std::endl;
         // serial processing.  Access the data locally
         this->values( n, vals, coords );
         //for ( int i=0; i<n; i++ ) {
         //    vals[i] = this->value(is[i],js[i],ks[i]);
         //}
     } else {

         // send "get data" command to central met reader process
         //cmd = PGR_CMD_GDATA;
         // pgroup->send_ints( metproc, 1, &cmd, PGR_TAG_GREQ );
         //- std::cerr << "  about to send N to " << metproc << std::endl;
         // send request for n points 
         pgroup->send_ints( metproc, 1, &n, PGR_TAG_GNUM );
         //- std::cerr << "--- tracproc n=" << n << std::endl;    
         //- std::cerr << "  sent N=" << n << " to " << metproc << std::endl;
         // send the coordinates
         pgroup->send_ints( metproc, n, coords, PGR_TAG_GCOORDS );
         //- std::cerr << "  sent pnt indices to " << metproc << std::endl;
         // receive the values
         pgroup->receive_reals( metproc, n, vals, PGR_TAG_GVALS );
         //- std::cerr << " yyyyyyyyyyyy: got " << n << " values " << std::endl;
     
         if ( done ) {
            //- std::cerr << " sending GDONE " << std::endl;
            svr_done();
         }
         
     }
     //- std::cerr << "--- tracproc stop receiving values" << std::endl;    
     
     delete[] coords;

}

void GridLatLonField3D::gridpoints( int n, int* indices, real* vals, int flags) const
{
     int cmd;
     int local;
     int done;
     
     local = flags & 0x01;
     done = flags * 0x02;

     if ( pgroup == NULLPTR || metproc < 0 || local != 0 ) {
         //- std::cerr << " serial gridpoints" << std::endl;
         // serial processing.  Access the data locally
         this->values( n, vals, indices );

     } else {

         // Never ask a dedicated met processor to fetch gridpoint data for itself
         if ( metproc == pgroup->id() ) {
            throw (badProcReq());
         }

         // send "get data" command to central met reader process
         //cmd = PGR_CMD_GDATA;
         // pgroup->send_ints( metproc, 1, &cmd, PGR_TAG_GREQ );
         //- std::cerr << "  about to send N to " << metproc << std::endl;
         // send request for n points 
         pgroup->send_ints( metproc, 1, &n, PGR_TAG_GNUM );
         //- std::cerr << "  sent N=" << n << " to " << metproc << std::endl;
         // send the coordinates
         pgroup->send_ints( metproc, n, indices, PGR_TAG_GCOORDS );
         //- std::cerr << "  sent pnt indices to " << metproc << std::endl;
         // receive the values
         pgroup->receive_reals( metproc, n, vals, PGR_TAG_GVALS );
         //- std::cerr << " yyyyyyyyyyyy: got " << n << " values " << std::endl;
     
         if ( done ) {
            //- std::cerr << " sending GDONE " << std::endl;
            svr_done();
         }
         
     }
     
}

void GridLatLonField3D::receive_meta()
{
     real* dimvals;
     int cmd;
     int i;

     if (  pgroup == NULLPTR || metproc < 0 ) {

         // serial processing.  Load nothing, but
         // check that the object has valid metadata
         if ( this->status() & (~0x10) ) {
            throw (baddataload());
         }
         
     } else {
     
         // Never ask a dedicated met processor to fetch metadata for itself
         if ( metproc == pgroup->id() ) {
            throw (badProcReq());
         }
     
         // receive the metadata
         //- std::cerr << "   GridLatLonField3D::receive_meta: r-100 with " << metproc << std::endl;
         pgroup->receive_string( metproc, &quant, PGR_TAG_GMETA ); // quantity
         //- std::cerr << "   GridLatLonField3D::receive_meta: r-110 from " << metproc  << std::endl;
         pgroup->receive_string( metproc, &uu, PGR_TAG_GMETA );  // units
         //- std::cerr << "   GridLatLonField3D::receive_meta: r-120 from " << metproc  << std::endl;
         pgroup->receive_string( metproc, &ctime, PGR_TAG_GMETA );  // met time stamp
         //- std::cerr << "   GridLatLonField3D::receive_meta: r-130 from " << metproc  << std::endl;
         pgroup->receive_doubles( metproc, 1, &mtime, PGR_TAG_GMETA );  // met time 
         //- std::cerr << "   GridLatLonField3D::receive_meta: r-140 from " << metproc  << std::endl;
         pgroup->receive_reals( metproc, 1, &fill_value, PGR_TAG_GMETA );  // fill value
         //- std::cerr << "   GridLatLonField3D::receive_meta: r-150 from " << metproc  << std::endl;

         pgroup->receive_string( metproc, &vquant, PGR_TAG_GMETA );  // vertical coordinate quantity
         //- std::cerr << "   GridLatLonField3D::receive_meta: r-200 from " << metproc  << std::endl;
         pgroup->receive_string( metproc, &vuu, PGR_TAG_GMETA );  // vertical coordinate units
         //- std::cerr << "   GridLatLonField3D::receive_meta: r-220 from " << metproc << std::endl;

         lons.receive_meta();
         lats.receive_meta();
         zs.receive_meta();
         //- std::cerr << "   GridLatLonField3D::receive_meta: r-320 got dims "<< std::endl;
        
         clearData();
   
         metaID = 0;
   
     }

}



void GridLatLonField3D::svr_send_meta(int id) const
{
     real* dimvals;
     int cmd;
     int i;

     //- std::cerr << "   GridLatLonField3D::svr_send_meta: entry" << std::endl;
     if (  pgroup == NULLPTR || metproc < 0 ) {
         //- std::cerr << "   GridLatLonField3D::svr_send_meta: 490 metproc = " << metproc << std::endl;
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
         //- std::cerr << "   GridLatLonField3D::svr_send_meta: s-100 with " << id << std::endl;
         pgroup->send_string( id, quant, PGR_TAG_GMETA ); // quantity
         //- std::cerr << "   GridLatLonField3D::svr_send_meta: s-110 to " << id << std::endl;
         pgroup->send_string( id, uu, PGR_TAG_GMETA );  // units
         //- std::cerr << "   GridLatLonField3D::svr_send_meta: s-120 to " << id << std::endl;
         pgroup->send_string( id, ctime, PGR_TAG_GMETA );  // met time stamp
         //- std::cerr << "   GridLatLonField3D::svr_send_meta: s-130 to " << id << std::endl;
         pgroup->send_doubles( id, 1, &mtime, PGR_TAG_GMETA );  // met time 
         //- std::cerr << "   GridLatLonField3D::svr_send_meta: s-140 to " << id << std::endl;
         pgroup->send_reals( id, 1, &fill_value, PGR_TAG_GMETA );  // fill value
         //- std::cerr << "   GridLatLonField3D::svr_send_meta: s-150 to " << id << std::endl;

         pgroup->send_string( id, vquant, PGR_TAG_GMETA ); // vertical coordinate quantity
         //- std::cerr << "   GridLatLonField3D::svr_send_meta: s-200 to " << id << std::endl;
         pgroup->send_string( id, vuu, PGR_TAG_GMETA ); // vertical coordinate units
         //- std::cerr << "   GridLatLonField3D::svr_send_meta: s-220 to " << id << std::endl;

         lons.svr_send_meta(id);
         lats.svr_send_meta(id);
         zs.svr_send_meta(id);
   
         //- std::cerr << "   GridLatLonField3D::svr_send_meta: s-260" << std::endl;
     }

}



void GridLatLonField3D::serialize(std::ostream& os) const
{
  int prec;
  string str;
  int len;
  int i, j, k;
  double t;
  real val;
  int ival;
  int version = 2;
  
  try {    

      GridField3D::serialize(os);
      
      // output the version
      os.write( reinterpret_cast<char *>( &version), static_cast<std::streamsize>( sizeof(int)));
      
      switch (version) {
      case 1: 
           // output the number of longitudes
           ival = lons.size();
           os.write( reinterpret_cast<char *>(&ival), static_cast<std::streamsize>( sizeof(int)));
           // output the number of latitudes
           ival = lats.size();
           os.write( reinterpret_cast<char *>(&ival), static_cast<std::streamsize>( sizeof(int)));
           // output the number of levels
           ival = zs.size();
           os.write( reinterpret_cast<char *>(&ival), static_cast<std::streamsize>( sizeof(int)));
    
           // output the longitudes
           for ( i=0; i < lons.size(); i++ ) {
               val = lons(i);
               os.write( reinterpret_cast<char *>(&val), static_cast<std::streamsize>( sizeof(real)));
           }    
           // output the latitudes
           for ( i=0; i < lats.size(); i++ ) {
               val = lats(i);
               os.write( reinterpret_cast<char *>(&val), static_cast<std::streamsize>( sizeof(real)));
           }    
           // output the levels
           for ( i=0; i < zs.size(); i++ ) {
               val = zs(i);
               os.write( reinterpret_cast<char *>(&val), static_cast<std::streamsize>( sizeof(real)));
           }    
           break;
      case 2: 
          lons.serialize(os);
          lats.serialize(os);
          zs.serialize(os);
          break; 
      }
      
      // output the data
      for ( k=0; k<zs.size(); k++ ) {
      for ( j=0; j<lats.size(); j++ ) {
      for ( i=0; i<lons.size(); i++ ) {
          val = value(i,j,k);
          os.write( reinterpret_cast<char *>(&val), static_cast<std::streamsize>( sizeof(real)));
      }
      }
      }
    

  } catch (...) {
      throw;
  }    

}

void GridLatLonField3D::deserialize(std::istream& is)
{
   string str;
   int len;
   char cc;
   int i;
   double t;
   real val;
   int ival;
   int nxlons, nxlats, nxzs;
   real* xdata;
   int version;
   real* xlons;
   real* xlats;
   real *xzs;

   clear();   

   try {
   
       GridField3D::deserialize(is);
      
       // read the version
       is.read(reinterpret_cast<char *>( &version), static_cast<std::streamsize>( sizeof(int)));
              
      switch (version) {
      case 1: 
            // read the number of longitudes
            is.read( reinterpret_cast<char *>(&nxlons), static_cast<std::streamsize>( sizeof(int) ));
            // read the number of latitudes
            is.read( reinterpret_cast<char *>(&nxlats), static_cast<std::streamsize>( sizeof(int) ));
            // read the number of levels
            is.read( reinterpret_cast<char *>(&nxzs), static_cast<std::streamsize>( sizeof(int) ));
            
            // read the longitudes
            xlons = new real[nxlons];
            for ( i=0; i<nxlons; i++ ) {
                is.read( reinterpret_cast<char *>(&val), static_cast<std::streamsize>( sizeof(real)) );
                xlons[i] = val;
            }    
            lons.absorb( nxlons, xlons );
            // read the latitudes
            xlats = new real[nxlats];
            for ( i=0; i<nxlats; i++ ) {
                is.read( reinterpret_cast<char *>(&val), static_cast<std::streamsize>( sizeof(real)) );
                xlats[i] = val;
            }    
            lats.absorb( nxlats, xlats );
            
            // read the levels
            xzs = new real[nxzs];
            for ( i=0; i<nxzs; i++ ) {
                is.read( reinterpret_cast<char *>(&val), static_cast<std::streamsize>( sizeof(real)) );
                xzs[i] = val;
            }   
            zs.absorb( nxzs, xzs );
            
             
      case 2: 
           lons.deserialize(is);
           lats.deserialize(is);
           zs.deserialize(is);
           
           nxlons = lons.size();
           nxlats = lats.size();
           nxzs = zs.size();
           
           break;
      } 
      
       // read the data
       xdata = new real[nxlats*nxlons*nxzs];
       for ( i=0; i<nxlats*nxlons*nxzs; i++ ) {
           is.read( reinterpret_cast<char *>(&val), static_cast<std::streamsize>( sizeof(real)) );
           xdata[i] = val;
       }    
      absorb( nxlons, nxlats, nxzs, xdata );          

   } catch (...) {
       throw;
   }    
   


}

/****************** Iterators **************************/


GridField3D::iterator GridLatLonField3D::begin()
{
   
   return GridField3D::iterator( this, 0, 0, nd-1 );

}

GridField3D::iterator GridLatLonField3D::end()
{
   
   return GridField3D::iterator( this, nd, 0, nd-1  );

}

GridField3D::iterator GridLatLonField3D::begin(int k)
{
   int start, end;
   
   start = lons.size()*lats.size()*k;
   end = lons.size()*lats.size()*(k+1) - 1;   
   
   return GridField3D::iterator( this, start, start, end );
   
}

GridField3D::iterator GridLatLonField3D::end(int k)
{
   int start, end;
   
   start = lons.size()*lats.size()*k;
   end = lons.size()*lats.size()*(k+1) - 1;
   
   return GridField3D::iterator( this, end+1, start, end );
   
}


///////////////////////////////


GridField3D::const_iterator GridLatLonField3D::begin() const
{
   
   return GridField3D::const_iterator( this, 0, 0, nd-1 );

}

GridField3D::const_iterator GridLatLonField3D::end() const
{
   
   return GridField3D::const_iterator( this, nd, 0, nd-1 );
   
}

GridField3D::const_iterator GridLatLonField3D::begin(int k) const
{
   int start, end;
   
   start = lons.size()*lats.size()*k;
   end = lons.size()*lats.size()*(k+1) - 1;   

   return GridField3D::const_iterator( this, start, start, end );

}


GridField3D::const_iterator GridLatLonField3D::end(int k) const
{
   int start, end;
   
   start = lons.size()*lats.size()*k;
   end = lons.size()*lats.size()*(k+1) - 1;   
   
   return GridField3D::const_iterator( this, end+1, start, end );

}

///////////////////////////////



GridField3D::profileIterator GridLatLonField3D::profileBegin()
{

   return GridField3D::profileIterator( this, 0, zs.size(), 0, lons.size()*lats.size()-1, lons.size()*lats.size() );
   
}

GridField3D::profileIterator GridLatLonField3D::profileEnd()
{
   
   return GridField3D::profileIterator( this, lons.size()*lats.size(), zs.size(), 0, lons.size()*lats.size()-1, lons.size()*lats.size()  );

}


///////////////////////////////


GridField3D::const_profileIterator GridLatLonField3D::profileBegin() const
{
   
   return GridField3D::const_profileIterator( this, 0, zs.size(), 0, lons.size()*lats.size()-1, lons.size()*lats.size() );

}

GridField3D::const_profileIterator GridLatLonField3D::profileEnd() const
{
   
   return GridField3D::const_profileIterator( this, lons.size()*lats.size(), zs.size(), 0, lons.size()*lats.size()-1, lons.size()*lats.size()  );

}

GridField3D::profileIterator GridLatLonField3D::profileBegin( int i, int j )
{
   
   return GridField3D::profileIterator( this, i+j*lons.size(), zs.size(), 0, lons.size()*lats.size()-1, lons.size()*lats.size() );

}

GridField3D::const_profileIterator GridLatLonField3D::profileBegin( int i, int j ) const
{
   
   return GridField3D::const_profileIterator( this, i+j*lons.size(), zs.size(), 0, lons.size()*lats.size()-1, lons.size()*lats.size() );
   
}

void GridLatLonField3D::splitIndex( int index, int* i, int*j, int*k ) const
{
    int subindex;
    
    // reduce the index to lie in the first vertical level
    subindex = index % (lats.size()*lons.size());
    // now get the indices in that level
    *i = subindex % lons.size();
    *j = subindex / lons.size();
    *k = index / (lats.size()*lons.size());

}

void GridLatLonField3D::splitIndex( int n, int *index, int* i, int* j, int* k ) const
{
    int subindex;
    int m;
    
    if ( n > 0 ) {
    
       for ( m=0; m < n; m++ ) {
          // reduce the index to lie in the first vertical level
          subindex = index[m] % (lats.size()*lons.size());
      
          // now get the indices in that level
          i[m] = subindex % lons.size();
          j[m] = subindex / lons.size();
          k[m] = index[m] / (lats.size()*lons.size());
       }
    }
}

int GridLatLonField3D::joinIndex( int i, int j, int k ) const
{
     int result;
     
     // ensure that the longitude index is legal
     i = lons.iwrap(i);
     // ensure that the latitude and vertical indices are legal
     if ( j < 0 || j >= lats.size() 
       || k < 0 || k >= zs.size() ) {
         std::cerr << "GridLatLonField3D: out-of-range latitude or vertical index "  << j << ", " << k << " of " << lats.size() << ", " << zs.size() << " in joinIndex() call" << std::endl;
         throw (baddatareq());
     }    

     result = (k*lats.size() + j)*lons.size() + i;

     return result;
}

int* GridLatLonField3D::joinIndex( int n, int *index, int* i, int* j, int* k ) const{
     
     if ( n > 0 ) {
     
        if ( index == NULLPTR ) {
           index = new int[n];
        }
     
        for ( int m=0; m<n; m++ ) {           // ensure that the longitude index is legal
            i[m] = lons.iwrap( i[m] );
            // ensure that the latitude and vertical indices are legal                                                   
            if ( j[m] < 0 || j[m] >= lats.size()                                                                                     
              || k[m] < 0 || k[m] >= zs.size() ) {                                                                                   
                std::cerr << "GridLatLonField3D:  out-of-range latitude or vertical index "  << j[m]   
                << ", " << k[m] << " of " << lats.size() << ", " << zs.size() << " in joinIndex() call" << std::endl;                       
                throw (baddatareq());                                                                                    
            }
                                                                                                                 
            index[m] = (k[m]*lats.size() + j[m])*lons.size() + i[m];
        }
     }
     
     return index;
}

//.......................................................................................

namespace gigatraj {

// outputs the Met Field 
std::ostream& operator<<( std::ostream& os, const GridLatLonField3D& p)
{
  
  try {

      p.serialize(os);
  
  } catch (...) {
      throw;
  }    
  
  return os;

};

// inputs the Met field
std::istream& operator>>( std::istream& is, GridLatLonField3D& p)
{

   try {
   
      p.deserialize(is);

   } catch (...) {
       throw;
   }    
   
   return is;

};

}
