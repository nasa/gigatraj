
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

#include <stdlib.h>
#include <iostream>

#include "gigatraj/Flock.hh"
#include "gigatraj/SerialGrp.hh"

using namespace gigatraj;



Flock::Flock( int n )
{

   Parcel p;

   pgroup = NULLPTR;
   
   exemplar = NULLPTR;
   
   allocate( n, NULLPTR, &p, 0 );

};


     
Flock::Flock( ProcessGrp *pgrp, int n, int r)
{
   int i;
   Parcel p;
   ProcessGrp* pg;
   
   pgroup = NULLPTR;
   
   exemplar = NULLPTR;
   
   allocate( n, pgrp, &p, r );

};



Flock::Flock( const Parcel &p, int n)
{
   int i;
   ProcessGrp* pg;
   
   pgroup = NULLPTR;
   
   exemplar = NULLPTR;
   
   allocate( n, NULLPTR, &p, 0 );

};

Flock::Flock( const Parcel &p, ProcessGrp* pgrp, int n, int r)
{
    ProcessGrp* pg;
   
    pgroup = NULLPTR;
    
    exemplar = NULLPTR;
   
    allocate( n, pgrp, &p, r );
   
}

std::string Flock::make_proc_id ( const std::string& tag, int i ) const
{
   std::string result;
   int maxstr = 4;
   char cc[4];
   int idx;
   
   result = tag;

   // convert i to a C string
   idx = 0;
   if ( i < 1000 ) {
      if ( i >= 100 ) {
         cc[idx] = '0' + ( i/100 );
         idx++;
         i = (i % 100);
      }
      if ( i >= 10 ) {
         cc[idx] = '0' + ( i/10 );
         idx++;
         i = ( i % 10 );
      }
      cc[idx] = '0' + i;
   } else { 
      cc[idx] = '*';
   }
   cc[idx+1] = 0;
   
   result = result + std::string(cc);
   
   return result;

}

