
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


#include "gigatraj/PGenDisc.hh"

using namespace gigatraj;

PGenDisc::PGenDisc()
{
   startup();

   lon0 = 0.0;
   lat0 = 0.0;
   z0 = 500.0;
   rad = 50.0;
   thk = 1e-10;
   min_sep = 5.0;
   abs_min_sep = 1.0;
   
}

PGenDisc::PGenDisc( const real lon, const real lat, const real level, const real r, const real thickness )
{
   
   startup();

   lon0 = lon;
   lat0 = lat;
   z0 = level;
   rad = r;
   thk = thickness;
   min_sep = 5.0;
   abs_min_sep = 1.0;
}

// destructor
PGenDisc::~PGenDisc()
{

}

// copy constructor
PGenDisc::PGenDisc(const PGenDisc& src) : ParcelGenerator(src)
{
   n_in_circ = src.n_in_circ;    
   cumul_n_in_circ = src.cumul_n_in_circ;   
   lon0 = src.lon0;
   lat0 = src.lat0;
   z0 = src.z0;
   rad = src.rad;
   thk = src.thk;
   min_sep = src.min_sep;
   abs_min_sep = src.abs_min_sep;

}

PGenDisc& PGenDisc::operator=(const PGenDisc& src)
{
    // handle assignment to self
    if ( this == &src ) {
       return *this;
    }
    
    this->assign( src ) ;
    
    return *this;
}

void PGenDisc::assign( const PGenDisc& src)
{

    ParcelGenerator::assign(src);
    
    n_in_circ = src.n_in_circ;    
    cumul_n_in_circ = src.cumul_n_in_circ;   
    lon0 = src.lon0;
    lat0 = src.lat0;
    z0 = src.z0;
    rad = src.rad;
    thk = src.thk;
    min_sep = src.min_sep;
    abs_min_sep = src.abs_min_sep;
}


const void PGenDisc::center( real &lon, real &lat, real &level )   
{
   lon = lon0;
   lat = lat0;
   level = z0;
}


void PGenDisc::set_center( const real lon, const real lat, const real level ) 
{
   lon0 = lon;
   lat0 = lat;
   z0 = level;
}


const void PGenDisc::size( real &r, real &thickness )
{
   r = rad;
   thickness = thk;
}


void PGenDisc::set_size( const real r, const real thickness )
{
   rad = r;
   thk = thickness;
}

