
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

#include "gigatraj/MetData.hh"
#include <math.h>
#include <iostream>
#include <fstream>
#include <iosfwd>
#include <sstream>

using namespace gigatraj;

// constructor
MetData::MetData()
{ 
     my_pgroup = NULLPTR;
     my_metproc = -1;
     now = time(NULL);
     wfctr = 1.0;
     flags = 0;
     dbug = 0;
     cfgFile = "";
};

// copy constructor
MetData::MetData( const MetData& src )
{

   my_pgroup  = src.my_pgroup;
   my_metproc = src.my_metproc;
   now = time(NULL);
   wfctr = src.wfctr;
   flags = 0;
   dbug = src.dbug;
   cfgFile = src.cfgFile;

}


void MetData::assign( const MetData& src )
{

   my_pgroup  = src.my_pgroup;
   my_metproc = src.my_metproc;
   now = src.now;
   wfctr = src.wfctr;
   flags = src.flags;
   dbug = src.dbug;
   cfgFile = src.cfgFile;

}

void MetData::get_uvw( double time, int n, real* lons, real* lats, real* zs
      , real *u, real *v, real *w)
{
    for (int i=0; i<n; i++ ) {
        u[i] = 0.0;
        v[i] = 0.0;
        w[i] = 0.0;
    }
}

double MetData::NaNTime() const
{
     const char *nanstr = "";
     return nan(nanstr);
}

double MetData::forecastLeadTime( double time )
{
     return NaNTime();
}


void MetData::setOption( const std::string &name, const std::string &value )
{
}

void MetData::setOption( const std::string &name, int value )
{
}

void MetData::setOption( const std::string &name, float value )
{
}

void MetData::setOption( const std::string &name, double value )
{
}


bool MetData::getOption( const std::string &name, std::string &value )
{
    value = "";
    return false;
}


bool MetData::getOption( const std::string &name, int &value )
{
    value = 0;
    return false;
}


bool MetData::getOption( const std::string &name, float &value )
{
    value = 0.0;
    return false;
}


bool MetData::getOption( const std::string &name, double &value )
{
    value = 0.0;
    return false;
}





void MetData::getData( string quantity, double time, int n, real* lons, real* lats, real* zs, real* values, int flags )
{
    
    for ( int i=0; i< n; i++ ) {      
        values[i] = getData( quantity, time, lons[i], lats[i], zs[i], flags);
    }
}

ProcessGrp* MetData::getPgroup( int* met) const
{
    if ( met != NULLPTR ) {
       *met = my_metproc;
    }
    
    return my_pgroup;   

}

void MetData::setPgroup( ProcessGrp *pg, int met )
{

   my_pgroup = pg;
   my_metproc = met;

}

void MetData::sync( int mode )
{
    
    if ( my_pgroup != NULLPTR ) {
       
       my_pgroup->sync();
       
    }
}

void MetData::delay()
{
    // do nothing
}

int MetData::receive_svr_status()
{
   int result;
   
   result = PGR_STATUS_OK;
   
   if ( isMetClient() ) {
       my_pgroup->receive_ints( my_metproc, 1, &result, PGR_TAG_STATUS );
   }

   return result;
}
  
void  MetData::send_svr_status( int status, int client )
{
   if ( ! isMetClient() ) {
       my_pgroup->send_ints( client, 1, &status, PGR_TAG_STATUS );
   }

} 


void MetData::signalMetDone()
{
   int client_req = PGR_CMD_DONE;

   if ( isMetClient() ) {
      // we are not a dedicated met processor.
      // send a "Done" message to the dedicated met processor
      if ( dbug >= 5 ) {
         std::cerr << "MetData::signalMetDone: " << my_pgroup->id() << "/" << my_pgroup->group_id()
                   << " signaling DONE to met proc " << my_metproc << std::endl;
      }
      my_pgroup->send_ints( my_metproc, 1, &client_req, PGR_TAG_REQ );
   }
   
   // everybody sync at this point
   if ( my_pgroup != NULLPTR ) {
      //my_pgroup->sync("MetData::signalMetDone");
      my_pgroup->sync();
   }

}


int MetData::useMet()
{
    
    if ( my_pgroup != NULLPTR ) {
       //my_pgroup->sync("MetData::useMet begins");
       my_pgroup->sync();
    }   

    if ( isMetServer() ) {
        // This is a dedicated met processor
        
        // listen for requests from other processors for our data
        serveMet();
    
        // and return 0, so that the caller can know that
        // data access statements are NOT to be executed
        return 0;
    } else {
        // This is NOT a dedicated met processor;
        // return 1 so that the caller can know if 
        // data access statements should be executed
        return 1;
    }   

}


