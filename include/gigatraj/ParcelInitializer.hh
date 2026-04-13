
#ifndef PARCELINITIALIZER_H
#define PARCELINITIALIZER_H

#include "gigatraj/gigatraj.hh"
#include "gigatraj/ParcelFilter.hh"

namespace gigatraj {

/*! \defgroup parcelinitializers Parcel Initialization Filters
\ingroup parcelfilters

\brief initialize Parcels

This is a superclass for classes that loads state information
into Parcel objects.

*/


/*!
\ingroup parcelinitializers

\brief an abstract base class for loading state into Parcels

The ParcelInitializer class is an abstract class for loading state information into a Parcel
object or collection of Parcel objects.  Note that an
object of this class does not create new Parcel objects;
the Parcel or collection of Parcels must already
exist.


*/

class ParcelInitializer : public ParcelFilter {

   public:
   

      /// An exception for some kind of problem with the filter
      class badinit: public ParcelFilter::badfilter {};

      /// Default constructor
      /*!
         This is the default contructor for the ParcelInitializer class.
      */
      ParcelInitializer();
      
      /// virtual destructor
      virtual ~ParcelInitializer();

      /// copy constructor
      /*!
         This is the copy contructor method for the ParcelInitializer class.
         
         \param src the source ParcelInitializer object to copy from
      */
      ParcelInitializer(const ParcelInitializer& src);


      /// copies settings from a source object to this one
      /*! 
           This method copies settings from a source ParcelInitializer object
           to this one.
           
           \param src the source ParcelInitializer object
      */     
      virtual void assign( const ParcelInitializer& src);




      /// Initialize a single Parcel
      /*! This method initializes a single Parcel object.
      
          \param p the Parcel object to be initialized

      */
      virtual void apply( Parcel& p ) = 0; 

      /// Initialize an array of parcels
      /*! This method initializes an array of Parcels
      
          \param p the array of Parcel objects to be initialized
           
          \param n the number of Parcel objects to be initialized

      */
      virtual void apply( Parcel * const p, const int n ) = 0; 

      /// Initialize a vector of parcels
      /*! This method initializes a vector of Parcels
      
          \param p the vector of Parcel objects to be initialized
           
      */
      virtual void apply( std::vector<Parcel>& p ) = 0; 

      /// Initialize a list of parcels
      /*! This method initializes a list of Parcels
      
          \param p the list of Parcel objects to be initialized
           
      */
      virtual void apply( std::list<Parcel>& p ) = 0; 

      /// Initialize a deque of parcels
      /*! This method initializes a deque of Parcels
      
          \param p the deque of Parcel objects to be initialized
           
      */
      virtual void apply( std::deque<Parcel>& p ) = 0; 

      /// Initialize a Flock of parcels
      /*! This method initializes a Flock of Parcels
      
          \param p the Flock of Parcel objects to be initialized
           
      */
      virtual void apply( Flock& p ) = 0; 

      /// Initialize a Swarm of parcels
      /*! This method initializes a Swarm of Parcels
      
          \param p the Swarm of Parcel objects to be initialized
           
      */
      virtual void apply( Swarm& p ) = 0; 

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

