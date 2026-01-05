
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

#include "gigatraj/GridFieldProfile.hh"

using namespace gigatraj;


const std::string GridFieldProfile::iam = "GridFieldProfile";


// constructor
GridFieldProfile::GridFieldProfile() : GridField()
{
   
   prof = "none";
   zs.clear();
   
   use_array = 1;
   dater = NULLPTR;

}


// destructor
GridFieldProfile::~GridFieldProfile()
{
};

// copy constructor
GridFieldProfile::GridFieldProfile(const GridFieldProfile& src) : GridField(src)
{

   prof = src.profile();
   zs = src.zs;
   zs.setPgroup( pgroup, metproc );

}

// copy assignment
GridFieldProfile& GridFieldProfile::operator=(const GridFieldProfile& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }
    
    this->assign( src ) ;
    
    return *this;
}


void GridFieldProfile::assign( const GridFieldProfile& src )
{
   
   GridField::assign(src);
   
   prof = src.profile();
   zs = src.zs;
   zs.setPgroup( pgroup, metproc );

}

void GridFieldProfile::clear() {

   prof = "none";
   
   zs.clear();
   
   GridField::clear();
      
}


std::string GridFieldProfile::vertical() const
{
   return zs.quantity();
}

void GridFieldProfile::set_vertical( const std::string vq )
{
   zs.set_quantity( vq );

}

std::string GridFieldProfile::vunits(real *scale, real*offset) const
{
   std::string result;
   
   result = zs.units( scale, offset );
   

   return result;
}

void GridFieldProfile::set_vunits( const std::string vu, real scale, real offset )
{
   zs.set_units( vu, scale, offset );
}         

std::vector<real> GridFieldProfile::levels() const 
{ 
   return zs.dimension();
};

void GridFieldProfile::levels( const std::vector<real> &newvert )
{
   if ( zs.size() > 0 && zs.size() != newvert.size() ) {
      std::cerr << "New vertical cooridnate vector does not match the old in size." << std::endl;
      throw (badDimensionMismatch());
   }

   zs.load( newvert );

}

real GridFieldProfile::level( const int k ) const
{
   return zs(k);
}; 


void GridFieldProfile::zindex( real z, int* i1, int* i2 ) const
{
     
     zs.index( z, i1, i2 );
};



int GridFieldProfile::status() const
{
    int result = 0;
    
    result = GridField::status();

    if ( prof == "none" ) {
       result = result | 0x02;
    }    
    if ( zs.size() <= 0 ) {
       result = result | GFS_NODIMS;
    }
    if ( nd != zs.size() ) {
       result = result | GFS_GRIDERR;
    }
    
    return result;

}


real GridFieldProfile::value( int i )  const
{
    
    if ( ! hasdata() ) {
        throw (baddatareq());
    }    
    
    // ensure that the coordinate index is legal
    if ( i < 0 || i >= zs.size() ) {
        throw (baddatareq());
    }    
    
    return dater[i];
};


real& GridFieldProfile::valueref( int i ) 
{
   real* result;

   if ( ! hasdata() ) {
       throw (baddatareq());
   }    
     
   // ensure that the coordinate index is legal
   if ( i < 0 || i >= zs.size() ) {
       throw (baddatareq());
   }    

   result = &(dater[i]);
   return *result;
}        

