#ifndef GIGATRAJ_METONTHEFLY_H
#define GIGATRAJ_METONTHEFLY_H

#include "gigatraj/gigatraj.hh"
#include "gigatraj/GridField.hh"

#include <string>
#include <vector>
#include <math.h>


namespace gigatraj {

/*! @name On-the-fly calculation flags
    These OTF_* constants are bitwise flag values that are
    used to specify certain aspects of how a meteorological data source
    should be calculated or returned.
*/
//@{
/// On-the-fly calculation flag: Convert to standard MKS units
const int OTF_MKS = 0x0002;
/// On-the-fly calculation flag: enable debugging messages
const int OTF_DEBUG = 0x0001;
//@}


/*!
   \defgroup OnTheFly On-The-Fly (OTF) Calculators

\brief Calculate certain data fields from other data fields

\ingroup MetDat
   
The OnTheFly (OTF) classes define objects that calculate
physical quantities from other physical quantities, "on the fly".
This is needed because not all meteorological data sources 
supply all desired physical quantities.  Since the burden of keeping
track of who supplies what should not be laid on the user,
a meteorological data source should be prepared to satisfy 
a user's request for a missing field by using an OTF object to generate
that field from other fields that it does have.  The OTF classes
typically receive as arguments one or more GridField objects
as input and generate a GridField object as output.  Note that the
input arguments are automatically upcast from their individual
classes (e.g., GridLAtLonField3D) to the more general GridField3D.
But the output variable must be explicitly downcast from the returned generic 
GridField3D abstract class to the more specific class such as GridLatLonField3D.

For example:
\code
     GridLatLonField3D temperature;
     GridLatLonField3D pressure;
     GridLatLonField3D theta
     ThetaOTF makeTheta;
     
     theta = dynamic_cast<GridLatLonField3D&>( makeTheta.calc( temperature, pressure ) );
     
\endcode

The MetOnTheFly subclasses rely on the ability of 
their input GridField objects to provide a scale and offset that
converts their values to 
standard SI ("MKS") units, regardless of the actual units
of the data they hold.  Consequently, a single MetOnTheFly
subclass can perform its calculation regardless of the
prevailing units that are used by the source of
the input fields.

It is important that individual OTF classes 
perform their calculations using the calc() method.
Any given OTF class's calc() method may take one, two,
or more arguments, depdending on how many are needed to 
carry out the calculation.  When a calculation can be carried
out using any of several sets of input fields, the
calc() method should accept any of those sets.
For example, air density might be calculated from temperature
and pressure, or from temperature and potential temperature,
or from pressure and potential temperature.  Rather than
calculate the density from a series of different
calc-like methods such as calc_pres_temp(), calc_temp_theta(), 
calc_pres_theta(), the preferred way is to provide
a single calc() method that examines its arguments and
performs its calculation according to those arguments'
quantities.

For this to work, it is necessary that the OTF object
be able to match the names of the input quantities
with the concepts of pressure, temperature, and so on.
The object must be able to determine that the first
GridField argument holds temperatures, regardless
of whether the actual quantity name the GridField returns
is "temperature", "air_temperature", "T", or "Temp."
This is accomplished by having the OTF object 
keep copies of the names of all of the physical quantities 
it might possibly use as an input field.  

Thus, if you write a new MetGridData class to read a 
particular data source, and you wish to make
some data fields available that are not present in the
original data source, you should have your new class
object record its preferred names for component
quantities in any OTF objects that it uses to
obtain missing fields.


*/

/*!
\ingroup OnTheFly

\brief abstract class for calculating meteorological data fields on the fly, given other meteorological data fields

The MetOnTheFly class is an abstract class.
An object of this class provides a two- or three-dimensional grid (longitude, latitude,
vertical-coordinate) of meteorological data at a given time.

Because different meteorological data sets provide different sets of
meteorological fields, it is often necessary to calculate a desired field
that is not provided, from other fields which are.  Moreover, such calculations
may be used repeatedly, for various data sets.  The MetOnTheFly class exists
to make such calculations easier and more straigbtforward, by providing a standardized
facility the is usable independent of the particular meteorological data set
being used.

*/

class MetOnTheFly {

   public:
      
      /// Error: calculation failed
      class badcalculation {};

      /// Error: mismatched input vectors
      class badprofile {};
     
      /// Error: input grids incompatibility
      class badgrid {};
      
      /// Error: bad input quantity
      class badinputquant {};
      
      /// Error: bad input quantity units
      class badinputunits {};

      
      /// Default constructor
      /*!
         This is the default contructor for the MetOnTheFly class.
      */
      MetOnTheFly();
      
      /// Default destructor
      /*!
         This is the default destructor for the MetOnTheFly class.
      */
      virtual ~MetOnTheFly();
      
      /// copy constructor
      /*!
         This is the copy contructor method for the MetOnTheFly class.
         
         \param src the source MetOnTheFly object to copy from
      */
      MetOnTheFly(const MetOnTheFly& src);


      /// copies settings from a source object to this one
      /*! 
           This method copies settings from a source MetOnTheFly object
           to this one.
           
           \param src the source MetOnTheFly object
      */     
      virtual void assign( const MetOnTheFly& src);


      
      /// returns the name of the physical quantity being calculated on the fly
      std::string quantity();
      
      /// returns the units of the  physical quantity being calculated on the fly
      std::string units();
      
      /// sets the name of the physical quantity to be calculated on the fly
      /*! An object of the MetOnTheFly class will through its calc() method 
          generate a GridField object that holds the results of the calculation.
          This function determines what the result will be called.
           
          param quantity the name of the physical quantity
      */     
      void set_quantity( std::string quantity ) {
         quant = quantity;
      }
      
      /// sets the the units of the physical quantity to be calculated
      /*! An object of the MetOnTheFly class will through its calc() method 
          generate a GridField object that holds the results of the calculation.
          This function determines what the result's physical units are.

           \param units the units
      */     
      void set_units( std::string units ) {
         uu = units;
      }
      
      
   protected:
      /// the name of the physical quantity being generated on the fly
      std::string quant;
      /// the units of the physical quantity being generated on the fly
      std::string uu;

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
