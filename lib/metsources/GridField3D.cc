
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

#include "gigatraj/GridField3D.hh"

using namespace gigatraj;


const std::string GridField3D::iam = "GridField3D";


// Default constructor
GridField3D::GridField3D(): GridField() 
{
   
   vquant = "none";
   vuu = "N/A";
   mksVScale = 1.0;
   mksVOffset = 0.0;

   use_array = 1;
   nd = 0;
   dater = NULLPTR;

}

// Default destructor
GridField3D::~GridField3D()
{
};

// copy constructor
GridField3D::GridField3D(const GridField3D& src): GridField(src)
{

        vquant = src.vertical();
           vuu = src.vunits();
     mksVScale = src.mksVScale;
    mksVOffset = src.mksVOffset;
        zs = src.zs;

        zs.setPgroup( pgroup, metproc );
}

void GridField3D::assign( const GridField3D& src )
{
   
   GridField::assign(src);
   
        vquant = src.vertical();
           vuu = src.vunits();
     mksVScale = src.mksVScale;
    mksVOffset = src.mksVOffset;
            zs = src.zs; 

    zs.setPgroup( pgroup, metproc );
}


void GridField3D::clear() {
   
   vquant = "none";
   vuu = "N/A";
   mksVScale = 1.0;
   mksVOffset = 0.0;
   zs.clear();
   
   GridField::clear();   

}

int GridField3D::status() const
{
    int result = 0;
    
    result = GridField::status();
    if ( vquant == "none" || vuu == "N/A" ) {
       result = result | 0x02;
    } 
    if ( zs.size() <= 0 ) {
       result = result | GFS_NODIMS;
    }
    
    return result;

}



real GridField3D::operator()( int i, int j, int k ) const 
{
   return value(i,j,k);
}        

real& GridField3D::operator()( int i, int j, int k )  
{
   return valueref(i,j,k);
}

std::string GridField3D::vertical() const
{
   return vquant;
}

void GridField3D::set_vertical( const std::string vq )
{
   vquant = vq;
   mksVScale = 1.0;
   mksVOffset = 0.0;
   
   zs.set_quantity(vq);
}

std::string GridField3D::vunits(real *scale, real*offset) const
{
   if ( scale != NULLPTR ) {
      *scale = mksVScale;
   }
   if ( offset != NULLPTR ) {
      *offset = mksVOffset;
   }

   return vuu;
}

void GridField3D::set_vunits( const std::string vu, real scale, real offset )
{
   vuu = vu;
   mksVScale = scale;
   mksVOffset = offset;
   
   zs.set_units( vu, scale, offset );
}         

void GridField3D::setPgroup( ProcessGrp* pg, int met)
{
       GridField::setPgroup( pg, met );

       zs.setPgroup( pg, met );

}

std::vector<real> GridField3D::levels() const 
{ 
   return zs.dimension();
};

void GridField3D::levels( const std::vector<real> &newvert )
{
   int nzs;
   
   nzs = zs.size();
   
   if ( nzs > 0 && nzs != newvert.size() ) {
      std::cerr << "New vertical cooridnate vector does not match the old in size." << std::endl;
      throw (badDimensionMismatch());
   }

   zs.load( newvert );

}

real GridField3D::level( const int k ) const
{
   return zs(k);
}; 

void GridField3D::absorbLevels( int n, real* zvals )
{
      if ( zvals != NULLPTR ) {
         zs.absorb( n, zvals );
      }

}

GridFieldDim GridField3D::getLevels() const
{
     return zs;
} 

void GridField3D::zindex( real z, int* i1, int* i2 ) const
{
     zs.index( z, i1, i2 );
};


void GridField3D::transform( const std::string unyts, real scale, real offset ) 
{
    real ms, mo;
    GridField3D::iterator datax;
    real s, o;
    
    uu = unyts;
    
    s = mksScale/scale;
    o = ( mksOffset - offset)/scale;
    
    for ( datax = this->begin(); datax != this->end(); datax++ ) {
        if (*datax != fill_value ) {
           *datax = (*datax)*s + o;
        }
    } 

    mksOffset = offset;
    mksScale  = scale;

}

