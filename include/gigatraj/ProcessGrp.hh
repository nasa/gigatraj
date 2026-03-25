#ifndef GIGATRAJ_PROCESSGRP_H
#define GIGATRAJ_PROCESSGRP_H

#include "gigatraj/gigatraj.hh"
#include "gigatraj/RandomSrc.hh"

#include <vector>

namespace gigatraj {

/*!  \defgroup pgroup  Process Groups

The gigatraj model may be used for either serial or parallel processing.
We assume that the computer system on which the model is run consists
of one or more independent processors.  The task of tracing parcel trajectories
is distributed among theses processors.  Because many meteorological data sets
are of high spatial resolution and thus require much memory, gigatraj
also allows some processors to be dedicated to handing these data, in order
to reduce the memory required by a model run.

The processors available for a model run are divided up into
groups.  Each processor in a group may take on one or more of three
possible roles:  tracing trajectories, handling meteorological data, and
coordinating processors.  (Within a group, at most one processor can take on
that last duty.)  Processor groups may be split into subgroups, each of which then
has a coordinating processes.   Data of various types may be exchanged
between processors

The Process Groups classes provide
a uniform interface for dealing with these processor management and communication functions.



*/


/*! @name Process Group Flags
    In gigatraj, parallel processing is done on the basis of groups of processors.
    (See the documentation for the ProcessGrp class.).  These PG_* flags
    are bitwise values that 
    set certain characteristics for how those groups behave.  They are
    defined here in order to make them global within the gigatraj namespace.
    
*/
///@{
/// Process group flag: Be strict about requested group sizes; fail if there are fewer processors than requested
const int PG_STRICT   = 0x0001;
/// Process group flag: Subgroups should inherit the current group's root
const int PG_KEEPROOT = 0x0002;
///@}


/*! @name Interprocess Communications Commands
    In gigatraj, parallel processes communicate with each other
    by sending commands. These are integer values that indicate
    some action that should be taken by the other processor.
*/
//@{
/// Interprocess Communications Commands: The recieving processor is advised that the sending processor is done and will issue no further requests.
static const int PGR_CMD_DONE = 0;
//@}

/*! @name Interprocess Communications Status
    In gigatraj, parallel processes communicate with each other
    by sending commands. These are integer values that indicate
    the status of some request undertaken here for the other processor.
*/
//@{
/// Interprocess Communications Status: The recieving processor is advised that the sending processor was successful.
static const int PGR_STATUS_OK = 0;
/// Interprocess Communications Status: The recieving processor is advised that the sending processor was unsuccessful.
static const int PGR_STATUS_FAILED = 0;
//@}

/*! @name Interprocess Communications Tags
    In gigatraj, parallel processes communicate with each other
    by exchanging information. Some parallelism frameworks use a
    tag to inter=process communications, to lessen the change that
    messages will become unintentionally unsynchronized.
    Process Group Tags are integer values that are used for this purpose.
*/
//@{
/// Interprocess Communications Tags: "This is a Request"
static const int PGR_TAG_REQ = 0;
/// Interprocess Communications Tags: "Current Status"
static const int PGR_TAG_STATUS = 5;
/// Interprocess Communications Tags: "Transfer a quantity name"
static const int PGR_TAG_QUANT = 10;
/// Interprocess Communications Tags: "Transfer an internal model time"
static const int PGR_TAG_TIME = 20;
/// Interprocess Communications Tags: "Transfer a calendar time"
static const int PGR_TAG_CTIME = 30;
/// Interprocess Communications Tags: "Transfer bad/fill value"
static const int PGR_TAG_BADVAL = 35;
/// Interprocess Communications Tags: "random number"
static const int PGR_TAG_RND = 40;
//@}
   
/*!

\ingroup pgroup
\brief an abstract class for managing process groups

The ProcessGrp class serves
as the abstract class upon which an interface is defined for dealing with
(i.e. defining and communicating within) groups of processors.

The underlying model is that there exists a collection of processors.  
These may be divided into groups which can then be set to work on
various tasks.  There is a default group to which all the processors
belong initially.  Each group has a "root" processor; the root can tell the other
processors what to do, and it can divide them up into groups.
Every processor has a role assigned to it.  The root processor has the role
of Coordinator.  Of course another key role is to trace parcel
trajectories: the Tracer role.  The Flock class, which holds a collection of parcels, 
knows about process groups and automatically divides its parcels up among the available 
Tracer processors. Similarly for the Swarm class, which appears to hold Parcels (but stores
the information dfferently internally).
In some cases, the meteorological data source consists of
large data arrays of such fine resolution
that the cost is prohibitive for each processor to hold its own copy of the grids in memory.
Thus, another processor role has been defined to handle these cases: the MetReader.
MetReader processors are dedicated to reading gridded meteorological data.  When a
Tracer processor needs to interpolate a field, it sends its desired grid indices
to a met processor, which returns the field values at those grid points.  The
Tracer processor then uses those values in the interpolation calculation.
In some implementations, this division of labor imposes a 
penalty in the form of a vastly increased
number of messages passing back and forth between processors, so it should be used only
when needed.  A processor may therefore have the role of Tracer, MetReader, Coordinator,
or All (which performs all of the roles).

Each program should have exactly one object of a ProcessGrp subclass.  In a parallel
processing environment, this object will contain all of the information needed
to distinguish one processor from another, to determine processor roles, and to
facilitiate inter-processor communication.  
Consequently, an object of a ProcessGrp subclass will be instantiated differently 
on different processors.   After creating ProcessGrp object, the calling code should 
have the object
query itself to find out what its role is and then take appropriate action
depending on that role.  In serial environments, the ProcessGrp object
will actually do very little.  But having it enables the library to handle both
the serial and parallel cases.

*/

class ProcessGrp {

