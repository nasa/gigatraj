/* todo
   -  force numbers of parcels to be unsigned long ints-- or better, size_t?
     since 2^32 = 4,294,967,296

   - add the advance() function
   
   - add code to constructor to manipulate met data source 

*/

#ifndef GIGATRAJ_FLOCK_H
#define GIGATRAJ_FLOCK_H

#include <vector>

#include "gigatraj/gigatraj.hh"
#include "gigatraj/Parcel.hh"
#include "gigatraj/PlanetNav.hh"
#include "gigatraj/MetData.hh"
#include "gigatraj/Integrator.hh"
#include "gigatraj/ProcessGrp.hh"

namespace gigatraj {

/*!  \ingroup parcels

\brief parallelizable collection of Parcels.  

The Flock class is a container class that holds Parcels.
It is iterable, and it also accomodates parallel processing
in a way that is transparent to the user.

The idea is that you would put your Parcels into a Flock,
and then iterate over the member Parcels of the Flock to carry 
out some kind of operation on each one, such as tracing its
trajectory or retrieving meteorological field values
at its location.

In a serial processing environment, this would be straighforward
to implement with any generic C++ container class, or even with
a simple array of parcels.  But in a parallel processing environment,
things are not so simple.  We certainly would not want every 
processor to apply an operation to every parcel; rather, we 
want to divide up the Parcels among the processors, with each processor
handling its share.  It would be good to do this automatically
somehow as well, with no explicit calls to inter-process communication
routines to clutter up the science logic of the program.
Moreover, in some situations such as file input,
we want an operation carried out on the rank-0 "root" processor 
and then hand the resulting parcel off to one of the processors.
Or conversely, we want the root processor to obtain each parcel
from the various processors and conduct some operation on it 
such as file output.

The Flock class provides these capabilities.  Using the 
parallel-processing abilities that have been abstracted into
the ProcessGrp class, the Flock class automatically assigns Parcels
to available processors.  An iterator is provided that lets each processor
loop in parallel through its share of the Parcels only.  Special class methods
exist as well to permit centralized serial processing of each Parcel.

In addition, the Flock can set some processors aside for
handling meteorological data instead of tracing trajectories.
This can be useful when you do not wish to hold a copies of 
large, high-resolution arrays of met data in every processor.
Instead, some number of processors hold the met data, 
and the tracing processors communicate with the met processors
to get their wind values.  Again, this should work transparently
with no additional programming effort required.  The Flock simply 
takes care of it.

Thus, the Flock class enables a programmer to 
write a custom massively-parallel trajectory model 
in a straighforward way, without having to worry (much)
about the mechanics of communication between parallel
processors.


*/

class Flock {
      

   public:
    
     /// An exception for some kind of problem with the Flock
     class badgeneration {};

     /// An exception for a bad number of parcels
     class badparcelcount {};

     /// An exception for a bad index into the parcels
     class badparcelindex {};

     /// An exception for attempting to use a met-processor as a parcel-tracer
     class badMetIsNotTracer {};

     /// The basic constructor
     /*! This is a Flock constructor that assigns N parcels. Note: this will result
         in a Flock that uses the SerialGrp class to simulate/bypass parallel
         processing, and the Parcel class as its parcel.
         
         A serial processing process group will automatically be assigned to the Flock.

         Note that the meteorological data source shared by all the parcels
         will have its process group replaced by a process (sub)group that is
         associated with this Flock. The original process group will be
         remembered and restored when this Flock is destroyed. In this way, PGroups
         are managed automatically in most common use cases. But when using multiple
         Parcel containers simultaneously, pgroups must be managed carefully with the
         getPGroup(), setPGroup(), rememberPGroup(), and restorePGroup() methods.  
      
         
         \param n  the number of parcels to put into the flock initially. 
     */
     Flock( int n=0 );
     
     /// parallel-processing constructor
     /*! This Flock constructor distributes the N parcels among
      potentially several processors, using a ProcessGrp object.
      Note: this will result in a Flock that uses the Parcel class 
      as its parcel.
      
         Note that the meteorological data source shared by all the parcels
         will have its process group replaced by a process (sub)group that is
         associated with this Flock. The original process group will be
         remembered and restored when this Flock is destroyed. In this way, PGroups
         are managed automatically in most common use cases. But when using multiple
         Parcel containers simultaneously, pgroups must be managed carefully with the
         getPGroup(), setPGroup(), rememberPGroup(), and restorePGroup() methods.  
      
         \param pgrp a process-group object that is used for parallel processing
         \param n    the number of parcels to put into the flock initially. Note that 
                     this is used only by the root processor in the process-group; all 
                     other processors receive their Flock size from the root processor.  
         \param r    the ratio of meteorological-data processors to parcel-tracing processors.
                     (For example, if r=3 then there will be one met processor for
                     every 3 parcel-tracing processors) 
                    
     */
     Flock( ProcessGrp* pgrp, int n=0, int r=0);
     