bool GridField3D::compatible( const GridField3D&  obj, int flags ) const
{
    std::vector<real> dim;
    int result = true;

    //- std::cerr << " Grid3D: compatibility start" << std::endl;
    if ( flags & METCOMPAT_VERT ) {
       // and the vertical coordinate types must match
       if ( obj.vertical() == vquant && obj.vunits() == vuu ) {
       
          //- std::cerr << " Grid3D: compatibility vert same" << std::endl;
          result = zs.compatible( obj.zs );
          //- std::cerr << " Grid3D: compatibility z" << result << std::endl;

       } else {
          //- std::cerr << " Grid3D: compatibility vert different" << std::endl;
          result = false;
       }
    }         

    if ( flags & METCOMPAT_TIME ) {
       // the model times and the calendar times must both match
          //- std::cerr << " Grid3D: compatibility time" << std::endl;
       if ( obj.time() != mtime || obj.met_time() != ctime ) {
          //- std::cerr << " Grid3D: compatibility time different" << std::endl;
          result = false;
       }   
    }

    //- std::cerr << " Grid3D: compatibility = " << result << std::endl;
    return result;

}

bool GridField3D::compatible( const GridFieldSfc& obj, int flags ) const
{
    int result = true;
    std::vector<real> dim;
    
    if ( flags & METCOMPAT_VERT ) {
       result = false;
    }         

    if ( flags & METCOMPAT_TIME ) {
       // the model times and the calendar times must both match
       if ( obj.time() != mtime || obj.met_time() != ctime ) {
          result = false;
       }   
    }
    
    return result;      
       

}


GridField3D* GridField3D::generateVertical() const
{
    GridField3D* newgrid;
    GridField3D::iterator destPnt;
    int k;

    real val;

    newgrid = this->duplicate();

    newgrid->set_quantity( this->vertical() );
    newgrid->set_units( this->vunits() );
    newgrid->mksScale = this->mksVScale;
    newgrid->mksOffset = this->mksVOffset;

    for ( k=0; k < zs.size(); k++ ) {
        val = this->level(k);
        for ( destPnt  = newgrid->begin(k); 
              destPnt != newgrid->end(k); 
              destPnt++ ) {
            
            *destPnt = val;
        }    
    }
    
    return newgrid;

}    

void GridField3D::newVertical( const std::vector<real>& newz )
{
     
     flushData();
     
     zs.load( newz );
     
}

void GridField3D::svr_send_vals( int id ) const
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

        //- std::cerr << "--- metproc starts sending values" << std::endl;    
        // get the number of points desired
        pgroup->receive_ints( id, 1, &n, PGR_TAG_GNUM );
        //- std::cerr << "      (*(*(* client " << id << " wants values for " << n << " points" << std::endl;

        //- std::cerr << "--- metproc n=" << n << std::endl;    
        // get the integer coordinates of the points
        try {
            coords = new int[n];
        } catch(...) {
           throw (badmemreq());
        }
        //- std::cerr << "      (*(*(* about to receive " << n << " indices from " << id << std::endl;
        pgroup->receive_ints( id, n, coords, PGR_TAG_GCOORDS );
        //- std::cerr << "      (*(*(* client " << id << " gave us indices" << std::endl;

        // send the data values requested
        vals = new real[n];
        //- std::cerr << "      (*(*(* about to load vals from coords; coords[0]= " << coords[0] << std::endl;
        for ( int i=0; i<n; i++ ) {
            vals[i] = this->value( coords[i] );
        }
        pgroup->send_reals( id, n, vals, PGR_TAG_GVALS );
        //- std::cerr << "      (*(*(* send client " << id << " gave us indices " << std::endl;
        //- std::cerr << "--- metproc stops sending values" << std::endl;    
        
        delete vals;
        delete coords;
     }

}


