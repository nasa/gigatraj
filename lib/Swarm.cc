
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

#include "gigatraj/Swarm.hh"
#include "gigatraj/SerialGrp.hh"

using namespace gigatraj;



Swarm::Swarm( int n )
{

   Parcel p;
   int i;
   ProcessGrp* pg;
   
   nav = NULLPTR;
   metsrc = NULLPTR;
   integ = NULLPTR;
   
   lons = NULLPTR;
   lats = NULLPTR;
   zs = NULLPTR;
   ts = NULLPTR;
   tgs = NULLPTR;
   flagsets = NULLPTR;
   statuses = NULLPTR;
   ids = NULLPTR;
   
   sample_p = NULLPTR;
   
   info_size = 0;
   info_inc = 100;
   
   if ( n >= 0 ) {

      // we will do serial processing
      pg = new SerialGrp();
   
      this->setup(p,pg,n,0);

   } else {
      throw (badparcelcount());
   }           


};


     
Swarm::Swarm( ProcessGrp *pgrp, int n, int r)
{
   int i;
   Parcel p;
   ProcessGrp* pg;
   
   nav = NULLPTR;
   metsrc = NULLPTR;
   integ = NULLPTR;
   
   lons = NULLPTR;
   lats = NULLPTR;
   zs = NULLPTR;
   ts = NULLPTR;
   tgs = NULLPTR;
   flagsets = NULLPTR;
   statuses = NULLPTR;
   ids = NULLPTR;
   
   sample_p = NULLPTR;
   
   info_size = 0;
   info_inc = 100;
   
   if ( n >= 0 ) {

      if ( pgrp != NULLPTR ) {
         // We make our own copy of the given process group,
         // so that we can delete it ourselves later, in our own destructor
         // (This has the overhead of creating a new MPI process group and communicator.) 
         pg  = pgrp->copy();
      } else {
         pg = new SerialGrp();
      }   

      this->setup(p,pg,n,r);

   } else {
      throw (badparcelcount());
   }           

};



Swarm::Swarm( const Parcel &p, int n)
{
   int i;
   ProcessGrp* pg;

   nav = NULLPTR;
   metsrc = NULLPTR;
   integ = NULLPTR;
   
   lons = NULLPTR;
   lats = NULLPTR;
   zs = NULLPTR;
   ts = NULLPTR;
   tgs = NULLPTR;
   flagsets = NULLPTR;
   statuses = NULLPTR;
   ids = NULLPTR;
   
   sample_p = NULLPTR;
   
   info_size = 0;
   info_inc = 100;
   
   if ( n >= 0 ) {

      // No processor group was given, so we will do serial processing
      pg = new SerialGrp();
   
      this->setup(p,pg,n,0);

   } else {
      throw (badparcelcount());
   }           
   

};

Swarm::Swarm( const Parcel &p, ProcessGrp* pgrp, int n, int r)
{
    ProcessGrp* pg;

   nav = NULLPTR;
   metsrc = NULLPTR;
   integ = NULLPTR;
   
    lons = NULLPTR;
    lats = NULLPTR;
    zs = NULLPTR;
    ts = NULLPTR;
    tgs = NULLPTR;
    flagsets = NULLPTR;
    statuses = NULLPTR;
   ids = NULLPTR;
   
   sample_p = NULLPTR;
   
   info_size = 0;
   info_inc = 100;
   
    if ( pgrp != NULLPTR ) {
       // We make our own copy of the given process group,
       // so that we can delete it ourselves later, in our own destructor
       // (This has the overhead of creating a new MPI process group and communicator.) 
       pg  = pgrp->copy();
    } else {
       pg = new SerialGrp();
    }   

    this->setup(p,pg,n,r);
}

std::string Swarm::make_proc_id ( const std::string& tag, int i ) const
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