// this is an internal function used by constructors to set up and initialize the Flock 
void Flock::setup( const Parcel &p, ProcessGrp* pgrp, int n, int r)
{
   // the number of processors in this group
   int numprocs;
   // the number of processors to be devoted to parcel-tracing
   int num_traceprocs;
   // the number of processors to be devotes to met data
   int num_metprocs;
   // the number of processing groups to be created
   int num_groups;
   // number of parcels not yet allocated
   int parcels_left;
   // array of indices for met processors
   int *met_procs;
   // size of a typical (sub)group of processors
   int normal_group_size;
   // number of "extra" processors that do not fit into a standard-sized group
   int extra_procs;
   // the max size a group can be
   int max_group_size;
   // array of processors used to define a subgroup
   int *grp_proc_list;
   // index of processors in the current group
   int old_proc_idx;
   // index into the array of processors in a new (sub)group
   int new_proc_idx;
   // number of processors in a new (sub)group
   int group_size;
   // the starting processor of a new (sub)group
   int poffset;
   // the ending processor of a new (sub)group
   int pend;
   // this processor's ID wrt the the current group
   int my_proc;
   // the number of parcels ordinarily handled by a tracer processor
   int normal_proc_size;
   // number of "extra" parcels that do not fit into a standard-sized processor
   int extra_parcels;
   // count of the number of parcels allocated so far
   int parcel_idx;
   // ID of this processor wrt the (sub)group
   int my_proc_sub;
   // proces group creattion flags
   int pflag;
   // new processor (sub)group
   ProcessGrp* new_proc_grp;
   // iterator over the (sub)groups
   std::vector<ProcessGrp*>::iterator pi;
   // start index for a particular processor's parcels
   int this_parcel_start;
   // end index for a particular processor's parcels
   int this_parcel_end;
   // number of parcels for a particular processor
   int this_num_parcels;
   // the Meteorologicla data source (obtained from the Parcel)
   MetData *metsrc;
   // is this processor the root processor?
   int i_am_root;
   // the id of the root processor
   int my_root;
   // subprocessor id
   std::string subproctag;


   // grab the met data source from the Parcel
   metsrc = p.getMet();
   
   if ( exemplar != NULLPTR ) {
      delete exemplar;
   }
   exemplar = p.copy();

   blocksize = 0;
   
   if ( pgrp != NULLPTR ) {
      pgroup = pgrp;
   }
      
   // how many processors do we have to work with?
   numprocs = pgroup->size();
   
   i_am_root = pgroup->is_root();

   my_proc = pgroup->id(); 
   
   my_root = pgroup->root_id();

   // take the root process's number-of-particles as the
   // authoritative version, and have all other processors
   // use that.
   for ( int ip = 0; ip < numprocs; ip++ ) {
       //pgroup->sync("Flock::setup: in propagate-size loop");
       pgroup->sync();
       if ( i_am_root ) {
          if ( ip != my_proc ) {
             pgroup->send_ints(ip, 1, &n );
          }
       } else {   
          if ( my_proc == ip ) {
             pgroup->receive_ints( my_root, 1, &n ); 
          }  
       }
   }    

   // Asking for a negative number of parcels?
   if ( n < 0 ) {
      throw (badparcelcount());
   }           
   
   // reserve space for as many parcel intervals
   // as there are processors in our group.
   pclstarts.reserve(numprocs);
   pclends.reserve(numprocs);
   for ( int ip=0; ip<numprocs; ip++ ) {
      pclstarts.push_back(-1);
      pclends.push_back(-1);
   }

   // will we doing parallel processing with dedicated met processors?
   //- std::cerr << "New Flock: r = " << r << std::endl;
   if ( r > 0 && numprocs > 1 ) {
   
      // Cap the number of tracing processors (approx. r) 
      // at the number of total procrssors, minus one 
      // for a met processor.
      if ( r > (numprocs - 1) ) {
         r = numprocs - 1;
      }
   
      // At the Flock level, the pgroup is set to be a coordinator
      pgrp->setType( -1, ProcessGrp::PGrpRole_Coordinator);

      // figure out how many processors to devote to met-handling
      num_metprocs = numprocs / (r+1);

      // how many processors are to be devoted to tracing parcels?
      num_traceprocs = numprocs - num_metprocs;
   
      // for EACH met-handler process, 
      // we will set up one (sub)group of parcel-tracing processors
      num_groups = num_metprocs ;
       
   } else {
      // every procesor will do its own met handling
      // so there will be just the one (sub)group of parcel-tracing processors
      num_metprocs = 0;
      num_traceprocs = numprocs;
      num_groups = 1;
   }
      
   // How big will our (sub)groups of processors be?
   if ( num_metprocs > 0 ) {

      // this array will hold a list of the met-handling processors
      met_procs = new int[num_groups];
      // how many processors will each (sub)group have, normally?
      normal_group_size = num_traceprocs / num_groups;
      // will there be any processors left over?
      extra_procs = num_traceprocs - normal_group_size * num_groups;
      // add one for the met processors, since each (sub)group
      // consists of one met-handler and one-or-more parcel-tracers
      normal_group_size++;
      // what is the max size (number of procs) of one of our process-tracing (sub)groups?
      max_group_size = normal_group_size;
      if ( extra_procs > 0 ) {
         max_group_size++;
      }
      
   } else {
      // no dedicated met-handling processors, so
      // we have just the one processing (sub)group
      met_procs = NULLPTR;
      normal_group_size = numprocs;
      extra_procs = 0;
      max_group_size = normal_group_size;
   }

   //- std::cerr << "[" << my_proc << "/" << num_traceprocs << "] traceprocs set " << std::endl;
   
   // this array will hold a list of the procs for each (sub)group
   // (we will re-use it as we set up each (sub)group in turn)
   grp_proc_list = new int[max_group_size];

   // how many parcels are we asking for?
   if ( n == 0 ) {
      n = num_traceprocs;  // default: one parcel per tracing-processor
   }
   // are there not enough parcels to occupy all of the processors?
   if ( n < num_traceprocs ) {
      // we should probably just not use some of the procoessors
      // but for now, we'll sonsider this a pathological case and
      // throw an exception
      if ( met_procs != NULLPTR ) {
         delete[] met_procs;
      }
      delete[] grp_proc_list;
      std::cerr << "Too few parcels (" << n 
                << ") to occupy the number of processors (" 
                << num_traceprocs << ")" << std::endl;
      throw (badparcelcount());
   }
      
   // we will use this many parcels
   num_parcels_total = n;

      
   // how many parcels for each processor?
   normal_proc_size = num_parcels_total / num_traceprocs;
   // any left over?
   extra_parcels = num_parcels_total - normal_proc_size * num_traceprocs;

   //- std::cerr << "[" << my_proc << "/" << num_traceprocs << "] groups=" << num_groups 
   //-   << " parcels=" << normal_proc_size << " extra=" << extra_parcels 
   //-   << " metprocs=" << num_metprocs << std::endl;
   

   // allocate space for the (sub)groups we are about to create
   subgroups.reserve(num_groups);
   pgstarts.reserve(num_groups);
   pgends.reserve(num_groups);
   
   // we start with this many parcels to dole out to the tracing processors
   parcels_left = num_parcels_total;
   parcel_idx = 0;

   // create each (sub)group
   old_proc_idx = 0;  // start with this current process
   for (int i=0; i < num_groups; i++ ) {
       //- std::cerr << "[" << my_proc << "/" << num_traceprocs << "] doing group " << i << std::endl;
       
       subproctag = make_proc_id("flksub" , i);
             
       // this is the starting point for the new (sub)group
       poffset = old_proc_idx;
       
       // processor slot (rank) in the new group
       new_proc_idx = 0;
     
       // how many processors will belong to this (sub)group?
       group_size = normal_group_size;
       if (extra_procs > 0 ) {
          group_size++;
          extra_procs--;
       }
       
       //- std::cerr << "[" << my_proc << "/" << num_traceprocs << "] grp size=" 
       //- << group_size << ", extra_procs=" << extra_procs  << std::endl;
       // do we add a met processor at the beginning of the group?
       if ( num_metprocs > 0 ) {
          met_procs[i] = old_proc_idx;
          //- std::cerr << "[" << my_proc << "/" << num_traceprocs << "] new_proc_idx=" 
          //- << new_proc_idx << std::endl;
          //- std::cerr << "[" << my_proc << "/" << num_traceprocs << "] Adding " << old_proc_idx  << std::endl;
          grp_proc_list[new_proc_idx] = old_proc_idx;
          new_proc_idx++;
          old_proc_idx++;
       } 
       
       // add the tracing processors to the group
       for ( ; new_proc_idx < group_size; new_proc_idx++ ) {
          //- std::cerr << "[" << my_proc << "/" << num_traceprocs << "] adding " << old_proc_idx  << std::endl;
          grp_proc_list[new_proc_idx] = old_proc_idx;
          old_proc_idx++;
// should we do this, too?
//new_proc_idx++
       }
       // Actually, because the rank-0 processor of the 0th subgroup
       // is the same as the rank-0 processor of the main group,
       // we will want it to be a parcel-tracer, not a met-handler.
       // Thus, in every subgroup, the rank-0 processor will be
       // a parcel-tracer, and the rank-1 processor (if there are
       // dedicated met processors) will be a met-processor.
       // And ranks > 1 will be parcel-tracers.
       // (See below, where we set the types)
       
       //- std::cerr << "[" << my_proc << "/" << num_traceprocs << "] whiz"  << std::endl;
              
       // split these processes off into a parcel-tracing (sub)group
       pflag = 0;  // no special flags for making this (sub)group
       try {
          //- std::cerr << "[" << my_proc << "/" << num_traceprocs << "] splitting"  
          //- << " group_size=" << group_size;
          //- for ( int ii=0; ii<group_size; ii++ ) { 
          //- std::cerr << " [" << ii << "]=" << grp_proc_list[ii];
          //- }
          //- std::cerr << " pflag=" << pflag << std::endl;
          new_proc_grp = pgroup->subgroup( group_size, grp_proc_list, pflag );
          new_proc_grp->tag( subproctag );

          //- std::cerr << "[" << my_proc << "/" << num_traceprocs << "] end split"  << std::endl;
       } catch (...) {
          // release stuff here
          delete[] met_procs;
          // destroy all (sub)groups created hitherto
          for ( pi=subgroups.begin(); pi != subgroups.end(); pi++ )
          {
              delete *pi;
          }
          
          // re-throw the exception
          throw;
       }
   
       //- std::cerr << "[" << my_proc << "/" << num_traceprocs << "] bang"  << std::endl;
       
       // get the rank of *this* processor in the new (sub)group
       // If this procesor is not a member of that (sub)group,
       // the id will be -1  
       my_proc_sub = new_proc_grp->id();

       // am I tracing parcels or processing met data?
       // am I in this group?
       if ( my_proc_sub >= 0 ) {
          if ( num_metprocs > 0 ) {
             // the first (sub)group: the rank-1 processor is the met-handler,
             // so that the rank-0 processor (which is also the rank-o processor
             // of the main group "pgroup") can remain a parcel-tracer
             if ( my_proc_sub == 1 ) {
                new_proc_grp->setType( my_proc_sub, ProcessGrp::PGrpRole_MetReader);
                my_num_parcels = 0;
                my_parcel_start = -1;
                is_met = 1;
                // even though as a met-processor our official my_num_parcels is 0,
                // we sneak one parcel in so that we have access to its met
                // data source, in case we need it.
                parcels.push_back(p.copy());
             } else {
                new_proc_grp->setType( my_proc_sub, ProcessGrp::PGrpRole_Tracer);
                is_met = 0;
                // parcels to be pushed into the vector below
             }          
          } else {
              // all procs are tracing and reading met data
              new_proc_grp->setType( my_proc_sub, ProcessGrp::PGrpRole_All);
              is_met = 0;
              // parcels to be pushed into the vector below
          }
       }
       
       // store the beginning and ending processor IDs
       // store this starting point
       pgstarts.push_back(poffset);
       // store this ending point
       pend =  poffset + group_size - 1; 
       pgends.push_back(pend);
       
       // store the sub-process
       subgroups.push_back( new_proc_grp );


       // for each process in this (sub)group
       for (int j=0; j<group_size; j++ ) {
          // are we to process any parcels
          // note: we have to loop though like this,
          // instead of querying the process type,
          // so that all processors (no matter which sub-group
          // they belong to) have the same
          // understanding of which parcel indices are
          // associated with which processors
          
          // if we are not using met handlers, or
          // if the jth processors in this (sub)group is not a met-handler...
          if ( (num_metprocs == 0)
            || (j!=1)  ) {
            
            // ...then it must be a parcel-tracer
            
            // are there parcels left to be allocated?
            if ( parcels_left > 0 ) {
               this_parcel_start = parcel_idx;
               this_num_parcels = normal_proc_size;
               if ( extra_parcels > 0 ) {
                  this_num_parcels++;
                  extra_parcels--;
               }
               parcel_idx = parcel_idx + this_num_parcels;
               
               // are these *my* parcels?
               if ( pgroup->id() == grp_proc_list[j] ) {
               
                  my_num_parcels = this_num_parcels;
                  my_parcel_start = this_parcel_start;
               
                  // allocate space for them
                  parcels.reserve(my_num_parcels);
               
                  // push the parcels in, for non-met-processing processors
                  // (the met processing case is handled above)
                  for (int ip=0; ip < my_num_parcels; ip++ ) {
                      parcels.push_back( p.copy() );
                  }
               }
               
               this_parcel_end = this_parcel_start + this_num_parcels - 1;

               // reduce the number of parcels to go
               parcels_left = parcels_left - this_num_parcels;
               
            } else {
               this_parcel_start = -1;
               this_num_parcels  =  0; 
               this_parcel_end = -1;              
            }      
             
          } else {
          
             // This is not a parcel-tracer processor
             this_parcel_start = -1;
             this_num_parcels  =  0;               
             this_parcel_end = -1;              
             
          }
          //- std::cerr << "===[" << my_proc << "/" << num_traceprocs << "] proc=" << j << " start=" << this_parcel_start
          //-  << " end=" << this_parcel_end << " ===" << std::endl;
          
          pclstarts[grp_proc_list[j]] = this_parcel_start;
          pclends[grp_proc_list[j]] = this_parcel_end;
          
       }
       
       // is this my group?
       if ( my_proc >= poffset && my_proc <= pend ) {
          my_group = i;
          
          my_met = -1;
          if ( num_metprocs > 0 ) {
             my_met = 1; // rank within the (sub)group of the met-handler for the (sub)group
          }
          
          
          
          /**** WARNING *****/
          // This sets the process group of the met data source for ALL parcels,
          // whether they are part of this Flock or not!
          // This is fine and necessary if only this flock is being
          // used to trace parcels. Otherwise, extreme caution should be used!
          // Save the met source's pgroup before creating this Flock, 
          // and restore the met source's pgroup after deleting the Flock.
          
          rememberPGroup(); 
          setPGroup( subgroups[i], my_met );
          unRestorePGroup();          

       }
         

   } // end of (sub)group creation loop

   delete[] grp_proc_list;
   if ( met_procs != NULLPTR ) {
      delete[] met_procs;
   }
   //- std::cerr << "[" << my_proc << "/" << num_traceprocs << "] exiting setup" << std::endl;


};

