
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
#include <time.h>

#include "gigatraj/GridField.hh"

using namespace gigatraj;

// constuctor
GridField::GridField() {

   quant = "none";
   uu = "N/A";
   ctime = "invalid";
   mtime = -1.0;
   fill_value = -1.0;
   flags = 0;
   set_nodata();
   set_cacheable();
   pgroup = NULLPTR;
   metproc = -1;
   metaID = 0; 
   mksScale = 1.0;
   mksOffset = 0.0;
   expiration = 0;
   
   use_array = false;
   nd = 0;
   dater = NULLPTR;
   
   //std::cerr << "+++ creating GridField object" << std::endl;

};

// destructor
GridField::~GridField()
{
    attrs.clear();
    
    if ( dater != NULLPTR ) {
       delete[] dater;
       dater = NULLPTR;
    } else {
    int junk1 = 10;
    }
    //std::cerr << "--- destroying GridField object" << std::endl;

};

// copy constructor
GridField::GridField(const GridField& src)
{
      // note: we could call assign() here but
      // we don't, to maintain parallelism with the
      // child class copy constructors, which
      // don't use the assign() method.
        quant = src.quantity();
           uu = src.units();
        ctime = src.met_time();
        mtime = src.time();
   fill_value = src.fillval();
       pgroup = src.pgroup;
      metproc = src.metproc;
        flags = src.flags;
       metaID = src.metaID;
     mksScale = src.mksScale;
    mksOffset = src.mksOffset;
   expiration = src.expiration;
        attrs = src.attrs;
    use_array = src.use_array;    
           nd = 0;
        dater = NULLPTR;
      
   //std::cerr << "+++ copy-constructing GridField object" << std::endl;

    // copy only if we have data
    if ( use_array ) {
       flushData();
       nd = src.nd;
       if ( nd > 0 ) {
          // note thet flushDAta() call above ensures dater is a null pointer
          dater = new real[nd];
          for ( int i=0; i < nd; i++ ) {
             dater[i] = src.dater[i];
          }
          clear_nodata();
       }
    } else { 
       data.clear();
       if ( src.data.size() > 0 ) {
            data = src.dump();
       }
       dater = NULLPTR;
    }

}

void GridField::set_fillval(const real val)
{
    int i;
     
    // replace all current fill values with the new fill value
    if ( use_array ) {
       for ( i=0; i<nd; i++) {
           if ( dater[i] == fill_value ) {
              dater[i] = val;
           }
       }
    } else {
       for ( i=0; i<data.size(); i++) {
           if ( data[i] == fill_value ) {
              data[i] = val;
           }
       }
    }
    
    // store the new fill value       
    fill_value = val;

}

void GridField::assign( const GridField& src)
{
        quant = src.quant;
           uu = src.uu;
        ctime = src.ctime;
        mtime = src.mtime;
   fill_value = src.fill_value;
       pgroup = src.pgroup;
      metproc = src.metproc;
        flags = src.flags;
       metaID = src.metaID;
     mksScale = src.mksScale;
    mksOffset = src.mksOffset;
   expiration = src.expiration;
        attrs = src.attrs;
    use_array = src.use_array;    

    if ( use_array ) {
       flushData();
       nd = src.nd;
       if ( nd > 0 ) {
          if ( dater != NULLPTR ) {
             delete[] dater;
          }
          dater = new real[nd];
          for ( int i=0; i < nd; i++ ) {
             dater[i] = src.dater[i];
          }
          clear_nodata();
       }
    } else { 
       data.clear();
       if ( src.data.size() > 0 ) {
            data = src.dump();
       }
       dater = NULLPTR;
       nd = 0;
    }

}

void GridField::flushData()
{
     if ( use_array ) {
        if ( dater != NULLPTR ) {
           delete[] dater;
           dater = NULLPTR;
        }
        nd = 0;
     } else {
        data.clear();
     }
     flags = 0;
     set_nodata();
     set_cacheable();
}
   
void GridField::setPgroup( ProcessGrp* pg, int met) 
{
    pgroup = pg;
    metproc = -1;
    if ( pg != NULLPTR ) {
       // we set the met processor only if the size of the group > 1
       if ( pgroup->size() > 1 ) {
          metproc = met;
       } 
    }   

};

