/* todo
   -  force numbers of parcels to be unsigned long ints-- or better, size_t?
     since 2^32 = 4,294,967,296

   - add the advance() function
   
   - add code to constructor to manipulate met data source 

*/

#ifndef GIGATRAJ_SWARM_H
#define GIGATRAJ_SWARM_H

#include <vector>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/Parcel.hh"
#include "gigatraj/PlanetNav.hh"
#include "gigatraj/MetData.hh"
#include "gigatraj/Integrator.hh"
#include "gigatraj/ProcessGrp.hh"

namespace gigatraj {

/*!  \ingroup parcels

\brief parallelizable collection of parcel information.  

The Swarm class is a container-like class that holds parcel information.
Unlike objects of the Flock class, which holds actual Parcel objects,
objects of the Swarm class hold the information that Parcels hold,
without holding actual Parcels. The reason for this is to provide
more efficient vectorizable access to that information when tracing Parcel trajectories,
especially in parallel-processing environments. 
The difference in internal storage, however, is mostly hidden from the user;
the Swarm accept sParcels for input, and its iterator returns a Parcle when
dereferenced.
Thus, the Swarm
may be considered a kind of "pseudo-container" class.

The idea is that you would put your Parcels into a Swarm,
and then apply certain operations such as computing trajectories 
to the Swarm as a whole. You can also 
iterate over the member Parcels of the Swarm to carry 
out some kind of operation on each one.

As with the Flock class,
parallel-processing abilities have been abstracted into
the ProcessGrp class, so the Swarm class automatically assigns parcel information
to available processors.  An iterator is provided that lets each processor
loop in parallel through its share of the parcels only.  Special class methods
exist as well to permit centralized serial processing of each parcel.

In addition, the Swarm can set some processors aside for
handling meteorological data instead of tracing trajectories.
This can be useful when you do not wish to hold a copies of 
large, high-resolution arrays of met data in every processor.
Instead, some number of processors hold the met data, 
and the tracing processors communicate with the met processors
to get their wind values.  Again, this should work transparently
with no additional programming effort required.  The Swarm simply 
takes care of it.

Thus, the Swarm class enables a programmer to 
write a custom massively-parallel trajectory model 
in a straighforward way, without having to worry (much)
about the mechanics of communication between parallel
processors. And because it holds its information internally
in a more efficient way, using a Swarm should be faster than using a Flock.


*/

class Swarm {
      

   public:
    
     /// An exception for some kind of problem with the Swarm
     class badgeneration {};

     /// An exception for a bad number of parcels
     class badparcelcount {};

     /// An exception for a bad index into the parcels
     class badparcelindex {};

     /// An exception for attempting to use a met-processor as a parcel-tracer
     class badMetIsNotTracer {};

     /// The basic constructor
     /*! This is a Swarm constructor that assigns N parcels. Note: this will result
         in a Swarm that uses the SerialGrp class to simulate/bypass parallel
         processing, and the Parcel class as its parcel.
         
         A serial processing process group will automatically be assigned to the Swarm.

         Note that the meteorological data source shared by all the parcels
         will have its process group replaced by a process (sub)group that is
         associated with this Swarm. The original process group will be
         remembered and restored when this Swarm is destroyed. In this way, PGroups
         are managed automatically in most common use cases. But when using multiple
         Parcel containers simultaneously, pgroups must be managed carefully with the
         getPGroup(), setPGroup(), rememberPGroup(), and restorePGroup() methods.  

         \param n  the number of parcels to put into the Swarm initially. 
     */
     Swarm( int n=0 );
     
     /// parallel-processing constructor
     /*! This Swarm constructor distributes the N parcels among
      potentially several processors, using a ProcessGrp object.
      Note: this will result in a Swarm that uses the Parcel class 
      as its parcel.
      
         Note that the meteorological data source shared by all the parcels
         will have its process group replaced by a process (sub)group that is
         associated with this Swarm. The original process group will be
         remembered and restored when this Swarm is destroyed. In this way, PGroups
         are managed automatically in most common use cases. But when using multiple
         Parcel containers simultaneously, pgroups must be managed carefully with the
         getPGroup(), setPGroup(), rememberPGroup(), and restorePGroup() methods.  
      
         \param pgrp a process-group object that is used for parallel processing
         \param n    the number of parcels to put into the Swarm initially. Note that 
                     this is used only by the root processor in the process-group; all 
                     other processors receive their Swarm size from the root processor.  
         \param r    the ratio of meteorological-data processors to parcel-tracing processors.
                     (For example, if r=3 then there will be one met processor for
                     every 3 parcel-tracing processors) 
                    
     */
     Swarm( ProcessGrp* pgrp, int n=0, int r=0);
     
