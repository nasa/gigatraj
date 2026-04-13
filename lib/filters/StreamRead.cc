
#include "config.h"

#include "gigatraj/StreamRead.hh"
#include <iostream>
#include <sstream>

using namespace gigatraj;


// default constructor
StreamRead::StreamRead( const std::string fmtstr )
{
    std::string fs;

    is = &std::cin;
    
    nf = 0;

    if ( fmtstr == "" ) {
       fs = "%T: %o, %a, %v\n";
    } else {
       fs = fmtstr;
    }
    format( fs );

};


// alternate constructor
StreamRead::StreamRead( std::istream& input,  const std::string fmtstr ) 
{
    std::string fs;

    is = &(input);

    nf = 0;

    if ( fmtstr == "" ) {
       fs = "%T: %o, %a, %v\n";
    } else {
       fs = fmtstr;
    }
    format( fs );
};


// destructor
StreamRead::~StreamRead()
{
     clearFormat();
}

StreamRead::StreamRead(const StreamRead& src) : ParcelInitializer(src)
{
    FmtSpec* fnew;
    
    is = src.is;
    nf = src.nf;
    
    fmt.clear();
    for (int i=0; i < fmt.size(); i++ ) {
       fnew = new FmtSpec;
       *fnew = *(src.fmt[i]);
       fmt.push_back( fnew );
    }
}

StreamRead& StreamRead::operator=(const StreamRead& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }

    this->assign( src ) ;
    
    return *this;
}

void StreamRead::assign( const StreamRead& src)
{
     FmtSpec* fnew;
    
     ParcelInitializer::assign( src );
     is = src.is;
     nf = src.nf;

     fmt.clear();
     for (int i=0; i < fmt.size(); i++ ) {
        fnew = new FmtSpec;
        *fnew = *(src.fmt[i]);
        fmt.push_back( fnew );
     }
}


