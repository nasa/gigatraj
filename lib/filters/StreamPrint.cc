
#include "config.h"

#include "gigatraj/StreamPrint.hh"
#include <iostream>
#include <sstream>

using namespace gigatraj;

// default constructor
StreamPrint::StreamPrint( const std::string fmtstr )
{
    std::string fs;
    
    nf = 0;
    
    metsrc = NULLPTR;
    
    nobad = 0;
    
    os = &std::cout;
    
    if ( fmtstr == "" ) {
       fs = "%T: %o, %a, %v\n";
    } else {
       fs = fmtstr;
    }
    format( fs );
    
};

// alternate constructor
StreamPrint::StreamPrint( std::ostream& output, const std::string fmtstr ) 
{
    std::string fs;

    os = &(output);
    
    nf = 0;
    
    metsrc = NULLPTR;
    
    nobad = 0;
    
    if ( fmtstr == "" ) {
       fs = "%T: %o, %a, %v\n";
    } else {
       fs = fmtstr;
    }
    format( fs );
};

StreamPrint::StreamPrint(const StreamPrint& src) : ParcelReporter(src)
{
    FmtSpec* fnew;

    os = src.os;
    nf = src.nf;
    metsrc = src.metsrc;
    nobad = src.nobad;
    
     fmt.clear();
     for (int i=0; i < fmt.size(); i++ ) {
        fnew = new FmtSpec;
        *fnew = *(src.fmt[i]);
        fmt.push_back( fnew );
     }
}

StreamPrint& StreamPrint::operator=(const StreamPrint& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }

    this->assign( src ) ;

    return *this;
}

void StreamPrint::assign( const StreamPrint& src)
{
     FmtSpec* fnew;

     ParcelReporter::assign( src );

     os = src.os;
     nf = src.nf;
     metsrc = src.metsrc;
     nobad = src.nobad;
    
     fmt.clear();
     for (int i=0; i < fmt.size(); i++ ) {
        fnew = new FmtSpec;
        *fnew = *(src.fmt[i]);
        fmt.push_back( fnew );
     }
}


// FmtSpec constructor
StreamPrint::FmtSpec::FmtSpec(const std::string type0, int start0, int len0, int fract0, std::string str0, int align0)
{
    align = 0;
    defaults( type0, start0, len0, fract0, str0, align0 );

}


StreamPrint::FmtSpec::~FmtSpec() 
{
}

StreamPrint::FmtSpec::FmtSpec(const StreamPrint::FmtSpec& src)
{
      type = src.type;
      start = src.start;
      len = src.len;
      fract = src.fract;
      align = src.align;
      str = src.str;
      field = src.field;    
}

StreamPrint::FmtSpec& StreamPrint::FmtSpec::operator=(const StreamPrint::FmtSpec& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }

    this->assign( src ) ;
    
    return *this;
}

void StreamPrint::FmtSpec::assign( const StreamPrint::FmtSpec& src)
{
      type = src.type;
      start = src.start;
      len = src.len;
      fract = src.fract;
      align = src.align;
      str = src.str;
      field = src.field;    
}