void GridField::ask_for_data( int local ) const
{
     real* dimvals;
     int cmd;
     int i;

     if (  pgroup == NULLPTR || metproc < 0 || local != 0 ) {
         // serial processing.  Load nothing, but
         // check that the object has valid data
         if ( this->status() & (GFS_NODATA) ) {
            throw (baddataload());
         }
         
     } else {
     
         // Never ask a dedicated met processor to fetch metadata for itself
         if ( metproc == pgroup->id() ) {
            throw (badProcReq());
         }
     
         // send "need_met data" status to central met reader process
         cmd = PGR_CMD_GDATA;
         // send request for metadata
         //- std::cerr << "sending GDATA for ask_for-data()  to " << metproc << std::endl;
         pgroup->send_ints( metproc, 1, &cmd, PGR_TAG_GREQ );
         
     }

}



int GridField::svr_start() const 
{
     // pgroup->sync();

     if ( metproc >= 0 && pgroup->id() != metproc ) {
         // This is NOT a dedicated met processor;
         // return 1 so that the caller can know if 
         // data access statements should be executed
         return 1;
     } else {
         // This is a dedicated met processor
         
         // listen for requests from other processors for our data
         svr_listen();
         
         // and return 0, so that the caller can know that
         // data access statements are NOT to be executed
         return 0;
     }
     
}

void GridField::sync() const
{
   if ( pgroup!= NULLPTR ) {
      //- std::cerr << "got to synch point" << std::endl;
      //pgroup->sync("GridFiueld::sync");
      pgroup->sync();
      //- std::cerr << " past synch point" << std::endl;    
   }

}

void GridField::sync(const std::string msg) const
{
   if ( pgroup!= NULLPTR ) {
      //- std::cerr << "got to synch point" << std::endl;
      //pgroup->sync("GridField::sync" + msg);
      pgroup->sync("GridField::sync" + msg);
      //- std::cerr << " past synch point" << std::endl;    
   }

}

void GridField::svr_done() const
{
   int cmd;

   if ( pgroup!= NULLPTR && metproc >= 0 && pgroup->id() != metproc ) {
      cmd = PGR_CMD_GDONE;
      //- std::cerr << "sending server-done " << cmd << " to " << metproc << std::endl;      
      pgroup->send_ints( metproc, 1, &cmd, PGR_TAG_GREQ );
   }
   
   //pgroup->sync();

}

void GridField::svr_listen( int client ) const
{

    int done_goal;
    int done_count;
    const GridField *grid;
    int client_cmd;
    int src;
    
    grid = this;
    
    if ( pgroup->id() == metproc ) {
       // This is the central met processor
       // listen to and satisfy data requests from other processors
       
       // how many processors do we need to tell us we are done?
       // (all except this one)
       if ( client < 0 ) {
          // listening to everyone, so they all have to drop out
          // before we are done
          done_goal = pgroup->size() - 1;       
       } else {
          // listening to only one client, so only that
          // one has to drop out for us to be done.
          done_goal = 1;
       }
       done_count = 0;
      
       while ( done_count < done_goal ) {
          // receive a signal from the client (or possibly any processor in this group)
          //- std::cerr << "svr_listen [" << pgroup->id() << "]"  << " listening for  cmd from any proc "  << std::endl; 
          pgroup->receive_ints( client, 1, &client_cmd, PGR_TAG_GREQ, &src );
          //- std::cerr << "svr_listen [" << pgroup->id() << "]"  << " got cmd " << client_cmd << " from proc " << src  << std::endl; 
          switch (client_cmd) {
          case PGR_CMD_GDONE: // that client processor is finished making requests
             done_count++;
             //- std::cerr << "svr_listen [" << pgroup->id() << "]" << " proc " << src << " is done" << std::endl; 
             break;
          case PGR_CMD_GDATA: // that client processor is making a data request
             // process a met data request from that processor
             // =========================  method send_vals
             //- std::cerr << "svr_listen [" << pgroup->id() << "]" << " about to send values to " << src  << std::endl; 
             grid->svr_send_vals(src);
             //- std::cerr << "svr_listen [" << pgroup->id() << "]" << " sent values to " << src  << std::endl; 
             break;
          case PGR_CMD_GMETA: // that client processor is making a metadata request
             // =========================  method send_meta
             grid->svr_send_meta(src);
             //- std::cerr << "svr_listen [" << pgroup->id() << "]" << " sent metadata to " << src  << std::endl; 
             break;
          }
       } 
    }  
    
    //pgroup->sync();
    
    return; 


}


