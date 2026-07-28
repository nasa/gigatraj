#ifndef GIGATRAJ_FILTER_ADVANCE
#define GIGATRAJ_FILTER_ADVANCE


#include "gigatraj/gigatraj.hh"
#include "gigatraj/ParcelFilter.hh"
#include "gigatraj/Integrator.hh"

namespace gigatraj {

/*!

\ingroup parcelfilters

\brief calculates the next time step pg parcel trajectories

The Filter_Advance class is a ParcelFilter that 
traces Parcel trajectories by one time step.

*/

class Filter_Advance : public ParcelFilter {

   public:

      /// An exception for not having enough idle Parcels to add new ones
      class badoutofparcels {};
   

      /// the constructor
      /*! This is the constructor for Filter_Advance objects.
           
          \param timestep  the time step  to be used, in model time. 
          \param integ a pointer to an object of the Integrator class
                       that will be used to do the time integration.
                       If NULLPTR, then the Integrator use dby
                       the first Parcel will be employed.

      */
      Filter_Advance( double timestep=0, Integrator* integ=NULLPTR );

      /// the destructor
      /*! This is the destructor for the Filter_Advance class
      */
      ~Filter_Advance();
      
      /// copy constructor
      /*!
         This is the copy contructor method for the Filter_Advance class.
         
         \param src the source Filter_Advance object to copy from
      */
      Filter_Advance(const Filter_Advance& src);

      /// copy assignment
      /*! 
          This is the copy assignment operator for the Filter_Advance class.
      */
      Filter_Advance& operator=(const Filter_Advance& src);

      /// copies settings from a source object to this one
      /*! 
           This method copies settings from a source Filter_Advance object
           to this one.
           
           \param src the source Filter_Advance object
      */     
      void assign( const Filter_Advance& src);



      /// return the time step
      /*! This method returns the integration time step
      
           \return the value of the time step, in model time
      */      
      double timestep() const;
      
      /// set the time step
      /*! This method sets the integration time step
      
          \param value the time step, in model time
      */    
      void timestep( double value );

      /// return the Integrator
      /*! This method returns the time Integrator object that is being used
      
           \return a pointer to the Integrator. If not NULLPTr, then 
                   this is a pointer to the actual object, 
                   not a copy, so do not delete it and then try to 
                   apply this filter.
           
      */      
      Integrator* integrator() const;
      
      /// set the time integrator
      /*! This method sets the object used for time integration
      
          \param value a pointer to an Integrator object such as IntegRK4a.
                       Note that no copy is made of the object, so it
                       should not be deleted before trying to aply this filter.
                       This pointer may be NULLPTR.
      */    
      void integrator( Integrator* value );



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
    
       /// the time step
       double tstep;
       
       /// the time integrator
       Integrator* intg;
    
                    
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