     /// Parcel-initializing constructor
     /*! This Swarm constructor assigns N parcels by copying a user-supplied Parcel object. 
         Note: this will result in a Swarm that uses the SerialGrp class to simulate/bypass parallel
         processing.
         
         A serial processing process group will automatically be assigned to the Swarm.

         Note that the meteorological data source shared by all the parcels
         will have its process group replaced by a process (sub)group that is
         associated with this Swarm. The original process group will be
         remembered and restored when this Swarm is destroyed. In this way, PGroups
         are managed automatically in most common use cases. But when using multiple
         Parcel containers simultaneously, pgroups must be managed carefully with the
         getPGroup(), setPGroup(), rememberPGroup(), and restorePGroup() methods.  

         \param p the input parcel object with whose copies this Swarm will be populated
         \param n  the number of parcels to put into the Swarm initially
     */
     Swarm( const Parcel &p, int n=0);
    
     /// Parcel-initializing constructor for parallel processing 
     /*! This Swarm constructor assigns N parcels by copying a user-supplied Parcel object.  
         It distributes the N parcels among
         potentially several processors, using a ProcessGrp object.

         Note that the meteorological data source shared by all the parcels
         will have its process group replaced by a process (sub)group that is
         associated with this Swarm. The original process group will be
         remembered and restored when this Swarm is destroyed. In this way, PGroups
         are managed automatically in most common use cases. But when using multiple
         Parcel containers simultaneously, pgroups must be managed carefully with the
         getPGroup(), setPGroup(), rememberPGroup(), and restorePGroup() methods.  

         \param p the input parcel object with whose copies this Swarm will be populated
         \param pgrp a process-group object that is used for parallel processing
         \param n  the number of parcels to put into the Swarm initially. Note that 
                     this is used only by the root processor in the process-group; all 
                     other processors receive their Swarm size from the root processor. 
         \param r  the ratio of meteorological-data processors ot tracing processors.
                  (For example, if r=3 then there will be one met processor for
                  every 3 parcel-tracing processors) 
                    
     */
     Swarm( const Parcel &p, ProcessGrp* pgrp, int n=0, int r=0);
     

     /// The destructor
     /*! 
        This is the destructor method for Swarm objects
        
        (This has the side effect of restoring the process group
        used by the meteorological data source shared by all Parcels.)
        
     */
     ~Swarm();

     /// returns whether the current processor is the root processor
     /*! This method determines whether the current processor 
         is the root processor for this Swarm. In a multi-processing environment,
         it can be useful to carry out certain operations only on
         the root processor (file I/O, for example). This function 
         permits that.
         
         \return true if this is the root processor of the Swarm's process group.
     
     */
     bool is_root() const;
     
     /// returns the ID of this processor in the group
     inline int proc_id() const 
     {
         return pgroup->id();
     }
     /// returns the ID of the root processor of this group 
     inline int root_id() const 
     {
         return pgroup->root_id();
     }
     /// returns the number of processors in this group
     inline int psize() const 
     {
         return pgroup->size();
     }
     /// returns whether this processor belongs to the process group
     inline int pbelongs() const 
     {
         return pgroup->belongs();
     }
     /// returns the process type
     inline int ptype() const 
     {
         return pgroup->type();
     }


     /// returns the met processing (sub)group that should be used by this Swarm instance 
     /*!  This method returns the process (sub)group that will be used by 
          the meteorological data source as this instance of the Swarm does its work.

          When a Swarm is created, it has a process group that is divided into
          subgroups, one for each collection of Parcels handled by a given processor
          running an instance of the Swarm. The meteorological data source that
          is shared by all of the Parcels running on that processor, will have
          its process group set to one of those process subgroups created by the Swarm.
          In this way, meteorological data-handling can be done in a parallel
          processing context. But the process group that the meteorological data source
          had before the flock was created, should be remembered and restored
          when the flock is destroyed. 

           \param id a pointer to an integer to hold the ID of a processor
                  within the processor group that is dedicated to 
                  managing meteorological data. 
           
           \return a pointer to the ProcessGrp to which the processor belongs       
      */
     ProcessGrp* getPGroup( int* id = NULLPTR ) const;
     
