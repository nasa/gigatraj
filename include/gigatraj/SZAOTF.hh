#ifndef GIGATRAJ_SZAOTF
#define GIGATRAJ_SZAOTF

#include <string>
#include <vector>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/MetOnTheFly.hh"
#include "gigatraj/GridField3D.hh"
#include "gigatraj/GridFieldSfc.hh"


namespace gigatraj {

/*!
\ingroup OnTheFly

\brief class for calculating solar zenith angles at specific longoitudes, latitudes, and times

\details

The SZAOTF class provides GridField3D and GridFieldSfc objects full of
solar zenith angles at their gridpoints for a given time.


It is intended that this class be used by implementers of new
subclasses of MetGridData, to satisfy users' requests solar zenith angle data,
which is rarely supplied in a meteorlogical data set.
Note that the calc() methods of this class take fields of the
GridField3D or GridFieldSfc virtual classes as input parameters and produce a new
GridField3D or GridFieldSfc object as output.  Objects of any single subclass 
of GridField3D/GridFieldSfc
may be used as inputs, so long as they are grid-compatible with each other,
and the return value will be of the same
subclass of GridField3D/GridFieldSfc as the inputs.  However, the calling routine will
need to do a dynamic cast from GridField3D or GridfieldSfc to the particular
subclass being used.

Users should obtain their solar zenith angle fields by requesting them
from a MetGridData subclass such as MetMERRA2, rather than by
reading other fields and giving them to SZAOTF.


*/

class SZAOTF : public MetOnTheFly {
  
     public:

         /// basic constructor 
         /*!
            This is the basic constructor for a new SZAOTF object.
         */
         SZAOTF();

         /// destructor
         /*! This is the class destructor
         */
         ~SZAOTF();

         /// does the calculation to produce a solar zenith angle field from a gridded 3D field
         /*! This function computes solar zenith angle (SZA) from a single input 3D field.

            \return a pointer to a new GridField3D object that holds the output SZA field.  
                    The units are in degrees, and the MKS scale factor can convert this to radians
                    if desired.  The calling routine is responsible for
                    deleting the new object when it is no longer needed.
            \param input the input data field.
            \param flags OTF_* flags to to affect the calculation results
         */
         GridField3D* calc( const GridField3D& input, int flags=0) const;


         /// does the calculation to produce a solar zenith angle field from a gridded 2D field
         /*! This function computes solar zenith angle (SZA) from a single input 2D pressure field.

            \return a pointer to a new GridField3D object that holds the output SZA field.  
                    The units are in degrees, and the MKS scale factor can convert this to radians
                    if desired.  The calling routine is responsible for
                    deleting the new object when it is no longer needed.
            \param input the input data field.
            \param flags OTF_* flags to to affect the calculation results
         */
         GridFieldSfc* calc( const GridFieldSfc& input, int flags=0) const;

         
         /// calculates solar zenith angle from a longitude, latitude, and time
         /*! This method does the work of calculating solar zenith angle from 
             longitude, latitude, and time. Note that the only time that makes sense here
             is the Gregorian Calendar.
             
             \param lon the input longitude value
             \param lat the input latitude value
             \param time the input time value
             \return the solar zenith angle, in degrees
         */    
         real calc( real lon, real lat, std::string& time ) const;
         
         /// calculates solar zenith angle from a longitude, latitude, and time
         /*! This method does the work of calculating solar zenith angle from 
             longitude, latitude, and time. Note that the only time that makes sense here
             is the Gregorian Calendar.
             
             \param lon the input longitude value
             \param lat the input latitude value
             \param jday the julian day
             \return the solar zenith angle, in degrees
         */    
         real calc( real lon, real lat, double jday) const;
         
         /// calculates solar zenith angle from arrays of longitudes and latitudes, at a given time
         /*! This method does the work of calculating solar zenith angle from 
             longitude, latitude, and time. Note that the only time that makes sense here
             is the Gregorian Calendar.
             
             \param lons a pointer to the array of input longitude value
             \param lats a pointer to the array of input latitude value
             \param n the number of elements in the ps array
             \param time the input time value
             \return a pointer to a new real array containing the solar zenith angle, in degrees 
                     It is the responsibilty
                     of the calling routine to delete this array when it is no longer used.
         */    
         real* calc( real* lons, real* lats, int n, std::string& time ) const;

         /// calculates solar zenith angle from arrays of longitudes and latitudes, at a given time
         /*! This method does the work of calculating solar zenith angle from 
             longitude, latitude, and time. Note that the only time that makes sense here
             is the Gregorian Calendar.
             
             \param lons a pointer to the array of input longitude value
             \param lats a pointer to the array of input latitude value
             \param n the number of elements in the ps array
             \param jday the julian day
             \return a pointer to a new real array containing the solar zenith angle, in degrees 
                     It is the responsibilty
                     of the calling routine to delete this array when it is no longer used.
         */    
         real* calc( real* lons, real* lats, int n, double jday ) const;

         /// calculate the julian day
         /*! This method calculates the Julian Day from the year, months, and day of the month
             
               \param year the year
               \param month the month (1-12)
               \param dom the day o fht emonth (1-31)
               \param hours the number of hours elapsed since midnight UTC
               \return the julian day
         */ 
         double julday( int year, int month, int dom, double hours=0.0 ) const;

         /// calculate the julian day from a string
         /*! This method calculates the Julian Day from the year, months, and day of the month
             
               \param time an ISO8601 formst UTC date string e.g., "2015-11-23T22:45:76.234"
               \return the julian day
         */ 
         double julday( std::string& time ) const;

     protected:
     
         /// breaks an ISO8601 data string into numeric year, month, dya, etc.
         /*! This method takes an ISO8601 format data string (e.g., "2014-11-23T23:45:13")
             and parses it into its numeric components.
             
             \param time a reference to the input string, in "YYYY-mm-ddTHH:MM:SS" format
             \param year a reference to a int to hold the output year (e.g., 2014)
             \param month a reference to a int to hold the output month (1-12)
             \param dom a reference to a int to hold the output day of the month (1-31)
             \param hr a reference to a int to hold the output hour (0-23)
             \param min a reference to a int to hold the output minute (0-59)
             \param sec a reference to a real to hold the output second (0-59)
             
        */     
        void date_to_parts( std::string& time, int& year, int& month, int& dom, int& hr, int& min, double& sec) const;

         /// takes the modulus of one number with respect to anotehr
         /*! This method computes the modulues of one floating po0int number with
             respect to another.  if A and B are positive, then
             modulus( A, B ) is ( A - n*B), where n is the floor of (A/B).
             
             Note that this function differes from the math library's remainder() function, in that
             their n is (A/B) rounded to the nearest integer.
             
             \param a the number of which the modulus is to be computed
             \param b the number with which the modulus is ot be computed
             \return the modulus of a with respect to b,
         */
         double modulus( double a, double b ) const;         
         
         // calendar object?

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
            
     
