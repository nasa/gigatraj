
#include "config.h"

#include "gigatraj/StreamDump.hh"

using namespace gigatraj;

// default constructor
StreamDump :: StreamDump(int format)
{
    os = &std::cout;
    mode = format;
};

// alternate constructor
StreamDump :: StreamDump( std::ostream& output, int format ) 
{
    os = &(output);
    mode = format;
};

StreamDump::~StreamDump() 
{
}

StreamDump::StreamDump(const StreamDump& src) : ParcelReporter(src)
{
    os = src.os;
    mode = src.mode;
}

StreamDump& StreamDump::operator=(const StreamDump& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }

    this->assign( src ) ;
    
    return *this;
}

void StreamDump::assign( const StreamDump& src)
{
     ParcelReporter::assign( src );
     os = src.os;
     mode = src.mode;
}

int StreamDump :: format( int fmt ) {
   
    if ( fmt >= 0 ) {
        mode = fmt;
    }
    
    return mode;

}


// dumps a single Parcel
void StreamDump :: apply( Parcel& p ) 
{
    char *output;
    int bn;
    
    try {
       if ( mode == 0 ) {
          *(os) << p;
       } else {
          output = p.bserialize( &bn );
          os->write( reinterpret_cast<char *>(&bn), static_cast<std::streamsize>(sizeof(int)) );
          os->write( output, static_cast<std::streamsize>(bn*sizeof(char)) );
          delete[] output;
       }
    } catch (std::ios::failure) {
       throw (StreamDump::badstreamdump());
    }   
};

// dumps an array of Parcels
void StreamDump :: apply( Parcel * const p, const int n )
{
    int i;
    char *output;
    int bn;
    std::streamsize sn;
    int n0;
    
    if ( n < 0 ) {
       throw (ParcelFilter::badparcelnum());
    };
    
    n0 = 0;
    
    if ( mode == 0 ) {
       *(os) << n;
       for ( i=0; i<n; i++ ) {
          try {
             *(os) << p[i];
          } catch (std::ios::failure) {
             throw (StreamDump::badstreamdump());
          }   
       } 
    } else {
       os->write( reinterpret_cast<const char *>(&n), static_cast<std::streamsize>(sizeof(int)) );
       bn = (p[0]).bsize();
       output = new char[bn];
       sn = static_cast<std::streamsize>( bn*sizeof(char) );
       os->write( reinterpret_cast<char *>(&bn), static_cast<std::streamsize>(sizeof(int)) );
       for ( i=0; i<n; i++ ) {
          try {
             (p[i]).bserialize( &bn, output );
             os->write( output, sn );
          } catch (std::ios::failure) {
             throw (StreamDump::badstreamdump());
          }   
       }
       delete[] output; 
    }
};



// dump a vector of Parcels
void StreamDump :: apply( std::vector<Parcel>& p )
{
    std::vector<Parcel>::iterator ip;
    int n;
    char *output;
    int bn;
    std::streamsize sn;
    
    n = p.size();
    
    if ( n <= 0 ) {
       throw (ParcelFilter::badparcelnum());
    };  
    
    if ( mode == 0 ) {
       *(os) << n;
       for ( ip=p.begin(); ip != p.end(); ip++ ) {
          try {
             *(os) << (*ip);
          } catch (std::ios::failure) {
             throw (StreamDump::badstreamdump());
          }   
       }   
    } else {
       os->write( reinterpret_cast<char *>(&n), static_cast<std::streamsize>(sizeof(int)) );
       bn = (p[0]).bsize();
       output = new char[bn];
       sn = static_cast<std::streamsize>( bn*sizeof(char) );
       os->write( reinterpret_cast<char *>(&bn), static_cast<std::streamsize>(sizeof(int)) );
       for ( ip=p.begin(); ip != p.end(); ip++ ) {
          try {
             ip->bserialize( &bn, output );
             os->write( output, sn );
          } catch (std::ios::failure) {
             throw (StreamDump::badstreamdump());
          }   
       }   
       delete[] output; 
    }

};


// dump a list of Parcels
void StreamDump :: apply( std::list<Parcel>& p )
{
    std::list<Parcel>::iterator ip;
    int n;
    char *output;
    int bn;
    std::streamsize sn;
    
    n = p.size();
    
    if ( n <= 0 ) {
       throw (ParcelFilter::badparcelnum());
    };  
    
    if ( mode == 0 ) {
       *(os) << n;
       for ( ip=p.begin(); ip != p.end(); ip++ ) {
          try {
             *(os) << (*ip);
          } catch (std::ios::failure) {
             throw (StreamDump::badstreamdump());
          }   
       }   
    } else {
       os->write( reinterpret_cast<char *>(&n), static_cast<std::streamsize>(sizeof(int)) );
       ip = ip=p.begin();
       bn = ip->bsize();
       output = new char[bn];
       sn = static_cast<std::streamsize>( bn*sizeof(char) );
       os->write( reinterpret_cast<char *>(&bn), static_cast<std::streamsize>(sizeof(int)) );
       for ( ip=p.begin(); ip != p.end(); ip++ ) {
          try {
             ip->bserialize( &bn, output );
             os->write( output, sn );
          } catch (std::ios::failure) {
             throw (StreamDump::badstreamdump());
          }   
       }   
       delete[] output; 
    }

};