     /// sets the met processing group that should be used by this Swarm instance 
     /*! This method sets the process group that will be used by 
          the meteorological data source as this instance of the Swarm does its work.

          When a Swarm is created, it has a process group that is divided into
          subgroups, one for each collection of Parcels handled by a given processor
          running an instance of the Swarm. The meteorological data source that
          is shared by all of the Parcels running on that processor, will have
          its process group set to one of those process subgroups created by the Swarm.
          In this way, meteorological data-handling can be done in a parallel
          processing context. But the process group that the meteorological data source
          had before the flock was created, should be remembered and restored
          when the flock is destroyed. 

        \param pg a pointer to the process group to be used
        \param id the processor ID within process group pg, that indicates a dedicated meteorological data processor.  
                   An ID of -1 indicates that no dedicated met processor is to be used.
  
     */
     void setPGroup( ProcessGrp *pg, int id = -1 );
     
     /// returns the met processing group that should be restored when this Swarm instance is destroyed
     /*!  This method returns the process group that was used by 
          the meteorological data source before this instance of the Swarm was created.

          When a Swarm is created, it has a process group that is divided into
          subgroups, one for each collection of Parcels handled by a given processor
          running an instance of the Swarm. The meteorological data source that
          is shared by all of the Parcels running on that processor, will have
          its process group set to one of those process subgroups created by the Swarm.
          In this way, meteorological data-handling can be done in a parallel
          processing context. But the process group that the meteorological data source
          had before the flock was created, should be remembered and restored
          when the flock is destroyed. 

           \param id a pointer to an integer to hold the ID of a processor
                  within the processor group that is dedicated to 
                  managing meteorological data. 
           
           \return a pointer to the ProcessGrp to which the processor belongs       
      */
     ProcessGrp* fetchRememberedPGroup( int* id = NULLPTR ) const;

     /// sets the met processing group that should be restored when this Swarm instance is destroyed
     /*! This method sets the process group that is to be used by 
          the meteorological data source after this instance of the Swarm is destroyed.

          When a Swarm is created, it has a process group that is divided into
          subgroups, one for each collection of Parcels handled by a given processor
          running an instance of the Swarm. The meteorological data source that
          is shared by all of the Parcels running on that processor, will have
          its process group set to one of those process subgroups created by the Swarm.
          In this way, meteorological data-handling can be done in a parallel
          processing context. But the process group that the meteorological data source
          had before the flock was created, should be remembered and restored
          when the flock is destroyed. 

        \param pg a pointer to the process group to be used. If NULLPTR, then the current
                  process group and id will be obtained form the met data source. 
        \param id the processor ID within process group pg, that indicates a dedicated meteorological data processor.  
                   An ID of -1 indicates that no dedicated met processor is to be used.
  
     */
     void rememberPGroup( ProcessGrp* pg=NULLPTR, int id = -1 );
     
     /// restores the met processing group to the one that should be used after the Swarm instance is destroyed
     /*! This method sets the processor group of the meteorologicla data source 
         to the one that should be used after this Swarm instance is destroyed.
         
          When a Swarm is created, it has a process group that is divided into
          subgroups, one for each collection of Parcels handled by a given processor
          running an instance of the Swarm. The meteorological data source that
          is shared by all of the Parcels running on that processor, will have
          its process group set to one of those process subgroups created by the Swarm.
          In this way, meteorological data-handling can be done in a parallel
          processing context. But the process group that the meteorological data source
          had before the flock was created, should be remembered and restored
          when the flock is destroyed. 
     
     */
     void restorePGroup();

     /// sets the met processing group to the one that should be used by this flock instance
     /*! This method sets the processor group of the meteorologicla data source 
         to the one that should be used by this Swarm instance.
         
          When a Swarm is created, it has a process group that is divided into
          subgroups, one for each collection of Parcels handled by a given processor
          running an instance of the Swarm. The meteorological data source that
          is shared by all of the Parcels running on that processor, will have
          its process group set to one of those process subgroups created by the Swarm.
          In this way, meteorological data-handling can be done in a parallel
          processing context. But the process group that the meteorological data source
          had before the flock was created, should be remembered and restored
          when the flock is destroyed. 
     
     */
     void unRestorePGroup();