// this is an internal function used by constructors to set up and initialize the Swarm 
void Swarm::setup( const Parcel &p, ProcessGrp* pgrp, int n, int r)
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
   // is this processor the root processor?
   int i_am_root;
   // the id of the root processor
   int my_root;
   // subprocessor id
   std::string subproctag;


   /* and get the navigation, met source, and integration objects */
   nav = p.nav;
   metsrc = p.metsrc;
   integ = p.integ;
   
   if ( sample_p != NULLPTR ) {
      delete sample_p;
   }
   sample_p = p.copy();

   blocksize = 0;
   
   pgroup = pgrp;
      
   // how many processors do we have to work with?
   numprocs = pgroup->size();
   
   i_am_root = pgroup->is_root();

   my_proc = pgroup->id(); 
   
   my_root = pgroup->root_id();

   // take the root process's number-of-particles as the
   // authoritative version, and have all other processors
   // use that.
   for ( int ip = 0; ip < numprocs; ip++ ) {
       //pgroup->sync("Swarm::setup: in propagate-size loop");
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
   //- std::cerr << "New Swarm: r = " << r << std::endl;
   if ( r > 0 && numprocs > 1 ) {
   
      // Cap the number of tracing processors (approx. r) 
      // at the number of total procrssors, minus one 
      // for a met processor.
      if ( r > (numprocs - 1) ) {
         r = numprocs - 1;
      }
   
      // At the Swarm level, the pgroup is set to be a coordinator
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
       
       subproctag = make_proc_id("swmsub" , i);
             
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
                sample_p = p.copy();
                //parcels.push_back(p.copy());
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
               
                  // create the arrays we will need
                  grow( my_num_parcels );
               
                  // push the parcels in, for non-met-processing processors
                  // (the met processing case is handled above)
                  for (int ip=0; ip < my_num_parcels; ip++ ) {
                      lons[ip] = p.lon;
                      lats[ip] = p.lat;
                      zs[ip]   = p.z;
                      ts[ip]   = p.t;
                      tgs[ip]  = p.tg;
                      flagsets[ip] = p.flagset;
                      statuses[ip] = p.statuses;
                      ids[ip] = ip;
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
          
          metsrc->setPgroup( subgroups[i] , my_met );

       }
         

   } // end of (sub)group creation loop

   delete[] grp_proc_list;
   if ( met_procs != NULLPTR ) {
      delete[] met_procs;
   }
   //- std::cerr << "[" << my_proc << "/" << num_traceprocs << "] exiting setup" << std::endl;


};

Swarm::~Swarm()
{
   int i;
   std::vector<ProcessGrp*>::iterator pi;

   // sync with all other processors
   //pgroup->sync("Swarm destructor sync");
   pgroup->sync();
   
   // unset the met process group for this processor
   if ( metsrc != NULLPTR ) {
      metsrc->setPgroup( NULLPTR, -1 );
   }
   
   // destroy all sub-groups
   for ( pi=subgroups.begin(); pi != subgroups.end(); pi++ )
   {
       delete *pi;
   }
   
   // destroy the process group
   delete pgroup;
   
   // destroy all parcels
   if ( lons != NULLPTR ) {
      delete[] lons;
   }
   if ( lats != NULLPTR ) {
      delete[] lats;
   }
   if ( zs != NULLPTR ) {
      delete[] zs;
   }
   if ( ts != NULLPTR ) {
      delete[] ts;
   }
   if ( tgs != NULLPTR ) {
      delete[] tgs;
   }
   if ( flagsets != NULLPTR ) {
      delete[] flagsets;
   }
   if ( statuses != NULLPTR ) {
      delete[] statuses;
   }
   if ( ids != NULLPTR ) {
      delete[] ids;
   }
   if ( sample_p != NULLPTR ) {
      delete sample_p;
   }
   
};

bool Swarm::is_root() const
{
   return pgroup->is_root();
}

// iterators: (must be forward-only!)
//     on begin:
//         sync all procs in group
//         if met, then listen for traffic from rest of group
//     on going past end:
//         if tracer proc, inform met proc it is time to go
//         met proc receives notice form all tracer procs, leaves
//         sync all procs in group
//     sync on greater than end

Swarm::iterator::iterator() 
{
   // default constructor is a dummy that does nothing
   my_parcel = -1;
   my_swarm = NULLPTR;
   loaded = false;
   
};

Swarm::iterator::iterator(int init, Swarm *swm) 
{
    set(init,swm);
};

Parcel& Swarm::iterator::operator*()  
{
    return pcl;
};

Parcel* Swarm::iterator::operator->() 
{
    return &pcl;
};

