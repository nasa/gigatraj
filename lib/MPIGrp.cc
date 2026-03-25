
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

#include "config.h"
#include <time.h>

#include <stdlib.h>
#include <unistd.h>

#include "gigatraj/MPIGrp.hh"

using namespace gigatraj;

#ifdef USE_MPI

MPIGrp::MPIGrp() : ProcessGrp()
{
   
   comm = MPI_COMM_WORLD;
   
   flags = 0;

   MPI_Comm_group( comm, &group );

   // how many processors in this communicator?
   MPI_Comm_size(comm, &num_procs);
     
   // which one am I?  
   MPI_Comm_rank(comm, &(gigatraj::MPIGrp::my_id));

   // But if we do not have enough entropy to make
   // the processors independent, then cycle through
   // a bunch right now to give them different seeds.
   if ( my_id > 0 ) {
      unsigned int seed = 0;
      for ( int i=1; i <= my_id; i++ ) {
          seed = rnd->raw();
      }
      rnd->seed( &seed );
   }
   
   role = PGrpRole_Unknown; 
   
};

MPIGrp::MPIGrp(int argc, char* argv[])  : ProcessGrp()
{
   // start up MPI
   MPI_Init(&argc, &argv);
   
   
   comm = MPI_COMM_WORLD;
   
   flags = 0;

   MPI_Comm_group( comm, &group );

   // how many processors in this communicator?
   MPI_Comm_size(comm, &num_procs);
     
   // which one am I?  
   MPI_Comm_rank(comm, &(gigatraj::MPIGrp::my_id));
   
}



MPIGrp::MPIGrp( MPI_Comm mpicomm, int free ) : ProcessGrp()
{

   comm = mpicomm;
   
   // free the group but not the comm
   flags = ( free & 1) | 2;

   // get the group associated with this communicator
   MPI_Comm_group( comm, &group );

   // how many processors in this communicator?
   MPI_Group_size(group, &num_procs);
     
   // which one am I?  
   MPI_Group_rank(group, &(gigatraj::MPIGrp::my_id));
   if ( my_id == -1 ) {
      throw (ProcessGrp::badparallelism() );
   }
            
   if ( my_id == MPI_UNDEFINED ) {
      my_id = -1;
   }   
   
};

MPIGrp::MPIGrp( MPI_Comm mpicomm, MPI_Group mpigroup, int free ) : ProcessGrp()
{
   MPI_Group tstgroup;
   int result;

   flags = ( free & 3 );

   comm = mpicomm;

   group = mpigroup;
   
   // how many processors in this communicator?
   (void) MPI_Group_size(group, &num_procs);

   // which one am I?  
   (void) MPI_Group_rank(group, &(gigatraj::MPIGrp::my_id));
   if ( my_id == -1 ) {
      throw (ProcessGrp::badparallelism() );
   }
            
   if ( my_id == MPI_UNDEFINED ) {
      my_id = -1;
   }
   
   if ( my_id >= 0 ) {
      // this processor is part of this group, so we can
      // operate with the communicator 

      // check to ensure that this group belongs to this communicator!
      (void) MPI_Comm_group( comm, &tstgroup);
      (void) MPI_Group_compare( group, tstgroup, &result );
      if ( result != MPI_IDENT ) {
         throw (ProcessGrp::badparallelism());
      }   
      (void) MPI_Group_free(&tstgroup);
   } else {
      // this processor is NOT part of this group, so
      // no further checks can be performed
      
      // and set so that the gorup and communicator are NOT freed on destruction
      flags = 0;

   }   
   
};

MPIGrp::MPIGrp(const MPIGrp& src) : ProcessGrp(src)
{
   
    // every processor gets its rank duplicated
    my_id = src.id();
    // every processor gets its role duplicated
    role = src.type();

    if ( my_id >= 0 ) {

       // make a new communicator, same as the current communicator
       MPI_Comm_dup(src.comm, &comm);

       // get the group associated with this communicator
       MPI_Comm_group( comm, &group );
       
       // these can and should be deleted when this object is deleted
       flags = 0;

    } else {
    
       // this processor does not belong to the communictor,
       // so just copy
       comm = src.comm;
       group = src.group;
    
       // because these were copied, they should not be deleted
       // when this object is deleted
       flags = 3;
    
    }
    num_procs = src.num_procs;
   
}


