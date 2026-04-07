#ifndef GIGATRAJ_PALTOTF
#define GIGATRAJ_PALTOTF

#include <string>
#include <vector>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/MetOnTheFly.hh"
#include "gigatraj/GridField3D.hh"
#include "gigatraj/GridFieldSfc.hh"


namespace gigatraj {

/*!
\ingroup OnTheFly

\brief class for computing pressure altitude from other meteorological data fields

\details

The PAltOTF class provides GridField3D and GridFieldSfc objects full of
pressure altitude values. Pressure altitude is an estimated altitude
that is a function of pressure only. Based on the U. S. Standard
Atmosphere (1962) profile, it is commonly used for aircraft altitudes.

Sources of gridded meteorological fields seldom provide pressure altitude
as one of their products, 
since their products are usually given on isobaric surfaces.
Nevertheless, pressure altitude can be a very useful 
quantity in its own right, especially when products have
been interpolated to new vertical coordinate systems.  
Since pressure altitude is easily calculated
from pressure, the PAltOTF class
ensures that pressure altitude is always available from a given meteorological
source.

It is intended that this class be used by implementers of new
subclasses of MetGridData, to satisfy users' requests pressure altitude data
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

Users should obtain their pressure altitude fields by requesting them
from a MetGridData subclass such as MetMERRA, rather than by
reading other fields and giving them to PAltOTF.


*/

class PAltOTF : public MetOnTheFly {
  
     public:

         /// basic constructor 
         /*!
            This is the basic constructor for a new PAltOTF object.
         */
         PAltOTF();

         /// constructor that initializes input quantity names
         /*! This version of the constructor that initializes the 
            names of the quantities that are used to derive pressure altitude.
            These names are used to identify the input gridded fields
            in the calc() methods.

            \param palt the name of the pressure altitude quantity
            \param press the name of the pressure quantity
         */
         PAltOTF(const std::string& palt, const std::string& press);

         /// destructor
         /*! This is the class destructor
         */
         ~PAltOTF();
      
         /// copy constructor
         /*!
            This is the copy contructor method for the PAltOTF class.
            
            \param src the source PAltOTF object to copy from
         */
         PAltOTF(const PAltOTF& src);

         /// copy assignment
         /*! 
             This is the copy assignment operator for the PAltOTF class.
         */
         PAltOTF& operator=(const PAltOTF& src);

         /// copies settings from a source object to this one
         /*! 
              This method copies settings from a source PAltOTF object
              to this one.
              
              \param src the source PAltOTF object
         */     
         void assign( const PAltOTF& src);


         /// sets the name of the input quantity used for air pressure
         /*! This function sets the name used to identify an input field as 
             the air pressure quantity, according to local 
             naming conventions.  
             The default is to use the UCAR CF conventions.

            \param quantity the name to be used
         */
         inline void setPressureName(const std::string quantity ) {
            press_name = quantity;
         }            


         /// does the calculation to produce a pressure altitude field from a gridded 3D field
         /*! This function computes pressure altitude from a single input field.
             This covers two cases: any quantity on pressure surfaces (i.e., pressure
             is the vertical coordinate), or pressure on any surface.

            \return a pointer to a new GridField3D object that holds the output pressure altitude field.  
                    The units are km.  
                    The calling routine is responsible for
                    deleting the new object when it is no longer needed.
            \param input the input data field.
            \param flags OTF_* flags to to affect the calculation results
         */
         GridField3D* calc( const GridField3D& input, int flags=0) const;


         /// does the calculation to produce a pressure altitude field from a gridded 2D field
         /*! This function computes pressure altitude from a single input 2D pressure field.

            \return a pointer to a new GridField3D object that holds the output pressure altitude field.
                    The units are km.  
                    The calling routine is responsible for
                    deleting the new object when it is no longer needed.
            \param input the input data field.
            \param flags OTF_* flags to to affect the calculation results
         */
         GridFieldSfc* calc( const GridFieldSfc& input, int flags=0) const;


         /// does the calculation to produce an air pressure field from a gridded 3D field of pressure altitude
         /*! This function computes air pressure from a single input 3D field of pressure altitudes.
             This covers two cases: any quantity on pressure altitude surfaces (i.e., pressure altitude
             is the vertical coordinate), or pressure altitude on any surface.

            \return a pointer to a new GridField3D object that holds the output pressure field. 
                    The input will be in hPa. 
                    The calling routine is responsible for
                    deleting the new object when it is no longer needed.
            \param input the input data field.
            \param flags OTF_* flags to to affect the calculation results
         */
         GridField3D* clac( const GridField3D& input, int flags=0) const;


         /// does the calculation to produce an air pressure field from a gridded 2D field of pressure altitude
         /*! This function computes air pressure from a single input 2D pressure field of pressure altitude.

            \return a pointer to a new GridField3D object that holds the output pressure field.  
                    The input will be in hPa. 
                    The calling routine is responsible for
                    deleting the new object when it is no longer needed.
            \param input the input data field.
            \param flags OTF_* flags to to affect the calculation results
         */
         GridFieldSfc* clac( const GridFieldSfc& input, int flags=0) const;
         
         /// calculates pressure altitude from a pressure value
         /*! This method does the work of converting a pressure
             value to a pressure altitude value. it works by interpolating
             a standard atmospheric profile in log-pressure.
             This is the inverse of the clac() method.
             
             \param p the input pressure value, in Pascals (not hPa, and not mb)
             \return the pressure altitude value, in km.
         */    
         real calc( real p ) const;

         /// calculates pressure from a pressure altitude value
         /*! This method does the work of converting a pressure altitude
             value to a pressure value. it works by interpolating
             a standard atmospheric profile in log-pressure.
             This is the inverse of the calc() method.
             
             \param z the input pressure altitude value, in km
             \return the pressure altitude value, in Pa.
         */    
         real clac( real z ) const;

     protected:
         /// name of the pressure quantity for the data source being used
         std::string press_name;
         
         /// length of standard profi;e
         static const int stdN;
         /// set of standard altitudes (in km)
         static const real *stdZ;
         /// set of natural log of standard pressures (in Pa, not hPa!)
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
            