real* GridFieldProfile::values( int n, real* vals, int *indices ) const
{
     int indx;
     int ref;
     int i,j;

     if ( ! hasdata() ) {
         std::cerr << "GridFieldProfile: has no data in values() call" << std::endl;
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

void GridFieldProfile::dims( int* nx )  const
{
   *nx = zs.size();
};

real GridFieldProfile::operator()( int i ) const 
{
   return value(i);
}        

real& GridFieldProfile::operator()( int i )
{
   return valueref(i);
}        

std::string GridFieldProfile::profile() const
{
   return prof;
}


void GridFieldProfile::set_profile( const std::string nm )
{
   prof = nm;
}

void GridFieldProfile::transform( const std::string unyts, real scale, real offset ) 
{
    real ms, mo;
    GridFieldProfile::iterator dat;
    real s, o;
    
    uu = unyts;

    s = mksScale/scale;
    o = ( mksOffset - offset)/scale;
    
    // Alter our scale and offset so that they will
    // still take us to MKS units, regardless of
    // what the old units were and what the new units will be.
    
    // apply the scaling
    for ( dat = this->begin(); dat != this->end(); dat++ ) {
        if (*dat != fill_value ) {
           *dat  = (*dat )*s + o;
        }
    } 

    mksOffset = offset;
    mksScale  = scale;

}


void GridFieldProfile::load( const realvec& inx, const realvec& indata, const int loadFlags  )
{
   int indx = 0;
   int i;
   
   zs.load( inx );   
   
   clearData();
   
   nd = zs.size();
   dater = new real[nd];

   // copy the data   
   for (int indx=0; indx < zs.size(); indx++ ) {
       dater[indx] = indata[indx];
   }
   clear_nodata();
   

}

void GridFieldProfile::load( const realvec& indata, const int loadFlags  )
{
   int indx = 0;
   int i;
   
   if ( zs.size() != indata.size() ) {
      throw(badincompatcoords());
   }      
   
   clearData();
   
   nd = zs.size();
   dater = new real[nd];

   // copy the data   
   for (int indx=0; indx < zs.size(); indx++ ) {
       dater[indx] = indata[indx];
   }
   
   clear_nodata();

}

void GridFieldProfile::loaddim( const realvec& inx, const int loadFlags  )
{
   int indx = 0;
   int i;
   
   zs.load( inx );
         
   // Don't try to copy any data 
   clearData();  
   if ( loadFlags & GFL_PREFILL ) {
      for ( i=0; i < zs.size(); i++ ) {
         dater[i] = fill_value;
      }
      clear_nodata();   
   }
   
}


bool GridFieldProfile::compatible( const GridFieldProfile& obj, int flags ) const  
{
    int result = true;
    std::vector<real> dim;
    
    if ( flags & METCOMPAT_VERT ) {
       // the surface types must match
       if ( obj.profile() == prof ) {
          // and the vertical coordinate types must match
          if ( obj.vertical() == zs.quantity() && obj.vunits() == zs.units() ) {
       
             // check that the vertical levels match in number and values
             dim = obj.levels();
             if ( dim.size() == zs.size() ) {
                for ( int i=0; i < zs.size(); i++ ) {
                   // levels must be within 1.0e-8 (of whatever units are being used)
                   if ( ABS( dim[i] - zs(i) ) > 1.0e-8 ) {
                      result = false;
                   }   
                }
             } else {
                result = false;
             }       
          } else {
             result = false;
          }    
       } else {             
          result = false;   
       }                    
    }         

    if ( flags & METCOMPAT_TIME ) {
       // the model times and the calendar times must both match
       if ( obj.time() != mtime || obj.met_time() != ctime ) {
          result = false;
       }   
    }
    
    return result;      

}

bool GridFieldProfile::match( const GridFieldProfile& obj ) const  
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

int GridFieldProfile::dataSize() const
{
    if ( zs.size() <= 0  ) {
       throw (badnodims());
    }
    
    return zs.size();   

}

void GridFieldProfile::gridpoints( int n, int* is, real* vals, int flags ) const
{
     int cmd;
     int local;
     int done;
     
     local = flags & 0x01;
     done = flags * 0x02;

     // get the data from a central met processors    

     if ( pgroup == NULLPTR || metproc < 0 || local != 0 ) {
         // serial processing.  Access the data locally
         this->values( n, vals, is );
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
         pgroup->send_ints( metproc, n, is, PGR_TAG_GCOORDS );
         // receive the values
         pgroup->receive_reals( metproc, n, vals, PGR_TAG_GVALS );

         if ( done ) {
            svr_done();
         }
     
     }

}

void GridFieldProfile::receive_meta()
{
     real* dimvals;
     int cmd;
     int i;
     int nzs;

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
     
         // receive theta metadata
         //- std::cerr << "   GridLatLonField3D::receive_meta: r-100 with " << metproc << std::endl;
         pgroup->receive_string( metproc, &quant, PGR_TAG_GMETA ); // quantity
         //- std::cerr << "   GridFieldProfile::receive_meta: r-110 from " << metproc  << std::endl;
         pgroup->receive_string( metproc, &uu, PGR_TAG_GMETA );  // units
         //- std::cerr << "   GridFieldProfile::receive_meta: r-120 from " << metproc  << std::endl;
         pgroup->receive_string( metproc, &ctime, PGR_TAG_GMETA );  // met time stamp
         //- std::cerr << "   GridFieldProfile::receive_meta: r-130 from " << metproc  << std::endl;
         pgroup->receive_doubles( metproc, 1, &mtime, PGR_TAG_GMETA );  // met time 
         //- std::cerr << "   GridFieldProfile::receive_meta: r-140 from " << metproc  << std::endl;
         pgroup->receive_reals( metproc, 1, &fill_value, PGR_TAG_GMETA );  // fill value
         //- std::cerr << "   GridFieldProfile::receive_meta: r-170 from " << metproc  << std::endl;
         pgroup->receive_string( metproc, &prof, PGR_TAG_GMETA ); // quantity
         //- std::cerr << "   GridFieldProfile::receive_meta: r-200 from " << metproc  << std::endl;

         set_nodata();  // there are no data
   
         // get the coordinate metadata
         zs.receive_meta();
         
         nzs = zs.size();
         
         try {
            dimvals = new real[nzs];
         } catch(...) {
            throw (badmemreq());
         }
         pgroup->receive_reals( metproc, nzs, dimvals, PGR_TAG_GDIMS ); 
         zs.absorb(nzs, dimvals);
         // note; no NOT delete dimvels here; zs now owns that array

         metaID = 0;
   
     }

}

void GridFieldProfile::svr_send_meta(int id) const
{
     real* dimvals;
     int cmd;
     int i;
     int nzs;

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

         nzs = zs.size();

         // receive the metadata
         //- std::cerr << "   GridFieldProfile::svr_send_meta: s-100 with " << id << std::endl;
         pgroup->send_string( id, quant, PGR_TAG_GMETA ); // quantity
         //- std::cerr << "   GridFieldProfile::svr_send_meta: s-110 to " << id << std::endl;
         pgroup->send_string( id, uu, PGR_TAG_GMETA );  // units
         //- std::cerr << "   GridFieldProfile::svr_send_meta: s-120 to " << id << std::endl;
         pgroup->send_string( id, ctime, PGR_TAG_GMETA );  // met time stamp
         //- std::cerr << "   GridFieldProfile::svr_send_meta: s-130 to " << id << std::endl;
         pgroup->send_doubles( id, 1, &mtime, PGR_TAG_GMETA );  // met time 
         //- std::cerr << "   GridFieldProfile::svr_send_meta: s-140 to " << id << std::endl;
         pgroup->send_reals( id, 1, &fill_value, PGR_TAG_GMETA );  // fill value
         //- std::cerr << "   GridFieldProfile::svr_send_meta: s-180 to " << id << std::endl;
         pgroup->send_string( id, prof, PGR_TAG_GMETA ); // surface quantity
         //- std::cerr << "   GridFieldProfile::svr_send_meta: s-200 to " << id << std::endl;

         zs.svr_send_meta(id);

         // send the coordinates
         try  {
            dimvals = new real[nzs];
         } catch(...)  {
            throw (badmemreq());
         }
         for ( i=0; i < nzs; i++ ) {
             dimvals[i] = zs(i);
         }    
         pgroup->send_reals( id, nzs, dimvals, PGR_TAG_GDIMS ); 
         delete[] dimvals;
   
     }

}