void StreamRead::ingest( Parcel& p )
{
   string input;
   real lat, lon, z;
   double time;
   string date;
   int i;
   int j;
   FmtSpec *ff;
   double fval;
   string  type, fmtx, val;
   int start, len, fract;
   std::string stmp;
   int spos;
   int slen;
   int vlen;
   int ival;
   int rlen;
   int rstart;
   MetData* metsrc;
   string skipstr;
   
   //std::cerr << " entry into ingest()" << std::endl;
   
   // pull the next line from the input stream into a string
   std::getline( *(is), input );

   //std::cerr << "input string <<" << input << ">>" << std::endl;
   
   if ( nf > 0 ) {
   
      slen = input.size();
      spos = 0;
      
      for ( i=0; i<nf; i++ ) {
      
          ff = fmt[i];
          type = ff->type;
      
          //std::cerr << "looking for fmt " << type << " at pos " << spos << std::endl;
      
          if ( type == "L" ) {
             // literal. Compare what is given to the input.
             
             //std::cerr << "getting literal from input: " << input.substr(spos, slen-spos) << std::endl;
             val = ff->str;
             len = val.size();
             if ( len > (slen - spos) ) {
                // remaining portion of the input string 
                // is not long enough to match the entire literal
                std::cerr << "input is shorter than format: " << input.substr(spos, slen-spos) << std::endl;
                throw (badinitrejected());
             }
             
             for ( j = 0; j<len; j++ ) {
                // the literal must match the input, character for character
                if ( input[spos] != val[j] ) {
                   std::cerr << "input does not match literal <<" << val << ">> : " 
                   << " <<" << input.substr(spos, slen-spos) << ">>" << std::endl;
                   throw (badinitrejected());
                }
                spos++;
             }
             
          } else if ( type == "T" ) {
             // calendar time

             metsrc = p.getMet();
             time = p.getTime();
             date = metsrc->time2Cal( time, 3 );
             rlen = date.size();
             len = ff->len;
             start = ff->start;
             //std::cerr << "%T: align,len,start = " << ff->align << " " << ff->len << " " << ff->start << std::endl;
             //std::cerr << "    init T time = <<" << date << ">>" << std::endl;             
             //std::cerr << "    spos, slen =" << spos << " " << slen << std::endl;
             
             if ( len < 0 ) {
                len = rlen;
                // or maybe scan ahead to the first non-whitespace character in the input?
             }
             if ( ff->align >= 0 ) {
                // start replacing this many characters from the 
                // beginning of the date string
                rstart = start;
             } else {
                // start replacing this many characters from the end of the date string
                // well, actually the final character is at (rlen - 1 - start)
                // this is the actual starting position
                rstart = rlen - start - len;
             }

             //std::cerr << "     rstart, rlen, len = " << rstart << " " << rlen << " " << len << std::endl;             
             
             if ( len > (slen - spos) ) {
                // not enough characters remaining in the input string 
                std::cerr << "input is shorter than format: " << input.substr(spos, slen-spos) << std::endl;
                throw (badinitrejected());
             }
             val = input.substr( spos, len );
             //std::cerr << "     val = <<" << val << ">>" << std::endl;             
             spos = spos + len;
             
             // sanity check
             if ( (rstart < 0 ) || (rstart + len) > rlen ) {
                // not enough characters in the date string
                std::cerr << "output Time is badly positioned " 
                          << ff->align << " " << ff->len << "." << ff->start 
                          << " in " << input.substr(spos, slen-spos) << std::endl;
                throw (badinitrejected());
             }
             
             // replace
             date.replace( rstart, len, val );
             //std::cerr << "     date = <<" << date << ">>" << std::endl;             

             //std::cerr << "final T time = <<" << date << ">>" << std::endl;             
             
             time = metsrc->cal2Time( date );
             
             p.setTime( time );
             
          } else if ( type == "t" ) {
             // model time

             //std::cerr << "getting time from input: " << input.substr(spos, slen-spos) << std::endl;
             spos = s2d( input, &fval, spos, ff->len, ff->fract ); 
             
             p.setTime( fval );
                          
          } else if ( type == "o" ) {
             // parcel longitude
   
             lat = p.getLat();

             //std::cerr << "getting lon from input: <<" << input.substr(spos, slen-spos) 
             //<< ">>" << std::endl;
             spos = s2d( input, &fval, spos, ff->len, ff->fract ); 
             lon = fval;
             
             p.setPos( lon, lat );
             
          } else if ( type == "a" ) {
             // parcel latitude

             lon = p.getLon();

             //std::cerr << "getting lat from input: " << input.substr(spos, slen-spos) << std::endl;
             spos = s2d( input, &fval, spos, ff->len, ff->fract ); 
             lat = fval;
             
             p.setPos( lon, lat );
             

             
          } else if ( type == "v" ) {
             // parcel vertical coordinate

             //std::cerr << "getting z from input: " << input.substr(spos, slen-spos) << std::endl;
             spos = s2d( input, &fval, spos, ff->len, ff->fract ); 
             
             p.setZ( fval );
             
          } else if ( type == "g" ) {
             // parcel tag

             //std::cerr << "getting tag from input: " << input.substr(spos, slen-spos) << std::endl;
             spos = s2d( input, &fval, spos, ff->len, ff->fract ); 
             
             p.tag( fval );
             
          } else if ( type == "m" ) {
             // met source field

            // std::cerr << "getting met from input: " << input.substr(spos, slen-spos) << std::endl;
             spos = s2d( input, &fval, spos, ff->len, ff->fract ); 
             
             // (no nothing with the value retrieved)
             
          } else if ( type == "f" ) {
             // parcel flags

             //std::cerr << "getting flags from input: " << input.substr(spos, slen-spos) << std::endl;
             spos = s2int( input, &ival, spos, ff->len ); 
             
             p.setFlags( ival );
             
          } else if ( type == "s" ) {
             // parcel status

             //std::cerr << "getting status from input: " << input.substr(spos, slen-spos) << std::endl;
             spos = s2int( input, &ival, spos, ff->len ); 
             
             p.setStatus( ival );
             
          } else if ( type == "i" ) {
             // parcel index

             //std::cerr << "getting index from input: " << input.substr(spos, slen-spos) << std::endl;
             spos = s2int( input, &ival, spos, ff->len ); 
             
             // (do nothing with the value retrieved)
             
          } else if ( type == "c" ) {
             // arbitrary characters to skip

             //std::cerr << "getting c from input: " << input.substr(spos, slen-spos) << std::endl;
             spos = s2c( input, 0, skipstr, spos, ff->len ); 
             
             // (do nothing with the value retrieved)
             
          } else if ( type == "x" ) {
             // arbitrary characters to skip

             //std::cerr << "getting x from input: " << input.substr(spos, slen-spos) << std::endl;
             spos = s2c( input, 1, skipstr, spos, ff->len ); 
             
             // (do nothing with the value retrieved)
             
          } else {
             std::cerr << "unknown format type:" << type << std::endl;
             throw (badinitformat());
          }
          
          
      }
   
   } else {
      throw (badinitformat());
   }

}