void GridField3D::serialize(std::ostream& os) const
{
  string str;
  int len;
  real val;
  int version = 1;
  
  try {

      GridField::serialize(os);
        
      // output the version
      os.write( reinterpret_cast<char *>( &version), static_cast<std::streamsize>( sizeof(int)));
      
      // output the vertical coordinate name
      len = vquant.length();
      os.write( reinterpret_cast<char *>( &len), static_cast<std::streamsize>( sizeof(int)));
      os.write( vquant.c_str(), static_cast<std::streamsize>( len*sizeof(char)));

      // output the vertical coordinate units
      len = vuu.length();
      os.write( reinterpret_cast<char *>( &len), static_cast<std::streamsize>( sizeof(int)));
      os.write( vuu.c_str(), static_cast<std::streamsize>( len*sizeof(char)));

      // output the MKS transformation parameters
      val = mksScale;
      os.write( reinterpret_cast<char *>( &val), static_cast<std::streamsize>( sizeof(real)));
      val = mksOffset;
      os.write( reinterpret_cast<char *>( &val), static_cast<std::streamsize>( sizeof(real)));

  } catch (...) {
      throw;
  }    
  
  //return os;


}


void GridField3D::deserialize(std::istream& is)
{
   string str;
   int len;
   char cc;
   int i;
   double t;
   real val;
   std::vector<real> dat;
   int ival;
   int version;
   

   try {

       GridField::deserialize(is);
        
       // read the version
       is.read(reinterpret_cast<char *>( &version), static_cast<std::streamsize>( sizeof(int)));
       
   
       // read the vertical coordinate quantity
       is.read(reinterpret_cast<char *>( &len), static_cast<std::streamsize>( sizeof(int)));
       str = "";
       if ( len >= 0 ) {
          str.clear();
          for ( i=0; i < len; i++ ) {
             is.read( &cc, static_cast<std::streamsize>( sizeof(char)));
             str.push_back( cc );
          }   
       }
       vquant = str;

       // read the vertical coordinate units
       is.read(reinterpret_cast<char *>( &len), static_cast<std::streamsize>( sizeof(int)));
       str = "";
       if ( len >= 0 ) {
          str.clear();
          for ( i=0; i < len; i++ ) {
             is.read( &cc, static_cast<std::streamsize>( sizeof(char)));
             str.push_back( cc );
          }   
       }
       vuu = str;

       // read the mks transform parameters
       is.read( reinterpret_cast<char *>(&mksVScale), static_cast<std::streamsize>( sizeof(real)) );
       is.read( reinterpret_cast<char *>(&mksVOffset), static_cast<std::streamsize>( sizeof(real)) );


   } catch (...) {
       throw;
   }    
   
   //return is;


}



/****************** Iterators **************************/

///////////////////////////////  iterator

GridField3D::iterator::iterator()
{
   my_index = 0;
   first = 0;
   last = 0;
   my_grid = NULLPTR;
}   


GridField3D::iterator::iterator( GridField3D* grid, int n, int beg, int fin )
{
   my_index = n;
   first = beg;
   last = fin;
   my_grid = grid;
   if ( last < first || first < 0 || last > grid->nd ) {
      throw (baddataindex());
   }
}

/// override operator =
GridField3D::iterator& GridField3D::iterator::operator=(const GridField3D::iterator& src)
{
   my_index = src.my_index;
   first = src.first;
   last = src.last;
   my_grid  = src.my_grid;
   
   return *this;
}


/// override operator *, returns a pointer to the current data element
real& GridField3D::iterator::operator*() const 
{
   
   if ( my_grid->use_array ) {     
      if ( my_index >= 0 && my_index < my_grid->nd ) {
         return (my_grid->dater)[my_index];      
      } else {
         throw (baddataindex());      
      }
   } else {
     if ( my_index >= first && my_index <= last ) {
        return (my_grid->data).at( my_index );
     } else {
        throw (baddataindex());
     }
   }
}

/// override operator == ; tests two iterators for equality
bool GridField3D::iterator::operator==(const iterator& x) const
{
   return ( my_index == x.my_index );
}

/// override operator != ; tests two iterators for inequality
bool GridField3D::iterator::operator!=(const iterator& x) const
{
   return ( my_index != x.my_index );
}

