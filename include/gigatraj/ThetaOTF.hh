#ifndef GIGATRAJ_THETAOTF_H
#define GIGATRAJ_THETAOTF_H

#include <string>
#include <vector>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/MetOnTheFly.hh"
#include "gigatraj/GridField3D.hh"
#include "gigatraj/GridFieldSfc.hh"


namespace gigatraj {

/*!
\ingroup OnTheFly

\brief class for calculating potential temperature from other meteorological data fields


\details

The ThetaOTF class provides GridField3D and GridFieldSfc objects full of
air potential temperature values.  

Sources of gridded meteorological fields provide air potential temperature
only occasionally as one of their products, since it is easily computed from
temperature and pressure.
The ThetaOTF class
ensures that potential temperature is always available from a given meteorological
source.

It is intended that this class be used by implementers of new
subclasses of MetGridData, to satisfy users' requests air potential temperature data
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

Users should obtain their air potential temperature fields by requesting them
from a MetGridData subclass such as MetMERRA, rather than by
reading other fields and giving them to ThetaOTF.


*/


class ThetaOTF : public MetOnTheFly {
  
     public:

         /// basic constructor 
         /*!
            This is the basic constructor for a new ThetaOTF object.
         */
         ThetaOTF();
         
         /// constructor that initializes input quantity names
         /*! This version of the constructor that initializes the 
            names of the quantities that are used to derive air potential temperature.
            These names are used to identify the input gridded fields
            in the calc() methods.

             \param theta the name of the potential temperature quantity
             \param temperature the name of the temperature quantity
             \param pressure the name of the pressure quantity
         */
         ThetaOTF( const std::string& theta, const std::string& temperature, const std::string& pressure); 
            
         /// destructor
         /*! This is the class destructor
         */
         ~ThetaOTF();

          /// copy constructor
          /*!
             This is the copy contructor method for the ThetaOTF class.
             
             \param src the source ThetaOTF object to copy from
          */
          ThetaOTF(const ThetaOTF& src);
     
          /// copy assignment
          /*! 
              This is the copy assignment operator for the ThetaOTF class.
          */
          ThetaOTF& operator=(const ThetaOTF& src);
     
          /// copies settings from a source object to this one
          /*! 
               This method copies settings from a source ThetaOTF object
               to this one.
               
               \param src the source ThetaOTF object
          */     
          void assign( const ThetaOTF& src);
     

         /// sets the name of the temperature quantity 
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

         /// calculates theta (potential temperature) from scalar values
         /*! This function calculates theta (potential temperature)
             from  temperature and pressure.
             Note: t MUST be in Kelvin, and p MUST be in kiloPascals (not millibars)!

            \return the value of potential temperature, in Kelvin
            \param t a single value of air temperatures
            \param p a single value of air pressures
         */   
         real calc( real t, real p) const;

         /// calculates the theta (potential temperature) field from two inputs
         /*! This function calculates the theta (potential temperature) field
             from fields of temperature and pressure.
             
            \return a pointer to a new GridField3D object that holds the output potential 
                    temperature field, in Kelvin. The calling routine is responsible for
                    deleting the object when it is no longer needed. 
            \param t a GridField3D field of air temperatures
            \param p a GridField3D field of air pressures
            \param flags OTF_* flags to to affect the calculation results
         */   
         GridField3D* calc( const GridField3D& t, const GridField3D& p, int flags=0) const;

         /// calculates the theta (potential temperature) field from a single input
         /*! This function calculates the theta (potential temperature) field
             from a field of temperature on .pressure surfaces.
             
            \return a pointer to a new GridField3D object that holds the output potential 
                    temperature field, in Kelvin. The calling routine is responsible for
                    deleting the object when it is no longer needed. 
            \param t a GridField3D field of air temperatures
            \param flags OTF_* flags to to affect the calculation results

         */   
         GridField3D* calc( const GridField3D& t, int flags=0 ) const;

         /// calculates the theta (potential temperature) field from two surface inputs
         /*! This function calculates the theta (potential temperature) field
             from fields of temperature and pressure on quasi-horizontal surfaces.  
             Note that
             the temperature and pressure fields must be on the same surface.
             
            \return a pointer to a new GridFieldSfc object that holds the output potential 
                    temperature field, in Kelvin. The calling routine is responsible for
                    deleting the object when it is no longer needed. 
            \param t a GridFieldSfc field of air temperatures
            \param p a GridFieldSfc field of air pressures
            \param flags OTF_* flags to to affect the calculation results
         */   
         GridFieldSfc* calc( const GridFieldSfc& t, const GridFieldSfc& p, int flags=0) const;


     protected:
     
        /// the name of the pressure quantity
        std::string pname;
        /// the name of the temperature quantity
        std::string tname;
        
        /*! calculates theta
          \param t temperature in Kelvin
          \param p pressure in Pascals
        */
        inline real dothet( real t, real p ) const
        {
           return t * POW( 100000.0/p, 2./7. );
        }
           

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