     /// Parcel-initializing constructor
     /*! This Flock constructor assigns N parcels by copying a user-supplied Parcel object. 
         Note: this will result in a Flock that uses the SerialGrp class to simulate/bypass parallel
         processing.
         
         A serial processing process group will automatically be assigned to the Flock.

         Note that the meteorological data source shared by all the parcels
         will have its process group replaced by a process (sub)group that is
         associated with this Flock. The original process group will be
         remembered and restored when this Flock is destroyed. In this way, PGroups
         are managed automatically in most common use cases. But when using multiple
         Parcel containers simultaneously, pgroups must be managed carefully with the
         getPGroup(), setPGroup(), rememberPGroup(), and restorePGroup() methods.  


         \param p the input parcel object with whose copies this Flock will be populated
         \param n  the number of parcels to put into the flock initially
     */
     Flock( const Parcel &p, int n=0);
    
     /// Parcel-initializing constructor for parallel processing 
     /*! This flock constructor assigns N parcels by copying a user-supplied Parcel object.  
         It distributes the N parcels among
         potentially several processors, using a ProcessGrp object.

         Note that the meteorological data source shared by all the parcels
         will have its process group replaced by a process (sub)group that is
         associated with this Flock. The original process group will be
         remembered and restored when this Flock is destroyed. In this way, PGroups
         are managed automatically in most common use cases. But when using multiple
         Parcel containers simultaneously, pgroups must be managed carefully with the
         getPGroup(), setPGroup(), rememberPGroup(), and restorePGroup() methods.  


         \param p the input parcel object with whose copies this Flock will be populated
         \param pgrp a process-group object that is used for parallel processing
         \param n  the number of parcels to put into the flock initially. Note that 
                     this is used only by the root processor in the process-group; all 
                     other processors receive their Flock size from the root processor. 
         \param r  the ratio of meteorological-data processors ot tracing processors.
                  (For example, if r=3 then there will be one met processor for
                  every 3 parcel-tracing processors) 
                    
     */
     Flock( const Parcel &p, ProcessGrp* pgrp, int n=0, int r=0);
     

     /// The destructor
     /*! 
        This is the destructor method for Flock objects.
        
        (This has the side effect of restoring the process group
        used by the meteorological data source shared by all Parcels.)
        
     */
     ~Flock();


     /// returns whether the current processor is the root processor
     /*! This method determines whether the current processor 
         is the root processor for this flock. In a multi-processing environment,
         it can be useful to carry out certain operations only on
         the root processor (file I/O, for example). This function 
         permits that.
         
         \return true if this is the root processor of the Flock's process group.
     
     */
     bool is_root() const;

     /// returns the process group id     
     inline int proc_id() const 
     {
         return pgroup->id();
     }
     /// returns the ID of the root processor of this process group 
     inline int root_id() const 
     {
         return pgroup->root_id();
     }
     /// returns the number of proessors in this process group
     inline int psize() const 
     {
         return pgroup->size();
     }
     /// returns whether this processor belongs to the process group
     inline int pbelongs() const 
     {
         return pgroup->belongs();
     }
     /// returns what kind of processor this is 
     inline int ptype() const 
     {
         return pgroup->type();
     }