/// override operator ++ ; increments the iterator to point to the next data element
GridField3D::iterator& GridField3D::iterator::operator++(int n) 
{
   my_index++;
   
   return *this;
}

void GridField3D::iterator::indices( int* i, int*j, int *k ) const
{
   my_grid->splitIndex( my_index, i, j, k );
}

void GridField3D::iterator::assign( real val )
{
  if ( my_grid->use_array ) {
     (my_grid->dater)[my_index] = val;
  } else {
     (my_grid->data).at(my_index) = val;  
  }
}


real GridField3D::iterator::area() const
{
    real ar;
    int i,j,k;

    my_grid->splitIndex( my_index, &i, &j, &k );

    ar = my_grid->area(i,j);

    return ar;
}    


/////////////////////////////// const_iterator

GridField3D::const_iterator::const_iterator()
{
   my_index = 0;
   first = 0;
   last = 0;
   my_grid = NULLPTR;
}   


GridField3D::const_iterator::const_iterator( const GridField3D* grid, int n, int beg, int fin )
{
   my_index = n;
   first = beg;
   last = fin;
   my_grid = grid;
   if ( last < first || first < 0 || last > grid->nd ) {
      throw (baddataindex());
   }
}

/// override operator =
GridField3D::const_iterator& GridField3D::const_iterator::operator=(const GridField3D::const_iterator& src)
{
   my_index = src.my_index;
   first = src.first;
   last = src.last;
   my_grid  = src.my_grid;
   
   return *this;
}


/// override operator *, returns the current data element
real GridField3D::const_iterator::operator*() const 
{
   if ( my_grid->use_array) {
      if ( my_index >= 0 && my_index < my_grid->nd ) {
         return (my_grid->dater)[my_index];
      } else {
         throw (baddataindex());
      }
   } else {
      if ( my_index >= 0 && my_index < my_grid->data.size() ) {
         return (my_grid->data)[my_index];
      } else {
         throw (baddataindex());
      }
   }
}

/// override operator == ; tests two const_iterators for equality
bool GridField3D::const_iterator::operator==(const const_iterator& x) const
{
   return ( my_index == x.my_index );
}

/// override operator != ; tests two const_iterators for inequality
bool GridField3D::const_iterator::operator!=(const const_iterator& x) const
{
   return ( my_index != x.my_index );
}

/// override operator ++ ; increments the const_iterator to point to the next data element
GridField3D::const_iterator& GridField3D::const_iterator::operator++(int n) 
{
   my_index++;
   
   return *this;
}

void GridField3D::const_iterator::indices( int* i, int*j, int *k ) const
{
   my_grid->splitIndex( my_index, i, j, k );
}

real GridField3D::const_iterator::area() const
{
    real ar;
    int i,j,k;

    my_grid->splitIndex( my_index, &i, &j, &k );

    ar = my_grid->area(i,j);

    return ar;
}    

/////////////////////////////// profileIterator

GridField3D::profileIterator::profileIterator()
{
   my_index = 0;
   plen = 0;
   first = 0;
   last = 0;
   skip = 0;
   my_grid = NULLPTR;
}   

GridField3D::profileIterator::profileIterator( GridField3D* grid, int n, int len, int beg, int fin, int skp )
{
    plen = len;
    first = beg;
    last = fin;
    skip = skp;
    if ( last < first || first < 0 || last > grid->nd || skp <= 0 ) {
       throw (baddataindex()); 
    }
    my_index = n;
    my_grid = grid;

}

/// override operator =
GridField3D::profileIterator& GridField3D::profileIterator::operator=(const GridField3D::profileIterator& src)
{
   my_index = src.my_index;
   plen     = src.plen;
   first    = src.first;
   last     = src.last;
   skip     = src.skip;
   my_grid  = src.my_grid;
   
   return *this;
}


