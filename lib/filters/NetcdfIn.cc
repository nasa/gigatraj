
#include "config.h"
#include "gigatraj/NetcdfIn.hh"

using namespace gigatraj;

NetcdfIn :: NetcdfIn()
{
    is_open = false;
    reset_all();
}

NetcdfIn::~NetcdfIn() {

    if ( is_open ) {
       close();
    }

}

// copy constructor
NetcdfIn::NetcdfIn(const NetcdfIn& src) : ParcelInitializer(src)
{
    reset_all();

    fname = src.fname;
    vcoord = src.vcoord;
    vunits = src.vunits;
    vfactor = src.vfactor;
    
    time0 = src.time0;
    t0 = src.t0;
    end = src.end;
    
    dbug = src.dbug;
    
    np = src.np;
    ip = src.ip;
    
    to = src.to;
    ts = src.ts;
    
    vtyp_lon = src.vtyp_lon;
    vtyp_lat = src.vtyp_lat;
    vtyp_z = src.vtyp_z;
    vtyp_status = src.vtyp_status;
    vtyp_flags = src.vtyp_flags;
    vtyp_tag = src.vtyp_tag;

}

NetcdfIn& NetcdfIn::operator=(const NetcdfIn& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }
    
    reset_all();
    this->assign( src ) ;
    
    return *this;
}

void NetcdfIn::assign( const NetcdfIn& src)
{
    reset_all();
    
    ParcelInitializer::assign(src);
    
    fname = src.fname;
    vcoord = src.vcoord;
    vunits = src.vunits;
    vfactor = src.vfactor;
    
    time0 = src.time0;
    t0 = src.t0;
    end = src.end;
    
    dbug = src.dbug;
    
    np = src.np;
    ip = src.ip;
    
    to = src.to;
    ts = src.ts;
    
    vtyp_lon = src.vtyp_lon;
    vtyp_lat = src.vtyp_lat;
    vtyp_z = src.vtyp_z;
    vtyp_status = src.vtyp_status;
    vtyp_flags = src.vtyp_flags;
    vtyp_tag = src.vtyp_tag;

}


void NetcdfIn::filename( const std::string file )
{

    if ( is_open ) {
       close();
    }
    
    if ( file.substr(0,1) != "-" ) {
       fname = file;
    } else {
       fname = file.substr(1);
       at_end(true);
    }
}


void NetcdfIn::clear()
{
    if ( ! is_open ) {
      do_flags = -1;
      do_status = -1;
      do_tag = -1;
      vid_lon = -1;
      vid_lat = -1;
      vid_z = -1;
      vid_status = -1;
      vid_flags = -1;
      vid_tag = -1;
    } else {
       throw new badNetcdfTooLate();
    }
}

void NetcdfIn::reset_all()
{
    const char *nanstr = "";

    if ( is_open ) {
        close();
    }
    clear();

    fname = "";
    vcoord = "";
    vunits = "";
    vfactor = 1.0;
    
    time0 = "";
    t0 = 0.0;
    end = false;
    
    dbug = 0;
    
    np = 0;
    ip = -1;
    
    to = 0.0;
    ts = 1.0;
    
    vtyp_lon = NC_NAT;
    vtyp_lat = NC_NAT;
    vtyp_z = NC_NAT;
    vtyp_status = NC_NAT;
    vtyp_flags = NC_NAT;
    vtyp_tag = NC_NAT;

}



std::string& NetcdfIn::filename()
{
    return fname;
}

void NetcdfIn::vertical( const std::string vert )
{

    vcoord = vert;

}

std::string& NetcdfIn::vertical()
{
    return vcoord;
}


std::string& NetcdfIn::cal()
{
    return time0;
}

double NetcdfIn::time()
{
    return t0;
}

void NetcdfIn::at_end( bool select )
{
    if ( ! isOpen() ){
       end = select;
    } else {
       std::cerr << "at_end() can be called only before open()" << std::endl;
       throw(badNetcdfTooLate()); 
    }
}

void NetcdfIn::readFlags( int mode )
{
    if ( ! is_open ) {
       do_flags = mode;
    } else {
        throw (badNetcdfTooLate());
    }
}

int NetcdfIn::readFlags()
{
    return do_flags;
}
    
void NetcdfIn::readStatus( int mode )
{
    if ( ! is_open ) {
       do_status = mode;
    } else {
        throw (badNetcdfTooLate());
    }
}

int NetcdfIn::readStatus()
{
    return do_status;
}