     /// returns the met processing (sub)group that should be used by this Flock instance 
     /*!  This method returns the process (sub)group that will be used by 
          the meteorological data source as this instance of the Flock does its work.

          When a Flock is created, it has a process group that is divided into
          subgroups, one for each collection of Parcels handled by a given processor
          running an instance of the Flock. The meteorological data source that
          is shared by all of the Parcels running on that processor, will have
          its process group set to one of those process subgroups created by the Flock.
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
     
     /// sets the met processing group that should be used by this Flock instance 
     /*! This method sets the process group that will be used by 
          the meteorological data source as this instance of the Flock does its work.

          When a Flock is created, it has a process group that is divided into
          subgroups, one for each collection of Parcels handled by a given processor
          running an instance of the Flock. The meteorological data source that
          is shared by all of the Parcels running on that processor, will have
          its process group set to one of those process subgroups created by the Flock.
          In this way, meteorological data-handling can be done in a parallel
          processing context. But the process group that the meteorological data source
          had before the flock was created, should be remembered and restored
          when the flock is destroyed. 

        \param pg a pointer to the process group to be used
        \param id the processor ID within process group pg, that indicates a dedicated meteorological data processor.  
                   An ID of -1 indicates that no dedicated met processor is to be used.
  
     */
     void setPGroup( ProcessGrp *pg, int id = -1 );
     
     /// returns the met processing group that should be restored when this Flock instance is destroyed
     /*!  This method returns the process group that was used by 
          the meteorological data source before this instance of the Flock was created.

          When a Flock is created, it has a process group that is divided into
          subgroups, one for each collection of Parcels handled by a given processor
          running an instance of the Flock. The meteorological data source that
          is shared by all of the Parcels running on that processor, will have
          its process group set to one of those process subgroups created by the Flock.
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

     /// sets the met processing group that should be restored when this Flock instance is destroyed
     /*! This method sets the process group that is to be used by 
          the meteorological data source after this instance of the Flock is destroyed.

          When a Flock is created, it has a process group that is divided into
          subgroups, one for each collection of Parcels handled by a given processor
          running an instance of the Flock. The meteorological data source that
          is shared by all of the Parcels running on that processor, will have
          its process group set to one of those process subgroups created by the Flock.
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
     
     /// restores the met processing group to the one that should be used after the Flock instance is destroyed
     /*! This method sets the processor group of the meteorologicla data source 
         to the one that should be used after this Flock instance is destroyed.
         
          When a Flock is created, it has a process group that is divided into
          subgroups, one for each collection of Parcels handled by a given processor
          running an instance of the Flock. The meteorological data source that
          is shared by all of the Parcels running on that processor, will have
          its process group set to one of those process subgroups created by the Flock.
          In this way, meteorological data-handling can be done in a parallel
          processing context. But the process group that the meteorological data source
          had before the flock was created, should be remembered and restored
          when the flock is destroyed. 
     
     */
     void restorePGroup();

     /// sets the met processing group to the one that should be used by this flock instance
     /*! This method sets the processor group of the meteorologicla data source 
         to the one that should be used by this Flock instance.
         
          When a Flock is created, it has a process group that is divided into
          subgroups, one for each collection of Parcels handled by a given processor
          running an instance of the Flock. The meteorological data source that
          is shared by all of the Parcels running on that processor, will have
          its process group set to one of those process subgroups created by the Flock.
          In this way, meteorological data-handling can be done in a parallel
          processing context. But the process group that the meteorological data source
          had before the flock was created, should be remembered and restored
          when the flock is destroyed. 
     
     */
     void unRestorePGroup();



     class iterator;
     friend class iterator;
     /*!
     \brief iterator for the Flock class.

     The iterator for this Parcel container class.

     In a serial-processing environment this class functions
     as a conventional iterator, cycling through every Parcel in the
     Flock. But in a parallel-processing environment, this class
     runs on each processor in such a way that it
     iterates only over the Parcels within the flock that are assigned to
     that processor.
     
     In other words, when multiprocessing this iterator is used to access only those parcels
     that belong to the current process. Thus, when a Flock
     is being processed across multiple processors, each
     processor is iterating over its own parcels only, 
     but from the viewpoint of the programmer it appears
     to be iteratating over all parcels in the flock.  The multi-processing
     aspect of the loop becomes a negligible detail.
     
     If some of the processors are dedicated to reading meteorological data,
     they must not try to trace parcels inside the loop.  This, too, is
     handled automatically.  See the Flock::begin() and Flock::end() methods,
     below.

     */
     class iterator  {
        public:
           
           /// default constructor for the Flock::iterator
           /*! This is the default constructor for the Flock::iterator class,
               and the version that is normally used.
               Using this version of the constructor requires initialization
               with  Flock::begin() or Flock::end(). 
           */
           iterator();

           /// alternate constructor for the Flock::iterator
           /*! This special version of the Flock::iterator class allows for initialization to a 
               user-specified Parcel in a user-specified Flock.

               \param init which of the local processor's parcels to point to
               \param flk the Flock for which this is an iterator
           */
           iterator(int init, Flock *flk);
           