void PGenDisc::next_coords( real &lat, real &lon, real &z, int p_no, const int n, PlanetNav *nav )
{
     static int n_layers;
     static int n_parcels;
     static int layer_no; // layer number
     static int max_np; // the max number of parcels in this layer
     static int n_circles; // number of circles in this layer
     static int circle_no; // circle number within a layer
     static int n_p; // the max number of parcels around this circle
     static int p_c; // parcel number within this circle
     static real delang; // the delta angle between successive parcles along the circle  
     static real offang; // bearing offset 
     static real prev_dang; // bearing offset from the previous circle
     static real dz; // increment of layer position
     static real zoff; // offset of layer position
     static real r; // circle radius
     real ang;
     int n_remain;

     n_remain = n - p_no;
         
     if ( p_no == 0 ) {
        // first parcel. initialize everything
        
         n_layers = layer_circle_spec.size();

         offang = 0.0;
         prev_dang = 0.0; 

         zoff = z0;
         dz = 0.0; 
         if ( n_layers > 1 ) {
            zoff = zoff - thk/2.0;
            dz = thk/(n_layers - 1);
         }   

         // init for the loop
         layer_no = -1;
         max_np = -1;
         n_circles = -1; 
         circle_no = -1; 
         n_p = -1;
         p_c = -1; 
     }

     
     p_c ++;

     //std::cerr << "****** next_coords: parcel " << p_no << " of " << n << ", p_c=" << p_c << std::endl; 

     // do we need to advance to the next circle?
     if ( p_c >= n_p ) {
        // yes.
        // on to the next circle
        circle_no++;
        // do we need to advance to the next layer?
        if ( circle_no >= n_circles ) {
           // yes. Advance to the next layer
           layer_no++;
           if ( layer_no >= n_layers ) {
              throw (ParcelGenerator :: badgeneration());
           }        
           circle_no = 0;
           n_circles = layer_circle_spec[layer_no];
           max_np = cumul_n_in_circ[n_circles-1];
           if ( max_np > n_remain ) {
              max_np = n_remain;
           }
           z = zoff + layer_no*dz;
           //std::cerr << "   ++++ layer advanced to " << layer_no << " at z = " << z 
           //<< " with " << max_np << " among " << n_circles << " circles " << std::endl;    
        }
        n_p = n_in_circ[circle_no];
        if ( n_p > max_np ) {
           n_p = max_np;
        }
        p_c = 0;
        offang = prev_dang/2.0;
        prev_dang = delang;
        if ( n_p > 1 ) {
           delang = 360.0/n_p;
        } else {
           delang = 0.0;
        }
        //std::cerr << "   ++ Advanced to next circle: " << circle_no << " of " << n_circles << std::endl;
            
        // the radius of the circle
        if ( n_circles > 1 ) {
           r = rad/(n_circles - 1.0)*circle_no;
        } else {
           r = 0.0;
        }
     }
     //std::cerr << "  --> parcel " << p_no << " with " << n_remain << " remaining" << std::endl;
     //std::cerr << "  position " << p_c << " of circle " << circle_no 
     //          << " (of " << n_p << " pts) of layer " << layer_no 
     //          << " (max " << n_circles << " circles) " << std::endl;
     //std::cerr << "     offang=" << offang << ", delang=" << delang << std::endl;

     // get the angle of this parcel
     ang = p_c * delang + offang;
     
     z = zoff + dz*layer_no;

     //std::cerr << "     r=" << r << ", ang=" << ang << ", z=" << z << std::endl;

     // the angle of the parcel
     nav->displace( lon0,lat0, r,ang, lon,lat);
     
}

template< template<class U, class = std::allocator<U> > class Seq>
void PGenDisc :: init( Seq<Parcel>* seq
                    )                          
{
     PlanetNav *nav;
     real lon, lat, z;
     int n;
     int p_no;
     
     if ( seq->size() <= 0 ) {
        throw  (ParcelGenerator :: badparcelcount());
     }

     // total paercels remaining
     n = seq->size();
     
     calc_p(n);

     try {                                       

         p_no = -1;
         for ( typename Seq<Parcel>::iterator it = seq->begin(); it != seq->end(); it++ ) {
             real lon, lat, z;
                          
             nav = it->getNav();

             // next parcel along the current circle
             p_no++;
             
             // get the parcel's coordinates
             next_coords( lat, lon, z, p_no, n, nav );
             
             
             it->setPos( lon, lat );
             it->setZ( z );
          
         }
    } catch (...) {
         throw (ParcelGenerator :: badgeneration());
    }
                                
     

};

