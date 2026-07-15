
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

#include "gigatraj/Filter_Add.hh"

using namespace gigatraj;

// constructor
Filter_Add::Filter_Add( ParcelFlag flagset, bool negate )
{
     flgs = flagset;
     ng = negate; 
     cnt =0;
     newp = NULLPTR;
}

// destructor
Filter_Add::~Filter_Add()
{

}

Filter_Add::Filter_Add(const Filter_Add& src) : ParcelFilter(src)
{
    flgs = src.flgs;
    ng = src.ng;
    cnt = src.cnt;
    newp = src.newp;
}

Filter_Add& Filter_Add::operator=(const Filter_Add& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }
    
    ParcelFilter::assign(src);
    
    this->assign( src ) ;
    
    return *this;
}

void Filter_Add::assign( const Filter_Add& src)
{
    flgs = src.flgs;     
    ng = src.ng;
    cnt = src.cnt;
    newp = src.newp;
}


ParcelFlag Filter_Add::flagset() const
{
    return flgs;
}

void Filter_Add::flagset( ParcelFlag value )
{
     flgs = value;
}

bool Filter_Add::negation() const
{
    return ng;
}

void Filter_Add::negation( bool value )
{
     ng = value;
}

std::vector<Parcel>* Filter_Add::source() const
{
    return newp;
}

void Filter_Add::source( std::vector<Parcel>* src ) 
{
    newp = src;
}

bool Filter_Add::test( Parcel* p ) const
{
      bool result;
      
      result =  (p->flags() & flgs) != 0;
      if ( ng ) {
         result = ! result;
      }

      return result;
}

int Filter_Add::nNew() const     
{
    int result;
    
    result = 0;
    if ( newp != NULLPTR ) {
       result = newp->size();
    }   

    return result;
}

void Filter_Add::apply( Parcel& p )
{
      ParcelFlag mask;
      int howmany;
      
      howmany = nNew();
            
      switch (howmany) {
      case 0: // do nothing
              break;
      case 1: // copy the one Parcel 
              // but only if that one parcel is available
              if ( test( &p ) ) {       
                 p = (*newp)[0];
              } else {
                 throw (badoutofparcels());
              }
              break;
      default: // too many new Parcels        
            throw (badoutofparcels());
            break;
      }
      
}

void Filter_Add::apply( Parcel * const p, const int n )
{          
    int i;
    int j;
    int howmany;
    
    // how many new parcels must we find room for?
    howmany = nNew();
    
    if ( n < 0 ) {
       throw (ParcelFilter::badparcelnum());
    };
       
    // now find idle parcels to hold the new ones
    if ( howmany > 0 ) {
       j = 0;
       for ( i=0; i<n; i++ ) {
           if ( test( p + i ) ) {
              if ( j < howmany ) {
                 p[i] = (*newp)[j];
                 j++;
              }  
           }
        }
        if ( j != howmany ) {
           throw (badoutofparcels());              
        }
    }   
}

void Filter_Add::apply( std::vector<Parcel>& p )
{
   std::vector<Parcel>::iterator ip;
   int j;
   int howmany;
   
   // how many new parcels must we find room for?
   howmany = nNew();
   
   if ( howmany > 0 ) {
      j = 0;
      for ( ip=p.begin(); ip != p.end(); ip++ ) {
          if ( test( &(*ip)) ) {
             if ( j < howmany ) {
                *ip = (*newp)[j];
                j++;
             }   
           }
      }
      if ( j != howmany ) {
         throw (badoutofparcels());              
      }
   }

}

void Filter_Add::apply( std::list<Parcel>& p )
{
   std::list<Parcel>::iterator ip;
   int j;
   int howmany;
   
   // how many new parcels must we find room for?
   howmany = nNew();
   
   if ( howmany > 0 ) {
      j = 0;
      for ( ip=p.begin(); ip != p.end(); ip++ ) {
          if ( test( &(*ip)) ) {
             if ( j < howmany ) {
                *ip = (*newp)[j];
                j++;
             }   
           }
      }
      if ( j != howmany ) {
         throw (badoutofparcels());              
      }
   }

}