           /// operator * override
           /*! This method overrides the * operator, permitting de-referencing the iterator
               to obtain the current Parcel.
               
               \return the current Parcel on this processor
           */    
           Parcel& operator*() const;

           /// operator -> override
           /*! This method overrides the -> operator, permitting de-referencing the iterator
               to obtain the current Parcel.
               
               \return the current Parcel on this processor
           */    
           Parcel* operator->() const;
           
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
               to the next Parcel in the Flock.
           */    
           iterator& operator++(int n);
           
           /// operator ++ override 
           /*! This method overrides the ++ prefix operator, permitting advancing
               to the next Parcel in the Flock.
           */    
           iterator& operator++();
           
           /// returns the index that corresponds to the iterator
           /*! This method returns an index into the Flock that corresponds to
               the iterator.
             
               For serial processing applicatins, this is simply the iteration number.
               But for multi-processing applications, where the local-processor iteration
               number does not correspond to the global index into the flock container,
               this provides a useful handle into the global context of a given Parcel.
               
               \return the index of this iterator's Parcel in the Flock. If the 
               iterator corresponds to no Parcel (e.g., if this is a met data processor), 
               then -1 is returned.
               
           */    
           int index() const;
     
        protected:
           friend class Flock;
           
           /// initializes the iterator
           /*! This allows the Flock to initialize an existing 
               Flock::Iter object

               \param init which of the local processor's parcels to point to
               \param flk the Flock for which this is an iterator
           */
           void set(int init, Flock *flk);
           
     
        private:
           // which parcel on this processor is the currently-indexed?
           int my_parcel;
           // which Flock is this an iterator for?
           Flock* my_flock;
     
     };

     /// Type Iter is the iterator for the Flock container
     /*!
         With the Flock::iterator class defined to iterate over elements
         in the Flock class (potentially when those elements are distributed
         among a number of processors run in parallel), the Iter type
         is defined as a convenient shorthand for Flock::iterator.
     */     
     typedef Flock::iterator Iter;
     
     /// returns the beginning of the flock
     /*!
         This method returns ana iterator that is initialized to the first
         Parcel in the Flock that is assigned to the current processor.
         In single-processing environments, this is simple the first element of
         the Flock. In multiprocessing environments, each processor has its
         own subset of Parcels in the Flock, and this method returns the first
         of the subset that is assgined to the processing it is running on.
         
         \return the Iter that is initialized to the first Parcel in the Flock that is to be dealt with. 
     */
     Iter begin();

     /// returns the end of the flock
     /*!
         This method returns ana iterator that is initialized to one past the last
         Parcel in the Flock that is assigned to the current processor.
         In single-processing environments, this is simple the last element of
         the Flock, plus one. In multiprocessing environments, each processor has its
         own subset of Parcels in the Flock, and this method returns one past the last
         of the subset that is assgined to the processing it is running on.
         
         \return the Iter that is initialized to just after the last Parcel in the Flock that is to be dealt with. 
     */
     Iter end();

     /// begins the use of the flock in an iteration loop
     /*! This method performs initial setup for iterating over Parcels in a flock.
         For example, in a multi-processor environment, it initializes
         the meteorological source.
         
         Ordinarily, this method should not be called directly
         from your programs, but it is available in case you need
         it for something unusual.
     */    
     void debut(); 

     /// ends the use of the flock in an iteration loop
     /*! This method performs final shutdown after iterating over Parcels in a flock.
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
         one flock.  If you are doing parallel processsing, with
         different parallel processes tracing distinct batches of 
         Parcels, then each process needs to set the navigation object.
        
         \param newnav the new planetary navigation object
        
     */   
     void setNav( PlanetNav& newnav );
     
     /// Returns the current planetary navigation object
     /*! This method returns a pointer to the naviagational object
         currently being used in this Flock. Unless has been set to
         something else, this will be the default spherical Earth navigation
         object.
     
          \return the current navigational object
     */
     PlanetNav* getNav();


     /// changes the meteorological data source
     /*!
        This method changes the meteorological data source used to 
        obtain the winds used to trace the path of Parcels in the Flock.
        
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


     /// returns the total number of parcels in the flock
     /*! This method returns the total number of Parcels in the Flock.

         Note that this is the total number of all Parcels summed
         across all processors, not the number of Parcles handled by any 
         one processor.
         
         \return the number of Parcels in the Flock
     */
     int size();
     
