
#include "config.h"
#include <time.h>
#include <string.h>
#include <sstream>
#include "gigatraj/NetcdfOut.hh"

using namespace gigatraj;

#ifdef USE_DOUBLE
#define NC_REEL NC_DOUBLE
#else
#define NC_REEL NC_FLOAT
#endif

NetcdfOut::NetcdfOut()
{

    is_open = false;

    reset();    
}


NetcdfOut::~NetcdfOut()
{

    if ( is_open ) {
       close();
    }

}

// copy constructor
NetcdfOut::NetcdfOut(const NetcdfOut& src) : ParcelReporter(src)
{
    reset();
    
    dbug = src.dbug;
    
    restore = src.restore;
    paranoyd = src.paranoyd;
    
    maxchunk = src.maxchunk;
    
    fname = src.fname;
    hdr_contents = src.hdr_contents;;
    hdr_contact = src.hdr_contact;
    dir = src.dir;
    
    vcoord = src.vcoord;
    vunits = src.vunits;
    vdesc = src.vdesc;
    vdir = src.vdir;
    vfactor = src.vfactor;
    
    do_si = src.do_si;
    
    tagquant = src.tagquant;
    tagunits = src.tagunits;
    tagdesc = src.tagdesc;
    
    t0 = src.t0;
    tstamp = src.tstamp;
    tyme = src.t0;
    pnum = src.pnum;
    ip = src.ip;
    tnum = src.tnum;
    do_status = src.do_status;
    do_flags = src.do_flags;
    do_tag = src.do_tag;
    do_tstamp = src.do_tstamp;
    do_fstamp = src.do_fstamp;
    
    // time is not transformed
    to = src.to;
    ts = src.ts;
    
    fmtspec = src.fmtspec;
    
    met = src.met;
    
    NaN = src.NaN;
    dNaN = src.dNaN;
    badval = src.badval;
    dbadval = src.dbadval;
        
    tyme = src.tyme;

    vtyp_lon = src.vtyp_lon;
    vtyp_lat = src.vtyp_lat;
    vtyp_z = src.vtyp_z;
    vtyp_status = src.vtyp_status;
    vtyp_flags = src.vtyp_flags;
    vtyp_tag = src.vtyp_tag;    
    vtyp_tstamp = src.vtyp_tstamp;

}

NetcdfOut& NetcdfOut::operator=(const NetcdfOut& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }
    
    this->assign( src ) ;
    
    return *this;
}

void NetcdfOut::assign( const NetcdfOut& src)
{
    reset();
    
    ParcelReporter::assign(src);
    
    dbug = src.dbug;
    
    restore = src.restore;
    paranoyd = src.paranoyd;
    
    maxchunk = src.maxchunk;
    
    fname = src.fname;
    hdr_contents = src.hdr_contents;;
    hdr_contact = src.hdr_contact;
    dir = src.dir;
    
    vcoord = src.vcoord;
    vunits = src.vunits;
    vdesc = src.vdesc;
    vdir = src.vdir;
    vfactor = src.vfactor;
    
    do_si = src.do_si;
    
    tagquant = src.tagquant;
    tagunits = src.tagunits;
    tagdesc = src.tagdesc;
    
    t0 = src.t0;
    tstamp = src.tstamp;
    tyme = src.t0;
    pnum = src.pnum;
    ip = src.ip;
    tnum = src.tnum;
    do_status = src.do_status;
    do_flags = src.do_flags;
    do_tag = src.do_tag;
    do_tstamp = src.do_tstamp;
    do_fstamp = src.do_fstamp;
    
    // time is not transformed
    to = src.to;
    ts = src.ts;
    
    fmtspec = src.fmtspec;
    
    met = src.met;
    
    NaN = src.NaN;
    dNaN = src.dNaN;
    badval = src.badval;
    dbadval = src.dbadval;
        
    tyme = src.tyme;

    vtyp_lon = src.vtyp_lon;
    vtyp_lat = src.vtyp_lat;
    vtyp_z = src.vtyp_z;
    vtyp_status = src.vtyp_status;
    vtyp_flags = src.vtyp_flags;
    vtyp_tag = src.vtyp_tag;    
    vtyp_tstamp = src.vtyp_tstamp;

}


void NetcdfOut::filename( const std::string file )
{

    if ( ! is_open ) {
       fname = file;
    } else {
       throw new badNetcdfTooLate();
    }
    

}

std::string& NetcdfOut::filename()
{
    return fname;
}

void NetcdfOut::resuming( bool res )
{
     restore = res;
}

bool NetcdfOut::resuming() const
{
    return restore;
}
void NetcdfOut::paranoid( bool par )
{
     paranoyd = par;
}

bool NetcdfOut::paranoid() const
{
     return paranoyd;
}

void NetcdfOut::si( bool value )
{
    if ( ! is_open ) {
       do_si = value;
       tweakUnits();
    } else {
       throw new badNetcdfTooLate();
    }
}
       
bool NetcdfOut::si() const
{
     return do_si;
}

void NetcdfOut::vertical( const std::string& vert, const std::string& units, int dir )
{

    if ( ! is_open ) {
       vcoord = vert;
       
       // if this is in the other quantities list, then remove it from there
       delQuantity( vcoord );
       
       vunits = units;
       
       tweakUnits();
       
       vdir = dir;
       // common case we may as well try to handle here
       if ( vcoord == "P" || vunits == "mb" || vunits == "hPa" ) {
          vdir = -1;
       }
       
       
    } else {
       throw new badNetcdfTooLate();
    }

}

void NetcdfOut::tweakUnits() {

    vfactor = 1.0;
    if ( do_si ) {
       if ( vunits == "km" ) {
          vfactor = 1000.0;
          vunits = "m";
       } else if  ( vunits == "hPa" || vunits == "mb" ) {
          vfactor = 100.0;
          vunits = "Pa";
       }
    }

}

std::string& NetcdfOut::vertical()
{
    return vcoord;
}

void NetcdfOut::writeStatus( bool doit )
{
    if ( ! is_open ) {
       do_status = doit;
    } else {
       throw new badNetcdfTooLate();
    }

}

bool NetcdfOut::writeStatus()
{
     return do_status;
}


void NetcdfOut::writeFlags( bool doit )
{
    if ( ! is_open ) {
       do_flags = doit;
    } else {
       throw new badNetcdfTooLate();
    }

}

bool NetcdfOut::writeFlags()
{
     return do_flags;
}


void NetcdfOut::writeTag( bool doit, const std::string& quant, const std::string& units, const std::string& desc )
{

    if ( ! is_open ) {
       do_tag = doit;
       if ( quant != "" ) {
          tagquant = quant;
          if ( desc != "" ) {
             tagdesc = desc;
             if ( units != "" ) {
                tagunits = units;
             }
          }
       }
    } else {
       throw new badNetcdfTooLate();
    }
     
}

bool NetcdfOut::writeTag()
{
     return do_tag;
}

bool NetcdfOut::metaFixed() const
{
     bool result;

     result = is_open;

     return result;
}                  

void NetcdfOut::addQuantity(  const std::string& quantity, const std::string& units, const std::string& desc  )
{

    if ( ! metaFixed() ) {
    
       if ( quantity == vcoord ) {
          // we will not add this quantity if it's already going to be written
          // out as the vertical coordinate.
          return;
       }
       
       for ( std::vector<std::string>::iterator idx = other.begin(); idx != other.end(); idx++ ) {
           if ( *idx == quantity ) {
              // already here. do nothing.
              return;
           }
       }
       
       other.push_back( quantity );
       other_units.push_back( units );
       other_desc.push_back( desc );
       other_factor.push_back( 1.0 );
       vid_other.push_back( -1 );
       vtyp_other.push_back( NC_REEL );

    } else {
       throw new badNetcdfTooLate();
    }
     
}

void NetcdfOut::delQuantity( std::string quantity )
{
    std::vector<std::string>::iterator qidx;
    std::vector<std::string>::iterator uidx;
    std::vector<std::string>::iterator didx;
     
    if ( ! metaFixed() ) {
    
       for ( qidx = other.begin(), uidx = other_units.begin(), didx=other_desc.begin(); 
             qidx != other.end(); 
             qidx++, uidx++, didx++ ) {
           if ( *qidx == quantity ) {
              // found it
              
              // remove this element
              qidx = other.erase( qidx );
              uidx = other_units.erase( uidx );
              didx = other_desc.erase( didx );
              
              // the other elements have not yet been used, so we
              // can just remove the one at the end--they are all the same
              vid_other.pop_back();
              vtyp_other.pop_back();
              
              break;
           }
       }
          
    } else {
       throw new badNetcdfTooLate();
    }
     
}

// should there be some kind of queryQuantity here?

void NetcdfOut::contents( std::string desc )
{
    if ( ! metaFixed() ) {
       hdr_contents = desc;
    } else {
       throw new badNetcdfTooLate();
    }

}

std::string& NetcdfOut::contents()
{
    return hdr_contents;
}

void NetcdfOut::contact( std::string addr )
{
    if ( ! metaFixed() ) {
       hdr_contact = addr;
    } else {
       throw new badNetcdfTooLate();
    }

}

std::string& NetcdfOut::contact()
{
   return hdr_contact;
}

void NetcdfOut::writeTimestamp( bool mode )
{
    if ( ! metaFixed() ) {
       do_tstamp = mode;
    } else {
       throw new badNetcdfTooLate();
    }
}

bool NetcdfOut::writeTimestamp()
{
   return do_tstamp;
}


void NetcdfOut::writeForecaststamp( bool mode )
{
    if ( ! metaFixed() ) {
       do_fstamp = mode;
    } else {
       throw new badNetcdfTooLate();
    }
}

bool NetcdfOut::writeForecaststamp()
{
   return do_fstamp;
}

std::string& NetcdfOut::cal()
{
    return tstamp;
}

double NetcdfOut::time0()
{
    return t0;
}

void NetcdfOut::maxSequence( int n )
{
   if ( n > 0 ) {
      maxchunk = n;
   }

}

int NetcdfOut::maxSequence()
{
    return maxchunk;
}

void NetcdfOut::direction( int mode )
{
    if ( ! metaFixed() ) {
       if ( ( mode == -1) || (mode == 1) ) {
          dir = mode;
       }
    } else {
       throw new badNetcdfTooLate();
    }
}

int NetcdfOut::direction()
{
    return dir;
}


void NetcdfOut::bad( real value )
{
    if ( ! metaFixed() ) {
       badval = value;
       if ( FINITE(value) ) {
          dbadval = value;
       } else {
          dbadval = dNaN;
       }
    } else {
       throw new badNetcdfTooLate();
    }

}

real NetcdfOut::bad()
{
   return badval;
}


bool NetcdfOut::is_root()
{
     ProcessGrp* pgrp;
     bool result;
     
     pgrp = met->getPgroup();
     if ( pgrp != NULLPTR ) { 
        result = pgrp->is_root();
     } else {
        result = true;
     }
     
     return result;
}

void NetcdfOut::clear()
{
    if ( ! metaFixed() ) {
       do_flags = false;
       do_status = false;
       do_tag = false;
       do_tstamp = false;
       do_fstamp = false;
       other.clear();
       other_units.clear();
       other_desc.clear();
       vid_other.clear();
       vtyp_other.clear();

    } else {
       throw new badNetcdfTooLate();
    }
}

void NetcdfOut::reset()
{
    const char *nanstr = "";

    if ( is_open ) {
        close();
    }
    clear();

    dbug = 0;
    
    restore = false;
    paranoyd = false;
    
    maxchunk = 1000;
    
    fname = "";
    hdr_contents = "gigatraj output: air parcle trajectcory histories";
    hdr_contact = "";  // replace this with a gigatraj contact, but NOT a specific human person
    dir = 0;
    
    vcoord = "";
    vunits = "UNKNOWN";
    vdesc = "";
    vdir = 0;
    vfactor = 1.0;
    
    do_si = false;
    
    tagquant = "";
    tagunits = "";
    tagdesc = "";
    
    t0 = 0.0;
    tstamp = "";
    pnum = 0;
    ip = 0;
    remember_ip = 0;
    tnum = 0;
    remember_tnum = 0;
    do_status = false;
    do_flags = false;
    do_tag = false;
    do_tstamp = false;
    do_fstamp = false;
    
    // time is not transformed
    to = 0.0;
    ts = 1.0;
    
    fmtspec = "";
    
    met = NULLPTR;
    
    NaN = RNAN(nanstr);
    dNaN = nan(nanstr);
    badval = NaN;
    dbadval = dNaN;
    
    tyme = dNaN;
    remember_tyme = t0;

    vid_lon = -1;
    vtyp_lon = NC_REEL;

    vid_lat = -1;
    vtyp_lat = NC_REEL;

    vid_z = -1;
    vtyp_z = NC_REEL;

    vid_status = -1;
    vtyp_status = NC_INT;

    vid_flags = -1;
    vtyp_flags = NC_INT;

    vid_tag = -1;
    vtyp_tag = NC_DOUBLE;
    
    vid_tstamp = -1;
    vtyp_tstamp = NC_STRING;
    
    vid_fstamp = -1;
    vtyp_fstamp = NC_DOUBLE;
}