   public:

      /// The role that a given process plays in the model
      enum ProcessRole {
          PGrpRole_All,        // does everything 
          PGrpRole_Tracer,     // traces parcel trajectories
          PGrpRole_MetReader,  // reads meteorological data
          PGrpRole_Coordinator, // coordinates other processes
          PGrpRole_Unknown     // 
      };

      /// An exception for trying to use a bad number of processors
      class badgroupsize {};
      
      /// An exception for trying to use a bad processor ID
      class badprocessor {};
      
      /// An exception for some kind of parallelism failure
      class badparallelism {};

      /// An exception for some other kind of data transfer failure
      class badtransfer {};
      
      /// default contructor
      /*! This is the default constructor for the ProcessGrp class.
      */
      ProcessGrp();
       
      /// Copy-constructor
      /*! This is the copy-constructor for the SerialGrp class
      */ 
      ProcessGrp(const ProcessGrp&);
      
      /// destructor
      /*! This is the  destructor for the ProcessGrp base class.
      */
      virtual ~ProcessGrp();

      /// returns the number of processors in this group
      /*!  This function returns the number of processors that belong to
           the current processor's group.
       
      
      */
      int size() const;
      
      /// determines whether the current processor is root of its group
      /*! This function answers the question, is this processor the
          coordinating processor, or root, of its group.
          
          \return 1 if this processor is the root, 0 otherwize.
      */    
      virtual int is_root() const = 0;
      

      /// returns a group ID for this group
      /*! This function returns an integer that identifies this processor.
      
          \return the ID within the current group, for this processor
      */    
      virtual int id() const = 0;
     
      /// returns the ID of the current group
      /*! This function returns an ID string for the current group.
          Note that this is merely an identifying tag (and may in fact
          be partially derived from the object's tag).
          
          \return the group ID
      */    
      virtual std::string group_id() const = 0;      

      /// returns the ID of this group's root  processor
      /*! This function returns an integer that identifies the coordinating, or root, processor
          within the current processor's group.  
          
          \return the ID within the current group, of the root processor.
                  If this processor is not a member of a group, -1 is returned.
      */
      virtual int root_id() const = 0;

      /// Determines group membership
      /*! This function finds out whether it is a member of a processor group.
        
          \return 1 if this processor is a member of a group, 0 otherwise
      */
      virtual int belongs() const = 0;
      
      /// Returns the type of this processor
      /*!  This function returns the type or role that this processor plays within
           its group.
           
           \return a ProcessRole value that indicates what role this processor is to play
      */
      ProcessRole type() const;

      /// Sets the type of a processor
      /*! Sets the type or role that a processor is to play within its group.
      
           \param id the integer identifier of the particular processor within
                 this group whose type is to be set. If -1, then every progrssor in 
                 the group is set to this type.
           \param type the role to be assigned to that processor 
      */     
      virtual void setType(int id, ProcessRole type) = 0;

