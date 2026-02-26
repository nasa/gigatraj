
#ifndef PARCELFILTER_H
#define PARCELFILTER_H

#include <iterator>
#include <deque>
#include <list>
#include <vector>


#include "gigatraj/gigatraj.hh"
#include "gigatraj/Parcel.hh"
#include "gigatraj/Flock.hh"
#include "gigatraj/Swarm.hh"

namespace gigatraj {

/*! \defgroup parcelfilters Parcel filters
\ingroup parcelstuff

\brief filters that create, initialize, destroy, report on, or otherwise deal with Parcels

This is a superclass from which every class that manipulates Parcel objects inherits.
Any object that creates or destroys Parcels, or which alters their state in some
what that is either impossible or inappropriate for the Parcels to do for themselves,
is a member of the ParcelFilter class.

*/

/*!
\ingroup parcelfilters

\brief an abstract class for operating on parcels

\details

The ParcelFilter class is an abstract class for performing a transformation on a collection
of Parcel objects.  This transformation may be a purely reporting function
(e.g., dumping the parcels' states to a file), or it may set parcels' states (e.g.,
a Parcel initializer). A ParcelFilter may be a source of Parcels (i.e., a member of
the ParcelGenerator class), or a sink for Parcels.


*/

class ParcelFilter {

   public:
   

      /// An exception for some kind of problem with the filter
      class badfilter {};

      /// An exception for a bad number of parcels
      class badparcelnum {};

      /// virtual destructor
      virtual ~ParcelFilter() {};

      /// Virtual method for applying the filter to a single Parcel
      /*! Apply the filter to a single parcel
      
      \param p the Parcel object to which the filter is to be applied

      */
      virtual void apply( Parcel& p ) = 0; 

      /// Virtual method for applying applying the filter to an array of Parcels
      /*! Apply the filter to an array of parcels
      
      \param p the array of Parcel objects to which the filter is to be applied
    
      \param n the number of Parcel objects in the array

      */
      virtual void apply( Parcel * const p, const int n ) = 0; 


      /// Virtual method for applying applying the filter to a vector of Parcels
      /*! Apply the filter to a vector of parcels
      
      \param p the vector of Parcel objects to which the filter is to be applied
    
      */
      virtual void apply( std::vector<Parcel>& p ) = 0; 

      /// Virtual method for applying applying the filter to a list of Parcels
      /*! Apply the filter to a list of parcels
      
      \param p the list of Parcel objects to which the filter is to be applied
    
      */
      virtual void apply( std::list<Parcel>& p ) = 0; 

      /// Virtual method for applying applying the filter to a deque of Parcels
      /*! Apply the filter to a deque of parcels
      
      \param p the deque of Parcel objects to which the filter is to be applied
    
      */
      virtual void apply( std::deque<Parcel>& p ) = 0; 

      /// Virtual method for applying applying the filter to a Flock of Parcels
      /*! Apply the filter to a Flock of parcels
      
      \param p the Flock of Parcel objects to which the filter is to be applied
    
      */
      virtual void apply( Flock& p ) = 0; 

      /// Virtual method for applying applying the filter to a Swarm of Parcels
      /*! Apply the filter to a Swarm of parcels
      
      \param p the Swarm of Parcel objects to which the filter is to be applied
    
      */
      virtual void apply( Swarm& p ) = 0; 


};


/// Method for applying a filter to an iterable container of Parcels. 
/*!  Applies a filter to an iterable container full of parcels

     \param parcels the container object of parcels
     \param fil the filter object to be applied to each parcel in the container 

*/
template<class Bunch, class Filter>
void sweep(Bunch& parcels, Filter fil) 
{  
   typename Bunch::iterator it = parcels.begin();
   while ( it != parcels.end() ) {
      (*fil) (*it);  // apply the filter 
      ++it;
   }   

}




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