     class iterator;
     friend class iterator;
     /*!
     \brief iterator for the Swarm class.

     The iterator for this Parcel container class.
     For efficiency's sake, a Swarm does not actually contains any actual Parcels, 
     but merely an array of Parcel contents. Think of it as a psuedo-container of Parcels.
     This makes an iterator tricky, since dereferencing a Swarm iterator is expected to
     result in a Parcel. This is simulated by maintaining a Parcel internal to the
     iterator, whose contents are initialized to the first element of the Swarm's arrays 
     (longitude, latitude, etc.)
     on creation at begin(). When the iterator is incremented, the contents of this internal 
     Parcel are stored back into the Swarm's arrays, and then it is loaded with the
     next element of those arrays.
     
     This means that the Swarm's contents must not be accessed with an iterator and 
     directly accessed (using the set() method, get() method, parcel() method, or [] operator)
     at the same time (i.e., within the same loop).  Choose one method or the other (iterator or direct), but do not
     intermix them in the same iterator loop.
     Also, since the de-referenced iterator doe snot point to any actual part
     of the Swarm's contents, but merely a copy, care must be taken when exiting a loop
     prematurely. If the iterator does not exit the loop the nromal way, by being incremented to end(), then
     the iterator's stop() method should be called once the loop is exited.

     In a serial-processing environment this class functions
     as a conventional iterator, cycling through every parcel in the
     Swarm. But in a parallel-processing environment, this class
     runs on each processor in such a way that it
     iterates only over the parcels within the Swarm that are assigned to
     that processor.
     
     In other words, when multiprocessing this iterator is used to access only those parcels
     that belong to the current process. Thus, when a Swarm
     is being processed across multiple processors, each
     processor is iterating over its own parcels only, 
     but from the viewpoint of the programmer it appears
     to be iteratating over all parcels in the Swarm.  The multi-processing
     aspect of the loop becomes a negligible detail.
     
     If some of the processors are dedicated to reading meteorological data,
     they must not try to trace parcels inside the loop.  This, too, is
     handled automatically.  See the Swarm::begin() and Swarm::end() methods,
     below.

     */
     class iterator  {
        public:
           
           /// default constructor for the Swarm::iterator
           /*! This is the default constructor for the Swarm::iterator class,
               and the version that is normally used.
               Using this version of the constructor requires initialization
               with  Swarm::begin() or Swarm::end(). 
      
           */
           iterator();

           /// alternate constructor for the Swarm::iterator
           /*! This special version of the Swarm::iterator class allows for initialization to a 
               user-specified Parcel in a user-specified Swarm.

               \param init which of the local processor's parcels to point to
               \param swm the Swarm for which this is an iterator
           */
           iterator(int init, Swarm *swm);
           
           /// operator * override
           /*! This method overrides the * operator, permitting de-referencing the iterator
               to obtain the current Parcel.
               
               \return the current Parcel on this processor
           */    
           Parcel& operator*();

           /// operator -> override
           /*! This method overrides the -> operator, permitting de-referencing the iterator
               to obtain the current Parcel.
               
               \return the current Parcel on this processor
           */    
           Parcel* operator->();
           
           /// operator == override 
           /*! This method overrides the == operator, allowing for
               testing two iterators for equallity.
               
               \return True if the two iterators point to the same Parcel, False otherwise
           */    
           bool operator==(const iterator& x) const;
           
           /// operator != override 
           /*! This method overrides the != operatpr, allowing for
               testing two iterators for inequality.
               
               \return True if the two iterators do not point to the same Parcel, False othereise.
           */
           bool operator!=(const iterator& x) const;
           
           /// operator ++ override 
           /*! This method overrides the ++ postfix operator, permitting advancing
               to the next Parcel in the Swarm.
           */    
           iterator& operator++(int n);
           
           /// operator ++ override 
           /*! This method overrides the ++ prefix operator, permitting advancing
               to the next Parcel in the Swarm.
           */    
           iterator& operator++();
           
           /// returns the index that corresponds to the iterator
           /*! This method returns an index into the Swarm that corresponds to
               the iterator.
             
               For serial processing applicatins, this is simply the iteration number.
               But for multi-processing applications, where the local-processor iteration
               number does not correspond to the global index into the Swarm container,
               this provides a useful handle into the global context of a given Parcel.
               
               \return the index of this iterator's Parcel in the Swarm. If the 
               iterator corresponds to no Parcel (e.g., if this is a met data processor), 
               then -1 is returned.
               
           */    
           int index() const;
           
           /// cleans up after a preature exit
           /*! This method should be called if an iterator loop is exited in some
               way other than incrementing the iterator to end(). It cleans up
               the iterators, mianly by copying it's copy of the cuttent Parcel object
               back into the Swarm's content.
           */
           void stop();    
     
