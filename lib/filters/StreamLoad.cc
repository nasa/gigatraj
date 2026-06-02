
#include "config.h"

#include "gigatraj/StreamLoad.hh"
#include <iostream>
#include <sstream>

using namespace gigatraj;


// default constructor
StreamLoad :: StreamLoad(int format)
{

    is = &std::cin;
    mode = format;

};


// alternate constructor
StreamLoad :: StreamLoad( std::istream& input, int format ) 
{

    is = &(input);
    mode = format;

};


// destructor
StreamLoad :: ~StreamLoad()
{
}

StreamLoad::StreamLoad(const StreamLoad& src) : ParcelInitializer(src)
{
    is = src.is;
    mode = src.mode;
}

StreamLoad& StreamLoad::operator=(const StreamLoad& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }

    this->assign( src ) ;
    
    return *this;
}

void StreamLoad::assign( const StreamLoad& src)
{
     ParcelInitializer::assign( src );
     is = src.is;
     mode = src.mode;
}


int StreamLoad :: format( int fmt ) {
   
    if ( fmt >= 0 ) {
        mode = fmt;
    }
    
    return mode;

}

std::istream* StreamLoad :: stream( std::istream* input )
{
    if ( input != NULL ) {
       is = input;
    }
    
    return is;
    
}


// init one Parcel
void StreamLoad :: apply( Parcel& p ) 
{
    char *input;
    int bn;

    try {
       if ( mode == 0 ) {
          *(is) >> p;
       } else {
          is->read(  reinterpret_cast<char *>(&bn), static_cast<std::streamsize>(sizeof(int)) );
          if ( bn == p.bsize() ) {
             input = new char[ bn ];
             is->read( input, static_cast<std::streamsize>(bn*sizeof(char)) );
             p.bdeserialize( input, bn );
             delete[] input;
          } else {
             // our parcel size does not match the file's parcel size
             throw (StreamLoad::badinitstream());          
          }
       }
    } catch (std::ios::failure) {
       throw (StreamLoad::badinitstream());
    }   

};


// load an array of Parcels
void StreamLoad :: apply( Parcel * const p, const int n )
{
    int i;
    char *input;
    int bn;
    int n0;
    
    if ( n < 0 ) {
       throw (ParcelFilter::badparcelnum());
    };
    
    n0 = 0;
    
    if ( mode == 0 ) {
       *(is) >> n0;
       if ( n0 <= n ) {
          for ( i=0; i<n; i++ ) {
             try {
                *(is) >> p[i];
             } catch (std::ios::failure) {
                throw (StreamLoad::badinitstream());
             }   
          }   
       } else {
          // our parcel size does not match the file's parcel size
          throw (StreamLoad::badinitstream());          
       }
    } else {
       is->read(  reinterpret_cast<char *>(&n0), static_cast<std::streamsize>(sizeof(int)) );
       if ( n0 <= n ) {
          is->read(  reinterpret_cast<char *>(&bn), static_cast<std::streamsize>(sizeof(int)) );
          if ( bn == p[0].bsize() ) {
             input = new char[ bn ];       
             for ( i=0; i<n; i++ ) {
                try {
                   is->read( input, static_cast<std::streamsize>(bn*sizeof(char)) );
                   p[i].bdeserialize( input, bn );
                } catch (std::ios::failure) {
                   throw (StreamLoad::badinitstream());
                }   
             }   
             delete[] input;
             
          } else {
             // our parcel size does not match the file's parcel size
             throw (StreamLoad::badinitstream());          
          }
       } else {
          // our parcel size does not match the file's parcel size
          throw (StreamLoad::badinitstream());          
       }
    }

    // now mark any remaining parcels as do-not-trace
    for ( i=n0; i<n; i++ ) {
        p[i].setNoTrace();
    }

};