void NetcdfOut::format( std::string fmt )
{
      int i;
      int state;
      std::string ch;
      bool first;
      std::string metfield;
      
      // since we are specifying a format, clear any previously-set flags
      clear();
      
      i = 0;
      state = 0;
      while ( i < fmt.size() ) {
         
         // grab the next character
         ch = fmt.substr(i,1);
      
         switch (state) {
         case 0: // in literal text, not a "%" sequence
             if ( ch == "%" ) {
                
                // and start a new format spec
                metfield = "";
                first = true;

                // start a % sequence
                state = 1;
             }
             break;
         case 1: // just begun a new "%" sequence
      
            if ( ch != "%" ) {
               
               // are we terminating the sequence already with 
               // a recognized format character?
               if ( ch == "t" || ch == "o" || ch == "a" || ch == "v" 
                 || ch == "i" || ch == "c" || ch == "x" ) {
                  
                  // these are all ignored
 
                  // reset the state to 0  
                  state = 0;             

               } else if ( ch == "T" ) {
               
                  do_tstamp = true;
                  
                  // reset the state to 0  
                  state = 0;             

               } else if ( ch == "F" ) {
               
                  do_fstamp = true;
                  
                  // reset the state to 0  
                  state = 0;             

               } else if ( ch == "f" ) {
               
                  do_flags = true;
                  
                  // reset the state to 0  
                  state = 0;             

               } else if ( ch == "s" ) {
               
                  do_status = true;
                  
                  // reset the state to 0  
                  state = 0;             

               } else if ( ch == "g" ) {
                  
                  do_tag = true;
                  
                  // reset the state to 0  
                  state = 0;             

               } else if ( ch == "{"  ) {
               
                  // this must be a met field
                                   
                  state = 10;               
               
               } else {
                  // no, the user is specifying something more
                  
                  // expect digits or a sewuence terminator next
                  state = 2;
                  
               }
               
            } else {
               // this is just a "%%" sequence.
               // ignore it.
               state = 0;
            }
      
            break;
         case 2: // in a digit sequence
            
            if ( ch == "0" || ch == "1" || ch == "2"  || ch == "3" || ch == "4" 
              || ch == "5" || ch == "6" || ch == "7" || ch == "8" || ch == "9" 
              || ch == "." ) {

              // ignore these numeric specifier characters              
            
            } else if ( ch == "t" || ch == "o" || ch == "a" || ch == "v" 
                     || ch == "i" || ch == "c" || ch == "x" ) {
                  // termination of a % sequence
                  
                  // ignore these sequences
                  
                  // on to the next sequence
                  state = 0;
                  
            } else if ( ch == "T" ) {
            
               do_tstamp = true;
               
               // reset the state to 0  
               state = 0;             

            } else if ( ch == "F" ) {
            
               do_fstamp = true;
               
               // reset the state to 0  
               state = 0;             

            } else if ( ch == "f" ) {
            
               do_flags = true;
               
               // reset the state to 0  
               state = 0;             

            } else if ( ch == "s" ) {
            
               do_status = true;
               
               // reset the state to 0  
               state = 0;             

            } else if ( ch == "g" ) {
               
               do_tag = true;
               
               // reset the state to 0  
               state = 0;             
            } else if ( ch == "{"  ) {      
                                            
                  // not quite at termination of a %i,j{field}m sequence
                                                              
                  state = 10;                  
                                            
            } else {
                std::cerr << "Bad format: " << fmt << std::endl;
                throw (NetcdfOut::badNetcdfFormatSpec());   
            }
            break;
         case 10: // in a met field sequence
            
            if ( ch != "}" ) {
               metfield = metfield + ch;         
            } else { 
               state = 11;
            }
            break;

         case 11: // ends a met field sequence

            if ( ch == "m" ) {
               
               addQuantity( metfield );
                           
            } else {          
                std::cerr << "Bad format: " << fmt << std::endl;
                throw (NetcdfOut::badNetcdfFormatSpec());   
            }
            
            // read for the next sequence
            state = 0;

            break;
         }
            
         i++;
      
      }
      
      fmtspec = fmt;
      
}

std::string NetcdfOut::format()
{
    std:string result;
    int nstuff;
    
    if ( fmtspec == "" ) {
       result = "%t %o %a %v";
       if ( do_flags ) {
          result = result + " %f";
       }
       if ( do_status ) {
          result = result + " %s";
       }
       if ( do_tag ) {
          result = result + " %g";
       }
       nstuff = other.size();
       if ( nstuff > 0 ) {
          for ( int i=0; i < nstuff; i++ ) {   
              result = result + " %{" + other[i] + "}m";
          }
       }
    } else {
       result = fmtspec;    
    }
    
    return result;
}

void NetcdfOut::debug( int mode )
{
     dbug = mode;
}

int NetcdfOut::debug()
{
     return dbug;
}

void NetcdfOut::setMet( MetData* metsrc )
{
     met = metsrc;
}

void NetcdfOut::setMet( Parcel* p )
{
    met = p->getMet();
}

MetData* NetcdfOut::getMet()
{
    return met;
}   

void NetcdfOut::init( Parcel *p, unsigned int n)
{
     init( p->getMet(), n );
}

void NetcdfOut::init( MetData *metsrc, unsigned int n)
{
    if ( ! metaFixed() ) {
    
       tyme = dNaN;
       tnum = 0;
       
       dir = 0;
    
       met = metsrc;
     
       if ( vcoord == "" ) {
          vcoord = met->vertical();
          vunits = met->vunits();
          vdesc = vcoord;
       }
       if ( (vunits == "") || (vunits == "UNKNOWN") ) {
          vunits = met->units( vcoord );
       }
     
       if ( do_tag ) {
          if ( tagquant != "" ) {
             tagquant = met->units( tagquant );
          }
       }
       
       if ( n > 0 ) {
          pnum = n;
       }
       
    } else {
       throw new badNetcdfTooLate();
    }
}

void NetcdfOut::setTimeTransform( double scale, double offset )
{
    if ( ! metaFixed() ) {
       ts = scale;
       to = offset;
    } else {
       throw new badNetcdfTooLate();
    }
}

void NetcdfOut::getTimeTransform( double* scale, double* offset )
{
     *scale = ts;
     *offset = to;
}

std::string NetcdfOut::tunits()
{
   std::string result;
   double tm;
   std:string units;
   std::ostringstream oo;
   
   
   
   if ( abs( to - 693596.00 ) < 1e-3 ) {
      // It's the GEOS FP reference time 1-1-1 00:00:00
      result = "1-1-1 00:00:00";
   } else {
      // first, get the model time that corresponds to the
      // zero netcdf time
      tm = (0.0 - to)/ts;
      // now translate this to a calendar timestamp
      result = met->time2Cal( tm ) + ":00";
   }
   // now size up the units
   units = "";
   if ( abs( ts - 1.0 ) < 1e-3 ) {
      // it's in days
      units = "days since ";
   } else if ( abs( ts - 24.0 ) < 1e-3 ) {
      // it's in hours
      units = "hours since ";      
   } else if ( abs( ts - 24.0*60.0 ) < 1e-3 ) {
      // it's in minutes
      units = "minutes since ";         
   } else if ( abs( ts - 24.0*60.0*60.0 ) < 1e-3 ) {
      // it's in seconds
      units = "seconds since ";         
   } else {
      
      oo.setf( std::ios::fixed );
      oo.width( 14 );
      oo.precision( 10 );
      oo << 1.0/ts;
      
      units = oo.str() + " days since ";
   }
   
   result = units + result; 
   
   return result;
}

void NetcdfOut::open( std::string file, Parcel* p, unsigned int n )
{
     if ( ! restore ) {
        // we create a new file, overwriting any old file,
        // if we are not restoring.
        newopen( file, p, n );
     } else {
        // we are restoing, but we open the file here
        // only if we are not being paranoid.
        // if we are paranoid, then
        // we will reopen and close for each apply().
        if ( ! paranoyd ) {
           reopen( file, p, n );
        } else {
           // we will need to remember file, p, and n for later.
           fakeReopen( file, p, n );
        }    
     }

}