bool Swarm::iterator::operator==(const iterator& x) const 
{
    return (my_swarm == x.my_swarm) && (my_parcel == x.my_parcel);
};

bool Swarm::iterator::operator!=(const iterator& x) const 
{
    return (my_swarm != x.my_swarm) || (my_parcel != x.my_parcel);
};

// postfix
Swarm::iterator& Swarm::iterator::operator++(int n) 
{
    int idx;
    
    /* before we move on, we need to copy the iterator's Parcel back into the swarm */
    if ( loaded && my_parcel >= 0 && my_parcel < my_swarm->my_num_parcels) {
       idx = my_swarm->ids[my_parcel];
       my_swarm->lons[idx]      = pcl.lon;      
       my_swarm->lats[idx]      = pcl.lat;      
       my_swarm->zs[idx]        = pcl.z;        
       my_swarm->ts[idx]        = pcl.t;        
       my_swarm->tgs[idx]       = pcl.tg;       
       my_swarm->flagsets[idx]  = pcl.flagset;  
       my_swarm->statuses[idx]  = pcl.statuses; 
       loaded = false;
    }
    
    my_parcel++;
    if ( my_parcel < my_swarm->my_num_parcels ) {
       idx = my_swarm->ids[my_parcel];
       pcl.lon = my_swarm->lons[idx];
       pcl.lat = my_swarm->lats[idx];
       pcl.z   = my_swarm->zs[idx];
       pcl.t   = my_swarm->ts[idx];
       pcl.tg  = my_swarm->tgs[idx];
       pcl.flagset = my_swarm->flagsets[idx];
       pcl.statuses = my_swarm->statuses[idx];
       loaded = true;
    } else {
       my_parcel = -1;
       my_swarm->fin();
    }
    return *this;
};

// prefix
Swarm::iterator& Swarm::iterator::operator++() 
{
    int idx;

    /* before we move on, we need to copy the iterator's Parcel back into the swarm */
    if ( loaded && my_parcel >= 0 && my_parcel < my_swarm->my_num_parcels) {
       idx = my_swarm->ids[my_parcel];
       my_swarm->lons[idx]      = pcl.lon;      
       my_swarm->lats[idx]      = pcl.lat;      
       my_swarm->zs[idx]        = pcl.z;        
       my_swarm->ts[idx]        = pcl.t;        
       my_swarm->tgs[idx]       = pcl.tg;       
       my_swarm->flagsets[idx]  = pcl.flagset;  
       my_swarm->statuses[idx]  = pcl.statuses; 
       loaded = false;
    }

    ++my_parcel;
    if ( my_parcel < my_swarm->my_num_parcels ) {
       idx = my_swarm->ids[my_parcel];
       pcl.lon = my_swarm->lons[idx];
       pcl.lat = my_swarm->lats[idx];
       pcl.z   = my_swarm->zs[idx];
       pcl.t   = my_swarm->ts[idx];
       pcl.tg  = my_swarm->tgs[idx];
       pcl.flagset = my_swarm->flagsets[idx];
       pcl.statuses = my_swarm->statuses[idx];
       loaded = true;
    } else {
       my_parcel = -1;
       my_swarm->fin();
    }
    return *this;
};

int Swarm::iterator::index() const
{
    int result = -1;

    if ( my_swarm->my_num_parcels > 0 ) {
       if ( my_parcel >= 0 && my_parcel < my_swarm->my_num_parcels ) {
       
          result = my_swarm->my_parcel_start + my_swarm->ids[my_parcel];

       }
    }
    
    return result;
}

void Swarm::iterator::stop()
{
    int idx;
    
    if ( loaded && my_parcel >= 0 && my_parcel < my_swarm->my_num_parcels) {
       idx = my_swarm->ids[my_parcel];
       my_swarm->lons[idx]      = pcl.lon;      
       my_swarm->lats[idx]      = pcl.lat;      
       my_swarm->zs[idx]        = pcl.z;        
       my_swarm->ts[idx]        = pcl.t;        
       my_swarm->tgs[idx]       = pcl.tg;       
       my_swarm->flagsets[idx]  = pcl.flagset;  
       my_swarm->statuses[idx]  = pcl.statuses; 
       loaded = false;
    }
    
    my_parcel = -1;
    my_swarm->fin();

}