MPIGrp::~MPIGrp()
{
   // We should perhaps go through the list
   // of children and make them all orphans.
   // But we do not know for sure which of those
   // points still point to valid objects.
   // The caller ought to have called disinherit()
   // for each child object deleted, but we cannot
   // count on that.

   // We will attempt to destroy the communicator and group
   // only if this processor belongs to them
   if ( my_id >= 0 ) {
      if ( flags & 1 ) {
         MPI_Comm_free(&comm);
      }   
      if ( flags & 2 ) {
         MPI_Group_free(&group);
      }   
   };  
   
};

int MPIGrp::is_root() const
{
   return (my_id == 0);
};

int MPIGrp::id() const
{
   return my_id;
};

std::string MPIGrp::group_id() const
{
   std::string result;
   
   // we'll start with the object's own tag
   result = mytag;
   
   return result;
};

int  MPIGrp::root_id() const
{
   return 0;
};

int  MPIGrp::belongs() const
{
   return (my_id >= 0);
};

void MPIGrp::setType(int id, ProcessRole type)
{
   if ( id >= 0 && id < num_procs ) {
      if ( my_id == id ) {
         role = type;
      }
   } else {
      if ( id == -1 ) {
         role = type;
      } else {
         throw (badprocessor());
      }
   }
};

MPIGrp* MPIGrp::copy() const
{
   MPIGrp* newgrp;

   newgrp = new MPIGrp( comm, group, flags );
   newgrp->setType( 0, role );
   
   if ( mytag != "" ) {
      newgrp->tag( mytag + "-copy" );
   }

   return newgrp;
};


void MPIGrp::sync() const
{
   int input = 1;
   int output = 0;

   if ( my_id >= 0 ) {

      //- std::cerr << "MPI Sync rank " << my_id << " in group " << mygroup << "!" << std::endl;
      // synchronize by summing input(=1) from all processes
      // in this communicator/group
      ///MPI_Reduce( &input, &output, 1, MPI_INT, MPI_SUM, 0, comm);
      //- time_t strt = time(NULLPTR);
      MPI_Barrier(comm);
      //- time_t fin = time(NULLPTR);
      //- std::cerr << fin - strt << " seconds" << std::endl;
   };
   
}
void MPIGrp::sync(int id) const
{
   int input = 1;
   int output = 0;

   input = id;
   //- std::cerr << "=-= SYNC =-= [" << my_id << "] (" << id << ")" << std::endl;   
   if ( my_id >= 0 ) {
      // synchronize by summing input(=1) from all processes
      // in this communicator/group
      //- std::cerr << "=-= SYNC =-= [" << my_id << "]:   begin MPI call to 0 (" << id << ")"<< std::endl;   
      MPI_Reduce( &input, &output, 1, MPI_INT, MPI_SUM, 0, comm);
      ///MPI_Barrier(comm);
      //- std::cerr << "=-= SYNC =-= [" << my_id << "]:     end MPI call to 0: (" << id << ")" << output << std::endl;   
   };
   
}

void MPIGrp::sync(const std::string& msg) const
{
   int input = 1;
   int output = 0;

   if ( my_id >= 0 ) {

      //- std::cerr << "MPI Sync: " << msg << ": proc #" << id() << " in group " << group_id() << "! "  << std::endl;
      // synchronize by summing input(=1) from all processes
      // in this communicator/group
      ///MPI_Reduce( &input, &output, 1, MPI_INT, MPI_SUM, 0, comm);
      //- time_t strt = time(NULLPTR);
      MPI_Barrier(comm);
      //- time_t fin = time(NULLPTR);
      //- std::cerr << fin - strt << " seconds" << std::endl;
   };
   
}

void MPIGrp::sync(int id, const std::string& msg) const
{
   int input = 1;
   int output = 0;

   input = id;
   //- std::cerr << "=-= SYNC =-= [" << my_id << "] (" << id << ")" << std::endl;   
   if ( my_id >= 0 ) {
      //- std::cerr << "MPI Sync: " << msg << ": " << my_id << " w/ " << id << "!  " ;
      // synchronize by summing input(=1) from all processes
      // in this communicator/group
      //- std::cerr << "=-= SYNC =-= [" << my_id << "]:   begin MPI call to 0 (" << id << ")"<< std::endl;   
      MPI_Reduce( &input, &output, 1, MPI_INT, MPI_SUM, 0, comm);
      ///MPI_Barrier(comm);
      //- std::cerr << "=-= SYNC =-= [" << my_id << "]:     end MPI call to 0: (" << id << ")" << output << std::endl;   
   };
   
}

