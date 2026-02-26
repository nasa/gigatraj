#ifndef GIGATRAJ_TROPOTF_H
#define GIGATRAJ_TROPOTF_H

#include <string>
#include <vector>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/MetOnTheFly.hh"
#include "gigatraj/GridField3D.hh"
#include "gigatraj/GridFieldSfc.hh"


namespace gigatraj {

/*!
\ingroup OnTheFly

\brief class for calculating tropopause locations from other meteorological data fields


\details

The TropOTF class provides GridFieldSfc objects which contain the vertical 
location of the tropopause as a 2D quasihorizontal field. 

Sources of gridded meteorological fields often provide the tropopause
air pressure as one of their products, but different sources sometimes use
different definitions of the tropopause, and sometimes one needs the tropopause
in terms of some other vertical coordinate.
The TropOTF class
ensures that tropopause pressures are always computed consistently 
from a given meteorological
source.

It is intended that this class be used by implementers of new
subclasses of MetGridData, to satisfy users' requests tropopause data
when the original data source does not provide such data
i the form desired.  But it can
also profitably be used by end-users who wish to compare the results of
different data sources using consistent tropopause definitions.

The tropopause here is calculated in terms of altitude.
The temperature data is usually given on surfaces of either 
log-pressure altitude, pressure, pressure altitude,
or potential temperature. Whichever is used as the temperature vertical
coordinate, it is converted to altitude first. then the tropopause 
altitude is calculated, and then it is converted to the temperature
vertical coordinate. In many cases, though, we want the tropopause to remain
as altitude--for example, if we are filtering Parcels based on how far they are from the 
tropopause, then it makes little sense to convert the calculated tropopause to 
the parcels' vertical coordinate, only to convert it back to altitude again.
So this class has a flag that can be set to return avoid the final conversion, so
that the returned values are all altitudes in km.


Note that the calc() methods of this class take fields of the
GridField3D virtual classes as input parameters and produce a new
GridFieldSfc object as output.  Objects of any single subclass 
of GridField3D
may be used as inputs, so long as they are grid-compatible with each other,
and the return value will be of the 
subclass of GridFieldSfc that corresponds to the inputs
(e.g., GridLatLonFieldSfc for GridLatLonField3D).  
However, the calling routine will
need to do a dynamic cast from GridfieldSfc to the particular
subclass being used.


*/

class TropOTF : public MetOnTheFly {
  
     public:
     
         /// basic constructor 
         /*!
            This is the basic constructor for a new ThetaOTF object.
         */
         TropOTF();
         
         /// constructor that initializes input quantity names
         /*! This version of the constructor that initializes the 
            names of the quantities that are used to derive tropopause pressure.
            These names are used to identify the input gridded fields
            in the various calculation methods.

             \param trop the name of the tropopause
             \param temperature the name of the temperature quantity
             \param pressure the name of the pressure quantity
             \param altitude the name of the altitude quantity
             \param theta the name of the potential temperature quantity
             \param density the name of the density quantity
             \param tkind the kind of quantity the tropopause is: 0=native coordinates, 1=pressure altitude
         */
         TropOTF(std::string trop, std::string temperature, std::string pressure, std::string altitude, std::string theta, std::string density, int tkind=0 );
         
         /// destructor
         /*! This is the class destructor
         */
         ~TropOTF();
         
         /// sets the name of the temperature quantity, according to local 
         /*! This function sets the name of the temperature quantity, according to local 
                 naming conventions.  The default is to use the UCAR CF conventions.
            
            \param quantity the name to be used
         */
         inline void setTemperatureName(const std::string quantity ) {
            tname = quantity;
         }            

         /// sets the name of the pressure quantity
         /*! This function sets the name of the pressure quantity, according to local 
                 naming conventions.  The default is to use the UCAR CF conventions.
            
            \param quantity the name to be used
         */
         inline void setPressureName(const std::string quantity ) {
            pname = quantity;
         }            