void Swarm::iterator::set(int init, Swarm *swm) 
{
   my_parcel = -1;
   my_swarm = swm; 
   loaded = false;
   int idx;
   
   if ( my_swarm->my_num_parcels >= 0 ) {
      if ( init <  my_swarm->my_num_parcels ) {

         my_parcel = init;
          if ( init >= 0 ) {
            idx = my_swarm->ids[my_parcel];
            pcl.lon = my_swarm->lons[idx];
            pcl.lat = my_swarm->lats[idx];
            pcl.z   = my_swarm->zs[idx];
            pcl.t   = my_swarm->ts[idx];
            pcl.tg  = my_swarm->tgs[idx];
            pcl.flagset = my_swarm->flagsets[idx];
            pcl.statuses = my_swarm->statuses[idx];

            loaded = true;
         }

      } else {
         throw(badparcelindex());
      }      
   } else {
      throw(badparcelindex());
   }      
}

void Swarm::debut()
{
    int n;

    //- std::cerr << "Swarm::debut: [" << pgroup->id() << "/" << pgroup->group_id() 
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
          subgroups in this Swarm. Some filters which require synchronization across the Swarm
          will initiate that sync themselves.
      */
    
       //- std::cerr << "Swarm::debut: [" << pgroup->id() << "/" << pgroup->group_id() 
       //-   << "]:    No parcels!" << std::endl;        
       if ( is_met ) {
          //- std::cerr << "Swarm::debut: [" << pgroup->id() << "/" << pgroup->group_id() 
          //- << "]:    I am Met-reader" << std::endl;        
          metsrc->serveMet();
          //pgroup->sync("Swarm::debut sync for met server, past serveMet");
          //pgroup->sync();
       }
       n = -1;
    }   

    //- std::cerr << "Swarm::debut: [" << pgroup->id() << "/" << pgroup->group_id() 
    //-      << "]: exiting " << std::endl;        

}

void Swarm::fin()
{

     //- std::cerr << "Swarm::fin: [" << pgroup->id() << "/" << pgroup->group_id() 
     //-     << "]: entering" << std::endl;        
     if ( my_met >= 0 ) {
        // if I am a tracing processor, send a termination notice
        // to my met processor.
        
        // if this is a met processor, I should
        // be in method metproc, 
        // listening to tracing processors for requests.
        // when i get all N termination requests,
        // I exit the metproc method.
        
        //- std::cerr << "Swarm::fin: [" << pgroup->id() << "/" << pgroup->group_id() 
        //-  << "]: signaling " << my_met << std::endl;        
        metsrc->signalMetDone();
        
        
     };
     
     // See the description in debut()above for who gets to call
     // this sync with the Swarm.
     //pgroup->sync("Swarm::fin sync");
     //pgroup->sync();

     //- std::cerr << "Swarm::fin: [" << pgroup->id() << "/" << pgroup->group_id() 
     //-     << ": exiting" << std::endl;        
};

Swarm::Iter Swarm::begin() 
{
    Swarm::Iter i;
    int n;
    
    
    this->debut();
    
    n=0; // start at psrcel index 0, regardless of processor
    // if we have no parcels (must be a met reader process)
    if ( my_num_parcels <= 0 ) {
       n = -1;  // unless this is a met-reading processor
    }  
     
    i.set( n, this );
    
    return i;

};

Swarm::Iter Swarm::end()
{
    Swarm::Iter i;
    
    i.set( -1, this );
    
    return i;

};



void Swarm::setNav( PlanetNav& newnav ) 
{

    nav = &newnav;

};
 
PlanetNav* Swarm::getNav() 
{
    return nav;        
};


void Swarm::setMet( MetData& newmet ) 
{
    metsrc = &newmet;
};

MetData* Swarm::getMet() 
{
    return metsrc;
};


