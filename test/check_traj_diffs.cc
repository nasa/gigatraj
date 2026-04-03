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


/*!

Reads two trajectory output files and compares the parcel positions
*/

#include <stdlib.h>     
#include <iostream>
#include <istream>
#include <fstream>
#include <math.h>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/Earth.hh"

#include "test_utils.hh"

using namespace gigatraj;
using std::cerr;
using std::endl;
using std::istringstream;

int main(int argc, char**argv) 
{

    Earth e;
    std::string file0;
    double t0;
    int id0;
    real lon0;
    real lat0;
    real z0;
    std::string file1;
    double t1;
    int id1;
    real lon1;
    real lat1;
    real z1;
    std::string sep;
    real z_diff;
    real dist;
    real t_diff;
    bool is_bad;
        


    if ( argc != 3 ) {
       cerr << "Must have two arguments, not " << argc << endl;
    }
    
    file0.assign( argv[1] );
    file1.assign( argv[2] );
    
    cerr << "Comparing " << file0 << " with " << file1 << endl;
    
    std::ifstream input0(file0.c_str());
    if ( ! input0.good() ) {
       cerr << "Could not open " << file0 ;
    }
    std::ifstream input1(file1.c_str());
    if ( ! input1.good() ) {
       cerr << "Could not open " << file1 ;
    }
    
     try {

        // catch I/O errors
        input0.exceptions(std::ios::badbit);
        input1.exceptions(std::ios::badbit);

        while ( (! input0.eof()) && (! input1.eof()) ) {
           input0 >> t0 >> sep >> id0 >> sep >> lon0 >> sep >> lat0 >> sep >>z0;
           input1 >> t1 >> sep >> id1 >> sep >> lon1 >> sep >> lat1 >> sep >> z1;
        
           is_bad = false;
           
           if ( id0 != id1 ) {
              is_bad = true;
              cerr << "Parcel ID mismatch" << endl;
           }
           if ( mismatch( t0, t1, 0.0001) ) {
              is_bad = true;
              cerr << " Time mismatch" << endl;
           }
           dist = e.distance(lon0, lat0, lon1, lat1);
           //if ( t0 > 0.0 ) {
           //   dist = dist/t0;
           //}
           if ( mismatch( 0.0, dist, 1.5) ) {
              is_bad = true;
              cerr << " Horiz Position mismatch" << endl;
           }
           if ( mismatch( z0, z1, 0.0001) ) {
              is_bad = true;
              cerr << " Vert Position mismatch" << endl;
           }
                      
           if ( is_bad ) {
              cerr << "   0: " << id0 << ", " << t0 << ", " << id0 << ", " << lon0 << ", " << lat0 << ", " << z0 << endl;
              cerr << "   1: " << id1 << ", "  << t1 << ", " << id1 << ", " << lon1 << ", " << lat1 << ", " << z1 << endl;
              cerr << "   dist = " << dist << endl;
              exit(1);
           }  
        }
     
        if ( ! input0.eof() ) {
           cerr << file0 << " is longer than " << file1 << endl;
           exit(1);
        }   
        if ( ! input1.eof() ) {
           cerr << file0 << " is shorter than " << file1 << endl;
           exit(1);
        }
        
     } catch (...) {
        cerr << "I/O error";
     }
    
    
    // close the files
    input0.close();
    input1.close();
    

}
