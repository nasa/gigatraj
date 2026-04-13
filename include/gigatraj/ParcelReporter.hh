
#ifndef PARCELREPORTER_H
#define PARCELREPORTER_H

#include "gigatraj/gigatraj.hh"
#include "gigatraj/ParcelFilter.hh"

namespace gigatraj {

/*! \defgroup parcelreporters Parcel Reporting Filters
\ingroup parcelfilters

\brief report on Parcels

This is a superclass for classes that extract state information from Parcels
and (possibly) do something with it.

*/

/*!
\ingroup parcelreporters

\brief an abstract class for extracting state from Parcels

The ParcelReporter class is an abstract class for extracting state information from
a Parcel object or a collection of Parcel objects. 
The basic idea is that the Parcel states are left unchanged, but the information
gained is used to some purpose.
The purpose may be almost
anything, from a creating a written file dump of each parcel's state,
to a computing a single statistic compiled from the ensemble of all parcels.


*/

class ParcelReporter : public ParcelFilter {

   public:
   

      /// An exception for some kind of problem with the filter
      class badreport: public ParcelFilter::badfilter {};


      /// Default constructor
      /*!
         This is the default contructor for the ParcelReporter class.
      */
      ParcelReporter();
      
      /// virtual destructor
      virtual ~ParcelReporter();

      /// copy constructor
      /*!
         This is the copy contructor method for the ParcelReporter class.
         
         \param src the source ParcelReporter object to copy from
      */
      ParcelReporter(const ParcelReporter& src);


      /// copies settings from a source object to this one
      /*! 
           This method copies settings from a source ParcelReporter object
           to this one.
           
           \param src the source ParcelReporter object
      */     
      virtual void assign( const ParcelReporter& src);



      /// Report on a single Parcel
      /*! This method reports on a single Parcel.
      
          \param p the Parcel object about which a report is to be made

      */
      virtual void apply( Parcel& p ) = 0; 

      /// Report on an array of Parcels
      /*! This method reports on an array of Parcels.
      
          \param p the array of Parcel objects about which a report is to be made
    
          \param n the number of Parcel objects to be operated on

      */
      virtual void apply( Parcel * const p, const int n ) = 0; 

      /// dump a vector of parcels
      /*! This method dumps a vector of Parcels
      
          \param p the vector of Parcel objects to be dumpd
           
      */
      virtual void apply( std::vector<Parcel>& p ) = 0; 

      /// dump a list of parcels
      /*! This method dumps a list of Parcels
      
          \param p the list of Parcel objects to be dumpd
           
      */
      virtual void apply( std::list<Parcel>& p ) = 0; 

      /// dump a deque of parcels
      /*! This method dumps a deque of Parcels
      
          \param p the deque of Parcel objects to be dumpd
           
      */
      virtual void apply( std::deque<Parcel>& p ) = 0; 


      /// dump a Flock of parcels
      /*! This method dumps a Flock of Parcels
      
          \param p the Flock of Parcel objects to be dumpd
           
      */
      virtual void apply( Flock& p ) = 0; 

      /// dump a Swarm of parcels
      /*! This method dumps a Swarm of Parcels
      
          \param p the Swarm of Parcel objects to be dumpd
           
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
