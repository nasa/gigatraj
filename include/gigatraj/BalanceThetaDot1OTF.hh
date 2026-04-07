#ifndef GIGATRAJ_BALANCETHETADOTOTF1
#define GIGATRAJ_BALANCETHETADOTOTF1

#include <string>
#include <vector>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/MetOnTheFly.hh"
#include "gigatraj/GridField3D.hh"
#include "gigatraj/GridFieldSfc.hh"
namespace gigatraj {

/*!

\ingroup OnTheFly

\brief class for adjusting the total time derivative of potential temperature  
(aka "potential temperature tendency", aka "theta dot") to force the net mass 
flux across each potential temperature surfaces to be zero.

\details

The BalanceThetaDot1OTF class provides a means of ensuring that the time rate of
change of air potential temperature is such that  global mass flux through a a set
of potential temperature ("theta") surfaces is zero.

Meteorological sources rarely if ever provide the time rate of change of air
potential temperature ("theta-dot") as such, but they often do provide other fields
from which this field can be calculated.  However, the resulting field may suffer
from biases.  One way to reduce the bias is to adjust the field on theta surfaces so
that no net global mass flux across those surfaces.  Strictly speaking, the net mass
flux across any given theta surface might not be zero at any given instant in time,
but  its time average should be zero.  And the error introduced by this
assumption seems to be less than the error incurred by using the unadjusted
theta-dot fields.

There are several ways this calculation might be done.  This version 
("1") is based on the technique used by Dr. Mark Schoeberl in his
"ftraj" trajectory model.  The input data objects used by this class
must be on isentropic aurfaces.  Theta-dot is multiplied by the air density
and a grid-point area weighting factor.  These values are summed over
the globe.  This total is then divided by the global total of the
area-weighted density to obtain an offset theta-dot value that is subtracted
from the theta-dot field at every grid point on that theta surface.
This procedure is applied only to theta surfaces at or below 500 Kelvin;
other theta surfaces are left unchanged.

Note that the calc() methods of this class take fields of the
GridField3D virtual class as input parameters and produce a new
GridField3D object as output.  Objects of any single subclass 
of GridField3D
may be used as inputs, so long as they are grid-compatible with each other,
and the return value will be of the same
subclass of GridField3D as the inputs.  However, the calling routine will
need to do a dynamic cast from GridField3D to the particular
subclass being used.

*/

class BalanceThetaDot1OTF : public MetOnTheFly {
  
     public:

         /// basic constructor
         /*!
             This is the basic constructor for a new BalanceThetaDot1OTF object.
         */    
         BalanceThetaDot1OTF();

         /// constructor that initializes input quantity names
         /*! This version of the constructor initializes the 
            names of the quantities that are used to derive potential temperature.
            These names are used to indentify the input gridded fields
            in the calc() methods.

            \param dthdt the name of the time rate of change of air potential vorticity
            \param dens the name of the air density quantity
            \param temp the name of the air temperature quantity
            \param press the name of the air pressure quantity
            \param theta the name of the air potential temperature quantity

         */
         BalanceThetaDot1OTF(const std::string& dthdt, const std::string& dens, const std::string& temp, const std::string& press, const std::string& theta);

         /// destructor
         /*! This is the class destructor
         */
         ~BalanceThetaDot1OTF();
      
         /// copy constructor
         /*!
            This is the copy contructor method for the BalanceThetaDot1OTF class.
            
            \param src the source BalanceThetaDot1OTF object to copy from
         */
         BalanceThetaDot1OTF(const BalanceThetaDot1OTF& src);

         /// copy assignment
         /*! 
             This is the copy assignment operator for the BalanceThetaDot1OTF class.
         */
         BalanceThetaDot1OTF& operator=(const BalanceThetaDot1OTF& src);

         /// copies settings from a source object to this one
         /*! 
              This method copies settings from a source BalanceThetaDot1OTF object
              to this one.
              
              \param src the source BalanceThetaDot1OTF object
         */     
         void assign( const BalanceThetaDot1OTF& src);


         /// sets the name of the input quantity used for air density
         /*! This function sets the name used to identify an input field as 
             the air density quantity, according to local 
             naming conventions.  
             The default is to use the UCAR CF conventions.

            \param quantity the name to be used
         */
         inline void setDensityName(const std::string quantity ) {
            dens_name = quantity;
         }            

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

         /// sets the name of the input quantity used for air pressure
         /*! This function sets the name used to identify an input field as 
             the air pressure quantity, according to local 
             naming conventions.  The default is to use the UCAR CF conventions.

            \param quantity the name to be used
         */
         inline void setPressureName(const std::string quantity ) {
            press_name = quantity;
         }            

         /// Does the calculation to produce a mass-balanced theta-dot field
         /*! This function mass-balances the time tendency of potential
             temperature ("theta-dot") using two input fields.
             The input fields may be of any of several physical quantities
             that can go into calculating the mass-balanced theta-dot field.
             The two input quantities must be grid-compatible with each other.
             
            \return a pointer to a new GridField3D object that holds a balanced theta-dot field having the same units as the input theta-dot field.
                    The calling routine is responsible for deleting the new object 
                    once it is no longer needed. 
            \param thetadot the input data field of theta-dot (time rate of change of potential temperature), on theta surfaces.
            \param input2 the second input data field: either density, temperature, or pressure on theta surfaces.
            \param flags OTF_* flags to to affect the calculation results
         */
         GridField3D* calc( const GridField3D& thetadot, const GridField3D& input2, int flags=0) const;

     protected:
         /// the name of the total time derivative of temperature
         std::string dtdt_name;
         /// the name of the temperature quantity for the data source being used
         std::string temp_name;
         /// the name of the potential temperature quantity
         std::string theta_name;
         /// the name of the pressure quantity
         std::string press_name;
         /// the name of the total time derivative of pressure quantity
         std::string dens_name;

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
            