// settle the defaults of a FmtSpec
void StreamPrint::FmtSpec::defaults( std::string type0, int start0, int len0, int fract0, std::string str0, int align0)
{
    if ( type0 == "" ) {
       type0 = "L";
    }   
    
    if ( align0 != -1 && align != 0 ) {
       align = 1;
    }   

    if ( type0== "T" ) {
       // leave as-is. len < 0 means use the actual string length
    } else if ( type0== "L" ) {
       if ( len0 < 0 ) {
          len0 = str0.length() - start0;
          if ( len0 < 0 ) {
             len0 = 0;
          }   
       }
    } else if ( type0== "t" ) {
       if ( len0 < 0 ) {
          len0 = 16;
       }
       if ( fract0 < 0 ) {
          fract0 = len0 - 5;
          if ( fract0 < 0 ) {
             fract0 = 0;
          }
       }
    } else if ( type0== "F" ) {
       if ( len0 < 0 ) {
          len0 = 8;
       }
       if ( fract0 < 0 ) {
          fract0 = len0 - 5;
          if ( fract0 < 0 ) {
             fract0 = 0;
          }
       }
    } else if ( type0== "o" ) {
       if ( len0 < 0 ) {
          len0 = 8;
       }
       if ( fract0 < 0 ) {
          fract0 = len0 - 5;
          if ( fract0 < 0 ) {
             fract0 = 0;
          }
       }
    } else if ( type0== "a" ) {
       if ( len0 < 0 ) {
          len0 = 7;
       }
       if ( fract0 < 0 ) {
          fract0 = len0 - 4;
          if ( fract0 < 0 ) {
             fract0 = 0;
          }
       }
    } else if ( type0== "v" ) {
       if ( len0 < 0 ) {
          len0 = 9;
       }
       if ( fract0 < 0 ) {
          fract0 = 3;
          if ( fract0 > (len0+3) ) {
             fract0 = 0;
          }
       }
    } else if ( type0== "g" ) {
       if ( len0 < 0 ) {
          len0 = 7;
       }
       if ( fract0 < 0 ) {
          fract0 = len0 - 4;
          if ( fract0 < 0 ) {
             fract0 = 0;
          }
       }
    } else if ( type0== "i" ) {
       if ( len0 < 0 ) {
          len0 = 9;
       }
    } else if ( type0== "c" ) {
       if ( len0 < 0 ) {
          len0 = 1;
       }
    } else if ( type0== "x" ) {
       if ( len0 < 0 ) {
          len0 = 1;
       }
    } else if ( type0== "f" ) {
       if ( len0 < 0 ) {
          len0 = 3;
       }
    } else if ( type0== "s" ) {
       if ( len0 < 0 ) {
          len0 = 3;
       }
    } else if ( type0== "m" ) {
       if ( len0 < 0 ) {
          len0 = 9;
       }
       if ( fract0 < 0 ) {
          fract0 = 3;
          if ( fract0 > (len0+3) ) {
             fract0 = 0;
          }
       }
    } else if ( type0== "x" ) {
       if ( len0 < 0 ) {
          len0 = 9;
       }
    } else if ( type0== "c" ) {
       if ( len0 < 0 ) {
          len0 = 9;
       }
    } else {
       std::cerr << "Bad format type: " << type0 << std::endl;
    }
    
    type = type0;
    start = start0;
    len = len0;
    fract = fract0;
    str = str0;
    align = align0;

}



StreamPrint::~StreamPrint()
{
    clearFormat();
}      

void StreamPrint::clearFormat()
{
    int i;
    
    for ( i=0; i<nf; i++ ) {
        delete fmt.back();
        fmt.pop_back();
    }
    nf = 0;

}

void StreamPrint::omitNonTraced()
{
    nobad = 1;
}

void StreamPrint::includeNonTraced()
{
    nobad = 0;
}

int StreamPrint::nonTraced()
{
    return nobad;  
}

int StreamPrint::s2i( const std::string str ) 
{
    int n;
    
    std::istringstream is(str);
    
    try {
       is >> n;
    } catch (...) {
       std::cerr << "bad numeric format in " << str << std::endl;
       throw (StreamPrint::badprintformat());
    }
    return n;
}

