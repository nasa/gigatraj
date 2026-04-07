#ifndef GIGATRAJ_THETADOTOTF
#define GIGATRAJ_THETADOTOTF

#include <string>
#include <vector>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/MetOnTheFly.hh"
#include "gigatraj/GridField3D.hh"
#include "gigatraj/GridFieldSfc.hh"
namespace gigatraj {

/*!
\ingroup OnTheFly

\brief class for computing the total time derivative of potential temperature  (aka "potential temperature tendency", aka "theta dot") from other meteorological data fields


\details

The ThetaDotOTF class provides GridField3D objects full of
the total time derivative of potential temperature, also known as potential
temperature tendency.  Since the traditional symbol for potential temperature
is theta, this quantity would be expressed as d(theta)/dt.  

Sources of gridded meteorological fields seldom provide potential temperature 
tendency as one of their products.
Nevertheless, this is a critical quantity to have when running
trajectories in quasi-isentropic coordinates, since it is the rate of
change of the parcel's vertical coordinate.  
Since potential temperature tendency is easily calculated
from other quantities that are commonly provided, the ThetaDotOTF class
ensures that Theta-Dot is always available from a given meteorological
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

Users should obtain their potential temperature tendency fields by requesting them
from a MetGridData subclass such as MetMERRA, rather than by
reading other fields and giving them to ThetaDotOTF.


*/

class ThetaDotOTF : public MetOnTheFly {
  
     public:

         /// basic constructor 
         /*!
            This is the basic constructor for a new ThetaDotOTF object.
         */
         ThetaDotOTF();

         /// constructor that initializes input quantity names
         /*! This version of the constructor that initializes the 
            names of the quantities that are used to derive theta-dot.
            These names are used to identify the input gridded fields
            in the calc() methods.

            \param dthdt the name of the air potential temperature tendency quantity
            \param dtdt the name of the air temperature tendency quantity
            \param temp the name of the air temperature quantity
            \param press the name of the air pressure quantity
            \param theta the name of the air potential temperature quantity
            \param omega the name of the air pressure tendency quantity
         */
         ThetaDotOTF(const std::string& dthdt, const std::string& dtdt, const std::string& temp, const std::string& press, const std::string& theta, const std::string& omega);

         /// destructor
         /*! This is the class destructor
         */
         ~ThetaDotOTF();
      
        /// copy constructor
        /*!
           This is the copy contructor method for the ThetaDotOTF class.
           
           \param src the source ThetaDotOTF object to copy from
        */
        ThetaDotOTF(const ThetaDotOTF& src);

        /// copy assignment
        /*! 
            This is the copy assignment operator for the ThetaDotOTF class.
        */
        ThetaDotOTF& operator=(const ThetaDotOTF& src);

        /// copies settings from a source object to this one
        /*! 
             This method copies settings from a source ThetaDotOTF object
             to this one.
             
             \param src the source ThetaDotOTF object
        */     
        void assign( const ThetaDotOTF& src);


         /// sets the name of the temperature tendency quantity
         /*! This function sets the name of the local rate of change of temperature quantity,
             also known as the temperature tendency, according to local 
             naming conventions.  The default is to use the UCAR CF conventions.
            
            \param quantity the name to be used
         */
         inline void setTemperatureDotName(const std::string quantity ) {
            dtdt_name = quantity;
         }            

         /// sets the name of the temperature quantity 
         /*! This function sets the name of the temperature quantity, according to local 
             naming conventions.  The default is to use the UCAR CF conventions.
          
             \param quantity the name to be used
         */
         inline void setTemperatureName(const std::string quantity ) {
            temp_name = quantity;
         }            

         /// sets the name of the potential temperature quantity
         /*! This function sets the name of the potential temperature quantity, according to local 
             naming conventions.  The default is to use the UCAR CF conventions.
        
             \param quantity the name to be used
         */
         inline void setPotentialTemperatureName(const std::string quantity ) {
            theta_name = quantity;
         }            

