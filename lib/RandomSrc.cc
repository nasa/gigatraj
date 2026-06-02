
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

#include <fstream>
#include <ctime>
#include <cstdlib>

#include "gigatraj/RandomSrc.hh"

using namespace gigatraj;


RandomSrc::RandomSrc() 
{
    unsigned int saw;
    
    rdev = "/dev/random";
    
    saw = genSeed();
    
    srand(saw);
    
};

RandomSrc::RandomSrc( unsigned int saw )
{
   rdev = "/dev/random";
    
   srand(saw);
}

RandomSrc::RandomSrc( const std::string dev )
{
    unsigned int saw;
    
    rdev = dev;
    
    saw = genSeed();
    
    srand(saw);

}


RandomSrc::~RandomSrc() {

}


real RandomSrc::uniform( real min, real max ) 
{
    real result;
    
    
    result = (real)(rand() % 32768)/32767.0;
    result = result * (max-min) + min;

    return result;
}       

int RandomSrc::raw()
{
   return rand();
}   

unsigned int RandomSrc::genSeed()
{
    unsigned int saw;

    try {
       // first, try to read the seed in from /dev/random
       
       // make a char array the size of "saw"
       char cc[sizeof(saw)+1];
       // open /dev/random
       std::ifstream r(rdev);
       // read those N bytes of randomness
       r.get(cc, sizeof(saw));
       r.close();
       // play with pointers to transfer the bytes just read to the seed
       saw = 0;
       char *cin = cc;
       char *cout = (char *)(&saw);
       for (int i=0; i<sizeof(saw); i++ ) {
           *cout = *cin;
           cin++;
           cout++;
       }
    } catch (...) {
       // or just get it from the time
       std::cerr << "no " << rdev << " access" << std::endl;
       saw = (unsigned int) time(NULLPTR);
    }
    
    return saw;

}

void RandomSrc::seed( const unsigned int *saw )
{
     unsigned int saw2;
     
     if ( saw == NULL ) {
        saw2 = genSeed();
     } else {
        saw2 = *saw;
     }   

     srand(saw2);

}