void StreamPrint::format( const std::string fmtstr )
{
   std::string tmpstr;
   int i;
   std::string type;
   std::string numbr;
   std::string metfield;
   int n;
   int start, len, fract, align;
   FmtSpec *ff;
   int state;
   std::string ch;
   bool first;
   
   clearFormat();
   
   i = 0;
   state = 0;
   
   ff = new FmtSpec;
   // start be assuming this is a literal
   ff->type = "L";
   ff->str = "";
   while ( i < fmtstr.size() ) {
      
      // grab the next character
      ch = fmtstr.substr(i,1);
      
      switch (state) {
      case 0: // in literal text, not a "%" sequence
         if ( ch == "%" ) {
            // save whatever we were working on
            fmt.push_back( ff );
            
            // and start a new format spec
            ff = new FmtSpec;
            start = 0;
            len = -1;
            fract = -1;
            align = 1;
            type = "";
            metfield = "";
            first = true;

            // start a % sequence
            state = 1;
         } else {
            // continue literal text
            type = "L";
            ff->str.append( ch );
         }
         break;
      case 1: // just begun a new "%" sequence
      
         if ( ch != "%" ) {
            
            // are we terminating the sequence already with 
            // a recognized format character?
            if ( ch == "T" || ch == "t" || ch == "F" || ch == "o" || ch == "a" || ch == "v" 
              || ch == "i" || ch == "f" || ch == "s" || ch == "c" || ch == "x" || ch == "g" ) {
               
               // set it up with defaults
               ff->defaults( ch );
               
               // end of a sequence, so save it
               fmt.push_back( ff );
               
               // and start a new format spec
               ff = new FmtSpec;
               start = 0;
               len = -1;
               fract = -1;
               align = 1;
               numbr = "";
               metfield = "";
               first = true;
               type = "";
               
               // reset the state to 0  
               state = 0;             

            } else if ( ch == "{"  ) {
            
               // set it up with defaults
               ff->defaults( "m" );
               
               state = 10;               
            
            } else {
               // no, the user is specifying something more
               
               if ( ch == "-" ) {
                  align = -1;
               } else {
                  numbr = ch;
               }
               // expect digits or a sewuence terminator next
               state = 2;
               
            }
            
         } else {
            // this is just a "%%" sequence.
            // set it up as the start of a literal sequence
            ff->type = "L";
            ff->str = "%";
            state = 0;
         }
      
         break;
      case 2: // in a digit sequence
         
         if ( ch == "0" || ch == "1" || ch == "2"  || ch == "3" || ch == "4" 
           || ch == "5" || ch == "6" || ch == "7" || ch == "8" || ch == "9" ) {
           
           // it's a digit, so add it to the number so far
           numbr.append(ch);
           
         } else if ( ch == "." ) {
         
            // end of the first of a pair of numbers
            // convert the first to a number
            n = s2i( numbr );
            
            first = false;
            
            if ( type == "T" ) {
               // CalTime type: start.len
               start = n;
            } else {
               // all numeric types: len.fract
               len = n;
            }
            
            // reset the number string in anticipation of the second in the pair
            numbr = "";
         
         } else if ( ch == "T" || ch == "t" || ch == "F" || ch == "o" || ch == "a" || ch == "v" 
                  || ch == "i" || ch == "f" || ch == "s" || ch == "c" || ch == "x" || ch == "g" ) {
               // termination of a % sequence
               
               type = ch;
               
               if ( numbr != "" ) {
                  // the end of s number. convert it
                  n = s2i( numbr );
            
                  if ( first ) {
                     if ( type == "T" ) {
                        // CalTime type: start.len
                        start = n;
                     } else {
                        // all numeric types: len.fract
                        len = n;
                     }                  
                  } else {
                     if ( type == "T" ) {
                        // CalTime type: start.len
                        start = len;
                        len = n;
                     } else {
                        // all numeric types: len.fract
                        fract = n;
                     }                  
                  
                  }
               }

               // set it up with what we have acquired so far
               ff->defaults( ch, start, len, fract, "", align );
               
               // end of a sequence, so save it
               fmt.push_back( ff );
               
               // reset the state 

               // and start a new format spec
               ff = new FmtSpec;
               start = 0;
               len = -1;
               fract = -1;
               align = 1;
               numbr = "";
               metfield = "";
               first = true;
               type = "";
         
               // reset the state to 0  
               state = 0;             

         } else if ( ch == "{"  ) {      
                                         
               // not quite at termination of a %i,j{field}m sequence
               
               type = "m";
               
               if ( numbr != "" ) {
                  // the end of s number. convert it
                  n = s2i( numbr );
            
                  if ( first ) {
                        // all numeric types: len.fract
                        len = n;
                  } else {
                        // all numeric types: len.fract
                        fract = n;
                  }
               }

               // set it up with what we have acquired so far
               ff->defaults( type, start, len, fract, "", align );
                                         
               state = 10;                  
                                         
         } else {
             std::cerr << "Bad format: " << fmtstr << std::endl;
             throw (StreamPrint::badprintformat());   
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
            
               // end of a sequence, so save it
               ff->field = metfield;
               fmt.push_back( ff );
               
               // reset the state 

               // and start a new format spec
               ff = new FmtSpec;
               start = 0;
               len = -1;
               fract = -1;
               align = 1;
               numbr = "";
               metfield = "";
               first = true;
               type = "";
         
         } else {          
             std::cerr << "Bad format: " << fmtstr << std::endl;
             throw (StreamPrint::badprintformat());   
         }
         
         // read for the next sequence
         state = 0;

         break;
      }
         
      i++;
   }
   // finish up with what we have acquired so far
   
   if ( state == 0 ) {
      // in literal
      // add it only if not blank
      if ( ff->str != "" ) {
         fmt.push_back( ff );      
      } else {
         delete ff;
      }
   } else {
      // just started a '%' sequence without finishing it
      std::cerr << "Bad end to format " << fmtstr << std::endl;
      throw (StreamPrint::badprintformat());
   }
   
   // note how many format elements we ended up with
   nf = fmt.size();


}