Flock::~Flock()
{

   clear();
   
   // destroy the process group
   delete pgroup;
   
   
};

bool Flock::is_root() const
{
   return pgroup->is_root();
}

ProcessGrp* Flock::getPGroup( int* id  ) const
{

    if ( id != NULLPTR ) {
       *id = preserved_flock_pid;
    }
    
    return preserved_flock_pgroup;
}

void Flock::setPGroup( ProcessGrp *pg, int id )
{
      preserved_flock_pgroup = pg;
      preserved_flock_pid = id;
}

ProcessGrp* Flock::fetchRememberedPGroup( int* id ) const
{

    if ( id != NULLPTR ) {
       *id = preserved_met_pid;
    }
    
    return preserved_met_pgroup;
}

void Flock::rememberPGroup( ProcessGrp *pg, int id )
{
    MetData* metsrc;
    
    if ( pg != NULLPTR ) {
       preserved_met_pgroup = pg;
       preserved_met_pid = id;
    } else {
       metsrc = NULLPTR;
       if ( size() > 0 ) {
          metsrc = parcels[0]->getMet();
       } 
       if ( metsrc != NULLPTR ) {
          preserved_met_pgroup = metsrc->getPgroup( &preserved_met_pid );
       }
    }   
}

void Flock::restorePGroup()
{
     MetData* metsrc;

     metsrc = NULLPTR;
     if ( size() > 0 ) {
        metsrc = parcels[0]->getMet();
     } 
     if ( (preserved_met_pgroup != NULLPTR) && (metsrc != NULLPTR) ) {
        metsrc->setPgroup( preserved_met_pgroup, preserved_met_pid );      
     }
}