      /// returns a tag string
      /*! returns an arbitrary string with which this process gorup has been tagged
      
          \return the tag string
      */
      std::string tag() const;    

      /// sets a tag string
      /*! tags this process groupo with an arbitrary tag string 
      
          \param tagstr the tag string
      */
      void tag( const std::string& tagstr );


      /// copies a process group object
      /*! This function copies a process group.
      
          \return a pointer to the new copy
      */
      virtual ProcessGrp* copy() const = 0;

      /// synchronizes with the rest of the group
      /*! This function causes the current processor to pause until
          the other processors in its group are caught up with it.
      */
      virtual void sync() const = 0;

      /// synchronizes with the rest of the group
      /*! This function causes the current processor to pause until
          the other processors in its group are caught up with it.
          
          \param msg a string message to be printed, to aid in debugging
      */
      virtual void sync( const std::string& msg ) const = 0;

      /// synchronizes with a particular processor
      /*! This function causes the current processor to pause until
          another processors in its group is caught up with it.
          
          \param id the ID of the individual processor with which this
                    one is to synchronize.
      */    
      virtual void sync(int id) const = 0;

      /// de-synchronizes with other processors, by introducing a random wait
      /*! This function causes the current processor to pause 
          a random amount of time, to reduce synchronization 
          with other processors. This routine has no effect if there
          is no multiprocessing; it does not wait at all.
          
          \param interval an interval, in seconds, that is the maximum 
                  time for this processor to wait before proceeding. 
                  The actual wait time is a uniformaly distributed random
                  number between zero and this time. 
      */    
      virtual void desync(int interval) = 0;
      
      /// counts the processors with a given type (ProcessRole)
      /*! This method counts how many processors are of a given ProcessRole.
          This can be used by calling routines to stagger accesses to online
          data sources to ensure that they are not overwhelmed 
          
          \return the number of processors with the given ProcessRole
      */
      int countType( ProcessRole type ) const;

      /// shuts down processing
      /*! This function whatever serial or parallel processing system is in use,
          performing any cleanup activities that are necessary.
      */    
      virtual void shutdown() = 0;

      /// creates a new group from the current group
      /*! This function creates a new group from the current group. the processors
          for the new subgroup are selected by specifying the processor ID
          of the first processor, and the number of processors.  
      
          \return a pointer to the new group object.
          \param size the desired number of processors in the new group
          \param flags  process group flags (PG_* values) or'd together to specify how the group is to be created.
          \param offset the first processor of the current group that is to be part of the new group
      */      
      virtual ProcessGrp* subgroup(int size=0, int flags=0, int offset=0 ) = 0;

      /// creates a subgroup from the current group
      /*! This function creates a subgroup from the current group, using a list 
          of desired members of the new group
          
          \return a pointer to the new group object.
          \param size the desired number of processors in the first new group
          \param list an array of processor IDs to be used to create the new group. 
                 The first ID will be the new group's root procesor.
          \param flags process group flags (PG_* values) or'd together to specify how the group is to be created.
      */
      virtual ProcessGrp* subgroup( int size, int *list, int flags=0 )  = 0;       

      /// creates two new groups from the current group
      /*! This function creates two new groups from the current group.
          That processor's ProcessGrp object will receive the reals using its
          \b receive_reals() method.
      
          \param size_a the desired number of processors in the first new group
          \param a the first new group, which is a subset of the current group
          \param b the second new group, which holds all members of the current group which are not in group "a"
          \param flags: process group flags (PG_* values) or'd together to specify how the group is to be created.
      */
      virtual void split( int size_a, ProcessGrp **a, ProcessGrp **b, int flags=0 ) const  = 0;

      /// returns the total number of distinct processors running in parallel
      /*! This method returns the number of processors. In serial processing,
          this is 1. In parallel processing, it is the total number of processes.
          
          Note that this is not the number of prcessors in this process group,
          which is obtained with the size() method, but the total number of
          processors, regardless of membership in a process group.
          
          This method is intended to be useful for determining delay times
          and numers-of-retries, to avoid conflicts in cases where a number of different processors
          may attempt to access the same resource.
      */
      virtual int numberOfProcessors() const = 0;   

