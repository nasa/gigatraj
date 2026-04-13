
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

#include "gigatraj/Filter_Null.hh"

using namespace gigatraj;

// constructor
Filter_Null::Filter_Null( int useless )
{
     unused = useless;
     
}

// destructor
Filter_Null::~Filter_Null()
{

}

Filter_Null::Filter_Null(const Filter_Null& src) : ParcelFilter(src)
{
    unused = src.unused;
}

Filter_Null& Filter_Null::operator=(const Filter_Null& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }
    
    ParcelFilter::assign(src);
    
    this->assign( src ) ;
    
    return *this;
}

void Filter_Null::assign( const Filter_Null& src)
{
    unused = src.unused;     
}


int Filter_Null::useless() const
{
    return unused;
}

void Filter_Null::useless( int value )
{
     unused = value;
}


void Filter_Null::apply( Parcel& p )
{
      // we do nothing to the Parcel
}

void Filter_Null::apply( Parcel * const p, const int n )
{          
    int i;
    
    if ( n < 0 ) {
       throw (ParcelFilter::badparcelnum());
    };
    
    for ( i=0; i<n; i++ ) {
    
       apply( p[i] );
    
    }   
}

void Filter_Null::apply( std::vector<Parcel>& p )
{
   std::vector<Parcel>::iterator ip;
   
   for ( ip=p.begin(); ip != p.end(); ip++ ) {

       apply( *ip );

   }

}

void Filter_Null::apply( std::list<Parcel>& p )
{
   std::list<Parcel>::iterator ip;
    
   for ( ip=p.begin(); ip != p.end(); ip++ ) {
    
       apply( *ip );

   }

}

void Filter_Null::apply( std::deque<Parcel>& p )
{
   std::deque<Parcel>::iterator ip;    
    
   for ( ip=p.begin(); ip != p.end(); ip++ ) {
    
       apply( *ip );

   }   

}

void Filter_Null::apply( Flock& p )
{
    Flock::iterator iter;

    for ( iter=p.begin(); iter != p.end(); iter++ ) {
        apply( *iter );
    }
    
}

void Filter_Null::apply( Swarm& p )
{
    Swarm::iterator iter;
      
    for ( iter=p.begin(); iter != p.end(); iter++ ) {
        apply( *iter );
    }

}