void GridField::ask_for_meta()
{
     real* dimvals;
     int cmd;
     int i;

     if (  pgroup == NULLPTR || metproc < 0 ) {

         // serial processing.  Load nothing, but
         // check that the object has valid metadata
         if ( this->status() & (GFS_NOQUANT | GFS_NOTIME | GFS_NODIMS)  ) {
            throw (baddataload());
         }
         
     } else {
     
         // Never ask a dedicated met processor to fetch metadata for itself
         if ( metproc == pgroup->id() ) {
            throw (badProcReq());
         }
     
         // send "need Sfc metadata" status to central met reader process
         cmd = PGR_CMD_GMETA;
         // send request for metadata
         //-std::cerr << "   GridField::ask for_meta:  (met client) sending cmd " << std::endl;
         pgroup->send_ints( metproc, 1, &cmd, PGR_TAG_GREQ );
         //-std::cerr << "   GridField::ask for_meta:  (met client) sent cmd " << std::endl;
     }

}


void GridField::clear() {
   
   quant = "none";
   uu = "N/A";
   ctime = "invalid";
   mtime = -1.0;
   fill_value = -1.0;
   flags = 0;
   set_nodata();
   set_cacheable();
   // we **don't** clear these two!!
   //pgroup = NULLPTR;
   //metproc = -1;
   metaID = 0; 
   mksScale = 1.0;
   mksOffset = 0.0;
   expiration = 0;
   attrs.clear();
   clearData();  
   
}

void GridField::clearData() {
   data.clear();  
   if ( dater != NULLPTR ) {
      delete[] dater;
      dater = NULLPTR;
   }
   nd = 0;   
   set_nodata();
}


int GridField::status() const
{
    int result = 0;
    
    if ( quant == "none" || uu == "N/A" ) {
       result = result | GFS_NOQUANT;
    }   
    if ( ctime == "invalid" ) {
       result = result | GFS_NOTIME;
    }   
    
    return result;

}

bool GridField::hasdata() const
{
   return ( (flags & GFS_NODATA) == 0 );
}   

std::vector<real> GridField::dump()  const
{ 
   std::vector<real> stuff;
   
   if ( hasdata() ) { 
      if ( use_array ) {
         stuff.resize(nd);
         for ( int i=0; i < nd; i++ ) {
             stuff.push_back( dater[i] );
         }
         
         return stuff;
      } else {
         return data;
      }
   } else {
       std::cerr << "GridField Has no data" << std::endl;
      throw (baddatareq());
   }   

};


void GridField::checkLons( const std::vector<real> lons ) const 
{
    real prevlon;
    real xlon;
    int dir;
    int nlons;
    int i;

    // we need more than 1 point to check:
    if ( lons.size() < 2 ) {
       throw(badcoords());
    }   

    dir = 1;
    if ( lons[1] < lons[0] ) {
       dir = -1;
    }
    nlons = lons.size();
    
    prevlon = lons[0];
    for ( i=1; i < nlons; i++ ) {

        // start with the value as given
        xlon = lons[i];

        // now adjust it if it needs wrapping
        if ( dir > 0 ) {
           // increasing lons
           
           // wrap
           while ( xlon < lons[0] ) {
              xlon = xlon + 360.0;
           }
           while ( xlon > lons[nlons-1] ) {
              xlon = xlon - 360.0;
           } 
           // in case we went one notch too far    
           if ( xlon < lons[0] ) {
              xlon = xlon + 360.0;
           }
           
           // having wrapped around the globe, the
           // longitudes must be monotonically increasing
           if ( xlon <= prevlon ) {
               throw (badcoords());
           }    
           
        } else {
           // decreasing lons
           
           // wrap
           while ( xlon > lons[0] ) {
              xlon = xlon - 360.0;
           }
           while ( xlon < lons[nlons-1] ) {
              xlon = xlon + 360.0;
           }        
           if ( xlon > lons[0] ) {
              xlon = xlon - 360.0;
           }

           // having wrapped around the globe, the
           // longitudes must be monotonically decreasing
           if ( xlon >= prevlon ) {
               throw (badcoords());
           }    
           
        }
        
        prevlon = xlon;
        
    }
}

