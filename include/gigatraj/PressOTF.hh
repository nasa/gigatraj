#ifndef GIGATRAJ_PRESSOTF
#define GIGATRAJ_PRESSOTF

#include <string>
#include <vector>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/MetOnTheFly.hh"
#include "gigatraj/GridField3D.hh"
#include "gigatraj/GridFieldSfc.hh"


namespace gigatraj {

/*!
\ingroup OnTheFly

\brief class for computing pressure from other meteorological data fields

\details

The PressOTF class provides GridField3D and GridFieldSfc objects full of
air pressure values.  

Sources of gridded meteorological fields seldom provide air pressure
as one of their products, except on a few specific surfaces, 
since their products are usually given on isobaric surfaces.
Nevertheless, pressure can be a very useful 
quantity in its own right, especially when products have
been interpolated to new vertical coordinate systems.  
Since pressure is easily calculated
from other quantities that are commonly provided, the PressOTF class
ensures that pressure is always available from a given meteorological
source.

It is intended that this class be used by implementers of new
subclasses of MetGridData, to satisfy users' requests air pressure data
when the original data source does not provide such fields explicitly.
Note that the calc() methods of this class take fields of the
GridField3D or GridFieldSfc virtual classes as input parameters and produce a new
GridField3D or GridFieldSfc object as output.  Objects of any single subclass 
of GridField3D/GridFieldSfc
may be used as inputs, so long as they are grid-compatible with each other,
and the return value will be of the same
subclass of GridField3D/GridFieldSfc as the inputs.  However, the calling routine will
need to do a dynamic cast from GridField3D or GridfieldSfc to the particular
subclass being used.

Users should obtain their air pressure fields by requesting them
from a MetGridData subclass such as MetMERRA, rather than by
reading other fields and giving them to PressOTF.


*/

class PressOTF : public MetOnTheFly {
  
     public:

         /// basic constructor 
         /*!
            This is the basic constructor for a new PressOTF object.
         */
         PressOTF();

         /// constructor that initializes input quantity names
         /*! This version of the constructor that initializes the 
            names of the quantities that are used to derive air pressure.
            These names are used to identify the input gridded fields
            in the calc() methods.

            \param press the name of the pressure quantity
            \param temp the name of the temperature quantity
            \param theta the name of the potential temperature quantity
            \param thick the name of the pressure thickness quantity
            \param dens the name of the density quantity
            \param alt the name of the pressure altitude quantity
         */
         PressOTF(const std::string& press, const std::string& temp, const std::string& theta, const std::string& thick, const std::string& dens, const std::string& alt="");

         /// destructor
         /*! This is the class destructor
         */
         ~PressOTF();
      
         /// copy constructor
         /*!
            This is the copy contructor method for the PressOTF class.
            
            \param src the source PressOTF object to copy from
         */
         PressOTF(const PressOTF& src);

         /// copy assignment
         /*! 
             This is the copy assignment operator for the PressOTF class.
         */
         PressOTF& operator=(const PressOTF& src);

         /// copies settings from a source object to this one
         /*! 
              This method copies settings from a source PressOTF object
              to this one.
              
              \param src the source PressOTF object
         */     
         void assign( const PressOTF& src);

         /// sets the name of the input quantity used for air temperature
         /*! This function sets the name used to identify an input field as 
             the air temperature quantity, according to local 
             naming conventions.  
             The default is to use the UCAR CF conventions.

            \param quantity the name to be used
         */
         inline void setTemperatureName(const std::string quantity ) {
            temp_name = quantity;
         }            

         /// sets the name of the input quantity used for air potential temperature
         /*! This function sets the name used to identify an input field as
             the air potential temperature quantity, according to local 
             naming conventions.  
             The default is to use the UCAR CF conventions.

            \param quantity the name to be used
         */
         inline void setPotentialTemperatureName(const std::string quantity ) {
            theta_name = quantity;
         }            

         /// sets the name of the input quantity used for air density
         /*! This function sets the name used to identify an input field as 
             the air density quantity, according to local 
             naming conventions.  The default is to use the UCAR CF conventions.

            \param quantity the name to be used
         */
         inline void setDensityName(const std::string quantity ) {
            dens_name = quantity;
         }            