void MPIGrp::desync( int interval )
{
     int waittime;
     
     waittime = rnd->uniform(0.0, interval ) + 0.5;
     //- std::cerr << "MPI De-sync: " << id() << " in " << group_id() << " for " << waittime  << std::endl;

     if ( waittime > 0 ) {

        (void) sleep( waittime ); 
     }  

}


void MPIGrp::shutdown() 
{

   MPI_Finalize();

}


MPIGrp* MPIGrp::subgroup(int size, int flags, int offset)
{
   int newsize;
   int *old_ranks;
   int src_idx;
   MPI_Group new_group;
   MPI_Comm new_comm;
   MPIGrp *new_mpigrp;

   // only do this if this process belongs to this group!
   if ( my_id >= 0 ) {

      // figure out the size (do sanity-checking)
      newsize = size;
      if ( newsize < 0 ) {
         throw(ProcessGrp::badgroupsize());
      }
      if ( newsize == 0 ) {
         // default to the same number of processors as the current group
         newsize = num_procs;
      }
      if ( newsize > num_procs ) {
         if ( flags && PG_STRICT ) {
            throw(ProcessGrp::badgroupsize());
         } else {
            newsize = num_procs;
         }
      }
      if ( newsize + offset > num_procs ) {
         throw(ProcessGrp::badgroupsize());
      }
   
      // set up ranks for the new group, in terms of the old group's ranks
      old_ranks = new int[newsize];
      if ( flags && PG_KEEPROOT ) {
         old_ranks[0] = 0; // keep the current root
         src_idx = offset;  // begin copying processes at the offset
         if ( offset == 0 ) {
            // don't duplicate the root process
            src_idx++;
         }   
      } else {
         old_ranks[0] = offset; // disregard the current root
         src_idx = offset + 1;  // begin copying at the next processor after this first one
      }      
      for (int proc = 1; proc < newsize; proc++ ) {
         old_ranks[proc] = src_idx;
         src_idx++;
      }
      
      // define a new MPI group with those processors
      MPI_Group_incl(group, newsize, old_ranks, &new_group);
      
      // create a new communicator based on the newly-defined group
      MPI_Comm_create(comm, new_group, &new_comm);
      
      // make a new MPIgrp object based on the newly-created communicator and group
      new_mpigrp = new MPIGrp(new_comm, new_group, 3);

      // free the ranks
      delete old_ranks;
   
   } else {
      // not a member of the parent group, so just return NULLPTR
      // We do not throw an exception here!
      new_mpigrp = NULLPTR;
   }
   
   return new_mpigrp;
};

MPIGrp* MPIGrp::subgroup( int size, int *list, int flags ) 
{
   MPI_Group new_group;
   MPI_Comm new_comm;
   MPIGrp *new_mpigrp;

   // notes: flags is ignored (for now)

   // is the requested size OK?
   if ( size <= 0 || size > num_procs ) {
      throw(ProcessGrp::badgroupsize());
   }
   
   // check each requested processor, to make sure it is in the current group
   for ( int i=0; i<size; i++ ) {
       if ( list[i] < 0 || list[i] >= num_procs ) {
          throw(ProcessGrp::badprocessor());
       }
   }
   
   // define a new MPI group with those processors
   MPI_Group_incl(group, size, list, &new_group);
      
   // create a new communicator based on the newly-defined group
   MPI_Comm_create(comm, new_group, &new_comm);
   
   // make a new MPIgrp object based on the newly-created communicator and group
   new_mpigrp = new MPIGrp(new_comm, new_group, 3);

   return new_mpigrp;

}          