void Swarm::grow( int n )
{
     int to_spare;
     unsigned int new_size;
     real* new_lons;
     real* new_lats;
     real* new_zs;
     double* new_ts;
     double* new_tgs;
     ParcelFlag* new_flagsets;
     ParcelStatus* new_statuses;
     int* new_ids;
     
     if ( n < 0 ) {
        // we only grow. We never shrink.
        return;
     }
     
     to_spare = info_size - my_num_parcels;
     
     if ( n > to_spare ) {
        // ok, we need to grow
        
        new_size = info_size + (n / info_inc + 1)*info_inc;
     
        new_lons = new real[new_size];
        new_lats = new real[new_size];
        new_zs   = new real[new_size];
        new_ts   = new double[new_size];
        new_tgs  = new double[new_size];
        new_flagsets = new ParcelFlag[new_size];
        new_statuses = new ParcelStatus[new_size];   
        new_ids = new int[new_size];
        
        if ( lons != NULLPTR ) {
           
           for ( int i=0; i < my_num_parcels; i++ ) {
               new_lons[i] = lons[i];
               new_lats[i] = lats[i];
               new_zs[i]   = zs[i];
               new_ts[i]   = ts[i];
               new_tgs[i]  = tgs[i];
               new_flagsets[i] = flagsets[i];
               new_statuses[i] = statuses[i];
               new_ids[i]  = ids[i];
           }
           
           delete[] ids;
           delete[] statuses;
           delete[] flagsets;
           delete[] tgs;
           delete[] ts;
           delete[] zs;
           delete[] lats;
           delete[] lons;
     
        }
        
        lons = new_lons;
        lats = new_lats;
        zs = new_zs;
        ts = new_ts;
        tgs = new_tgs;
        flagsets = new_flagsets;
        statuses = new_statuses;
        ids = new_ids;
        
        info_size = new_size;
        
     }

}

int Swarm::size()
{
    return num_parcels_total;

};

int Swarm::belongs( const int n ) const
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

void Swarm::set( const int n,  const Parcel& p, const int mode)
{
   int idx;
   int proc_idx;
   int my_parcel_end ;
   std::vector<int>::iterator pits, pite;
   Parcel pcl;

   //- std::cerr << "Swarm::set: entry for parcel " << n << std::endl;
   
   if ( pgroup->type() == ProcessGrp::PGrpRole_MetReader ) {
      //- std::cerr << "Swarm::set: I am a met processor. Bye-bye!" << std::endl;
      return;
   }
   
   if ( n < 0 | n > num_parcels_total ) {
      throw ( Swarm::badparcelindex() );
   }
   
   // is n between myprocessSTART and (myprocessID+myprocessSIZE)?   
   // then if we are the root process, just return the parcel
   // but if we are not the root process, send/receive the parcel

   my_parcel_end = my_parcel_start + my_num_parcels - 1;
   //- std::cerr << "Swarm::set: this processors handles parcels " 
   //- << my_parcel_start << " through " << my_parcel_end << std::endl;
   //- std::cerr << "Swarm::set: my root processor is " << pgroup->root_id() << std::endl; 

   // does the requested parcel belong to this processor?   
   if ( n >= my_parcel_start && n <= my_parcel_end ) {
      //- std::cerr << "Swarm::set:    Parcel " << n << " is MY parcel! (" 
      //- << my_parcel_start << "-" << my_parcel_end << ")" << std::endl;     
      // index relative to the start of this processor's parcels
      idx = ids[n - my_parcel_start];
      
      if ( pgroup->id() != 0 && mode == 0) {
         // we are not the root process, so we have
         // this parcel receive itself from the root
         //- std::cerr << "Swarm::set:    about to receive" << std::endl;
         pcl.receive(pgroup, 0);
         //- std::cerr << "Swarm::set:    received" << std::endl;
         lons[idx]      = pcl.lon;      
         lats[idx]      = pcl.lat;      
         zs[idx]        = pcl.z;        
         ts[idx]        = pcl.t;        
         tgs[idx]       = pcl.tg;       
         flagsets[idx]  = pcl.flagset;  
         statuses[idx]  = pcl.statuses; 
      } else {
         // we are the root processor, and this is our parcel,
         // OR mode!=0.  Either way, we want to use our local copy,
         // so just set it here.
         //- std::cerr << "Swarm::set:    about to copy" << std::endl;
         lons[idx]      = p.lon;      
         lats[idx]      = p.lat;      
         zs[idx]        = p.z;        
         ts[idx]        = p.t;        
         tgs[idx]       = p.tg;       
         flagsets[idx]  = p.flagset;  
         statuses[idx]  = p.statuses; 
         //- std::cerr << "Swarm::set:    copied" << std::endl;
      }
      
   } else {
      
      // This is not our parcel
      // but if we are the root processor (and if we are in the 
      // quasi-serial mode 0), then we must send
      // this parcel out to the processor whose parcel it is
      //- std::cerr << "Swarm::set:    Parcel " << n << " is NOT my parcel." << std::endl;

      if ( pgroup->id() == 0 && mode == 0 ) {
         // which processor owns this parcel?
         proc_idx = this->belongs(n);
         //- std::cerr << "Swarm::set:    but I am root: about to send to " << proc_idx << std::endl;
         p.send( pgroup, proc_idx );
         //- std::cerr << "Swarm::set:    sent " << std::endl;
      }
   
   }

   //- std::cerr << "Swarm::set:   done " << std::endl;

}


