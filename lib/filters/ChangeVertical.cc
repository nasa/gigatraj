
#include "config.h"

#include "gigatraj/ChangeVertical.hh"
using namespace gigatraj;

ChangeVertical :: ChangeVertical( const std::string to, const std::string from, MetData* met )
{
    fromQuantity = from;
    toQuantity = to;
    metsrc = met;
    myMet = false;
}

ChangeVertical :: ~ChangeVertical()
{
    if ( myMet ) {
       if ( metsrc != NULL ) {
          delete metsrc;
       }
    }
}


void ChangeVertical :: setMet( MetData* met ) 
{
     std::string metQ;
     std::string metUnits;
     std::string fromQ;
     std::string fromUnits;
     std::string toQ;


     // set our met source
     metsrc = met;
     myMet = false;
     
     metQ = metsrc->vertical(&metUnits);

     fromQ = fromQuantity;
     fromUnits = "";
     // do we have a from quantity specified?
     if ( fromQ == "" ) {
        // take it from the met source
        fromQ = metQ;
        fromUnits = metUnits;
     } else if ( fromQ != metQ ) {
        // We cannot use this met source as-is, since
        // its vertical coord does not match our from coord
        // So we copy the met source and set its coord
        // to what we need
        metsrc = metsrc->genericCopy();
        // ok if fromUnits is ""; a defaul twill be used
        metsrc->set_vertical(fromQ,fromUnits);
        // mark this source so we know to delete it later
        myMet = true;
     }
     
}


void ChangeVertical :: apply( Parcel& p )
{
     std::string metQ;
     std::string metUnits;
     std::string fromQ;
     std::string toQ;
     std::string midQ;
     bool nonVertical;
     real oldv;
     real midv;
     real newv;
     real longitude, latitude;
     double time;
     MetData* psrc;
     MetData* met;
     
     
     oldv = p.getZ();
     p.getPos( &longitude, &latitude );
     time = p.getTime();
     
     // we are going to be referring to the parcel's met source
     psrc = p.getMet();

     if ( metsrc == NULL ) {
        // no met source specified yet?
        // use the Parcel's met source
        setMet( psrc );
     } 
        
     if ( ! p.queryNonVert() ) {
        // the parcel's vert coord value is legit for its met source
        
        // that means that the parcel's vertical position is
        // consistent with its met source vertical coordinate
        // So the parcel's met source becomes relevent to our purpose here.
                
        fromQ = fromQuantity;
        if ( fromQ == "" ) {
           // if our "from" coordinate is not set,
           // use the parcel met source's vertical coordinate
           fromQ = psrc->vertical();
        } else { 
           // our "from" coordinate is set, and
           // the parcel's NonVert status is clear, so
           // the from coord MUST match the parcel met source's vertical coord
           if ( fromQ != psrc->vertical() ) {
              throw (badvcoordconflict());
           }
        }
        
        // now consider what coordinate variable
        // we need to convert to
        toQ = toQuantity;
        if ( toQ == "" ) {
           // No "to" coordinate has been specified,

           // Note that at this point we know that the 
           // "from" coordinate is the vertical coordinate of
           // the parcel's met source.
           // The only choice we have for the "to" coordinate
           // is the vertical coordinate of our own met source.
           toQ = metsrc->vertical();
           // Do the conversion using the parcel's met source
           newv = psrc->getData( toQ, time, longitude, latitude, oldv, METDATA_THROWBAD );
        } else {
           // We have a desired value for the "to" coordinate
           if ( toQ != psrc->vertical() ) {
              // we can maybe use the psrc met source,

              if ( psrc->legalQuantity(toQ) ) {
                 // yes! go ahead and do the conversion now
                 newv = psrc->getData( toQ, time, longitude, latitude, oldv, METDATA_THROWBAD );
              } else { 
                 // ok ,things are a bit more complicated
                 
                 // Can we do an intermediate conversion?
                 if ( psrc->legalQuantity( metsrc->vertical() ) && metsrc->legalQuantity( toQ ) ) {
                    midv = psrc->getData( metsrc->vertical(), time, longitude, latitude, oldv, METDATA_THROWBAD );
                    newv = metsrc->getData( toQ, time, longitude, latitude, midv, METDATA_THROWBAD );             
                 } else {
                    throw (badvcoordconflict());              
                 }
              
              }
           } else {
              // toQ is the parcel met sources;' vert coord
              // and fromQ is the parcel met source's vert coord
              // which means that toQ == fromQ
              if ( toQ == fromQ ) {
                 // which means that no conversion is needed
                 newv = oldv;
              } else {
                 // really should never get here
                 throw (badvcoordconflict());
              }
           
           }
        } 
     
     } else {
     
        // OK, the percel's current vertical coordinate is NOT
        // the same as its meteorological data sourxce's vertical coordinate
        
        fromQ = fromQuantity;
        if ( fromQ == "" ) {
           // if our "from" coordinate is not set,
           // use our met source's vertical coordinate
           fromQ = metsrc->vertical();
        } else { 
           // our "from" coordinate is set, then
           // the from coord MUST match our met source's vertical coord
           if ( fromQ != metsrc->vertical() ) {
              throw (badvcoordconflict());
           }        
        }
        
        toQ = toQuantity;
        if ( toQ == "" ) {
           // the only thing to choose from is the parcel's met source's vert coord
           toQ = psrc->vertical();
           // Do the conversion using ourmet source
           newv = metsrc->getData( toQ, time, longitude, latitude, oldv, METDATA_THROWBAD );
           
        } else {
           
           // We have a desired value for the "to" coordinate
           if ( toQ != metsrc->vertical() ) {
              // we can maybe use our met source,

              if ( metsrc->legalQuantity(toQ) ) {
                 // yes! go ahead and do the conversion now
                 newv = metsrc->getData( toQ, time, longitude, latitude, oldv, METDATA_THROWBAD );
              } else { 
                 
                 // Can we do an intermediate conversion?
                 if ( metsrc->legalQuantity( psrc->vertical() ) && psrc->legalQuantity( toQ ) ) {
                    midv = metsrc->getData( psrc->vertical(), time, longitude, latitude, oldv, METDATA_THROWBAD );
                    newv = psrc->getData( toQ, time, longitude, latitude, midv, METDATA_THROWBAD );             
                 } else {
                    throw (badvcoordconflict());              
                 }
              
              }
           } else {
              // toQ is our sources;' vert coord
              // and fromQ is our met source's vert coord
              // which means that toQ == fromQ
              if ( toQ == fromQ ) {
                 // which means that no conversion is needed
                 newv = oldv;
              } else {
                 // really should never get here
                 throw (badvcoordconflict());
              }
           
           }
        
        }
     
     
     }
     
     if ( fromQuantity == "" ) {
        fromQuantity = fromQ;
     }
     if ( toQuantity == "" ) {
        toQuantity = toQ;
     }         

     
     p.setZ(newv);

     if ( toQ != psrc->vertical() ) {
        p.setNonVert();
     } else {
        p.clearNonVert();
     }

}

  
  