void MPIGrp::split( int size, ProcessGrp **a, ProcessGrp **b, int flags ) const
{
   int size_a, size_b;
   MPI_Group old_group;
   MPI_Group group_a, group_b;
   int *old_ranks;
   MPI_Comm comm_a, comm_b;
   int beg_idx;
   
   // only do this if this process belongs to this group!
   if ( my_id >= 0 ) {

      // figure out the size (do sanity-checking)
      size_a = size;
      if ( size_a <= 0 ) {
         throw(ProcessGrp::badgroupsize());
      }
      if ( size_a > num_procs ) {
         if ( flags && PG_STRICT ) {
            throw(ProcessGrp::badgroupsize());
         } else {
            size_a = num_procs;
         }
      }
   
      // get the current communications group
      MPI_Comm_group(comm, &old_group);
   
      // set up the first new group, which always includes the root of the current process
      old_ranks = new int[size_a];
      for (int proc = 0; proc < size_a; proc++ ) {
         old_ranks[proc] = proc;
      }
      MPI_Group_incl(old_group, size_a, old_ranks, &group_a);
      MPI_Comm_create(comm, group_a, &comm_a);
      *a = new MPIGrp(comm_a, group_a, 3);
      delete old_ranks;

      // how many processors are left over for the second group?
      size_b = num_procs - size_a;
      if ( flags && PG_KEEPROOT ) {
         // we need to keep the root processor here, too
         size_b++;
      }

      if ( size_b > 0 ) {
         // set up the second new group
         old_ranks = new int[size_b];
         if ( flags && PG_KEEPROOT ) {
            old_ranks[0] = 0; // keep the current root
            beg_idx = 1;
         } else {
            beg_idx = 0;
         }      
         for (int proc = beg_idx; proc < size_b; proc++ ) {
            old_ranks[proc] = size_a + (proc - beg_idx);
         }
         MPI_Group_incl(old_group, size_b, old_ranks, &group_b);
         MPI_Comm_create(comm, group_b, &comm_b);
         *b = new MPIGrp(comm_b, group_b, 3);
         delete old_ranks;
      } else {
         *b = NULLPTR;
      }

   } else {
      // not a member of the parent group, so just return NULLPTR
      // We do not throw an exception here!
      *a = NULLPTR;
      *b = NULLPTR;
   }
   

}

int MPIGrp::numberOfProcessors() const  
{
    int result;

    // how many processors in this communicator?
    MPI_Comm_size( MPI_COMM_WORLD, &result);

    return result;
}  

real MPIGrp::random() const
{
    real result;
    
    result = 0.0;
    if ( my_id >= 0 ) {
       if ( my_id == 0 ) {       
          result = rnd->uniform(0.0, 1.0 ); 
          for ( int i = 1; i < num_procs; i++ ) {
              send_reals( i, 1, &result, PGR_TAG_RND);          
          }
       } else {
          receive_reals( 0, 1, &result, PGR_TAG_RND);                 
       }
    }
    
    return result;
}

MPI_Comm MPIGrp::MPIcomm() const
{
   return comm;
};

MPI_Group MPIGrp::MPIgroup() const
{
   return group;
};

void MPIGrp::send_reals( int id, int n, const real *vals, int tag) const
{

   int err;
   
   //- std::cerr << "           ((((  Sending " << n << " reals from  " << my_id << " to " << id << std::endl;
   if ( my_id >= 0 ) {
      if ( id < 0 || id >= num_procs ) {
         throw (badprocessor());
      }
   
      //- std::cerr << "           ((((  Sending " << n << " reals (" << vals[0] << " -- " << vals[n-1] << " eith tag " << tag << " to " << id << std::endl;
      err = MPI_Send( (void *) vals, n, MPI_REAL_VALUE, id, tag, comm);  
   
      if ( err != MPI_SUCCESS ) {
         throw (badparallelism());
      }      
   }
   //- std::cerr << "           ((((  Sent reals to " << id << std::endl;

};

void MPIGrp::send_doubles( int id, int n, const double *vals, int tag) const
{

   int err;
   
   //- std::cerr << "           ((((  Sending " << n << " doubles from  " << my_id << " to " << id << std::endl;
   if ( my_id >= 0 ) {
      if ( id < 0 || id >= num_procs ) {
         throw (badprocessor());
      }
   
      //- std::cerr << "           ((((  Sending " << n << " doubles (" << vals[0] << " -- " << vals[n-1] << " eith tag " << tag << " to " << id  << std::endl;
      err = MPI_Send( (void *) vals, n, MPI_DOUBLE, id, tag, comm);  
   
      if ( err != MPI_SUCCESS ) {
         throw (badparallelism());
      }      
   }
   //- std::cerr << "           ((((  Sent doubles to " << id << std::endl;

};