// dump a deque of Parcels
void StreamDump :: apply( std::deque<Parcel>& p )
{
    std::deque<Parcel>::iterator ip;
    int n;
    char *output;
    int bn;
    std::streamsize sn;
    
    n = p.size();
    
    if ( n <= 0 ) {
       throw (ParcelFilter::badparcelnum());
    };  
    
    if ( mode == 0 ) {
       *(os) << n;
       for ( ip=p.begin(); ip != p.end(); ip++ ) {
          try {
             *(os) << (*ip);
          } catch (std::ios::failure) {
             throw (StreamDump::badstreamdump());
          }   
       }   
    } else {
       os->write( reinterpret_cast<char *>(&n), static_cast<std::streamsize>(sizeof(int)) );
       ip = ip=p.begin();
       bn = ip->bsize();
       output = new char[bn];
       sn = static_cast<std::streamsize>( bn*sizeof(char) );
       os->write( reinterpret_cast<char *>(&bn), static_cast<std::streamsize>(sizeof(int)) );
       for ( ip=p.begin(); ip != p.end(); ip++ ) {
          try {
             ip->bserialize( &bn, output );
             os->write( output, sn );
          } catch (std::ios::failure) {
             throw (StreamDump::badstreamdump());
          }   
       }   
       delete[] output; 
    }

};


// dump a Flock of Parcels
void StreamDump :: apply( Flock& p )
{
    int n;
    Parcel *px;
    bool i_am_root;
    char *output;
    int bn;
    std::streamsize sn;
    
    n = p.size();

    if ( n <= 0 ) {
       throw (ParcelFilter::badparcelnum());
    };  
    
    i_am_root = p.is_root();

    output = NULLPTR;
    bn = -1;

    if ( mode == 0 ) {
       *(os) << n; 
    } else {
       os->write( reinterpret_cast<char *>(&n), static_cast<std::streamsize>(sizeof(int)) );
       p.sync();
       px =  p.parcel( 0, 1 );
       bn = px->bsize();
       if ( ( px != NULLPTR ) && i_am_root ) {
          output = new char[bn];
          sn = static_cast<std::streamsize>( bn*sizeof(char) );
          os->write( reinterpret_cast<char *>(&bn), static_cast<std::streamsize>(sizeof(int)) );
       }
    }
    
    if ( (mode != 0) && (bn < 0) ) {
       std::cerr << " Bad parcel size " << std::endl;
       std::cerr << " mode = " << mode << " , bn = " << bn << std::endl;
       throw (StreamDump::badstreamdump());
    }
    if ( (mode != 0) && (output == NULLPTR) && i_am_root ) {
       std::cerr << " Bad output stream " << std::endl;
       throw (StreamDump::badstreamdump());
    }
    
    for ( int i=0; i<n; i++ ) {
       try {
       
           // the processors all need to be synchronized, so that
           // as one sends the ith parcel, the root is ready to receive
           // the ith parcel.
           p.sync();
         
           try {
              // get the parcel.
              // If this is the root processor, we get a valid pointer.
              // If this is the non-root owner processor, we also get a valid pointer 
              // If this is a non-root non-owner processor for the ith parcel, we get NULLPTR
              // or a badparcelindex error.
              px =  p.parcel( i, 1 );
              
              if ( ( px != NULLPTR ) && i_am_root ) {
                 if ( mode == 0 ) {
                    // and dump it
                    *(os) << *px;
                 } else {
                    px->bserialize( &bn, output );
                    os->write( output, sn );
                 }
              }   
           } catch (Flock::badparcelindex()) {
             // no problem. ignore this.  
           }; 

       } catch (std::ios::failure) {
          throw (StreamDump::badstreamdump());
       }   
    }
    
    if ( output != NULL ) {   
       delete[] output; 
    }

};


// dump a Swarm of Parcels
void StreamDump :: apply( Swarm& p )
{
    int n;
    Parcel *px;
    bool i_am_root;
    char *output;
    int bn;
    std::streamsize sn;
    
    n = p.size();

    if ( n <= 0 ) {
       throw (ParcelFilter::badparcelnum());
    };  
    
    i_am_root = p.is_root();

    output = NULL;

    if ( mode == 0 ) {
       *(os) << n; 
    } else {
       os->write( reinterpret_cast<char *>(&n), static_cast<std::streamsize>(sizeof(int)) );
       p.sync();
       px =  p.parcel( 0, 1 );
       if ( ( px != NULLPTR ) && i_am_root ) {
          bn = px->bsize();
          output = new char[bn];
          sn = static_cast<std::streamsize>( bn*sizeof(char) );
          os->write( reinterpret_cast<char *>(&bn), static_cast<std::streamsize>(sizeof(int)) );
       }
    }
    
    for ( int i=0; i<n; i++ ) {
       try {
       
           // the processors all need to be synchronized, so that
           // as one sends the ith parcel, the root is ready to receive
           // the ith parcel.
           p.sync();
         
           try {
              // get the parcel.
              // If this is the root processor, we get a valid pointer.
              // If this is the non-root owner processor, we also get a valid pointer 
              // If this is a non-root non-owner processor for the ith parcel, we get NULLPTR
              // or a badparcelindex error.
              px =  p.parcel( i, 1 );
              
              if ( ( px != NULLPTR ) && i_am_root ) {
                 if ( mode == 0 ) {
                    // and dump it
                    *(os) << *px;
                 } else {
                    px->bserialize( &bn, output );
                    os->write( output, sn );
                 }
              }   
           } catch (Swarm::badparcelindex()) {
             // no problem. ignore this.  
           }; 

       } catch (std::ios::failure) {
          throw (StreamDump::badstreamdump());
       }   
    }
    
    if ( output != NULL ) {   
       delete[] output; 
    }

};

