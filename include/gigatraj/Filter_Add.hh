#ifndef GIGATRAJ_FILTER_ADD
#define GIGATRAJ_FILTER_ADD


#include "gigatraj/gigatraj.hh"
#include "gigatraj/ParcelFilter.hh"

namespace gigatraj {

/*!

\ingroup parcelfilters

\brief a null filter for Parcles that does nothing

The Filter_Add class is a ParcelFilter that adds new Parcels
to a collection of Parcels by replacing existing Parcels
that have been marked with a specified set of flags.

*/

class Filter_Add : public ParcelFilter {

   public:

      /// An exception for not having enough idle Parcels to add new ones
      class badoutofparcels {};
   

      /// the constructor
      /*! This is the constructor for Filter_Add objects.
           
          \param flagvalue  a bitwise combination of ParcelFlag values to check for
          \param negate if true, then the sense of the test is reversed

      */
      Filter_Add( ParcelFlag flagvalue=0, bool negate=false);

      /// the destructor
      /*! This is the destructor for the Filter_Add class
      */
      ~Filter_Add();
      
      /// copy constructor
      /*!
         This is the copy contructor method for the Filter_Add class.
         
         \param src the source Filter_Add object to copy from
      */
      Filter_Add(const Filter_Add& src);

      /// copy assignment
      /*! 
          This is the copy assignment operator for the Filter_Add class.
      */
      Filter_Add& operator=(const Filter_Add& src);

      /// copies settings from a source object to this one
      /*! 
           This method copies settings from a source Filter_Add object
           to this one.
           
           \param src the source Filter_Add object
      */     
      void assign( const Filter_Add& src);



      /// return the useless parameter
      /*! This method returns the flags to test against
      
           \return the value of the useless parameter
      */      
      ParcelFlag flagset() const;
      
      /// set the useless parameter
      /*! This method sets the flags ot test against
      
          \param value a bitwise combination of ParcelFlag values
                 against which Parcel flags will be tested
      */    
      void flagset( ParcelFlag value );

      /// return whether the sense of the test is to be negated
      /*! This method returns whether the sense of the test is to be negated
      
           \return true if the test is to be negated, false otherwise
      */      
      bool negation() const;
      
      /// set whether the sense of the test is to be negated
      /*! This method sets whether the sense of the test is to be negated.
      
          \param value if true, the sense of the flags tests will be negated
      */    
      void negation( bool value );

      /// returns a pointer to the source of new Parcels
      /*! This method returns a pointer to a vector that holds
          new Parcels to be added.

          \return a pointer to a vector of Parcels. Do not delete the vector
          referred to by this pointer until this Filter_Add object is 
          no longer neeeded.
          
      */
      std::vector<Parcel>* source() const;
      
      /// sets the source of new Parcels
      /*! This method sets the source of new Parcels ot be added.
      
          \param src a pointer to a vector of Parcels that will be used as the
          source of Parcels in the apply() methods. The calling routine is responsible
          for managing this vector. 
      */
      void source( std::vector<Parcel>* src );



      /// method for applying applying the filter to a single Parcel
      /*! Apply the filter to a single parcel.
      
          Note that, unlike the other apply() methods, this 
          one does not reset the internal counter before
          testing. This allows you to call it serially
          on individual Parcels, and get a meaningful count
          once you are done.
      
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
    
       /// the Parcel flags to test against
       ParcelFlag flgs;
       /// whether we negate the test 
       bool ng;
       /// the count, which can be queried for after filtering
       int cnt;
       
       /// a pointer to a source of new Parcels
       std::vector<Parcel>* newp;
       
       /// determines whether a given Parcel fits the criteria
       /*! This method determines whether a given parcel satisfies 
           the specified flag criteria
       
            \return true if the Parcel satisfies the criteria, false otherwise
            \param p a pointer to the Parcel to be tested
            
       */
       bool test( Parcel* p ) const;
       
       /// get the number of new PArcels to add
       /*! This method returns the number of new Parcels to add
           in an apply() method.
        
           \return the number of new Parcels in the source vector
       */
       int nNew() const;       
             
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