void ChangeVertical :: apply( Parcel * const p, const int n )
{
     int i;
     
     for (i = 0; i<n; i++ ) {
         
         apply( p[i] );
     
     }

}

void ChangeVertical :: apply( std::vector<Parcel>& p )
{
    std::vector<Parcel>::iterator ip;
    int n;
    int i;
    
    n = p.size();
    
    if ( n <= 0 ) {
       throw (ParcelFilter::badparcelnum());
    };  
    
    for ( ip = p.begin(); ip != p.end(); ip++ ) {
        apply( *ip );
    }

}

void ChangeVertical :: apply( std::list<Parcel>& p )
{
    std::list<Parcel>::iterator ip;
    int n;
    
    n = p.size();
    
    if ( n <= 0 ) {
       throw (ParcelFilter::badparcelnum());
    };  
    
    
   for ( ip=p.begin(); ip != p.end(); ip++ ) {
      apply( *ip );
   }   

};

void ChangeVertical :: apply( std::deque<Parcel>& p )
{
    std::deque<Parcel>::iterator ip;
    int n;
    
    n = p.size();
    
    if ( n <= 0 ) {
       throw (ParcelFilter::badparcelnum());
    };  
    
    
   for ( ip=p.begin(); ip != p.end(); ip++ ) {
      apply( *ip );
   }   

};

void ChangeVertical :: apply( Flock& p )
{
    Flock::iterator iter;
    int n;
    Parcel px;
    
    n = p.size();
    
    if ( n <= 0 ) {
       throw (ParcelFilter::badparcelnum());
    };  
  

    for ( iter = p.begin(); iter != p.end(); iter++ ) {
        px = *iter;
        apply( px );
        *iter = px;
    }    

};

void ChangeVertical :: apply( Swarm& p )
{
    Swarm::iterator iter;
    int n;
    Parcel px;
    
    n = p.size();
    
    if ( n <= 0 ) {
       throw (ParcelFilter::badparcelnum());
    };  
  

    for ( iter = p.begin(); iter != p.end(); iter++ ) {
        px = *iter;
        apply( px );
        *iter = px;
    }    

};
