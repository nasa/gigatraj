
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


#include "gigatraj/PGenNetcdf.hh"

using namespace gigatraj;

PGenNetcdf::PGenNetcdf()
{
     dbug = 0;
     fname = "";
     vcoord = "";
     caltime = "";
     time0 = 0.0;
     atend = false;
     n = 0;
     
}

PGenNetcdf::~PGenNetcdf()
{

}

// copy constructor
PGenNetcdf::PGenNetcdf(const PGenNetcdf& src) : ParcelGenerator(src)
{
     dbug = src.dbug;
     fname = src.fname;
     vcoord = src.vcoord;
     caltime = src.caltime;
     time0 = src.time0;
     atend = false;
     n = src.n;
     ncIn = src.ncIn;
     pgen = src.pgen;
}

PGenNetcdf& PGenNetcdf::operator=(const PGenNetcdf& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }
    
    this->assign( src ) ;
    
    return *this;
}

void PGenNetcdf::assign( const PGenNetcdf& src)
{

    ParcelGenerator::assign(src);

     dbug = src.dbug;
     fname = src.fname;
     vcoord = src.vcoord;
     caltime = src.caltime;
     time0 = src.time0;
     atend = false;
     n = src.n;
     ncIn = src.ncIn;
     pgen = src.pgen;
    
}



void PGenNetcdf::open( const std::string &file)
{
     if ( ( file != fname ) && ncIn.isOpen() ) {
        ncIn.close();
     }
     
     if ( file != "" ) {
        fname = file;
     }
     
     if ( fname != "" ) {
        if (vcoord != "" ) {
           ncIn.vertical( vcoord );
        }
        ncIn.at_end( atend );
        ncIn.debug( dbug );
     
        ncIn.open( fname );
     
        n = ncIn.n_parcels();
        vcoord = ncIn.vertical();
        caltime = ncIn.cal();
        time0 = ncIn.time();
     }   
}

bool PGenNetcdf::isOpen()
{
     return ncIn.isOpen();
}

Parcel * PGenNetcdf::create_array(const Parcel& p, size_t *np
                     , const std::string &file
                     ) 
{
     Parcel *result;
     
     if ( ! ncIn.isOpen() ) {
        open( file );
     }
     
     
     result = pgen.create_array( p, n );
     
     ncIn.apply( result, n );
     
     ncIn.close();
     
     *np = n;
     return result;
}

std::vector<Parcel>* PGenNetcdf::create_vector(const Parcel& p
                    , const std::string &file
                    )

{
    std::vector<Parcel>* result;
    
     if ( ! ncIn.isOpen() ) {
        open( file );
     }
     
     result = pgen.create_vector( p, n );
     
     ncIn.apply( *result );
     
     ncIn.close();
     
     return result;

}



std::list<Parcel>* PGenNetcdf::create_list(const Parcel& p
                    , const std::string &file 
                    )
{
    std::list<Parcel>* result;
    
     if ( ! ncIn.isOpen() ) {
        open( file );
     }
     
     result = pgen.create_list( p, n );
     
     ncIn.apply( *result );
     
     ncIn.close();
     
     return result;


}
      


std::deque<Parcel>* PGenNetcdf::create_deque(const Parcel& p
                    , const std::string &file
                    )
{
    std::deque<Parcel>* result;
    
     if ( ! ncIn.isOpen() ) {
        open( file );
     }
     
     result = pgen.create_deque( p, n );
     
     ncIn.apply( *result );
     
     ncIn.close();
     
     return result;

}
      


Flock* PGenNetcdf::create_Flock(const Parcel& p
                    , const std::string &file, ProcessGrp* pgrp, int r
                    )
{
    Flock* result;
    
     if ( ! ncIn.isOpen() ) {
        open( file );
     }
     
     result = pgen.create_Flock( p, n, pgrp, r );
     
     ncIn.apply( *result );
     
     ncIn.close();
     
     return result;

}
      
            

Swarm* PGenNetcdf::create_Swarm(const Parcel& p
                    , const std::string &file, ProcessGrp* pgrp, int r
                    )
{
    Swarm* result;
    
     if ( ! ncIn.isOpen() ) {
        open( file );
     }
     
     result = pgen.create_Swarm( p, n, pgrp, r );
     
     ncIn.apply( *result );
     
     ncIn.close();
     
     return result;

}



std::string& PGenNetcdf::filename()
{
    return fname;
}




void PGenNetcdf::vertical( std::string vert )
{
    vcoord = vert;
    //ncIn.vertical( vert );
}
   
std::string& PGenNetcdf::vertical()
{
   return vcoord;
}

   
std::string& PGenNetcdf::cal()
{
    return caltime;
}
   
double PGenNetcdf::time()
{
    return time0;
}

void PGenNetcdf::at_end( bool select )
{
     atend = select;
     //ncIn.at_end( select );
}
     
bool PGenNetcdf::at_end()
{
    return atend;
}
     
size_t PGenNetcdf::n_parcels()
{
    return n;
}
        
void PGenNetcdf::debug( int mode )
{
     dbug = mode;
     //ncIn.debug( mode );
}
    
int PGenNetcdf::debug()
{
    return dbug;
}

void PGenNetcdf::readFlags( int mode )
{
    ncIn.readFlags( mode );
}

int PGenNetcdf::readFlags()
{
    return ncIn.readFlags();
}


void PGenNetcdf::readStatus( int mode )
{
    ncIn.readStatus( mode );
}


int PGenNetcdf::readStatus()
{
    return ncIn.readStatus();
}


void PGenNetcdf::readTag( int mode )
{
    ncIn.readTag( mode );
}


int PGenNetcdf::readTag()
{
    return ncIn.readTag();
}


void PGenNetcdf::format( std::string fmt )
{
    ncIn.format( fmt );
}

  
std::string PGenNetcdf::format()
{
    return ncIn.format();
}

      
void PGenNetcdf::setTimeTransform( double scale, double offset )
{
     ncIn.setTimeTransform( scale, offset );
}

void PGenNetcdf::getTimeTransform( double* scale, double* offset )
{
     ncIn.getTimeTransform( scale, offset );
}
    
