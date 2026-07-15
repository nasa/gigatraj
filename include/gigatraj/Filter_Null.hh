#ifndef GIGATRAJ_FILTER_NULL
#define GIGATRAJ_FILTER_NULL


#include "gigatraj/gigatraj.hh"
#include "gigatraj/ParcelFilter.hh"

namespace gigatraj {

/*!

\ingroup parcelfilters

\brief a null filter for Parcels that does nothing

The Filter_Null class provides an example of a ParcelFilter subclass
implementation. It illustrates the mechanisms used by ParcelFilters,
but it does not actually do anything to the Parcels to which it is applied.

*/

class Filter_Null : public ParcelFilter {

   public:
   
      /// the constructor
      /*! This is the constructor for Filter_Null objects.
           
          \param useless  merely an example of an internal Filter parameter. :

      */
      Filter_Null( int useless=0);

      /// the destructor
      /*! This is the destructor for the Filter_Null class
      */
      ~Filter_Null();
      
      /// copy constructor
      /*!
         This is the copy contructor method for the Filter_Null class.
         
         \param src the source Filter_Null object to copy from
      */
      Filter_Null(const Filter_Null& src);

      /// copy assignment
      /*! 
          This is the copy assignment operator for the Filter_Null class.
      */
      Filter_Null& operator=(const Filter_Null& src);

      /// copies settings from a source object to this one
      /*! 
           This method copies settings from a source Filter_Null object
           to this one.
           
           \param src the source Filter_Null object
      */     
      void assign( const Filter_Null& src);



      /// return the useless parameter
      /*! This method returns the value of the useless parameter
      
           returns the value of the useless parameter
      */      
      int useless() const;
      
      /// set the useless parameter
      /*! This method sets the value of the useless parameter.
      */    
      void useless( int value );



      /// method for applying applying the filter to a single Parcel
      /*! Apply the filter to a single parcel
      
      \param p the Parcel objects to which the filter is to be applied
    
      */
      void apply( Parcel& p ); 
      
      /// method for applying applying the filter to an array of Parcels
      /*! Apply the filter to an array of parcels
      
      \param p the array of Parcel objects to which the filter is to be applied
    
      \param n the number of Parcel objects in the array

      */
      void apply( Parcel * const p, const int n ); 
   
      /// method for applying applying the filter to a vector of Parcels
      /*! Apply the filter to a vector of Parcels
      
      \param p the vector of Parcel objects to which the filter is to be applied
    
      */
      void apply( std::vector<Parcel>& p ); 
   
      /// method for applying applying the filter to a list of Parcels
      /*! Apply the filter to a list of Parcels
      
      \param p the list of Parcel objects to which the filter is to be applied
    
      */
      void apply( std::list<Parcel>& p ); 
   
      /// method for applying applying the filter to a deque of Parcels
      /*! Apply the filter to a deque of Parcels
      
      \param p the deque of Parcel objects to which the filter is to be applied
    
      */
      void apply( std::deque<Parcel>& p ); 
   
      /// method for applying applying the filter to a Flock of Parcels
      /*! Apply the filter to a Flock of Parcels
      
      \param p the Flock of Parcel objects to which the filter is to be applied
    
      */
      void apply( Flock& p ); 
   
      /// method for applying applying the filter to a Swarm of Parcels
      /*! Apply the filter to a Swarm of Parcels
      
      \param p the Swarm of Parcel objects to which the filter is to be applied
    
      */
      void apply( Swarm& p ); 
      
    private:
    
       /// a perfectly useless internal parameter
       int unused;
             
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