/// override operator *, returns a pointer to the current data profile
std::vector<real>* GridField3D::profileIterator::operator*() const 
{
   std::vector<real>* profile;
   real val;

   if ( my_index >= first && my_index <= last ) {
      profile = new std::vector<real>;
      profile->reserve(plen);
      for ( int k=0; k < plen ; k++ ) {
          profile->push_back( (my_grid->dater)[my_index + k*skip] );
      }
      return profile;
   } else {
      throw (baddataindex());
   }
}

void GridField3D::profileIterator::assign( const std::vector<real>& src ) 
{

   if ( my_index >= first && my_index <= last && plen == src.size() ) {
      for ( int k=0; k<plen; k++ ) {
          (my_grid->dater)[my_index + k*skip] = src[k];
      }
   } else {
      throw (baddataindex());
   }


}

/// override operator == ; tests two profileIterators for equality
bool GridField3D::profileIterator::operator==(const profileIterator& x) const
{
   return ( my_index == x.my_index );
}

/// override operator != ; tests two profileIterators for inequality
bool GridField3D::profileIterator::operator!=(const profileIterator& x) const
{
   return ( my_index != x.my_index );
}

/// override operator ++ ; increments the profileIterator to point to the beginning of the next profile
GridField3D::profileIterator& GridField3D::profileIterator::operator++(int n) 
{
   my_index++;
   
   return *this;
}

void GridField3D::profileIterator::indices( int *i, int *j ) const
{
   int k;

   my_grid->splitIndex( my_index, i, j, &k );
}

real GridField3D::profileIterator::area() const
{
    real ar;
    int i,j,k;

    my_grid->splitIndex( my_index, &i, &j, &k );

    ar = my_grid->area(i,j);

    return ar;
}    


/////////////////////////////// const_profileIterator

GridField3D::const_profileIterator::const_profileIterator()
{
   my_index = 0;
   plen = 0;
   first = 0;
   last = 0;
   skip = 0;
   my_grid = NULLPTR;
}   

GridField3D::const_profileIterator::const_profileIterator( const GridField3D* grid, int n, int len, int beg, int fin, int skp )
{
    plen = len;
    first = beg;
    last = fin;
    skip = skp;
    if ( last < first || first < 0 || last > grid->nd || skp <= 0 ) {
       throw (baddataindex()); 
    }
    my_index = n;
    my_grid = grid;

}

/// override operator =
GridField3D::const_profileIterator& GridField3D::const_profileIterator::operator=(const GridField3D::const_profileIterator& src)
{
   my_index = src.my_index;
   plen     = src.plen;
   first    = src.first;
   last     = src.last;
   skip     = src.skip;
   my_grid  = src.my_grid;
   
   return *this;
}


/// override operator *, returns a pointer to the current data profile
std::vector<real>* GridField3D::const_profileIterator::operator*() const 
{
   std::vector<real>* profile;
   real val;

   if ( my_index >= first && my_index <= last ) {
      profile = new std::vector<real>;
      profile->reserve(plen);
      for ( int k=0; k<plen ; k++ ) {
          profile->push_back( (my_grid->dater)[my_index + k*skip] );
      }
      return profile;
   } else {
      throw (baddataindex());
   }
}


/// override operator == ; tests two const_profileIterators for equality
bool GridField3D::const_profileIterator::operator==(const const_profileIterator& x) const
{
   return ( my_index == x.my_index );
}

/// override operator != ; tests two const_profileIterators for inequality
bool GridField3D::const_profileIterator::operator!=(const const_profileIterator& x) const
{
   return ( my_index != x.my_index );
}

/// override operator ++ ; increments the const_profileIterator to point to the beginning of the next profile
GridField3D::const_profileIterator& GridField3D::const_profileIterator::operator++(int n) 
{
   my_index++;
   
   return *this;
}

void GridField3D::const_profileIterator::indices( int *i, int *j ) const
{
   int k;
   
   my_grid->splitIndex( my_index, i, j, &k );
}

real GridField3D::const_profileIterator::area() const
{
    real ar;
    int i,j,k;

    my_grid->splitIndex( my_index, &i, &j, &k );

    ar = my_grid->area(i,j);

    return ar;
}    