        protected:
           friend class Swarm;
           
           /// initializes the iterator
           /*! This allows the Swarm to initialize an existing 
               Swarm::Iter object

               \param init which of the local processor's parcels to point to
               \param flk the Swarm for which this is an iterator
           */
           void set(int init, Swarm *flk);
           
        private:
           // which parcel on this processor is the currently-indexed?
           int my_parcel;
           // which Swarm is this an iterator for?
           Swarm* my_swarm;
           // the Parcel that holds the info from a single parcel of the Swarm
           Parcel pcl;
           // a flag that indicates whether the pcl Parcel has been loaded with values
           bool loaded;
     
     };

     /// Type Iter is the iterator for the Swarm container
     /*!
         With the Swarm::iterator class defined to iterate over elements
         in the Swarm class (potentially when those elements are distributed
         among a number of processors run in parallel), the Iter type
         is defined as a convenient shorthand for Swarm::iterator.
     */     
     typedef Swarm::iterator Iter;
     
     /// returns the beginning of the Swarm
     /*!
         This method returns ana iterator that is initialized to the first
         Parcel in the Swarm that is assigned to the current processor.
         In single-processing environments, this is simple the first element of
         the Swarm. In multiprocessing environments, each processor has its
         own subset of Parcels in the Swarm, and this method returns the first
         of the subset that is assgined to the processing it is running on.
         
         \return the Iter that is initialized to the first Parcel in the Swarm that is to be dealt with. 
     */
     Iter begin();

     /// returns the end of the Swarm
     /*!
         This method returns ana iterator that is initialized to one past the last
         Parcel in the Swarm that is assigned to the current processor.
         In single-processing environments, this is simple the last element of
         the Swarm, plus one. In multiprocessing environments, each processor has its
         own subset of Parcels in the Swarm, and this method returns one past the last
         of the subset that is assgined to the processing it is running on.
         
         \return the Iter that is initialized to just after the last Parcel in the Swarm that is to be dealt with. 
     */
     Iter end();

     /// begins the use of the Swarm in an iteration loop
     /*! This method performs initial setup for iterating over Parcels in a Swarm.
         For example, in a multi-processor environment, it initializes
         the meteorological source.
         
         Ordinarily, this method should not be called directly
         from your programs, but it is available in case you need
         it for something unusual.
     */    
     void debut(); 

     /// ends the use of the Swarm in an iteration loop
     /*! This method performs final shutdown after iterating over Parcels in a Swarm.
         For example, in a multi-processor environment, it manages
         shutting down the meteorological source.

         Ordinarily, this method should not be called directly
         from your programs, but it is available in case you need
         it for something unusual.
     */    
     void fin(); 

     
     /// changes the planetary navigation
     /*! This method changes the navigational object used for tracing
         Parcels.
        
         The default navigation object is for the spherical Earth, which is almost certainly
         what you want to use.
        
         Note: the navigational object is shared by all parcels
         in a given process.  Thus, setting the navigation object for one parcel
         sets it for all parcels in the process.  If you are
         doing serial processing, just set the navigation for
         one Swarm.  If you are doing parallel processsing, with
         different parallel processes tracing distinct batches of 
         Parcels, then each process needs to set the navigation object.
        
         \param newnav the new planetary navigation object
        
     */   
     void setNav( PlanetNav& newnav );
     
     /// Returns the current planetary navigation object
     /*! This method returns a pointer to the navigational object
         currently being used in this Swarm. Unless has been set to
         something else, this will be the default spherical Earth navigation
         object.
     
          \return the current navigational object
     */
     PlanetNav* getNav();


     /// changes the meteorological data source
     /*!
        This method changes the ,eteorological data source used to 
        obtain the winds used to trace the path of Parcels in the Swarm.
        
        The default meteorological data source is MetSBRot, or
        tilted solid-body rotation winds.  Unless you are 
        doing testing, this is almost certainly not what you want.
        But as a source of purely artificial, analytically-determined
        winds, MetSBRot is always guaranteed to
        be present. Because the availability of actualy meteorological data and
        formats varies widely from user site to user site, MetSBrot
        is the only safe choice for a default.
        
        Note: the meteorological object is shared among all
        parcels for a given process.  Thus, setting the 
        meteorological source for one parcel
        sets it for all parcels in the process. If you are
        doing serial processing, just set the met source for
        one parcel.  If you are doing parallel processsing, with
        different parallel processing tracing distinct batches of 
        Parcels, then each process needs to set the met source object.
             
        \param newmet the new met data source object
        
     */   
     void setMet( MetData& newmet );
     