     /// replaces a specified parcel in this flock with a new parcel
     /*! This method assigns a Parcel to a specific element in the Flock.
     
          \param n the index of the parcel to be replaced.  (This is relative
                  to the entire flock, not just one processor's subset.)
          \param p the parcel of which a copy will be put into the Flock. 
          \param mode if mode==0 (the default), then the input parcel p is taken
                     from the root processor's copy only.  In a multiprocrssor
                     environment, this will incur interprocrssor communication.
                     If mode==1, then the processor that owns the parcel 
                     will uses its own local value for the input parcel.
     */   
     void set( const int n, const Parcel& p, const int mode=0);
     
     ///  returns a a pointer to a parcel from this Flock
     /*! This method returns a pointer to a specific Parcel in the Flock.
     
          \param n the index of the parcel to be replaced.  (This is relative
                  to the entire flock, not just one processor's subset.)
                  
          \param mode if mode==0 (the default), then the parcel gets propagated to 
                     every processor, so that on every processor the 
                     return value has the correct value.  This may take some time.
                     
                     If mode==1, then only the root processor of the Flock's
                     main processor group will return a valid value.
                     All other processors return a NULLPTR.
                     Regardless of mode setting, all met-reading processors
                     will return a NULLPTR pointer from this method.

          \return a pointer to the desired Parcel. or NULLPTR (see mode, above).
                  If not NULLPTR, and the Parcel is local to this processor,
                  then it points to the actual Parcel in the Flock.
                  If not NULLPTR, and the Parcel is not local to this processor,
                  then it points to a copy of the Parcel that has been obtained from
                  the Parcel's owner processor. (The copy is owned by the Flock
                  and should NOT be deleted).
                 
                  
     */
     Parcel* parcel( const int n, const int mode=0 ) const;
     
     /// returns a parcel from this Flock 
     /*! This method returns a reference to a specific Parcel in the Flock.
         It exists for convenience only, and merely calls the parcel() method.
     
          \param n the index of the parcel to be replaced.  (This is relative
                  to the entire flock, not just one processor's subset.)
                  
          \param mode if mode==0 (the default), then the parcel gets propagated to 
                     every processor, so that on every processor the 
                     return value has the correct value.  This may take some time.
                     If mode==1, then only the root processor of the Flock's
                     main processor group will return a valid value.
                     All other processors throw as badparcelindex error.
                     Regardless of mode setting, all met-reading processors
                     will throw a badparcelindex error from this method.

          \return a reference to the desired Parcel. Note that this reference points to 
                  a member of the Flock and thus should never be deleted!.
                           
     */
     Parcel& get( const int n, const int mode=0 ) const;
     
     /// operator [] override
     /*! This method overrides the [] operator, providing a convenient notation
         for accessing individual Parcels within the Flock container, as
         if they were elements in an Array.
         This method is equivalent to get(n)
          
          \param n the index of the parcel to be replaced.  (This is relative
                  to the entire flock, not just one processor's subset.)

          \return a reference to the desired Parcel. Note that this reference points to 
                  a member of the Flock and thus should never be deleted!.
     */
     Parcel& operator[]( int n );
     

     /// adds a single parcel to the flock
     /*! This method adds a new Parcel to the end of the Flock.

          WARNING: on a multi-processing system, push_back() will
          most likely not add the parcel to the end.  
          
          \param p the parcel to be added ot the Flock

          \param mode if 0, then the value will be taken from the root processor in a multiprocessing system
                      if 1, then the value will be taken from the local processor.

     */
     void add( const Parcel& p, const int mode=0 );
     
     /// advance the parcels by one time step
     /*! This methods advances the parcels in the Flock in time.
      
         \param dt the delta-time over which the parcel is to advance
         
         \return always returns zero.
         
     */
     int advance( double dt );
     
     /// count the number of Parcels with one or more ParcelFlag values set. 
     /*! This methods counts the number of parcels in the Flock 
         whose flags settings match a query.
      
         \param flgs a bitwise ORing of possible ParcelFlag settings.
         \param negate set to true if a count is desired of the Parcels whose flags are NOT set
         
         \return the count.
         
     */
     int countFlags( ParcelFlag flgs=0, bool negate=false );
     