// load a vector of Parcels
void StreamLoad :: apply( std::vector<Parcel>& p )
{
    std::vector<Parcel>::iterator ip;
    int n;
    char *input;
    int bn;
    int n0;
    int i;
    
    n = p.size();
    if ( n <= 0 ) {
       throw (ParcelFilter::badparcelnum());
    };  
    
    n0 = 0;
    
    ip = p.begin();
      
    if ( mode == 0 ) {
       *(is) >> n0;
       if ( n0 <= n ) {
          for ( i=0; (ip != p.end()) && (i < n0); ip++, i++ ) {
             try {
                *(is) >> (*ip);
             } catch (std::ios::failure) {
                throw (StreamLoad::badinitstream());
             }   
          }   
       } else {
          // our parcel size does not match the file's parcel size
          throw (StreamLoad::badinitstream());          
       }
    } else {
       is->read(  reinterpret_cast<char *>(&n0), static_cast<std::streamsize>(sizeof(int)) );
       if ( n0 <= n ) {
          is->read(  reinterpret_cast<char *>(&bn), static_cast<std::streamsize>(sizeof(int)) );
          if ( bn == ip->bsize() ) {
             input = new char[ bn ];       
             for ( i=0; (ip != p.end()) && (i < n0); ip++, i++ ) {
                try {
                   is->read( input, static_cast<std::streamsize>(bn*sizeof(char)) );
                   ip->bdeserialize( input, bn );
                } catch (std::ios::failure) {
                   throw (StreamLoad::badinitstream());
                }   
             }   
             delete[] input;
          } else {
             // our parcel size does not match the file's parcel size
             throw (StreamLoad::badinitstream());          
          }
       } else {
          // our parcel size does not match the file's parcel size
          throw (StreamLoad::badinitstream());          
       }
    }

    // now mark any remaining parcels as do-not-trace
    for (  ; ip != p.end() ; ip++ ) {
        ip->setNoTrace();
    }

};


// load a list of Parcels
void StreamLoad :: apply( std::list<Parcel>& p )
{
    std::list<Parcel>::iterator ip;
    int n;
    char *input;
    int bn;
    int n0;
    int i;
    
    n = p.size();
    
    if ( n <= 0 ) {
       throw (ParcelFilter::badparcelnum());
    };  
    
    n0 = 0;

    ip = p.begin();
    
    if ( mode == 0 ) {
       *(is) >> n0;
       if ( n0 <= n ) {
          for ( i=0; (ip != p.end()) && (i < n0); ip++, i++ ) {
             try {
                *(is) >> (*ip);
             } catch (std::ios::failure) {
                throw (StreamLoad::badinitstream());
             }   
          }   
       } else {
          // our parcel size does not match the file's parcel size
          throw (StreamLoad::badinitstream());          
       }
    } else {
       is->read(  reinterpret_cast<char *>(&n0), static_cast<std::streamsize>(sizeof(int)) );
       if ( n0 <= n ) {
          is->read(  reinterpret_cast<char *>(&bn), static_cast<std::streamsize>(sizeof(int)) );
          if ( bn == ip->bsize() ) {
             input = new char[ bn ];       
             for ( i=0; (ip != p.end()) && (i < n0); ip++, i++ ) {
                try {
                   is->read( input, static_cast<std::streamsize>(bn*sizeof(char)) );
                   ip->bdeserialize( input, bn );
                } catch (std::ios::failure) {
                   throw (StreamLoad::badinitstream());
                }   
             }   
             delete[] input;
          } else {
             // our parcel size does not match the file's parcel size
             throw (StreamLoad::badinitstream());          
          }
       } else {
          // our parcel size does not match the file's parcel size
          throw (StreamLoad::badinitstream());          
       }
    }


    // now mark any remaining parcels as do-not-trace
    for (  ; ip != p.end() ; ip++ ) {
        ip->setNoTrace();
    }

};


// load a deque of Parcels
void StreamLoad :: apply( std::deque<Parcel>& p )
{
    std::deque<Parcel>::iterator ip;
    int n;
    char *input;
    int bn;
    int n0;
    int i;
    
    n = p.size();

    ip = p.begin();
    
    if ( n <= 0 ) {
       throw (ParcelFilter::badparcelnum());
    };  
    
    if ( mode == 0 ) {
       *(is) >> n0;
       if ( n0 <= n ) {
          for ( i=0; (ip != p.end()) && (i < n0); ip++, i++ ) {
             try {
                *(is) >> (*ip);
             } catch (std::ios::failure) {
                throw (StreamLoad::badinitstream());
             }   
          }   
       } else {
          // our parcel size does not match the file's parcel size
          throw (StreamLoad::badinitstream());          
       }
    } else {
       is->read(  reinterpret_cast<char *>(&n0), static_cast<std::streamsize>(sizeof(int)) );
       if ( n0 <= n ) {
          is->read(  reinterpret_cast<char *>(&bn), static_cast<std::streamsize>(sizeof(int)) );
          if ( bn == ip->bsize() ) {
             input = new char[ bn ];       
             for ( i=0; (ip != p.end()) && (i < n0); ip++, i++ ) {
                try {
                   is->read( input, static_cast<std::streamsize>(bn*sizeof(char)) );
                   ip->bdeserialize( input, bn );
                } catch (std::ios::failure) {
                   throw (StreamLoad::badinitstream());
                }   
             }   
             delete[] input;
          } else {
             // our parcel size does not match the file's parcel size
             throw (StreamLoad::badinitstream());          
          }
       } else {
          // our parcel size does not match the file's parcel size
          throw (StreamLoad::badinitstream());          
       }
    }

    // now mark any remaining parcels as do-not-trace
    for (  ; ip != p.end() ; ip++ ) {
        ip->setNoTrace();
    }

};