void PGenDisc :: startup()
{
     // this gives us how many points
     // should be arranged in one of
     // the concentric circles in a single layer. 
     n_in_circ.clear();
     n_in_circ.reserve(30);
     n_in_circ.push_back(   1);
     n_in_circ.push_back(   6);
     n_in_circ.push_back(  12);
     n_in_circ.push_back(  18);
     n_in_circ.push_back(  25);
     n_in_circ.push_back(  31);
     n_in_circ.push_back(  37);
     n_in_circ.push_back(  43);
     n_in_circ.push_back(  50);
     n_in_circ.push_back(  56);
     n_in_circ.push_back(  62);
     n_in_circ.push_back(  69);
     n_in_circ.push_back(  75);
     n_in_circ.push_back(  81);
     n_in_circ.push_back(  87);
     n_in_circ.push_back(  94);
     n_in_circ.push_back( 100);
     n_in_circ.push_back( 106);
     n_in_circ.push_back( 113);
     n_in_circ.push_back( 119);
     n_in_circ.push_back( 125);
     n_in_circ.push_back( 131);
     n_in_circ.push_back( 138);
     n_in_circ.push_back( 144);
     n_in_circ.push_back( 150);
     n_in_circ.push_back( 157);
     n_in_circ.push_back( 163);
     n_in_circ.push_back( 169);
     n_in_circ.push_back( 175);
     n_in_circ.push_back( 182);
     
     // this is the cumulative total fo the above number,
     // giving the total number of points in a layer with
     // that many concentric circles     
     cumul_n_in_circ.clear();
     cumul_n_in_circ.reserve(30);
     cumul_n_in_circ.push_back(    1);
     cumul_n_in_circ.push_back(    7);
     cumul_n_in_circ.push_back(   19);
     cumul_n_in_circ.push_back(   37);
     cumul_n_in_circ.push_back(   62);
     cumul_n_in_circ.push_back(   93);
     cumul_n_in_circ.push_back(  130);
     cumul_n_in_circ.push_back(  173);
     cumul_n_in_circ.push_back(  223);
     cumul_n_in_circ.push_back(  279);
     cumul_n_in_circ.push_back(  341);
     cumul_n_in_circ.push_back(  410);
     cumul_n_in_circ.push_back(  485);
     cumul_n_in_circ.push_back(  566);
     cumul_n_in_circ.push_back(  653);
     cumul_n_in_circ.push_back(  747);
     cumul_n_in_circ.push_back(  847);
     cumul_n_in_circ.push_back(  953);
     cumul_n_in_circ.push_back( 1066);
     cumul_n_in_circ.push_back( 1185);
     cumul_n_in_circ.push_back( 1310);
     cumul_n_in_circ.push_back( 1441);
     cumul_n_in_circ.push_back( 1579);
     cumul_n_in_circ.push_back( 1723);
     cumul_n_in_circ.push_back( 1873);
     cumul_n_in_circ.push_back( 2030);
     cumul_n_in_circ.push_back( 2193);
     cumul_n_in_circ.push_back( 2362);
     cumul_n_in_circ.push_back( 2537);
     cumul_n_in_circ.push_back( 2719);
}

