#ifndef GIGATRAJ_PALTDOTOTF
#define GIGATRAJ_PALTDOTOTF

#include <string>
#include <vector>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/MetOnTheFly.hh"
#include "gigatraj/GridField3D.hh"
#include "gigatraj/GridFieldSfc.hh"
#include "gigatraj/PAltOTF.hh"

namespace gigatraj {

/*!
\ingroup OnTheFly

\brief class for computing the total time derivative of pressure altitude from other meteorological data fields


\details

\details

The PAltDotOTF class provides GridField3D objects full of
the total time derivative of pressure altitude. This is not
exactly the same as the vertical wind, or "w", as those
that quantity is the total time derivative of geometric altitude.
But the two should be similar.

Sources of gridded meteorological fields seldom provide "w", 
much less the time rate of change of pressure altitude.
Nevertheless, this is a critical quantity to have when running
trajectories in pressure-altitude coordinates, since it is the rate of
change of the parcel's vertical coordinate.  
Since the pressure altitude tendency is easily calculated
from other quantities that are commonly provided (notably the
pressure tendency omega), the PAltDotOTF class
ensures that PAlt-Dot is always available from a given meteorological
source.

It is intended that this class be used by implementers of new
subclasses of MetGridData, to satisfy users' requests 
when the original data source does not provide the requested fields explicitly.
Note that the calc() methods of this class take fields of the
GridField3D virtual class as input parameters and produce a new
GridField3D object as output.  Objects of any single subclass of GridField3D
may be used as inputs, so long as they are grid-compatible with each other,
and the return value will be of the same
subclass of GridField3D as the inputs.  However, the calling routine will
need to do a dynamic cast from GridField3D to the particular
subclass being used.

Users should obtain their pressure altitude tendency fields by requesting them
from a MetGridData subclass such as MetMERRA, rather than by
reading other fields and giving them to PAltDotOTF.


*/

class PAltDotOTF : public MetOnTheFly {
  
     public:

         /// basic constructor 
         /*!
            This is the basic constructor for a new PAltDotOTF object.
         */
         PAltDotOTF();

         /// constructor that initializes input quantity names
         /*! This version of the constructor that initializes the 
            names of the quantities that are used to derive theta-dot.
            These names are used to identify the input gridded fields
            in the calc() methods.

            \param w the name of the pressure altitude tendency quantity
            \param omega the name of the air pressure tendency quantity
            \param press the name of the air pressure quantity
            \param palt the name of the pressure altitdue quantity
         */
         PAltDotOTF(const std::string& w, const std::string& omega, const std::string& press, const std::string& palt);

         /// destructor
         /*! This is the class destructor
         */
         ~PAltDotOTF();
      
         /// copy constructor
         /*!
            This is the copy contructor method for the PAltDotOTF class.
            
            \param src the source PAltDotOTF object to copy from
         */
         PAltDotOTF(const PAltDotOTF& src);

         /// copy assignment
         /*! 
             This is the copy assignment operator for the PAltDotOTF class.
         */
         PAltDotOTF& operator=(const PAltDotOTF& src);

         /// copies settings from a source object to this one
         /*! 
              This method copies settings from a source PAltDotOTF object
              to this one.
              
              \param src the source PAltDotOTF object
         */     
         void assign( const PAltDotOTF& src);


         /// sets the name of the pressure altitude tendency quantity
         /*! This function sets the name of the local rate of change of pressure altitude quantity,
             also known as the temeprature tendency, according to local 
             naming conventions.  The default is to use the UCAR CF conventions.
            
            \param quantity the name to be used
         */
         inline void setPAltDotName(const std::string quantity ) {
            quant = quantity;
         }            


         /// sets the name of the pressure tendency quantity
         /*! This function sets the name of the local rate of  change of pressure quantity, 
             also known as the pressure tendency (and often designtated as "omega"),
             according to local 
             naming conventions.  The default is to use the UCAR CF conventions.
       
            \param quantity the name to be used
         */
         inline void setPressureDotName(const std::string quantity ) {
            omega_name = quantity;
         }            


         /// sets the name of the pressure altitude quantity
         /*! This function sets the name of the pressure altitude quantity,
             according to local 
             naming conventions.  The default is to use the UCAR CF conventions.
       
            \param quantity the name to be used
         */
         inline void setPressureAltitudeName(const std::string quantity ) {
            palt_name = quantity;
         }            

         /// sets the name of the pressure quantity
         /*! This function sets the name of the pressure quantity, according to local 
             naming conventions.  The default is to use the UCAR CF conventions.
        
            \param quantity the name to be used
         */
         inline void setPressureName(const std::string quantity ) {
            press_name = quantity;
         }            

         /// does the calculation to produce pressure altitude tendency from a gridded 3D field
         /*! This function computes pressure altitude tendency from a single input 3D field.

            \return a pointer to a new GridField3D object that holds the output pressure 
                    altitude tendency field, in units of meters/second.  
                    The calling routine is responsible for deleting the new object 
                    once it is no longer needed. 
            \param input1 the pressure tendency field (i.e., omega).
            \param flags OTF_* flags to to affect the calculation results
         */
         GridField3D* calc( const GridField3D& input1, int flags=0 ) const;

         /// does the calculation to produce pressure altitude tendency from a gridded 3D field
         /*! This function computes pressure altitude tendency from two input 3D fields.

            \return a pointer to a new GridField3D object that holds the output pressure 
                    altitude tendency field, in units of meters/second.  
                    The calling routine is responsible for deleting the new object 
                    once it is no longer needed. 
            \param input1 the pressure tendency field (i.e., omega).
            \param input2 the pressure field or the pressure altitude field.
            \param flags OTF_* flags to to affect the calculation results
         */
         GridField3D* calc( const GridField3D& input1, const GridField3D& input2, int flags=0 ) const;

         /// does the calculation to produce pressure altitude tendency from a gridded 2D field
         /*! This function computes pressure altitude tendency from two input 2D fields.

            \return a pointer to a new GridFieldSfc object that holds the output pressure 
                    altitude tendency field, in units of meters/second.  
                    The calling routine is responsible for deleting the new object 
                    once it is no longer needed. 
            \param input1 the pressure tendency field (i.e., omega).
            \param input2 the pressure field or the pressure altitude field.
            \param flags OTF_* flags to to affect the calculation results
         */
         GridFieldSfc* calc( const GridFieldSfc& input1, const GridFieldSfc& input2, int flags=0 ) const;

         /// interpolate dz/dp to the given altitude
         /*! This function returns the derivative of pressure altitude with respect to pressure.
             It is used for converting pressure derivatives to altitude derivatives.
             
             \return the derivative, in km/Pa
             \param alt the altitude, in km.
         */    
         real get_dzdp( real alt ) const; 

     protected:

         /// name of the pressure altitude quantity
         std::string palt_name;
         /// name of the pressure quantity
         std::string press_name;
         /// name of the total time derivative of pressure quantity
         std::string omega_name;
         
         /// for converting between pressure and pressure altitude
         PAltOTF palt;
         
         /// length of standard profi;e
         static const int stdN;
         /// set of standard altitudes (in km)
         static const real *stdZ;
         /// set of natural log of standard pressures (in Pa, not hPa!)
         static const real *stdLogDzDp;
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
            