// init one Parcel
void StreamRead::apply( Parcel& p ) 
{
    ingest( p );    
};


// init an array of Parcels
void StreamRead::apply( Parcel * const p, const int n )
{
    int i;
    
    if ( n < 0 ) {
       throw (ParcelFilter::badparcelnum());
    };
    
    for ( i=0; i<n; i++ ) {
       try {
          ingest( p[i] );
       } catch (std::ios::failure) {
          throw (StreamRead::badinitstream());
       }   
    }   

};

// init a vector of Parcels
void StreamRead::apply( std::vector<Parcel>& p )
{
    std::vector<Parcel>::iterator ip;
    int n;
    
    n = p.size();
    
    if ( n <= 0 ) {
       throw (ParcelFilter::badparcelnum());
    };  
    
    for ( ip=p.begin(); ip != p.end(); ip++ ) {
       try {
          ingest( *ip );
       } catch (std::ios::failure) {
          throw (StreamRead::badinitstream());
       }   
    }   

};


// init a list of Parcels
void StreamRead::apply( std::list<Parcel>& p )
{
    std::list<Parcel>::iterator ip;
    int n;
    
    n = p.size();
    
    if ( n <= 0 ) {
       throw (ParcelFilter::badparcelnum());
    };  
    
    for ( ip=p.begin(); ip != p.end(); ip++ ) {
       try {
          ingest( *ip );
       } catch (std::ios::failure) {
          throw (StreamRead::badinitstream());
       }   
    }   

};


// init a deque of Parcels
void StreamRead::apply( std::deque<Parcel>& p )
{
    std::deque<Parcel>::iterator ip;
    int n;
    
    n = p.size();
    
    if ( n <= 0 ) {
       throw (ParcelFilter::badparcelnum());
    };  
    
    try {
       ingest( *ip );
    } catch (std::ios::failure) {
       throw (StreamRead::badinitstream());
    }   

};

// init a Flock of Parcels
void StreamRead::apply( Flock& p )
{
    Flock::iterator ip;
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
       
       try {
          if ( i_am_root ) {
             // get the parcel
             ingest( px );
          }   
          // If we are the root processor, the parcel px
          // is valid, and it is sent to the processor
          // to which it belongs.
          // If we are not the root processor, then
          // then px has no valid value, but the
          // processor will ignore it and receive its
          // parcel value from the root processor.
          p.set( i, px, 0 );

       } catch (std::ios::failure) {
          throw (StreamRead::badinitstream());
       }   
    }   

};


// init a Swarm of Parcels
void StreamRead::apply( Swarm& p )
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
       
       try {
          if ( i_am_root ) {
             // get the parcel
             ingest( px );
          }   
          // If we are the root processor, the parcel px
          // is valid, and it is sent to the processor
          // to which it belongs.
          // If we are not the root processor, then
          // then px has no valid value, but the
          // processor will ignore it and receive its
          // parcel value from the root processor.
          p.set( i, px, 0 );

       } catch (std::ios::failure) {
          throw (StreamRead::badinitstream());
       }   
    }   

};



