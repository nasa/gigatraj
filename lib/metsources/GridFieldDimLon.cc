
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

#include "gigatraj/GridFieldDimLon.hh"

using namespace gigatraj;


const std::string GridFieldDimLon::iam = "GridFieldDimLon";


// constructor
GridFieldDimLon::GridFieldDimLon() : GridFieldDim()
{
   
   wraps = false;
   
}


// destructor
GridFieldDimLon::~GridFieldDimLon()
{
};

// copy constructor
GridFieldDimLon::GridFieldDimLon(const GridFieldDimLon& src) : GridFieldDim(src)
{

   wraps = src.wraps;

}

// copy assignment
GridFieldDimLon& GridFieldDimLon::operator=(const GridFieldDimLon& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }

    this->assign( src ) ;
    
    return *this;
}


void GridFieldDimLon::assign( const GridFieldDimLon& src )
{
   
   GridFieldDim::assign(src);
   
   wraps = src.wraps;

}

void GridFieldDimLon::clear() {
   
   GridFieldDim::clear();

   wraps = false;
      
}

void GridFieldDimLon::wrapping( bool wrp )
{
     wraps = wrp;
}

void GridFieldDimLon::index( real z, int* i1, int* i2 ) const
{
     int i;
     
     if ( nzs <= 1 ) {
        throw (baddataindex());
     }
     
     if ( wraps ) {
        z = wrap( z );
     }

     if ( zdir > 0 ) {
        // levels increase

        // find the highest-indexed z that is below the test z
        i = 0;
        *i1 = -1;
        while ( i < nzs && dater[i] < z ) {
        
           if ( dater[i] < z ) {
              *i1 = i;
           } 
           i++;
        }
        if ( *i1 != -1 ) {
           // we have a lower bound for the test z in zs
        
           // do we have an upper bound within the index range?
           if ( *i1 < (nzs - 1) ) {
              // yes,. use that.
              *i2 = *i1 + 1;
           } else {
              // upper indexd is not in the allowed index range
              
              // we want to handle the special case in which the final
              // value is pretty much the value we are looking for.
              if ( ABS( dater[nzs-1] - z ) > 0.0001 ) {
                 // not the value we are looking for
                 // what we do next depends on whether we are wrapping
                 if ( wraps ) {
                    // set the index to a value that is just beyond the allowed range.
                    // note, though, that we will need to call iwrap() on this
                    //before ever using it.
                    *i2 = nzs;
                 } else {    
                    // not wrapping
                    // defini9tely trying to accfess a value that is
                    // outside our range
                    throw (baddataindex());
                 }
              } else {
                 // ok, so our desired value is pretty much the same
                 // as the final longitude value. 
                    // yes, it's close enough
                    // set both indices
                    *i2 = nzs - 1;
                    *i1 = *i2 - 1;
              }      
           }   
        } else {
           if ( ! wraps ) {
              if ( ABS( dater[0] - z ) > 0.0001 ) {
                 // test z lies below the range of zs
                 throw (baddataindex());
              } else {
                 *i1 = 0;
                 *i2 = 1;
              }   
           }  else {
              *i1 = 0;
              *i2 = 1;
           } 
        }   
     } else {
        // levels decrease
        
        // do everything we do above, except backwards

        // find the lowest-indexed z that is below the test z
        i = nzs - 1;
        *i2 = -1;
        while ( i >= 0 && dater[i] < z ) {
        
           if ( dater[i] < z ) {
              *i2 = i;
           } 
        
           i--;
        }
        if ( *i2 != -1 ) {
           // we have an upper bound for the test z in zs
        
           // do we have an lower bound?
           if ( *i2 > 0 ) {
              // then set the lower bound
              *i1 = *i2 - 1;
           } else {
              // handle the special case for which the test vlaue is at the beginning
              if ( ABS( dater[0] - z ) > 0.0001 ) {
                 // the regular case
                 if ( wraps ) {
                    // this value is useable only if we call iwrap() for it
                    *i1 = -1;
                 } else {
                    // test val lies outside the allowed range of longitudes
                    throw (baddataindex());
                 }
              } else {
                 // the special case
                 *i1 = 0;
                 *i2 = 1;
              }   
           }   
        } else {
              if ( ! wraps ) {
                 if ( ABS( dater[nzs-1] - z ) > 0.0001 ) {
                    // test z lies outside the range of zs
                    throw (baddataindex());
                 } else {
                    *i1 = nzs-2;
                    *i2 = nzs-1;
                 }   
              } else {
                 *i1 = 0;
                 *i2 = -1;
              }  
        }
     
     }

};


void GridFieldDimLon::setWraps( const int loadFlags )
{
   real dz;
   real past_end;
   
   // set the wrap flag
   if ( loadFlags & ( GFL_WRAP | GFL_NOWRAP ) ) {
      // the wrap flag was specificed explicitly
      if ( loadFlags & GFL_NOWRAP ) {
         wraps = false;
      }   
      if ( loadFlags & GFL_WRAP ) {
         wraps = true;
      }
   } else {
      // wrap flag was not specified explicitly.
      // guess it from the longitude values
      
      // find the delta between longitudes
      dz = dater[1] - dater[0];
      // use the delta to go oine past the last longuitd,e and then wrap around
      if ( dz > 0 ) {
         past_end =  dater[nzs-1] + dz - 360.0;
      } else {
         past_end =  dater[nzs-1] + dz + 360.0;      
      }
      // now see if the past_end value is much different from the first value
      // Note: it does not have to be exact, and the values do not have to
      // be regularly-spaced. All we are testing for is
      // whether they are close enough that we can consider the values
      // to cover the domain
      
      wraps =  ( ABS( past_end - dater[0] ) <  ABS(dz)/4.0 );
   
   }
}