std::string StreamPrint::print( const Parcel& p, MetData *metsrc, int index )
{
    std::string date;
    double time;
    real lat, lon, vert;
    real fval;
    double dval;
    int i;
    std::string  type, fmtx, val;
    int start, len, fract;
    std::string stmp;
    FmtSpec *ff;
    std::ostringstream oo;
    MetData* met;
    int st, fl;

    
    std::string output = "";

    // we use our own private ostringstream here
    // to construct the output string, instead of 
    // sending things directly to the user's ostream,
    // because we are manipulating various settings
    // for formatting, and we do not want to disturb
    // the user's ostream, and we are not completely sure
    // what are the capabilities of the user's ostream.
    oo.str("");
    
    for ( i=0; i<nf; i++ ) {
        ff = fmt[i];
        type = ff->type;
        if ( type == "L" ) {
           // literal. Just use what is given.
           oo << ff->str;
        } else if ( type == "T" ) {
           // calendar time. extract the substring and use that.

           time = p.getTime();
           date = metsrc->time2Cal( time, 3 );
           
           len = ff->len;
           if ( len < 0 ) {
              len = date.size();
           }
           
           // extract
           oo << date.substr( ff->start, len );
           
        } else if ( type == "t" ) {
           // model time

           time = p.getTime();

           oo.setf( std::ios::fixed );
           oo.width( ff->len );
           oo.precision( ff->fract );
           oo << time;
           
        } else if ( type == "o" ) {
           // parcel longitude
   
           lon = p.getLon();

           oo.setf( std::ios::fixed );
           oo.width( ff->len );
           oo.precision( ff->fract );
           oo << lon;
           
        } else if ( type == "a" ) {
           // parcel latitude

           lat = p.getLat();

           oo.setf( std::ios::fixed );
           oo.width( ff->len );
           oo.precision( ff->fract );
           oo << lat;
           
        } else if ( type == "v" ) {
           // parcel vertical coordinate

           vert = p.getZ();

           oo.setf( std::ios::fixed );
           oo.width( ff->len );
           oo.precision( ff->fract );
           oo << vert;
           
        } else if ( type == "g" ) {
           // parcel tag

           dval = p.tag();

           oo.setf( std::ios::fixed );
           oo.width( ff->len );
           oo.precision( ff->fract );
           oo << dval;
           
        } else if ( type == "m" ) {
           // met source field

           if ( ff->field != "" ) {              
              fval = p.field( ff->field, metsrc );           
           } else {
              fval = 0.0;
           }
           oo.setf( std::ios::fixed );
           oo.width( ff->len );
           oo.precision( ff->fract );
           oo << fval;
           
        } else if ( type == "F" ) {
           // forecast lead time

           dval = metsrc->forecastLeadTime( p.getTime() );
           oo.setf( std::ios::fixed );
           oo.width( ff->len );
           oo.precision( ff->fract );
           oo << dval;
           
        } else if ( type == "i" ) {
           // parcel index

           oo.setf( std::ios::fixed );
           oo.width( ff->len );
           oo.precision( 0 );
           oo << index;

        } else if ( type == "c" ) {
           // parcel index

           oo << "c";
           if ( ff->len > 1 ) {
              for ( int i=1; i<ff->len; i++ ) {
                  oo << "c";
              }
           }
           
        } else if ( type == "x" ) {
           // parcel index

           oo << "x";
           if ( ff->len > 1 ) {
              for ( int i=1; i<ff->len; i++ ) {
                  oo << "x";
              }
           }
           
        } else if ( type == "f" ) {
           // parcel flags

           fl = p.flags();
           
           oo.setf( std::ios::fixed );
           oo.width( ff->len );
           oo.precision( 0 );
           oo << fl;
           
        } else if ( type == "s" ) {
           // parcel status

           st = p.status();
           
           oo.setf( std::ios::fixed );
           oo.width( ff->len );
           oo.precision( 0 );
           oo << st;
           
        } else {
           std::cerr << "unknown format type:" << type << std::endl;
        }
        
        
    }
    
    output = oo.str();

    return output;
}