void NetcdfIn::readTag( int mode )
{
    if ( ! is_open ) {
       do_tag = mode;
    } else {
        throw (badNetcdfTooLate());
    }
}

int NetcdfIn::readTag()
{
    return do_tag;
}

bool NetcdfIn::at_end()
{
    return end;
}

size_t NetcdfIn::n_parcels()
{
     return np;
}

void NetcdfIn::debug( int mode )
{
     dbug = mode;
}

int NetcdfIn::debug()
{
     return dbug;
}

std::string NetcdfIn::findVertical()
{
    int var_id;
    int err;
    int nattrs;
    std::string result;
    int attr_id;
    char c_aname[NC_MAX_NAME + 1];
    char* c_avalue;
    std::string aname;
    std::string avalue;
    size_t alen;
    nc_type atype;
    char c_varname[NC_MAX_NAME + 1];
    bool done;
    
    
    result = "";
    done = false;
    
    for ( var_id = 0; var_id < nvars; var_id++ ) {
        err = nc_inq_varnatts( ncid, var_id, &nattrs );
        if ( err == NC_NOERR ) {
           for ( attr_id = 0; attr_id < nattrs; attr_id++ ) {
               err = nc_inq_attname( ncid, var_id, attr_id, c_aname );
               if ( err == NC_NOERR ) {
                  aname = std::string(c_aname);
                  if ( aname == "vertical_coordinate" ) {
                     err = nc_inq_atttype( ncid, var_id, c_aname, &atype );
                     if ( err == NC_NOERR ) {
                        err = nc_inq_attlen( ncid, var_id, c_aname, &alen );
                        if ( err == NC_NOERR ) {
                        
                           if ( atype == NC_STRING ) {
                              err = nc_get_att_string( ncid, var_id, c_aname, &c_avalue );
                              if ( err == NC_NOERR ) {
                                 avalue = std::string(c_avalue);
                                 if ( avalue == "yes" ) {
                                    nc_free_string( 1, &c_avalue );
                                    err = nc_inq_varname( ncid, var_id, c_varname );
                                    if ( err == NC_NOERR ) {
                                       result = std::string( c_varname );
                                       done = true;
                                       break;
                                    }
                                 }
                              }
                           } else if ( atype == NC_CHAR ) {
                              c_avalue = new char[alen + 1];
                              err = nc_get_att_text( ncid, var_id, c_aname, c_avalue );
                              if ( err == NC_NOERR ) {
                                 c_avalue[alen] = 0;
                                 avalue = std::string(c_avalue);
                                 if ( avalue == "yes" ) {
                                    delete[] c_avalue;
                                    err = nc_inq_varname( ncid, var_id, c_varname );
                                    if ( err == NC_NOERR ) {
                                       result = std::string( c_varname );
                                       done = true;
                                       break;
                                    }
                                 }
                              }   
                              
                           }
                        }
                     }
                  }
               } 
           } // end of inner for
        }
        
        if ( done ) {
           break;
        }
    }
    
    return result;
}   

std::string NetcdfIn::findVunits( int vid )
{
    std::string result;
    int err;
    int nattrs;
    int attr_id;
    char c_varname[NC_MAX_NAME + 1];
    char c_aname[NC_MAX_NAME + 1];
    char* c_avalue;
    nc_type atype;
    size_t alen;
    std::string aname;
    std::string varname;
    
    
    result = "";

    err = nc_inq_varnatts( ncid, vid, &nattrs );
    if ( err == NC_NOERR ) {
       for ( attr_id = 0; attr_id < nattrs; attr_id++ ) {
           err = nc_inq_attname( ncid, vid, attr_id, c_aname );
           if ( err == NC_NOERR ) {
              aname = std::string(c_aname);
              if ( aname == "units" ) {
                 err = nc_inq_atttype( ncid, vid, c_aname, &atype );
                 if ( err == NC_NOERR ) {
                    err = nc_inq_attlen( ncid, vid, c_aname, &alen );
                    if ( err == NC_NOERR ) {                    
                       if ( atype == NC_STRING ) {
                          err = nc_get_att_string( ncid, vid, c_aname, &c_avalue );
                          if ( err == NC_NOERR ) {
                             result = std::string(c_avalue);
                             break;
                          }
                       } else if ( atype == NC_CHAR ) {
                          c_avalue = new char[alen + 1];
                          err = nc_get_att_text( ncid, vid, c_aname, c_avalue );
                          if ( err == NC_NOERR ) {
                             c_avalue[alen] = 0;
                             result = std::string(c_avalue);
                             break;
                          }   
                          
                       }
                    }
                 }
              }
           } 
       }
    }

    if ( result == "" ) {
       err = nc_inq_varname( ncid, vid, c_varname );
       if ( err == NC_NOERR ) {
          varname = std::string( c_varname );
          if ( varname == "p" || varname == "P" ) {
             result = "hPa";
          } else if ( varname == "PAlt" ) {
             result = "km";
          } else if ( varname == "Theta" ) {
             result = "K";
          }
       }
    }    
    
    return result;


}