void Flock::unRestorePGroup()
{
     MetData* metsrc;

     metsrc = NULLPTR;
     if ( size() > 0 ) {
        metsrc = parcels[0]->getMet();
     } 
     if ( (preserved_flock_pgroup != NULLPTR) && (metsrc != NULLPTR) ) {
        metsrc->setPgroup( preserved_flock_pgroup, preserved_flock_pid );  
     }
}



Flock::iterator::iterator() 
{
   // default constructor is a dummy that does nothing
   my_parcel = -1;
   my_flock = NULLPTR;
};

Flock::iterator::iterator(int init, Flock *flk) 
{
    set(init, flk);
};

void Flock::iterator::set(int init, Flock *flk)
{
   my_flock = flk; 
   if ( my_flock->my_num_parcels >= 0 ) {
      if ( init <  my_flock->my_num_parcels ) {
         my_parcel = init;
      } else {
         throw(badparcelindex());
      }      
   } else {
      throw(badparcelindex());
   }      

}

Parcel& Flock::iterator::operator*() const 
{
    return *(my_flock->parcels[my_parcel]);
};

Parcel* Flock::iterator::operator->() const 
{
    return my_flock->parcels[my_parcel];
};

bool Flock::iterator::operator==(const iterator& x) const 
{
    return (my_flock == x.my_flock) && (my_parcel == x.my_parcel);
};

bool Flock::iterator::operator!=(const iterator& x) const 
{
    return (my_flock != x.my_flock) || (my_parcel != x.my_parcel);
};

// postfix
Flock::iterator& Flock::iterator::operator++(int n) 
{
    my_parcel++;
    if ( my_parcel >= my_flock->my_num_parcels ) {
       my_parcel = -1;
       my_flock->fin();
    }
    return *this;
};

// prefix
Flock::iterator& Flock::iterator::operator++() 
{
    ++my_parcel;
    if ( my_parcel >= my_flock->my_num_parcels ) {
       my_parcel = -1;
       my_flock->fin();
    }
    return *this;
};

int Flock::iterator::index() const
{
    int result = -1;

    if ( my_flock->my_num_parcels > 0 ) {
       if ( my_parcel >= 0 && my_parcel < my_flock->my_num_parcels ) {
       
          result = my_flock->my_parcel_start + my_parcel;

       }
    }
    
    return result;
}


void Flock::debut()
{
    int n;
    MetData *met;

    //- std::cerr << "Flock::debut: [" << pgroup->id() << "/" << pgroup->group_id() 
    //-      << "]: entering " << my_num_parcels << std::endl;        
    
    n = 0;
    // if we have no parcels (must be a met reader process)
    if ( my_num_parcels <= 0 ) {
    
       /* This is a little tricky.
          Tracing processors and met procrssors move through an iteration loop
          in different ways. For tracing processors, the iterator is set to
          the first of the procrssor's parcels with begin(), and when the iterator is
          incremented it moves to the next of that processor's parcels, and
          when the iterator goes beyond this procorssor's parcels te parcle index
          is set to -1, which matches the end(). 
          Met processors work differently. When the iterator is initialized with begin(),
          the met processsor starts serving mt data until it is signalled not to
          by all of its clients.  Only at that point does the iterator leave the begin()
          routine, with a parcel index value of -1, which matches the end(), which
          means that the actions inside the loop never get done for a met processor,
          just as desired. 
          The processors within a subgroup (tracing procerssors and their associated 
          met processor) are kept synchronized automatically by that signalling.

          Note that this processor subgroup's processors syncs within the subgroup 
          once the met data accesses have stopped. But they do not sync with other
          subgroups in this Flock. Some filters which require synchronization across the Flock
          will initiate that sync themselves.
      */
    
       //- std::cerr << "Flock::debut: [" << pgroup->id() << "/" << pgroup->group_id() 
       //-   << "]:    No parcels!" << std::endl;        
       if ( is_met ) {
          //- std::cerr << "Flock::debut: [" << pgroup->id() << "/" << pgroup->group_id() 
          //- << "]:    I am Met-reader" << std::endl;        
          met = parcels[0]->getMet();
          met->serveMet();
          //pgroup->sync("Flock::debut sync for met server, past serveMet");
          //pgroup->sync();
       }
       n = -1;
    }   

    //- std::cerr << "Flock::debut: [" << pgroup->id() << "/" << pgroup->group_id() 
    //-      << "]: exiting " << std::endl;        

}

void Flock::fin()
{
   MetData *met;

     //- std::cerr << "Flock::fin: [" << pgroup->id() << "/" << pgroup->group_id() 
     //-     << "]: entering" << std::endl;        
     if ( my_met >= 0 ) {
        // if I am a tracing processor, send a termination notice
        // to my met processor.
        
        // if this is a met processor, I should
        // be in method metproc, 
        // listening to tracing processors for requests.
        // when i get all N termination requests,
        // I exit the metproc method.
        
        //- std::cerr << "Flock::fin: [" << pgroup->id() << "/" << pgroup->group_id() 
        //-  << "]: signaling " << my_met << std::endl;        
        met = parcels[0]->getMet();
        met->signalMetDone();
        
        
     };
     
     // See the description in debut()above for who gets to call
     // this sync with the Flock.
     //pgroup->sync("Flock::fin sync");
     //pgroup->sync();

     //- std::cerr << "Flock::fin: [" << pgroup->id() << "/" << pgroup->group_id() 
     //-     << ": exiting" << std::endl;        
};