     /// count the number of Parcels with one or more ParcelStatus values set. 
     /*! This methods counts the number of parcels in the Flock 
         whose status settings match a query.
      
         \param stat a bitwise ORing of possible ParcelStatus settings.
         \param negate set to true if a count is desired of the Parcels whose status settings are NOT set
         
         \return the count.
         
     */
     int countStatus( ParcelStatus stat=0, bool negate=false );
     
     /// clears the Flock so that is has no Parcels
     /*! This method empties the Flock of its Parcels.
     
         As a side effect, it also restores the Pgroup of the 
         current Parcels' met data source, and it destroyes its
         internal PGroups.
     
     */
     void clear();
     
     /// allocates space in the Flock for Parcels
     /*! This method clears the flock and then
         allocates the required number of Parcels.
         
         In a multiprocessing environment, it also
         allocates Parcles to each processor and sets up PGroups to
         handle them.
         
         \param n the number of parcels
         \param pgrp a process-group object that is used for parallel processing. If NULLPTR,
                  then the Flock's current PGroup is used.
         \param p a pointer to a sample Parcel used to populate the Flock. If NULLPTR,
                  then a standard Parcel is created for this.
         \param r  the ratio of meteorological-data processors ot tracing processors.
                  (For example, if r=3 then there will be one met processor for
                  every 3 parcel-tracing processors) 
     */
     void allocate( int n, ProcessGrp* pgrp, const Parcel* p=NULLPTR, int r=0 );     
     
     /// synchronizes the Flock's processors
     /*! This method synchronizes the Flock's processors.
     */
     void sync();
     
     /// synchronizes the Flock's processors
     /*! This method synchronizes the Flock's processors.
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
     /*! This method prints out some summary information about the Flock.
         It is used for debugging.
     */    
     void dump() const;

         
     /// processor ID of a Flock's Parcel
     /*! This method returns the ID of the processor (with respect to the Flock's main processor group)
         which handles the parcel whose index is n
         
         \param n the index of the parcel whose ownership is to be determined
     
         \return the index of the processor in the Flock's main processor group
     */    
     int belongs(const int n) const;

   private:
   
     /// the number of parcels in this flock
     /*! The total number of parcels in the Flock summed over
         all processors.
     */    
     int num_parcels_total;
     
     /// the process group object
     /*! The process group object that handles multiprocessing for
         this Flock. This enables use of parallel processing, if desired.
         
         In a parallel processing environment, 
         this process group will be split into one or more subgroups to handle
         sets of the Flock's Parcels, with each subgroup having possibly one 
         single-processor subgroup
         to handle meteorological data.
     */    
     ProcessGrp *pgroup;
     
     /// a sample parcel that we can use for references
     Parcel *exemplar;
     
     /// The met data source's pgroup that it had before this Flock was created
     ProcessGrp *preserved_met_pgroup;
     /// The met data source's processor id that it had before this Flock was created
     int preserved_met_pid;
     /// This flock's instance's pgroup, which is what the met data source's pgroup is set to when using this Flock
     ProcessGrp *preserved_flock_pgroup;
     /// This flock's instance's processor id, which is what the met data source's pgroup is set to when using this Flock
     int preserved_flock_pid;
     
     /// holds the processor groups for this flock
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
         This holds the starting index of this Flock's Parcels that are handled by
         the current local processor.
     */
     int my_parcel_start;

     /// the number of parcels handled by this process
     /*!
        This holds the number of this Flock's Parcels that are handled by the
        current local processor.
     */
     int my_num_parcels;

     /// the list of this processor'e parcels 
     /*!
        This vector hold the Parcels that are being traced on this processor.
        
        In a multi-processor environment handlig huge number of parcels, 
        we do not need or want to keep a copy of each Parcel on each processor.
        So this vector holds only those Parcles that have been assigned to the
        local processor. 
        
     */
     std::vector<Parcel*>  parcels;
     
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
     
     /// universal constructor routine
     /*!
        This is an internal method used by all the constructors to initialize the Flock.
        
        It allocates Parcels to each processor and sets up Process groups to handle them.
        
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

     /// (stub) copy constructor
     /*! Note: Copy construction is not permitted.  The Flock has a potentially
         large collection of processors and Parcels, and there is no good
         purpose in trying to make copies for pass-by-value arguments.
         Any attempt to do so is almost certainly a coding error, so
         we simply make the operation illegal.
     */    
     Flock(const Flock&) {};
     
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