void MPIGrp::send_ints( int id, int n, const int *vals, int tag) const
{

   int err;
   
   //- std::cerr << "           ((((  Sending " << n << " ints from  " << my_id << " to " << id << std::endl;
   if ( my_id >= 0 ) {
      if ( id < 0 || id >= num_procs ) {
         throw (badprocessor());
      }
   
      //- std::cerr << "           ((((  Sending " << n << " ints (" << vals[0] << " -- " << vals[n-1] << " eith tag " << tag << " to " << id  << std::endl;
      err = MPI_Send( (void *) vals, n, MPI_INT, id, tag, comm); 
   
      if ( err != MPI_SUCCESS ) {
         throw (badparallelism());
      }      
   }
   //- std::cerr << "           ((((  Sent ints to " << id << std::endl;

};

void MPIGrp::send_string( int id, const std::string &str, int tag ) const {
   const char *contents;
   int err;
   int n;

   contents = str.c_str();
      
   n = str.length();
   
   //- std::cerr << "           ((((  Sending <<" << str << ">> string with tag " << tag << " to " << id  << std::endl;
   if ( my_id >= 0 ) {
      if ( id < 0 || id >= num_procs ) {
         throw (badprocessor());
      }
   
      err = MPI_Send( (void *) &n, 1, MPI_INT, id, tag, comm);    
      //- std::cerr << "           ((((  Sent string length " << n << " to " << id << std::endl;
      if ( err != MPI_SUCCESS ) {
         throw (badparallelism());
      }      
      if ( n > 0 ) {
         err = MPI_Send( (void *) contents, n, MPI_CHAR, id, tag, comm);    
         //- std::cerr << "           ((((  Sent char array to " << id << std::endl;
         if ( err != MPI_SUCCESS ) {
            throw (badparallelism());
         }      
      }      
   }


}


void MPIGrp::receive_reals( int id, int n, real *vals, int tag, int *src) const
{

   int err;
   MPI_Status status;
   
   //- std::cerr << "           ))))  Receiving " << n << " reals on " << my_id << " from " << id << std::endl;
   if ( my_id >= 0 ) {
      if ( id >= num_procs ) {
         throw (badprocessor());
      }
   
      //- std::cerr << "           )))) pre-received reals " << vals[0] << " -- " << vals[n-1] << std::endl;
      if ( id >= 0 ) {
         // receive from a specific processor
         //- std::cerr << "           )))) " << n << " reals being read with tag " << tag << " from " << id  << std::endl;
         err = MPI_Recv( (void *) vals, n, MPI_REAL_VALUE, id, tag, comm, &status); 
   
         if ( err != MPI_SUCCESS ) {
            throw (badparallelism());
         }   
      } else {
         // receive from any processor in the group
         //- std::cerr << "           )))) " << n << " reals being read from ANY proc " << " with tag " << tag  << std::endl;
         err = MPI_Recv( (void *) vals, n, MPI_REAL_VALUE, MPI_ANY_SOURCE, tag, comm, &status); 
   
         if ( err != MPI_SUCCESS ) {
            throw (badparallelism());
         }   
      }
      if ( src != NULLPTR ) {
         *src = status.MPI_SOURCE;         
      }   
   } else {
      if ( src != NULLPTR ) {
         *src = -1;         
      }   
   }   
   //- std::cerr << "           )))) received reals " << vals[0] << ", " << vals[n-1] << " from " << id << std::endl;

};