void StreamPrint::setMet( MetData* met ) {

     metsrc = met;

}
      
MetData* StreamPrint::getMet() {

     return metsrc;
     
}   

bool StreamPrint::printThis( const Parcel& p ) const
{
     if ( ( nobad == 0 ) || ( ! p.queryNoTrace() ) ) {
        return true;
     } else {
        return false;
     }   
}

bool StreamPrint::printThis( const Parcel* p ) const
{
     if ( ( nobad == 0 ) || ( ! p->queryNoTrace() ) ) {
        return true;
     } else {
        return false;
     }   
}

// prints a single Parcel
void StreamPrint::apply( Parcel& p )
{
    std::string output;
    MetData *metsrc;

    metsrc = p.getMet();
    
    if (printThis(p)) {
       output = print( p, metsrc );
       try {
          *(os) << output;
       } catch (std::ios::failure) {
          throw (StreamPrint::badstreamprint());
       }   
    }
};

// prints an array of Parcels
void StreamPrint::apply( Parcel * const p, const int n )
{
    std::string output;
    MetData *metsrc;
    int i;
    
    if ( n < 0 ) {
       throw (ParcelFilter::badparcelnum());
    };

    metsrc = p[0].getMet();
    // everybody sync up as we begin
    metsrc->sync(1);
    
    for ( i=0; i<n; i++ ) {
       if (printThis(p)) {
          output = print( p[i], metsrc, i );
          try {
             *(os) << output;
          } catch (std::ios::failure) {
             throw (StreamPrint::badstreamprint());
          }
       }   
    }   

};



// print a vector of Parcels
void StreamPrint::apply( std::vector<Parcel>& p )
{
    std::string output;
    MetData *metsrc;
    std::vector<Parcel>::iterator ip;
    int n;
    int i;
    
    n = p.size();
    
    if ( n <= 0 ) {
       throw (ParcelFilter::badparcelnum());
    };  
    
    metsrc = p[0].getMet();
    // everybody sync up as we begin
    metsrc->sync(1);
    
   for ( ip=p.begin(), i=0; ip != p.end(); ip++, i++ ) {
      if (printThis(*ip)) {
         output = print( *ip, metsrc, i );
         try {
            *(os) << output;
         } catch (std::ios::failure) {
            throw (StreamPrint::badstreamprint());
         }   
      }   
   }   

};


// print a list of Parcels
void StreamPrint::apply( std::list<Parcel>& p )
{
    std::string output;
    MetData *metsrc;
    std::list<Parcel>::iterator ip;
    int n;
    int i;
    
    n = p.size();
    
    if ( n <= 0 ) {
       throw (ParcelFilter::badparcelnum());
    };  
    
    
   for ( ip=p.begin(), i=0; ip != p.end(); ip++, i++ ) {
      if ( ip == p.begin() ) {      
         metsrc = ip->getMet();
          // everybody sync up as we begin
          metsrc->sync(1);
      }
      if (printThis(*ip)) {
         output = print( *ip, metsrc, i );
         try {
           *(os) << output;
         } catch (std::ios::failure) {
            throw (StreamPrint::badstreamprint());
         }   
      }   
   }   

};