Flock::Iter Flock::begin() 
{
    Flock::Iter ii;
    int n;
    
    
    this->debut();
    
    n=0; // start at psrcel index 0, regardless of processor
    // if we have no parcels (must be a met reader process)
    if ( my_num_parcels <= 0 ) {
       n = -1;  // unless this is a met-reading processor
    }  
     
    ii.set( n, this );
    
    return ii;

};

Flock::Iter Flock::end()
{
    Flock::Iter ii;
    
    ii.set( -1, this );
    
    return ii;

};



void Flock::setNav( PlanetNav& newnav ) 
{

    if ( num_parcels_total > 0 ) {
       parcels[0]->setNav(newnav);
    } else {
       throw ( Flock :: badparcelcount() );
    };     
};
 
PlanetNav* Flock::getNav() 
{
    if ( num_parcels_total > 0 ) {
       return parcels[0]->getNav();
    } else {
       throw ( Flock :: badparcelcount() );
    };     
};


void Flock::setMet( MetData& newmet ) 
{
    if ( num_parcels_total > 0 ) {
       parcels[0]->setMet(newmet);
    } else {
       throw ( Flock :: badparcelcount() );
    };     
};

MetData* Flock::getMet() 
{
    if ( num_parcels_total > 0 ) {
       return parcels[0]->getMet();
    } else {
       throw ( Flock :: badparcelcount() );
    };     
};


// iterators: (must be forward-only!)
//     on begin:
//         sync all procs in group
//         if met, then listen for traffic from rest of group
//     on going past end:
//         if tracer proc, inform met proc it is time to go
//         met proc receives notice form all tracer procs, leaves
//         sync all procs in group
//     sync on greater than end


int Flock::size()
{
    return num_parcels_total;

};

int Flock::belongs( const int n ) const
{
    int proc_idx;
    typedef std::vector<int>::iterator Iter; 
    Iter pits, pite;
    int done = 0;

    // sanity-check
    if ( n < 0 || n >= num_parcels_total ) {
       throw (badparcelindex());
    }
       
    // which processor owns this parcel?
    proc_idx = 0;
    for ( proc_idx=0; proc_idx < pclstarts.size() && !done ; ) {

       if ( n >= pclstarts[proc_idx] && n <= pclends[proc_idx] ) {
          done = 1;
       } else {
          proc_idx++;
       }   
    
    }

    return proc_idx;
}

void Flock::set( const int n,  const Parcel& p, const int mode)
{
   int idx;
   int proc_idx;
   int my_parcel_end ;
   std::vector<int>::iterator pits, pite;

   //- std::cerr << "Flock::set: entry for parcel " << n << std::endl;
   
   if ( pgroup->type() == ProcessGrp::PGrpRole_MetReader ) {
      //- std::cerr << "Flock::set: I am a met processor. Bye-bye!" << std::endl;
      return;
   }
   
   if ( n < 0 | n > num_parcels_total ) {
      throw ( Flock::badparcelindex() );
   }
   
   // is n between myprocessSTART and (myprocessID+myprocessSIZE)?   
   // then if we are the root process, just return the parcel
   // but if we are not the root process, send/receive the parcel

   my_parcel_end = my_parcel_start + my_num_parcels - 1;
   //- std::cerr << "Flock::set: this processors handles parcels " 
   //- << my_parcel_start << " through " << my_parcel_end << std::endl;
   //- std::cerr << "Flock::set: my root processor is " << pgroup->root_id() << std::endl; 

   // does the requested parcel belong to this processor?   
   if ( n >= my_parcel_start && n <= my_parcel_end ) {
      //- std::cerr << "Flock::set:    Parcel " << n << " is MY parcel! (" 
      //- << my_parcel_start << "-" << my_parcel_end << ")" << std::endl;     
      // index relative to the start of this processor's parcels
      idx = n - my_parcel_start;
      
      if ( pgroup->id() != 0 && mode == 0) {
         // we are not the root process, so we have
         // this parcel receive itself from the root
         //- std::cerr << "Flock::set:    about to receive" << std::endl;
         parcels[idx]->receive(pgroup, 0);
         //- std::cerr << "Flock::set:    received" << std::endl;
      } else {
          // we are the root processor, and this is our parcel,
          // OR mode!=0.  Either way, we want to use our local copy,
          // so just set it here, after deleting the old parcel first.
          delete parcels[idx];
         //- std::cerr << "Flock::set:    about to copy" << std::endl;
          parcels[idx] = p.copy();
         //- std::cerr << "Flock::set:    copied" << std::endl;
      }
      
   } else {
      
      // This is not our parcel
      // but if we are the root processor (and if we are in the 
      // quasi-serial mode 0), then we must send
      // this parcel out to the processor whose parcel it is
      //- std::cerr << "Flock::set:    Parcel " << n << " is NOT my parcel." << std::endl;

      if ( pgroup->id() == 0 && mode == 0 ) {
         // which processor owns this parcel?
         proc_idx = this->belongs(n);
         //- std::cerr << "Flock::set:    but I am root: about to send to " << proc_idx << std::endl;
         p.send( pgroup, proc_idx );
         //- std::cerr << "Flock::set:    sent " << std::endl;
      }
   
   }

   //- std::cerr << "Flock::set:   done " << std::endl;

}