void NetcdfIn::format( std::string fmt )
{
      int i;
      int state;
      std::string ch;
      std::string metfield;
      
      // since we are specifying a format, clear any previously-set flags
      // and turn everything off.
      do_flags = 0;
      do_status = 0;
      do_tag = 0;
      
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

                // start a % sequence
                state = 1;
             }
             break;
         case 1: // just begun a new "%" sequence
      
            if ( ch != "%" ) {
               
               // are we terminating the sequence already with 
               // a recognized format character?
               if ( ch == "t" || ch == "o" || ch == "a" || ch == "v" 
                 || ch == "i" || ch == "c" || ch == "x" || ch == "T" ) {
                  
                  // these are all ignored
 
                  // reset the state to 0  
                  state = 0;             

               } else if ( ch == "f" ) {
               
                  do_flags = 1;
                  
                  // reset the state to 0  
                  state = 0;             

               } else if ( ch == "s" ) {
               
                  do_status = 1;
                  
                  // reset the state to 0  
                  state = 0;             

               } else if ( ch == "g" ) {
                  
                  do_tag = 1;
                  
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
                     || ch == "i" || ch == "c" || ch == "x" || ch == "T" ) {
                  // termination of a % sequence
                  
                  // ignore these sequences
                  
                  // on to the next sequence
                  state = 0;
                  
            } else if ( ch == "f" ) {
            
               do_flags = 1;
               
               // reset the state to 0  
               state = 0;             

            } else if ( ch == "s" ) {
            
               do_status = 1;
               
               // reset the state to 0  
               state = 0;             

            } else if ( ch == "g" ) {
               
               do_tag = 1;
               
               // reset the state to 0  
               state = 0;             
            } else if ( ch == "{"  ) {      
                                            
                  // not quite at termination of a %i,j{field}m sequence
                                                              
                  state = 10;                  
                                            
            } else {
                std::cerr << "Bad format: " << fmt << std::endl;
                throw (NetcdfIn::badNetcdfFormatSpec());   
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
               
              // ignore the met field spec
                           
            } else {          
                std::cerr << "Bad format: " << fmt << std::endl;
                throw (NetcdfIn::badNetcdfFormatSpec());   
            }
            
            // read for the next sequence
            state = 0;

            break;
         }
            
         i++;
      
      }
      
      fmtspec = fmt;
      

}

std::string NetcdfIn::format()
{
    if ( fmtspec != "" ) {
       return fmtspec;
    } else {
       return "%t %o %a %v";
    }
}

void NetcdfIn::setTimeTransform( double scale, double offset )
{
    if ( ! is_open ) {
       ts = scale;
       to = offset;
    } else {
       throw new badNetcdfTooLate();
    }
}

void NetcdfIn::getTimeTransform( double* scale, double* offset )
{
     *scale = ts;
     *offset = to;
}

void NetcdfIn::parseTimeUnits( std::string tunits )
{
     // not yet implemented 
     
     double tmp_ts;
     double tmp_to;
     size_t pos;
     
     tmp_ts = 1.0;
     tmp_to = 0.0;
     
     try {
         if ( (tunits.size() > 11) && (tunits.substr(0,1) == "d" || tunits.substr(0,1) == "D" ) ) {
            if ( tunits.substr(1,10) == "ays since " ) {
               tmp_ts = 1.0;
               pos = 11;      
            }
         } else if ( (tunits.size() > 12) && (tunits.substr(0,1) == "h" || tunits.substr(0,1) == "H" ) ) {
            if ( tunits.substr(1,11) == "ours since " ) {
               tmp_ts = 24.0;
               pos = 12;      
            }
         } else if ( (tunits.size() > 14) && (tunits.substr(0,1) == "m" || tunits.substr(0,1) == "M" ) ) {
            if ( tunits.substr(1,13) == "inutes since " ) {
               tmp_ts = 24.0*60.0;
               pos = 14;      
            }
         } else if ( (tunits.size() > 14) && (tunits.substr(0,1) == "s" || tunits.substr(0,1) == "S" ) ) {
            if ( tunits.substr(1,13) == "econds since " ) {
               tmp_ts = 24.0*60.0*60.0;      
               pos = 14;
            }
         }
     
         if ( tunits.substr(pos,11) == "1-1-1 00:00" ) {
            tmp_to = 693596.00;
         } else {
            // tmp_to = met->cal2Time( tunits.substr(pos,11) );
         }
     
         to = tmp_to;
         ts = tmp_ts;
     } catch(...) {
         std::cerr << "NetcdfIn time units unkown: " << tunits << std::endl;
     }
}