void GridFieldProfile::serialize(std::ostream& os) const
{
  int prec;
  string str;
  int len;
  int i, j;
  double t;
  real val;
  int ival;
  const char *cstr;
  int version = 1;
  
  try {

      GridField::serialize(os);
        
      // output the version
      os.write( reinterpret_cast<char *>( &version), static_cast<std::streamsize>( sizeof(int)));
      
      // output the profile name
      len = prof.length();
      os.write( reinterpret_cast<char *>( &len), static_cast<std::streamsize>( sizeof(int)));
      os.write( prof.c_str(), static_cast<std::streamsize>( len*sizeof(char)));
      
      // output the coordinates
      zs.serialize( os );
  
      // output the data
      for ( i=0; i < nd; i++ ) {
          val = dater[i];
          os.write( reinterpret_cast<char *>( &val), static_cast<std::streamsize>( sizeof(real)));
      }
    

  } catch (...) {
      throw;
  }    

}

void GridFieldProfile::deserialize(std::istream& is)
{
   string str;
   int len;
   char cc;
   int i;
   double t;
   real val;
   int ival;
   int nxzs;
   std::vector<real> dat;
   int version;
   
   clear();

   try {
        
      GridField::deserialize(is);
   
       // read the version
       is.read(reinterpret_cast<char *>( &version), static_cast<std::streamsize>( sizeof(int)));
          
       // read the profile name
       is.read(reinterpret_cast<char *>( &len), static_cast<std::streamsize>( sizeof(int)));
       str = "";
       if ( len >= 0 ) {
          prof.clear();
          for ( i=0; i < len; i++ ) {
             is.read( &cc, static_cast<std::streamsize>( sizeof(char)));
             str.push_back( cc );
          }   
       }
       prof = str;

       // read the coordinates
       zs.deserialize( is );

       nxzs = zs.size();
       
       if ( nxzs > 0 ) {
          // read the data
          clearData();
          nd = nxzs;
          dater = new real[nd];
          for ( i=0; i < nd; i++ ) {
              is.read( reinterpret_cast<char *>(&val), static_cast<std::streamsize>( sizeof(real) ));
              dater[i] = val;
          }
          clear_nodata();
                 
       }
   } catch (...) {
       throw;
   }    

}