     /// Returns the current meteorological data source
     /*! This method returns a pointer to the current meteorological data source object.
        
          \return a pointer to the current MetData met source.
     */
     MetData* getMet();


     /// returns the total number of parcels in the Swarm
     /*! This method returns the total number of Parcels in the Swarm.

         Note that this is the total number of all Parcels summed
         across all processors, not the number of Parcles handled by any 
         one processor.
         
         \return the number of Parcels in the Swarm
     */
     int size();
     
     /// replaces a specified parcel in this Swarm with a new parcel
     /*! This method assigns a Parcel to a specific element in the Swarm.
     
          \param n the index of the parcel to be replaced.  (This is relative
                  to the entire Swarm, not just one processor's subset.)
          \param p the parcel of which a copy will be put into the Swarm. 
          \param mode if mode==0 (the default), then the input parcel p is taken
                     from the root processor's copy only.  In a multiprocrssor
                     environment, this will incur interprocrssor communication.
                     If mode==1, then the processor that owns the parcel 
                     will uses its own local value for the input parcel.
     */   
     void set( const int n, const Parcel& p, const int mode=0);
     
     ///  returns a parcel from this Swarm
     /*! This method returns a pointer to a specific Parcel in the Swarm.
     
          \param n the index of the parcel to be replaced.  (This is relative
                  to the entire Swarm, not just one processor's subset.)
                  
          \param mode if mode==0 (the default), then the parcel gets propagated to 
                     every processor, so that on every processor the 
                     return value has the correct value.  This may take some time.
                     If mode==1, then only the root processor of the Swarm's
                     main processor group will return a valid value.
                     All other processors return a NULLPTR.
                     Regardless of mode setting, all met-reading processors
                     will return a NULL pointer from this method.

          \return a pointer to the desired Parcel. or NULL (see mode, above)
                  
     */
     Parcel* parcel( const int n, const int mode=0 ) const;
     
     /// returns a parcel from this Swarm 
     /*! This method returns a reference to a specific Parcel in the Swarm.
         It exists for convenience only, and merely calls the parcel() method.
     
          \param n the index of the parcel to be replaced.  (This is relative
                  to the entire Swarm, not just one processor's subset.)
                  
          \param mode if mode==0 (the default), then the parcel gets propagated to 
                     every processor, so that on every processor the 
                     return value has the correct value.  This may take some time.
                     If mode==1, then only the root processor of the Swarm's
                     main processor group will return a valid value.
                     All other processors throw as badparcelindex error.
                     Regardless of mode setting, all met-reading processors
                     will throw a badparcelindex error from this method.

          \return a copy of the desired Parcel. Note that changing this Parcel
                    does NOT affect the original that is still in the Swarm.
                           
     */
     Parcel get( const int n, const int mode=0 ) const;
     
     /// operator [] override
     /*! This method overrides the [] operator, providing a convenient notation
         for accessing individual Parcels within the Swarm container, as
         if they were elements in an Array.
         This method is equivalent to get(n)
          
          \param n the index of the parcel to be replaced.  (This is relative
                  to the entire Swarm, not just one processor's subset.)

          \return a copy of the desired Parcel. Note that changing this Parcel
                    does NOT affect the original that is still in the Swarm.
                           
     */
     Parcel operator[]( int n );
     

     /// adds a single parcel to the Swarm
     /*! This method adds a new Parcel to the end of the Swarm.

          WARNING: on a multi-processing system, push_back() will
          most likely not add the parcel to the end.  
          
          \param p the parcel to be added ot the Swarm

          \param mode if 0, then the value will be taken from the root processor in a multiprocessing system
                      if 1, then the value will be taken from the local processor.

     */
     void add( const Parcel& p, const int mode=0 );
     
     /// advance the parcels by one time step
     /*! This methods advances the parcels in the Swarm in time.
      
         \param dt the delta-time over which the parcel is to advance
         
         \return always returns zero.
         
     */
     int advance( double dt );
     
     /// count the number of Parcels with one or more ParcelFlag values set. 
     /*! This methods counts the number of parcels in the Swarm 
         whose flags settings match a query.
      
         \param flgs a bitwise ORing of possible ParcelFlag settings.
         \param negate set to true if a count is desired of the Parcels whose flags are NOT set
         
         \return the count.
         
     */
     int countFlags( ParcelFlag flgs=0, bool negate=false );
     