void NetcdfIn::open( std::string file )
{
     int err;
     const char *attr_name;
     const char *var_name;
     char *val;
     int len;
     nc_type attr_type;
     int attr_id;
     int var_id;
     std::string name;
     size_t  attr_size;
     nc_type var_type;
     int var_ndims;
     int var_dims[NC_MAX_VAR_DIMS];
     int  var_natts;
     int dim_id;
     size_t ntimes;
     int time_index;
     size_t istart;
     size_t icount;
     int ndimens;
     int unlimdim_idx;
     char c_vname[NC_MAX_NAME + 1];
     char c_vunits[NC_MAX_NAME + 1];
     double tt0;
     std::string alt_vertname;
     
     if ( is_open ) {
        close();
     }
     
     if ( file != "" ) {
        filename( file );
     }
     
     err = nc_open( fname.c_str(), NC_NOWRITE, &ncid);     
     if ( err != NC_NOERR ) {
        std::cerr << "NetcdfIn::Popen: failed to open file: " << fname << std::endl;
        throw(badNetcdfOpen(err));
     }
     
     is_open = true;
     if ( dbug > 2 ) {
        std::cerr << "NetcdfIn::open: nc_open success!" << std::endl;
     }
     
     // do the initial inquiries to get basic sizes and shapes
     err = nc_inq(ncid, &ndimens, &nvars, &ngatts, &unlimdim_idx);
     if ( err != NC_NOERR ) {
        throw(badNetcdfError(err));
     }
     if ( dbug > 2 ) {
        std::cerr << "NetcdfIn::open: initial inq: " << ndimens << ", " << nvars << ", " << ngatts << ", " << unlimdim_idx << std::endl;
     }
     
     
     // get the base time from the global attributes
     time0 = "";

     name = "Trajectory_start";
     attr_name = name.c_str();
     err = nc_inq_attid( ncid, NC_GLOBAL, attr_name, &attr_id );
     if ( err == NC_NOERR ) {
     
        err = nc_inq_att(ncid, NC_GLOBAL, attr_name, &attr_type, &attr_size );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     
        if ( attr_type == NC_STRING && attr_size == 1 ) {
           
           err = nc_get_att_string( ncid, NC_GLOBAL, attr_name, &val );
           if ( err != NC_NOERR ) {
              throw(badNetcdfError(err));
           }

           time0.assign( val );
           if ( dbug > 1 ) {
              std::cerr << "NetcdfIn::open: base time: " << time0 << std::endl;
           }
           
           free(val);

        }
        // todo: handle the case where the timestamp is a char array

     }

     // handle the dimensions

     // get the dimensional IDs for time and id
     err = nc_inq_dimid( ncid, "time", &did_time);
     if ( err == NC_EBADDIM ) {
        std::cerr << " No 'time' dimension in file" << std::endl;
        throw(badFileConventions());  
     } else if ( err != NC_NOERR ) {
        throw(badNetcdfError(err));
     }     
     err = nc_inq_dimid( ncid, "id", &did_id);
     if ( err == NC_EBADDIM ) {
        std::cerr << " No 'id' dimension in file" << std::endl;
        throw(badFileConventions());  
     } else if ( err != NC_NOERR ) {
        throw(badNetcdfError(err));
     }
     

     // find the time variable, and the number of times
     name = "time";
     var_name = name.c_str();
     
     err = nc_inq_dimid( ncid, var_name, &dim_id );
     if ( err == NC_EBADID || err == NC_EBADDIM ) {
        std::cerr << " No 'time' variable in file" << std::endl;
        throw(badFileConventions());
     } else if ( err != NC_NOERR ) {
        throw(badNetcdfError(err));
     }

     err = nc_inq_dimlen( ncid, dim_id, &ntimes );
     if ( err != NC_NOERR ) {
        throw(badNetcdfError(err));
     }
     if ( dbug > 10 ) {
        std::cerr << "NetcdfIn::open: There are " << ntimes << " times" << std::endl;
     }   
     
     name = "time";
     var_name = name.c_str();
     err = nc_inq_varid( ncid, var_name, &var_id );
     if ( err == NC_ENOTVAR ) {
        std::cerr << " No 'time' variable in file" << std::endl;
        throw(badFileConventions());
     } else if ( err != NC_NOERR ) {
        throw(badNetcdfError(err));
     }
     
     err = nc_inq_var( ncid, var_id, NULL, &var_type, &var_ndims, var_dims, &var_natts);
     if ( err != NC_NOERR ) {
        throw(badNetcdfError(err));
     }
     if ( dbug > 10 ) {
        std::cerr << "NetcdfIn::open: time varid: " << var_id
                  << "; time type: " << var_type
                  << "; time ndims: " << var_ndims
                  << "; time dims[0]: " << var_dims[0] 
                  << "; time natts: " << var_natts << std::endl;
     }
     if ( (var_type != NC_DOUBLE) || (var_ndims != 1) || (did_time != var_dims[0]) ) {
        std::cerr << " 'time' variable has wrong type or wrong dimensions" << std::endl;
        throw(badFileConventions());
     }
     
     // todo: maybe get the timestamp from long_name if we did not get it from the global attribute?
     // todo: maybe check the units as"day"
     

     time_index = 0;
     if ( end ) {
        time_index = ntimes - 1;
     }
     it = time_index;
     icount = 1;
     err = nc_get_vara_double( ncid, var_id, &it, &icount, &tt0 );
     if ( err != NC_NOERR ) {
        throw(badNetcdfError(err));
     }
     if ( dbug > 1 ) {
        std::cerr << "NetcdfIn::open: Parcel time is " << t0 << std::endl;
     }
     t0 = tconv( tt0 );
     
     // now find out about parcels (mainly, how many there are)
     name = "id";
     var_name = name.c_str();
     
     err = nc_inq_dimid( ncid, var_name, &dim_id );
     if ( err == NC_EBADDIM ) {
        std::cerr << name << " variable does not exist" << std::endl;
        throw(badFileConventions());
     } else if ( err != NC_NOERR ) {
        throw(badNetcdfError(err));
     }

     err = nc_inq_dimlen( ncid, dim_id, &np );
     if ( err != NC_NOERR ) {
        throw(badNetcdfError(err));
     }
     if ( dbug > 10 ) {
        std::cerr << "NetcdfIn::open: There are " << np << " parcels" << std::endl;
     }
     // start with the zeroeth prcel
     ip = 0;   
     
     
     // now find the var ids for: longitude, latitude (mandatory)
     
     vid_lon = get_var_id( "lon", true, "", &vtyp_lon );
     if ( dbug > 1 ) {
        std::cerr << "NetcdfIn::open: Got the id for the 'lon' coordinate." <<  std::endl;
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
           badlon = ACOS(2.0); // i.e., NaN
        }
     }    
     
     vid_lat = get_var_id( "lat", true, "", &vtyp_lat );
     if ( dbug > 1 ) {
        std::cerr << "NetcdfIn::open: Got the id for the 'lat' coordinate." <<  std::endl;
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
           badlat = ACOS(2.0); // i.e., NaN
        }
     }    
     
     try {
        vid_z   = get_var_id( vcoord, true, "vertical_coordinate", &vtyp_z );
     } catch (badFileConventions) {
        alt_vertname = findVertical();
        vid_z   = get_var_id( alt_vertname, true, "vertical_coordinate", &vtyp_z );
     }  
     if ( vid_z < 0 ) {
        alt_vertname = findVertical();
        vid_z   = get_var_id( alt_vertname, true, "vertical_coordinate", &vtyp_z );     
     }
     if ( dbug > 1 ) {
        std::cerr << "NetcdfIn::open: Got the id for the vertical coordinate: " << vcoord <<  std::endl;
     }
     if ( vcoord == "" ) {
        err = nc_inq_varname( ncid, vid_z, c_vname );
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
        vcoord.assign( c_vname);
        vunits = findVunits( vid_z );
        vfactor = 1.0;
        if ( vunits == "m" ) {
           // convert meters to kilometers
           vfactor = 1.0/1000.0;
           vunits = "km";
        } else if ( vunits == "Pa" ) {
           vfactor = 1.0/100.0;
           vunits = "hPa";
        }
        if ( dbug > 1 ) {
           std::cerr << "NetcdfIn::open: Found a vertical coordinate " << vcoord <<  " [ " << vunits <<  "]" << std::endl;
        }
     }
     if ( vid_z < 0 ) {
        if ( (vid_status == -1) && (do_status == 1) ) {
           std::cerr <<  "No vertical coordinate was found in the netcdf input file" << std::endl;
           throw (badFileConventions());
        }
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
           badvert = ACOS(2.0); // i.e., NaN
        }
     }    
     
     if ( do_status != 0 ) {   
        vid_status = get_var_id( "status", false, "", &vtyp_status );
        if ( (vid_status == -1) && (do_status == 1) ) {
           std::cerr <<  "status variable does not exist but is required" << std::endl;
           throw (badFileConventions());
        }
        if ( (vid_status != -1) && (do_status == 0) ) {
           vid_status = -1;
        }
        if ( dbug > 1 && vid_status >= 0 ) {
           std::cerr << "NetcdfIn::open: Got the id for the 'status' coordinate." <<  std::endl;
        }
     }
     if ( do_flags != 0 ) {   
        vid_flags  = get_var_id( "flags", false, "", &vtyp_flags );
        if ( (vid_flags == -1) && (do_flags == 1) ) {
           std::cerr <<  "flags variable does not exist but is required" << std::endl;
           throw (badFileConventions());
        }
        if ( (vid_flags != -1) && (do_flags == 0) ) {
           vid_flags = -1;
        }
        if ( dbug > 1 && vid_flags >= 0 ) {
           std::cerr << "NetcdfIn::open: Got the id for the 'flags' coordinate." <<  std::endl;
        }
     }
     if ( do_tag != 0 ) {   
        vid_tag    = get_var_id( "tag",  false, "tag", &vtyp_tag );
        if ( (vid_tag == -1) && (do_tag == 1) ) {
           std::cerr <<  "tag variable does not exist but is required" << std::endl;
           throw (badFileConventions());
        }
        if ( (vid_tag != -1) && (do_tag == 0) ) {
           vid_tag = -1;
        }
        if ( vid_tag >= 0 ) {
           if ( dbug > 1 ) {
              std::cerr << "NetcdfIn::open: Got the id for the tag coordinate." <<  std::endl;
           }
           err = nc_get_att_double( ncid, vid_tag, "missing_value", &badtag);
           if ( err != NC_NOERR ) {
              err = nc_get_att_double( ncid, vid_tag, "_FillValue", &badtag);
              if ( err != NC_NOERR ) {
                 badtag = ACOS(2.0); // i.e., NaN
              }
           }   
        }    
     }

     
     
}

