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





#include <cstdlib>
#include <iostream>
#include <sstream>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/RandomSrc.hh"
#include "gigatraj/GridLatLonField3D.hh"
#include "gigatraj/GridLatLonFieldSfc.hh"
#include "gigatraj/TropOTF.hh"

#include "test_utils.hh"

using namespace gigatraj;
using std::cerr;
using std::endl;
using std::ostringstream;
using std::istringstream;
using std::operator<<;
using std::operator>>;

int main() 
{
    RandomSrc rnd(1234);
    GridLatLonField3D t_grid, a_grid;
    GridLatLonField3D *tmp1;
    GridLatLonFieldSfc t_sfc, p_sfc;
    GridLatLonField3D theta_1, theta_2;
    GridLatLonFieldSfc theta_3;
    GridLatLonFieldSfc *tmp2;
    std::vector<real> olons;
    std::vector<real> olats;
    std::vector<real> opres;
    std::vector<real> odata;
    std::vector<real> odata2;
    std::vector<real> otemp;
    std::vector<real> othet;
    std::vector<real> odens;
    std::vector<real> oz;
    std::vector<real> ozkm;
    int i, j, k, idx;
    int n1, n2, n3;
    real val, val2;
    real z;
    int ival, ival2;
    double tyme;
    string str;
    int ia[3], ja[3], ka[3];
    real da[3];
    GridLatLonField3D const *gridy;
    GridField3D::iterator looper;
    GridField3D::const_iterator clooper;
    TropOTF trop;
    real trop_z, trop_p, trop_theta, trop_dens;
    
    olons.reserve(72);
    for (i=0; i<72; i++ ) {
       olons.push_back(i*5.0);
    }   
    olats.reserve(37);
    for (i=0; i<37; i++ ) {
       olats.push_back( i*5.0-90.0 );
    }   
    opres.reserve(18);
    opres.push_back( 1000.0);
    opres.push_back(  850.0);
    opres.push_back(  700.0);
    opres.push_back(  500.0);
    opres.push_back(  400.0);
    opres.push_back(  300.0);
    opres.push_back(  250.0);
    opres.push_back(  200.0);
    opres.push_back(  150.0);
    opres.push_back(  100.0);
    opres.push_back(   70.0);
    opres.push_back(   50.0);
    opres.push_back(   30.0);
    opres.push_back(   10.0);
    opres.push_back(    5.0);
    opres.push_back(    2.0);
    opres.push_back(    1.0);
    opres.push_back(    0.4);

    otemp.reserve(18);
    oz.reserve(18);
    ozkm.reserve(18);
    othet.reserve(18);
    odens.reserve(18);
    for ( k=0; k<18; k++ ) {
        // fake temperature profile
        z = 7.0*LOG(1000.0/opres[k]);
        val = 210.0 + 70.*EXP(- POW(z/7.0, 2.0))+60.0*EXP(- POW((z-50.0)/20.0, 2.0));
        otemp.push_back(val);
        oz.push_back(z*1000.0);
        ozkm.push_back(z);
        othet.push_back( val * POW( 1000.0/opres[k], 2./7.) );
        odens.push_back( opres[k]*100.0/287.04/val );
    }

    odata.reserve(72*37*18);
    odata2.reserve(72*37*18);
    for ( k=0; k<18; k++ ) {
    for (i=0; i<72*37; i++ ) {
       odata.push_back( otemp[k] );
       odata2.push_back( ozkm[k] );
    }   
    }


    // =========================  set up the temperature grid on pressure surfaces
    t_grid.set_quantity("air_temperature");
    t_grid.set_units("Kelvin");
    t_grid.set_fillval(-1234.0);
    t_grid.set_time( 3.14159, "Grue 34, year of the kumquat");
    
    
    
    
    // test single profile
    val = trop.wmo( otemp, oz );
    val2 = 11613.7;
    if ( mismatch( val, val2, val2*0.01 ) ) {
       cerr << "single profile TropOTF mismatch: " << val << " vs " << val2 << endl;
       exit(1);
    }
    
    // test grid - alts
    t_grid.set_vertical("altitude");
    t_grid.set_vunits("km");
    t_grid.load( olons, olats, ozkm, odata );
    t_grid.mksVScale = 1000.0; // (km->m)
    tmp2 = dynamic_cast<GridLatLonFieldSfc*>(trop.wmo( t_grid ));
    t_sfc = *tmp2;
    delete tmp2;
    trop_z =  t_sfc(34,23); 
    if ( mismatch( trop_z, val2/1000.0, val2/1000.0*0.01 ) ) {
       cerr << "z-grid TropOTF mismatch: " << trop_z << " vs " << val2/1000.0 << endl;
       exit(1);
    }
    
    // test grid - pressure
    t_grid.set_vertical("air_pressure");
    t_grid.set_vunits("millibar");
    t_grid.load( olons, olats, opres, odata );
    t_grid.mksVScale = 100.0; // (mb->Pa)
    tmp2 = dynamic_cast<GridLatLonFieldSfc*>(trop.wmo( t_grid ));
    t_sfc = *tmp2;
    delete tmp2;
    trop_p =  t_sfc(34,23);
    val2 = 1000.0*EXP(-trop_z/7.0);
    if ( mismatch( trop_p, val2, val2*0.01 ) ) {
       cerr << "p-grid TropOTF mismatch: " << val << " vs " << val2 << std::endl;
       exit(1);
    }
    
    // test grid - theta
    t_grid.set_vertical("air_potential_temperature");
    t_grid.set_vunits("Kelvin");
    t_grid.load( olons, olats, othet, odata );
    t_grid.mksVScale = 1.0;
    tmp2 = dynamic_cast<GridLatLonFieldSfc*>(trop.wmo( t_grid ));
    t_sfc = *tmp2;
    delete tmp2;
    trop_theta =  t_sfc(34,23);
    val2 = 346.408;
    if ( mismatch( trop_theta, val2, val2*0.01 ) ) {
       cerr << "theta-grid TropOTF mismatch: " << trop_theta << " vs " << val2 << std::endl;
       exit(1);
    }
    
    // test grid - density
    t_grid.set_vertical("air_density");
    t_grid.set_vunits("kg/m^3");
    t_grid.load( olons, olats, odens, odata );
    t_grid.mksVScale = 1.0;
    tmp2 = dynamic_cast<GridLatLonFieldSfc*>(trop.wmo( t_grid ));
    t_sfc = *tmp2;
    delete tmp2;
    trop_dens =  t_sfc(34,23);
    val2 = trop_p*100.0/287.04/trop_theta*POW( trop_p/1000.0, -2./7.) ;
    if ( mismatch( trop_dens, val2, val2*0.01 ) ) {
       cerr << "rho-grid TropOTF mismatch: " << trop_dens << " vs " << val2 << std::endl;
       exit(1);
    }

    // =========================  set up the altitude grid on pressure surfaces
    a_grid.set_quantity("altitude");
    a_grid.set_units("km");
    a_grid.mksScale = 1000.0;
    a_grid.set_fillval(-999.0);
    a_grid.set_time( 3.14159, "Grue 34, year of the kumquat");
    a_grid.set_vertical("air_pressure");
    a_grid.set_vunits("millibar");
    a_grid.load( olons, olats, opres, odata2 );
    a_grid.mksVScale = 100.0; // (mb->Pa)
    // test grid - pressure
    t_grid.set_vertical("air_pressure");
    t_grid.set_vunits("millibar");
    t_grid.load( olons, olats, opres, odata );
    t_grid.mksVScale = 100.0; // (mb->Pa)
    tmp2 = dynamic_cast<GridLatLonFieldSfc*>(trop.wmo( t_grid, a_grid ));
    t_sfc = *tmp2;
    delete tmp2;
    val =  t_sfc(34,23);
    if ( mismatch( val, trop_z, trop_z*0.01 ) ) {
       cerr << "p-grid/a-grid TropOTF mismatch: " << val << " vs " << trop_z << std::endl;
       exit(1);
    }
    
    exit(0);

}
