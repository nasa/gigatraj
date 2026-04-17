
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

#include "gigatraj/CountFlags.hh"

using namespace gigatraj;

// constructor
CountFlags::CountFlags( ParcelFlag flagset, bool negate )
{
     flgs = flagset;
     ng = negate; 
     cnt =0;
}

// destructor
CountFlags::~CountFlags()
{

}

CountFlags::CountFlags(const CountFlags& src) : ParcelFilter(src)
{
    flgs = src.flgs;
    ng = src.ng;
    cnt = src.cnt;
}

CountFlags& CountFlags::operator=(const CountFlags& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }
    
    ParcelFilter::assign(src);
    
    this->assign( src ) ;
    
    return *this;
}

void CountFlags::assign( const CountFlags& src)
{
    flgs = src.flgs;     
    ng = src.ng;
    cnt = src.cnt;
}


ParcelFlag CountFlags::flagset() const
{
    return flgs;
}

void CountFlags::flagset( ParcelFlag value )
{
     flgs = value;
}

bool CountFlags::negation() const
{
    return ng;
}

void CountFlags::negation( bool value )
{
     ng = value;
}

void CountFlags::reset()
{
     cnt = 0;
}

void CountFlags::apply( Parcel& p )
{
      ParcelFlag mask;
      bool tst;
      
      tst =  (p.flags() & flgs) != 0;
      if ( ng ) {
         tst = ! tst;
      }
            
      if ( tst ) {
         cnt++;
      }
      
}

void CountFlags::apply( Parcel * const p, const int n )
{          
    int i;
    
    reset();

    if ( n < 0 ) {
       throw (ParcelFilter::badparcelnum());
    };
    
    for ( i=0; i<n; i++ ) {
    
       apply( p[i] );
    
    }   
}

void CountFlags::apply( std::vector<Parcel>& p )
{
   std::vector<Parcel>::iterator ip;
   
   reset();
   for ( ip=p.begin(); ip != p.end(); ip++ ) {

       apply( *ip );

   }

}

void CountFlags::apply( std::list<Parcel>& p )
{
   std::list<Parcel>::iterator ip;
    
   reset();
   for ( ip=p.begin(); ip != p.end(); ip++ ) {
    
       apply( *ip );

   }

}

void CountFlags::apply( std::deque<Parcel>& p )
{
   std::deque<Parcel>::iterator ip;    
    
   reset();
   for ( ip=p.begin(); ip != p.end(); ip++ ) {
    
       apply( *ip );

   }   

}

void CountFlags::apply( Flock& p )
{

    cnt = p.countFlags( flgs, ng );
    
}

void CountFlags::apply( Swarm& p )
{

    cnt = p.countFlags( flgs, ng );

}

bool CountFlags::query( const std::string keyword, int* value ) const
{
     // we ignore the keyword, which should be "count"
     
     if ( value != NULLPTR ) {
        *value = cnt;
     }
     
     return true;
}