int StreamRead::s2i( const std::string str ) 
{
    int n;
    
    std::istringstream is(str);
    
    try {
       is >> n;
    } catch (...) {
       std::cerr << "bad numeric format in " << str << std::endl;
       throw (StreamRead::badinitformat());
    }
    return n;
}

std::string StreamRead::i2s( int i ) const
{
    std::string result;
    std::ostringstream cc;
    
    cc << i;
    
    result = cc.str();
    
    return result;
}

int StreamRead::s2d( const std::string &str, double *result, int beg, int wid, int dec ) 
{
    int intg;
    int pos;
    bool done;
    double intpart;
    double frcpart;
    int exppart;
    double fractor;
    int cpos;
    char cc;
    int state;
    double sygn;
    double esygn;
    bool more;
    int len;
    
    *result = 0.0;
    
    len = str.size();
    
    intg = -1;
    if ( wid > 0 && dec < (wid-1) ) {
       intg = wid - 1 - dec;
    }
    
    if ( str.empty() ) {
        throw (badinitrejected());  
    }

    // std::cerr << "s2d: input <<" << str << ">>" << std::endl;
    intpart = 0.0;
    frcpart = 0.0;
    exppart = 0;
    fractor = 1.0;
    sygn = 1.0;
    esygn = 1.0;
    state = 0;
    cpos = 0;
    more = true;
    while ( more ) {
       cc = str.at( beg + cpos );

       //std::cerr << "   " << cpos + beg << " : " << cpos << " '" << cc << "' " << state << "; " 
       //<< sygn <<  " " << intpart << " " << frcpart << " " << esygn << " " << exppart << std::endl;      
       switch (state) {
       case 0: // beginning; scans past whitespace
               // until not-whitespace, which must be 
               // a sign, a digit, or a decimal point
            switch (cc) {
            case ' ': 
                break;
            case '+':
                state = 1; // start scanning integer or fractional part digits next
                break;
            case '-':
                state = 1; // start scanning integer or fractional part digits next
                sygn = -1.0;
                break;
            case '.':
                state = 10; // start fractional digits next
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                state = 5; // continue scanning integer digits next
                intpart = cc - '0';
                break;
            default:
                state = 50;  
                break;
            }
            break;
       case 1: // past a sign. scan for digits or a decimal point
            switch (cc) {
            case '.':
                state = 10; // start fractional digits next
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                state = 5; // continue scanning integer digits next
                intpart = cc - '0';
                break;
            default:
                state = 50;  
                break;
            }
            break;
       case 5: // in the integer digits. scan for more digits, a decimal point, or an exponent
            switch (cc) {
            case '.':
                state = 10; // start fractional digits next
                break;
            case 'e':
            case 'E':
                state = 15; // start exponent scan
                break;    
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                intpart = intpart*10 + (cc - '0');
                break;
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                state = 40;  
                break;
            default:
                state = 50;  
                break;
            }
            break;
       case 10: // in the fractional digits. scan for more digits, or an exponent
            switch (cc) {
            case 'e':
            case 'E':
                state = 15; // start exponent scan
                break;    
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                fractor = fractor / 10.0;
                frcpart = frcpart + (cc - '0')*fractor;
                break;
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                state = 40;  
                break;
            default:
                state = 50;  
                break;
            }
            break;
       case 15: // in the exponent, scanning for a sign or digits
            switch (cc) {
            case '-':
                state = 17;
                esygn = -1.0;
                break;    
            case '+':
                state = 17;
                break;    
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                state = 17;
                exppart = exppart*10 + (cc - '0');
                break;
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                state = 40;  
                break;
            default:
                state = 50;  
                break;
            }
            break;
       case 17: // in the exponent, scanning for more digits
            switch (cc) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                exppart = exppart*10 + (cc - '0');
                break;
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                state = 40;  
                break;
            default:
                state = 50;  
                break;
            }
            break;
       case 40: // whitespace: either the end of the scan, or whitespace added at the end
            if ( cc != ' ' && cc != '\t' ) {
               // not in whitespace on the same line anymore
               state = 50;
            }
            break;       
       case 50: // some unexpected character: scan finished
            // (should never actually get here)
            break;       
       }
    
       if ( wid <= 0 ) {
          if ( state == 40 || state == 50 || (beg + cpos) >= (len - 1) ) {
             // encountered an invalid expression character, 
             // or the end of string,
             // but no input width was specified, 
             // so we assume that what we got so far is ok,
             // and we are done now.
             
             more = false;
          }  
       }  else {
          // a width was specified
          
          if ( cpos >= (wid - 1) || (beg + cpos) >= (len - 1) ) {
             // we reached the end of the width 
             if (state != 50) {
               // we have not encountered any unexpected characters,
               // so we must be done
               more = false;
    
               // (if there is no more for this field, then cpos will not
               // be incremented below. But because this was a fixed-width
               // field, cpos currently ppoints to the last character
               // that is part of this field, when it should point
               // one character beyond that, for returning to the caller. Fix that here.)
               if ( (beg + cpos) < (len - 1) ) {
                 cpos++;
               }
               
             } else {
               // bad number in input
               throw (badinitrejected());
             }
          }
          if ( dec > 0 && intg >= 0 && cpos == intg && cc != '.' ) {
             // no decimal point where needed
             throw (badinitrejected());
          }
       }
       
       if ( more ) {
          // on to the next character position
          cpos ++;       
       } else {
          // all done, so put the parts together
          
          *result = sygn*( intpart + frcpart );
          if ( exppart > 0 ) {
             *result = *result * pow( 10.0, esygn*exppart );
          } 
       }

    } // end of while loop
        
       
