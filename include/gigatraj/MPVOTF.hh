#ifndef GIGATRAJ_MPVOTF_H
#define GIGATRAJ_MPVOTF_H

#include <string>
#include <vector>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/MetOnTheFly.hh"
#include "gigatraj/GridField3D.hh"
#include "gigatraj/GridFieldSfc.hh"


namespace gigatraj {

/*!
\ingroup OnTheFly

\brief class for calculating "modified" potential vorticity ("MPV") data fields from other meteorological data fields

\details

The MPVOTF class provides GridField3D and GridFieldSfc objects full of
"modified potential vorticity" values.  (See Lait, 1994: An alternative form
for potential vorticity.  \a J. \a Atmos. \a Sci., \b 51, 1754-1759.)

MPV is not a standard product from sources of 
gridded meteorological fields, but it can be a very useful 
quantity.  Since it is easily calculated from other quantities
that are commonly provided by meteorological data sources, the
MPVOTF class ensures that MPV is always available from a given meteorological
source.

It is intended that this class be used by implementers of new
subclasses of MetGridData, to satisfy users' requests MPV data
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

Users should obtain their MPV fields by requesting them
from a MetGridData subclass such as MetMERRA, rather than by
reading other fields and giving them to MPVOTF.



*/

class MPVOTF : public MetOnTheFly {
  
     public:

         /// basic constructor 
         /*!
            This is the basic constructor for a new MPVOTF object.
         */
         MPVOTF();
         
         /// constructor that initializes input quantity names
         /*! This version of the constructor that initializes the 
            names of the quantities that are used to derive MPV.
            These names are used to identify the input gridded fields
            in the calc() methods.
            
             \param epv the name of the Ertel's potential vorticity quantity
             \param theta the name of the potential temperature quantity
         */
         MPVOTF( const std::string& epv, const std::string& theta ); 
            
         /// destructor
         /*! This is the class destructor
         */
         ~MPVOTF();
      
         /// copy constructor
         /*!
            This is the copy contructor method for the MPVOTF class.
            
            \param src the source MPVOTF object to copy from
         */
         MPVOTF(const MPVOTF& src);

         /// copy assignment
         /*! 
             This is the copy assignment operator for the MPVOTF class.
         */
         MPVOTF& operator=(const MPVOTF& src);

         /// copies settings from a source object to this one
         /*! 
              This method copies settings from a source MPVOTF object
              to this one.
              
              \param src the source MPVOTF object
         */     
         void assign( const MPVOTF& src);

         /// sets the name of the input quantity used for EPV
         /*! This function sets the name used to identify an input field as 
             Ertel's potential vorticity, according to local 
             naming conventions.  
             The default is to use the UCAR CF conventions.

            \param quantity the name to be used
         */
         inline void setEPVName(const std::string quantity ) {
            ename = quantity;
         }            

         /// sets the name of the input quantity used for air potential temperature
         /*! This function sets the name used to identify an input field as
             the air potential temperature quantity, according to local 
             naming conventions.  
             The default is to use the UCAR CF conventions.

            \param quantity the name to be used
         */
         inline void setPotentialTemperatureName(const std::string quantity ) {
            hname = quantity;
         }            


         /// Calculates MPV from EPV and Theta
         /*! This function calculates the MPV field
             from fields of Ertel's potential vorticity (EPV) and potential temperature (theta).
            
            \return a pointer to a new GridField3D object that holds the output MPV field.  
                    Its values will have the same units as the input EPV field.
                    The calling routine is responsible for deleting the new object 
                    once it is no longer needed. 
            \param epv a GridData3D field of Ertel's potential vorticity
            \param theta a GridField3D field of potential temperatures
            \param flags OTF_* flags to to affect the calculation results
         */   
         GridField3D* calc( const GridField3D& epv, const GridField3D& theta, int flags=0) const;


         /// Calculates MPV from EPV on isentropic surfaces
         /*! This function calculates the MPV field
             from a field of Ertel's potential vorticity on isentropic surfaces
             
            \return a pointer to a new GridField3D object that holds the output MPV field.  
                    Its values will have the same units as the input EPV field. 
                    The calling routine is responsible for deleting the new object 
                    once it is no longer needed. 
            \param epv a GridField3D field of isentropic Ertel's potential vorticity
            \param flags OTF_* flags to to affect the calculation results

         */   
         GridField3D* calc( const GridField3D& epv, int flags=0 ) const;


         /// Calculates MPV from EPV and Theta
         /*! This function calculates the MPV field
             from fields of Ertel's potential vorticity and potential temperature
             on quasi-horizontal 2D surfaces.  
             Note that the EPV and theta fields must be on the same surface.
             
            \return a pointer to a new GridFieldSfc object that holds the output MPV field.  
                    Its values will have the same units as the input EPV field. 
                    The calling routine is responsible for deleting the new object 
                    once it is no longer needed. 
            \param epv a GridFieldSfc field of Ertel's potential vorticity
            \param theta a GridFieldSfc field of potential temperatures 
            \param flags OTF_* flags to to affect the calculation results
         */           
         GridFieldSfc* calc( const GridFieldSfc& epv, const GridFieldSfc& theta, int flags=0) const;

     protected:
        /// the name of the EPV quantity
        std::string ename;
        /// the name of the potential temperature quantity
        std::string hname;
        

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