         /// sets the name of the pressure quantity
         /*! This function sets the name of the pressure quantity, according to local 
             naming conventions.  The default is to use the UCAR CF conventions.
        
            \param quantity the name to be used
         */
         inline void setPressureName(const std::string quantity ) {
            press_name = quantity;
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


         /// does the calculation to produce potential temperature tendency from a gridded 3D field
         /*! This function computes air potential temperature tendency from a single input 3D field.
             This covers two cases: dtdt and temperature on pressure surfaces, and
             dtdt and theta on pressure surfaces.
             Note that this function uses an approximation that will hold for about
             99% of the time, but not for about 1%.

            \return a pointer to a new GridField3D object that holds the output potential 
                    temperature tendency field, in units of K/second.  
                    The calling routine is responsible for deleting the new object 
                    once it is no longer needed. 
            \param input1 the temperature tendency field.
            \param input2 can be either the temperature or theta field.
            \param flags OTF_* flags to to affect the calculation results
         */
         GridField3D* calc( const GridField3D& input1, const GridField3D& input2, int flags=0 ) const;

         /// does the calculation to produce potential temperature tendency from three input fields.
         /*! This function computes potential temperature tendency from three input fields.
             This covers three cases: temperature tendency, temperature, and pressure; 
             temperature tendency, potential temeprature, and pressure; 
             and temperature tendency, temperature, and potential temperature.
             These quantities may use any vertical coordinate.
             Note that this result uses an approximation that will hold for about
             99% of the time, but not for about 1%.

            \return a pointer to a new GridField3D object that holds the output potential 
                    temperature tendency field, in units of K/second.  
                    The calling routine is responsible for deleting the new object 
                    once it is no longer needed. 

            \param input1 is the temperature tendency field.
            \param input2 is the temperature or potential temperature field.
            \param input3 is the potential temperature or pressure field.
            \param flags OTF_* flags to to affect the calculation results
         */
         GridField3D* calc( const GridField3D& input1, const GridField3D& input2, const GridField3D& input3, int flags=0 ) const;

         /// does the calculation to produce potential temperature tendency from four input fields.
         /*! This function computes potential temperature tendency from four input fields.
             This covers four cases: 
             temperature tendency, temperature, pressure, and potential temperature; 
             temperature tendency, potential temperature, pressure, and pressure tendency; 
             temperature tendency, temperature, pressure, and pressure tendency;
             and temperature tendency, temperature, potential temperature, and pressure tendency.
             These quantities may use any vertical coordinate.
             Note that the result calculated without the pressure tendency field uses an 
             approximation that will hold for about 99% of the time, but not for about 1%.

            \return a pointer to a new GridField3D object that holds the output potential 
                    temperature tendency field, in units of K/second.  
                    The calling routine is responsible for deleting the new object 
                    once it is no longer needed. 

            \param input1 is the temperature tendency field.
            \param input2 is the temperature or potential temperature field.
            \param input3 is the potential temperature or pressure field.
            \param input4 is the pressure tendency field.
            \param flags OTF_* flags to to affect the calculation results
         */
         GridField3D* calc( const GridField3D& input1, const GridField3D& input2, const GridField3D& input3, const GridField3D& input4, int flags=0 ) const;

         /// does the calculation to produce potential temperature tendency from five input fields.
         /*! This function computes potential temperature tendency from five input fields.
             These quantities may use any vertical coordinate.

            \return a pointer to a new GridField3D object that holds the output potential 
                    temperature tendency field, in units of K/second.  
                    The calling routine is responsible for deleting the new object 
                    once it is no longer needed. 

            \param input1 is the temperature tendency field.
            \param input2 is the temperature field.
            \param input3 is the potential temperature field.
            \param input4 is the pressure field.
            \param input5 is the pressure tendency field.
            \param flags OTF_* flags to to affect the calculation results
         */
         GridField3D* calc( const GridField3D& input1, const GridField3D& input2, const GridField3D& input3, const GridField3D& input4 , const GridField3D& input5, int flags=0) const;



     protected:
         /// name of the total time derivative of temperature
         std::string dtdt_name;
         /// name of the temperature quantity for the data source being used
         std::string temp_name;
         /// name of the potential temperature quantity
         std::string theta_name;
         /// name of the pressure quantity
         std::string press_name;
         /// name of the total time derivative of pressure quantity
         std::string omega_name;

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
            