void GridField::checkLats( const std::vector<real> lats ) const
{
    int dir;
    int i;

    // we need more than 1 point to check:
    if ( lats.size() < 2 ) {
       throw(badcoords());
    }   

    
    dir = 1;
    if ( lats[1] < lats[0] ) {
       dir = -1;
    }
    
    // check zeroeth point for range
    if ( lats[0] < -90.0 || lats[0] > 90.0 ) {
       throw(badcoords());
    }
    // check the rest of the points for range and for monotonicity
    for ( i=1; i < lats.size(); i++ ) {

        // check for range
        if ( lats[i] < -90.0 || lats[i] > 90.0 ) {
           throw(badcoords());
        }   
           
        // check for monotonicity
        if ( dir > 0 && lats[i] <= lats[i-1] ) {
           throw(badcoords());
        }   
        if ( dir < 0 && lats[i] >= lats[i-1] ) {
           throw(badcoords());
        }   
    }
    
    
}

void GridField::checkLevs( const std::vector<real> levs ) const
{
    int dir;
    int i;

    // we need more than 1 point to check:
    if ( levs.size() < 2 ) {
       throw(badcoords());
    }   

    
    dir = 1;
    if ( levs[1] < levs[0] ) {
       dir = -1;
    }
    
    // check all but the first point for monotonicity
    for ( i=1; i < levs.size(); i++ ) {

        if ( dir > 0 && levs[i] <= levs[i-1] ) {
           throw(badcoords());
        }   
        if ( dir < 0 && levs[i] >= levs[i-1] ) {
           throw(badcoords());
        }   
    }
    
    
}


time_t GridField::expires() const
{
    return expiration;
}

void GridField::set_expires( time_t exptime )
{
    expiration = exptime;
}  


void GridField::serialize(std::ostream& os) const
{
  string str;
  double t;
  real val;
  int len;
  int version = 2;
  time_t tyme;
  std::map< std::string, std::string >::const_iterator mapit;
  
  try {
  
      // output the version
      os.write( reinterpret_cast<char *>( &version), static_cast<std::streamsize>( sizeof(int)));
      
      // output the quantity
      len = quant.length();
      os.write( reinterpret_cast<char *>( &len), static_cast<std::streamsize>( sizeof(int)));
      os.write( quant.c_str(), static_cast<std::streamsize>( len*sizeof(char)));

      // output the units
      len = uu.length();
      os.write( reinterpret_cast<char *>( &len), static_cast<std::streamsize>( sizeof(int)));
      os.write( uu.c_str(), static_cast<std::streamsize>( len*sizeof(char)));
      
      // output the MKS transformation parameters
      val = mksScale;
      os.write( reinterpret_cast<char *>( &val), static_cast<std::streamsize>( sizeof(real)));
      val = mksOffset;
      os.write( reinterpret_cast<char *>( &val), static_cast<std::streamsize>( sizeof(real)));

      // output the time stamp
      len = ctime.length();
      os.write( reinterpret_cast<char *>( &len), static_cast<std::streamsize>( sizeof(int)));
      os.write( ctime.c_str(), static_cast<std::streamsize>( len*sizeof(char)));
      // output the time
      t = mtime;
      os.write( reinterpret_cast<char *>( &t), static_cast<std::streamsize>( sizeof(double)));

      // output the expiration
      tyme = expiration;
      os.write( reinterpret_cast<char *>( &tyme), static_cast<std::streamsize>( sizeof(time_t)));
          
      // output the fill value      
      val = fill_value;
      os.write( reinterpret_cast<char *>( &val), static_cast<std::streamsize>( sizeof(real)));

      // output the attributes
      // first the number of attributes
      len = attrs.size();
      os.write( reinterpret_cast<char *>( &len), static_cast<std::streamsize>( sizeof(int)));
      // then each attribute
      for ( mapit=attrs.begin(); mapit!=attrs.end(); mapit++ ) {
          // first the key
          str = (*mapit).first;
          len = str.length();
          os.write( reinterpret_cast<char *>( &len), static_cast<std::streamsize>( sizeof(int)));
          os.write( str.c_str(), static_cast<std::streamsize>( len*sizeof(char)));
          // then the value
          str = (*mapit).second;
          len = str.length();
          os.write( reinterpret_cast<char *>( &len), static_cast<std::streamsize>( sizeof(int)));
          os.write( str.c_str(), static_cast<std::streamsize>( len*sizeof(char)));
      }
      
  } catch (...) {
      throw;
  }    
  
  //return os;


}


