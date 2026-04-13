#ifndef PARCELGENERATOR_H
#define PARCELGENERATOR_H

#include <deque>
#include <list>
#include <vector>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/Parcel.hh"
#include "gigatraj/Flock.hh"
#include "gigatraj/Swarm.hh"
#include "gigatraj/SerialGrp.hh"

namespace gigatraj {

/*! \defgroup parcelgenerators Parcel Generators
\ingroup parcelstuff

   \brief create Parcels
   
This is a superclass for classes that generate Parcel objects.

*/
// It is a subclass of the ParcelFilter class.

/*!
\ingroup parcelgenerators

\brief a base class for generating collections of Parcels

The ParcelGenerator class is a base class for creating collections of Parcel objects.
If the methods of this base class are called, the Parcels created will be uninitialized.
Various subclasses that inherit from ParcelGenerator may create Parcels which are 
initialized in various ways: distributed randomly,
or read from a file, or on a regularly-spaced grid. The functionality that those subclasses
have in common, is found here.


*/

class ParcelGenerator {

   public:
   
      /// An exception for some kind of problem with the Generator
      class badgeneration {};

      /// An exception for a bad number of parcels
      class badparcelcount {};

      /// \brief default constructor
      /*! 
            This is the basic constructor for the ParcelGenerator class.
      */
      ParcelGenerator();

      /// \brief destructor
      /*! 
         This is the destructor method for the ParcelGenerator class.
      */
      virtual ~ParcelGenerator(); 

      /// \brief Copy-constructor
      /*!
         This is the copy contructor method for the Filter_Null class.
         
         \param src the source Filter_Null object to copy from
      */
      ParcelGenerator(const ParcelGenerator& src);

      /// \brief copy assignment
      /*!
          This is the copy assignment operator for the ParcelGenerator class.
      */
      ParcelGenerator& operator=(const ParcelGenerator& src);


      /// copies settings from a source object to this one
      /*! 
           This method copies settings from a source ParcelGenerator object
           to this one.
           
           \param src the source ParcelGenerator object
      */     
      virtual void assign( const ParcelGenerator& src);
      
   
      /// Create an array of Parcels
      /*! This method creates an array of Parcels. 
      
          \param n the number of parcels to create
         
          \return a Parcel pointer that points to an array of Parcel objects. Be sure to delete [] this Array when it is no longer needed.        
      */   
      Parcel * create_array(int n);


      /// Create vector container of Parcels
      /*! This method creates a vector container that holds Parcel objects.
      
          \param n  the size of the vector object; the number of parcels to create

          \return a pointer to a vector of Parcel objects. Be sure to delete this vector when it is no longer needed.
      */
       std::vector<Parcel>* create_vector( int n );
      
      /// Create a list container of Parcels
      /*! This method creates a list container of Parcel objects .
      
          \param n  the size of the list object; the number of parcels to create
         
          \return a pointer to a list of Parcel obejcts. Be sure to delete this list when it is no longer needed.
      */
       std::list<Parcel>* create_list( int n );
      
      /// Create an deque container of Parcel objects 
      /*! This method creates an deque container of Parcel objects.
      
        \param n  the size of the deque object; the number of parcels to create
         
         \return a pointer to a deque of Parcel objects. Be sure to delete this deque when it is no longer needed.
      */
       std::deque<Parcel>* create_deque( int n );
      
      
      /// Create a Flock container of Parcels
      /*! This method creates a Flock container of Parcels
      
         \param n  the size of the Flock object; the number of parcels to create
         \param pgrp a process-group object that is used for parallel processing
         \param r    the ratio of meteorological-data processors to parcel-tracing processors.
                     (For example, if r=3 then there will be one met processor for
                     every 3 parcel-tracing processors) 
         
         \return a pointer to a Flock of Parcel objects. Be sure to delete this Flock when it is no longer needed.
      */
       Flock* create_Flock( int n
                           , ProcessGrp* pgrp=NULLPTR, int r=0
                           );
      
      /// Create a Swarm pseudo-container of Parcels
      /*! This method creates a Swarm pseudo-container of Parcels
      
         \param n  the size of the Swarm object; the number of parcels to create
         \param pgrp a process-group object that is used for parallel processing
         \param r    the ratio of meteorological-data processors to parcel-tracing processors.
                     (For example, if r=3 then there will be one met processor for
                     every 3 parcel-tracing processors) 
         
         \return a pointer to a Swarm of Parcel objects. Be sure to delete this Swarm when it is no longer needed.
      */
       Swarm* create_Swarm( int n
                           , ProcessGrp* pgrp=NULLPTR, int r=0
                           );

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