void GridFieldDimLon::setDir( const int loadFlags )
{
     zdir = 0;
     
     if ( nzs > 1 ) {

        zdir = ( dater[1] > dater[0] );
     
        setWraps( loadFlags );

     }
     
}



void GridFieldDimLon::gridpoints( int n, int* is, real* vals, int flags ) const
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

void GridFieldDimLon::receive_meta()
{
     real* dimvals;
     int cmd;
     int i;

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
     
         // receive metadata
         //- std::cerr << "   GridFieldDimLon::receive_meta: r-100 with " << metproc << std::endl;
         pgroup->receive_string( metproc, &quant, PGR_TAG_GMETA ); // quantity
         //- std::cerr << "   GridFieldDimLon::receive_meta: r-110 from " << metproc  << std::endl;
         pgroup->receive_string( metproc, &uu, PGR_TAG_GMETA );  // units
         //- std::cerr << "   GridFieldDimLon::receive_meta: r-170 from " << metproc  << std::endl;
         pgroup->receive_ints( metproc, 1, &nzs, PGR_TAG_GMETA );  // number of lats
         //- std::cerr << "   GridFieldDimLon::receive_meta: r-200 from " << metproc  << std::endl;

         set_nodata();  // there are no data
   
         // get the coordinates
         flushData();
         nd = nzs;
         try {
            dater = new real[nd];
         } catch(...) {
            throw (badmemreq());
         }
         pgroup->receive_reals( metproc, nzs, dater, PGR_TAG_GDIMS ); 

         setDir();
         clear_nodata();

         metaID = 0;
   
     }

}

void GridFieldDimLon::svr_send_meta(int id) const
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
         //- std::cerr << "   GridFieldDimLon::svr_send_meta: s-100 with " << id << std::endl;
         pgroup->send_string( id, quant, PGR_TAG_GMETA ); // quantity
         //- std::cerr << "   GridFieldDimLon::svr_send_meta: s-110 to " << id << std::endl;
         pgroup->send_string( id, uu, PGR_TAG_GMETA );  // units
         //- std::cerr << "   GridFieldDimLon::svr_send_meta: s-160 to " << id << std::endl;
         pgroup->send_ints( id, 1, &nzs, PGR_TAG_GMETA );  // number of lons
         //- std::cerr << "   GridFieldDimLon::svr_send_meta: s-200 to " << id << std::endl;

         // send the coordinates
         pgroup->send_reals( id, nd, dater, PGR_TAG_GDIMS ); 
   
     }

}


void GridFieldDimLon::serialize(std::ostream& os) const
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

      GridFieldDim::serialize(os);
        
      // output the version
      os.write( reinterpret_cast<char *>( &version), static_cast<std::streamsize>( sizeof(int)));
      
      // output the wrapping
      ival = 0;
      if ( wraps ) {
         ival = 1;
      }
      os.write( reinterpret_cast<char *>( &ival), static_cast<std::streamsize>( sizeof(int)));
  

  } catch (...) {
      throw;
  }    

}

void GridFieldDimLon::deserialize(std::istream& is)
{
   string str;
   int len;
   char cc;
   int i;
   double t;
   real val;
   int ival;
   int nxzs;
   std::vector<real> xzs, xdata;
   std::vector<real> dat;
   int version;
   
   clear();

   try {
        
      GridFieldDim::deserialize(is);
   
       // read the version
       is.read(reinterpret_cast<char *>( &version), static_cast<std::streamsize>( sizeof(int)));
          
       // read the wrapping flag
       is.read( reinterpret_cast<char *>(&ival), static_cast<std::streamsize>( sizeof(int)) );
       wraps = ( ival != 0);
       
   } catch (...) {
       throw;
   }    

}

void GridFieldDimLon::svr_send_vals( int id ) const
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

real GridFieldDimLon::wrap( real val ) const{
     real extval;
     
     if ( nd <= 1 ) {
        throw (baddataindex());
     }
     
     real dat0 = dater[0];
     
     if ( zdir > 0 ) {
     
        // calculate the max valgitude
        extval = dat0 + 360.0;
     
        // valgitudes increase with index
        while ( val < dat0 ) {
           val = val + 360.0;
        }
        while ( val > extval ) {
           val = val - 360.0;
        } 
        // in case we went one notch too far       
        if ( val < dat0 ) {
           val = val + 360.0;
        }
     } else {
        // calculate the min valgitude
        extval = dat0 - 360.0;
     
        // valgitudes decrease with index
        while ( val > dat0 ) {
           val = val - 360.0;
        }
        while ( val < extval ) {
           val = val + 360.0;
        }        
        if ( val > dat0 ) {
           val = val - 360.0;
        }
     }
     
     return val;

}

int GridFieldDimLon::iwrap( int i ) const
{
     
     if ( nd <= 1 ) {
        throw (baddataindex());
     }
     
     if ( wraps ) {
        while ( i < 0 ) {
           i = i + nd;
        }
        while ( i >= nd ) {
           i = i - nd;
        } 
     } else {
        if ( i < 0 || i >= nd ) {
           throw (baddataindex());
        }
     }
     
     return i;

};

 