void GridFieldProfile::svr_send_vals( int id ) const
{
     int* coords;
     real* vals;
     int cmd;
     int n;
     
     if ( metproc < 0 ) {
         // serial processing.  Send nothing, but
         // check that the object has valid data
         if ( ! this->hasdata() ) {
            throw (baddataload());
         }
         
     } else {
     
        // get the number of points desired
        pgroup->receive_ints( id, 1, &n, PGR_TAG_GNUM );

        // get the integer coordinates of the points
        try {
           coords = new int[n];
        } catch (...) {
           throw (badmemreq());
        }
        pgroup->receive_ints( id, n, coords, PGR_TAG_GCOORDS );

        // send the data values requested
        vals = new real[n];
        for ( int i=0; i<n; i++ ) {
            vals[i] = this->value( coords[i] );
        }
        // todo: send error instead of numbers
        
        pgroup->send_reals( id, n, vals, PGR_TAG_GVALS );
        
        delete vals;
        delete coords;
     }

}




/****************** Iterators **************************/

GridFieldProfile::iterator::iterator()
{
   my_index = 0;
   my_grid = NULLPTR;
}   


GridFieldProfile::iterator::iterator( GridFieldProfile* grid, int n )
{
   my_index = n;
   my_grid = grid;
}

/// override operator =
GridFieldProfile::iterator& GridFieldProfile::iterator::operator=(const GridFieldProfile::iterator& src)
{
   my_index = src.my_index;
   my_grid  = src.my_grid;
   
   return *this;
}