void MPIGrp::receive_doubles( int id, int n, double *vals, int tag, int *src) const
{

   int err;
   MPI_Status status;
   
   //- std::cerr << "           ))))  Receiving " << n << " doubles on " << my_id << " from " << id << std::endl;
   if ( my_id >= 0 ) {
      if ( id >= num_procs ) {
         throw (badprocessor());
      }
   
      //- std::cerr << "           )))) pre-received doubles " << vals[0] << " -- " << vals[n-1] << std::endl;
      if ( id >= 0 ) {
         // receive from a specific processor
         //- std::cerr << "           )))) " << n << " doubles being read with tag " << tag << " from " << id  << std::endl;
         err = MPI_Recv( (void *) vals, n, MPI_DOUBLE, id, tag, comm, &status);
        
         if ( err != MPI_SUCCESS ) {
            throw (badparallelism());
         }   
      } else {
         // receive from any processor in the group
         //- std::cerr << "           )))) " << n << " doubles being read from ANY proc " << " with tag " << tag  << std::endl;
         err = MPI_Recv( (void *) vals, n, MPI_DOUBLE, MPI_ANY_SOURCE, tag, comm, &status);        
         if ( err != MPI_SUCCESS ) {
            throw (badparallelism());
         }
      }
      if ( src != NULLPTR ) {
         *src = status.MPI_SOURCE;
      }   
   } else {
      if ( src != NULLPTR ) {
         *src = -1;         
      }   
   }
   //- std::cerr << "           )))) received doubles " << vals[0] << ", " << vals[n-1] << " from " << id << std::endl;

};

void MPIGrp::receive_ints( int id, int n, int *vals, int tag, int *src) const
{

   int err;
   MPI_Status status;
   
   //- std::cerr << "           ))))  Receiving " << n << " ints on " << my_id << " from " << id << std::endl;
   if ( my_id >= 0 ) {
      if ( id >= num_procs ) {
         throw (badprocessor());
      }
   
      //- std::cerr << "           )))) pre-received ints " << vals[0] << " -- " << vals[n-1] << std::endl;
      if ( id >= 0 ) {
         // receive from a specific processor

         //- std::cerr << "           )))) " << n << " ints being read with tag " << tag << " from " << id  << std::endl;
         err = MPI_Recv( (void *) vals, n, MPI_INT, id, tag, comm, &status);  
         if ( err != MPI_SUCCESS ) {
            throw (badparallelism());
         }   
      } else {
         // receive from any processor in the group
         //- std::cerr << "           )))) " << n << " ints being read from ANY proc " << " with tag " << tag  << std::endl;
         err = MPI_Recv( (void *) vals, n, MPI_INT, MPI_ANY_SOURCE, tag, comm, &status);  
         if ( err != MPI_SUCCESS ) {
            throw (badparallelism());
         }   
      
      }
      if ( src != NULLPTR ) {
         *src = status.MPI_SOURCE;         
      }            
   } else {
      if ( src != NULLPTR ) {
         *src = -1;         
      }   
   }
   //- std::cerr << "           )))) received ints " << vals[0] << ", " << vals[n-1] << " from " << id << std::endl;

};

void MPIGrp::receive_string( int id, std::string *str, int tag, int *src) const
{

   int err;
   MPI_Status status;
   char *contents;
   int n;
   int use_id;
   int i;
   
   //- std::cerr << "           ))))  Receiving string from " << id << std::endl;
   if ( my_id >= 0 ) {
      if ( id >= num_procs ) {
         throw (badprocessor());
      }
   
      if ( id >= 0 ) {
         // receive from a specific processor
         use_id = id;
      } else {
         use_id = MPI_ANY_SOURCE;
      }   
         
      err = MPI_Recv( (void *) &n, 1, MPI_INT, id, tag, comm, &status);  
      //- std::cerr << "           ))))  got string length " << n << " from " << id << std::endl;
      if ( err != MPI_SUCCESS ) {
         throw (badparallelism());
      }
      
      try {
          contents = new char[n + 1];
      } catch (...) {
         throw (badtransfer());
      }
      if ( n > 0 ) {
         err = MPI_Recv( (void *) contents, n, MPI_CHAR, id, tag, comm, &status);  
         //- std::cerr << "           ))))  got char array " << n << " from " << id << std::endl;
         if ( err != MPI_SUCCESS ) {
            throw (badparallelism());
         }
      }
      contents[n] = 0;
      
      // put the content into the string
      str->clear();
      for ( i=0; i < n; i++ ) {
         str->push_back( contents[i] );
      }
      delete contents;
            
      if ( src != NULLPTR ) {
         *src = status.MPI_SOURCE;         
      }            
   } else {
      if ( src != NULLPTR ) {
         *src = -1;         
      }   
   }

   //- std::cerr << "           ))))  finished getting string <<" << *str << ">> with tag " << tag << " from " << id   << std::endl;



}

#endif
