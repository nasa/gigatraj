
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

#include "gigatraj/Filter_Trop.hh"

using namespace gigatraj;

// constructor
Filter_Trop::Filter_Trop( int direction, real tolerance, std::string quantity, std::string field)
{
     neg = false;
     dir = direction;
     tol = tolerance;
     if ( tol < 0 ) {
        neg = true;
        tol = - tol;
     }
     if ( quantity != "" ) {
        tquant = quantity;
     } else {   
        tquant = "Trop";
     }
     if ( field != "" ) {
        tkind = field;
     } else {   
        tkind = "alt";
     }
     
}

// destructor
Filter_Trop::~Filter_Trop()
{

}

Filter_Trop::Filter_Trop(const Filter_Trop& src) : ParcelFilter(src)
{
     neg = src.neg;
     dir = src.dir;
     tol = src.tol;
     tkind = src.tkind;
}

Filter_Trop& Filter_Trop::operator=(const Filter_Trop& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }

    ParcelFilter::assign(src);

    this->assign( src ) ;
    
    return *this;
}

void Filter_Trop::assign( const Filter_Trop& src)
{
     ParcelFilter::assign( src );
     neg = src.neg;
     dir = src.dir;
     tol = src.tol;
     tkind = src.tkind;
}

std::string Filter_Trop::quantity() const
{
    return tquant;
}

void Filter_Trop::quantity( std::string q )
{
     tquant = q;
}

std::string Filter_Trop::field() const
{
    return tkind;
}

void Filter_Trop::field( std::string q )
{
     tkind = q;
}

int Filter_Trop::direction() const
{
    return dir;
}

void Filter_Trop::direction( int d )
{
     switch (d) {
     case -1: dir = -1;
            break;
     case  0: dir = 0;
            break;
     case  1: dir = 1;
            break;
     }       

}

real Filter_Trop::threshold() const
{
    return tol;
}

void Filter_Trop::threshold( real t )
{
     tol = t;
}

bool Filter_Trop::negation() const
{
    return neg;
}

void Filter_Trop::negation( bool n )
{
     neg = n;
}

void Filter_Trop::apply( Parcel& p )
{
     MetData* metsrc;
     double time;
     real lat, lon, vert;
     real tropp, tropz;
     real z;
     std::string paname;
     std::string prsname;
     bool halt;
     real zdif;
     std::string trop_units;
     bool tooLow;
     bool tooHigh;
     int vertdir;
     int kinddir;
     real zu;
     real zl;

     // we only test Parcels that are being traced
     if ( ! p.queryNoTrace() ) {

        metsrc = p.getMet();
        time = p.getTime();
        p.getPos( &lon, &lat );
        vert = p.getZ();
        if ( ! metsrc->getOption( "PressureAltitudeName", paname ) ) {
           paname = "PAlt";
        }
        if ( ! metsrc->getOption( "PressureName", prsname ) ) {
           prsname = "P";
        }
    
        // does the vertical coordinate increase or decrease with altitude?
        vertdir = metsrc->vIncrease();
        
        
        // we assume that we will halt the Parcel,
        // so that Parcels with invalid tropopause values
        // will be halted, in addition to those that
        // trigger the tropopause tests below.
        halt = true;
        

        if ( metsrc->vertical() == tkind ) {
           
           kinddir =vertdir;

           // get the trop vert coord at this parcel's location
           tropz = metsrc->getData( tquant, time, lon, lat, vert, METDATA_NANBAD );

           if ( FINITE(tropz) ) {
              // note; there is a possibility here
              // that tropz will be in different units than vert
              // fixing will require some reform in how MetData handles
              // vertical coordinates.
              
              z = vert;
              zl = z - vertdir*tol;
              zu = z + vertdir*tol;
           }
        } else {
        
           kinddir = 1;
           if ( tkind == prsname ) {
              kinddir = -1;
           }
        
           // get the trop value at this parcel's location
           tropz = metsrc->getData( tquant, time, lon, lat, vert, METDATA_NANBAD | METDATA_MKS );

           if ( FINITE(tropz) ) {
              // convert the parcel's vertical coordinate to tropopause quantity
              //z = metsrc->getData( tkind, time, lon, lat, vert, METDATA_NANBAD | METDATA_MKS );
              zu = metsrc->getData( tkind, time, lon, lat, vert + vertdir*tol, METDATA_NANBAD | METDATA_MKS );
              zl = metsrc->getData( tkind, time, lon, lat, vert - vertdir*tol, METDATA_NANBAD | METDATA_MKS );
           }
        } 
           
        if ( FINITE(tropz) ) {
           if ( FINITE(zu) && FINITE(zl) ) {
              if ( kinddir >= 0 ) {
                 tooLow  = zu < tropz;
                 tooHigh = zl > tropz;
              } else {
                 tooLow = zu > tropz;
                 tooHigh = zl < tropz;                 
              }
           }

           switch (dir) {
           case -1: // must be below the trop 
           
              halt = tooHigh;
           
              break;
           case  1: // must be above the trop
           
              halt = tooLow;

              break;
           case 0: // must be close to the trop
              
              halt = tooLow || tooHigh;
              
              break;
           }
                 
           if ( neg ) {
              halt = ! halt;
           }
           
           
           if ( halt ) {
              p.setNoTrace();
           }

        }
     }
}

void Filter_Trop::apply( Parcel * const p, const int n )
{          
    int i;
    
    if ( n < 0 ) {
       throw (ParcelFilter::badparcelnum());
    };
    
    for ( i=0; i<n; i++ ) {
    
       apply( p[i] );
    
    }   
}

void Filter_Trop::apply( std::vector<Parcel>& p )
{
   std::vector<Parcel>::iterator ip;
   
   for ( ip=p.begin(); ip != p.end(); ip++ ) {

       apply( *ip );

   }

}

void Filter_Trop::apply( std::list<Parcel>& p )
{
   std::list<Parcel>::iterator ip;
    
   for ( ip=p.begin(); ip != p.end(); ip++ ) {
    
       apply( *ip );

   }

}

void Filter_Trop::apply( std::deque<Parcel>& p )
{
   std::deque<Parcel>::iterator ip;    
    
   for ( ip=p.begin(); ip != p.end(); ip++ ) {
    
       apply( *ip );

   }   

}

void Filter_Trop::apply( Flock& p )
{
    Flock::iterator iter;
    for ( iter=p.begin(); iter != p.end(); iter++ ) {
        apply( *iter );
    }
        
}

void Filter_Trop::apply( Swarm& p )
{
    Swarm::iterator iter;
      
    for ( iter=p.begin(); iter != p.end(); iter++ ) {
        apply( *iter );
    }

}