void GridField::deserialize(std::istream& is)
{
   string str;
   string key;
   int nattrs;
   int len;
   char cc;
   int i;
   int j;
   double t;
   real val;
   std::vector<real> dat;
   int ival;
   int version;
   time_t tyme;
   std::map< std::string, std::string >::const_iterator mapit;
   

   try {
   
       // read the version
       is.read(reinterpret_cast<char *>( &version), static_cast<std::streamsize>( sizeof(int)));
       
       // read the quantity
       is.read(reinterpret_cast<char *>( &len), static_cast<std::streamsize>( sizeof(int)));
       str = "";
       if ( len >= 0 ) {
          str.clear();
          for ( i=0; i < len; i++ ) {
             is.read( &cc, static_cast<std::streamsize>( sizeof(char)));
             str.push_back( cc );
          }   
       }
       quant = str;

       // read the units
       is.read(reinterpret_cast<char *>( &len), static_cast<std::streamsize>( sizeof(int)));
       str = "";
       if ( len >= 0 ) {
          str.clear();
          for ( i=0; i < len; i++ ) {
             is.read( &cc, static_cast<std::streamsize>( sizeof(char)));
             str.push_back( cc );
          }   
       }
       uu = str;

       // read the mks transform parameters
       is.read( reinterpret_cast<char *>(&mksScale), static_cast<std::streamsize>( sizeof(real)) );
       is.read( reinterpret_cast<char *>(&mksOffset), static_cast<std::streamsize>( sizeof(real)) );

       // read the time stamp and time
       is.read(reinterpret_cast<char *>( &len), static_cast<std::streamsize>( sizeof(int)));
       str = "";
       if ( len >= 0 ) {
          str.clear();
          for ( i=0; i < len; i++ ) {
             is.read( &cc, static_cast<std::streamsize>( sizeof(char)));
             str.push_back( cc );
          }   
       }
       ctime = str;
       is.read( reinterpret_cast<char *>(&mtime), static_cast<std::streamsize>( sizeof(double)) );
       
       if ( version > 1 ) {
          is.read( reinterpret_cast<char *>(&tyme), static_cast<std::streamsize>( sizeof(time_t)) );
          expiration = tyme;
       }
       
       // read the fill value
       is.read( reinterpret_cast<char *>(&fill_value), static_cast<std::streamsize>( sizeof(real)) );

       // read the number of attributes
       is.read( reinterpret_cast<char *>(&nattrs), static_cast<std::streamsize>( sizeof(int)) );
       // for each attribute
       for ( int j=1; j<=nattrs; j++ ) {
          // first read the key string
          is.read(reinterpret_cast<char *>( &len), static_cast<std::streamsize>( sizeof(int)));
          key = "";
          if ( len >= 0 ) {
             key.clear();
             for ( i=0; i < len; i++ ) {
                is.read( &cc, static_cast<std::streamsize>( sizeof(char)));
                key.push_back( cc );
             }   
          }
          // then read the value string
          is.read(reinterpret_cast<char *>( &len), static_cast<std::streamsize>( sizeof(int)));
          str = "";
          if ( len >= 0 ) {
             str.clear();
             for ( i=0; i < len; i++ ) {
                is.read( &cc, static_cast<std::streamsize>( sizeof(char)));
                str.push_back( cc );
             }   
          }
          
          // now store the key-valuepair
          attrs[key] = str;

       }

   } catch (...) {
       throw;
   }    
   
   //return is;


}

void GridField::set_attribute( const std::string key, const std::string value)
{
    
    attrs[key] = value;

}


std::string GridField::attribute( const std::string key ) const
{
    std::map< std::string, std::string >::const_iterator mapit;

    mapit = attrs.find(key);
    if ( mapit != attrs.end() ) {
       return (*mapit).second;
    } else {
       //- std::cerr << "Missing attribute '" << key << "'" << std::endl;
       throw (badMissingAttr());
    }

}