// do get like set above
Parcel* Swarm::parcel( int n, int flag ) const
{
   Parcel* p;
   int pid;
   bool found;
   int parcel_proc;
   int my_proc;
   int i;
   int valid_parcel = 0;
   int idx;
   
   if ( pgroup->type() == ProcessGrp::PGrpRole_MetReader ) {
      return NULLPTR;
   }
   
   // sanity-check the index
   if ( n < 0 | n >= num_parcels_total ) {
      throw ( Swarm::badparcelindex() );
   }

   // find the processor to which this parcel index belongs
   parcel_proc = this->belongs(n);

   my_proc = pgroup->id();

   // is this the parcel's owner?
   if ( my_proc == parcel_proc ) {
   
      // get it
      
      p = new Parcel;
      idx = ids[n - my_parcel_start];
      p->lon = lons[idx];
      p->lat = lats[idx];
      p->z   = zs[idx];
      p->t   = ts[idx];
      p->tg  = tgs[idx];
      p->flagset = flagsets[idx];
      p->statuses = statuses[idx];
      
      valid_parcel = 1;

      // If we are the root processor...
      if ( my_proc == 0 ) {

         //std::cerr << "Swarm::parcel: root proc: my parcel " << n << std::endl;         
         // send it out to the other processors?
         if ( flag == 0 ) {
         
            for ( i = 1; i<pgroup->size(); i++ ) {
                // (don't try to send to met-reading processors)
                if ( pclstarts[i] >= 0 ) {
                   p->send(pgroup, i);
                }   
            }
         }
         
      } else {
         //std::cerr << "Swarm::parcel: non-root proc: sending parcel " << n << std::endl;         
         // the owner is not the root processor
         // So send the parcel to the root processor
         p->send(pgroup, 0);
      }

   } else {
   
      // we do not own the parcel
      p = new Parcel;

      // are we the root processor?
      if ( my_proc == 0 ) {
         // then receive the parcel from its owner
         //std::cerr << "Swarm::parcel: root proc: receiving parcel " << n << std::endl;         
         p->receive(pgroup, parcel_proc);
         valid_parcel = 1;
         
         // and send it out to the other parcels, if desired
         if ( flag == 0 ) {
         
            for ( i = 1; i<pgroup->size(); i++ ) {
                // but don't try to send to ourself, or to met-reading processors
                if ( i != parcel_proc && pclstarts[i] >= 0 ) {
                   p->send(pgroup, i);
                }   
            }
         }
      
      } else {
      
         //std::cerr << "Swarm::parcel: non-root proc: ignoring parcel " << n << std::endl;         
         // we are neither the owner nor the root processor
         // nor are we the met-reader
         if ( ! is_met ) {
            if ( flag == 0 ) {
               // receive the parcel from the root processor
               p->receive(pgroup,0);
               valid_parcel = 1;
            }         
         }
      }       
         
   
   }
   
   if ( valid_parcel == 0 ) {
      delete p;
      p = NULLPTR;
   }
      
   return p;

}

Parcel Swarm::get( int n, int flag ) const
{
   Parcel* p;
   Parcel pp;
   
   p = this->parcel(n, flag);
   if ( p != NULLPTR ) {
      pp = *p;
      delete p;
      return pp;
   } else {
      throw (badparcelindex());
   }
}

Parcel Swarm::operator[]( int n )
{
   
   return this->get(n);
   
}