     /// count the number of Parcels with one or more ParcelStatus values set. 
     /*! This methods counts the number of parcels in the Swarm 
         whose status settings match a query.
      
         \param stat a bitwise ORing of possible ParcelStatus settings.
         \param negate set to true if a count is desired of the Parcels whose status settings are NOT set
         
         \return the count.
         
     */
     int countStatus( ParcelStatus stat=0, bool negate=false );
     
     /// clears the Swarm so that is has no Parcels
     /*! This method empties the Swarm of its Parcels.
     
         As a side effect, it also restores the Pgroup of the 
         current Parcels' met data source, and it destroyes its
         internal PGroups.
     
     */
     void clear();
     
     /// allocates space in the Swarm for Parcels
     /*! This method clears the flock and then
         allocates the required number of Parcels.
         
         In a multiprocessing environment, it also
         allocates Parcles to each processor and sets up PGroups to
         handle them.
         
         \param n the number of parcels
         \param pgrp a process-group object that is used for parallel processing. If NULLPTR,
                  then the Swarm's current PGroup is used.
         \param p a pointer to a sample Parcel used to populate the Swarm. If NULLPTR,
                  then a standard Parcel is created for this.
         \param r  the ratio of meteorological-data processors ot tracing processors.
                  (For example, if r=3 then there will be one met processor for
                  every 3 parcel-tracing processors) 
     */
     void allocate( int n, ProcessGrp* pgrp, const Parcel* p=NULLPTR, int r=0 );     
     
     
     
     
     /// synchronizes the Swarm's processors
     /*! This method synchronizes the Swarm's processors.
     */
     void sync();
     
     /// synchronizes the Swarm's processors
     /*! This method synchronizes the Swarm's processors.
     */
     void sync(const std::string msg);
     
     /// introduces a possible delay in a parallel-processing environment
     /*! This method calls the delay() methods of the meteorological data source.
         For certain data sources in a parallel-processing environment, this introduces
         a delay to certain processors, to de-synchronize them to reduce the
         number of simultaneous requests to a network data service such as OPeNDAP.
     */
     void metDelay();
     
     /// convenience function for debugging
     /*! This method prints out some summary information about the Swarm.
         It is used for debugging.
     */    
     void dump() const;





   private:
   
     /// the number of parcels in this Swarm
     /*! The total number of parcels in the Swarm summed over
         all processors.
     */    
     int num_parcels_total;
     
     /// the process group object
     /*! The process group object that handles multiprocessing for
         this Swarm. This enables use of parallel processing, if desired.
         
         In a parallel processing environment, 
         this process group will be split into one or more subgroups to handle
         sets of the Swarm's Parcels, with each subgroup having possibly one 
         single-processor subgroup
         to handle meteorological data.
     */    
     ProcessGrp *pgroup;
     
     /// The met data source's pgroup that it had before this Flock was created
     ProcessGrp *preserved_met_pgroup;
     /// The met data source's processor id that it had before this Flock was created
     int preserved_met_pid;
     /// This flock's instance's pgroup, which is what the met data source's pgroup is set to when using this Flock
     ProcessGrp *preserved_flock_pgroup;
     /// This flock's instance's processor id, which is what the met data source's pgroup is set to when using this Flock
     int preserved_flock_pid;
     
     /// holds the processor groups for this Swarm
     /*!
         As noted above, the pgroup is split into multiple processor
         groups. This vector holds the objects for those (sub)groups.
     */
     std::vector<ProcessGrp*> subgroups;

     // the (sub)group to which this processor belongs
     /*!
        This is the index of the subgroups vector that
        corresponds to the (sub)group that
        this processor belongs to. 
     */
     int my_group;

     /// The beginning pgroup index of each processor subgroup
     /*! This holds the ranks-in-pgroup of the first processor in each new (sub)group
      (subsequent processors in the new group are subsequent processors
      in pgroup.)
     */ 
     std::vector<int> pgstarts;

     /// The ending pgroup index of each processor subgroup
     /*! This holds the number of processors in each new (sub)group.
     */
     std::vector<int> pgends;
     
     /// The starting index of this processor's set of Parcels
     /*! This holds the index of the beginning parcel of each tracing processor
        (for a given parcel, this is the same as my_parcel_start)
     */
     std::vector<int> pclstarts;