// load a Flock of Parcels
void StreamLoad :: apply( Flock& p )
{
    Flock::iterator ip;
    int n;
    Parcel px;
    bool i_am_root;
    char *input;
    int bn;
    int n0;
    int i;
    
    n = p.size();
    
    if ( n <= 0 ) {
       throw (ParcelFilter::badparcelnum());
    };  
    
    i_am_root = p.is_root();
    
    if ( mode == 0 ) {
       // text input
       
       *(is) >> n0;
       if ( n0 <= n ) {
          for ( i=0; i<n0; i++ ) {
             p.sync();
             
             try {
                if ( i_am_root ) {
                   // get the parcel
                   *(is) >> px;
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
                throw (StreamLoad::badinitstream());
             }   
          }   
       } else {
          // our parcel size does not match the file's parcel size
          throw (StreamLoad::badinitstream());          
       }
    } else {
       // binary input
       
       is->read(  reinterpret_cast<char *>(&n0), static_cast<std::streamsize>(sizeof(int)) );
       if ( n0 <= n ) {
          is->read(  reinterpret_cast<char *>(&bn), static_cast<std::streamsize>(sizeof(int)) );
          if ( bn == px.bsize() ) {
             input = new char[ bn ];       
             for ( i=0; i<n0; i++ ) {
                p.sync();
                
                try {
                   if ( i_am_root ) {
                      // get the parcel
                      is->read( input, bn*static_cast<std::streamsize>(sizeof(char)) );
                      px.bdeserialize( input, bn );
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
                   throw (StreamLoad::badinitstream());
                }   
             }   
             delete[] input;
             // now mark any unfilled parcels as do-not-trace
             if ( i_am_root ) {
                px.setNoTrace();
             }   
          } else {
             // our parcel size does not match the file's parcel size
             throw (StreamLoad::badinitstream());          
          }
       } else {
          // our parcel size does not match the file's parcel size
          throw (StreamLoad::badinitstream());          
       }
       
       // mark any unused parcels as do-not-trace
       for ( i=n0; i<n; i++ ) {
          p.sync();
          
          try {

             p.set( i, px, 0 );

          } catch (std::ios::failure) {
             throw (StreamLoad::badinitstream());
          }   
       }
        
    }
};

// load a Swarm of Parcels
void StreamLoad :: apply( Swarm& p )
{
    Swarm::iterator ip;
    int n;
    Parcel px;
    bool i_am_root;
    char *input;
    int bn;
    int n0;
    int i;
    
    n = p.size();
    
    if ( n <= 0 ) {
       throw (ParcelFilter::badparcelnum());
    };  
    
    i_am_root = p.is_root();
    
    if ( mode == 0 ) {
       // text input
       
       *(is) >> n0;
       if ( n0 <= n ) {
          for ( i=0; i<n0; i++ ) {
             p.sync();
             
             try {
                if ( i_am_root ) {
                   // get the parcel
                   *(is) >> px;
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
                throw (StreamLoad::badinitstream());
             }   
          }   
       } else {
          // our parcel size does not match the file's parcel size
          throw (StreamLoad::badinitstream());          
       }
    } else {
       is->read(  reinterpret_cast<char *>(&n0), static_cast<std::streamsize>(sizeof(int)) );
       if ( n0 <= n ) {
          is->read(  reinterpret_cast<char *>(&bn), static_cast<std::streamsize>(sizeof(int)) );
          if ( bn == px.bsize() ) {
             input = new char[ bn ];       
             for ( i=0; i<n0; i++ ) {
                p.sync();
                
                try {
                   if ( i_am_root ) {
                      // get the parcel
                      is->read( input, bn*static_cast<std::streamsize>(sizeof(char)) );
                      px.bdeserialize( input, bn );
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
                   throw (StreamLoad::badinitstream());
                }   
             }   
             delete[] input;
             // now mark any unfilled parcels as do-not-trace
             if ( i_am_root ) {
                px.setNoTrace();
             }   
          } else {
             // our parcel size does not match the file's parcel size
             throw (StreamLoad::badinitstream());          
          }
       } else {
          // our parcel size does not match the file's parcel size
          throw (StreamLoad::badinitstream());          
       }
       
       // mark any unused parcels as do-not-trace
       for ( i=n0; i<n; i++ ) {
          p.sync();
          
          try {

             p.set( i, px, 0 );

          } catch (std::ios::failure) {
             throw (StreamLoad::badinitstream());
          }   
       }
        
    }
};