void MetData::configFile( const std::string &filename )
{
     if ( cfgFile != filename ) {
        cfgFile = filename;
     }
}

std::string MetData::configFile() const
{
    return cfgFile;
}

void MetData::readConfig()
{
    std::string line;
    bool more;
    std::string keyword;
    std::string typ;
    std::string value;
    char sep = ':';
    int valInt;
    float valFlt;
    double valDbl;
    std::string valStr;
    bool valBool;
    int pos;
    
    if ( cfgFile != "" ) {
       std::ifstream input( cfgFile.c_str());
       if ( input.good() ) {
     
          more = true;
          while ( more ) {
     
             std::getline( input, line );
             more = input.good();

             trimString( line );
             
             // skip empty lines
             if ( line.size() > 0 ) {
                // skip comments
                if ( line[0] != '#' ) {
                
                   // parse the line
                   pos = 0;
                   keyword = get_cfg_keyword( line, &pos, ':' );
                   if ( keyword != "" ) {
                   
                      typ = get_cfg_type( line, &pos, ':' );
                      if ( typ != "" ) {
                          
                         value = get_cfg_value( line, &pos );
                         
                         if ( typ == "I" ) {
                            if ( str2int( value, &valInt ) ) {
                               setOption( keyword, valInt );
                            }
                         } else if ( typ == "F" ) {
                            if ( str2float( value, &valFlt ) ) {
                               setOption( keyword, valFlt );
                            }                      
                         } else if ( typ == "D" ) {
                            if ( str2dbl( value, &valDbl ) ) {
                               setOption( keyword, valDbl );
                            }                                            
                         } else if ( typ == "S" ) {
                            setOption( keyword, value );
                         } else if ( typ == "B" ) {
                            if ( str2bool( value, &valBool ) ) {
                               setOption( keyword, valBool );
                            }                                                                     
                         } else {
                             // should never get here
                         }
                      }
                   
                   }
                }  
             }
               
          }
     
          input.close();

       } else {
          std::cerr << "Wanring: could not open " << cfgFile 
                    << " for MetData configuration." << std::endl;
       } 
    }
}

void MetData::trimString( std::string& line ) const
{
     char cc;
     size_t i;
     size_t len;
     
     len = line.size();
     if ( len > 0 ) {
        for ( i=0; i < len; i++ ) {
            cc = line[i];
            if ( cc != ' ' && cc != '\t' ) {
               break;
            }
        }
        if ( i < len ) {
           // found a non-whitespace character
           // trim the leading whitespace from the string
           line = line.substr( i, len - i + 1);
        } else {
           // no non-whitespace found
           // make this the empty string
           line.clear();
        }
        // now trim the end
        len = line.size();
        if ( len > 0 ) {
           for ( i=len - 1; i >= 0; i-- ) {
              cc = line[i];
              if ( cc != ' ' && cc != '\t' ) {
                 break;
              }
           }
           if ( i >= 0 ) {
              line = line.substr( 0, i + 1 );
           } else {
              line.clear();
           }
        }
     }
     
}

std::string MetData::get_cfg_keyword( std::string& line, int* pos, char sep )
{
     std::string result;
     int i;
     int state;
     int len;
     char cc;
     
     result = "";
     
     len = line.size();
     i = *pos;
     state = 0;
     while ( state >= 0 ) {
        if ( i < len ) {

           cc = line[i];
           if ( cc != ' ' && cc != '\t' ) {
              // not whitespace
              
              if ( cc == sep ) {
                 // we have the terminator
                 // successful scan
                 state = -1;
              } else if ( (cc >= 'a' && cc <= 'z')
                       || (cc >= 'A' && cc <= 'Z')
                       || (cc >= '0' && cc <= '9')
                       || ( cc == '_' ) ) {
              
                  if ( state == 0 || state == 1 ) {
                      result = result + cc;
                  
                     state = 1;
                  } else {
                     // this is an error:
                     // whitespace followed a keyword (state == 2)
                     // which is now followed by non-whitespace
                     state = -2;
                  }      
              } else {
                 // unexpected character
                 state = -2;
              }
           } else {
              // whitespace
              
              if ( state == 1 ) {
                 state = 2;
              }
              
           }
        
           i++;
        } else {
           // syntax error: no terminating separator
           state = -2;
        }
     }
     if ( state == -1 ) {
        // success
        *pos = i;
     } else {
        std::cerr << "Warning: Config syntax error in line '" << line << "'" << std::endl;
     }
     
     return result;
} 