void Swarm::add( const Parcel& p, const int mode )
{
    int lowest_pop = -1;
    int lowest_proc = -1;
    int proc_idx = -1;
    int num_p;
    int my_rank;
    Parcel* parcl;
    int idx;
    std::vector<int>::iterator pits, pite;
    int new_num_parcels;
   
    if ( sample_p == NULLPTR ) {
       sample_p = p.copy();
    }
    
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
        // increment everybody's scopy of the starting and stopping indices
        if ( i > 0 && pclstarts[i] >= 0 ) {
           pclstarts[i]++;
        }
        if ( pclends[i] >= 0 ) {
           pclends[i]++;
        }
        // if I am one of the processors whose indices are to be
        // adjusted, then let's do that now.
        if ( my_rank == i ) {
           my_parcel_start = pclstarts[i];
           // this should be the same as before, except for the lowest_proc processor
           my_num_parcels = pclends[i] - my_parcel_start + 1;
        }
    }
    // oh, and increment the total as well
    num_parcels_total++;
    
    // "Is this my parcel?" asks the local processor
    if ( lowest_proc == my_rank ) {

       // grow the info arrays if we have to
       grow( 1 );

       parcl = p.copy();
    
       // depending on the mode, either use the value we have,
       // or receive the value from the root processor
       if ( mode == 0 ) {
          // receive the value from the root processor
          parcl->receive( pgroup, 0 );
       }

       // add it to our local list
       ids[my_num_parcels - 1] = my_num_parcels - 1;

       idx = ids[my_num_parcels - 1];

       lons[idx]     = parcl->lon;
       lats[idx]     = parcl->lat;
       zs[idx]       = parcl->z;
       ts[idx]       = parcl->t;
       tgs[idx]      = parcl->tg;
       flagsets[idx] = parcl->flagset;
       statuses[idx] = parcl->statuses;

       delete parcl;

    } else {
       // not my parcel. 
       // Am I the root process?
       if ( my_rank == 0 ) {
          // send the value to the new owner
          p.send( pgroup, lowest_proc );
       }

    }

}

int Swarm::arrange()
{
    int i;
    int last_valid;
    int nn;
    real tmplon;
    real tmplat;
    real tmpz;
    int tmpid;
    double tmptime;
    ParcelFlag tmpflag;
    ParcelStatus tmpstatus;
    double tmptag;
    
    nn = 0;
    
    for ( last_valid = my_num_parcels - 1; last_valid >= 0; last_valid-- ) {
        if ( (flagsets[last_valid] == 0) && (statuses[last_valid] == 0) ) {
           break;
        }
    }
    
    if ( last_valid >= 0 ) {
       
       for ( i=0; i<last_valid; i++ ) {
           if ( (flagsets[i] != 0) || (statuses[i] != 0) ) {
              // bad parcel.  Exchange it with the last good parcel
              tmpid     = ids[i];
              tmplon    = lons[i];
              tmplat    = lats[i];
              tmpz      = zs[i];
              tmptime   = ts[i];
              tmptag    = tgs[i];
              tmpflag   = flagsets[i];
              tmpstatus = statuses[i];

              ids[i]      = ids[last_valid];
              lons[i]     = lons[last_valid];
              lats[i]     = lats[last_valid];
              zs[i]       = zs[last_valid];
              ts[i]       = ts[last_valid];
              tgs[i]      = tgs[last_valid];
              flagsets[i] = flagsets[last_valid];
              statuses[i] = statuses[last_valid];

              ids[last_valid]      = tmpid; 
              lons[last_valid]     = tmplon; 
              lats[last_valid]     = tmplat;  
              zs[last_valid]       = tmpz;    
              ts[last_valid]       = tmptime; 
              tgs[last_valid]      = tmptag;  
              flagsets[last_valid] = tmpflag; 
              statuses[last_valid] = tmpstatus; 
              
              last_valid--;
              
           } else {
              nn++;
           }
           
       }      
    
    }
    
    return nn;
    
}