// print a deque of Parcels
void StreamPrint::apply( std::deque<Parcel>& p )
{
    std::string output;
    MetData *metsrc;
    std::deque<Parcel>::iterator ip;
    int n;
    int i;
    
    n = p.size();
    
    if ( n <= 0 ) {
       throw (ParcelFilter::badparcelnum());
    };  
    
    for ( ip=p.begin(), i=0; ip != p.end(); ip++, i++ ) {
       if ( ip == p.begin() ) {      
          metsrc = ip->getMet();
          // everybody sync up as we begin
          metsrc->sync(1);
       }
       if (printThis(*ip)) {
          output = print( *ip, metsrc, i );
          try {
             *(os) << output;
          } catch (std::ios::failure) {
             throw (StreamPrint::badstreamprint());
          }   
       }   
    }   

};


// print a Flock of Parcels
void StreamPrint::apply( Flock& p )
{
    std::string output;
    MetData *metsrc;
    int n;
    int i;
    bool gotit;
    Parcel *px;
    bool i_am_root;
    
    n = p.size();
    
    if ( n <= 0 ) {
       throw (ParcelFilter::badparcelnum());
    };  
  

    i_am_root = p.is_root();

    metsrc = p.getMet();
    // everybody sync up as we begin
    metsrc->sync(1);
        
    for ( i=0; i<n; i++ ) {

        // the processors all need to be synchronized, so that
        // as one sends the ith parcel, the root is ready to receive
        // the ith parcel.
        //p.sync("StreamPrint::apply");

        try {
           // get the parcel.
           // If this is the root processor, we get a valid pointer.
           // If this is the non-root owner processor, we also get a valid pointer 
           // If this is a non-root non-owner processor for the ith parcel, we get NULLPTR
           // or a badparcelindex error.
           px =  p.parcel( i, 1 );
           // But we only want the case of the root processor
           gotit = ( ( px != NULLPTR ) && i_am_root );

        } catch (Flock::badparcelindex()) {
          // no problem. ignore this. 
          gotit = false; 
        }; 
        
        if ( gotit ) { 
           // We are the root processor, and we got the parcel. 
           // we are in business for this parcel.
           if ( i == 0 ) {
              metsrc = px->getMet();
           }
  
           if (printThis(*px)) {
              output = print( *px, metsrc, i );
              try {
                 *(os) << output;
              } catch (std::ios::failure) {
                 throw (StreamPrint::badstreamprint());
              }
           }

        }

   }   

};


// print a Swarm of Parcels
void StreamPrint::apply( Swarm& p )
{
    std::string output;
    MetData *metsrc;
    int n;
    int i;
    bool gotit;
    Parcel *px;
    bool i_am_root;
    
    n = p.size();
    
    if ( n <= 0 ) {
       throw (ParcelFilter::badparcelnum());
    };  
  

    i_am_root = p.is_root();

    metsrc = p.getMet();
    // everybody sync up as we begin
    metsrc->sync(1);
        
    for ( i=0; i<n; i++ ) {

        // the processors all need to be synchronized, so that
        // as one sends the ith parcel, the root is ready to receive
        // the ith parcel.
        //p.sync("StreamPrint::apply");

        try {
           // get the parcel.
           // If this is the root processor, we get a valid pointer.
           // If this is the non-root owner processor, we also get a valid pointer 
           // If this is a non-root non-owner processor for the ith parcel, we get NULLPTR
           // or a badparcelindex error.
           px =  p.parcel( i, 1 );
           // But we only want the case of the root processor
           gotit = ( ( px != NULLPTR ) && i_am_root );

        } catch (Swarm::badparcelindex()) {
          // no problem. ignore this. 
          gotit = false; 
        }; 
        
        if ( gotit ) { 
           // We are the root processor, and we got the parcel. 
           // we are in business for this parcel.
           if ( i == 0 ) {
              metsrc = px->getMet();
           }
  
           if (printThis(*px)) {
              output = print( *px, metsrc, i );
              try {
                 *(os) << output;
              } catch (std::ios::failure) {
                 throw (StreamPrint::badstreamprint());
              }
           }
           
           delete px;

        }

   }   

};