// do get like set above
Parcel* Flock::parcel( int n, int flag ) const
{
   Parcel* p;
   Parcel* pp;
   int pid;
   bool found;
   int parcel_proc;
   int my_proc;
   int i;
   int valid_parcel = 0;
   
   if ( pgroup == NULLPTR ) {
      // safety 
      return NULLPTR;
   }
   if ( pgroup->type() == ProcessGrp::PGrpRole_MetReader ) {
      // met processors do not handle parcels
      return NULLPTR;
   }
   
   // sanity-check the index
   if ( n < 0 | n >= num_parcels_total ) {
      throw ( Flock::badparcelindex() );
   }

   // find the processor to which this parcel index belongs
   parcel_proc = this->belongs(n);

   // and who am I?
   my_proc = pgroup->id();

   // am I this parcel's owner?
   if ( my_proc == parcel_proc ) {
   
      // get the parcel
      p = parcels[ n - my_parcel_start ];

      valid_parcel = 1;

      // If we are the root processor...
      if ( my_proc == 0 ) {

         //std::cerr << "Flock::parcel: root proc: my parcel " << n << std::endl;         

         // send this out to the other processors?
         if ( flag == 0 ) {
         
            // for each processor in our group, aside from the root...
            for ( i = 1; i<pgroup->size(); i++ ) {
                // (don't try to send to met-reading processors)
                if ( pclstarts[i] >= 0 ) {
                   p->send(pgroup, i);
                }
            }
         }
         
      } else {
         // we are the owner, but not the root processor

         //std::cerr << "Flock::parcel: non-root proc: sending parcel " << n << std::endl;         

         // send the parcel to the root processor
         p->send(pgroup, 0);

      }

   } else {
   
      // we do not own the parcel, so 
      // either we return a NULLPTR (if flag == 1)
      // or we receive this parcel
      // from its owner (if we are root) or from
      // root (if we are neither the owner nor root)

      // use the sample parcel, so that the calling
      // routine does not have to worry about deleting it
      p = exemplar;

      
      // are we the root processor?
      if ( my_proc == 0 ) {

         // then receive the parcel from its owner

         //std::cerr << "Flock::parcel: root proc: receiving parcel " << n << std::endl;         
         p->receive(pgroup, parcel_proc);
         valid_parcel = 1;
            
         if ( flag == 0 ) {

            // send it out to the other parcels, if desired
            for ( i = 1; i<pgroup->size(); i++ ) {
                // but don't try to send to its owner, or to met-reading processors
                if ( i != parcel_proc && pclstarts[i] >= 0 ) {
                   p->send(pgroup, i);
                }   
            }
         }
             
      } else {
         // we are neither the owner nor the root processor
      
         //std::cerr << "Flock::parcel: non-root proc: ignoring parcel " << n << std::endl;         

         if ( flag == 0 ) {
            // we must not be the met-reader either
            if ( ! is_met ) {
               // receive the parcel from the root processor
               p->receive(pgroup,0);
               valid_parcel = 1;
            }
         } else {
            p = NULLPTR;
            valid_parcel = 0;
         }
      }       

   }
   
   if ( valid_parcel == 0 && p != NULLPTR ) {
      p = NULLPTR;
   }
      
   return p;

}

Parcel& Flock::get( int n, int flag ) const
{
   Parcel* p;
   
   p = this->parcel(n, flag);
   if ( p != NULLPTR ) {
      return *p;
   } else {
      throw (badparcelindex());
   }
}

Parcel& Flock::operator[]( int n )
{
   /* Note that we need to return a reference
      to the actual Flock parcel, not
      to a copy. Otherwise, assignments
      like "F[i] = px" will not work as expected.
   */   
   return this->get(n, 0);
   
}

void Flock::add( const Parcel& p, const int mode )
{
    int lowest_pop = -1;
    int lowest_proc = -1;
    int proc_idx = -1;
    int num_p;
    int my_rank;
    Parcel* parcl;
    std::vector<int>::iterator pits, pite;
   
    
    my_rank = pgroup->id();

    // find the process (sub)group that has the fewest Parcels 
    for ( proc_idx = 0;  proc_idx < pclstarts.size() ; proc_idx++ ) {        
         num_p = pclends[proc_idx] - pclstarts[proc_idx] + 1;
         if ( num_p > lowest_pop ) {
            lowest_pop = num_p;
            lowest_proc = proc_idx;
         }
    }
    
    // found none? Throw an error.
    if ( lowest_proc < 0 ) {
       throw (badparcelindex());
    }
    
    // increment the start and end parcel indices
    // past this processor's
    for ( int i=lowest_proc; i<pclstarts.size(); i++) {
        if ( i > 0 && pclstarts[i] >= 0 ) {
           pclstarts[i]++;
        }
        if ( pclends[i] >= 0 ) {
           pclends[i]++;
        }
        if ( my_rank == i ) {
           my_parcel_start = pclstarts[i];
           my_num_parcels = pclends[i] - my_parcel_start + 1;
        }
    }
    // oh, and increment the total as well
    num_parcels_total++;
    
    // "Is this my parcel?" asks the local processor
    if ( lowest_proc == my_rank ) {

       parcl = p.copy();
    
       // depending on the mode, either use the value we have,
       // or receive the value from the root processor
       if ( mode == 0 ) {
          // receive the value from the root processor
          parcl->receive( pgroup, 0 );
       }
       // add it to our local list
       parcels.push_back( parcl );
    
    } else {
       // not my parcel. 
       // Am I the root process?
       if ( my_rank == 0 ) {
          // send the value to the new owner
          p.send( pgroup, lowest_proc );
       }

    }

}