bool NetcdfIn::isOpen()
{
    return is_open;
}

void NetcdfIn::close()
{
     int err;
  
     if ( is_open ) {
     
        err = nc_close(ncid);
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     
        is_open = false;
     
     }

     time0 = "";
     t0 = 0.0;
    
     np = 0;
     ip = -1;
    
     vid_lon = -1;
     vid_lat = -1;
     vid_z = -1;
     vid_status = -1;
     vid_flags = -1;
     vid_tag = -1;

     vtyp_lon = NC_NAT;
     vtyp_lat = NC_NAT;
     vtyp_z = NC_NAT;
     vtyp_status = NC_NAT;
     vtyp_flags = NC_NAT;
     vtyp_tag = NC_NAT;

}

int NetcdfIn::get_var_id( const std::string &varname, bool required, const std::string &flag, int*vtype )
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
     
     
     // if we found the variable, then do a little anity checking
     if ( result >= 0 ) {
        err = nc_inq_var( ncid, result, NULL, &var_type, &var_ndims, var_dims, &natts);
        if ( err != NC_NOERR ) {
           throw(badNetcdfError(err));
        }
     
        if ( var_ndims != 2 ) {
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

void NetcdfIn::rd_real( int var_id, int var_type, size_t n, real* destination )
{
    int err;
    size_t starts[2];
    size_t counts[2];
    ptrdiff_t strides[2];
    float  *fbuffr;
    double *dbuffr;
    short  *sbuffr;
    int    *ibuffr;
    long   *lbuffr;
    
    // trying to read too many Parcels
    if ( ( n + ip ) > np ) {
       std::cerr << " trying to read too many parcels: " << n + ip << " of " << np << std::endl;
       throw(badFileConventions());        
    } 
    
    starts[0]= it;
    starts[1] = ip;
    counts[0] = 1;
    counts[1] = n;
    strides[0] = 1;
    strides[1] = 1;
    
    switch (var_type) {
    case NC_FLOAT:
    
         fbuffr = new float[n];
         err = nc_get_vars_float( ncid, var_id, starts, counts, strides, fbuffr );
         if ( err != NC_NOERR ) {
            throw(badNetcdfError(err));
         }
    
         for ( int i=0; i < n; i++ ) {
             destination[i] = fbuffr[i];
         }
    
         delete[] fbuffr;
         
         break;
    case NC_DOUBLE:
    
         dbuffr = new double[n];
         err = nc_get_vars_double( ncid, var_id, starts, counts, strides, dbuffr );
         if ( err != NC_NOERR ) {
            throw(badNetcdfError(err));
         }
    
         for ( int i=0; i < n; i++ ) {
             destination[i] = dbuffr[i];
         }
    
         delete[] dbuffr;
         
         break;
    case NC_SHORT:
    
         sbuffr = new short[n];
         err = nc_get_vars_short( ncid, var_id, starts, counts, strides, sbuffr );
         if ( err != NC_NOERR ) {
            throw(badNetcdfError(err));
         }
    
         for ( int i=0; i < n; i++ ) {
             destination[i] = sbuffr[i];
         }
    
         delete[] sbuffr;
        
         break;
    case NC_INT:
    
         ibuffr = new int[n];
         err = nc_get_vars_int( ncid, var_id, starts, counts, strides, ibuffr );
         if ( err != NC_NOERR ) {
            throw(badNetcdfError(err));
         }
    
         for ( int i=0; i < n; i++ ) {
             destination[i] = ibuffr[i];
         }
    
         delete[] ibuffr;
      
         break;
    }

}

int NetcdfIn::next_parcel()
{
   if ( ip >= 0 && ip < np ) {
       return ip;
    } else {
       return -1;
    }
}
    
void NetcdfIn::reset()
{
    ip = 0;
}

void NetcdfIn::rd_int( int var_id, int var_type, size_t n, int* destination )
{
    int err;
    size_t starts[2];
    size_t counts[2];
    ptrdiff_t strides[2];
    float  *fbuffr;
    double *dbuffr;
    short  *sbuffr;
    int    *ibuffr;
    long   *lbuffr;
    
    // trying to read too many Parcels
    if ( ( n + ip ) > np ) {
       std::cerr << " trying to read too many parcels: " << n + ip << " of " << np << std::endl;
       throw(badFileConventions());        
    } 
    
    starts[0]= it;
    starts[1] = ip;
    counts[0] = 1;
    counts[1] = n;
    strides[0] = 1;
    strides[1] = 1;
    
    switch (var_type) {
    case NC_FLOAT:
    
         fbuffr = new float[n];
         err = nc_get_vars_float( ncid, var_id, starts, counts, strides, fbuffr );
         if ( err != NC_NOERR ) {
            throw(badNetcdfError(err));
         }
    
         for ( int i=0; i < n; i++ ) {
             destination[i] = roundf(fbuffr[i]);
         }
    
         delete[] fbuffr;
         
         break;
    case NC_DOUBLE:
    
         dbuffr = new double[n];
         err = nc_get_vars_double( ncid, var_id, starts, counts, strides, dbuffr );
         if ( err != NC_NOERR ) {
            throw(badNetcdfError(err));
         }
    
         for ( int i=0; i < n; i++ ) {
             destination[i] = round(dbuffr[i]);
         }
    
         delete[] dbuffr;
         
         break;
    case NC_SHORT:
    
         sbuffr = new short[n];
         err = nc_get_vars_short( ncid, var_id, starts, counts, strides, sbuffr );
         if ( err != NC_NOERR ) {
            throw(badNetcdfError(err));
         }
    
         for ( int i=0; i < n; i++ ) {
             destination[i] = sbuffr[i];
         }
    
         delete[] sbuffr;
        
         break;
    case NC_INT:
    
         ibuffr = new int[n];
         err = nc_get_vars_int( ncid, var_id, starts, counts, strides, ibuffr );
         if ( err != NC_NOERR ) {
            throw(badNetcdfError(err));
         }
    
         for ( int i=0; i < n; i++ ) {
             destination[i] = ibuffr[i];
         }
    
         delete[] ibuffr;
      
         break;
    }

}



void NetcdfIn::apply( Parcel& p )
{
    real lon, lat, z, tag;
    double t;
    int status,flags;
    
    
    lon = 0.0;
    rd_real( vid_lon, vtyp_lon, 1, &lon );

    lat = 0.0;
    rd_real( vid_lat, vtyp_lat, 1, &lat );

    z = 0.0;
    rd_real( vid_z, vtyp_z, 1, &z );
    z = z*vfactor;

    p.setTime( t0 );
    
    if ( vid_tag >= 0 ) {
       tag = badtag;
       rd_real( vid_tag, vtyp_tag, 1, &tag );
    
       p.tag( tag );
    }

    if ( vid_status >= 0 ) {
       status = 0.0;
       rd_int( vid_status, vtyp_status, 1, &status );
    
       p.setStatus( status );
    }
    
    if ( vid_flags >= 0 ) {
       flags = 0.0;
       rd_int( vid_flags, vtyp_flags, 1, &flags );
    
       p.setFlags( flags );
    }
    
    // should only need to check longitude, since for dead Parcels
    // all of lat, lon, z, and tag will be NaN.
    if ( FINITE(lon) && ( lon != badlon ) ) {
       p.setPos( lon, lat );
       p.setZ( z );
    } else {
       p.setNoTrace();
       p.setPos( 0.0, 0.0 );
       p.setZ( 0.0 );
    }
        
    ip = ip + 1;
    
}

void NetcdfIn::apply( Parcel * p, const int n ) 
{
    int i;
    int nn;
    
    //if ( n < 0 ) {
    //   throw (ParcelFilter::badparcelnum());
    //};
    
    nn = n;
    
    if ( nn <= 0 ) {
       nn = np - ip;
    }
    
    for ( i=0; i<nn; i++ ) {
       apply( p[i] );
    }   

}

void NetcdfIn::apply( std::vector<Parcel>& p ) 
{
    std::vector<Parcel>::iterator iip;
    size_t n;
    
    n = p.size();
    if ( n == 0 ) {
       n = np - ip;
       p.resize(n);
    }
    
    for ( iip=p.begin(); iip != p.end(); iip++ ) {
        apply( *iip );
    }   

}

void NetcdfIn::apply( std::list<Parcel>& p ) 
{
    std::list<Parcel>::iterator ip;
    int n;
    
    n = p.size();
    
    if ( n <= 0 ) {
       throw (ParcelFilter::badparcelnum());
    };  
    
    for ( ip=p.begin(); ip != p.end(); ip++ ) {
        apply( *ip );
    }   

}

void NetcdfIn::apply( std::deque<Parcel>& p ) 
{
    std::deque<Parcel>::iterator ip;
    int n;
    
    n = p.size();
    
    if ( n <= 0 ) {
       throw (ParcelFilter::badparcelnum());
    };  
    
     apply( *ip );

}

void NetcdfIn::apply( Flock& p ) 
{
    Flock::iterator itp;
    int n;
    Parcel px;
    bool i_am_root;
    
    n = p.size();
    
    if ( n <= 0 ) {
       throw (ParcelFilter::badparcelnum());
    };  
    
    i_am_root = p.is_root();

    for ( int i=0; i<p.size(); i++ ) {
    
       p.sync();
       
       if ( i_am_root ) {
          // get the parcel
          apply( px );
       }   
       // If we are the root processor, the parcel px
       // is valid, and it is sent to the processor
       // to which it belongs.
       // If we are not the root processor, then
       // then px has no valid value, but the
       // processor will ignore it and receive its
       // parcel value from the root processor.
       p.set( i, px, 0 );

    }   

}

void NetcdfIn::apply( Swarm& p )
{
    Swarm::iterator ip;
    int n;
    Parcel px;
    bool i_am_root;
    
    n = p.size();
    
    if ( n <= 0 ) {
       throw (ParcelFilter::badparcelnum());
    };  
    
    i_am_root = p.is_root();

    for ( int i=0; i<p.size(); i++ ) {

       p.sync();
       
          if ( i_am_root ) {
             // get the parcel
             apply( px );
          }   
          // If we are the root processor, the parcel px
          // is valid, and it is sent to the processor
          // to which it belongs.
          // If we are not the root processor, then
          // then px has no valid value, but the
          // processor will ignore it and receive its
          // parcel value from the root processor.
          p.set( i, px, 0 );

    }   

}