      /// generates a random number that is shared throughout the whole group
      /*! This methods produces a uniform (pseudo-)random number that, while
          differing for each invocations and differing between Process Groups,
          is the same within a process group.
          
          \return a random floating-point nunber between 0.0 and 1,0
      */
      virtual real random() const = 0;  
   
      /// sends a set of reals to another processor in this group.
      /*! This function sends a set of reals to another processor in this group

           \param id the ID (with respect to this group) of the other processor
           \param n the number of reals to send
           \param vals an array of reals to be sent
           \param tag an arbitrary number used to label the content in vals
      */
      virtual void send_reals( int id, int n, const real *vals, int tag=0) const = 0;

      /// sends a set of doubles to another processor in this group
      /*! This function sends a set of doubles to another processor in this group
          That processor's ProcessGrp object will receive the doubles using its
          \b receive_doubles() method.

           \param id the ID (with respect to this group) of the other processor
           \param n the number of doubles to send
           \param vals an array of doubles to be sent
           \param tag an arbitrary number used to label the content in vals
      */
      virtual void send_doubles( int id, int n, const double *vals, int tag=0) const = 0;

      /// sends a set of integers to another processor in this group
      /*! This function sends a set of integers to another processor in this group.
          That processor's ProcessGrp object will receive the integers using its
          \b receive_ints() method.

           \param id the ID (with respect to this group) of the other processor
           \param n the number of integers to send
           \param vals an array of integers to be sent
           \param tag an arbitrary number used to label the content in vals
      */
      virtual void send_ints( int id, int n, const int *vals, int tag=0) const = 0;

      /// sends a string to another processor in this group
      /*! This function sends a string to another processor in this group.
          That processor's ProcessGrp object will receive the string using its
          \b receive_string() method.
          
           \param id the ID (with respect to this group) of the other processor
           \param str the string to be sent
           \param tag an arbitrary number used to label the content in vals
      */
      virtual void send_string( int id, const std::string &str, int tag=0) const = 0;

   
      /// receives a set of reals from another processor in this group
      /*! This function receives a set of reals sent from another processor in this group
          using the \b send_reals() method.

           \param id the ID (with respect to this group) of the other processor.
                 If -1, then the values will be received from any processor in this group.
           \param n the number of reals to receive
           \param vals an array of reals to be sent
           \param tag an arbitrary number used to label the content in vals
           \param src returns the ID of the processor which sent the values
      */
      virtual void receive_reals( int id, int n, real *vals, int tag=0, int *src=NULL) const = 0;

      /// receives a set of doubles from another processor in this group
      /*! This function receives a set of doubles sent from another processor in this group
          using the \b send_doubles() method.

           \param id the ID (with respect to this group) of the other processor.
                 If -1, then the values will be received from any processor in this group.
           \param n the number of doubles to receive
           \param vals an array of doubles to be sent
           \param tag an arbitrary number used to label the content in vals
           \param src returns the ID of the processor which sent the values
      */
      virtual void receive_doubles( int id, int n, double *vals, int tag=0, int *src=NULL) const = 0;

      /// receives a set of integers from another processor in this group
      /*! This function receives a set of integers sent from another processor in this group
          using the \b send_ints() method.

           \param id the ID (with respect to this group) of the other processor.
                 If -1, then the values will be received from any processor in this group.
           \param n the number of integers to receive
           \param vals an array of integers to be sent
           \param tag an arbitrary number used to label the content in vals
           \param src returns the ID of the processor which sent the values
      */
      virtual void receive_ints( int id, int n, int *vals, int tag=0, int *src=NULL) const = 0;

      /// receives a string from another processor in this group.
      /*! This function receives a string sent from another processor in this group using 
          the \b send_string() method.

           \param id the ID (with respect to this group) of the other processor
                 If -1, then the values will be received from any processor in this group.
           \param vals the string to receive
           \param tag an arbitrary number used to label the content in vals
           \param src returns the ID of the processor which sent the values
      */
      virtual void receive_string( int id, string *vals, int tag=0, int *src=NULL) const = 0;


   protected:

      /// the function that this processor serves within goigatraj   
      ProcessRole role;

      /// The number of processes in this group
      int num_procs;
      
      /// the id of this processor within the group. A vlaue of -1 means it is undefined or unknown; legitimate values ramge from 0 through (num_procs - 1).
      int my_id;
     
      /// source of random numbers
      RandomSrc *rnd;

      /// a string with which to tag this process group
      std::string mytag;
          
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