//std::cerr << "Result = " << *result << std::endl;
    
    return cpos + beg;
}


int StreamRead::s2int( const std::string &str, int *result, int beg, int wid )
{
    int intg;
    int pos;
    bool done;
    int intpart;
    int cpos;
    char cc;
    int state;
    int sygn;
    bool more;
    int len;
    
    *result = 0;
    
    len = str.size();
        
    if ( str.empty() ) {
        throw (badinitrejected());  
    }

    // std::cerr << "s2d: input <<" << str << ">>" << std::endl;
    intpart = 0;
    sygn = 1;
    state = 0;
    cpos = 0;
    more = true;
    while ( more ) {
       cc = str.at( beg + cpos );

       //std::cerr << "   " << cpos + beg << " : " << cpos << " '" << cc << "' " << state << "; " 
       //<< sygn <<  " " << intpart << " " << frcpart << " " << esygn << " " << exppart << std::endl;      
       switch (state) {
       case 0: // beginning; scans past whitespace
               // until not-whitespace, which must be 
               // a sign or a digit
            switch (cc) {
            case ' ': 
                break;
            case '+':
                state = 5; // start scanning integer or fractional part digits next
                break;
            case '-':
                state = 5; // start scanning integer or fractional part digits next
                sygn = -1;
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                state = 5; // continue scanning integer digits next
                intpart = cc - '0';
                break;
            default:
                state = 50;  
                break;
            }
            break;
       case 5: // in the integer digits. scan for more digits
            switch (cc) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                intpart = intpart*10 + (cc - '0');
                break;
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                state = 40;  
                break;
            default:
                state = 50;  
                break;
            }
            break;
       case 40: // whitespace: either the end of the scan, or whitespace added at the end
            if ( cc != ' ' && cc != '\t' ) {
               // not in whitespace on the same line anymore
               state = 50;
            }
            break;       
       case 50: // some unexpected character: scan finished
            // (should never actually get here)
            break;       
       }
    
       if ( wid <= 0 ) {
          if ( state == 40 || state == 50 || (beg + cpos) >= (len - 1) ) {
             // encountered an invalid expression character, 
             // or the end of string,
             // but no input width was specified, 
             // so we assume that what we got so far is ok,
             // and we are done now.
             
             more = false;
          }  
       }  else {
          // a width was specified
          
          if ( cpos >= (wid - 1) || (beg + cpos) >= (len - 1) ) {
             // we reached the end of the width 
             if (state != 50) {
               // we have not encountered any unexpected characters,
               // so we must be done
               more = false;
    
               // (if there is no more for this field, then cpos will not
               // be incremented below. But because this was a fixed-width
               // field, cpos currently ppoints to the last character
               // that is part of this field, when it should point
               // one character beyond that, for returning to the caller. Fix that here.)
               if ( (beg + cpos) < (len - 1) ) {
                 cpos++;
               }
               
             } else {
               // bad number in input
               throw (badinitrejected());
             }
          }

       }
       
       if ( more ) {
          // on to the next character position
          cpos ++;       
       } else {
          // all done, so put the parts together
          
          *result = sygn*( intpart );

       }

    } // end of while loop
        
       