void PGenDisc :: calc_p( const int n )
{
     // the minimum separation between parcels
     real minsep; 
     // the number of layers in the disk
     int n_layers;
     // the number of circles in a single layer
     int n_circles;
     // the maximum number of circles in a single layer
     int max_n_circles;
     //
     int max_pts_in_circle;
     // the number of points to put on a circle
     std::vector<int> npc;
     // the remaining number of ppints to allocate to a circle
     int nrem;
     // the number of parcels per layer
     int n_pts_layer;
     // the number of points in a given layer
     int n_p;
     //
     int nc;
     // 
     real trial_min_sep;
     // 
     int trial_n_circle;

     minsep = min_sep;
     
     npc.clear();
     if ( n > 0 ) {
        
        //std::cerr << "number of points = " <<   n  << std::endl;
        
        // this is how many circles can possibly fit within the allowed radius
        // (But we may actually use fewer)
        max_n_circles = TRUNC( rad/minsep + 1);
        if ( max_n_circles > 30 ) {
           // but no more than 30. Let's not get silly.
           max_n_circles = 30;
        }
        //std::cerr << "max num circles = " <<   max_n_circles  << std::endl;
     
        // How many points could such a set of circles hold?
        max_pts_in_circle = cumul_n_in_circ[max_n_circles - 1];

        //std::cerr << "max points in these circles = " <<   max_pts_in_circle  << std::endl;

        /* This may require some explanation.
           If the thickness is very, very thin, then we really want just one layer.
           If the thickness is very large, then we want a lot of layers.
           How thin is thin? Well, divide the thickness by the level, to make it dimensionless,
           and call this the relative thickness. At one extreme, when 
           relative thickness is 0, then we want just 1 layer.
           At the other extreme, if the relative thickness is 1.0(the disk is roughly
           as thick as the atmosphere), then we want every point to have its own layer.
           We construct a linear scale between those two points.
           Throw in an extra factor of two as a refinement, and we get the following
           equation.
        */
        n_layers = TRUNC(  1.0 + (n - 1)/2.0*thk/z0 + 0.5 );
        //std::cerr << "prelim number layers = " <<   n_layers  << std::endl;
     
        // how many points/parcels in each layer?
        n_pts_layer = n / n_layers;
        //std::cerr << "prelim points per layer = " <<   n_pts_layer  << std::endl;
        
        // sanity check
        if ( n_pts_layer > max_pts_in_circle ) {
           /* not workable as is. This number of layers requires more
              points to be in a layer than we can accommodate with the
              available number of circles in a single layer.
              
              We can keep the smallest separation distance, minsep, and compute
              a new n_layers from that.
              Or we can decrease the minimum separation distance to accommodate
              the current n_layers.
           */
           
           // try adjusting the min separation first
           
           // find the number of points in the layer, using the max number of circles
           for ( int i=1; i< cumul_n_in_circ.size(); i++ ) {
               if ( n_pts_layer <= cumul_n_in_circ[i] ) {
                  minsep = rad/i;
                  break;
               }
           }
           
           if ( minsep < abs_min_sep ) {
              // The new minimum separation is too small.
              // So we increase the number of layers, too.

              n_pts_layer = max_pts_in_circle;
              n_layers = n / n_pts_layer;
              if ( n_layers*n_pts_layer < n ) {
                 n_layers++;
              }
           }
           
        }
        //std::cerr << "final number layers = " <<   n_layers  << std::endl;
        //std::cerr << "final points per layer = " <<   n_pts_layer  << std::endl;
        //for ( int i=0; i< cumul_n_in_circ.size(); i++ ) {
        //    std::cerr << "  cumul_n_in_circ[" << i << "] = " 
        //    << cumul_n_in_circ[i] << std::endl;
        //}
        
        // find how many concentric circles we will need to accommodate the points
        // in a single layer
        n_circles = 0;
        for ( int i=0; i< cumul_n_in_circ.size(); i++ ) {
            //std::cerr << "  i,cumul_n_in_circ,n_pts_layer=" << i << ", "
            //<< cumul_n_in_circ[i] << ", " << n_pts_layer << ", " << n_circles  << std::endl;
            if ( n_pts_layer <= cumul_n_in_circ[i] ) {
               n_circles = i + 1;
               //std::cerr << "     Adjusting n_circles to " << n_circles << std::endl;
               break;
            }
        }
        //std::cerr << "final final number of circles = " <<   n_circles  << std::endl;
     
        // Since we have not yet allocated any parcels to any circles or layers,
        // all parcels remain.
        nrem = n;
     
        // for each layer
        layer_circle_spec.clear();
        layer_circle_spec.reserve(n_layers);
        for ( int k=0; k<n_layers; k++ ) {
        
            layer_circle_spec.push_back( n_circles );
            //std::cerr << "Defining layer " << k << " with " <<   n_circles  << " circles " << std::endl;
        
        }

        //std::cerr << "------- end pcalc -----------" << std::endl;        
        
     }
     

}


 
Parcel * PGenDisc :: create_array(Parcel parcel, int n ) 
{
    Parcel* pa;
    PlanetNav *nav;
     
    
    if ( n <= 0 ) {
       throw (ParcelGenerator :: badparcelcount());
    };  
     
    nav = parcel.getNav();

    calc_p(n);

    
    try {

       pa = new Parcel[n];
       // initialize the parcel value
       for (int i=0; i<n; i++ ) {
          real lon, lat, z;

          // get the parcel's coordinates
          next_coords( lat, lon, z, i, n, nav );

          pa[i] = parcel;
          pa[i].setPos( lon, lat );
          pa[i].setZ( z );
       }
    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     

    return pa;


}; 

std::vector<Parcel>* PGenDisc :: create_vector(Parcel parcel, int n 
                                             )
{
    std::vector<Parcel>*bunch;
     
    
    if ( n <= 0 ) {
       throw (ParcelGenerator :: badparcelcount());
    };  
    
    try {
       bunch = new std::vector<Parcel>;
       bunch->reserve(n);
    
       for (int i=0; i<n; i++ ) {
          bunch->push_back(parcel);
       }
       
       PGenDisc::init(bunch);

    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     
    
    return bunch;

};

std::list<Parcel>* PGenDisc :: create_list(Parcel parcel, int n 
                                         )
{
    std::list<Parcel>*bunch;
     
    
    if ( n <= 0 ) {
       throw (ParcelGenerator :: badparcelcount());
    };  
    
    try {
       bunch = new std::list<Parcel>;
    
       for (int i=0; i<n; i++ ) {
          bunch->push_back(parcel);
       }

       PGenDisc::init(bunch);

    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     
    
    return bunch;

};

std::deque<Parcel>* PGenDisc :: create_deque(Parcel parcel, int n 
                                           )
{
    std::deque<Parcel>*bunch;
     
    
    if ( n <= 0 ) {
       throw (ParcelGenerator :: badparcelcount());
    };  
    
    try {
       bunch = new std::deque<Parcel>;
    
       for (int i=0; i<n; i++ ) {
          bunch->push_back(parcel);
       }

       PGenDisc::init(bunch);

    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     
    
    return bunch;

};


Flock* PGenDisc :: create_Flock(const Parcel& p, int n
                   , ProcessGrp* pgrp, int r
                   )                        
{
     // the parcel container
     Flock *flock;
     Flock::iterator ip;         
     real lon,lat,z, rx, ang;
     int status = 0;
     PlanetNav *nav;
     Parcel *pcl;
     

     if ( n <= 0 ) {
        throw (ParcelGenerator :: badparcelcount());
     };  

     try {
     
        // now create a Flock of that many parcels
        flock = new Flock( p, pgrp, n, r);

        calc_p(n);

        // sync all the processors before we start loading
        if ( pgrp != NULLPTR ) {
           pgrp->sync();
        }

        int p_no = -1;
        
        pcl = p.copy();

        nav = pcl->getNav();
          
        for ( int i=0; i<n; i++  ) {
            try {
            
              flock->sync();
                          
              p_no++;
              
              // get the parcel's coordinates
              next_coords( lat, lon, z, p_no, n, nav );
              
               // load the parcel location into the parcel
              pcl->setPos( lon, lat );
              pcl->setZ( z );

              // If we are the root processor, the parcel pcl
              // is valid, and it is sent to the processor
              // to which it belongs.
              // If we are not the root processor, then
              // then pcl has no valid value, but the
              // processor will ignore it and receive its
              // parcel value from the root processor.
              flock->set( i, *pcl, 0 );

            } catch (std::ios::failure) {
               throw (ParcelGenerator :: badgeneration());
            }   
        } 
        
        delete pcl;  
     
    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     


    return flock;

};

Swarm* PGenDisc :: create_Swarm(const Parcel& p, int n
                   , ProcessGrp* pgrp, int r
                   )                        
{
     // the parcel container
     Swarm *swarm;
     Swarm::iterator ip;         
     real lon,lat,z, rx, ang;
     int status = 0;
     PlanetNav *nav;
     Parcel *pcl;
     

     if ( n <= 0 ) {
        throw (ParcelGenerator :: badparcelcount());
     };  

     try {
     
        // now create a Swarm of that many parcels
        swarm = new Swarm( p, pgrp, n, r);

        calc_p(n);

        // sync all the processors before we start loading
        if ( pgrp != NULLPTR ) {
           pgrp->sync();
        }

        int p_no = -1;
        
        pcl = p.copy();

        nav = pcl->getNav();
          
        for ( int i=0; i<n; i++  ) {
            try {
            
              swarm->sync();
                          
              p_no++;
              
              // get the parcel's coordinates
              next_coords( lat, lon, z, p_no, n, nav );
              
               // load the parcel location into the parcel
              pcl->setPos( lon, lat );
              pcl->setZ( z );

              // If we are the root processor, the parcel pcl
              // is valid, and it is sent to the processor
              // to which it belongs.
              // If we are not the root processor, then
              // then pcl has no valid value, but the
              // processor will ignore it and receive its
              // parcel value from the root processor.
              swarm->set( i, *pcl, 0 );

            } catch (std::ios::failure) {
               throw (ParcelGenerator :: badgeneration());
            }   
        } 
        
        delete pcl;  
     
    } catch(...) {
       throw ( ParcelGenerator :: badgeneration() );
    };     


    return swarm;

};