/// override operator *, returns a reference to the current data element
real& GridFieldProfile::iterator::operator*() const 
{

   if ( my_index >= 0 && my_index < my_grid->nd ) {
      return (my_grid->dater)[my_index];
   } else {
      throw (baddataindex());
   }
}

/// override operator == ; tests two iterators for equality
bool GridFieldProfile::iterator::operator==(const iterator& x) const
{
   return ( my_index == x.my_index );
}

/// override operator != ; tests two iterators for inequality
bool GridFieldProfile::iterator::operator!=(const iterator& x) const
{
   return ( my_index != x.my_index );
}

/// override operator ++ ; increments the iterator to point to the next data element
GridFieldProfile::iterator& GridFieldProfile::iterator::operator++(int n) 
{
   my_index++;

   return *this;
}

void GridFieldProfile::iterator::assign( real val )
{
  (my_grid->dater)[my_index] = val;
}

void GridFieldProfile::iterator::indices( int* i ) const 
{
    *i = my_index;
}

GridFieldProfile::iterator GridFieldProfile::begin()
{
   
   return GridFieldProfile::iterator( this );

}

GridFieldProfile::iterator GridFieldProfile::end()
{
   
   return GridFieldProfile::iterator( this, nd );

}

 
///////////////////////////////

GridFieldProfile::const_iterator::const_iterator()
{
   my_index = 0;
   my_grid = NULLPTR;
}   


GridFieldProfile::const_iterator::const_iterator( const GridFieldProfile* grid, int n )
{
   my_index = n;
   my_grid = grid;
}

/// override operator =
GridFieldProfile::const_iterator& GridFieldProfile::const_iterator::operator=(const GridFieldProfile::const_iterator& src)
{
   my_index = src.my_index;
   my_grid  = src.my_grid;

   return *this;
}


/// override operator *, returns the current data element
real GridFieldProfile::const_iterator::operator*() const 
{

   if ( my_index >= 0 && my_index < my_grid->nd ) {
      return (my_grid->dater)[my_index];
   } else {
      throw (baddataindex());
   }
}

/// override operator == ; tests two const_iterators for equality
bool GridFieldProfile::const_iterator::operator==(const const_iterator& x) const
{
   return ( my_index == x.my_index );
}

/// override operator != ; tests two const_iterators for inequality
bool GridFieldProfile::const_iterator::operator!=(const const_iterator& x) const
{
   return ( my_index != x.my_index );
}

void GridFieldProfile::const_iterator::indices( int* i ) const 
{
    *i = my_index;
}

/// override operator ++ ; increments the const_iterator to point ot the next data element
GridFieldProfile::const_iterator& GridFieldProfile::const_iterator::operator++(int n) 
{
   my_index++;

   return *this;
}

GridFieldProfile::const_iterator GridFieldProfile::begin() const
{
   
   return GridFieldProfile::const_iterator( this );

}

GridFieldProfile::const_iterator GridFieldProfile::end() const
{
   
   return GridFieldProfile::const_iterator( this, nd );

}


void GridFieldProfile::setPgroup( ProcessGrp* pg, int met)
{
       GridField::setPgroup( pg, met );
       zs.setPgroup( pg, met );

}

void GridFieldProfile::absorb( int n, real* vals, real* dvals )
{
     if ( dvals != NULLPTR ) {
        zs.absorb( n, dvals );
     }
          
     clearData();
     
     nd = n;
     dater = vals;
     
     clear_nodata();
}

void GridFieldProfile::absorbZs( int n, real* dvals )
{
     zs.absorb( n, dvals );     
}



namespace gigatraj {

// outputs the Met Field 
std::ostream& operator<<( std::ostream& os, const GridFieldProfile& p)
{
  
  try {
  
      p.serialize(os);
  

  } catch (...) {
      throw;
  }    
  
  return os;

};

// inputs the Met field
std::istream& operator>>( std::istream& is, GridFieldProfile& p)
{

   try {
   
       p.deserialize(is);

   } catch (...) {
       throw;
   }    
   
   return is;

};

}

 