int Flock::advance( double dt )
{
   /*
    Flock::iterator pk;

    pk = this->begin();
    while ( pk != this->end() ) {
       pk->advance(dt);
       ++pk;
    }
    */

    int i;
    MetData* met;
    double tyme;
 
    if ( parcels.size() > 0 ) {
       
       met = parcels[0]->getMet();
       
       if ( is_met ) {
          //- std::cerr << "Flock::dadvance: [" << pgroup->id() << "/" << pgroup->group_id() 
          //- << "]:    I am Met-reader" << std::endl;        
          met->serveMet();
       } else {

          Parcel* p;
       
          int blk = my_num_parcels;
          if ( blocksize > 0 && blocksize < blk ) {
             blk = blocksize;
          }
       
          real* const lons = new real[blk];
          real* const lats = new real[blk];
          real* const zs = new real[blk];
          ParcelFlag* const flags = new ParcelFlag[blk];
          ParcelStatus* const statuses = new ParcelStatus[blk];
          int* const traceflags = new int[blk];

       
          p = parcels[0];
          tyme = p->t;
          Integrator* integ = p->integrator();
          PlanetNav* nav = p->getNav();

          /*
          for ( i=0; i < my_num_parcels; i++ ) {

              p = parcels[i];              
          
              p->advance( dt );
              
          }
          */
          
          i=0;
          while ( i < my_num_parcels ) {
          
              int jmax = i + blk;
              if ( jmax >= my_num_parcels ) {
                 jmax = my_num_parcels - 1;
              }   
              
              int nn = jmax - i + 1;
              for ( int j = i; j <= jmax; j++ ) {

                  p = parcels[j];              
          
                  lons[j] = p->lon;
                  lats[j] = p->lat;
                  zs[j] = p->z;
                  flags[j] = p->flagset;
                  statuses[j] = p->statuses;
                  
                  traceflags[j] = 0;
                  
                  if ( ( statuses[j] & (HitBad | HitBdy) )
                    || ( flags[j] & NoTrace ) 
                    || ( (flags[j] & SyncTrace) && (p->t >= tyme) )
                  ) {
                     traceflags[j] = 1;
                  }
              }

              integ->go( nn, lons, lats, zs, traceflags, tyme, met, nav, dt ); 
          
              for ( int j = i; j <= jmax; j++ ) {

                  p = parcels[j];           
          
                  p->lon = lons[j];
                  p->lat = lats[j];
                  p->z = zs[j];
                  p->t = tyme;
                  
                  if ( traceflags[j] ) {
                     statuses[j] = statuses[j] | HitBad;
                     flags[j] = flags[j] | NoTrace;
                  }
                  
                  p->flagset = flags[j];
                  p->statuses = statuses[j];
              }
              
              i = jmax + 1;
                  
          }
          
          delete[] traceflags;
          delete[] statuses;
          delete[] flags;
          delete[] zs;
          delete[] lats;
          delete[] lons;
          
          
          met->signalMetDone();
       }
    }
    
    return 0;
}

int Flock::countFlags( ParcelFlag flgs, bool negate )
{
    int result;
    int local_result;
    int remote_result;
    bool i_am_root;
    int i;
    ParcelFlag mask;
    int my_groot;
    int my_gid;
    int my_gn;
 
    result = 0;
    
    i_am_root = is_root();
    
    // first, take a local count based on our own parcels
    // (met processors can do this too; their local_result will be 0)
    local_result = 0;    
    if ( parcels.size() > 0 ) {
              
       Parcel* p;
       
       for ( i = 0; i < my_num_parcels; i++ ) {      
           if ( parcels[i]->flagset & flgs ) {
              local_result ++;
           }
       
       }

    }
    
    // now we have to sum these local results over all processors
    // in this Flock's group.
    
    my_groot = pgroup->root_id();
    my_gid = pgroup->id();
    my_gn = pgroup->size();
    
    //std::cerr << "I am processor " << my_gid;
    //if ( is_met ) {
    //   std::cerr << " [MET] ";
    //} else {
    //   std::cerr << " [TRACING] ";    
    //}
    //if ( i_am_root ) {
    //   std::cerr << "[ROOT] ";
    //}
    //std::cerr << ", and my local count is " << local_result<< std::endl;    
 
    
    // now the root processor queries the other processor
    // for their local results, which it then totals.
    sync();   
    if ( i_am_root ) {       
       for ( i = 0; i < my_gn; i++ ) {
           if ( i == my_groot ) {
              //std::cerr << "I am processor " << my_gid << "<" << i  << ">, adding my local result to the total " << std::endl;    
              result = result + local_result;  
           } else {
              //std::cerr << "I am processor " << my_gid << "<" << i  << ">, receiving a remote result from " << i << std::endl;    
              pgroup->receive_ints( i, 1, &remote_result, 44 );
              result = result + remote_result;
           }
       }
       
    } else {
           //std::cerr << "I am processor " << my_gid << ", sending local result to " << my_groot << std::endl;    
           pgroup->send_ints( my_groot, 1, &local_result, 44 );
    }

    // now the root processor send the final tally to the other processors
    if ( i_am_root ) {       
       for ( i = 0; i < my_gn; i++ ) {
           if ( i != my_groot ) {
              //std::cerr << "I am processor " << my_gid << ", sending final result to " << i << std::endl;    
              pgroup->send_ints( i, 1, &result, 45 );
           }
       }
       
    } else {
           //std::cerr << "I am processor " << my_gid << ", receiving final result from " << my_groot << std::endl;    
           pgroup->receive_ints( my_groot, 1, &result, 45 );
    }
    
    if ( negate) {
       result = num_parcels_total - result;
    }


    return result;
}