         /// sets the name of the input quantity used for pressure altitude
         /*! This function sets the name used to identify an input field as 
             the pressure altitude quantity, according to local 
             naming conventions.  

            \param quantity the name to be used
         */
         inline void setAltitudeName(const std::string quantity ) {
            alt_name = quantity;
         }            

         /// sets the name of the input quantity used for air pressure layer thickness
         /*! This function sets the name used to identify an input field as 
             the air pressure layer thickness quantity, according to local 
             naming conventions.  The default is to use the UCAR CF conventions.

            \param quantity the name to be used
         */
         inline void setPressureThicknessName(const std::string quantity ) {
            thick_name = quantity;
         }            

         /// does the calculation to produce an air pressure field from a gridded 3D field
         /*! This function computes air pressure from a single input 3D field.
             This covers three cases: 
               * any quantity on pressure surfaces (i.e., pressure is the vertical coordinate), 
               * temperature on isentropic surfaces (i.e., potential temperature is the vertical coordinate).
               * thicknesses on model levels, to be integrated into pressures on model levels. In this case, the start parameter must be set.
            \return a pointer to a new GridField3D object that holds the output pressure field.  
                    When the input is on pressure surfaces, the output values 
                    will have the same units as the vertical coordinate of the 
                    input field.  When the input is on isentropic surfaces,
                    the output will have units of hectoPascals (hPa), which is
                    the same as millibars (mb). 
                    If the input is an array of pressure altitude values, or 
                    has pressure altitude as a vertical coordinate, then the
                    output will be in hPa as well.
                    The calling routine is responsible for
                    deleting the new object when it is no longer needed.
            \param start if not -1.0, this is the starting base value to be used in the integration, in the same
                    units as the thicknesses
            \param input the input data field.
            \param flags OTF_* flags to to affect the calculation results
         */
         GridField3D* calc( const GridField3D& input, real start=-1.0, int flags=0) const;


         /// does the calculation to produce an air pressure field from two gridded 3D fields
         /*! This function computes air pressure from two input 3D fields.
             This covers two cases: temperature and potential temperature
             on arbitrary surfaces, and temperature and density on arbitrary
             surfaces.  Thus, the two input fields may be temperature and 
             theta, or temperature and density.  The order of the two input
             parameters is not important. 
            
             \return a pointer to a new GridField3D object that holds the output pressure field.  
                     Its values will have the units of hectoPascals (hPa), which
                     is the same as millibars (mb). The calling routine is responsible for
                    deleting the new object when it is no longer needed.        
             \param input1 the first input data field.
             \param input2 the second input data field.
             \param flags OTF_* flags to to affect the calculation results
         */
         GridField3D* calc( const GridField3D& input1, const GridField3D& input2, int flags=0 ) const;

         /// does the calculation to produce an air pressure field from two gridded 2D fields
         /*! This function computes air pressure from two input fields on 
             quasi-horizontal 2D surfaces.
             This covers two cases: temperature and potential temperature,
             and temperature and density.  Thus, the two input fields may be 
             temperature and  theta, or temperature and density.  The order 
             of the two input parameters is not important. 
            
             \return a pointer to a new GridFieldSfc object that holds the output pressure field.  
                     Its values will have the units of hectoPascals (hPa), which
                     is the same as millibars (mb). The calling routine is responsible for
                    deleting the new object when it is no longer needed.
             \param input1 the first input data field.
             \param input2 the second input data field.
             \param flags OTF_* flags to to affect the calculation results
         */
         GridFieldSfc* calc( const GridFieldSfc& input1, const GridFieldSfc& input2, int flags=0 ) const;

         /// calculates pressure from a pressure altitude value
         /** This method does the work of converting a pressure altitude
             value to a presure value. it works by interpolating
             a standard atmospheric profile in log-pressure.
             
             \param alt the input pressure altitude value, in meters (not km)
             \return the pressure value, in hPa.
         */    
         real calp( real alt ) const;

     protected:
         /// name of the temperature quantity for the data source being used
         std::string temp_name;
         /// name of the potential temperature quantity
         std::string theta_name;
         /// name of the pressure layer thickness quantity
         std::string thick_name;
         /// name of the density quantity
         std::string dens_name;
         /// name of the pressure altitude quantity
         std::string alt_name;

         
         /// length of standard profi;e
         static const int stdN;
         /// set of standard altitudes (in m, not km!)
         static const real *stdZ;
         /// set of standard pressures (in hPa)
         static const real *stdLogP;

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
            