std::string MetData::get_cfg_type( std::string& line, int* pos, char sep )
{
     std::string result;
     int i;
     int status;
     int len;
     char cc;
     
     i = *pos;
     result = get_cfg_keyword( line, &i, sep );
     
     if ( (result == "I" )
       || (result == "F" )
       || (result == "D" )
       || (result == "S" )
       || (result == "B" ) 
       ) {
       
        *pos = i;

     } else {
        std::cerr << "Warning: Config syntax error in line '" << line << "'" << std::endl;     
     }  
     
     return result;
}

std::string MetData::get_cfg_value( std::string& line, int* pos )
{
     std::string result;
     int i;
     int status;
     int len;
     
     result = "";
     
     len = line.size();
     i = *pos;
     status = 0;
     while ( status == 0) {
        if ( i < len ) {
        
           result = result + line[i];

           i++;
        } else {
           // syntax error: no terminating separator
           status = -1;
        }
     }
     if ( status > 0 ) {
        *pos = i + 1;
     }
     
     trimString(result); 

     return result;
}

bool MetData::str2int(const std::string &value, int* result  ) const
{
    std::istringstream *numstr; 
    bool status;
    
    status = false;
    numstr = new std::istringstream(value.c_str());
    try {
       *numstr >> *result;
       status = true;
    } catch (...) {
    }
    delete numstr;
    
    return status;
}

bool MetData::str2float(const std::string &value, float* result) const
{
    std::istringstream *numstr; 
    bool status;
    
    status = false;
    
    numstr = new std::istringstream(value);
    try {
       *numstr >> *result;
       status = true;
    } catch (...) {
    }
    delete numstr;
    
    return status;
}

bool MetData::str2dbl(const std::string &value, double* result) const
{
    std::istringstream *numstr; 
    bool status;
    
    status = false;
    
    numstr = new std::istringstream(value);
    try {
       *numstr >> *result;
       status = true;
    } catch (...) {
    }
    delete numstr;
    
    return status;
}

bool MetData::str2bool(const std::string &value, bool* result) const
{
    std::string cc;
    bool status;
    
    status = false;
    
    // extract the first character
    cc = value.substr(0,1);

    // and test it
    if ( cc == "Y" || cc == "y" || cc == "T" || cc == "t" ) {
       *result = true;
       status = true;
    } else {
       if ( cc == "N" || cc == "n" || cc == "F" || cc == "f" ) {
          *result = false;
          status = true;
       } else {
       }
    }
    return status;
}

bool MetData::int2str( int value, std::string& result ) const
{
    bool status;
    std::ostringstream oo;
    
    status = true;
    oo << value;
    result = oo.str();
    
    
    return status;
}

bool MetData::float2str( float value, std::string& result ) const
{
    bool status;
    std::ostringstream oo;
    
    status = true;
    oo << value;
    result = oo.str();
    
    
    return status;
}

bool MetData::dbl2str( double value, std::string& result ) const
{
    bool status;
    std::ostringstream oo;
    
    status = true;
    oo << value;
    result = oo.str();
        
    return status;
}

bool MetData::bool2str( bool value, std::string& result ) const
{
    bool status;
    std::ostringstream oo;
    
    status = true;
    oo << value;
    result = oo.str();
    
    return status;
}



void MetData::debug( int level )
{
    dbug = level;
}

void MetData::setIsoVertical()
{
     flags = flags | MET_ISO;
}

void MetData::clearIsoVertical() 
{
     flags = flags & ( ~ MET_ISO );
}

std::string MetData::u_wind( std::string *units ) const
{
    if ( units != NULL ) {
       *units = "";
    }
    return "";   
}

std::string MetData::v_wind( std::string *units ) const
{
    if ( units != NULL ) {
       *units = "";
    }
    return "";   
}

std::string MetData::w_wind( std::string *units ) const
{
    if ( units != NULL ) {
       *units = "";
    }
    return "";   
}

std::string MetData::vertical( std::string *units ) const
{
    if ( units != NULL ) {
       *units = "";
    }
    return "";   
}

std::string MetData::vunits() const
{
    return "";   
}