int Swarm::advance( double dt )
{

    int i;
    int j;
    int jj;
    double tyme;
    int nn;
    int num_to_trace;
 
    if ( sample_p != NULLPTR ) {
       
       if ( is_met ) {
          //- std::cerr << "Swarm::dadvance: [" << pgroup->id() << "/" << pgroup->group_id() 
          //- << "]:    I am Met-reader" << std::endl;        
          metsrc->serveMet();
       } else {

          int blk = my_num_parcels;
          if ( blocksize > 0 && blocksize < blk ) {
             blk = blocksize;
          }
       

          num_to_trace = arrange();
       
          tyme = ts[0];

          int* const traceflags = new int[blk];

          i=0;
          while ( i < num_to_trace ) {
                    
              int jmax = i + blk;
              if ( jmax >= my_num_parcels ) {
                 jmax = my_num_parcels - 1;
              }   
              
              int nn = jmax - i + 1;
              
              for ( j = i; j <= jmax; j++ ) {
                  jj = j - i;
                  
                  traceflags[jj] = 0;
                  
                  if ( ( statuses[jj] & (HitBad | HitBdy) )
                    || ( flagsets[jj] & NoTrace ) 
                    || ( (flagsets[jj] & SyncTrace) && (ts[jj] >= tyme) )
                  ) {
                     traceflags[j] = 1;
                  }
              }
                  
              integ->go( nn, &(lons[i]), &(lats[i]), &(zs[i]), flagsets, tyme, metsrc, nav, dt ); 
              
              for ( j = i; j <= jmax; j++ ) {
                  jj = j - i;
                  
                  ts[j] = tyme;
                  
                  if ( traceflags[jj] ) {
                     statuses[j] = statuses[j] | HitBad;
                     flagsets[j] = flagsets[j] | NoTrace;
                  }
              }
              
              i = jmax + 1;
                  
          }
                    
          metsrc->signalMetDone();
       }
    }
    
    return 0;
}

int Swarm::countFlags( ParcelFlag flgs, bool negate )
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
    if ( my_num_parcels > 0 ) {
              
       for ( i = 0; i < my_num_parcels; i++ ) {      
           if ( flagsets[i] & flgs ) {
              local_result ++;
           }
       
       }

    }
    
    // now we have to sum these local results over all processors
    // in this Swarm's group.
    
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

int Swarm::countStatus( ParcelStatus stat, bool negate )
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
    if ( my_num_parcels > 0 ) {
              
       for ( i = 0; i < my_num_parcels; i++ ) {      
           if (statuses[i] & stat ) {
              local_result ++;
           }
       
       }

    }
    
    // now we have to sum these local results over all processors
    // in this Swarm's group.
    
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


void Swarm::sync()
{
   if ( pgroup != NULLPTR ) {
      pgroup->sync();
   }
}

void Swarm::sync(const std::string msg)
{
   if ( pgroup != NULLPTR ) {
      pgroup->sync("Swarm::sync: " + msg);
   }
}

void Swarm::metDelay() 
{
     if ( metsrc != NULLPTR ) {
        metsrc->delay();
     }
}

void Swarm::dump() const
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

   
   std::cerr << "Swarm::Dump: Overall, I am processor #" << my_proc 
             << " in group " << pgroup->group_id() 
             << ", which has " << num_procs << " processors" 
             << " and " << subgroups.size() << " subgroups."
             << std::endl;
             
   // I am a member of subgroup N, which has M processors.
             
   std::cerr << "Swarm::Dump: In this group, #" << my_root << " is the root."
             << " I am";
   if ( i_am_root == 0 ) {
      std::cerr << " NOT";
   }
   std::cerr << " the root processor in this group." << std::endl; 
   
   if ( num_procs > 1 ) {
   
      subgrp = subgroups[my_group];
      
      std::cerr << "Swarm::Dump: We are multi-processing."
                << " I am processor " << subgrp->id() 
                << " in subgroup " << subgrp->group_id() << "." 
                << std::endl;
      std::cerr << "Swarm::Dump: The met processor in my subgroup is #" << my_met << "."
                << " I am";
      if ( is_met == 0 ) {
         std::cerr << " NOT";
      }
      std::cerr << " the met processor in this group." << std::endl; 
      std::cerr << "Swarm::Dump:  I am responsible for " << my_num_parcels << " parcels (of " << num_parcels_total << "), "
      << " starting with parcel " << my_parcel_start
      << " and ending with parcel " << my_parcel_start + my_num_parcels - 1 
      << std::endl;
  } else {
      std::cerr << "Swarm::Dump: We are using serial processing." << std::endl;
  } 

};