     /// The ending index of this processor's set of Parcels
     /*! This holds the index of the ending parcel of each tracing processor
         (for a given parcel, this is the same as my_parcel_start+my_num_parcels-1)
     */
     std::vector<int> pclends;
   

     /// the starting parcel index for this process
     /*!
         This holds the starting index of this Swarm's Parcels that are handled by
         the current local processor.
     */
     int my_parcel_start;
     /// the number of parcels handled by this process
     /*!
        This holds the number of this Swarm's Parcels that are handled by the
        current local processor.
     */
     int my_num_parcels;

     /// a sample parcel
     Parcel* sample_p;
     
     /// The size of the arrays that hold the Parcel information
     unsigned int info_size;
     
     /// How much to increment the parcel info arrays when they need to grow
     unsigned int info_inc;
     
     /// the process that handles the met data
     /*! 
          This holds the index of the processor that handles meteorological data.
     */
     int my_met;
     
     /// am I a met processor?
     /*! This reflects whether the current processor is dedicated
         to handling meteorological data (1), or whether it traces Parcels  (0).
     */    
     int is_met;
     
     
     /// the number of parcels to be processed at one time by the advance() method
     /*! This is the number of parcels to be processed as a single block
         by the advance() method. If <= 0, then all of the parcels
         local to this processor will be processed as a single block.
     */
     int blocksize;


     //! Longitudes
     /*!
      The parcel longitudes 
     */
     real *lons;

     //! Latitudes
     /*!
      The parcel latitudes
     */
     real *lats;

     //! Vertical positions
     /*!
     The parcel vertical positions
     */
     real *zs;

     //! Times
     /*!      
     The  parcel times, in internal model units.
     */
     double *ts;
     
     //! Tags
     /*!
     The parcel tags
     */
     double *tgs;
     
     //! Flags
     /*!      
     The parcel flags 
     */
     ParcelFlag *flagsets;

     //! Status
     /*!      
     The parcel status
     */
     ParcelStatus *statuses;
     
     
     //! ID number
     /*!
     The intra-processor Parcel ID number
     Used to keep track of parcels as they may be rearranged internally.
     But these IDs are only local to a given processor and its local
     local collection of parcels. That is, the ID numbers
     run from zero to ( my_num_parcels - 1 ), not (num_parcels_total - 1 ).
     */
     int *ids;

     //! Navigation
     /*!   The planetary navigation object to be used for all parcels
     */
     PlanetNav *nav;
      
     //! Meteorological data source
     /*! The meteorological data source to be used for all parcels
     */
     MetData *metsrc;

     //* Integration method
     /*! the integration method to be used to trace the trajectories
     */
     Integrator *integ;

     
     /// processor ID
     /*! This method returns the ID of the processor (with respect to the Swarm's main processor group)
         which handles the parcel whose index is n
         
         \param n the index of the parcel whose ownership is to be determined
     
         \return the index of the processor in the Swarm's main processor group
     */    
     int belongs(const int n) const;
     
     /// universal constructor routine
     /*!
        This is an internal method used by all the constructors to initialize the Swarm.
     */
     void setup( const Parcel &p, ProcessGrp* pgrp, int n, int r);

     /// makes an id string for a processor group
     /*! This method makes a string that can be used as a tag 
         for a processing group.
         
         \param tag a string used as the starting point
         \param i an integer that is to be appended to the tag
         
         \return the tag string
     */    
     std::string make_proc_id ( const std::string& tag, int i ) const;
     
     /// grow the number of parcels this Swarm can hold by N
     /*!
         This method check to see if the internal Parcle information arrays
         can hold another N parcels. If they can, no action is taken.
         If not, then new, larger arrays are created that replace
         the current arrays.
         
         \param n the number of Parcels to be added to this Swarm.

     */
     void grow( int n=1 );
     
     /// Internally rearrange parcels 
     /*! This method rearranged the parcels internally to make
         it more efficient for the parcel integrator to ignore
         parcels which should not be traced.
        
         \return the number of valid parcels to be traced
     */
     int arrange();

     /// (stub) copy constructor
     /*! Note: Copy construction is not permitted.  The Swarm has a potentially
         large collection of processors and Parcels, and there is no good
         purpose in trying to make copies for pass-by-value arguments.
         Any attempt to do so is almost certainly a coding error, so
         we simply make the operation illegal.
     */    
     Swarm(const Swarm&) {};
     
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