//std::cerr << "Result = " << *result << std::endl;
    
    return cpos + beg;

}


int StreamRead::s2c( const std::string &str, int mode, string &sval, int beg, int wid )
{
    int pos;
    bool done;
    int cpos;
    char cc;
    int state;
    bool more;
    int len;
        
    len = str.size();
        
    if ( str.empty() ) {
        throw (badinitrejected());  
    }

    sval = "";
    
    // if the field field is givenm then we simply got forward that many characters
    // regardless of the mode
    if ( wid > 0 ) {
       // fixed-width field
    
       // check that we have tht many characters to read
       if ( (beg + wid) >= len ) {
          // asking for more characters than we have
          throw (badinitrejected());
       }
       
       sval = str.substr(beg, wid);
       
       cpos = wid;
    
    } else {
    
       // free(ish)-form input
       // if mode == 0, we skip past characters until we hit a non-numeric
       // if mode == 1, we skip past characters until we hit a non-numeric or whitespace
    
    
       // std::cerr << "s2d: input <<" << str << ">>" << std::endl;
       state = 0;
       cpos = 0;
       more = true;
       while ( more ) {
          cc = str.at( beg + cpos );
    
          //std::cerr << "   " << cpos + beg << " : " << cpos << " '" << cc << "' " << state << "; " 
          //<< sygn <<  " " << intpart << " " << frcpart << " " << esygn << " " << exppart << std::endl;      
          switch (state) {
          case 0: // beginning for mode 0; scans until a digit or sign or decimal point (or maybe whitespace)
               switch (cc) {
               case ' ': 
               case '\t': 
                   if ( mode == 0 ) {
                      // whitespace terminates mode 0
                      state = 50; 
                   }  
                   break;
               case '+':
               case '-':
               case '.':
               case '0':
               case '1':
               case '2':
               case '3':
               case '4':
               case '5':
               case '6':
               case '7':
               case '8':
               case '9':
                   state = 50; // continue scanning integer digits next
                   break;
               default:
                   sval = sval + cc;
                   break;
               }
               break;
          case 50: // some character: scan finished
               // (should never actually get here)
               break;       
          }
    
          if ( state == 50 || (beg + cpos) >= (len - 1) ) {
             // encountered end of the scan, 
             // or the end of string,
             
             more = false;
          }  
          
          if ( more ) {
             // on to the next character position
             cpos ++;       
          }
    
       } // end of while loop
    
       if ( cpos > 0 ) {
          sval = str.substr( beg, cpos - 1);
       }
        
    }
       
//std::cerr << "Result = " << sval << std::endl;
    
    return cpos + beg;


} 

void StreamRead::clearFormat()
{
    int i;
    
    for ( i=0; i<nf; i++ ) {
        delete fmt.back();
        fmt.pop_back();
    }
    nf = 0;

}