void Filter_Add::apply( std::deque<Parcel>& p )
{
   std::deque<Parcel>::iterator ip;    
   int j;
   int howmany;
   
   // how many new parcels must we find room for?
   howmany = nNew();
   
   if ( howmany > 0 ) {
      j = 0;
      for ( ip=p.begin(); ip != p.end(); ip++ ) {
          if ( test( &(*ip)) ) {
             if ( j < howmany ) {
                *ip = (*newp)[j];
                j++;
             }   
           }
      }
      if ( j != howmany ) {
         throw (badoutofparcels());              
      }
   }
    
}

void Filter_Add::apply( Flock& p )
{
   int i;       
   int j;
   int howmany;
   Parcel *px;
   int my_procid;
   int my_root;
   int parcel_owner;
   bool am_root;
   ProcessGrp *pgrp;
   
   // how many new parcels must we find room for?
   howmany = nNew();
   
   if ( howmany > 0 ) {
      j = 0;

      // sync all the processors before we start loading
      p.sync();
      
      my_procid = p.proc_id();
      my_root = p.root_id();
      am_root = p.is_root();
      pgrp = p.getPGroup();
      
      for ( i=0; i<p.size(); i++ ) {
      
          // we get the parcel only if we are root or the parcel's owner process
          px = p.parcel( i, 1 );
          if ( px != NULLPTR ) {
             // both the root processor and the parcel's owner get to this point
             // but only the root processor has
             // a valid value for "j"
             // so send it
             parcel_owner = p.belongs(i);
             if ( test( px ) ) {
                if ( (! am_root) || (parcel_owner != my_procid ) ) {
                   if ( am_root ) {
                      // send j
                      pgrp->send_ints( parcel_owner, 1, &j, 137 );
                   } else {
                      // receive j
                      pgrp->receive_ints( my_root, 1, &j, 137 );
                   }
                }
                if ( j < howmany ) {
                   // set this
                   p.set( i, (*newp)[j], 0 );
                   px = p.parcel(i, 1 );
                   j++;
                }   
             }   
          }

      }   
          
      if ( p.is_root() && (j != howmany) ) {
         throw (badoutofparcels());              
      }
   }
    
}

void Filter_Add::apply( Swarm& p )
{
   int i;       
   int j;
   int howmany;
   Parcel *px;
   int my_procid;
   int my_root;
   int parcel_owner;
   bool am_root;
   ProcessGrp *pgrp;
   
   // how many new parcels must we find room for?
   howmany = nNew();
   
   if ( howmany > 0 ) {
      j = 0;

      // the root processor should do all of the work here
      
      // sync all the processors before we start loading
      p.sync();
      
      my_procid = p.proc_id();
      my_root = p.root_id();
      am_root = p.is_root();
      pgrp = p.getPGroup();
      
      for ( i=0; i<p.size(); i++ ) {
      
          px = p.parcel( i, 1 );
          if ( px != NULLPTR ) {
             // both the root processor and the parcel's owner get to this point
             // but only the root processor has
             // a valid value for "j"
             // so send it
             parcel_owner = p.belongs(i);
             if ( test( px ) ) {
                if ( (! am_root) || (parcel_owner != my_procid ) ) {
                   if ( am_root ) {
                      // send j
                      pgrp->send_ints( parcel_owner, 1, &j, 137 );
                   } else {
                      // receive j
                      pgrp->receive_ints( my_root, 1, &j, 137 );
                   }
                }
                if ( j < howmany ) {
                   // set this
                   p.set( i, (*newp)[j], 0 );
                   px = p.parcel(i, 1 );
                   j++;
                }   
             }   

          }

      }   
      
      if ( p.is_root() && (j != howmany) ) {
         throw (badoutofparcels());              
      }
   }

}