int Flock::countStatus( ParcelStatus stat, bool negate )
{
    int result;    
    int local_result;
    int remote_result;
    bool i_am_root;
    int i;
    ParcelStatus mask;
    int my_groot;
    int my_gid;
    int my_gn;
 
    result = 0;
    
    i_am_root = is_root();
    
    // first, take a local count based on our own parcels
    // (met processors can do this too; their local_result will be 0)
    local_result = 0;    
    if ( parcels.size() > 0 ) {
              
       Parcel* p;
       
       for ( i = 0; i < my_num_parcels; i++ ) {      
           if ( parcels[i]->statuses & stat ) {
              local_result ++;
           }
       
       }

    }
    
    // now we have to sum these local results over all processors
    // in this Flock's group.
    
    my_groot = pgroup->root_id();
    my_gid = pgroup->id();
    my_gn = pgroup->size();
    
    //std::cerr << "I am processor " << my_gid;
    //if ( is_met ) {
    //   std::cerr << " [MET] ";
    //} else {
    //   std::cerr << " [TRACING] ";    
    //}
    //if ( i_am_root ) {
    //   std::cerr << "[ROOT] ";
    //}
    //std::cerr << ", and my local count is " << local_result<< std::endl;    
 
    
    // now the root processor queries the other processor
    // for their local results, which it then totals.
    sync();   
    if ( i_am_root ) {       
       for ( i = 0; i < my_gn; i++ ) {
           if ( i == my_groot ) {
              //std::cerr << "I am processor " << my_gid << "<" << i  << ">, adding my local result to the total " << std::endl;    
              result = result + local_result;  
           } else {
              //std::cerr << "I am processor " << my_gid << "<" << i  << ">, receiving a remote result from " << i << std::endl;    
              pgroup->receive_ints( i, 1, &remote_result, 54 );
              result = result + remote_result;
           }
       }
       
    } else {
           //std::cerr << "I am processor " << my_gid << ", sending local result to " << my_groot << std::endl;    
           pgroup->send_ints( my_groot, 1, &local_result, 54 );
    }

    // now the root processor send the final tally to the other processors
    if ( i_am_root ) {       
       for ( i = 0; i < my_gn; i++ ) {
           if ( i != my_groot ) {
              //std::cerr << "I am processor " << my_gid << ", sending final result to " << i << std::endl;    
              pgroup->send_ints( i, 1, &result, 55 );
           }
       }
       
    } else {
           //std::cerr << "I am processor " << my_gid << ", receiving final result from " << my_groot << std::endl;    
           pgroup->receive_ints( my_groot, 1, &result, 55 );
    }
    
    if ( negate) {
       result = num_parcels_total - result;
    }


    return result;
}


void Flock::clear()
{
     int i;
     std::vector<ProcessGrp*>::iterator pi;

     if ( pgroup != NULLPTR ) {
        // sync with all other processors
        pgroup->sync();
   
        // restore the met data source's settings 
        restorePGroup();
     
        // destroy all this Flock's sub-groups
        for ( pi=subgroups.begin(); pi != subgroups.end(); pi++ )
        {
            delete *pi;
        }
   
        // destroy all parcels
        if ( my_num_parcels > 0 ) {
           // parcel-tracer: destroy all parcels
           for (i=0; i < my_num_parcels; i++ ) {
               delete parcels[i];
           }
        } else {
           // and even met-readers have a sample parcel
           delete parcels[0];
        }
     }

     parcels.clear();
     if ( exemplar != NULLPTR ) {
        delete exemplar;
        exemplar = NULLPTR;
     }
     
     my_num_parcels = 0;
     my_parcel_start = 0;
     num_parcels_total = 0;
     
     preserved_met_pgroup = NULLPTR;
     preserved_met_pid = -1;
     preserved_flock_pgroup = NULLPTR;
     preserved_flock_pid = -1;
}

void Flock::allocate( int n, ProcessGrp* pgrp, const Parcel* p, int r )
{
    ProcessGrp* pg;
    bool genParcel;
   
    clear();
   
    if ( pgrp != NULLPTR ) {
       // We make our own copy of the given process group,
       // so that we can delete it ourselves later, in our own destructor
       // (This has the overhead of creating a new MPI process group and communicator.) 
       pg  = pgrp->copy();
       
       // delete our current one
       if ( pgroup != NULLPTR ) {
          delete pgroup;
       }
       
    } else {
       if ( pgroup != NULLPTR ) {
          pg = pgroup;
       } else {
          pg = new SerialGrp();
       }
    }
    
    genParcel = ( p == NULLPTR );
    if ( genParcel ) {
       p = new Parcel;
    }
    
    if ( n >= 0 ) {
       this->setup(*p,pg,n,r);
    } else {
       throw (badparcelcount());
    }           

    if ( genParcel ) {
       delete p;
    }

}     

void Flock::sync()
{
   if ( pgroup != NULLPTR ) {
      pgroup->sync();
   }
}

void Flock::sync(const std::string msg)
{
   if ( pgroup != NULLPTR ) {
      pgroup->sync("Flock::sync: " + msg);
   }
}

void Flock::metDelay() 
{
     if ( parcels.size() > 0 ) {
        ((parcels[0])->getMet())->delay();
     }
}

void Flock::dump() const
{
   // my processor id
   int my_proc;
   // is this processor the root processor?
   int i_am_root;
   // the id of the root processor
   int my_root;
   // number of processors in this group
   int num_procs;
   // subgroup processor 
   ProcessGrp *subgrp;
   
   my_proc = pgroup->id();

   i_am_root = pgroup->is_root();

   my_root = pgroup->root_id();

   num_procs = pgroup->size();

   
   std::cerr << "Flock::Dump: Overall, I am processor #" << my_proc 
             << " in group " << pgroup->group_id() 
             << ", which has " << num_procs << " processors" 
             << " and " << subgroups.size() << " subgroups."
             << std::endl;
             
   // I am a member of subgroup N, which has M processors.
             
   std::cerr << "Flock::Dump: In this group, #" << my_root << " is the root."
             << " I am";
   if ( i_am_root == 0 ) {
      std::cerr << " NOT";
   }
   std::cerr << " the root processor in this group." << std::endl; 
   
   if ( num_procs > 1 ) {
   
      subgrp = subgroups[my_group];
      
      std::cerr << "Flock::Dump: We are multi-processing."
                << " I am processor " << subgrp->id() 
                << " in subgroup " << subgrp->group_id() << "." 
                << std::endl;
      std::cerr << "Flock::Dump: The met processor in my subgroup is #" << my_met << "."
                << " I am";
      if ( is_met == 0 ) {
         std::cerr << " NOT";
      }
      std::cerr << " the met processor in this group." << std::endl; 
      std::cerr << "Flock::Dump:  I am responsible for " << my_num_parcels << " parcels (of " << num_parcels_total << "), "
      << " starting with parcel " << my_parcel_start
      << " and ending with parcel " << my_parcel_start + my_num_parcels - 1 
      << std::endl;
  } else {
      std::cerr << "Flock::Dump: We are using serial processing." << std::endl;
  } 

};