void NetcdfOut::newopen( std::string file, Parcel* p, unsigned int n )
{
     int err;
     std::string aname;
     std::string val;
     const char *aval;
     time_t t;
     int dims[2];
     float fval;
     double dval;
     int ival;
     const char* nanstr = "";
     int vid;
     size_t put_count;
     size_t put_start;
     ptrdiff_t put_stride;
     char *xstamp;
     struct tm *tm;
     std::string *tst1;
     ProcessGrp *pgrp;
     bool i_am_root;

     if ( is_open ) {
        close();
     }

     if ( file != "" ) {
        filename( file );
     }

     if ( dbug > 1 ) {
        std::cerr << "NetcdfOut::newopen: Trying to open " << fname <<  std::endl;
     }

     
     if ( p != NULLPTR ) {
        init(p);
     }

     if ( n > 0 ) {
        pnum = n;
     }

     i_am_root = is_root();
     
     if ( i_am_root ) {
        err = nc_create( fname.c_str(), NC_CLOBBER | NC_NETCDF4, &ncid);
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     
     is_open = true;

     // write Contents global attribute
     aname = "Contents";
     aval = hdr_contents.c_str();
     if ( i_am_root ) {
        err = nc_put_att_string( ncid, NC_GLOBAL, aname.c_str(), 1, &aval );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     
     // write Creation_date global attribute
     aname = "Creation_date";
     // get the time
     t = time(NULL);
     // translate it to GMT in a time strcuture
     tm = gmtime( &t );
     // turn the time structure into a string
     tst1 = new std::string( asctime(tm)  );
     // replace the newline at the end with a space
     tst1->replace( tst1->size() - 1, 1, " " );
     // add the time zone at the end
     *tst1 = *tst1 + " GMT";
     // convert to a C string
     aval = tst1->c_str();
     if ( i_am_root ) {
        err = nc_put_att_string( ncid, NC_GLOBAL, aname.c_str(), 1, &aval );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     delete tst1;
     
     // write Contact global attribute
     if ( hdr_contact != "" ) {
        aname = "Contact";
        aval = hdr_contact.c_str();
        if ( i_am_root ) {
           err = nc_put_att_string( ncid, NC_GLOBAL, aname.c_str(), 1, &aval );
           if ( err != NC_NOERR ) {
              throw(badNetcdfError(err));
           }
        }
     }

     // write Trajectory_direction global attribute
     // (but only if dir != 0; otherwise it is determined dynamically and written by writeout()
     if ( dir == 1 ) {
        aname = "Trajectory_direction";
        val = "fwd";
        aval = val.c_str();
        if ( i_am_root ) {
           err = nc_put_att_string( ncid, NC_GLOBAL, aname.c_str(), 1, &aval );
           if ( err != NC_NOERR ) {
              throw(badNetcdfError(err));
           }        
        }        
     } else if ( dir == -1 ) {
        aname = "Trajectory_direction";
        val = "bck";
        aval = val.c_str();
        if ( i_am_root ) {
           err = nc_put_att_string( ncid, NC_GLOBAL, aname.c_str(), 1, &aval );
           if ( err != NC_NOERR ) {
              throw(badNetcdfError(err));
           }        
        }        
     
     }

     // write Trajectory_start global attribute
     aname = "Trajectory_start";
     if ( (tstamp == "") && (met != NULLPTR) ) {
        tstamp = met->time2Cal( t0 );
     }
     aval = tstamp.c_str();
     if ( i_am_root ) {
        err = nc_put_att_string( ncid, NC_GLOBAL, aname.c_str(), 1, &aval );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     
     // define time dimension unbounded
     val = "time";
     if ( i_am_root ) {
        err = nc_def_dim( ncid, val.c_str(), NC_UNLIMITED, &did_time );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }

     // define id dimension unbounded
     val = "id";
     if ( i_am_root ) {
        err = nc_def_dim( ncid, val.c_str(), pnum, &did_id );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     
     //// define time variable
     val = "time";
     if ( i_am_root ) {
        err = nc_def_var( ncid, val.c_str(), NC_DOUBLE, 1, &did_time, &vid_time );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
        err = nc_def_var_fill( ncid, vid_time, NC_NOFILL, NULL );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     if ( dbug > 50 ) {
        std::cerr << "defined time variable w/ id = " << vid_time << std::endl;
     }
     // write long_name time attribute
     aname = "long_name";
     //val = tunits(); 
     val = "time"; 
     aval = val.c_str();
     if ( i_am_root ) {
        err = nc_put_att_string( ncid, vid_time, aname.c_str(), 1, &aval );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     // write standard_name time attribute
     aname = "standard_name";
     val = "time"; 
     aval = val.c_str();
     if ( i_am_root ) {
        err = nc_put_att_string( ncid, vid_time, aname.c_str(), 1, &aval );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     // write units time attrbiute
     aname = "units";
     //val = "day"; 
     val = tunits(); 
     aval = val.c_str();
     if ( i_am_root ) {
        err = nc_put_att_string( ncid, vid_time, aname.c_str(), 1, &aval );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     
     
     //// define id variable
     val = "id";
     if ( i_am_root ) {
        err = nc_def_var( ncid, val.c_str(), NC_DOUBLE, 1, &did_id, &vid_id );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     if ( dbug > 50 ) {
        std::cerr << "defined id variable w/ id = " << vid_id << std::endl;
     }
     // write long_name id attribute
     aname = "long_name";
     val = "parcel id"; 
     aval = val.c_str();
     if ( i_am_root ) {
        err = nc_put_att_string( ncid, vid_id, aname.c_str(), 1, &aval );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     // write units id attrbiute
     aname = "units";
     val = "1"; 
     aval = val.c_str();
     if ( i_am_root ) {
        err = nc_put_att_string( ncid, vid_id, aname.c_str(), 1, &aval );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     
     // set up dinensions for the regular variables
     dims[0] = did_time;
     dims[1] = did_id;
     
     //// define lon variable
     val = "lon";
     if ( i_am_root ) {
        err = nc_def_var( ncid, val.c_str(), NC_REEL, 2, dims, &vid_lon );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     if ( dbug > 50 ) {
        std::cerr << "defined lon variable w/ id = " << vid_lon << std::endl;
     }
     // define missing_value attribute
     if ( i_am_root ) {
        err = nc_def_var_fill( ncid, vid_lon, NC_FILL, &badval );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     aname = "missing_value";
     if ( i_am_root ) {
#ifdef USE_DOUBLE    
        err = nc_put_att_double( ncid, vid_lon, aname.c_str(), NC_DOUBLE, 1, &badval );
#else
        err = nc_put_att_float( ncid, vid_lon, aname.c_str(), NC_FLOAT, 1, &badval );
#endif
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     // define _FillValue attribute
     aname = "_FillValue";
     if ( i_am_root ) {
#ifdef USE_DOUBLE    
        err = nc_put_att_double( ncid, vid_lon, aname.c_str(), NC_DOUBLE, 1, &badval );
#else
        err = nc_put_att_float( ncid, vid_lon, aname.c_str(), NC_FLOAT, 1, &badval );
#endif
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     // define long_name attribute
     aname = "long_name";
     val = "longitude"; 
     aval = val.c_str();
     if ( i_am_root ) {
        err = nc_put_att_string( ncid, vid_lon, aname.c_str(), 1, &aval );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     // define units attribute
     aname = "units";
     val = "degrees_east"; 
     aval = val.c_str();
     if ( i_am_root ) {
        err = nc_put_att_string( ncid, vid_lon, aname.c_str(), 1, &aval );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     
     //// define lat variable
     val = "lat";
     if ( i_am_root ) {
        err = nc_def_var( ncid, val.c_str(), NC_REEL, 2, dims, &vid_lat );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     if ( dbug > 50 ) {
        std::cerr << "defined lat variable w/ id = " << vid_lat << std::endl;
     }
     // define missing_value attribute
     if ( i_am_root ) {
        err = nc_def_var_fill( ncid, vid_lat, NC_FILL, &badval );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     aname = "missing_value";
     if ( i_am_root ) {
#ifdef USE_DOUBLE    
        err = nc_put_att_double( ncid, vid_lat, aname.c_str(), NC_DOUBLE, 1, &badval );
#else
        err = nc_put_att_float( ncid, vid_lat, aname.c_str(), NC_FLOAT, 1, &badval );
#endif
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     // define _FillValue attribute
     aname = "_FillValue";
     if ( i_am_root ) {
#ifdef USE_DOUBLE    
        err = nc_put_att_double( ncid, vid_lat, aname.c_str(), NC_DOUBLE, 1, &badval );
#else
        err = nc_put_att_float( ncid, vid_lat, aname.c_str(), NC_FLOAT, 1, &badval );
#endif
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     // define long_name attribute
     aname = "long_name";
     val = "latitude"; 
     aval = val.c_str();
     if ( i_am_root ) {
        err = nc_put_att_string( ncid, vid_lat, aname.c_str(), 1, &aval );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     // define units attribute
     aname = "units";
     val = "degrees_north"; 
     aval = val.c_str();
     if ( i_am_root ) {
        err = nc_put_att_string( ncid, vid_lat, aname.c_str(), 1, &aval );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     
     //// define vertical coordinate variable
     val = vcoord;
     if ( i_am_root ) {
        err = nc_def_var( ncid, val.c_str(), NC_REEL, 2, dims, &vid_z );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     if ( dbug > 50 ) {
        std::cerr << "defined vcoord variable w/ id = " << vid_z << std::endl;
     }
     // define missing_value attribute
     if ( i_am_root ) {
        err = nc_def_var_fill( ncid, vid_z, NC_FILL, &badval );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     aname = "missing_value";
     if ( i_am_root ) {
#ifdef USE_DOUBLE    
        err = nc_put_att_double( ncid, vid_z, aname.c_str(), NC_DOUBLE, 1, &badval );
#else
        err = nc_put_att_float( ncid, vid_z, aname.c_str(), NC_FLOAT, 1, &badval );
#endif
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     // define _FillValue attribute
     aname = "_FillValue";
     if ( i_am_root ) {
#ifdef USE_DOUBLE    
        err = nc_put_att_double( ncid, vid_z, aname.c_str(), NC_DOUBLE, 1, &badval );
#else
        err = nc_put_att_float( ncid, vid_z, aname.c_str(), NC_FLOAT, 1, &badval );
#endif
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     // define long_name attribute
     if ( vdesc != "" ) {
        aname = "long_name";
        val = vdesc; 
        aval = val.c_str();
        if ( i_am_root ) {
           err = nc_put_att_string( ncid, vid_z, aname.c_str(), 1, &aval );
           if ( err != NC_NOERR ) {
              throw(badNetcdfError(err));
           }
        }
     }
     // define units attribute
     aname = "units";
     val = vunits; 
     aval = val.c_str();
     if ( i_am_root ) {
        err = nc_put_att_string( ncid, vid_z, aname.c_str(), 1, &aval );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     // define the positive attribute
     aname = "positive";
     val = "up"; 
     // note: this should be determined by the MetData object 
     // but it can be set by the user
     if ( vdir < 0 ) {
        val = "down";
     }
     aval = val.c_str();
     if ( i_am_root ) {
        err = nc_put_att_string( ncid, vid_z, aname.c_str(), 1, &aval );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     //define the vertical_coordinate attribute
     aname = "vertical_coordinate";
     val = "yes"; 
     aval = val.c_str();
     if ( i_am_root ) {
        err = nc_put_att_string( ncid, vid_z, aname.c_str(), 1, &aval );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     
     
     /// define the timestamp variable
     if ( do_tstamp ) {
        val = "timestamp";
        if ( i_am_root ) {
           err = nc_def_var( ncid, val.c_str(), NC_STRING, 1, &did_time, &vid_tstamp );
           if ( err != NC_NOERR ) {
              throw(badNetcdfError(err));
           }
        }
        if ( dbug > 50 ) {
           std::cerr << "defined timestamp variable w/ id = " << vid_tstamp << std::endl;
        }
        // write long_name time attribute
        aname = "long_name";
        val = "Date + time"; 
        aval = val.c_str();
        if ( i_am_root ) {
           err = nc_put_att_string( ncid, vid_tstamp, aname.c_str(), 1, &aval );
           if ( err != NC_NOERR ) {
              throw(badNetcdfError(err));
           }
        }
        // write units time attrbiute
        aname = "units";
        val = ""; 
        aval = val.c_str();
        if ( i_am_root ) {
           err = nc_put_att_string( ncid, vid_tstamp, aname.c_str(), 1, &aval );
           if ( err != NC_NOERR ) {
              throw(badNetcdfError(err));
           }
        }
     }
     
     
     /// define the forecast lead time variable
     if ( do_fstamp ) {
        val = "forecast";
        if ( i_am_root ) {
           err = nc_def_var( ncid, val.c_str(), NC_DOUBLE, 1, &did_time, &vid_fstamp );
           if ( err != NC_NOERR ) {
              throw(badNetcdfError(err));
           }
        }
        if ( dbug > 50 ) {
           std::cerr << "defined forecast variable w/ id = " << vid_fstamp << std::endl;
        }
        // write long_name time attribute
        aname = "long_name";
        val = "Forecast lead time"; 
        aval = val.c_str();
        if ( i_am_root ) {
           err = nc_put_att_string( ncid, vid_fstamp, aname.c_str(), 1, &aval );
           if ( err != NC_NOERR ) {
              throw(badNetcdfError(err));
           }
        }
        // write units time attrbiute
        aname = "units";
        val = tunits();  
        aval = val.c_str();
        if ( i_am_root ) {
           err = nc_put_att_string( ncid, vid_fstamp, aname.c_str(), 1, &aval );
           if ( err != NC_NOERR ) {
              throw(badNetcdfError(err));
           }
        }
     }
     
     if ( do_status ) {
        //// define status variable
        val = "status";
        if ( i_am_root ) {
           err = nc_def_var( ncid, val.c_str(), NC_INT, 2, dims, &vid_status );
           if ( err != NC_NOERR ) {
              throw(badNetcdfError(err));
           }
        }
        if ( dbug > 50 ) {
           std::cerr << "defined status variable w/ id = " << vid_status << std::endl;
        }
        // define long_name attribute
        aname = "long_name";
        val = "parcel bitwise status "; 
        aval = val.c_str();
        if ( i_am_root ) {
           err = nc_put_att_string( ncid, vid_status, aname.c_str(), 1, &aval );
           if ( err != NC_NOERR ) {
              throw(badNetcdfError(err));
           }
        }
        // define units attribute
        aname = "units";
        val = "1"; 
        aval = val.c_str();
        if ( i_am_root ) {
           err = nc_put_att_string( ncid, vid_status, aname.c_str(), 1, &aval );
           if ( err != NC_NOERR ) {
              throw(badNetcdfError(err));
           }
        }
     }
     
     if ( do_flags ) {
        //// define flags variable
        val = "flags";
        if ( i_am_root ) {
           err = nc_def_var( ncid, val.c_str(), NC_INT, 2, dims, &vid_flags);
           if ( err != NC_NOERR ) {
              throw(badNetcdfError(err));
           }
        }
        if ( dbug > 50 ) {
           std::cerr << "defined flags variable w/ id = " << vid_flags << std::endl;
        }
        // define long_name attribute
        aname = "long_name";
        val = "parcel bitwise flags"; 
        aval = val.c_str();
        if ( i_am_root ) {
           err = nc_put_att_string( ncid, vid_flags, aname.c_str(), 1, &aval );
           if ( err != NC_NOERR ) {
              throw(badNetcdfError(err));
           }
        }
        // define units attribute
        aname = "units";
        val = "1"; 
        aval = val.c_str();
        if ( i_am_root ) {
           err = nc_put_att_string( ncid, vid_flags, aname.c_str(), 1, &aval );
           if ( err != NC_NOERR ) {
              throw(badNetcdfError(err));
           }
        }
     }
     
     if ( do_tag ) {
        // define tag variable
        val = "tag";
        if ( i_am_root ) {
           err = nc_def_var( ncid, val.c_str(), NC_DOUBLE, 2, dims, &vid_tag );
           if ( err != NC_NOERR ) {
              throw(badNetcdfError(err));
           }
        }
        if ( dbug > 50 ) {
           std::cerr << "defined tag variable w/ id = " << vid_tag << std::endl;
        }
        // define missing_value attribute       
        if ( i_am_root ) {
           err = nc_def_var_fill( ncid, vid_tag, NC_FILL, &dbadval );
           if ( err != NC_NOERR ) {
              throw(badNetcdfError(err));
           }
        }
        aname = "missing_value";
        if ( i_am_root ) {
           err = nc_put_att_double( ncid, vid_tag, aname.c_str(), NC_DOUBLE, 1, &dbadval );
           if ( err != NC_NOERR ) {
              throw(badNetcdfError(err));
           }
        }
        // define _FillValue attribute
        aname = "_FillValue";
        if ( i_am_root ) {
           err = nc_put_att_double( ncid, vid_tag, aname.c_str(), NC_DOUBLE, 1, &dbadval );
           if ( err != NC_NOERR ) {
              throw(badNetcdfError(err));
           }
        }
        if ( tagdesc != "" ) {
           // define long_name attribute
           aname = "long_name";
           val = tagquant; 
           if ( tagdesc != "" ) {
              val = val + "; " + tagdesc;
           }
           aval = val.c_str();
           if ( i_am_root ) {
              err = nc_put_att_string( ncid, vid_tag, aname.c_str(), 1, &aval );
              if ( err != NC_NOERR ) {
                 throw(badNetcdfError(err));
              }
           }
        }
        if ( tagunits != "" ) {
           // define units attribute
           aname = "units";
           val = tagunits; 
           aval = val.c_str();
           if ( i_am_root ) {
              err = nc_put_att_string( ncid, vid_tag, aname.c_str(), 1, &aval );
              if ( err != NC_NOERR ) {
                 throw(badNetcdfError(err));
              }
           }
        }
     }

     std::string pressure_name;
     std::string palt_name;
     std::string altitude_name;
     std::string theta_name;
     
     bool junk;
     junk = met->getOption("PressureName", pressure_name );
     junk = met->getOption("AltitudeName", altitude_name );
     junk = met->getOption("PressureAltitudeName", palt_name );
     junk = met->getOption("PotentialTemperatureName", theta_name );
     
     for ( int i=0; i < other.size(); i++ ) {
         //// define other variable
         val = other[i];
         if ( i_am_root ) {
            err = nc_def_var( ncid, val.c_str(), NC_REEL, 2, dims, &vid );
            if ( err != NC_NOERR ) {
               throw(badNetcdfError(err));
            }
         }
         vid_other[i] = vid;
         vtyp_other[i] = NC_REEL;
         
         // define missing_value attribute
         if ( i_am_root ) {
            err = nc_def_var_fill( ncid, vid, NC_FILL, &badval );
            if ( err != NC_NOERR ) {
               throw(badNetcdfError(err));
            }
         }
         aname = "missing_value";
         if ( i_am_root ) {
#ifdef USE_DOUBLE    
            err = nc_put_att_double( ncid, vid, aname.c_str(), NC_DOUBLE, 1, &badval );
#else
            err = nc_put_att_float( ncid, vid, aname.c_str(), NC_FLOAT, 1, &badval );
#endif
            if ( err != NC_NOERR ) {
               throw(badNetcdfError(err));
            }
         }
         // define _FillValue attribute
         aname = "_FillValue";
         if ( i_am_root ) {
#ifdef USE_DOUBLE    
            err = nc_put_att_double( ncid, vid, aname.c_str(), NC_DOUBLE, 1, &badval );
#else
            err = nc_put_att_float( ncid, vid, aname.c_str(), NC_FLOAT, 1, &badval );
#endif
            if ( err != NC_NOERR ) {
               throw(badNetcdfError(err));
            }
         }
         val = other_desc[i];
         if ( val != "" ) {
            // define long_name attribute
            aname = "long_name";
            aval = val.c_str();
            if ( i_am_root ) {
               err = nc_put_att_string( ncid, vid, aname.c_str(), 1, &aval );
               if ( err != NC_NOERR ) {
                  throw(badNetcdfError(err));
               }
            }
         }
         val = other_units[i];
         if ( val == "UNKNOWN" ) {
            val = met->units( other[i] );
         }
         if ( val != "UNKNOWN" ) {
            // define units attribute
            if ( do_si ) {
               if ( val == "km" ) {
                  val = "m";
                  other_factor[i] = 1000.0;
               } else if ( val == "hPa" ) {
                  val = "Pa";
                  other_factor[i] = 100.0;
               }
            }
            aname = "units";
            aval = val.c_str();
            if ( i_am_root ) {
               err = nc_put_att_string( ncid, vid, aname.c_str(), 1, &aval );
               if ( err != NC_NOERR ) {
                  throw(badNetcdfError(err));
               }
            }
         }
         std::string qdir = "";
         
         if ( other[i] == pressure_name ) {
            qdir = "down";
         } else if ( (other[i] == altitude_name) 
                  || (other[i] == palt_name) 
                  || (other[i] == theta_name) ) {
         
             qdir = "up";
         
         }  
         
         if ( qdir != "" ) {
            aname = "positive";
            aval = qdir.c_str();
            if ( i_am_root ) {
               err = nc_put_att_string( ncid, vid, aname.c_str(), 1, &aval );
               if ( err != NC_NOERR ) {
                  throw(badNetcdfError(err));
               }
            }
         
         }       
        
     }
     
     
     // end definition mode
     if ( i_am_root ) {
        err = nc_enddef( ncid );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     }
     
     // write id variable values
     put_count = 1;
     put_stride = 1;
     for ( int i=0; i < pnum; i++ ) {
         dval = i;
         put_start = i;
         if ( i_am_root ) {
            err = nc_put_vars_double( ncid, vid_id, &put_start, &put_count, &put_stride, &dval );
            if ( err != NC_NOERR ) {
               throw(badNetcdfError(err));
            }
         }
     }

     if ( dbug > 1 ) {
        std::cerr << "NetcdfOut::newopen: " << fname << " is opened." << std::endl;
     }

     if ( paranoyd ) {
        rclose();
     }

}    


void NetcdfOut::reopen( std::string file, Parcel* p, unsigned int n )
{
     int err;
     std::string aname;
     std::string val;
     char *cval;
     const char *attr_name;
     const char *var_name;
     std::string name;
     nc_type attr_type;
     size_t  attr_size;
     int attr_id;
     int var_id;
     const char *aval;
     nc_type var_type;
     int var_ndims;
     int var_dims[NC_MAX_VAR_DIMS];
     int  var_natts;
     time_t t;
     int dims[2];
     float fval;
     double dval;
     int ival;
     const char* nanstr = "";
     int vid;
     size_t put_count;
     size_t put_start;
     ptrdiff_t put_stride;
     char *xstamp;
     struct tm *tm;
     std::string *tst1;
     ProcessGrp *pgrp;
     std::string traj_start;
     std::string my_traj_start;
     bool i_am_root;
     int ndimens;
     int unlimdim_idx;
     size_t ntimes;
     int time_index;
     size_t istart;
     size_t icount;
     double file_t0;
     double file_t1;
     double tt0;
     size_t time_idx;
     size_t file_pnum;
     int dim_id;
     int file_direction;
     real badlon;
     real badlat;
     real badvert;
     int vtype;
     double t0x;

     if ( is_open ) {
        close();
     }

     if ( file != "" ) {
        filename( file );
     }

     if ( dbug > 1 ) {
        std::cerr << "NetcdfOut::reopen: Trying to open " << fname <<  std::endl;
     }

     i_am_root = is_root();
     
     if ( i_am_root ) {
        err = nc_open( fname.c_str(), NC_WRITE, &ncid);     
        if ( err != NC_NOERR ) {
           if ( err == NC_ENOTFOUND ) {
              std::cerr << "NetcdfOut::reopen: ***WARNING*** output netcdf file  " 
              << fname << " does not exist. Opening as a new file. " << std::endl;
              open( fname, p, n);
              return;
           } else {
              std::cerr << "NetcdfOut::reopen: failed to open file: " << fname << std::endl;
              throw(badNetcdfOpen(err));
           }
        }
     }
     
     is_open = true;

     
     if ( p != NULLPTR ) {
        init(p);
     }

     if ( n > 0 ) {
        pnum = n;
     }
     
     if ( i_am_root ) {

        // read Contents global attribute
        aname = "Contents";
        attr_name = aname.c_str();
        err = nc_inq_attid( ncid, NC_GLOBAL, attr_name, &attr_id );
        if ( err == NC_NOERR ) {
           err = nc_inq_att(ncid, NC_GLOBAL, attr_name, &attr_type, &attr_size );
           if ( err != NC_NOERR ) {
              throw(badNetcdfError(err));
           }
           if ( attr_type == NC_STRING && attr_size == 1 ) {
              
              err = nc_get_att_string( ncid, NC_GLOBAL, attr_name, &cval );
              if ( err != NC_NOERR ) {
                 throw(badNetcdfError(err));
              }

              hdr_contents.assign( cval );
              
              nc_free_string(1, &cval);
     
           }
        }
     
        // leave the Creation_date attribute as-is

        // read Contact global attribute
        aname = "Contact";
        attr_name = aname.c_str();
        err = nc_inq_attid( ncid, NC_GLOBAL, attr_name, &attr_id );
        if ( err == NC_NOERR ) {
           err = nc_inq_att(ncid, NC_GLOBAL, attr_name, &attr_type, &attr_size );
           if ( err != NC_NOERR ) {
              throw(badNetcdfError(err));
           }
           if ( attr_type == NC_STRING && attr_size == 1 ) {
              
              err = nc_get_att_string( ncid, NC_GLOBAL, attr_name, &cval );
              if ( err != NC_NOERR ) {
                 throw(badNetcdfError(err));
              }

              hdr_contact.assign( cval );
              
              nc_free_string(1, &cval);
     
           }
        }
     

        // read the base time global attribute
        aname = "Trajectory_start";
        attr_name = aname.c_str();
        err = nc_inq_attid( ncid, NC_GLOBAL, attr_name, &attr_id );
        if ( err == NC_NOERR ) {
           err = nc_inq_att(ncid, NC_GLOBAL, attr_name, &attr_type, &attr_size );
           if ( err != NC_NOERR ) {
              throw(badNetcdfError(err));
           }
           if ( attr_type == NC_STRING && attr_size == 1 ) {
              
              err = nc_get_att_string( ncid, NC_GLOBAL, attr_name, &cval );
              if ( err != NC_NOERR ) {
                 throw(badNetcdfError(err));
              }

              traj_start.assign( cval );
              
              nc_free_string(1, &cval);
     
           }
           // todo: handle the case where the timestamp is a char array
        }
        t0x = dNaN;
        if (met != NULLPTR) {
           if ( tstamp == "" ) {
              tstamp = met->time2Cal( t0 );
           }
           if ( traj_start != "" ) {
              t0x = met->cal2Time( traj_start );
           }
        }
        if ( dbug > 1 ) {
           std::cerr << "NetcdfOut::reopen: current t0=" << t0 << "(" << tstamp << ")"
                     << "; file t0 = " << t0x << " (" << traj_start << ")" << std::endl;
        }
        
        // get the trajectory direction and ensure we are working in the same direction
        aname = "Trajectory_direction";
        attr_name = aname.c_str();
        err = nc_inq_attid( ncid, NC_GLOBAL, attr_name, &attr_id );
        if ( err == NC_NOERR ) {
           err = nc_inq_att(ncid, NC_GLOBAL, attr_name, &attr_type, &attr_size );
           if ( err != NC_NOERR ) {
              throw(badNetcdfError(err));
           }
           if ( attr_type == NC_STRING && attr_size == 1 ) {
              
              err = nc_get_att_string( ncid, NC_GLOBAL, attr_name, &cval );
              if ( err != NC_NOERR ) {
                 throw(badNetcdfError(err));
              }

              val.assign( cval );
              nc_free_string(1, &cval);
              
              file_direction = 0;
              if ( val == "fwd" ) {
                 file_direction = 1;
              } else if ( val == "bck" ) {
                 file_direction = -1;           
              }
              
              if ( dir != 0 ) {              
                 if ( file_direction != dir ) {
                     std::cerr << " reopened file's trajectory direction is not the same ours " << std::endl;
                     throw(badNetcdfReOpenMismatch());
                 }
              } else {
                 dir = file_direction;
              }
     
           }
        }
     
     
        // do the initial inquiries to get basic sizes and shapes
        err = nc_inq(ncid, &ndimens, &nvars, &ngatts, &unlimdim_idx);
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
        if ( dbug > 2 ) {
           std::cerr << "NetcdfOut::reopen: initial inq: " << ndimens << ", " << nvars << ", " << ngatts << ", " << unlimdim_idx << std::endl;
        }

        // get the dimensional IDs for time and id
        err = nc_inq_dimid( ncid, "time", &did_time);
        if ( err == NC_EBADDIM ) {
           std::cerr << " No 'time' dimension in file" << std::endl;
           throw(badFileConventions());  
        } else if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }     
        if ( did_time != unlimdim_idx ) {
           std::cerr << " reopened file's time dimension index is not the same as its unlimited dinmension index " << std::endl;
           throw(badNetcdfReOpenMismatch());
        }
        err = nc_inq_dimlen( ncid, did_time, &ntimes );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
        if ( dbug > 10 ) {
           std::cerr << "NetcdfOut::open: There are " << ntimes << " times" << std::endl;
        }   
     
        err = nc_inq_dimid( ncid, "id", &did_id);
        if ( err == NC_EBADDIM ) {
           std::cerr << " No 'id' dimension in file" << std::endl;
           throw(badFileConventions());  
        } else if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }

        // set up dimensions for the regular variables
        dims[0] = did_time;
        dims[1] = did_id;
     


        name = "time";
        var_name = name.c_str();
        err = nc_inq_varid( ncid, var_name, &vid_time );
        if ( err == NC_ENOTVAR ) {
           std::cerr << " No 'time' variable in file" << std::endl;
           throw(badFileConventions());
        } else if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
        if ( dbug > 50 ) {
           std::cerr << "read time variable w/ id = " << vid_time << std::endl;
        }

        err = nc_inq_var( ncid, vid_time, NULL, &var_type, &var_ndims, var_dims, &var_natts);
        if ( err != NC_NOERR ) {
           std::cerr << " 'time' variable has wrong type or wrong dimensions" << std::endl;
           throw(badNetcdfError(err));
        }
        if ( dbug > 10 ) {
           std::cerr << "NetcdfOut::reopen: time varid: " << vid_time
                     << "; time type: " << var_type
                     << "; time ndims: " << var_ndims
                     << "; time dims[0]: " << var_dims[0] 
                     << "; time natts: " << var_natts << std::endl;
        }
        if ( (var_type != NC_DOUBLE) || (var_ndims != 1) || (vid_time != var_dims[0]) ) {
           std::cerr << " 'time' variable has wrong type or wrong dimensions" << std::endl;
           throw(badFileConventions());
        }

        time_idx = 0;
        icount = 1;
        if ( ntimes > 0 ) {
           // get the first time from the file
           err = nc_get_vara_double( ncid, vid_time, &time_idx, &icount, &tt0 );
           if ( err == NC_NOERR ) {
              file_t0 = tvnoc( tt0 );
              if ( dbug > 1 ) {
                 std::cerr << "NetcdfOut::reopen: 0th Parcel time is " << file_t0 << std::endl;
              }
              // get the last time from the file
              time_idx = ntimes - 1;
              icount = 1;
              err = nc_get_vara_double( ncid, vid_time, &time_idx, &icount, &tt0 );
              if ( err != NC_NOERR ) {
                 throw(badNetcdfError(err));
              }
              file_t1 = tvnoc( tt0 );
              if ( dbug > 1 ) {
                 std::cerr << "NetcdfOut::reopen: final Parcel time is " << file_t1 << std::endl;
              }
              // now check whether the starting time stamps match
//              my_traj_start = met->time2Cal( t0 + file_t0 );
//              if ( my_traj_start != traj_start ) {
//                 std::cerr << "NetcdfOut::reopen: WARNING: reopened file's start time " << traj_start 
//                 << " and the given start time " << my_traj_start << " do not match!" 
//                 << " This could also be a symptom of the zero time of the former run not matching the current zero time."
//                 << std::endl;
//              }
              
              if ( ! paranoyd ) {
                 tyme = file_t1;
                 tnum = ntimes;
              } else {
                 tyme = file_t1;
                 tnum = ntimes;
              }   
              
              // we could put a number of other time checks in here as well.

           } else {
              throw(badNetcdfError(err));              
           }
        } else {
           // no time values have been written
           if ( ! paranoyd ) {
              tyme = t0;
              tnum = 0;
           } else {
              tyme = remember_tyme;
              tnum = remember_tnum;
           }   
           if ( dbug > 1 ) {
              std::cerr << "NetcdfOut::reopen: no time values are in the file " << std::endl;
           }
        }

     
     
        // now find out about parcels (mainly, how many there are)
        name = "id";
        var_name = name.c_str(); 
        err = nc_inq_varid( ncid, var_name, &vid_id );
        if ( dbug > 50 ) {
           std::cerr << "read id variable w/ id = " << vid_id << std::endl;
        }
        if ( err == NC_EBADDIM ) {
           std::cerr << name << " variable does not exist" << std::endl;
           throw(badFileConventions());
        } else if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
        err = nc_inq_dimlen( ncid, vid_id, &file_pnum );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
        if ( dbug > 10 ) {
           std::cerr << "NetcdfOut::reopen: There are " << file_pnum << " parcels" << std::endl;
        }
        if ( file_pnum != pnum ) {
           std::cerr << " reopened file's number of parcels does not match ours " << std::endl;
           throw(badNetcdfReOpenMismatch());
        }

        // start with the zeroeth parcel, unless we are reopening from the last write.
        if ( ! paranoyd ) {
           ip = 0;
        } else {   
           ip = remember_ip;
        }

        // get longitude variable id
        vid_lon = get_var_id( "lon", true, "", &vtyp_lon );
        if ( dbug > 1 ) {
           std::cerr << "NetcdfOut::reopen: Got the id for the 'lon' coordinate: " <<  vid_lon << std::endl;
        }
        // get the bad-value flag for longitudes
#ifdef USE_DOUBLE
        err = nc_get_att_double( ncid, vid_lon, "missing_value", &badlon);
#else
        err = nc_get_att_float( ncid, vid_lon, "missing_value", &badlon);
#endif 
        if ( err != NC_NOERR ) {
#ifdef USE_DOUBLE
           err = nc_get_att_double( ncid, vid_lon, "_FillValue", &badlon);
#else
           err = nc_get_att_float( ncid, vid_lon, "_FillValue", &badlon);
#endif      
           if ( err != NC_NOERR ) {
              badlon = badval;
           }   
        }
        if ( (FINITE(badlon) || FINITE(badval)) && (badval != badlon ) ) {
           std::cerr << " reopened file's longitude bad value does not match ours " << std::endl;
           throw(badNetcdfReOpenMismatch());     
        }

        // get latitude variable id
        vid_lat = get_var_id( "lat", true, "", &vtyp_lat );
        if ( dbug > 1 ) {
           std::cerr << "NetcdfOut::reopen: Got the id for the 'lat' coordinate: " <<  vid_lat << std::endl;
        }
        // get the bad-value flag for latitudes
#ifdef USE_DOUBLE
        err = nc_get_att_double( ncid, vid_lat, "missing_value", &badlat);
#else
        err = nc_get_att_float( ncid, vid_lat, "missing_value", &badlat);
#endif 
        if ( err != NC_NOERR ) {
#ifdef USE_DOUBLE
           err = nc_get_att_double( ncid, vid_lat, "_FillValue", &badlat);
#else
           err = nc_get_att_float( ncid, vid_lat, "_FillValue", &badlat);
#endif      
           if ( err != NC_NOERR ) {
              badlat = badval;
           }   
        }
        if ( (FINITE(badlat) || FINITE(badval)) && (badval != badlat ) ) {
           std::cerr << " reopened file's latitude bad value does not match ours " << std::endl;
           throw(badNetcdfReOpenMismatch());     
        }
     
        // get the vertical coordinate variable id
        val = vcoord;
        vid_z = get_var_id( vcoord, true, "vertical_coordinate", &vtyp_z );
        if ( dbug > 1 ) {
           std::cerr << "NetcdfOut::reopen: Got the id for the vert coordinate " << vcoord << ": " << vid_z << std::endl;
        }
#ifdef USE_DOUBLE
        err = nc_get_att_double( ncid, vid_z, "missing_value", &badvert);
#else
        err = nc_get_att_float( ncid, vid_z, "missing_value", &badvert);
#endif 
        if ( err != NC_NOERR ) {
#ifdef USE_DOUBLE
           err = nc_get_att_double( ncid, vid_z, "_FillValue", &badvert);
#else
           err = nc_get_att_float( ncid, vid_z, "_FillValue", &badvert);
#endif  
               
           if ( err != NC_NOERR ) {
              badvert = badval;
           }
        }    
        if ( (FINITE(badvert) || FINITE(badval)) && (badval != badvert ) ) {
           std::cerr << " reopened file's vertical coordinate " << vcoord << " bad value does not match ours " << std::endl;
           throw(badNetcdfReOpenMismatch());     
        }
     
        // try timestamp
        vid_tstamp = get_var_id( "timestamp", false, "", &vtyp_tstamp );
        if ( dbug > 50 ) {
           std::cerr << "NetcdfOut::reopen: vid_tstamp = " << vid_status << ", doing status = " << do_tstamp << std::endl;
        }
        if (  ((vid_tstamp >= 0) && (! do_tstamp) )
           || ((vid_tstamp < 0) && (do_tstamp) ) ) {
           std::cerr << " reopened file and our output do not match with respect to outputting tstamp values " << std::endl;
           throw(badNetcdfReOpenMismatch());     
        }
     
        // try forecast
        vid_fstamp = get_var_id( "forecast", false, "", &vtyp_fstamp );
        if ( dbug > 50 ) {
           std::cerr << "NetcdfOut::reopen: vid_fstamp = " << vid_status << ", doing status = " << do_fstamp << std::endl;
        }
        if (  ((vid_fstamp >= 0) && (! do_fstamp) )
           || ((vid_fstamp < 0) && (do_fstamp) ) ) {
           std::cerr << " reopened file and our output do not match with respect to outputting fstamp values " << std::endl;
           throw(badNetcdfReOpenMismatch());     
        }
     
        // try status
        vid_status = get_var_id( "status", false, "", &vtyp_status );
        if ( dbug > 50 ) {
           std::cerr << "NetcdfOut::reopen: vid_status = " << vid_status << ", doing status = " << do_status << std::endl;
        }
        if (  ((vid_status >= 0) && ( ! do_status) )
           || ((vid_status < 0) && (do_status) ) ) {
           std::cerr << " reopened file and our output do not match with respect to outputting status " << std::endl;
           throw(badNetcdfReOpenMismatch());     
        }
           
        // try flags
        vid_flags = get_var_id( "flags", false, "", &vtyp_flags );
        if (  ((vid_flags >= 0) && (! do_flags) )
           || ((vid_flags < 0) && (do_flags) ) ) {
           std::cerr << " reopened file and our output do not match with respect to outputting flags " << std::endl;
           throw(badNetcdfReOpenMismatch());     
        }
     
        // try tag
        vid_tag = get_var_id( "tag", false, "", &vtyp_tag );
        if (  ((vid_tag >= 0) && (! do_tag) )
           || ((vid_tag < 0) && (do_tag) ) ) {
           std::cerr << " reopened file and our output do not match with respect to outputting tag values " << std::endl;
           throw(badNetcdfReOpenMismatch());     
        }
     
        // try met variables
        for ( int i=0; i < other.size(); i++ ) {
            //// define other variable
            val = other[i];

            vid = get_var_id( val, true, "", &vtype );
            if ( vid >= 0 ) {
            
               // we should probably check the units, bad-value, etc. before proceeding.
               // maybe someday....
            
               vid_other[i] = vid;
               vtyp_other[i] = vtype;
            } else {
               std::cerr << " reopened file does not have the met field " << val  << std::endl;
               throw(badNetcdfReOpenMismatch());     
            }
        }

// test file's traj_start against our tstamp?
     
     }     

     if ( dbug > 1 ) {
        std::cerr << "NetcdfOut::reopen: " << fname << " is opened." << std::endl;
     }

}    



void NetcdfOut::fakeReopen( std::string file, Parcel* p, unsigned int n )
{
     int err;
     std::string aname;
     std::string val;
     char *cval;
     const char *attr_name;
     const char *var_name;
     std::string name;
     nc_type attr_type;
     size_t  attr_size;
     int attr_id;
     int var_id;
     const char *aval;
     nc_type var_type;
     int var_ndims;
     int var_dims[NC_MAX_VAR_DIMS];
     int  var_natts;
     time_t t;
     int dims[2];
     float fval;
     double dval;
     int ival;
     const char* nanstr = "";
     int vid;
     size_t put_count;
     size_t put_start;
     ptrdiff_t put_stride;
     char *xstamp;
     struct tm *tm;
     std::string *tst1;
     ProcessGrp *pgrp;
     std::string traj_start;
     std::string my_traj_start;
     bool i_am_root;
     int ndimens;
     int unlimdim_idx;
     size_t ntimes;
     int time_index;
     size_t istart;
     size_t icount;
     double file_t0;
     double file_t1;
     double tt0;
     size_t time_idx;
     size_t file_pnum;
     int dim_id;
     int file_direction;
     real badlon;
     real badlat;
     real badvert;
     int vtype;


     if ( is_open ) {
        close();
     }

     if ( file != "" ) {
        filename( file );
     }

     if ( dbug > 1 ) {
        std::cerr << "NetcdfOut::fakeReopen: Trying to fake-open " << fname <<  std::endl;
     }

     i_am_root = is_root();
     
     
     is_open = false;

     
     if ( p != NULLPTR ) {
        init(p);
     }

     if ( n > 0 ) {
        pnum = n;
     }
     

     if ( dbug > 1 ) {
        std::cerr << "NetcdfOut::fakeReopen: " << fname << " is fake-opened." << std::endl;
     }

}    



void NetcdfOut::close()
{
     int err;
     bool i_am_root;
     
     if ( is_open ) {

        i_am_root = is_root();

        if ( i_am_root) {
           err = nc_close(ncid);
           if ( err != NC_NOERR ) {
              throw(badNetcdfError(err));
           }
        }

        if ( paranoyd ) {
           remember_tyme = dNaN;
           remember_ip = 0;
           remember_tnum = 0;
        }

        is_open = false;
        
        if ( dbug > 1 ) {
           std::cerr << "NetcdfOut::close: " << fname << " is closed." << std::endl;
        }

     }
     
     vid_lon = -1;
     vid_lat = -1;
     vid_z = -1;
     vid_status = -1;
     vid_flags = -1;
     vid_tag = -1;
     vid_tstamp = -1;
     vid_fstamp = -1;
     for ( int i=0; i < other.size(); i++ ) {
         vid_other[i] = -1;
     }
     
}

void NetcdfOut::rclose()
{
     
     if ( is_open ) {

        close();
        
        if ( paranoyd ) {
           remember_tyme = tyme;
           remember_ip = ip;
           remember_tnum = tnum;
        }

        if ( dbug > 1 ) {
           std::cerr << "NetcdfOut::rclose: " << fname << " is closed." << std::endl;
        }

     }
     
     vid_lon = -1;
     vid_lat = -1;
     vid_z = -1;
     vid_status = -1;
     vid_flags = -1;
     vid_tag = -1;
     vid_tstamp = -1;
     vid_fstamp = -1;
     for ( int i=0; i < other.size(); i++ ) {
         vid_other[i] = -1;
     }
     
}

int NetcdfOut::get_var_id( const std::string &varname, bool required, const std::string &flag, int*vtype )
{
     int result;
     int err;
     const char* c_var_name;
     int var_id;
     int var_type;
     int var_ndims;
     int nvars;
     int natts;
     int att_id;
     int att_type;
     size_t att_len;
     char c_aname[NC_MAX_NAME + 1];
     std::string aname;
     char *c_att_val;
     std::string att_val;
     int var_dims[NC_MAX_VAR_DIMS];
     std::string alt_vertname;
     
     result = -1;

     if ( varname != "" ) {
     
         c_var_name = varname.c_str();
         err = nc_inq_varid( ncid, c_var_name, &var_id );
         if ( err == NC_NOERR ) {
            result = var_id;
         } else if ( err == NC_ENOTVAR ) {
            if ( required ) {
               std::cerr << varname << " is required but not present in this file " << std::endl;
               throw(badFileConventions());
            }
         } else {
            throw(badNetcdfError(err));
         }
     
     }
     
     if ( (result == -1) && (flag != "") ) {
        // no var ID yet
        
        // go through each variable in the file
        err = nc_inq_nvars( ncid, &nvars );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
        
        for ( var_id=0; var_id < nvars; var_id++ ) {
             
             // go through the atttributes
             err = nc_inq_varnatts( ncid, var_id, &natts );
             if ( err != NC_NOERR ) {
                throw(badNetcdfError(err));
             }
             for ( att_id=0; att_id < natts; att_id++ ) {
        
                 err = nc_inq_attname( ncid, var_id, att_id, c_aname );
                 if ( err != NC_NOERR ) {
                    throw(badNetcdfError(err));
                 }
        
                 err = nc_inq_att( ncid, var_id, c_aname, &att_type, &att_len );
                 if ( err != NC_NOERR ) {
                    throw(badNetcdfError(err));
                 }
                 
                 aname.assign( c_aname );
                 if ( aname == flag ) {
                    // we fund the flag attribute in one of the variables
                    
                    // now check that it is set to "yes"
                    if ( att_type == NC_STRING && att_len == 1 ) {
                    
                       err = nc_get_att_string( ncid, var_id, c_aname, &c_att_val );
                       if ( err != NC_NOERR ) {
                          throw(badNetcdfError(err));
                       }
                       att_val.assign( c_att_val );
                       free(c_att_val);
                       
                       if ( att_val == "yes" ) {
                          result = var_id;
                          break;
                       }
                    
                    }
                    
                 
                 
                 }
                 
        
             }
             
             if ( result != -1 ) {
                break;
             }
        }
     }
     
     
     // if we found the variable, then do a little sanity checking
     if ( result >= 0 ) {
        err = nc_inq_var( ncid, result, NULL, &var_type, &var_ndims, var_dims, &natts);
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     
        if ( var_ndims != 2 && varname != "timestamp" ) {
           std::cerr << "variable " << varname << " has " << var_ndims << " dimensions instead of 2 " << std::endl;
           throw(badFileConventions());
        }
        if ( ( var_dims[0] != did_time ) || ( var_dims[1] != did_id ) ) {
           std::cerr << " variable " << varname 
           << " has dimensions that do not match time and/or did_id" << std::endl;
           throw(badFileConventions());        
        }
     
        *vtype = var_type;
     
     }
     
     return result;

}



void NetcdfOut::writeout( double t, unsigned int n, real *lons, real *lats, real *zs, int *flags, int *statuses, double *tags, real **stuff )
{
   int err;
   bool notrace;
   size_t t_start;
   size_t t_count;
   ptrdiff_t t_stride;
   size_t put_start[2];
   size_t put_count[2];
   ptrdiff_t put_stride[2];
   int nstuff;
   int vid;
   std::string aname;
   std::string val;
   const char *aval;
   bool i_am_root;
   double netcdf_time;
   real* znew;
   real* newstuff;


   if ( ! paranoyd ) {
      if ( ! is_open ) {
         open();
      }
   } else {
      reopen();
   }

   i_am_root = is_root();


   if ( dbug > 5 ) {
      std::cerr << "NetcdfOut::writeout: seeking to write " << n << " Parcels at time " << t << std::endl;
   }
   if ( ( ip == 0 ) || ( ! isfinite( tyme ) ) || ( ( t != tyme ) && (! notrace) ) ) {
      // new time to output to file as part of the time dimension
      if ( dbug > 10 ) {
         std::cerr << "NetcdfOut::writeout: advancing time from " << tyme << " to " << t << std::endl;
      }
      
      // sanity check
      if ( ip != 0 ) {
         std::cerr << "NetcdfOut::writeout: starting a new time, but with ip (" << ip 
         << ")!= pnum (" << pnum << ")" << std::endl;
         throw(badNetcdfBadNumberParcels());         
      }
      
      // between the two times, determine which direction we are moving
      if ( (dir == 0) && isfinite(tyme) ) {
         if ( tyme < t ) {
            dir = 1;
         } else if ( tyme > t ) {
            dir = -1;
         }
         if ( dbug > 10 ) {
            std::cerr << "NetcdfOut::writeout: setting traj direction to " << dir << std::endl;
            std::cerr << "                     tyme = " << tyme << ", t = " << t << std::endl;
         }
         if ( dir > 0 ) {
            aname = "Trajectory_direction";
            val = "fwd";
            aval = val.c_str();
            if ( i_am_root) {
               err = nc_put_att_string( ncid, NC_GLOBAL, aname.c_str(), 1, &aval );
               if ( err != NC_NOERR ) {
                  throw(badNetcdfError(err));
               }        
            }        
         } else if ( dir < 0 ) {
            aname = "Trajectory_direction";
            val = "bck";
            aval = val.c_str();
            if ( i_am_root) {
               err = nc_put_att_string( ncid, NC_GLOBAL, aname.c_str(), 1, &aval );
               if ( err != NC_NOERR ) {
                  throw(badNetcdfError(err));
               }        
            }        
     
         }
      }
      
      t_start = tnum;
      t_count = 1;
      t_stride = 1;
      if ( i_am_root) {
         netcdf_time = tconv( t );
         err = nc_put_vars_double( ncid, vid_time, &t_start, &t_count, &t_stride, &netcdf_time );
         if ( err != NC_NOERR ) {
            throw(badNetcdfError(err));
         }
      }
      if ( do_tstamp ) {
         std::string ts = met->time2Cal( t );
         const char *tsc = ts.c_str();
         if ( i_am_root ) {
            err = nc_put_vars_string( ncid, vid_tstamp, &t_start, &t_count, &t_stride, &tsc );
            if ( err != NC_NOERR ) {
               throw(badNetcdfError(err));
            }
         }
      }
      if ( do_fstamp ) {
         netcdf_time = met->forecastLeadTime( t );
         // netcdf_time = tconv( netcdf_time );
         if ( i_am_root ) {
            err = nc_put_vars_double( ncid, vid_fstamp, &t_start, &t_count, &t_stride, &netcdf_time );
            if ( err != NC_NOERR ) {
               throw(badNetcdfError(err));
            }
         }
      }
      
      tnum++;
      tyme = t;
      ip = 0;
   
   }
   
   // Can we write this many Parcels?
//std::cerr << "n=" << n << ", ip=" << ip << ", pnum=" << pnum << std::endl;
   if ( ( n > 0 ) && ( (ip+n) <= pnum ) ) {
   
       put_start[0] = tnum - 1;
       put_start[1] = ip;
       put_count[0] = 1;
       put_count[1] = n;
       put_stride[0] = 1;
       put_stride[1] = 1;
       
       if ( i_am_root) {
#ifdef USE_DOUBLE   
          err = nc_put_vars_double( ncid, vid_lon, put_start, put_count, put_stride, lons );
#else
          err = nc_put_vars_float( ncid, vid_lon, put_start, put_count, put_stride, lons );
#endif
          if ( err != NC_NOERR ) {
             throw(badNetcdfError(err));
          }
       }
         
       if ( i_am_root) {
#ifdef USE_DOUBLE   
          err = nc_put_vars_double( ncid, vid_lat, put_start, put_count, put_stride, lats );
#else
          err = nc_put_vars_float( ncid, vid_lat, put_start, put_count, put_stride, lats );
#endif
          if ( err != NC_NOERR ) {
             throw(badNetcdfError(err));
          }
       }

       if ( i_am_root) {
          if ( vfactor != 1.0 ) {
             znew = new real[n];
             for ( int i=0; i < n; i++ ) {
                 znew[i] = zs[i]*vfactor;
             }
          } else {
             znew = zs;
          }
#ifdef USE_DOUBLE   
          err = nc_put_vars_double( ncid, vid_z, put_start, put_count, put_stride, znew );
#else
          err = nc_put_vars_float( ncid, vid_z, put_start, put_count, put_stride, znew );
#endif
          if ( vfactor != 1.0 ) {
             delete[] znew;
          }

          if ( err != NC_NOERR ) {
             throw(badNetcdfError(err));
          }
       }
   
       if ( flags != NULL ) {
          if ( i_am_root) {
             err = nc_put_vars_int( ncid, vid_flags, put_start, put_count, put_stride, flags );
             if ( err != NC_NOERR ) {
                throw(badNetcdfError(err));
             }
          }
       }
       if ( statuses != NULL ) {
          if ( i_am_root) {
             err = nc_put_vars_int( ncid, vid_status, put_start, put_count, put_stride, statuses );
             if ( err != NC_NOERR ) {
                throw(badNetcdfError(err));
             }
          }
       }
       if ( tags != NULL ) {
          if ( i_am_root) {
             err = nc_put_vars_double( ncid, vid_tag, put_start, put_count, put_stride, tags );
             if ( err != NC_NOERR ) {
                throw(badNetcdfError(err));
             }
          }
       }
       if ( stuff != NULL ) {
          nstuff = other.size();
          for ( int i=0; i< nstuff; i++ ) {
              vid = vid_other[i];
              if ( i_am_root) {
                 real fctr = other_factor[i];
                 if ( fctr == 1.0 ) {
                    newstuff = stuff[i];
                 } else {
                    newstuff = new real[n];
                    for ( int k=0; k<n; k++ ) {
                       newstuff[k] = stuff[i][k]*fctr;
                    }
                 }
#ifdef USE_DOUBLE   
                 err = nc_put_vars_double( ncid, vid, put_start, put_count, put_stride, newstuff );
#else
                 err = nc_put_vars_float( ncid, vid, put_start, put_count, put_stride, newstuff );
#endif
                 if ( err != NC_NOERR ) {
                    throw(badNetcdfError(err));
                 }
                 if ( fctr != 1.0 ) {
                    delete[] newstuff;
                 }
              }
          
          }
       }
   
   
       ip += n;
       
       if ( ip >= pnum ) {
          ip = 0;
       }
       
   } else {
      std::cerr << "Attempted to write " <<  n << " Parcels." << std::endl;
      throw(badNetcdfBadNumberParcels());
   } 
   
   if ( paranoyd ) {
      rclose();
   }
   
}

void NetcdfOut::apply( Parcel& p )
{
   double t;
   real val;
   real lon,lat,z;
   double tagval;
   double* tag;
   bool notrace;
   int iflags;
   int* flags;
   int istatus;
   int* statuses;
   real **stuff;
   int nstuff;
   
   notrace = p.queryNoTrace();
   
   
   t = p.getTime();
 
   lon = badval;
   lat = badval;
   z = badval;
   tag = NULL;
   flags = NULL;
   statuses = NULL;
   stuff = NULL;
   
   
   if ( ! notrace ) {
      lon = p.getLon();
      lat = p.getLat();
      z = p.getZ();
   }
   if ( do_flags ) {
      iflags = p.flags();
      flags = &iflags;
   }
   if ( do_status ) {
      istatus = p.status();
      statuses = &istatus;
   }
   if ( do_tag ) {
      tagval = p.tag();
      tag = &tagval;
   }
   nstuff = other.size();
   if ( nstuff > 0 ) {
      stuff = new real*[nstuff];
      for ( int i=0; i < nstuff; i++ ) {
          stuff[i] = new real[1];
          if ( ! notrace ) {
             val = met->getData( other[i], t, lon, lat, z );
          } else {
             val = badval;
          }
          *(stuff[i]) = val;
      }
   }
 
   writeout( t, 1, &lon, &lat, &z, flags, statuses, tag, stuff );
 
   if ( nstuff > 0 ) {
      for ( int i=0; i < nstuff; i++ ) {
          delete stuff[i];
      }
      delete stuff;
   }

}

void NetcdfOut::apply( Parcel * const p, const int n )
{
   int chunksize;
   double t;
   double tt;
   real val;
   real* lons;
   real* lats;
   real* zs;
   double* tags;
   bool notrace;
   int* flags;
   int* statuses;
   real **stuff;
   int nstuff;
   int ii;
   int j;
   
   stuff = NULLPTR;
   
   if ( n > 0 )  { 
   
      // first, allocate the space for this chunk
      lons = new real[maxchunk];
      lats = new real[maxchunk];
      zs   = new real[maxchunk];
      flags = NULL;
      if ( do_flags ) {
          flags = new int[maxchunk];
      }
      statuses = NULL;
      if ( do_status ) {
          statuses = new int[maxchunk];
      }
      tags = NULL;
      if ( do_tag ) {
          tags = new double[maxchunk];
      }    
      nstuff = other.size();
      if ( nstuff > 0 ) {
         stuff = new real*[nstuff];
         for ( int i=0; i < nstuff; i++ ) {
             stuff[i] = new real[maxchunk];
         }
      }
      
      // find the time for this batch of Parcels
      tt = 0.0;
      for ( j=0; j < pnum; j++ ) {
          notrace = p[j].queryNoTrace();
          if ( ! notrace ) {
             // probably need to be more sophisticated about this
             tt = p[j].getTime();
          }
      }
      if ( dbug > 50 ) {
         std::cerr << " output time tt=" << tt << std::endl;      
      }
      ii = 0;
      while ( ii < n ) {
   
         chunksize = n - ii;
         if ( chunksize > maxchunk ) {
            chunksize = maxchunk;
         }
         
        
         // now load the content
         for ( int i=0; i < chunksize; i++ ) {
         
             j = ii + i;
         
             notrace = p[j].queryNoTrace();
             if ( ! notrace ) {
                // we only output parcels which are out the standard time for this batch of parcels
                if (  abs( p[j].getTime() - tt ) > 1e-5 ) {
                   notrace = true;
                }
             }
             
             lons[i] = badval;
             lats[i] = badval;
             zs[i]   = badval;
             if ( do_tag ) {
                tags[i] = dbadval;
             }   
             if ( ! notrace ) {
                p[j].getPos( &(lons[i]), &(lats[i]) );
                zs[i] = p[j].getZ();
                
                if ( do_tag ) {
                   tags[i] = p[j].tag();
                }
                         
             }
             if ( do_flags ) {
                flags[i] = p[j].flags();
             }
             if ( do_status ) {
                statuses[i] = p[j].status();
             }
         
         }
         if ( nstuff > 0 ) {
            for ( int i=0; i < nstuff; i++ ) {
               met->getData( other[i], tt, chunksize, lons, lats, zs, stuff[i], METDATA_NANBAD );
               for ( int k=0; k < chunksize; k++ ) {
                   j = ii + k;
                   notrace = p[j].queryNoTrace();
                   if ( notrace ) {
                      (stuff[i])[j] = badval;
                   }
               }
            }
         }
        
         writeout( tt, chunksize, lons, lats, zs, flags, statuses, tags, stuff );
        
       
       
         // on to the next chunk
         ii = ii + chunksize;
         
      }
      // now free the space for this chunk
      if ( nstuff > 0 ) {
         for ( int i=0; i < nstuff; i++ ) {
             delete[] stuff[i];
         }
         delete[] stuff;
      }
      if ( tags != NULL ) {
         delete[] tags;
      }
      if ( statuses != NULL ) {
         delete[] statuses;
      }
      if ( flags != NULL ) {
         delete[] flags;
      } 
      delete[] zs;
      delete[] lats;
      delete[] lons;
   
   } else {
      // zero or fewer Parcels
      throw(badNetcdfBadNumberParcels());
   } 
   
}

void NetcdfOut::apply( std::vector<Parcel>& p )
{
   int chunksize;
   double t;
   double tt;
   real val;
   real* lons;
   real* lats;
   real* zs;
   double* tags;
   bool notrace;
   int* flags;
   int* statuses;
   real **stuff;
   int nstuff;
   int ii;
   int j;
   int n;
   std::vector<Parcel>::iterator vi;
   
   
   n = p.size();
   
   stuff = NULLPTR;
   
   if ( n > 0 )  { 
   
      // first, allocate the space for this chunk
      lons = new real[maxchunk];
      lats = new real[maxchunk];
      zs   = new real[maxchunk];
      flags = NULL;
      if ( do_flags ) {
          flags = new int[maxchunk];
      }
      statuses = NULL;
      if ( do_status ) {
          statuses = new int[maxchunk];
      }
      tags = NULL;
      if ( do_tag ) {
          tags = new double[maxchunk];
      }    
      nstuff = other.size();
      if ( nstuff > 0 ) {
         stuff = new real*[nstuff];
         for ( int i=0; i < nstuff; i++ ) {
             stuff[i] = new real[maxchunk];
         }
      }
      
      // find the time for this batch of Parcels
      tt = 0.0;
      for ( vi = p.begin(); vi != p.end(); vi++ ) {
          notrace = vi->queryNoTrace();
          if ( ! notrace ) {
             // probably need to be more sophisticated about this
             tt = vi->getTime();
          }
      }
      if ( dbug > 50 ) {
         std::cerr << " output time tt=" << tt << std::endl;      
      }
      ii = 0;
      vi = p.begin();
      while ( ii < n ) {
   
         chunksize = n - ii;
         if ( chunksize > maxchunk ) {
            chunksize = maxchunk;
         }
         
        
         // now load the content
         for ( int i=0; i < chunksize; i++ ) {
         
             notrace = vi->queryNoTrace();
             if ( ! notrace ) {
                // we only output parcels which are out the standard time for this batch of parcels
                if (  abs( vi->getTime() - tt ) > 1e-5 ) {
                   notrace = true;
                }
             }
             
             lons[i] = badval;
             lats[i] = badval;
             zs[i]   = badval;
             if ( do_tag ) {
                tags[i] = dbadval;
             }   
             if ( ! notrace ) {
                vi->getPos( &(lons[i]), &(lats[i]) );
                zs[i] = vi->getZ();
                
                if ( do_tag ) {
                   tags[i] = vi->tag();
                }
                         
             }
             if ( do_flags ) {
                flags[i] = vi->flags();
             }
             if ( do_status ) {
                statuses[i] = vi->status();
             }
         
             vi++;
         }
         if ( nstuff > 0 ) {
            for ( int i=0; i < nstuff; i++ ) {
               met->getData( other[i], tt, chunksize, lons, lats, zs, stuff[i], METDATA_NANBAD );
               for ( int k=0; k < chunksize; k++ ) {
                   j = ii + k;
                   notrace = vi->queryNoTrace();
                   if ( notrace ) {
                      (stuff[i])[j] = badval;
                   }
               }
            }
         }
        
         writeout( tt, chunksize, lons, lats, zs, flags, statuses, tags, stuff );
        
         // on to the next chunk
         ii = ii + chunksize;
         
      }
      // now free the space for this chunk
      if ( nstuff > 0 ) {
         for ( int i=0; i < nstuff; i++ ) {
             delete[] stuff[i];
         }
         delete[] stuff;
      }
      if ( tags != NULL ) {
         delete[] tags;
      }
      if ( statuses != NULL ) {
         delete[] statuses;
      }
      if ( flags != NULL ) {
         delete[] flags;
      } 
      delete[] zs;
      delete[] lats;
      delete[] lons;
   
   } else {
      // zero or fewer Parcels
      throw(badNetcdfBadNumberParcels());
   } 
   
}

void NetcdfOut::apply( std::list<Parcel>& p )
{
   int chunksize;
   double t;
   double tt;
   real val;
   real* lons;
   real* lats;
   real* zs;
   double* tags;
   bool notrace;
   int* flags;
   int* statuses;
   real **stuff;
   int nstuff;
   int ii;
   int j;
   int n;
   std::list<Parcel>::iterator vi;
   
   
   n = p.size();
   
   stuff = NULLPTR;
   
   if ( n > 0 )  { 
   
      // first, allocate the space for this chunk
      lons = new real[maxchunk];
      lats = new real[maxchunk];
      zs   = new real[maxchunk];
      flags = NULL;
      if ( do_flags ) {
          flags = new int[maxchunk];
      }
      statuses = NULL;
      if ( do_status ) {
          statuses = new int[maxchunk];
      }
      tags = NULL;
      if ( do_tag ) {
          tags = new double[maxchunk];
      }    
      nstuff = other.size();
      if ( nstuff > 0 ) {
         stuff = new real*[nstuff];
         for ( int i=0; i < nstuff; i++ ) {
             stuff[i] = new real[maxchunk];
         }
      }
      
      // find the time for this batch of Parcels
      tt = 0.0;
      for ( vi = p.begin(); vi != p.end(); vi++ ) {
          notrace = vi->queryNoTrace();
          if ( ! notrace ) {
             // probably need to be more sophisticated about this
             tt = vi->getTime();
          }
      }
      if ( dbug > 50 ) {
         std::cerr << " output time tt=" << tt << std::endl;      
      }
      ii = 0;
      vi = p.begin();
      while ( ii < n ) {
   
         chunksize = n - ii;
         if ( chunksize > maxchunk ) {
            chunksize = maxchunk;
         }
         
        
         // now load the content
         for ( int i=0; i < chunksize; i++ ) {
         
             notrace = vi->queryNoTrace();
             if ( ! notrace ) {
                // we only output parcels which are out the standard time for this batch of parcels
                if (  abs( vi->getTime() - tt ) > 1e-5 ) {
                   notrace = true;
                }
             }
             
             lons[i] = badval;
             lats[i] = badval;
             zs[i]   = badval;
             if ( do_tag ) {
                tags[i] = dbadval;
             }   
             if ( ! notrace ) {
                vi->getPos( &(lons[i]), &(lats[i]) );
                zs[i] = vi->getZ();
                
                if ( do_tag ) {
                   tags[i] = vi->tag();
                }
                         
             }
             if ( do_flags ) {
                flags[i] = vi->flags();
             }
             if ( do_status ) {
                statuses[i] = vi->status();
             }
         
             vi++;
         }
         if ( nstuff > 0 ) {
            for ( int i=0; i < nstuff; i++ ) {
               met->getData( other[i], tt, chunksize, lons, lats, zs, stuff[i], METDATA_NANBAD );
               for ( int k=0; k < chunksize; k++ ) {
                   j = ii + k;
                   notrace = vi->queryNoTrace();
                   if ( notrace ) {
                      (stuff[i])[j] = badval;
                   }
               }
            }
         }
        
         writeout( tt, chunksize, lons, lats, zs, flags, statuses, tags, stuff );
        
         // on to the next chunk
         ii = ii + chunksize;
         
      }
      // now free the space for this chunk
      if ( nstuff > 0 ) {
         for ( int i=0; i < nstuff; i++ ) {
             delete[] stuff[i];
         }
         delete[] stuff;
      }
      if ( tags != NULL ) {
         delete[] tags;
      }
      if ( statuses != NULL ) {
         delete[] statuses;
      }
      if ( flags != NULL ) {
         delete[] flags;
      } 
      delete[] zs;
      delete[] lats;
      delete[] lons;
   
   } else {
      // zero or fewer Parcels
      throw(badNetcdfBadNumberParcels());
   } 
   
}

void NetcdfOut::apply( std::deque<Parcel>& p )
{
   int chunksize;
   double t;
   double tt;
   real val;
   real* lons;
   real* lats;
   real* zs;
   double* tags;
   bool notrace;
   int* flags;
   int* statuses;
   real **stuff;
   int nstuff;
   int ii;
   int j;
   int n;
   std::deque<Parcel>::iterator vi;
   
   stuff = NULLPTR;
   
   n = p.size();
   
   if ( n > 0 )  { 
   
      // first, allocate the space for this chunk
      lons = new real[maxchunk];
      lats = new real[maxchunk];
      zs   = new real[maxchunk];
      flags = NULL;
      if ( do_flags ) {
          flags = new int[maxchunk];
      }
      statuses = NULL;
      if ( do_status ) {
          statuses = new int[maxchunk];
      }
      tags = NULL;
      if ( do_tag ) {
          tags = new double[maxchunk];
      }    
      nstuff = other.size();
      if ( nstuff > 0 ) {
         stuff = new real*[nstuff];
         for ( int i=0; i < nstuff; i++ ) {
             stuff[i] = new real[maxchunk];
         }
      }
      
      // find the time for this batch of Parcels
      tt = 0.0;
      for ( vi = p.begin(); vi != p.end(); vi++ ) {
          notrace = vi->queryNoTrace();
          if ( ! notrace ) {
             // probably need to be more sophisticated about this
             tt = vi->getTime();
          }
      }
      if ( dbug > 50 ) {
         std::cerr << " output time tt=" << tt << std::endl;      
      }
      ii = 0;
      vi = p.begin();
      while ( ii < n ) {
   
         chunksize = n - ii;
         if ( chunksize > maxchunk ) {
            chunksize = maxchunk;
         }
         
        
         // now load the content
         for ( int i=0; i < chunksize; i++ ) {
         
             notrace = vi->queryNoTrace();
             if ( ! notrace ) {
                // we only output parcels which are out the standard time for this batch of parcels
                if (  abs( vi->getTime() - tt ) > 1e-5 ) {
                   notrace = true;
                }
             }
             
             lons[i] = badval;
             lats[i] = badval;
             zs[i]   = badval;
             if ( do_tag ) {
                tags[i] = dbadval;
             }   
             if ( ! notrace ) {
                vi->getPos( &(lons[i]), &(lats[i]) );
                zs[i] = vi->getZ();
                
                if ( do_tag ) {
                   tags[i] = vi->tag();
                }
                         
             }
             if ( do_flags ) {
                flags[i] = vi->flags();
             }
             if ( do_status ) {
                statuses[i] = vi->status();
             }
         
             vi++;
         }
         if ( nstuff > 0 ) {
            for ( int i=0; i < nstuff; i++ ) {
               met->getData( other[i], tt, chunksize, lons, lats, zs, stuff[i], METDATA_NANBAD );
               for ( int k=0; k < chunksize; k++ ) {
                   j = ii + k;
                   notrace = vi->queryNoTrace();
                   if ( notrace ) {
                      (stuff[i])[j] = badval;
                   }
               }
            }
         }
        
         writeout( tt, chunksize, lons, lats, zs, flags, statuses, tags, stuff );
        
         // on to the next chunk
         ii = ii + chunksize;
         
      }
      // now free the space for this chunk
      if ( nstuff > 0 ) {
         for ( int i=0; i < nstuff; i++ ) {
             delete[] stuff[i];
         }
         delete[] stuff;
      }
      if ( tags != NULL ) {
         delete[] tags;
      }
      if ( statuses != NULL ) {
         delete[] statuses;
      }
      if ( flags != NULL ) {
         delete[] flags;
      } 
      delete[] zs;
      delete[] lats;
      delete[] lons;
   
   } else {
      // zero or fewer Parcels
      throw(badNetcdfBadNumberParcels());
   } 
   
}

void NetcdfOut::apply( Flock& p )
{
   int chunksize;
   double t;
   double tt;
   double ttbck;
   double tttmp;
   real val;
   real* lons;
   real* lats;
   real* zs;
   double* tags;
   bool *xnotrace;
   bool notrace;
   bool anytrace;
   int* flags;
   int* statuses;
   real **stuff;
   int nstuff;
   int ii;
   int j;
   int n;
   Flock::iterator vi;
   bool i_am_root;
   Parcel* px;
   bool gotit;
   
   stuff = NULLPTR;

   i_am_root = p.is_root();
   
   n = p.size();
   
   // initialize the parcel time to NaN
   tt = dNaN;
   
   if ( n > 0 )  { 
   
      // allocate the space for this chunk
      lons = new real[maxchunk];
      lats = new real[maxchunk];
      zs   = new real[maxchunk];
      xnotrace = new bool[maxchunk];
      flags = NULL;
      if ( do_flags ) {
          flags = new int[maxchunk];
      }
      statuses = NULL;
      if ( do_status ) {
          statuses = new int[maxchunk];
      }
      tags = NULL;
      if ( do_tag ) {
          tags = new double[maxchunk];
      } 
      // misc met fields
      stuff = NULLPTR;   
      nstuff = other.size();
      if ( nstuff > 0 ) {
         stuff = new real*[nstuff];
         for ( int i=0; i < nstuff; i++ ) {
             stuff[i] = new real[maxchunk];
         }
      }
       
      p.sync();
      
      // find the time for this batch of Parcels
      // (A better way to do this would be to have each
      // processor find the time for all of its share of the parcels.
      // then have the root processor select the best time from the other processors.)
      
      // initialize the parcel time to NaN
      tt = dNaN;
      // Assume that none of the parcels is valid
      anytrace = false;
      // for each Parcel in the Flock....
      for ( int j=0; j < pnum; j++ ) {
          
          notrace = true;
          
          try {
             // Try to get the parcel.
             // If this is the root processor, we get a valid pointer.
             // If this is the non-root owner processor, we also get a valid pointer 
             // If this is a non-root non-owner processor for the ith parcel, we get NULLPTR
             // or a badparcelindex error.
             px =  p.parcel( j, 1 );
             // But we only want the case of the root processor
             gotit = ( ( px != NULLPTR ) && i_am_root );

          } catch (Swarm::badparcelindex()) {
            // no problem. ignore this. 
            gotit = false; 
          }; 
          if ( gotit ) {
             // OK, we are in the root processor with a valid parcel pointer
             
             // is this parcel being traced?
             notrace = px->queryNoTrace();
             // are any of the parcels in this Flock being traced?
             anytrace = anytrace || ( ! notrace );
             
             // get this parcel's time
             tttmp = px->getTime();
             if ( j != 0 ) {
                // not the first parcel
                if ( dir == -1 ) {
                   // note: this does the right thing even if the output time is still NaN
                   if ( tttmp < tyme ) {
                      // the direction is backwards, and 
                      // this parcel's time is before the output time
                      // so replace the previous parcel time with this parcel's time.
                      ttbck = tttmp;
                   }   
                } else if ( dir == 1 ) {
                   if ( tttmp > tyme ) {
                      // the direction is forward, and
                      // this parcel's time is after the output time,
                      // so replace the previous parcel time with this parcel's time
                      ttbck = tttmp;
                   }
                }
             } else {
                // first time through: initialize
                // the previous parcel time to be the same as this parcel's time
                ttbck = tttmp;
             }
             if ( ! notrace ) {
                // if we are are tracing this parcel, then set
                // the time to this parcel's time, as determined above.
                // Otherwise, the time tt remains NaN as set above.
                // (probably need to be more sophisticated about this)
                tt = tttmp;
             }
          }   
      }
      
      if ( ! isfinite(tt) ) {
         // No Parcels in this batch being traced.
         // We must therefore be careful, since none
         // of their times are valid for output, and
         // thus we need to output them with a made-up time. 
         
         // We start with a valid time
         // but if there is no valid time yet, we use the
         // last valid parcel time that we know of
         if ( isfinite(tyme) ) {
            tt = tyme;
         } else {
            tt = ttbck;
         }
         // this is probably a previously-used time
         // so shift this item by about 10 s.
         if ( dir == -1 ) {
            tt = tt - 1e-4;
         } else if ( dir == 1 ) {
            tt = tt + 1e-4;
         }
      } 
      if ( dbug > 50 ) {
         std::cerr << " output time tt=" << tt << std::endl;      
      }
      
      // we will be writing the data out in chunks
      
      // ii is the parcel base index (i.e. the start of the current chunk of output)
      ii = 0;
      // for each parcel, chunked...
      while ( ii < n ) {
   
         // how many parcels are left to do?
         // this will be the chunk size, up
         // to a maximum size of maxchunk
         chunksize = n - ii;
         if ( chunksize > maxchunk ) {
            chunksize = maxchunk;
         }
         
        
         // now load the content for this chunk
         
         // for each of the parcels in this chunk...
         for ( int i=0; i < chunksize; i++ ) {
         
             // the parcel number: the base index plus the chunk-relative index
             j = ii + i;
         
             // start by assuming this is no valid parcel
             notrace = true;
             lons[i] = badval;
             lats[i] = badval;
             zs[i]   = badval;
             if ( do_tag ) {
                tags[i] = dbadval;
             }   
             if ( do_flags ) {
                flags[i] = NoTrace;
             }
             if ( do_status ) {
                statuses[i] = Inert;
             }
             
             try {
                // Now try to get the parcel.
                // If this is the root processor, we get a valid pointer.
                // If this is the non-root owner processor, we also get a valid pointer 
                // If this is a non-root non-owner processor for the ith parcel, we get NULLPTR
                // or a badparcelindex error.
                px =  p.parcel( j, 1 );
                // But we only want the case of the root processor
                gotit = ( ( px != NULLPTR ) && i_am_root );

             } catch (Flock::badparcelindex()) {
               // no problem. ignore this. 
               gotit = false; 
             }; 
             if ( gotit ) {
                // we have a parcel to output
                
                //notrace = xnotrace[j];
                notrace = px->queryNoTrace();
                if ( ! anytrace ) {
                   // this should not be necessary
                   notrace = true;
                }
                if ( ! notrace ) {
                   // we only output parcels which are around the standard time for this batch of parcels
                   if (  abs( px->getTime() - tt ) > 1e-5 ) {
                      notrace = true;
                   }
                }
                
                // save for later
                xnotrace[i] = notrace;
             
                if ( ! notrace ) {
                   // load the position and tag data
                   // only if this parcel is being traced
                   px->getPos( &(lons[i]), &(lats[i]) );
                   zs[i] = px->getZ();
                   
                   if ( do_tag ) {
                      tags[i] = px->tag();
                   }
                            
                }
                if ( do_flags ) {
                   flags[i] = px->flags();
                }
                if ( do_status ) {
                   statuses[i] = px->status();
                }
         
             }
         }
         if ( nstuff > 0 ) {
            // for each extra met field we want to outpout...
            for ( int k=0; k < nstuff; k++ ) {
               met->getData( other[k], tt, chunksize, lons, lats, zs, stuff[k], METDATA_NANBAD );
               for ( int i=0; i < chunksize; i++ ) {
                   j = ii + i;
                   notrace = xnotrace[i];
                   if ( notrace ) {
                      (stuff[k])[i] = badval;
                   }
               }
            }
         }
        
         writeout( tt, chunksize, lons, lats, zs, flags, statuses, tags, stuff );
        
         // on to the next chunk
         ii = ii + chunksize;
         
      }
      // now free the space for this chunk
      if ( nstuff > 0 ) {
         for ( int i=0; i < nstuff; i++ ) {
             delete[] (stuff[i]);
         }
         delete[] stuff;
      }
      if ( tags != NULL ) {
         delete[] tags;
      }
      if ( statuses != NULL ) {
         delete[] statuses;
      }
      if ( flags != NULL ) {
         delete[] flags;
      } 
      delete[] xnotrace;
      delete[] zs;
      delete[] lats;
      delete[] lons;
   
   } else {
      // zero or fewer Parcels
      std::cerr << "Flock size if " <<  n << "Parcels." << std::endl;
      throw(badNetcdfBadNumberParcels());
   } 
   
}

void NetcdfOut::apply( Swarm& p )
{
   int chunksize;
   double t;
   double tt;
   double ttbck;
   double tttmp;
   real val;
   real* lons;
   real* lats;
   real* zs;
   double* tags;
   bool *xnotrace;
   bool notrace;
   bool anytrace;
   int* flags;
   int* statuses;
   real **stuff;
   int nstuff;
   int ii;
   int j;
   int n;
   Swarm::iterator vi;
   bool i_am_root;
   Parcel* px;
   bool gotit;

   stuff = NULLPTR;
      
   i_am_root = p.is_root();
   
   n = p.size();
   
   if ( n > 0 )  { 
   
      // first, allocate the space for this chunk
      lons = new real[maxchunk];
      lats = new real[maxchunk];
      zs   = new real[maxchunk];
      xnotrace = new bool[maxchunk];
      flags = NULL;
      if ( do_flags ) {
          flags = new int[maxchunk];
      }
      statuses = NULL;
      if ( do_status ) {
          statuses = new int[maxchunk];
      }
      tags = NULL;
      if ( do_tag ) {
          tags = new double[maxchunk];
      }    
      stuff = NULL;   
      nstuff = other.size();
      if ( nstuff > 0 ) {
         stuff = new real*[nstuff];
         for ( int i=0; i < nstuff; i++ ) {
             stuff[i] = new real[maxchunk];
         }
      }
       
      p.sync();
      
      // find the time for this batch of Parcels
      // (A better way to do this would be to have each
      // processor find the time for all of its share of the parcels.
      // then have the root processor select the best time from the other processors.)
      tt = dNaN;
      anytrace = false;
      for ( int j=0; j < pnum; j++ ) {

          notrace = true;
          
          try {
             // get the parcel.
             // If this is the root processor, we get a valid pointer.
             // If this is the non-root owner processor, we also get a valid pointer 
             // If this is a non-root non-owner processor for the ith parcel, we get NULLPTR
             // or a badparcelindex error.
             px =  p.parcel( j, 1 );
             // But we only want the case of the root processor
             gotit = ( ( px != NULLPTR ) && i_am_root );

          } catch (Swarm::badparcelindex()) {
            // no problem. ignore this. 
            gotit = false; 
          }; 
          if ( gotit ) {

             notrace = px->queryNoTrace();
             //xnotrace[j] = px->queryNoTrace();
             anytrace = anytrace || ( ! notrace );
             
             tttmp = px->getTime();
             if ( j != 0 ) {
                if ( dir == -1 ) {
                   // note: this works even if tyme is still NaN
                   if ( tttmp < tyme ) {
                      ttbck = tttmp;
                   }   
                } else if ( dir == 1 ) {
                   if ( tttmp > tyme ) {
                      ttbck = tttmp;
                   }
                }
             } else {
                ttbck = tttmp;
             }
             if ( ! notrace ) {
                // probably need to be more sophisticated about this
                tt = tttmp;
             }
             
             delete px;
             
          }   
      }
      if ( ! isfinite(tt) ) {
         // all Parcels in this batch are no-trace
         if ( isfinite(tyme) ) {
            tt = tyme;
         } else {
            tt = ttbck;
         }
         // this is probably a previously-used time
         // so shift this item by about 10 s.
         if ( dir == -1 ) {
            tt = tt - 1e-4;
         } else if ( dir == 1 ) {
            tt = tt + 1e-4;
         }
      } 
      if ( dbug > 50 ) {
         std::cerr << " output time tt=" << tt << std::endl;      
      }
      ii = 0;
      while ( ii < n ) {
   
         chunksize = n - ii;
         if ( chunksize > maxchunk ) {
            chunksize = maxchunk;
         }
         
        
         // now load the content
         for ( int i=0; i < chunksize; i++ ) {
         
             j = ii + i;
         
             notrace = true;
             lons[i] = badval;
             lats[i] = badval;
             zs[i]   = badval;
             if ( do_tag ) {
                tags[i] = dbadval;
             }   
             if ( do_flags ) {
                flags[i] = NoTrace;
             }
             if ( do_status ) {
                statuses[i] = Inert;
             }

             try {
                // get the parcel.
                // If this is the root processor, we get a valid pointer.
                // If this is the non-root owner processor, we also get a valid pointer 
                // If this is a non-root non-owner processor for the ith parcel, we get NULLPTR
                // or a badparcelindex error.
                px =  p.parcel( j, 1 );
                // But we only want the case of the root processor
                gotit = ( ( px != NULLPTR ) && i_am_root );

             } catch (Swarm::badparcelindex()) {
               // no problem. ignore this. 
               gotit = false; 
             }; 
             if ( gotit ) {
             
                //notrace = xnotrace[j];
                notrace = px->queryNoTrace();
                if ( ! anytrace ) {
                   // this should not be necessary
                   notrace = true;
                }
                if ( ! notrace ) {
                   // we only output parcels which are out the standard time for this batch of parcels
                   if (  abs( px->getTime() - tt ) > 1e-5 ) {
                      notrace = true;
                   }
                }
                
                xnotrace[i] = notrace;

                if ( ! notrace ) {
                   px->getPos( &(lons[i]), &(lats[i]) );
                   zs[i] = px->getZ();
                   
                   if ( do_tag ) {
                      tags[i] = px->tag();
                   }
                            
                }
                if ( do_flags ) {
                   flags[i] = px->flags();
                }
                if ( do_status ) {
                   statuses[i] = px->status();
                }
         
                delete px;
                
             }
         }
         if ( nstuff > 0 ) {
            for ( int k=0; k < nstuff; k++ ) {
               met->getData( other[k], tt, chunksize, lons, lats, zs, stuff[k], METDATA_NANBAD );
               for ( int i=0; i < chunksize; i++ ) {
                   j = ii + i;
                   if ( xnotrace[i] ) {
                      (stuff[k])[j] = badval;
                   }
               }
            }
         }

         writeout( tt, chunksize, lons, lats, zs, flags, statuses, tags, stuff );
        
         // on to the next chunk
         ii = ii + chunksize;
         
      }
      // now free the space for this chunk
      if ( nstuff > 0 ) {
         for ( int i=0; i < nstuff; i++ ) {
             delete[] stuff[i];
         }
         delete[] stuff;
      }
      if ( tags != NULL ) {
         delete[] tags;
      }
      if ( statuses != NULL ) {
         delete[] statuses;
      }
      if ( flags != NULL ) {
         delete[] flags;
      } 
      delete[] xnotrace;
      delete[] zs;
      delete[] lats;
      delete[] lons;
   
   } else {
      // zero or fewer Parcels
      std::cerr << "Swarm size if " <<  n << "Parcels." << std::endl;
      throw(badNetcdfBadNumberParcels());
   } 
   
}