         /// sets the name of the altitude quantity
         /*! This function sets the name of the altitude quantity, according to local 
                 naming conventions.  The default is to use the UCAR CF conventions.
            
            \param quantity the name to be used
         */
         inline void setAltitudeName(const std::string quantity ) {
            aname = quantity;
         }            

         /// sets the name of the potential temperature quantity, according to local 
         /*! This function sets the name of the potential temperature quantity, according to local 
                 naming conventions.  The default is to use the UCAR CF conventions.
            
            \param quantity the name to be used
         */
         inline void setPotentialTemperatureName(const std::string quantity ) {
            hname = quantity;
         }            

         /// sets the name of the air density quantity
         /*! This function sets the name of the air density quantity, according to local 
                 naming conventions.  The default is to use the UCAR CF conventions.
            
            \param quantity the name to be used
         */
         inline void setDensityName(const std::string quantity ) {
            dname = quantity;
         }            

         /// sets the name of the tropopause
         /*! This function sets the name of the tropopause, according to local 
                 naming conventions.  The default is "tropopause".
                 Note that this is the name of the tropopause itself,
                 not the default name of the physical quantity calculated
                 by this class.  (That quantity is set using
                 the \b quantity() method.) 

            \param quantity the name to be used
         */
         inline void setTropName(const std::string quantity ) {
            sfc = quantity;
         }            



        /// finds the WMO tropopause from vector data
        /*! This function calculates the tropopause pressure
            using the WMO definition of the tropopause.
            
            \return the value of the tropopause altitude, in meters.
            \param t a vector of temperatures, in Kelvin or Celsius
            \param alt a vector of altitudes, in meters
            \param flags OTF_* flags to to affect the calculation results
        */
        real wmo( const std::vector<real>&t, const std::vector<real>&alt, int flags=0) const;

        /// finds the WMO tropopause from a field of temperature 
         /*! This function calculates the WMO tropopause from temperatures on 
             surfaces of either altitude, pressure, potential temperature, or density

            \return a pointer to a new GridFieldSfc object containing the  tropopause location,
            in terms of the vertical coordinates of the input \p t grid, and
            in the same units. The calling routine is responsible for deleting the new
            object once it is no longer needed.

            \param t a GridField3D field of air temperatures on altitudes
            \param flags OTF_* flags to to affect the calculation results
         */   
         GridFieldSfc* wmo( const GridField3D& t, int flags=0 ) const;

        /// finds the WMO tropopause from a fields of temperature and an altitude coordinate 
         /*! This function calculates the WMO tropopause from 3D fields of temperature and 
             an altitude coordinate (geometric altitude, log-pressure altitude, pressure altitude, geopotential height, etc.).
         
            \return a pointer to a new GridFieldSfc object containing the tropopause location,
            in terms of the physical quantity of the input \p alt grid, and
            in the same units. The calling routine is responsible for deleting the new
            object once it is no longer needed.


            \param t a GridField3D field of air temperatures
            \param alt a GridField3D field of an altitude quantity.
            \param flags OTF_* flags to to affect the calculation results
         */   
         GridFieldSfc* wmo( const GridField3D& t, const GridField3D& alt, int flags=0) const;



     protected:
     
        /// the name of the tropopause
        std::string sfc;
        /// the coordinate type of the tropopause: 0=pressure, 1=altitude
        int tropkind;
        /// the name of the pressure quantity
        std::string pname;
        /// the name of the temperature quantity
        std::string tname;
        /// the name of the altitude quantity
        std::string aname;
        /// the name of the potential temperature quantity
        std::string hname;
        /// the name of the density quantity
        std::string dname;
        
        /*! linear vertical profile interpolator.
            returns the interpolate dvalue (0 if extrapolation is attempted)
            \param xi the new independent variavble value at which we want interpolated results
            \param x the vector of independent variable values
            \param y the vector of dependent variable values
        */
        real intrp( real xi, std::vector<real>&x, std::vector<real>&y ) const;

};
}

#endif



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