void StreamRead::format( const std::string fmtstr )
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
   std::string fs;

   if ( fmtstr == "" ) {
      fs = "%T: %o, %a, %v\n";
   } else {
      fs = fmtstr;
   }
   
   clearFormat();
   
   
   i = 0;
   state = 0;
   
   ff = new FmtSpec;
   // start be assuming this is a literal
   ff->type = "L";
   ff->str = "";
   while ( i < fs.size() ) {
      
      // grab the next character
      ch = fs.substr(i,1);
      
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
         } else if ( ch == "\n" ) {
            // end of line--we stop scanning here
            i = fs.size();
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
            if ( ch == "T" || ch == "t" || ch == "o" || ch == "a" || ch == "v" 
               || ch == "i" || ch == "g" || ch == "c" || ch == "x" 
               || ch == "s" || ch == "f" ) {
               
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
         
         } else if ( ch == "T" || ch == "t" || ch == "o" || ch == "a" || ch == "v" 
                   || ch == "i" || ch == "g" || ch == "c" || ch == "x" 
                   || ch == "s" || ch == "f" ) {
               // termination of a % sequence
               
               type = ch;
               
               if ( numbr != "" ) {
                  // the end of a number. convert it
                  n = s2i( numbr );
            
                  if ( first ) {
                     len = n;
                  } else {
                     if ( type == "T" ) {
                        // CalTime type: start.len
                        start = n;
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
             std::cerr << "Bad format: " << fs << std::endl;
             throw (StreamRead::badinitformat());   
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
             std::cerr << "Bad format: " << fs << std::endl;
             throw (StreamRead::badinitformat());   
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
      std::cerr << "Bad end to format " << fs << std::endl;
      throw (StreamRead::badinitformat());
   }
   
   // note how many format elements we ended up with
   nf = fmt.size();

}

std::string StreamRead::format() const
{
    string result;
    int i;
    FmtSpec *ff;
    string  type, fmtx, val;
    int ival;
    
    result = "";
    
    if ( nf > 0 ) {
    
       for ( i=0; i<nf; i++ ) {
      
           ff = fmt[i];
           type = ff->type;
           
           if ( type == "L" ) {
             // literal. Compare what is given to the input.
             
             if ( ff->str == "%" ) {
                result = result + "%";
             }
             result = result + ff->str;

          } else if ( type == "T" ) {
             // calendar time

             result = result + "%";
             if ( ff->len > 0 ) {
                if ( ff->align < 0 ) {
                   result = result + "-";
                }   
                result = result + i2s( ff->len );
                if ( ff->start > 0 ) {
                   result = result + "." + i2s( ff->start );
                }
             }
             result = result + "T";
             
          } else if ( type == "t" ) {
             // model time

             result = result + "%";
             if ( ff->len > 0 ) {
                if ( ff->align < 0 ) {
                   result = result + "-";
                }   
                result = result + i2s( ff->len );
                if ( ff->fract > 0 ) {
                   result = result + "." + i2s( ff->fract );
                }
             }
             result = result + "t";
                          
          } else if ( type == "o" ) {
             // parcel longitude
   
             result = result + "%";
             if ( ff->len > 0 ) {
                if ( ff->align < 0 ) {
                   result = result + "-";
                }   
                result = result + i2s( ff->len );
                if ( ff->fract > 0 ) {
                   result = result + "." + i2s( ff->fract );
                }
             }
             result = result + "o";
             
          } else if ( type == "a" ) {
             // parcel latitude

             result = result + "%";
             if ( ff->len > 0 ) {
                if ( ff->align < 0 ) {
                   result = result + "-";
                }   
                result = result + i2s( ff->len );
                if ( ff->fract > 0 ) {
                   result = result + "." + i2s( ff->fract );
                }
             }
             result = result + "a";
             
          } else if ( type == "v" ) {
             // parcel vertical coordinate

             result = result + "%";
             if ( ff->len > 0 ) {
                if ( ff->align < 0 ) {
                   result = result + "-";
                }   
                result = result + i2s( ff->len );
                if ( ff->fract > 0 ) {
                   result = result + "." + i2s( ff->fract );
                }
             }
             result = result + "v";
             
          } else if ( type == "g" ) {
             // parcel tag

             result = result + "%";
             if ( ff->len > 0 ) {
                if ( ff->align < 0 ) {
                   result = result + "-";
                }   
                result = result + i2s( ff->len );
                if ( ff->fract > 0 ) {
                   result = result + "." + i2s( ff->fract );
                }
             }
             result = result + "g";
             
          } else if ( type == "m" ) {
             // met source field

             result = result + "%";
             if ( ff->len > 0 ) {
                if ( ff->align < 0 ) {
                   result = result + "-";
                }   
                result = result + i2s( ff->len );
                if ( ff->fract > 0 ) {
                   result = result + "." + i2s( ff->fract );
                }
             }
             result = result + "{" + ff->field + "}m";
             
          } else if ( type == "f" ) {
             // parcel flags

             result = result + "%";
             if ( ff->len > 0 ) {
                if ( ff->align < 0 ) {
                   result = result + "-";
                }   
                result = result + i2s( ff->len );
             }
             result = result + "f";
             
          } else if ( type == "s" ) {
             // parcel status

             result = result + "%";
             if ( ff->len > 0 ) {
                if ( ff->align < 0 ) {
                   result = result + "-";
                }   
                result = result + i2s( ff->len );
             }
             result = result + "s";
             
          } else if ( type == "i" ) {
             // parcel index

             result = result + "%";
             if ( ff->len > 0 ) {
                if ( ff->align < 0 ) {
                   result = result + "-";
                }   
                result = result + i2s( ff->len );
             }
             result = result + "i";
             
          } else if ( type == "c" ) {
             // (non-whitespace, non-numeric) arbitrary character

             result = result + "%";
             if ( ff->len > 0 ) {
                if ( ff->align < 0 ) {
                   result = result + "-";
                }   
                result = result + i2s( ff->len );
             }
             result = result + "c";
             
          } else if ( type == "x" ) {
             // (non-numeric) arbitrary character

             result = result + "%";
             if ( ff->len > 0 ) {
                if ( ff->align < 0 ) {
                   result = result + "-";
                }   
                result = result + i2s( ff->len );
             }
             result = result + "x";
             
          } else {
             std::cerr << "unknown format type:" << type << std::endl;
             throw (badinitformat());
          }
       }
    }
    
    return result;
}

// FmtSpec constructor
StreamRead::FmtSpec::FmtSpec(const std::string type0, int start0, int len0, int fract0, std::string str0, int align0)
{

    align = 0;
    
    defaults( type0, start0, len0, fract0, str0, align0 );

}

StreamRead::FmtSpec::~FmtSpec() 
{
}

StreamRead::FmtSpec::FmtSpec(const StreamRead::FmtSpec& src)
{
     type = src.type;
     start = src.start;
     len = src.len;
     fract = src.fract;
     align = src.align;
     str = src.str;
     field = src.field;
}

StreamRead::FmtSpec& StreamRead::FmtSpec::operator=(const StreamRead::FmtSpec& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }

    this->assign( src ) ;
    
    return *this;
}

void StreamRead::FmtSpec::assign( const StreamRead::FmtSpec& src)
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
void StreamRead::FmtSpec::defaults( std::string type0, int start0, int len0, int fract0, std::string str0, int align0)
{
    if ( type0 == "" ) {
       type0 = "L";
    }   
    
    if ( align0 != -1 && align != 0 ) {
       align = 1;
    }   

    if ( len0 <= 0 ) {
       len0 = -1;
    }
    if ( start0 < 0 ) {
       start0 = 0;
    }
    if ( fract0 <= 0 ) {
       fract0 = -1;
    }      
    
    type = type0;
    start = start0;
    len = len0;
    fract = fract0;
    str = str0;
    align = align0;

}

