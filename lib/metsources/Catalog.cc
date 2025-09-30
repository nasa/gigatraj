
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

#include <deque>

#include <math.h>

#include "gigatraj/Catalog.hh"

#include <fstream>
#include <iostream>
#include <sstream>

#include <stdlib.h>

using namespace gigatraj;



Catalog::VarVal::VarVal( char typ  )
{
     type = typ;
     flags = 0;
     fmt1 = -1;
     fmt2 = -1;
}

Catalog::VarVal::VarVal( const std::string& val, char typ )
{
     type = typ;
     fmt1 = -1;
     fmt2 = -1;
     
     nominal.assign( val );
     flags = ValidNominal;
     
     // set the eval* value from the nominal
     n2e();

}
Catalog::VarVal::VarVal( bool val )
{
     type = 'B';
     nominal = "";
     fmt1 = -1;
     fmt2 = -1;
     evalB = val;
     flags = ValidEval | IsLiteral;

}
Catalog::VarVal::VarVal( int val )
{
     type = 'I';
     nominal = "";
     fmt1 = -1;
     fmt2 = -1;
     evalI = val;
     flags = ValidEval | IsLiteral;

}
Catalog::VarVal::VarVal( float val )
{
     type = 'F';
     nominal = "";
     fmt1 = -1;
     fmt2 = -1;
     evalF = val;
     flags = ValidEval | IsLiteral;

}
Catalog::VarVal::VarVal( double val )
{
     type = 'D';
     nominal = "";
     fmt1 = -1;
     fmt2 = -1;
     evalD = val;
     flags = ValidEval | IsLiteral;
     
}

Catalog::VarVal* Catalog::VarVal::copy() const
{
    VarVal* result;
    
    result = new VarVal();
    
    result->nominal = nominal;
    result->type = type;
    result->flags = flags;
    result->evalS = evalS;
    result->evalB = evalB;
    result->evalI = evalI;
    result->evalF = evalF;
    result->evalD = evalD;
    result->fmt1 = fmt1;
    result->fmt2 = fmt2;

    return result;
}

void Catalog::VarVal::convertType( char newtype )
{
    bool ok;
    
    if ( newtype != type ) {
       
       // we need a valid eval in place
       if ( ! hasEval() ) {
          if ( hasNominal() && hasLiteral() ) {
             // at least we have something toconvert from
             
             // convert nominal to eval
             n2e();
             
          } 
       }
       
       ok = true;
       
       switch (type) {
          case 'S':
                 switch (newtype) {
                 case 'S':
                    break;
                 case 'B':
                    ok = s2Bool( evalS, evalB );
                    break;
                 case 'I':
                    ok = s2Int( evalS, evalI );
                    break;
                 case 'F':
                    ok = s2Float( evalS, evalF );
                    break;
                  case 'D':
                    ok = s2Date( evalS, evalD );
                    break;
                 }
                 break;
          case 'B':
                 switch (newtype) {
                 case 'S':
                    ok = b2String( evalB, evalS );
                    break;
                 case 'B':
                    break;
                 case 'I':
                    evalI = ( evalB ) ? 1 : 0;
                    break;
                 case 'F':
                    evalF = (evalB) ? 1.0 : 0.0;
                    break;
                  case 'D':
                    evalD = (evalB) ? 1.0 : 0.0;
                    break;
                 }
                 break;
          case 'I':
                 switch (newtype) {
                 case 'S':
                    ok = i2String( evalI, evalS );
                    break;
                 case 'B':
                    evalB = ( evalI != 0 );
                    break;
                 case 'I':
                    break;
                 case 'F':
                    evalF = evalI;
                    break;
                  case 'D':
                    evalD = evalI;
                    break;
                 }
                 break;
          case 'F':
                 switch (newtype) {
                 case 'S':
                    ok = f2String( evalF, evalS );
                    break;
                 case 'B':
                    evalB = ( evalF != 0.0 );
                    break;
                 case 'I':
                    evalI = evalF;
                    break;
                 case 'F':
                    break;
                  case 'D':
                    evalD = evalF;
                    break;
                 }
                 break;
          case 'D':
                 break;
                 switch (newtype) {
                 case 'S':
                    ok = d2String( evalD, evalS );
                    break;
                 case 'B':
                    evalB = ( evalD != 0.0 );
                    break;
                 case 'I':
                    evalI = evalD;
                    break;
                 case 'F':
                    evalF = evalD;
                    break;
                  case 'D':
                    break;
                 }
       }
       
       if ( ! ok ) {
          std::cerr << "Bad conversion from " << type << " to " << newtype << std::endl;
          throw (badValueConversion());
       }
       
       // change the type, of course
       type = newtype;
       
       // but we do a conversion only if this is a literal
       // (if it's a variable reference, there is nothing to convert)
       if ( hasEval() && hasLiteral() ) {
          e2n();
       }
       
    }

}

void Catalog::VarVal::reconcile()
{
 
    if ( hasEval() ) {
       // if this was a variable reference
       // it isn't anymore: it's a literal now      
       flags = flags | IsLiteral;
          
       e2n();
    
    } else {
    
       if ( hasNominal() && hasLiteral() ) {
          
          n2e();
          
       } 
    
    }
    
}

bool Catalog::VarVal::isLiteral( std::string& str )
{
     bool result;
     int slen;
     int i;
     int literallen;
     
     slen = str.length();
     
     switch (type) {
     case 'S':
           if ( slen > 0 ) {
              literallen = isStringLiteral( str.c_str(), str.size() );
              result = (literallen > 0 );
           } else {
              // the empty string is a valid literal
              result = true;
           }
           break;
     case 'B':
             literallen = isBoolLiteral( str.c_str(), str.size() );
             result = ( literallen > 0 );
           break;
     case 'I':
             literallen = isIntLiteral( str.c_str(), str.size() );
             result = ( literallen > 0 );
           break;
     case 'F':
             literallen = isFloatLiteral( str.c_str(), str.size() );
             result = ( literallen > 0 );
           break;
     case 'D':
             literallen = isDateLiteral( str.c_str(), str.size() );
             result = ( literallen > 0 );
           break;
        
     }
     
     return result;
}


bool Catalog::VarVal::print( std::string& output ) const
{
     bool result;
     
     
     result = false;
     result = "";
     
     if ( flags & (IsLiteral | ValidEval) ) {
        switch (type) {
        case 'S':
                result = s2eString( evalS, output, fmt1, fmt2);
                break;
        case 'B': 
                result = b2String( evalB, output, fmt1);
                break;
        case 'I': 
                result = i2String( evalI, output, fmt1 );
                break;
        case 'F': 
                result = f2String( evalF, output, fmt1, fmt2);
                break;
        case 'D': 
                result = d2String( evalD, output, fmt1, fmt2);
                break;
        }
     }

     return result;
}

void Catalog::VarVal::n2e()
{
     bool islit;

     
     if ( isLiteral( nominal ) ) {
        flags = flags | IsLiteral;
     } else {
        flags = flags & ( ~ IsLiteral );
     }
     
     switch (type) {
     case 'B':
           if ( s2Bool( nominal, evalB )) {
              flags = flags | ValidEval;           
           } else {
               flags = flags & ( ~ValidEval );
           }
           break;
     case 'I':
           if ( s2Int( nominal, evalI )) {
              flags = flags | ValidEval;           
           } else {
               flags = flags & ( ~ValidEval );
           }
           break;
     case 'F':
           if ( s2Float( nominal, evalF )) {
              flags = flags | ValidEval;           
           } else {
               flags = flags & ( ~ValidEval );
           }
           break;
     case 'D':
           if ( s2Date( nominal, evalD )) {
              flags = flags | ValidEval;           
           } else {
              flags = flags & ( ~ValidEval );
           }
           break;
     case 'S':
           islit = isStringLiteral( nominal.c_str(), nominal.size() );
           if ( islit ) {
              if ( s2uString( nominal, evalS )) {
                  flags = flags | ValidEval;
              } else {
                  flags = flags & ( ~ValidEval );
                  evalS = "";
              }
           } else {
              evalS = "";
              flags = flags & ( ~ValidEval );
              int nlen;
              nlen = nominal.size();
              std::string name;
              if ( nlen > 3 ) {
                 size_t ix = 0;
                 int fmt1;
                 int fmt2;
                 std::string xref;
                 extractVarRef( nominal.c_str(), ix, xref, name, &fmt1, &fmt2 );
                 // if the nominal string is just a simple var ref ,
                 // the then the nominal strin must consist of
                 // just the variable name plus "$", "{", and "}".
                 if ( name != "" &&  ( nominal.size() == (name.size() + 3) ) )  {
                    // ok. simple var ref.
                    // stick the name in the Eval field
                    evalS = name;
                    flags = flags | ValidEval;
                 }
              }       
           }   
           break;
     }

}

void Catalog::VarVal::e2n()
{
     bool ok;
     
     ok = print( nominal );
     
     if ( ok ) {
        flags = ( flags | ValidNominal );
     } else {
         flags = flags & ( ~ ValidNominal );
     }

}

void Catalog::VarVal::dump( int indent ) const
{
   std::string spaces;
   
   for ( int i=0; i < indent; i++ ) {
       spaces.push_back(' ');
   }
   
   std::cerr << spaces << "Value:  type " << type ;
   if ( flags & IsLiteral )  {
      std::cerr << spaces << " (literal)";
   } else {   
      std::cerr << spaces << " (expression)";
   } 
   std::cerr << std::endl;
   
   std::cerr << spaces        << "    flags=" << flags << ", formats=" << fmt1 << "," << fmt2 << std::endl;
   if ( flags & ValidNominal )  {
      std::cerr << spaces     << "    nominal=<<" << nominal << ">>" << std::endl;
   }
   if ( flags & ValidEval )  {
      switch (type) {
      case 'S':
          std::cerr << spaces << "    eval=<<" << evalS << ">>" << std::endl;
          break;
      case 'B':
          std::cerr << spaces << "    eval=<<" << evalB << ">>" << std::endl;
          break;
      case 'I':
          std::cerr << spaces << "    eval=<<" << evalI << ">>" << std::endl;
          break;
      case 'F':
          std::cerr << spaces << "    eval=<<" << evalF << ">>" << std::endl;
          break;
      case 'D':
          std::cerr << spaces << "    eval=<<" << evalD << ">>" << std::endl;
          break;
      }
   }       
}

//////////////////////////////////////////////////////////////////////////////////

Catalog::VarOp::VarOp(  )
{
     id = NOP;
     typ = 'U';
     nops = 0;
     priority = -1;
}

Catalog::VarOp::VarOp( Catalog::VarOperator op, int pri )
{
     id = op;

     switch (op) {
     case TestEQ:
           typ = 'B';
           nops = 2;
           priority = 20 + pri;
          break;
     case TestNE:
           typ = 'B';
           nops = 2;
           priority = 20 + pri;
          break;
     case TestGT:
           typ = 'B';
           nops = 2;
           priority = 20 + pri;
          break;
     case TestGE:
           typ = 'B';
           nops = 2;
           priority = 20 + pri;
          break;
     case TestLT:
           typ = 'B';
           nops = 2;
           priority = 20 + pri;
          break;
     case TestLE:
           typ = 'B';
           nops = 2;
           priority = 20 + pri;
          break;
     case TestOR:
           typ = 'B';
           nops = 2;
           priority = 10 + pri;
          break;
     case TestAND:
           typ = 'B';
           nops = 2;
           priority = 10 + pri;
          break;
     case TestNOT:
           typ = 'B';
           nops = 1;
           priority = 30 + pri;
          break;
     case Add:
           typ = 'U';
           nops = 2;
           priority = 50 + pri;
          break;
     case Subtract:
           typ = 'N';
           nops = 2;
           priority = 50 + pri;
          break;
     case Multiply:
           typ = 'N';
           nops = 2;
           priority = 60 + pri;
          break;
     case Divide:
           typ = 'N';
           nops = 2;
           priority = 60 + pri;
          break;
     case Remainder:
           typ = 'N';
           nops = 2;
           priority = 60 + pri;
          break;
     case Negate:
           typ = 'N';
           nops = 1;
           priority = 80 + pri;
          break;
     }
 
 
}

Catalog::VarOp* Catalog::VarOp::copy() const
{
    VarOp* result;
    
    result = new VarOp();
    
    result->id = id;
    result->priority = priority;
    result->nops = nops;
    result->typ = typ;
    
    return result;
}

void Catalog::VarOp::dump( int indent ) const
{
   std::string spaces;
   std::string name;
   
   for ( int i=0; i < indent; i++ ) {
       spaces.push_back(' ');
   }
   std::cerr << spaces << "VarOp Dump: " << std::endl;
   
   switch (id) {
     case TestEQ:
          name = "==";
          break;
     case TestNE:
          name = "!=";
          break;
     case TestGT:
          name = ">";
          break;
     case TestGE:
          name = ">=";
          break;
     case TestLT:
          name = "<";
          break;
     case TestLE:
          name = "<=";
          break;
     case TestOR:
          name = "||";
          break;
     case TestAND:
          name = "&&";
          break;
     case TestNOT:
          name = "!";
          break;
     case Add:
          name = "+";
          break;
     case Subtract:
          name = "-";
          break;
     case Multiply:
          name = "*";
          break;
     case Divide:
          name = "/";
          break;
     case Remainder:
          name = "%";
          break;
     case Negate:
          name = "(-)";
          break;
   }
   
   std::cerr << spaces << "    operator {" << name << "}";
   std::cerr << " type " << typ;
   if ( nops == 1 ) {
      std::cerr << " unary ";
   } else {
      std::cerr << " binary ";
   }    
   std::cerr << " priority " << priority;
   std::cerr << std::endl;

}

//////////////////////////////////////////////////////////////////////////////////

Catalog::VarExprItem::VarExprItem() 
{
     val = NULLPTR;
     op = NULLPTR;
     isVal = false;
     isOp = false;
}

Catalog::VarExprItem::VarExprItem( Catalog::VarVal* v )
{
     val = NULLPTR;
     op = NULLPTR;
     isVal = false;
     isOp = false;
     
     set( v );
}

Catalog::VarExprItem::VarExprItem( Catalog::VarOp* o )
{
     val = NULLPTR;
     op = NULLPTR;
     isVal = false;
     isOp = false;
     
     set( o );
}

Catalog::VarExprItem::~VarExprItem()
{
    clear();
}

void Catalog::VarExprItem::clear()
{
     if ( val != NULLPTR ) {
        delete val;
     }
     if ( op != NULLPTR ) {
        delete op;
     }
     op = NULLPTR;
     isOp = false;

     val = NULLPTR;
     isVal = false;
}

void Catalog::VarExprItem::set( Catalog::VarVal* v )
{
     clear();

     val = v->copy();
     isVal = true; 
}

void Catalog::VarExprItem::set( Catalog::VarOp* o )
{
     clear();
  
     op = o->copy();
     isOp = true;
     
}

void Catalog::VarExprItem::dump( int indent ) const
{
   std::string spaces;
   
   for ( int i=0; i < indent; i++ ) {
       spaces.push_back(' ');
   }
 
   std::cerr << spaces << "_______ VarExprItem: ";
   if ( isVal ) {
      std::cerr << " VALUE " << std::endl;
      val->dump( indent + 10 );   
   } else if ( isOp ) {   
      std::cerr << " OPERATOR " << std::endl;
      op->dump( indent + 10 );
   } else {
      std::cerr << " NULL " << std::endl;
   }
   
   
}
//////////////////////////////////////////////////////////////////////////////////

Catalog::VarExpr::~VarExpr()
{
     for ( int i=0; i<items.size(); i++ ) {
         delete items[i];
     }
}

 void Catalog::VarExpr::add( Catalog::VarVal* v )
 {
      VarExprItem* item;
      
      item = new VarExprItem( v );
      items.push_back( item );
      
 }
 
 void Catalog::VarExpr::add( Catalog::VarOp* o )
 {
      VarExprItem* item;
      
      item = new VarExprItem( o );
      items.push_back( item );
      
 }

void Catalog::VarExpr::dump( int indent ) const
{
   std::string spaces;
   
   for ( int i=0; i < indent; i++ ) {
       spaces.push_back(' ');
   }
   std::cerr << spaces << "_____ VarExpr Dump: " << std::endl;
  
   std::cerr << spaces << "     " << items.size() << " items, ";
   for ( int i=0; i < items.size(); i++ ) {
       std::cerr << spaces << "   item " << i << ":" << std::endl;
       (items[i])->dump( indent + 10 );
   }
     
} 


//////////////////////////////////////////////////////////////////////////////////

Catalog::Variable::Variable( const std::string& naym, char typ )
{
    name = naym;
    type = typ;
    // no values yet
    defined = false;
}

Catalog::Variable::~Variable()
{
     for ( int i=0; i < tests.size(); i ++ ) {
         delete (tests[i]);
     }

     for ( int i=0; i < values.size(); i ++ ) {
         delete (values[i]);
     }


}
int Catalog::Variable::size()
{
    return values.size();
}

void Catalog::Variable::addValue( VarExpr* value, VarExpr* test)
{

     values.push_back(value);
     tests.push_back(test);

     defined = true;
}

void Catalog::Variable::getExprs( size_t index, VarExpr** value, VarExpr** test)
{
      *value = values[index];
      *test  = tests[index];
}

void Catalog::Variable::dump( int indent) const
{
   std::string spaces;
   int i;
   VarExpr* expr;
   
   for ( int i=0; i < indent; i++ ) {
       spaces.push_back(' ');
   }
   
   std::cerr << spaces << "VARIABLE '" << name << "' of type " <<  type << std::endl;
   if ( defined ) {
      std::cerr << spaces << "   Value/test dump: ";
      
      for ( int i=0; i < values.size(); i++ ) {
          std::cerr << spaces << "   Value " << i << ":" << std::endl;
          std::cerr << spaces << "      Test Expression: " << std::endl;
          expr = tests[i];
          expr->dump( indent + 10 );
          std::cerr << spaces << "      Value Expression: " << std::endl;
          expr = values[i];
          expr->dump( indent + 10 );
      } 
   } else {
      std::cerr << spaces << "*** undefined ***" << std::endl;
   }
   std::cerr << spaces << std::endl;
   
}

//////////////////////////////////////////////////////////////////////////////////

Catalog::VarSet::VarSet()
{
    has_locals = false;
    quant = "";
    time = 0.0;
    tag = "";
}

void Catalog::VarSet::define( const std::string& name, const char type )
{
     Variable *var;
     std::map< std::string, Variable>::iterator item;
     char vtype;
     
     if ( exists( name ) ) {
        item = vars.find( name );
        vtype = (*item).second.type;
        if ( vtype != type ) {
           std::cerr << "config Variable mismatch in " << name << ": " << type << " should be " << vtype <<  std::endl;
           throw (badVarType());
        }
     } else {
        var = new Variable;
        var->name = name;
        var->type = type;
        vars.insert( make_pair(name, *var) );
        delete var;
     }

}

void Catalog::VarSet::undefine( const std::string& name )
{
     if ( exists( name ) ) {
        vars.erase( name );
     }
}


bool Catalog::VarSet::exists( const std::string& name ) const
{
    bool result;
    size_t items;
    
    items = vars.count( name );
    
    result = ( items > 0 );

    return result;
} 


void Catalog::VarSet::addValue( const std::string& name, const char type, VarExpr* value, VarExpr* test)
{
     std::map< std::string, Variable>::iterator item;
     
     define( name, type );
     
     item = vars.find( name );
     // (the define() above guarantees that the name was found
     (*item).second.addValue( value, test );

} 

Catalog::Variable* Catalog::VarSet::getVariable(  const std::string& name )
{
     std::map< std::string, Variable>::iterator item;
     Variable* result;
     
     // get the variable to be evaluated
     item = vars.find( name );
     if ( item != vars.end() ) {
        result = &(*item).second;     
     } else {
        result = NULLPTR;
     }
     
     return result;
}


void Catalog::VarSet::clear()
{

     vars.clear();
     has_locals = false;
} 


void Catalog::VarSet::dump( int indent) const
{
   std::map< std::string, Variable>::const_iterator item;
   std::string spaces;
   int i;
   const Variable* var;
   
   for ( int i=0; i < indent; i++ ) {
       spaces.push_back(' ');
   }
   
   std::cerr << spaces << "******* Begin VarSet Dump ********** " << std::endl;
   
   std::cerr << spaces << vars.size() << " variables defined: ";
   
   for ( item = vars.cbegin(); item != vars.cend(); item++ ) {
       var = &(item->second);
       var ->dump();
   }
   std::cerr << spaces << "******* End VarSet Dump ********** " << std::endl;

}

//////////////////////////////////////////////////////////////////////////////////
Catalog::Dimension::Dimension()
{

      name = "";
      units = "";
      mode = 0;
      start = 0.0;
      end = 0.0;
      delta = 0.0;
      n = 0;

}

void Catalog::Dimension::set( float strt, size_t num, float delt)
{
     if ( num > 0 ) {
         
         n = num;         
         start = strt;         
         delta = delt;         
         mode = 2;
     
     }
}

void Catalog::Dimension::set( float strt, float fin, size_t num)
{

     if ( num > 0 ) {
         
         n = num;         
         start = strt;         
         end = fin;         
         mode = 3;
     
     }

}

void Catalog::Dimension::set( std::vector<float>& v )
{
    vals = v;
    mode = 1;
}

std::vector<float>* Catalog::Dimension::values()
{
    std::vector<float>* result;
    
    result = NULLPTR;
    
    switch (mode) {
    case 0: break;
    case 1:
            result = new std::vector<float>;
            *result = vals;
            break;
    case 2:
            result = new std::vector<float>;
            for ( int i=0; i < n ; i++ ) {
                result->push_back( start + i*delta );
            }
            break;
    case 3:
            result = new std::vector<float>;
            if ( n > 1 ) {
               delta = ( end - start )/(n - 1);
            } else {
               delta = 0.0;
            }   
            for ( int i=0; i < n ; i++ ) {
                result->push_back( start + i*delta );
            }
            break;    
    }
    
    return result;
    
}
void Catalog::Dimension::dump( int indent ) const
{
   std::string spaces;
   std::map< std::string, std::string >::const_iterator item;
   
   for ( int i=0; i < indent; i++ ) {
       spaces.push_back(' ');
   }

   std::cerr << spaces << "++++++++Dimension '" << name << "':" << std::endl;
   std::cerr << spaces << "     " << "quantity = '" << quant << "'" << std::endl; 
   std::cerr << spaces << "     " << "units = '" << units << "'" << std::endl; 
   std::cerr << spaces << "     " << "mode = " << mode  << std::endl; 
   if ( mode == 2 ) {
      std::cerr << spaces << "     " << "start = " << start 
                << ", delta = " << delta << ", n = " << n << std::endl; 
   } else if ( mode == 3 ) {
      std::cerr << spaces << "     " << "start = " << start 
                << ", end = " << end << ", n = " << n << std::endl;   
   } else if ( mode == 1 ) {
      std::cerr << spaces << "     " << "values = " << vals[0];
      for ( int i=1; i < vals.size(); i++ ) {
           std::cerr << ", " << vals[i];
      }    
      std::cerr << std::endl;   
   
   }
}


//////////////////////////////////////////////////////////////////////////////////

void Catalog::DimensionSet::define( const Dimension* dim )
{
     Dimension *tar;
     std::map< std::string, Dimension>::iterator item;
     std::string name;
     
     name = dim->name;
     
     if ( exists( name ) ) {
        std::cerr << "Multiple definitions of Dimension " << name <<  std::endl;
        throw (badMultiplyDefinedDimension());
     } else {
        dims.insert( make_pair(name, *dim) );
     }
}

bool Catalog::DimensionSet::exists( const std::string& name ) const
{
    bool result;
    size_t items;
    
    items = dims.count( name );
    
    result = ( items > 0 );

    return result;
}

Catalog::Dimension* Catalog::DimensionSet::getDimension( const std::string& name )
{
     std::map< std::string, Dimension>::iterator item;
     Dimension* result;
     
     // get the variable to be evaluated
     item = dims.find( name );
     if ( item != dims.cend() ) {
        result = &(*item).second;     
     } else {
        result = NULLPTR;
     }

     return result;
}

void Catalog::DimensionSet::clear()
{
     std::map< std::string, Dimension>::iterator item;
     Dimension* dim;
  
     for ( item = dims.begin(); item != dims.end(); item++ ) {
         dim = &(item->second);
         // delete dim;
     }
     
     dims.clear();
}

void Catalog::DimensionSet::dump( int indent ) const
{
   std::map< std::string, Dimension>::const_iterator item;
   std::string spaces;
   int i;
   const Dimension* dim;
   
   for ( int i=0; i < indent; i++ ) {
       spaces.push_back(' ');
   }
   
   std::cerr << spaces << "******* Begin DimensionSet Dump ********** " << std::endl;
   
   std::cerr << spaces << dims.size() << " Dimensions defined: " << std::endl;
   
   for ( item = dims.cbegin(); item != dims.cend(); item++ ) {
       dim = &(item->second);
       dim ->dump();
   }
   
   std::cerr << spaces << "******* End DimensionSet Dump ********** " << std::endl;


}


//////////////////////////////////////////////////////////////////////////////////

Catalog::TimeInterval::TimeInterval()
{
      years = 0;
      months = 0;
      days = 0;
      secs = 0.0;
      allHere = true;
}

size_t Catalog::TimeInterval::parse( const std::string& interval )
{
    size_t result;
    const char *cc;
    
    if ( interval != "" ) {
       cc =  interval.c_str();
       
       allHere = false;
       
       result = parse( cc, interval.size(), 0 );
    
    } else {
       // empty spec means all the data are present
       allHere = true;

       result = 0;
    }
    return result;
}

size_t Catalog::TimeInterval::parse( const char* interval, size_t len, size_t start )
{
    size_t result;
    size_t i;
    int numbr;
    float tyme;
    bool inTime;
    int state;
    
    result = 0;
    
    years = 0;
    months = 0;
    days = 0;
    secs = 0.0;
    
    /* 	state = 
      x00: all done
      x0f: looking for years, months, days, or time
      x07: looking for months, days, or time
      x03: looking for days or time
      x01: looking for time
      x70: in time, looking for hours, minutes, or seconds
      x30; in time, looking for minutes or seconds
      x10: in time, looking for seconds
    */
    state = 0;
    
    inTime = false;
    
    if ( len > 0 ) {
    
       i=start;
       if ( interval[i] == 'P' ) {
         
          result = i;
          
          state = 0x0f;
    
          i++;
          result = i;
          
          while ( (i < len) && (state != 0) ) {
    
             // if we are able to accept times...
             if ( (state & 0x01) && (interval[i] == 'T') ) {
                // we are starting to parse a time interval
                state = 0x70;
             
                i++;
                result = i;
             
             } else {
             
                // we should see a number at this point
                numbr = 0;
                if ( isDigit( interval[i] ) ) {
                   while ( isDigit( interval[i] ) ) {
                      numbr = numbr*10 + ( interval[i] - '0' );
                      i++;
                   }
                   
                   // got an integer. 
                   // The code at the end of the number designates what it is
                   // And our current state specifies what codes are acceptable.
                   // This prevents cods form being repeated, as in "P5Y4Y".
                   if ( (state & 0x0f) && (interval[i] == 'Y') ) {
                      years = numbr;
                      state = 0x07;
                   } else if ( (state & 0x07) && (interval[i] == 'M') ) {
                      months = numbr;
                      state = 0x03;
                   } else if ( (state & 0x03) && (interval[i] == 'D') ) {
                      days = numbr;
                      state = 0x01;
                   } else if ( (state & 0xf0) && (interval[i] == 'H') ) {
                      secs = secs + 3600.0*numbr;
                      state = 0x30;
                   } else if ( (state & 0x30) && (interval[i] == 'M') ) {
                      secs = secs + 60.0*numbr;
                      state = 0x10;
                   } else if ( (state & 0x10) && (interval[i] == 'S') ) {
                      secs = secs + numbr;
                      state = 0;
                   } else {
                      // syntax error--bad code or code in the wrong place
                      result = 0;
                      state = 0;
                      break;
                   }
                   
                   i++;
                   result = i;
       
                   
                } else {
                   // no digit when expecting one
                   result = 0;
                   break;
                }
             }   
          } 
          
       }
    
    }
    

    return result;
}

//////////////////////////////////////////////////////////////////////////////////

std::string Catalog::Target::getAttr( const std::string& attr)
{
     std::string result;
     std::map< std::string, std::string>::iterator item;

     result = "";
     
     item = attrs.find( attr );
     if ( item != attrs.end() ) {
     
        result = item->second;
     
     }
     
     
     return result;

}

void Catalog::Target::dump( int indent ) const
{
   std::string spaces;
   std::map< std::string, std::string >::const_iterator item;
   
   for ( int i=0; i < indent; i++ ) {
       spaces.push_back(' ');
   }

   std::cerr << spaces << "++++++++ Target '" << name << "':" << std::endl;
   std::cerr << spaces << "     " << "base date = " << dbase.nominal 
             << ", base time = " << basetime 
             << ", delta time = " << inctime << std::endl;
   std::cerr << spaces << "     " << attrs.size() << " attributes" << std::endl;
   for ( item = attrs.cbegin(); item != attrs.cend(); item++ ) {
       std::cerr << spaces << "        " << item->first << ": " << item->second << std::endl;
   }
}

//////////////////////////////////////////////////////////////////////////////////

void Catalog::TargetSet::define( const Target* tgt )
{
     Target *tar;
     std::map< std::string, Target>::iterator item;
     std::string name;
     
     name = tgt->name;
     
     if ( exists( name ) ) {
        std::cerr << "Multiple definitions of Target " << name <<  std::endl;
        throw (badMultiplyDefinedTarget());
     } else {
        targets.insert( make_pair(name, *tgt) );
     }
}

bool Catalog::TargetSet::exists( const std::string& name ) const
{
    bool result;
    size_t items;
    
    items = targets.count( name );
    
    result = ( items > 0 );

    return result;
}

Catalog::Target* Catalog::TargetSet::getTarget( const std::string& name )
{
     std::map< std::string, Target>::iterator item;
     Target* result;
     
     // get the variable to be evaluated
     item = targets.find( name );
     if ( item != targets.end() ) {
        result = &(*item).second;     
     } else {
        result = NULLPTR;
     }

     return result;
}

void Catalog::TargetSet::clear()
{
     targets.clear();
}

void Catalog::TargetSet::dump( int indent ) const
{
   std::map< std::string, Target>::const_iterator item;
   std::string spaces;
   int i;
   const Target* tgt;
   
   for ( int i=0; i < indent; i++ ) {
       spaces.push_back(' ');
   }
   
   std::cerr << spaces << "******* Begin TargetSet Dump ********** " << std::endl;
   
   std::cerr << spaces << targets.size() << " targets defined: " << std::endl;
   
   for ( item = targets.cbegin(); item != targets.cend(); item++ ) {
       tgt = &(item->second);
       tgt ->dump();
   }
   
   std::cerr << spaces << "   Template = '" << tgt->templayt << "'" << std::endl;
   
   std::cerr << spaces << "******* End TargetSet Dump ********** " << std::endl;


}


//////////////////////////////////////////////////////////////////////////////////


Catalog::TargetRef::TargetRef()
{
    tname = "";
    units = "";
    scale = 1.0;
    offset = 0.0;
    description = "";
}
Catalog::TargetRef::TargetRef( const std::string& tget, const std::string& uu, float scl, float offst, const std::string& dd )
{
     tname = tget;
     units = uu;
     scale = scl;
     offset = offst;
     description = dd;
}

void Catalog::TargetRef::dump( int indent ) const
{
   std::string spaces;
   int i;
   
   for ( int i=0; i < indent; i++ ) {
       spaces.push_back(' ');
   }
   
   std::cerr << spaces << "    Target '" << tname  << "'" << std::endl;
   std::cerr << spaces << "          units='" << units << "'  scale=" << scale << ", offset " << offset << std::endl; 
   std::cerr << spaces << "          description='" << description << "'" << std::endl;
   
}


//////////////////////////////////////////////////////////////////////////////////


Catalog::Quantity::Quantity( const std::string& nayme, const std::string& stdnayme )
{
     name = nayme;
     stdname = stdnayme;
     
}

void Catalog::Quantity::clear()
{
     tlist.clear();
}

int Catalog::Quantity::size() const
{
    return tlist.size();
}

void Catalog::Quantity::dump( int indent ) const
{
   std::string spaces;
   int i;
   
   for ( int i=0; i < indent; i++ ) {
       spaces.push_back(' ');
   }
   
   std::cerr << spaces << "Quantity Dump for '" << name << "'" << std::endl;
   std::cerr << spaces << "     " << tlist.size() << " targets: " << std::endl;
   for ( int i=0; i < tlist.size(); i++ ) {
       std::cerr << spaces << "    Target " << i << std::endl;
       tlist[i].dump( indent + 5 ); 
   }
      
}

///////////////////////////////////////////


void Catalog::QuantitySet::define( const Quantity* quant )
{
     std::map< std::string, Quantity>::iterator item;
     std::string name;
     
     name = quant->name;
     
     if ( exists( name ) ) {
        std::cerr << "Multiple definitions of Quantity " << name <<  std::endl;
        throw (badMultiplyDefinedQuantity());
     } else {
        quants.insert( make_pair(name, *quant) );
     }
}

bool Catalog::QuantitySet::exists( const std::string& name ) const
{
    bool result;
    size_t items;
    
    items = quants.count( name );
    
    result = ( items > 0 );

    return result;
}

Catalog::Quantity* Catalog::QuantitySet::getQuantity( const std::string& name )
{
     std::map< std::string, Quantity>::iterator item;
     Quantity* result;
     
     // get the variable to be evaluated
     item = quants.find( name );
     if ( item != quants.end() ) {
         result = &(*item).second;     
     } else {
         result = NULLPTR;
     }
     
     return result;
}

void Catalog::QuantitySet::clear()
{
     quants.clear();
}

int Catalog::QuantitySet::size() const
{
     return quants.size();
}

void Catalog::QuantitySet::dump( int indent ) const
{
   std::map< std::string, Quantity>::const_iterator item;
   std::string spaces;
   int i;
   const Quantity* qnt;
   
   for ( int i=0; i < indent; i++ ) {
       spaces.push_back(' ');
   }
   
   std::cerr << spaces << "$$$$$$$$ Begin QuantitySet Dump $$$$$$$$ " << std::endl;
   
   std::cerr << spaces << quants.size() << " quantities defined: " << std::endl;
   
   for ( item = quants.cbegin(); item != quants.cend(); item++ ) {
       qnt = &(item->second);
       qnt ->dump();
   }
   
   
   std::cerr << spaces << "$$$$$$$ End QuantitySet Dump $$$$$$$$$$ " << std::endl;


}


//////////////////////////////////////////////////////////////////////////////////

bool Catalog::isWhitespace( char cc )
{
   bool result;
   
   result = (cc == ' ')
         || (cc == '\t' )
         || (cc == '\r' )
         || (cc == '\n' )
         ; 

   return result;
}


bool Catalog::isLetter( char cc )
{
    bool result;
    
    result =  ( cc >= 'A' && cc <= 'Z' )
           || ( cc >= 'a' && cc <= 'z' );
           
     return result;      
}
      
bool Catalog::isDigit( char cc )
{
   bool result;

   result = ( cc >= '0' && cc <= '9' );
   
   return result;
}


size_t Catalog::isStringLiteral( const char* str, size_t len, size_t start) 
{
   // the length of the literal in our character array
   size_t litlen;
   // index into the character array
   size_t i;
   // the number of characters to scan
   size_t slen;
   // are we in an escape sequence
   bool inEsc;
   // status indicator
   bool ok;
   // test character
   char cc;
   // var ref
   std::string vref;
   // var ref name
   std::string refname;
   // var ref formatingcodes
   int fmt1, fmt2;
   
   litlen = 0;
   i = start;
   slen = len - start;
   inEsc = false;
   
   ok = true;
   
   if ( slen > 0 ) {
      
      for ( i = start; i < len; i++ ) {
          
          if ( inEsc ) {
             // in an escape sequence.
             // any character is valid, so test nothing here.
             // but the escape sequence doe sned with this character
             inEsc = false;
          } else {
              // not in an escape sequence
              if ( str[i] == '\\' ) {
                 // and now we are in an escape sequence
                 inEsc = true;
              } else if ( str[i] == '"' || str[i] == '\'' ) {
                // an unescaped quote. This is an expression
                ok = false;
                break;
              } else if ( str[i] == '$' ) {
                // an unescaped $. Is this the beginning of a variable reference?
                
                bool varref = false;
                
                extractVarRef( str, i, vref, refname, &fmt1, &fmt2 );
                if ( vref != "" ) {
                   varref = true;
                }   

                // if this is a variable reference, then this must be an expression of some kind (including a var ref)
                ok = ! varref;
                
              }

          }
      
      }
   
   }
   
   if ( ok ) {
      litlen = i - start + 1;
   } else {
      litlen = 0;
   }
   
   return litlen;
}

size_t Catalog::isBoolLiteral( const char* str, size_t len, size_t start) 
{

   // the length of the literal in our character array
   size_t litlen;
   // index into the character array
   size_t i;
   // the number of characters to scan
   size_t slen;
   
   litlen = 0;
   i = start;
   slen = len - start;

   if ( slen == 4 ) {
      if ( (str[i] == 't') && (str[i+1] == 'r') && (str[i+2] == 'u') && (str[i+3] == 'e') ) {
         litlen = slen;
      } else {
         litlen = 0;
      }   
   } else if ( slen == 5 ) {
      if ( (str[i] == 'f') && (str[i+1] == 'a') && (str[i+2] == 'l') && (str[i+3] == 's') && (str[i+4] == 'e') ) {
         litlen = slen;
      } else {
         litlen = 0;
      }      
   } else { 
      litlen = 0;
   }

   return litlen;
}

size_t Catalog::isIntLiteral( const char* str, size_t len, size_t start) 
{

   // the length of the literal in our character array
   size_t litlen;
   // index into the character array
   size_t i;
   // the number of characters to scan
   size_t slen;
   // status
   bool ok;
   
   litlen = 0;
   i = start;
   slen = len - start;

   ok = true;
   if ( str[i] == '+' || str[i] == '-' ) {
      i++;
      if ( i >= len ) {
         // only a leading sign with nothing else
         ok = false;
      }
   }
   if ( ok ) { 
      for ( ; i < len; i++ ) {
         if ( str[i] < '0' || str[i] > '9' ) {
            // non-digit
            ok = false;
            break;
         }
      }
   }

   if ( ok ) {
      // i was left 1 past len  at the end of the for loop
      litlen = i - start;
   } else {
      litlen = 0;
   }

   return litlen;
}

size_t Catalog::isFloatLiteral( const char* str, size_t len, size_t start) 
{

   // the length of the literal in our character array
   size_t litlen;
   // index into the character array
   size_t i;
   // the number of characters to scan
   size_t slen;
   // status
   bool ok;
   
   litlen = 0;
   i = start;
   slen = len - start;

   ok = true;
   if ( str[i] == '+' || str[i] == '-' ) {
      i++;
      if ( i >= len ) {
         // only a leading sign with nothing else
         ok = false;
      }
   }
   if ( ok ) { 
      bool got_dec = false;
      for ( ; i < len; i++ ) {
         if ( str[i] < '0' || str[i] > '9' ) {
            // non-digit--it had better be a decimal point
            if ( (! got_dec) && (str[i] == '.' ) ) {
               got_dec = true;
            } else {
               ok = false;
               break;
            }
         }
      }
   }

   if ( ok ) {
      // i was left 1 past len  at the end of the for loop
      litlen = i - start;
   } else {
      litlen = 0;
   }

   return litlen;
}

size_t Catalog::isDateLiteral( const char* str, size_t len, size_t start) 
{

   // the length of the literal in our character array
   size_t litlen;
   // index into the character array
   size_t i;
   // the number of characters to scan
   size_t slen;
   // status
   bool ok;
   
   litlen = 0;
   i = start;
   slen = len - start;

   ok = true;                                                                                           
   // big enough to contain a year?                                                                         
   if ( slen >= 10 ) {                                                                                      
      int limit = slen;                                                                                     
      // check the year--needs to be either 1* (as in 1993) or 2* (as in 2021)                              
      if ( str[i] == '1' || str[i] == '2' ) {
         
         if ( ! ( isdigit(str[i + 1]) && isdigit(str[i + 2]) && isDigit(str[i + 3]) ) ) {
             ok = false;
         }    
                                                                       
         if ( ok ) {                                                                                    
            // year was ok.

            i = i + 4;
                                                                                                            
            // the dash between year and month                                                              
            if ( str[i] != '-' ) {                                                                          
               ok = false;                                                                              
            }                                                                                               
                                                                                                            
            // check the month                                                                              
            if ( ok ) {                                                                                 
               i++;                                                                                         
               if ( str[i] == '0' || str[i] == '1' ) {                                                      
                  i++;                                                                                      
                  if ( ! isDigit(str[i]) ) {                                                     
                     // the second digit of the month is not a digit                                        
                     ok = false;                                                                        
                  }                                                                                         
               } else {                                                                                     
                  // the first digit of the month is not 0 or 1                                             
                  ok = false;                                                                           
               }                                                                                            
            }                                                                                               
         }                                                                                                  
         if ( ok ) {                                                                                    
            // month was ok.                                                                                
                                                                                                            
            i++;                                                                                            

            // the dash between month and day                                                               
            if ( str[i] != '-' ) {                                                                          
               ok = false;                                                                              
            }                                                                                               
                                                                                                            
            // check the day                                                                                
            if ( ok ) {                                                                                 
               i++;                                                                                         
               if ( str[i] >= '0' || str[i] <= '3' ) {                                                      
                  i++;                                                                                      
                  if ( ! isDigit(str[i]) ) {                                                     
                     // the second digit of the day is not a digit                                          
                     ok = false;                                                                        
                  }                                                                                         
               } else {                                                                                     
                  // the first digit of the day is not between 0 and 3                                      
                  ok = false;                                                                           
               }                                                                                            
            }                                                                                               
         }  
         if ( ok && slen >= 13 ) {                                                                      
            // the string is long enough to hold the time, too                                              
                                                                                                            
            i++;                                                                                            
            if ( str[i] != 'T' ) {                                                                          
               ok = false;                                                                              
            }                                                                                               
                                                                                                            
            // inspect the hour                                                                             
            if ( ok ) {                                                                                 
               i++;                                                                                         
               if ( str[i] >= '0' || str[i] <= '2' ) {                                                      
                  i++;                                                                                      
                  if ( ! isDigit(str[i]) ) {                                                     
                     // the second digit of the hour is not a digit                                         
                     ok = false;                                                                        
                  }                                                                                         
               } else {                                                                                     
                  // the first digit of the hour is not between 0 and 2                                     
                  ok = false;                                                                           
               }                                                                                            
            }                                                                                               
                                                                                                            
         }
         if ( ok && slen >= 16 ) {                                                                      
                                                                                                            
            i++;                                                                                            
            if ( str[i] != ':' ) {                                                                          
               ok = false;                                                                              
            }                                                                                               
                                                                                                            
            // inspect the minute                                                                           
            if ( ok ) {                                                                                 
               i++;                                                                                         
               if ( str[i] >= '0' || str[i] <= '5' ) {                                                      
                  i++;                                                                                      
                  if ( ! isDigit(str[i]) ) {                                                     
                     // the second digit of the minute is not a digit                                       
                     ok = false;                                                                        
                  }                                                                                         
               } else {                                                                                     
                  // the first digit of the minute is not between 0 and 5                                   
                  ok = false;                                                                           
               }                                                                                            
            }                                                                                               
                                                                                                            
         }                                                                                                  
         if ( ok && slen >= 19 ) {                                                                      
                                                                                                            
            i++;                                                                                            
            if ( str[i] != ':' ) {                                                                          
               ok = false;                                                                              
            }                                                                                               
                                                                                                            
            // inspect the integer second                                                                           
            if ( ok ) {                                                                                 
               i++;                                                                                         
               if ( str[i] >= '0' || str[i] <= '5' ) {                                                      
                  i++;                                                                                      
                  if ( ! isdigit(str[i]) ) {                                                     
                     // the second digit of the minute is not a digit                                       
                     ok = false;                                                                        
                  }                                                                                         
                                                                                                            
                  if ( slen > 19 ) {                                                                        
                     i++;                                                                                   
                     if ( str[i] == '.' ) {                                                                 
                        // the second has a decimal point                                                   
                        if ( slen > 20 ) {                                                                  
                           i++;                                                                             
                           for (  ; i < slen; i++ ) {                                                       
                               if ( ! isDigit(str[i]) ) {                                        
                                  // a decimal fractional digit is not a digit                              
                                  ok = false;                                                           
                                  break;                                                                    
                               }                                                                            
                           }                                                                                
                        }                                                                                   
                     } else {                                                                               
                        // something after the second digit of the second was not a decimal point           
                        ok = false;                                                                     
                     }                                                                                      
                  }                                                                                         
                                                                                                            
               } else {                                                                                     
                  // the first digit of the minute is not between 0 and 5                                   
                  ok = false;                                                                           
               }                                                                                            
            }                                                                                               
                                                                                                            
         }                                                                                                  
     } else {                                                                                               
        // leading digit of the year was wrong                                                              
        ok = false;                                                                                     
     }                                                                                                      
   } else {                                                                                                 
      // the string was not even big enough for a year                                                      
      ok = false;                                                                                       
   }                                                                                                        

   if ( ok ) {
      litlen = slen;
   } 
                                                                                              
   return litlen;
}


bool Catalog::s2uString( const std::string& str, std::string& result)
{
    int slen;
    bool status;
    
    status = true; // unless something is really messed up
    
    slen = str.length();
    
    result.clear();
    
    for ( int i=0; i < slen ; i++ ) {
        // the Escape charactetr is "\", which we have
        // to escape here to avoid confusing the compiler.
        if ( str[i] == '\\' ) {
           // skip to the next character, to un-escape it
           i++;
           // but not beyond the end of the string
           if ( i >= slen ) {
              status = false;
              break;
           }
        }
        result.push_back( str[i] );
    }
    
    return true;
}


bool Catalog::s2eString( const std::string& str, std::string& result, int wid, int start)
{
    int slen;
    
    slen = str.length();
    
    result.clear();
    
    for ( int i=0; i < slen ; i++ ) {
        // We escape "$" and "\" itself with "\".
        if ( str[i] == '\\' || str[i] == '$' ) {
           result.push_back('\\');
        }
        result.push_back( str[i] );
    }
    if ( (wid > 0) && (result.size() >= start) ) {
       result = result.substr( start, wid );
    }
    
    return true;

}


bool Catalog::s2Bool( const std::string& str, bool&result)
{
    bool status;
    
    status = false;
    if ( str == "true" ) {
       result = true;
       status = true;
    } else if ( str == "false" ) {
       result = false;
       status = true;
    } 
    
    return status;
}


bool Catalog::b2String( bool val, std::string& result, int wid )
{
    bool status;
    
    if ( val ) {
       result = "true";
    } else {
       result = "false";
    }
    if ( wid > 0 ) {
       result = result.substr(0, wid);
    }
     
    status = true;
    
    return status;

}


bool Catalog::s2Int( const std::string& str, int &result )
{
    bool status;
    int sign;
    int i;
    int slen;
    char cc;
    
    sign = 1;
    result = 0;
    status = false;
    
    slen = str.length();
    
    if ( slen > 0 ) {
       i = 0;
       if ( str[i] == '+' ) {
          i++;
       } else if ( str[i] == '-' ) {
          i++;
          sign = -1;
       }
    
       for (  ; i < slen; i++ ) {
           cc = str[i];
           
           if ( ( cc >= '0' ) && ( cc <= '9' ) ) {
              result = result*10 + ( cc - '0' );
           } else {
              // non-digit character
              break;            
           }
       }
       if ( i = slen ) {
           // got all the way to the end of the string with
           // no unexpected characters
           if ( sign < 0 ) {
              result = - result;
           }
           status = true;   
       }
    } 
    
    return status;
}


bool Catalog::i2String( int val, std::string& result, int wid )
{
    bool status;
    std::ostringstream oo;
    
    if ( wid > 0 ) {
       oo.setf( std::ios::fixed );
       oo.width( wid );
       oo.fill('0');
    }   
    oo.precision( 0 );
    oo << val;

    result = oo.str();
    
    status = true;
    
    return status;

}


bool Catalog::s2Float( const std::string& str, float& result )
{
    bool status;
    int sign;
    int i;
    int slen;
    char cc;
    float fctr;
    
    status = false;
    
    sign = 1;
    result = 0.0;
    
    slen = str.length();
    
    if ( slen > 0 ) {
    
       i = 0;
       if ( str[i] == '+' ) {
          i++;
       } else if ( str[i] == '-' ) {
          i++;
          sign = -1;
       }
    
       for (  ; i < slen; i++ ) {
           cc = str[i];
           
           if ( ( cc >= '0' ) && ( cc <= '9' ) ) {
              result = result*10.0 + ( cc - '0' );
           } else {
              break;               
           }
       }
       if ( i < slen ) {
          if ( str[i] == '.' ) {
             i++;
             fctr = 1.0;
             for ( ; i < slen; i++ ) {
                 cc = str[i];
                 if ( ( cc >= '0' ) && ( cc <= '9' ) ) {
                    fctr = fctr/10.0;
                    result = result + ( cc - '0' )*fctr;
                 } else {
                    break;
                 }          
             }
             if ( i == slen ) {
                // made it to the end of the string with no unexpected characters
                status = true;
             }
          }
       } else {
          // at the end of the string, and there was no decimal part, but that is ok
          status = true;
       }

       if ( status && (sign < 0) ) {
           result = - result;
       }
    
    }
    
    return status;

}

bool Catalog::f2String( float val, std::string& result, int wid, int dec )
{
    bool status;
    std::ostringstream oo;
    
    if ( wid > 0 ) {
       oo.setf( std::ios::fixed );
       oo.width( wid );
       if ( dec >= 0 ) {
          oo.precision( dec );
       }
    }   
    oo << val;

    result = oo.str();
    
    status = true;
    
    return status;

}


// the length of each month (in a non-leap year)
// (Note that the month number (January = 1) can
// be used as an index into this array, hence the
// leading 0 element at the beginning.)
const int Catalog::monthlengths[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
// the day of the year at the end of each month (of a non-leap year)
const int Catalog::daynums[13] = { 0,31,59,90,120,151,181,212,243,273,304,334,365 };

bool Catalog::s2Date( const std::string& str, double& result )
{
    bool status;

     // the component parts of a date
     int year, month, day, hours, minutes;
     float seconds;
     // the number of years since 1900
     int yr;
     // the lengths of the current month
     int monthlen;
     // a string input stream, used for converting strings to integers
     std::istringstream itmp;
     // the maximum day of this month
     int dmax;
     // correction term for leap years
     int leapadj;


   // the length of the literal in our character array
   size_t litlen;
   // index into the character array
   size_t i;
   // the number of characters to scan
   size_t slen;
   // status
   bool ok;
   
   
   litlen = 0;
   i = 0;
   slen = str.size();

   year = 0;
   month = 0;
   day = 0;
   hours = 0;
   minutes = 0;
   seconds = 0.0;
   
   ok = true;                                                                                           
   // big enough to contain a year?                                                                         
   if ( slen >= 10 ) {
      int limit = slen;
      // check the year--needs to be either 1* (as in 1993) or 2* (as in 2021)                              
      if ( str[i] == '1' || str[i] == '2' ) {
         year = str[i] - '0';
         for ( int ii=1; (ii < 4) && ok; ii++ ) {
            if ( isdigit(str[i + ii]) ) {
               year = year*10 + ( str[i + ii] - '0');
            } else {
               ok = false;
               break;
            } 
         }
                                                                       
         if ( ok ) {                                                                                    
            // year was ok.

            i = i + 4;
                                                                                                            
            // the dash between year and month                                                              
            if ( str[i] != '-' ) {                                                                          
               ok = false;                                                                              
            }                                                                                               
                                                                                                            
            // check the month                                                                              
            if ( ok ) {                                                                                 
               i++;                                                                                         
               if ( str[i] == '0' || str[i] == '1' ) {  
               
                  month = str[i] - '0';
                                                                   
                  i++;                                                                                      
                  if ( isDigit(str[i]) ) {
                     month = month*10 + (str[i] - '0');
                  } else {                                                     
                     // the second digit of the month is not a digit                                        
                     ok = false;                                                                        
                  }                                                                                         
               } else {                                                                                     
                  // the first digit of the month is not 0 or 1                                             
                  ok = false;                                                                           
               }                                                                                            
            }                                                                                               
         }                                                                                                  
         if ( ok ) {                                                                                    
            // month was ok.                                                                                
                                                                                                            
            i++;                                                                                            

            // the dash between month and day                                                               
            if ( str[i] != '-' ) {                                                                          
               ok = false;                                                                              
            }                                                                                               
                                                                                                            
            // check the day                                                                                
            if ( ok ) {                                                                                 
               i++;                                                                                         
               if ( str[i] >= '0' || str[i] <= '3' ) { 
                  day = str[i] - '0';                                                     
                  i++;                                                                                      
                  if ( isDigit(str[i]) ) {
                     day = day*10 + (str[i] - '0');
                  } else {                                                     
                     // the second digit of the day is not a digit                                          
                     ok = false;                                                                        
                  }                                                                                         
               } else {                                                                                     
                  // the first digit of the day is not between 0 and 3                                      
                  ok = false;                                                                           
               }                                                                                            
            }                                                                                               
         }  
         if ( ok && slen >= 13 ) {                                                                      
            // the string is long enough to hold the time, too                                              
                                                                                                            
            i++;                                                                                            
            if ( str[i] != 'T' ) {                                                                          
               ok = false;                                                                              
            }                                                                                               
                                                                                                            
            // inspect the hour                                                                             
            if ( ok ) {                                                                                 
               i++;                                                                                         
               if ( str[i] >= '0' || str[i] <= '2' ) {
                  hours = str[i] - '0';                                                  
                  i++;                                                                                      
                  if ( isDigit(str[i]) ) {                                                     
                     hours = hours*10 + (str[i] - '0');
                  } else {
                     // the second digit of the hour is not a digit                                         
                     ok = false;                                                                        
                  }                                                                                         
               } else {                                                                                     
                  // the first digit of the hour is not between 0 and 2                                     
                  ok = false;                                                                           
               }                                                                                            
            }                                                                                               
                                                                                                            
         }
         if ( ok && slen >= 16 ) {                                                                      
                                                                                                            
            i++;                                                                                            
            if ( str[i] != ':' ) {                                                                          
               ok = false;                                                                              
            }                                                                                               
                                                                                                            
            // inspect the minute                                                                           
            if ( ok ) {                                                                                 
               i++;                                                                                         
               if ( str[i] >= '0' || str[i] <= '5' ) { 
                  minutes = str[i] - '0';                                                     
                  i++;                                                                                      
                  if ( isDigit(str[i]) ) { 
                     minutes = minutes*10 + (str[i] - '0');
                  } else {                                                    
                     // the second digit of the minute is not a digit                                       
                     ok = false;                                                                        
                  }                                                                                         
               } else {                                                                                     
                  // the first digit of the minute is not between 0 and 5                                   
                  ok = false;                                                                           
               }                                                                                            
            }                                                                                               
                                                                                                            
         }                                                                                                  
         if ( ok && slen >= 19 ) {                                                                      
                                                                                                            
            i++;                                                                                            
            if ( str[i] != ':' ) {                                                                          
               ok = false;                                                                              
            }                                                                                               
                                                                                                            
            // inspect the integer second                                                                           
            if ( ok ) {                                                                                 
               i++;                                                                                         
               if ( str[i] >= '0' || str[i] <= '5' ) { 
                  seconds = str[i] - '0';                                                     
                  i++;                                                                                      
                  if ( isdigit(str[i]) ) {
                     seconds = seconds*10.0 + (str[i] - '0');
                  } else {                                                     
                     // the second digit of the minute is not a digit                                       
                     ok = false;                                                                        
                  }                                                                                         
                                                                                                            
                  if ( slen > 19 ) {                                                                        
                     i++;                                                                                   
                     if ( str[i] == '.' ) {                                                                 
                        // the second has a decimal point                                                   
                        if ( slen > 20 ) {                                                                  
                           i++;
                           float dec = 1.0;                                                                             
                           for (  ; i < slen; i++ ) {  
                               dec = dec/10.0;                                                     
                               if ( isDigit(str[i]) ) {
                                  seconds = seconds + dec*( str[i] - '0' );
                               } else {                                        
                                  // a decimal fractional digit is not a digit                              
                                  ok = false;                                                           
                                  break;                                                                    
                               }                                                                            
                           }                                                                                
                        }                                                                                   
                     } else {                                                                               
                        // something after the second digit of the second was not a decimal point           
                        ok = false;                                                                     
                     }                                                                                      
                  }                                                                                         
                                                                                                            
               } else {                                                                                     
                  // the first digit of the minute is not between 0 and 5                                   
                  ok = false;                                                                           
               }                                                                                            
            }                                                                                               
                                                                                                            
         }                                                                                                  
     } else {                                                                                               
        // leading digit of the year was wrong                                                              
        ok = false;                                                                                     
     }                                                                                                      
   } else {                                                                                                 
      // the string was not even big enough for a year                                                      
      ok = false;                                                                                       
   }                                                                                                        

   if ( ok ) {
     
     // sanity-check the parts
     if ( year < 1583 || month < 1 || month > 12 ) {
        ok = false;
     }   
 
   }
     
   if ( ok ) {
  
     // start with the number of years since 1900     
     yr = year - 1900;
     leapadj = 0;
     // how long is this month?
     monthlen = monthlengths[month];
     // what is the day of the year at the end of this month?
     dmax = daynums[month];
     // adjust for leap years as necessary
     if ( isLeap(year) ) {
        if ( month == 2 ) {
           // Leap February is longer
           monthlen++;
           dmax++;
        } 
        if ( month > 2 ) {
           // Leap post-February will need adjustment
           leapadj ++;
        }      
     }  
     // more sanity checking
     if ( day < 1 || day > monthlen ) {
        ok = false;
     }        

   }
     
   if ( ok ) {
     if ( day > dmax ) {
        day = dmax;
     }
     
     // calculate the days elapsed since 1900-01-01.
     result = static_cast<double>( yr*365+ (yr-1)/4+leapadj + daynums[month-1] + day );
     
     // add in the time as a fractional part of the day
     result = result + ( static_cast<double>(hours) 
                       + ( static_cast<double>(minutes)
                         + static_cast<double>(seconds)/60.0
                         )/60.0 
                       )/24.0;
     
    }
    
    status = ok;

    
    return status;
}

bool Catalog::isLeap( int year )
{
    bool result;
    
    result =(   ((year % 4) == 0) 
             && ( ((year % 100) != 0) || (( year % 400) == 0 ) )
            ); 

    return result;
}

void Catalog::d2parts( double val, int& year, int& month, int& dayofmonth, int& dayofyear, int& hours, int& minutes, float& seconds )
{
    // a calculation term
    int residual;
    // used for adjusting for leap years
    int leapadj = 0;
    bool is_leap;
    double day;
    double day2;
    double d0;
    // the time of the day
    double time;
    int y,d,m;
    int iday, nyr;

    day = floor(val);
     
    // now do the time
    time = (val - day) * 24.0 ;
    hours = static_cast<int>( time );
    minutes =  static_cast<int>(( time - static_cast<double>(hours) )*60.0 );
    seconds = ( time - static_cast<double>(hours) )*3600.0 - minutes*60.0;
    
    // round up if we have to
    if ( seconds >= 59.99999999 ) {
       seconds = seconds - 60.0;
       if ( seconds < 0 ) {
          seconds = 0.0;
       }
       minutes = minutes + 1.0;
    }
    if ( minutes >= 59.5 ) {
       minutes = minutes - 60.0;
       if ( minutes < 0.0 ) {
          minutes = 0.0;
       }
       hours = hours + 1.0;
    }
    if ( hours >= 23.5 ) {
       hours = hours - 24.0;
       if ( hours < 0 ) {
          hours = 0;
       }
       day = day +  + 1;
    }


    day2 = day + 693900.0;
    y = floor( (day2 + 1.478)/365.2425 );
    if ( day2 < (365*y+y/4-y/100+y/400) ) {
       y--;
    }
    d = floor( day2 - (365*y+y/4-y/100+y/400) );
    m = (100*d+52)/3060;
    month = ((m+2) % 12)+1;
    year = y+(m+2)/12;
    dayofmonth = d - (m*306+5)/10+1;
    
    iday = static_cast<int>( day );
    nyr = (iday*100)/36525;
    
    dayofyear = iday - nyr*365 - (nyr - 1)/4;
/*      
    is_leap = isLeap(year);
    d0 = (y - 1900)*365 + (y - 1900 -1)/4;
    if ( is_leap ) {
       d0 = d0 + 1.0;
    }
    dayofyear = d - d0;
*/       

}


bool Catalog::d2String( double val, std::string& result, int wid, int start )
{
    bool status;
    // the output date string
    std::string date;
    // the component parts of the day
    int year, month, dayofmonth, dayofyear, hours, minutes;
    float seconds;
    // a calculation term
    int residual;
    // used for adjusting for leap years
    int leapadj = 0;
    // string output stream used for converting integers to strings
    std::ostringstream otmp;
    // the time of the day
    double time;
    double day2;
    int y,d,m;
    int lastmon;
    int endidx;
    bool is_leap;
    double day;
    
    status = false;
    
    result.clear();
    
    
    d2parts( val, year, month, dayofmonth, dayofyear, hours, minutes, seconds );
     
    if ( start < 0 ) {
       start = 0;
    }    
    if ( wid <= 0 ) {
       wid = 19;
    }  
    
    // what is the index of the last character in the string?
    endidx = start + wid - 1;
    
    
    // now round according to the output format
    
    if ( endidx <= 18 ) {
       // no floating-point seconds, so round them to integer
       seconds = roundf( seconds );
    }
    while ( seconds >= 60.0 ) {
       minutes = minutes + 1;
       seconds = seconds - 60.0;
    }
    if ( endidx <= 15) {
       // no seconds at all--round the minutes
       if ( seconds >= 30.0 ) {
          seconds = 0;
          minutes = minutes + 1;
       }
    }     
    while ( minutes >= 60.0 ) {
       hours = hours + 1.0;
       minutes = minutes - 60.0;
    }
    if ( endidx <= 12 ) {
       // no minutes--round to hours
       if ( minutes > 30 ) {
          minutes = 0;
          hours = hours + 1;
       }
    }

    // At this point, we could round hours to days.
    // But it seems wrong to round 2012-04-15T13:45:35 to
    // 2012-04-16. But what about 2012-04-15T23:59:59.99999?
    // It seems right to round that to 2012-04-16.
    while ( hours >= 24 ) {
       hours = hours - 24;
       dayofmonth = dayofmonth + 1;
       lastmon = monthlengths[month];
       if ( isLeap(year) && ( month == 2 ) ) {
          lastmon++;
       }
       if ( dayofmonth > lastmon ) {
          dayofmonth = 1;
          month++;
          if ( month > 12 ) {
             month = 1;
             year ++;
          }
       }
    }
    

    // set the formatting: decimal (base-10) output
    otmp.setf(std::ios::dec, std::ios::basefield);
    otmp.setf(std::ios::fixed, std::ios::floatfield);
    // all numbers should be printed with leading zeroes
    otmp.fill('0');
    // fill from the front
    otmp.setf(std::ios::internal, std::ios::adjustfield);
    // year is four digits
    otmp.width(4);
    // do the year and the dash that follows
    otmp << year << "-";
    // months is two digits
    otmp.width(2);
    // do the month and the dash that follows
    otmp << month << "-";
    // day is two digits
    otmp.width(2);
    // do the day
    otmp << dayofmonth;
    // do the "T" that separates date from time
    otmp << "T";
    // hour is two digits
    otmp.width(2);
    // do the hour
    otmp << hours;
    // do the ":" that separates hour from minute
    otmp << ":";
    // minute is two digits
    otmp.width(2);
    // do the minute
    otmp << minutes;
    // do the ":" that separates minute from second
    otmp << ":";
    if ( endidx <= 18 ) {
       // set the output to two integer digits
       otmp.width(2);
       // do the second
       otmp << static_cast<int>(seconds + 0.5);
    } else {
       // set the output to two digits before the decimal
       // followed by four decimal digits
       
       int secwid = endidx - 18 - 1;
       
       otmp.width( 3 + secwid );
       otmp.precision(secwid);
       otmp << seconds;
    }
    
    // now retrieve the formatted string from the output stream.
    date = otmp.str();
    if ( wid > 0 ) {
       result = date.substr( start, wid );
       status = true;
    }
    
    return status;

}



int Catalog::findNext( char ch, const char* str, size_t start, int stop )
{
    int result;
    
    if ( stop < start ) {
       stop = -1;
    }
    for ( result = start; ! ((str[result] == 0) || ((stop < 0) && (result == stop))); result++ ) {
        if ( str[result] == ch ) {
           break;
        }
    }
    
    if ( str[result] == 0 ) {
       result = -1;
    }
    
    return result;

}

int Catalog::scanFor( char ch, const char* str, size_t start, int stop )
{
    int result;
    bool inEsc;
    bool inQuote;
    bool insQuote;
    char cc;
    bool done;
    
    done = false;
    inEsc = false;
    inQuote= false;
    insQuote = false;
    if ( stop < start ) {
       stop = -1;
    }
    for ( result = start; ! ((str[result] == 0) || ((stop < 0) && (result == stop))); result++ ) {
    
        cc = str[result];
    
        if ( ! inEsc ) {
           if ( cc == '\\' ) {
              inEsc = true;
           }
           switch (cc) {
           case '\\':  // this escapes the next charcater 
                     inEsc = true;
                     break;
           case '$': // unescaped, this starts a variable reference
                     // and must not be counted as a regular '$'
                     cc = 0xff;
                     break;
           case '"': // unescaped, this starts or ends a quote
                     inQuote = ! inQuote;                     
                     if ( cc == ch ) { 
                        done = true;
                     }
                     break;
           case '\'': // unescaped, this starts or ends a single-quote
                     insQuote = ! insQuote;                     
                     if ( cc == ch ) { 
                        done = true;
                     }
                     break;
           default: // everything else gets passed as-is, except
                    // we do not scan inside quoted text
                     if ( (cc == ch) && (! inQuote) && (! insQuote ) ) { 
                        done = true;
                     }
                     break;
           }
           
        } else {
           // escaped characters
           switch (cc) {
           case 'n': cc = '\n';
                     break;
           case 'r': cc = '\r';
                     break;
           case 't': cc = '\t';
                     break;
           default: // everything else gets passed as-is
           
                     // but it matches only if it is not in quoted text
                     if ( (cc == ch) && (! inQuote) && (! insQuote ) ) { 
                        done = true;
                     }
                     break;
           }

        }   
        
        if (done ) {
           break; // out of the loop
        }
    }
    
    if ( str[result] == 0 ) {
       result = -1;
    }
    
    return result;

}

int Catalog::matchAgainst( const std::string& match, const char* str, size_t start, int stop )
{
    int result;
    bool inEsc;
    char cc;
    bool matched;
    int ir;
    int im;

    matched = true;
    inEsc = false;  
    if ( stop < start ) {
       stop = -1;
    }
    for ( ir = start, im=0; ! ((str[ir] == 0) || ((stop < 0) && (ir == stop))) && (im < match.size()); ir++, im++ ) {
    
        cc = str[ir];
        if ( ! inEsc ) {
           if ( cc == '\\' ) {
              inEsc = true;
           }
        } else {
           if ( cc != 0 ) {
              inEsc = false;
           }
        }   
        if ( (! inEsc) && (cc != match[im] )) {
           matched = false;
           break; 
        }
            
    }
        
    if ( matched ) {
       result = ir;
    } else {
       result = -1;
    }

    return result;
}

void Catalog::extractVarRef( const char* str, size_t& idx, std::string& name, std::string& ref, int* fmt1, int* fmt2 )
{
     int j;
     int i;
     size_t k;
     std::string s;

     name = "";
     ref = "";
     *fmt1 = -1;
     *fmt2 = -1;
     
     i = idx;
     if ( str[i] == '$' ) {
        i++;
      
        // check for a format number
        if ( isDigit(str[i]) ) {
           while ( isDigit(str[i]) ) {
               s.push_back( str[i] );
               i++;
            }
            if ( ! s2Int( s, *fmt1 ) ) {
               std::cerr << "Bad format integer " << s << " in variable ref in '" << str << "'" << std::endl;
               throw (badConfigSyntax()); 
            }
            s.clear();
         
           // check for a  period and a second format number
           if ( str[i] == '.' ) {
              i++;
              if ( isDigit(str[i]) ) {
                 while ( isDigit(str[i]) ) {
                    s.push_back( str[i] );
                    i++;
                 }
                if ( ! s2Int( s, *fmt2 ) ) {
                   std::cerr << "Bad format integer " << s << " in variable ref in '" << str << "'" << std::endl;
                   throw (badConfigSyntax()); 
                }
                s.clear();               
              }
           }      
        } 
        // now start in on the reference proper
        if ( str[i] == '{' ) {
           j = scanFor( '}', str, i );
           if ( j > i ) {
              k = 0;
              ref = getIdentifier( str + i + 1, k );
              k = k + i + 1;
              if ( (ref != "") && (k == j) ) {
                 s.clear();
                 s.insert( 0, str + idx, j - idx + 1 );
                 idx = j + 1;   
                 
                 name = s;            
              }
           }
        }
     } 

}


std::string Catalog::getIdentifier(  const char* str, size_t& idx )
{
    char cc;
    std::string ident;
    
    ident = "";
    if ( isLetter( str[idx] ) ) {
       
       ident.push_back( str[idx] );
    
       idx++;
       
       while (  (  isLetter( str[idx] ) 
                || isDigit( str[idx] ) 
                || str[idx] == '_'
                ) && ( str[idx] != 0 ) ) {
        
           ident.push_back( str[idx] );
           
           idx++;
                   
       }
    }

    return ident;
}


std::string Catalog::getWeirdIdentifier(  const char* str, size_t& idx )
{
    char cc;
    std::string ident;
    int nOpenParens;
    
    ident = "";
    if ( str[idx] == '(' ) {
       idx++;
       
       nOpenParens= 1;
       
       while ( str[idx] != 0 ) {
          // the string is not ended yet
          
          // count any open parens
          if ( str[idx] == '(' ) {
             nOpenParens++;
          }
          // and close any parens we encounter as well.
          if ( str[idx] == ')' ) {
             nOpenParens--;
          }
          
          if ( nOpenParens > 0 ) {
             // so long as we have an open parenthesis,
             // we add this character to the weird identifier 
             ident.push_back( str[idx] );
             
             // next char
             idx++;
             
          } else {
             // we close our opening parenthesis, so
             // the identifier must be complete
             
             // advance beyond the closing parenthesis
             idx++;
             break;
          }
          
       }
    
       if ( nOpenParens > 0 ) {
          std::cerr << "Error in weird identifier: " << str << std::endl;
          throw (badConfigSyntax());
       }
    
    }
    
    return ident;
}

void Catalog::timeSpacing( double tinc, double toff )
{
     des_tinc = tinc;
     des_toff = toff;
}

double Catalog::timeSpacing( double* toff )
{
     if ( toff != NULLPTR ) {
        *toff = des_toff;
     }
     
     return des_tinc;
}      

void Catalog::desired( const std::string& attr, const std::string& value, int priority )
{
     des_attrs[ attr ] = value;
     des_priorities[ attr ] = priority;
}   

std::string Catalog::desired( std::string& attr, int* priority ) const
{
     std::string result;
     std::map<std::string, std::string>::const_iterator item;
     std::map<std::string, int>::const_iterator pitem;
     int pri;
     
     result = "";
     pri = 0;
     
     if ( des_attrs.count( attr ) > 0 ) {
        item = des_attrs.find( attr );
        result = item->second;
        pitem = des_priorities.find( attr );
        pri = pitem->second;
     }
     
     if ( priority != NULLPTR ) {
        *priority = pri;
     }
     
     return result;
}

Catalog::DataSource::DataSource()
{
    name = "";
    time = "";
    target = "";
    units = "";
    scale = 1.0;
    offset = 0.0;
    description = "";
    type = -1;
    t0 = 0.0;
    t1 = 0.0;
    pre = "";
    post = "";
}

void Catalog::DataSource::dump( int indent ) const
{
   std::string spaces;
   std::string stuff;
   
   for ( int i=0; i < indent; i++ ) {
       spaces.push_back(' ');
   }
   
   std::cerr << spaces << "@@@@@@@@@@@@@@@ DataSource Dump:"  << std::endl;
   std::cerr << spaces << " Quantity: " << name << " at time " << time << std::endl;
   std::cerr << spaces << " units=" << units << ", scale=" << scale << ", offset=" << offset<< std::endl;
   std::cerr << spaces << " description='" << description << "'" << std::endl;
   std::cerr << spaces << "Target = " << target << std::endl;
   switch (type ) {
   case -1: stuff = "Unknown";
            break;
   case  0: stuff = "filename";
            break;
   case  1: stuff = "file";
            break;
   case  2: stuff = "URL";
            break;
   
   }
   std::cerr << spaces << " Type = " << type << " (" << stuff << ")" << std::endl;
   std::cerr << spaces << " Pre : (" << t0 << ") '" << pre  << "'" << std::endl;
   std::cerr << spaces << " Post: (" << t1 << ") '" << post << "'" << std::endl;
   
}


Catalog::Catalog( const std::string& tagg )
{
     my_confLocator = "";
     dbug = 0;
     currentTarget = NULLPTR;
     des_tinc = 0.0;
     des_toff = 0.0;
     nrules = 0;
          
     if ( tagg != "" ) {
        confLocator( tagg );
        
        std::string cfgfile = findConfig();
        if ( cfgfile == "" ) {
           std::cerr << "Empty config file name" << std::endl;
           throw (badNoFile());
        }
        
        load(cfgfile);
        
     }
}

void Catalog::debug( int level)
{
    dbug = level;
}

int Catalog::debug()
{    
    return dbug;
    
}      

void Catalog::confLocator( const std::string& tagg )
{
    my_confLocator = tagg;

}
   
std::string Catalog::confLocator()
{
    return my_confLocator;
}


std::string Catalog::findConfig() const
{
    std::string result;
    std::string dir;
    
    result = "";
    
    if ( my_confLocator != "" ) {
        
        result = my_confLocator + ".cat";
        
        if ( ! fileExists( result ) ) {
           // not in the current working directory
           
           // try the env variable
           dir = getEnv("GIGATRAJ_CATALOGS");
           if ( dir != "" ) {
              dir = dir + "/";
              if ( fileExists( dir + result ) ) {
                 result = dir + result;
              }
           } else {                                                                    
              // try the prefix data location                                          
              dir = GTCONFIGDIR;                                                       
              dir = dir + "/cat/";                                                      
              if ( fileExists( dir + result ) ) {                                      
                   result = dir + result;                                              
              }                                                                        
           }                                                                           
        
        }
    
    }
    
    return result;

}

void Catalog::load( const std::string& filename )
{
  const int MAXLINE = 2048;
  char cline[MAXLINE - 1];
  char *cbeg;
  std::string line;
  bool more;
  std::string fname;
  std::string cfloc;
  
  cfloc = confLocator();
  clear();
  confLocator( cfloc );
  
  if ( filename == "" ) {
     fname = findConfig();
  } else {
      fname = filename;
  }
  
  if ( fname != "" ) {
     std::ifstream input(fname.c_str());
     if ( ! input.good() ) {
        std::cerr << "Could not open '" << fname << "'" << std::endl;
        throw (badNoFile());
     }
     
     more = true;
     while ( more ) {
     
        std::getline( input, line );
        if ( ! input.good() ) {
           more = false;
        } 
        if ( line.size() > 0 ) {
           addRule( line );           
        }
          
     }
     
     input.close();
     
     if ( dbug > 10 ) {    
        varset.dump();  
        targetset.dump(); 
        quantityset.dump();  
     }

     
  } else {
     std::cerr << "Cannot open empty filename" << std::endl;
     throw (badNoFile());
  }

}

void Catalog::addRule( std::string& cfg )
{

   // remove leading and trailing whitespace
   trim( cfg );
   
   // skip empty lines
   if ( cfg.size() > 0 ) {
      // skip comments
      if ( cfg[0] != '#' ) {
         if ( dbug > 0 ) {
            std::cerr << "******* new config line <<" << cfg << ">>" << std::endl; 
         }
         parse( cfg );
         
         nrules++;

      }
   }
      
}

bool Catalog::query( const std::string& quantity, const std::string& validAt,  std::vector<DataSource>& dests, const std::string& tag )
{
     Quantity* quant;
     TargetRef qt;
     int it;
     std:string tname;
     double tyme;
     double preTime, postTime;
     double xt;
     int nt; 
     std::string s1, s2;
     VarVal* pattern;
     bool ok;
     DataSource dd;
     bool result;
     double preDelta;
     double postDelta;
     int preN;
     int postN;
     std::string preStart;
     std::string postStart;
     bool junk;
     double preURLtime;
     double postURLtime;
     
     result = false;
     
     if ( nrules <= 0 ) {
        load();
     }
     
     dests.clear();
     
     if ( dbug > 10 ) {
        std::cerr << "Catalog::Starting query for " << quantity 
                  << " at " << validAt << " with tag=" << tag << std::endl;
     }
     
     if ( ! s2Date( validAt, tyme ) ) {   
        std::cerr << "Improper date-time specifcation '" << validAt << "'" << std::endl;
        throw (badDateString());     
     }
     
     quant = quantityset.getQuantity( quantity );
     if ( quant != NULLPTR ) {
     
        for (it = 0; it < quant->tlist.size(); it++ ) {
            
            qt = quant->tlist[it];
            tname = (quant->tlist[it]).tname;
        
            if ( targetset.exists( tname ) ) {
            
               currentTarget = targetset.getTarget( tname );
                            
              // get the times that bracket this one
              // (note: for this it is not necessary for the dbase to have the date right
              // We just need the base time right)
              
               // set up local variables, so that bracket() is called with the built-in time variabled defined
               setup_vars( quantity, tyme, tag );
               
               bracket( tyme, preTime, preURLtime, preN, postTime, postURLtime, postN, nt );
               
               // ok, now we need to set the temp variables to the actual Pre time
               setup_vars( quantity, preURLtime, tag );
                           
               ok = false;
            
               // resolve the pattern
               pattern = new VarVal( currentTarget->templayt, 'S' );
               if ( dbug > 60 ) {
                  std::cerr << "*+*+*+*+ Catalog::query: pattern VarVal is:" << std::endl;
                  pattern->dump(10);
               }
               
               if ( eval( pattern ) ) {
                  s1 = pattern->evalS;
                  if ( dbug > 60 ) {
                     std::cerr << "*+*+*+*+ Catalog::query: finishd pattern eval: " << s1 << std::endl;
                  }
                  
                  // get the time characteristics of the target
                  //get_targetTimeInfo( *currentTarget, &preDelta, preStart, &preN );
                  
                  if ( preURLtime < tyme ) {
                  
                     setup_vars( quantity, postURLtime, tag );
                     delete pattern;
                     pattern = new VarVal( currentTarget->templayt, 'S' );
                     if ( dbug > 10 ) {
                        std::cerr << "Catalog::query pattern=" << std::endl;
                        pattern->dump();
                     }
                     if ( eval( pattern ) ) {
                        s2 = pattern->evalS;
                        if ( dbug > 10 ) {
                           std::cerr << "Catalog::query interpolated pattern='" << s2 << "'" << std::endl;
                        }

                        // get the time characteristics of the target
                        //get_targetTimeInfo( *currentTarget, &postDelta, postStart, &postN );

                        ok = true;
                     }

                 }  else {
                    s2 = s1;
                    postTime = preTime;
                    postURLtime = preURLtime;
                    postN = preN;
                    ok = true;
                 }
               }
               delete pattern;
               
               if ( ok ) {
                  dd.name = quantity;
                  dd.time = validAt;
                  dd.dims = quant->dims;
                  dd.target = tname;
                  dd.units = (quant->tlist[it]).units;
                  dd.scale  = (quant->tlist[it]).scale;
                  dd.offset = (quant->tlist[it]).offset;
                  dd.description = (quant->tlist[it]).description;
                  dd.tN = nt;
                  
                  dd.preStart_t = preURLtime;
                  junk = d2String( preTime, dd.t0 );
                  junk = d2String( preURLtime, dd.preStart );
                  dd.preN = preN;
                  
                  //dd.tDelta = ( postURLtime - preURLtime )/( preN + 1);
                  dd.tDelta = currentTarget->inctime/24.0;
                  if ( dd.tDelta == 0 ) {
                     if ( preN > 0 ) {
                        dd.tDelta = ( preTime - preURLtime )/( preN );
                     } else {
                        if (currentTarget->nt > 0 ) {
                           dd.tDelta = ( postURLtime - preURLtime )/(currentTarget->nt);
                        } else {
                           dd.tDelta = ( postURLtime - preURLtime );                        
                        }
                     }
                  }
                  
                  //dd.t1 = postTime;
                  // dd.postStart_t = preURLtime;
                  dd.postStart_t = postURLtime;
                  junk = d2String( postTime, dd.t1 );
                  junk = d2String( postURLtime, dd.postStart );
                  dd.postN = postN;
                  
                  if ( s1[0] == '/' 
                    || s1.substr(0,2) == "./"
                    || s1.substr(0,3) == "../" 
                    || s1.substr(0,7) == "file://" ) {
                     dd.type = 0;
                  } else if ( s1.substr(0,8) == "https://" || s1.substr(0,7) == "http://" ) {
                     dd.type = 1;
                  } else if ( s1.substr(0,6) == "OTF://" ) {
                      dd.type = 2;
                  } else {
                      dd.type = -1;
                  }
                  dd.pre = s1;
                  dd.post = s2;
                  
                  dests.push_back( dd );
               }
            
            } else {
               std::cerr << "Quantity '" << quantity << "' references an undefined target '" << tname << "'" << std::endl;
               throw (badUnknownTarget());
            }
        
        }
     
     } 
     
     if ( dests.size() > 0 ) {
     
        filter_DataSource( dests );
     
        result = true;
     }
     
     if ( dbug > 10 ) {
        std::cerr << "Catalog::Leaving query " << std::endl;
     }
     
     return result;
}

std::string Catalog::stdLookup( std::string& stdname )
{
   std::map< std::string, std::string >::iterator item;
   std::string result = "";

     if ( nrules <= 0 ) {
        load();
     }
     
    if ( quantityset.stdnames.count( stdname ) ) {
       item = quantityset.stdnames.find( stdname );
       if ( item != quantityset.stdnames.end() ) {
          result = (*item).second;
       }
    }
    
    return result;

}


std::string Catalog::attrName( int index ) 
{
    std::string result;
    
    result = "";
     
    if ( nrules <= 0 ) {
       load();
    }
    
    if ( index >= 0 && index < attrNames.size() ) {
       result = attrNames[ index ];
    }
    
    return result;
}

int Catalog::attrIndex( const std::string& name ) 
{
    int result;
    
    result = -1;
    
    if ( nrules <= 0 ) {
       load();
    }
    
    for ( int i = 0; i < attrNames.size(); i++ ) {
        if ( attrNames[i] == name ) {
           result = i;
           break;
        }
    }

    return result;
}   

std::string Catalog::getAttr( const std::string& target, int index )
{
     std::string result;
     Target* t;
     std::string attr;
     
     result = "";
     
    if ( nrules <= 0 ) {
       load();
    }
    
     if ( index >= 0 && index < attrNames.size() ) {
     
        t = targetset.getTarget( target );
        if ( t != NULLPTR ) {
            result = t->getAttr( attr );
        }
     
     }
     
     return result;
}

std::string Catalog::getAttr( const std::string& target, const std::string& attr )
{
     std::string result;
     Target* t;
     
     result = "";
     
     if ( nrules <= 0 ) {
        load();
     }
    
     t = targetset.getTarget( target );
     if ( t != NULLPTR ) {
         result = t->getAttr( attr );
     }
     
     return result;
}

std::string Catalog::getAttr( const DataSource& dest, const std::string& attr )
{
     std::string result;
     Target* t;
     std::string target;
     
     result = "";

     if ( nrules <= 0 ) {
        load();
     }
    
     target = dest.target;     
     t = targetset.getTarget( target );
     if ( t != NULLPTR ) {
         result = t->getAttr( attr );
     }
     
     return result;
}

void Catalog::get_timeSpacing( const DataSource& dest, double* tinc, std::string& toff, int* n )
{
     std::string result;
     Target* t;
     std::string target;
     double tx;
     
     result = "";

     if ( nrules <= 0 ) {
        load();
     }
    
     target = dest.target;     
/*
     t = targetset.getTarget( target );
     if ( t != NULLPTR ) {
         *tinc = t->inctime;
         *n = t->nt;
         toff = dest.preStart;
     }
*/
     *tinc = dest.tDelta;
     *n = dest.tN;
     toff = dest.preStart;
     
}

void Catalog::get_targetTimeInfo( Target& targ, double* tinc, std::string& toff, int* n )
{
     std::string result;
     std::string target;
     double tx;
     bool junk;
     VarVal* tmpVal;
     
     result = "";

     tmpVal = targ.dbase.copy();
     eval( tmpVal );
     if ( ! tmpVal->hasEval() ) {
        std::cerr << "Bad Target base date: " << targ.dbase.nominal << std::endl;
        throw (badDateString());
     }
     // get the starting time of the file
    junk = d2String( tmpVal->evalD, toff );
    *tinc = targ.inctime;
    *n = targ.nt;
    
    delete tmpVal;
     
}

bool Catalog::variableValue( const std::string& name, std::string& output )
{
     bool result;
     VarVal* value;
     
     result = false;

     if ( nrules <= 0 ) {
        load();
     }
    
     value = getValue( name );
     if ( value != NULLPTR ) {
        // and this is what we will return
        result = value->print( output );
        
        delete value;
     }             
     
     return result;
}

bool Catalog::dimensionValues( const std::string& name, std::string& quantity, std::string& units, std::vector<float>** values )
{
     bool result;
     Dimension* dim;
     
     result = false;
     units = "";
     if ( values != NULLPTR ) {
        *values = NULLPTR;
     }
       
     if ( dimset.exists(name) ) {
        dim = dimset.getDimension( name );
        result = true;
        units = dim->units;
        quantity = dim->quant;
        if ( values != NULLPTR ) {
           *values = dim->values();
        }
        
        //delete dim;
     }
     
     return result;
     
}    


bool Catalog::fileExists( const std::string& fname ) const
{
  bool result;
  
  result = false;
  
  if ( fname != "" ) {
  
     std::ifstream input(fname.c_str());
     if ( input.good() ) {
        input.close();
        result = true;
     }
     
  
  }

  return result;
  
}


std::string Catalog::getEnv( const std::string& varname ) const
{
   std::string varval;
   char *envval;
   
   varval = "";
   
   if ( varname != "" ) {
      envval = getenv( varname.c_str() );
      if ( envval != NULL ) {
         varval = std::string( envval );
      }
   }
   
   return varval;
   
}


void Catalog::clear()
{

     varset.clear();
     attrNames.clear();
     dimset.clear();
     targetset.clear();
     quantityset.clear();
     des_attrs.clear();
     des_priorities.clear();

     my_confLocator = "";
     first_date = "";
     final_date = "";
     currentTarget = NULLPTR;
     des_tinc = 0.0;
     des_toff = 0.0;
     nrules = 0;

}


void Catalog::trim( std::string& line ) const
{
     char cc;
     size_t i;
     size_t len;
     
     len = line.size();
     if ( len > 0 ) {
        for ( i=0; i < len; i++ ) {
            cc = line[i];
            if ( cc != ' ' && cc != '\t' ) {
               break;
            }
        }
        if ( i < len ) {
           // found a non-whitespace character
           // trim the leading whitespace from the string
           line = line.substr( i, len - i + 1);
        } else {
           // no non-whitespace found
           // make this the empty string
           line.clear();
        }
        // now trim the end
        len = line.size();
        if ( len > 0 ) {
           for ( i=len - 1; i >= 0; i-- ) {
              cc = line[i];
              if ( cc != ' ' && cc != '\t' ) {
                 break;
              }
           }
           if ( i >= 0 ) {
              line = line.substr( 0, i + 1 );
           } else {
              line.clear();
           }
        }
     }
     
}

void Catalog::skipwhite( const char* str, size_t& idx ) const
{
     while ( isWhitespace( str[idx] ) && ( str[idx] != 0 ) ) {
        idx++;
     }

}


bool Catalog::isVarType( char cc ) const
{
    bool result;
    
    result =  ( cc == 'I' )
           || ( cc == 'F' ) 
           || ( cc == 'S' )
           || ( cc == 'D' )
           || ( cc == 'B' );
           
     return result;      

}



void Catalog::parseAttrNames( const char* str, size_t& idx )
{
     std::string name;
     
     // only the attribute definition line starts with a ";"
     while ( str[idx] == ';' ) {
        
        idx++;
        
        skipwhite( str, idx );
        name = getIdentifier( str, idx );
        if ( name == "" ) {
           throw (badConfigSyntax());
        }
        
        attrNames.push_back( name );
        des_attrs[ name ] = "";
        des_priorities[ name ] = 0;
        
        if ( dbug > 0 ) {
           std::cerr << "Adding attribute name " << name << std::endl;
        }
        
        skipwhite( str, idx );
        
     }
     if ( str[idx] != 0 ) {
        throw (badConfigSyntax());     
     }

}

Catalog::Dimension* Catalog::parseDimDef( const char* str, size_t& idx )
{
     Dimension* result;
     size_t i;
     int isep;
     size_t sep;
     std::string quantity;
     std::string units;
     std::string mode;
     std::vector<float> vals;
     bool more;
     size_t n;
     VarVal* vx;

     if ( dbug > 5 ) {
       std::cerr << "parseDimension: parsing <<" << std::string(str) << ">>, starting at " << idx << std::endl;
     }
     
     result = NULLPTR;
     
     i = idx;
     
     // look for the next semicolon
     isep = scanFor( ';', str, i );
     sep = static_cast<size_t>(isep);
     if ( dbug > 70 ) {
        std::cerr << "parseDimension:found next ; at " << isep << ", idx=" << idx << std::endl;
     }
     if ( isep != -1 ) {
     
        quantity.insert( 0, str + i, sep - i );
        if ( dbug > 70 ) {
           std::cerr << "parseDimension: quantity = <<" << quantity << ">>" << std::endl;
        }
        trim(quantity);
        
        i = isep + 1;
 
        isep = scanFor( ';', str, i );
        sep = static_cast<size_t>(isep);
        if ( dbug > 70 ) {
          std::cerr << "parseDimension:found next ; at " << isep << ", idx=" << idx << std::endl;
        }
        if ( isep != -1 ) {
     
           units.insert( 0, str + i, sep - i );
           if ( dbug > 70 ) {
              std::cerr << "parseDimension: units = <<" << units << ">>" << std::endl;
           }
           trim(units);
        
           i = isep + 1;
        
           // now extract the format code
           skipwhite( str, i );
           isep = scanFor( ';', str, i );
           sep = static_cast<size_t>(isep);
           if ( dbug > 70 ) {
             std::cerr << "parseDimension:found next ; at " << isep << ", i=" << i << ", str[i]=" << str[i] << std::endl;
           }
           if ( isep != -1 ) {
     
              mode.insert( 0, str + i, sep - i );
              trim(mode);
              if ( dbug > 70 ) {
                 std::cerr << "parseDimension: mode = <<" << mode << ">>" << std::endl;
              }
        
              i = isep + 1;
              skipwhite( str, i );
        
              // now scan through and get the floating-point numbers
              more = true;
              while ( more ) {
                 if ( dbug > 70 ) {
                   std::cerr << "parseDimension: looking for number at " << i << std::endl;
                 }
              
                 vx = parseNumber( str, i );
                 if ( vx != NULLPTR ) {
                    if ( dbug > 70 ) {
                      std::cerr << "parseDimension: found number up to " << i << std::endl;
                    }
                    if ( vx->hasLiteral() ) {
                       vx->n2e();
                       vx->convertType('F'); 
                       if ( dbug > 70 ) {
                         vx->dump();
                         std::cerr << "parseDimension:    value is  " << vx->evalF << std::endl;
                       }
                       vals.push_back( vx->evalF );
                    } else {
                        std::cerr << "Dimension values must be literal: " << str << std::endl;
                        throw (badConfigSyntax());                                    
                    }
                 
                    delete vx;
                    
                 } else {
                    if ( dbug > 70 ) {
                      std::cerr << "parseDimension: hit nom-number at " << i << std::endl;
                    }
                    std::cerr << "Dimension hit non-numeric: " << str << " after " << i << std::endl;
                    throw (badConfigSyntax());                                    
                 }
                 
                 // find the next comma separator
                 isep = scanFor( ',', str, i );
                 sep = static_cast<size_t>(isep);
                 if ( dbug > 70 ) {
                   std::cerr << "parseDimension:found next , at " << isep << ", idx=" << idx << std::endl;
                 }
                 if ( isep != -1 ) {
     
                   i = isep + 1;
                   skipwhite( str, i );
              
                 } else {
                    more = false;
                 }
                 
              }    
              
              result = new Dimension();
              result->quant = quantity;
              result->units = units;
                 
              if ( mode == "LDN" ) {
                 if ( vals.size() == 3 ) {
                    n = vals[2];
                    result->set( vals[0], n, vals[1] );
                 } else {
                    std::cerr << "Dimension format LDN must have exactly three values: " << str  << std::endl;
                    throw (badConfigSyntax());                                    
                 }
              } else if ( mode == "LHN" ) {
                 if ( vals.size() == 3 ) {
                    n = vals[2];
                    result->set( vals[0], vals[1], n );               
                 } else {
                    std::cerr << "Dimension format LHN must have exactly three values: " << str  << std::endl;
                    throw (badConfigSyntax());                                    
                 }
              
              } else if ( mode == "V" ) {
                 if ( vals.size() >= 1 ) {
                    result->set( vals );                              
                 } else {
                    std::cerr << "Dimension format V must have at least one value: " << str  << std::endl;
                    throw (badConfigSyntax());                                    
                 }
              
              } else {
                 std::cerr << "Dimension definition has an invalid format code: " << str << std::endl;
                 throw (badConfigSyntax());                                                   
              }
           }
        }
     }
     
     return result;   
     
}

void Catalog::parseVarDef( Variable* var, const char* str, size_t& idx )
{
     int isep;
     size_t sep;
     VarVal* test;
     VarVal* value;
     VarExpr* vexp;
     VarExpr* texp;

     if ( dbug > 5 ) {
       std::cerr << "parseVarDef: parsing <<" << std::string(str + idx) << ">>" << std::endl;
     }
     isep = scanFor( '?', str, idx );
     sep = static_cast<size_t>(isep);
     if ( dbug > 70 ) {
        std::cerr << "parseVarDef:found next ? at " << isep << std::endl;
     }
     if ( isep == -1 ) {
        // no test expression
        // create a single boolean value set to true
        if ( dbug > 5 ) {
          std::cerr << "parseVarDef: no test expression" << std::endl;
        }
        texp = new VarExpr();
        test = new VarVal( true );
        // texp->operands.push_back( test );
        texp->add( test );
        delete test;
        
        // parse the value expression

        if ( dbug > 5 ) {
          std::cerr << "parseVarDef: about to parse var expr " << std::string(str + idx) << std::endl;
        }
        vexp = parseVarExpr( str, idx );
        
     } else {
        if ( dbug > 5 ) {
          std::cerr << "parseVarDef: found a test expression and a value expression " << std::string(str + idx) << std::endl;
        }
        size_t stop = sep - 1;
        
        // parse the test expression from idx to (sep-1)
        if ( dbug > 15 ) {
          std::cerr << "parseVarDef: parsing test expression first" << std::endl;
        }
        texp = parseVarExpr( str, idx, stop );
        
        size_t start = sep + 1;
        // parse the value expresion from (sep_1) to EOL
        if ( dbug > 15 ) {
          std::cerr << "parseVarDef: parsing value expression second" << std::endl;
        }
        vexp = parseVarExpr( str, start );
     }
     
     var->addValue( vexp, texp );

}

Catalog::VarVal* Catalog::parseString(  const char* str, size_t& idx ) const
{
   VarVal* result;
   char cc;
   char quote;
   int i;
   int j;
   int k;
   
   result = NULLPTR;
   i = idx;
   if ( str[i] == '"' || str[i] == '\'' ) {
   
      if ( dbug > 50 ) {
         std::cerr << "parseString: Found something in quotes: <<" << std::string(str) << ">>" << std::endl;
      }
   
      quote = str[i];

      i++;
      
      // starting a string 
      j = scanFor( quote, str, i );
      if ( j > idx ) {
         std::string s;
         s.insert(0, str + i, j - i);
         if ( dbug > 50 ) {
            std::cerr << "parseString: extracted: <<" << s << ">> from chars " << i << " through " << j << std::endl;
         }
         // found the closing quote.
         // So check whether this is a string literal or a string
         k = isStringLiteral( str, j, i );
         //k = isDateLiteral( str, j, i );
         if ( k > 0 ) {
            // a string literal
            if ( dbug > 10 ) {
               std::cerr << "got a literal string: " << s << std::endl;
            }
            result = new VarVal( s, 'S' );
            idx = k + i; // That's "eye", not "one"!
         } else {
            if ( dbug > 10 ) {
               std::cerr << "parseString: got a string w/ varref: " << std::endl;
            }
            result = new VarVal( s, 'S' );
            idx = j + 1;   
         }
      } else {
         std::cerr << "No closing quote in string literal: " << std::string( str ) << std::endl;
      }
      
   }
   
   if ( result != NULLPTR ) {
      if ( dbug > 10 ) {
//         result->dump();
      }
   }
   
   return result;

}

Catalog::VarVal* Catalog::parseDate(  const char* str, size_t& idx ) const
{
   VarVal* result;
   int i;
   int j;
   int k;
   std::string s;

   result = NULLPTR;
   i = idx;
   if ( str[i] == '[' ) {

      i++;

      s.clear();
      
      // starting a date 
      
      // find the end
      j = scanFor( ']', str, i );
      if ( j > idx ) {
         s.insert(0, str + i, j - i);
         // found the closing date marker.
         // So check whether this is a well-formed date literal or not
         k = isDateLiteral( str, j, i );
         if ( k > 0 ) {
            // a dateliteral
            if ( dbug > 10 ) {
               std::cerr << "parseDate: got a literal date: " << s << std::endl;
            }
            result = new VarVal( s, 'D' );
            idx = j + 1;
         } else {
            if ( dbug > 10 ) {
               std::cerr << "parseDate: got a date expression: " << s << std::endl;
            }
            // should probably test here that the non-date-lkike part of the string
            // is a variable reference.
            
            // std::cerr << "malfomed date literal in: " << std::string(str) << std::endl;
            result = new VarVal( s, 'D' );
            result->flags = VarVal::ValidNominal;  // i.e., not a literal!
            idx = j + 1;
         }
      } else {
         std::cerr << "Unclosed date literal in: " << std::string( str ) << std::endl;
      }
   }

   if ( result != NULLPTR ) {
      if ( dbug > 10 ) {
//         result->dump();
      }
   }
   
   return result;
}

Catalog::VarVal* Catalog::parseBool(  const char* str, size_t& idx ) const
{
   VarVal* result;
   int i;

   i = idx;
   result = NULLPTR;
   i = matchAgainst( "true", str, idx );
   if ( i >= 0 ) {
      idx = i;
      result = new VarVal( "true", 'B');
   } else {
      i = matchAgainst( "false", str, idx );
      if ( i >= 0 ) {
         idx = i;
         result = new VarVal( "false", 'B');
      }   
   }

   if ( result != NULLPTR ) {
      if ( dbug > 10 ) {
//         result->dump();
      }
   }

   return result;

}

Catalog::VarVal* Catalog::parseNumber(  const char* str, size_t& idx ) const
{
   VarVal* result;
   int i;
   int sgn;
   bool hasDigit;
   std::string s;
    
   result = NULLPTR;
   hasDigit = false;

   if ( dbug > 150 ) {
      std::cerr << " parseNumber: <<" << std::string(str) << ">>" << std::endl;
   }
   i = idx;
   if ( str[i] == '+' || str[i] == '-' ) {
      if ( dbug > 50 ) {
         std::cerr << " parseNumber: found a sign" << std::endl;
      }
      
      i++;
   }
   
   while ( isDigit( str[i] )) {
      hasDigit = true;
      i++;
   }  

   if ( hasDigit ) {
      if ( dbug > 50 ) {
         std::cerr << " parseNumber: found digit[s]" << std::endl;
      }
      // ok, we have a sequence of at least one digit,
      // possibly preceeded by a sign.
      // This is either an integer or the beginning of a float
      if ( str[i] != '.' ) {
         // It's an integer
         s.insert( 0, str + idx, i - idx ); 
         result = new VarVal( s, 'I' );
         idx = i;
      } else {
         // It's a float
         
         // find more digits
         i++;
         while ( isdigit( str[i] )) {
            i++;
         }
         s.insert( 0, str + idx, i - idx ); 
         result = new VarVal( s, 'F' );
         idx = i;
      }
    
   }

   if ( result != NULLPTR ) {
      if ( dbug > 10 ) {
//         result->dump();
      }
   }
   
   return result;

}

Catalog::VarVal* Catalog::parseVarRef(  const char* str, size_t& idx ) const
{
   VarVal* result;
   int i;
   int j;
   size_t k;
   std::string s;
   std::string ref;
   int fmt1;
   int fmt2;
   
   result = NULLPTR;
   fmt1 = -1;
   fmt2 = -1;
   
   k = idx;
   extractVarRef( str, idx, s, ref, &fmt1, &fmt2 );
   
   if ( s != "" ) {
      result = new VarVal( s, 'S' );
      result->evalS = ref;
      result->flags = result->flags | VarVal::ValidEval & ( ~ VarVal::IsLiteral );
      result->fmt1 = fmt1;
      result->fmt2 = fmt2;
    
   }
   
   if ( result != NULLPTR ) {
      if ( dbug > 10 ) {
//         result->dump();
      }
   }
   
   return result;
}


Catalog::VarOp* Catalog::parseOp( const char* str, size_t& idx, int pri ) const
{
    VarOp* result;
    size_t i;
    char cc;
    bool err;

    result = NULLPTR;
    err = false;
    i = idx;
    
     if ( str[i] == '!' ) {
        i++;
        if ( str[i] == '=' ) {
           result = new VarOp( TestNE, pri  );
           i++;
        } else { 
           result = new VarOp( TestNOT, pri  );
        }    
     } else if ( str[i] == '<' ) {
        i++;
        if ( str[i] == '=' ) {
           result = new VarOp( TestLE, pri  );
           i++;
        } else { 
           result = new VarOp( TestLT, pri  );
        }    
     } else if ( str[i] == '>' ) {
        i++;
        if ( str[i] == '=' ) {
           result = new VarOp( TestGE, pri  );
           i++;
        } else { 
           result = new VarOp( TestGT, pri  );
        }     
     } else if ( str[i] == '=' ) {
        i++;
        if ( str[i] == '=' ) {
           result = new VarOp( TestEQ, pri  );
           i++;
        } else { 
           err = true;
        }         
     } else if ( str[i] == '&' ) {
        i++;
        if ( str[i] == '&' ) {
           result = new VarOp( TestAND, pri  );
           i++;
        } else { 
           err = true;
        }         
     } else if ( str[i] == '|' ) {
        i++;
        if ( str[i] == '|' ) {
           result = new VarOp( TestOR, pri  );
           i++;
        } else { 
           err = true;
        }         
     } else if ( str[i] == '+' ) {
        result = new VarOp( Add, pri  );
        i++;
     } else if ( str[i] == '-' ) {
        result = new VarOp( Subtract, pri  );
        i++;
     } else if ( str[i] == '*' ) {
        result = new VarOp( Multiply, pri  );
        i++;
     } else if ( str[i] == '/' ) {
        result = new VarOp( Divide, pri  );
        i++;
     } else if ( str[i] == '%' ) {
        result = new VarOp( Remainder, pri  );
        i++;
     }
    
     if ( ! err ) {
        idx = i;
     }
     
     return result;
}


Catalog::VarExpr* Catalog::parseVarExpr( const char* str, size_t& idx, int stop ) const
{
   VarExpr* result;
   bool err;
   VarVal* vval;
   VarOp* vop;
   size_t i;
   int priority;
   std::deque<VarOp*> opStack;
   int nop;
   int tosPri;
   VarOp* tmpOp;
   bool prev_was_val;
   bool prev_was_op;
   bool got_token;
   
   result = new VarExpr();
   priority = 0;
   
   err = false;
   i = idx;
   tosPri = -1;
   nop = opStack.size();
   prev_was_val = false;
   prev_was_op = false;
   
   if ( dbug > 50 ) {
      std::cerr << "Catalog::parseVarExpr: entering to parse to char " << stop 
      << " in <<" << str+idx << ">>" << std::endl; 
   }
   
   // until we hit the terminating null char (or the specified stop point) ...
   while ( ! ( (str[i] == 0) || ( (stop>=idx) && (i == stop) ) )) {
       // skip any leading whitespace
       skipwhite( str, i );
       
       if ( dbug >= 30 ) {
          std::cerr << "parseVarExpr: LOOP: at char " << i << ": <<" << std::string(str + i) << ">>" << std::endl;
       
       }
       
       got_token = false;
       
       // we look for a value token only if the previous token was not a value token
       if ( ! prev_was_val ) {
          // check for '"' and string literal and '"'
          vval = parseString( str, i );
          if ( vval != NULLPTR ) {
             result->add( vval );
             prev_was_val = true;
             delete vval;
          } else {
             // check for "[" and date literal and "]"
             vval = parseDate( str, i );
             if ( vval != NULLPTR ) {
                // deal w/ the varval
                result->add( vval );
                prev_was_val = true;
                delete vval;
             } else {
                // check for "true" or "false" for boolean literal
                vval = parseBool( str, i );
                if ( vval != NULLPTR ) {
                   // deal with varvel
                   result->add( vval );
                   prev_was_val = true;
                   delete vval;
                 } else {   
                   // check for [+-0-9] and float litreral or else int literal
                   vval = parseNumber( str, i );
                   if ( vval != NULLPTR ) {
                      result->add( vval );
                      prev_was_val = true;
                      delete vval;
                   } else {
                      vval = parseVarRef( str, i );
                      if ( vval != NULLPTR ) {
                         result->add( vval );
                         prev_was_val = true;
                         delete vval;
                      }
                   }
                }
             }
          }
          if ( prev_was_val ) {
             got_token = true;
             prev_was_op = false;
          }          

       }
       if ( ! got_token ) {
         if ( ! prev_was_op ) {
            if ( str[i] == '(' ) {
               priority += 100;
               i++;
               prev_was_val = false;
               prev_was_op = false;
               got_token = true;
            } else if ( str[i] == ')' ) {
               priority -= 100;
               if ( priority < 0 ) {
                   std::cerr << "unbalanced parenthees expression in '" << str + idx << "'" << std::endl;
                   throw (badConfigSyntax());                      
               }
               i++;
               prev_was_val = false;
               prev_was_op = false;
               got_token = true;
            } else {
               // check for operators "+", "-", "*", "/", "%", "==", "!=", "<", "<=", ">", ">=", "&&", "||", "!"
               vop = parseOp( str, i, priority );
               if ( vop != NULLPTR ) {
               
                  // got an operator
                  if ( nop > 0 ) {
                     tosPri = (opStack[ nop - 1])->priority;
                     while ( (nop > 0) && (vop->priority < tosPri) ) {
                           tmpOp = opStack.back();
                           result->add( tmpOp );
                           opStack.pop_back();
                           delete tmpOp;
                           nop = opStack.size();
                           if ( nop > 0 ) {
                              tosPri = (opStack[ nop - 1])->priority;
                           }                                  
                     }
                  }
                  opStack.push_back( vop );
                  nop++;
                  prev_was_op = true;
               } else {
                   // syntax error
                   std::cerr << "malformed expression in '" << str + idx << "'" << std::endl;
                   throw (badConfigSyntax());
               }
            }
            if ( prev_was_op ) {
               got_token = true;
               prev_was_val = false;
            }          
         } else {
            if ( str[i] == '(' ) {
               priority += 100;
               i++;
               prev_was_val = false;
               prev_was_op = false;
               got_token = true;
            } else if ( str[i] == ')' ) {
               priority -= 100;
               if ( priority < 0 ) {
                   std::cerr << "unbalanced parenthees expression in '" << str + idx << "'" << std::endl;
                   throw (badConfigSyntax());                      
               }
               i++;
               prev_was_val = false;
               prev_was_op = false;
               got_token = true;
            }
         }  
       }
       if ( ! got_token ) {
          std::cerr << "bad expression: doubled values or operators? at " << idx << " in '" << str << "'" << std::endl;
          throw (badConfigSyntax());
       }
       
       if ( dbug > 90 ) {
          std::cerr << "parseVarExpr: ----- exited tests at char position " << i;
          if ( str[i] != 0 ) { 
              std::cerr << "'" << str[i] << "'";
          } else {
              std::cerr << "(null)";
          }
          std::cerr << std::endl;
       }
   

   } 
       
   while ( opStack.size() > 0 ) {
        tmpOp = opStack.back();
        result->add( tmpOp );
        delete tmpOp;
        opStack.pop_back();
   }
   
   if ( priority != 0 ) {
       std::cerr << "Unbalanced parenthees expression in '" << str + idx << "'" << std::endl;
       throw (badConfigSyntax());                      
   }
       
   idx = i;
   
   //   result->dump();
      
   return result;
}

Catalog::Target* Catalog::parseTarget( const char* str, size_t& idx ) const
{
    size_t i;
    int ii;
    int j;
    int k;
    Target* result;
    int nattrs;
    std::string aname;
    std::string avalue;
    std::string templayt;
    std::string time;
    std::string numbr;
    int n;
    float t;
    VarVal *vx;
    
    nattrs = attrNames.size();
    
    result = new Target();
    i = idx;
    
    // get the date base
    result->dbase.flags = 0;
    skipwhite( str, i );
    j = findNext( ';', str, i );
    ii = static_cast<int>(i);    
    if ( j >= ii ) {
    
       time.clear();
       time.insert(0, str, i, j - i);
       trim(time);
       size_t idx = 0;
       vx = parseDate( time.c_str(), idx );
       if ( vx != NULLPTR ) {
          result->dbase = *vx;
          delete vx;
       } else {
          std::cerr << "Bad base date from target definition: '" << str << "'" << std::endl;
          throw (badConfigSyntax());
       }
       i = j + 1;
    
    } else {
       std::cerr << "Missing base date from target definition: '" << str << "'" << std::endl;
       throw (badConfigSyntax());
    }
    result->basetime = 0.0;
    
//-       if ( ! s2Float( time, t ) ) {
//-       result->basetime = t;
       
    skipwhite( str, i );
    j = findNext( ';', str, i );
    ii = static_cast<int>(i);    
    if ( j >= ii ) {
       time.clear();
       time.insert(0, str, i, j - i);
       trim(time);
       k = result->urlSep.parse( time );
       if ( k == 0 && time != "" ) {
          std::cerr << "Bad URL separation time interval from target definition: '" << str << "'" << std::endl;
          throw (badConfigSyntax());       
       }
    } else {
       std::cerr << "Bad target definition: '" << str << "'" << std::endl;
       throw (badConfigSyntax());       
    }
    
    i = j + 1;
    
    result->inctime = 1.0;
    skipwhite( str, i );
    j = findNext( ';', str, i );
    ii = static_cast<int>(i);    
    if ( j >= ii ) {
    
       time.clear();
       time.insert(0, str, i, j - i);
       trim(time);
       if ( ! s2Float( time, t ) ) {
          std::cerr << "Bad inc time from target definition: '" << str << "'" << std::endl;
          throw (badConfigSyntax());
       }
       result->inctime = t;
    
       i = j + 1;
    
    } else {
       std::cerr << "Missing inc time from target definition: '" << str << "'" << std::endl;
       throw (badConfigSyntax());
    }

    result->nt = 0;
    skipwhite( str, i );
    j = findNext( ';', str, i );
    ii = static_cast<int>(i);    
    if ( j >= ii ) {
    
       numbr.clear();
       numbr.insert(0, str, i, j - i);
       trim(numbr);
       if ( ! s2Int( numbr, n ) ) {
          std::cerr << "Bad number-of-times from target definition: '" << str << "'" << std::endl;
          throw (badConfigSyntax());
       }
       result->nt = n;
    
       i = j + 1;
    
    } else {
       std::cerr << "Missing number-of-times from target definition: '" << str << "'" << std::endl;
       throw (badConfigSyntax());
    }
    
    
    // go through each attribute
    for ( int ia=0; ia < nattrs; ia++ ) {
        
        aname = attrNames[ia];
        
        skipwhite( str, i );
        
        j = findNext( ';', str, i );
        ii = static_cast<int>(i);    
        if ( j >= ii ) {
           avalue.clear();
           avalue.insert(0, str, i, j - i  );
        
           if ( dbug > 30 ) {
              std::cerr << " target attribute <<" << aname << ">> = <<" << avalue << ">>" << std::endl;
           }
           result->attrs[aname] = avalue;
           
           i = j + 1;
        
        } else {
           throw (badConfigSyntax());
        }
    
    }
    
    skipwhite( str, i );
    
    j = i;
    while ( (str[j] != 0) && (str[j] != ' ') ) {
       j++;
    }
    
    result->templayt.clear();
    result->templayt.insert( 0, str + i, j - i );
    
    if ( dbug > 30 ) {
       std::cerr << " i=" << i << " <<" << str << ">>" << std::endl;
       std::cerr << " target template = <<" << result->templayt << ">>" << std::endl;
    }
    
    
    
    idx = j;
    
    return result;
}


void Catalog::parseQuantity(Catalog::Quantity* quant, const char* str, size_t& idx ) const
{
     bool done;
     std::string tname;
     size_t i;
     int ii;
     int j;
     int idx_eol;
     int idx_endstd;
     int idx_targetsep;
     int idx_begbracket;
     int idx_endbracket;
     int idx_sc1;
     int idx_sc2;
     int idx_enduu;
     int idx_endsc1;
     int idx_endsc2;
     TargetRef* qt;
     float f;
     std::string s;
     std::string uu;
     std::string dd;
     float scl;
     float off;
     std::string stdname;
     int dims;
     std::string dimstring;
     
     done = false;
     
     // find the end of the line (index of the terminating null)
     idx_eol = idx;
     while ( str[idx_eol] != 0 ) {
        idx_eol++;
     }
     
     
     // read the standardized name
     skipwhite( str, idx );
     i = idx;
     idx_endstd = findNext( ':', str, i );
     ii = static_cast<int>(i);
     if ( idx_endstd < ii ) {
        std::cerr << "missing standard name from quantity line: '" << str << "'" << std::endl;
        throw (badConfigSyntax());
     }
     // (omit any whitespace at the end of the std name)
     ii = idx_endstd - 1;
     while ( (ii > idx) && isWhitespace( str[ii] ) ) {
        ii --;
     }
     // (copy the string)   
     stdname = "";
     for ( i=idx; i <= ii; i++ ) {
         stdname.push_back( str[i] );
     }
     quant->stdname = stdname;
     
     idx = idx_endstd + 1;
     
     // read the dimensionality
     skipwhite( str, idx );
     j = findNext( ':', str, idx );
     ii = static_cast<int>(idx);    
     if ( j >= ii ) {
    
       dimstring.clear();
       dimstring.insert(0, str, idx, j - idx);
       trim(dimstring);
       if ( ! s2Int( dimstring, dims ) ) {
          std::cerr << "Bad dimensionality from quantity definition: '" << str << "'" << std::endl;
          throw (badConfigSyntax());
       }
       quant->dims = dims;
       
       idx = j + 1;
    
    } else {
       std::cerr << "Missing dimensionality from quantity definition: '" << str << "'" << std::endl;
       throw (badConfigSyntax());
    }
     
     
     if ( dbug > 50 ) {
        std::cerr << "parseQuantity: scanning for targets starting with idx " << idx 
        << "in '" << str << "'" << std::endl;     
     }

     int kkk = 0;
     while ( ! done ) {
     
         kkk++;
         if ( kkk > 2000 ) {
            std::cerr << "bad wolf!" << kkk << std::endl;
         }
     
         // we may need to backtrack, so work with
         // this temporary copy of idx fo rnow
         i = idx;

         skipwhite( str, i );
     
         tname = getIdentifier( str, i );
         if ( tname != "" ) {

            if ( dbug > 20 ) {
               std::cerr << "^^^^^^^^ parsing target specifier for " << tname << std::endl;            
            }
            //  start it up 
            qt = new TargetRef(tname);
            
            // find the end of this target specifier
            idx_targetsep = findNext( '|', str, i );
            ii = static_cast<int>(i);
            if ( idx_targetsep < ii ) {
               // or the end of line will work, too, if
               // this line has only one target specifier
               idx_targetsep = idx_eol;
            }
            
            if ( dbug > 80 ) { 
               std::cerr << "target sep at " << idx_targetsep << " from " << i << " to " << idx_eol << std::endl;
               std::cerr << "tname=" << tname << ", i=" << i << ", lastchar=" << str[i] << std::endl;         
            }

            uu.clear();
            dd.clear();
            scl = 1.0;
            off = 0.0;
               
            skipwhite( str, i );
         
            if ( str[i] == '[' ) {
            
               idx_begbracket = i + 1;
               
               idx_endbracket = findNext(']', str, idx_begbracket );
               //std::cerr << "idx_endbracket=" <<  idx_endbracket << ", found next=" << str[idx_endbracket] 
               //<< ", end at " << idx_targetsep << std::endl;         
               if ( ( idx_endbracket > idx_begbracket ) && (idx_endbracket < idx_targetsep) ) {
                  // found the terminating "]" for this target specifier
                  
                  // look for the delimiter that starts the scale factor
                  idx_sc1 = findNext( ';', str, idx_begbracket );
                  //std::cerr << "idx_begbracket = " << idx_begbracket << ", char=" << str[idx_begbracket] << std::endl;
                  //std::cerr << "idx_sc1=" <<  idx_sc1 << ", found next=" << str[idx_sc1] << std::endl;         
                  if ( (idx_sc1 > idx_begbracket) && (idx_sc1 < idx_targetsep) ) {
                     // we have at least a scale factor
  
                     // this marks the end of the units string
                     idx_enduu = idx_sc1 - 1;                  
                     // this marks the beginning of the scale factor
                     idx_sc1++;
                     
                     if ( dbug > 80 ) {
                        std::cerr << "w/ scale/offset, end of units spec is " << idx_enduu 
                                  << " end char =" << str[idx_enduu] << std::endl;         
                     }
                     
                     // look for an offset delimiter         
                     idx_sc2 = findNext( ';', str, idx_sc1 );
                     if ( (idx_sc2 > idx_sc1) && (idx_sc2 < idx_targetsep) ) {
                        // we have both a scale factor and an offset
                        
                        // the end of the scale factor string
                        idx_endsc1 = idx_sc2 - 1;
                        // the start of the offset string
                        idx_sc2++;
                        
                        // the end of the offset string
                        idx_endsc2 = idx_endbracket - 1;

                        // unpack the offset
                        s.clear();
                        s.insert(0, str, idx_sc2, idx_endsc2 - idx_sc2 + 1);
                        trim(s);
                        //std::cerr << "offset starts at " << idx_sc2 <<" ends at " << idx_endsc2 
                        //<< ", string ='" << s  << "'" << std::endl;         
                        if ( ! s2Float( s, off ) ) {
                           std::cerr << "bad offset=" << std::string(str + idx_sc1)  << std::endl;         
                           throw (badConfigSyntax());
                        }
                        if ( dbug > 70 ) {
                           std::cerr << "offset=" << off  << std::endl;         
                        }

                     } else {
                        // only a scale, no offset
                        
                        // this marks the end of the scale factor string
                        idx_endsc1 = idx_endbracket - 1;
                     }
                     
                     // unpack the scale factor
                     s.clear();
                     s.insert(0, str, idx_sc1, idx_endsc1 - idx_sc1 + 1);
                     trim(s);
                     //std::cerr << "scale starts at " << idx_sc1 <<" ends at " << idx_endsc1 
                     //<< ", string ='" << s  << "'" << std::endl;         
                     if ( ! s2Float( s, scl ) ) {
                        std::cerr << "bad scale=" << std::string(str + idx_sc1)  << std::endl;         
                        throw (badConfigSyntax());
                     }
                     if ( dbug > 70 ) {
                        std::cerr << "scale=" << scl  << std::endl;         
                     }

                  } else {


                     // no scale or offset
                     idx_enduu = idx_endbracket - 1;
                     if ( dbug > 70 ) {
                        std::cerr << "no scale/offset, end of units spec is " << idx_enduu 
                        << " end char =" << str[idx_enduu] << std::endl;         
                     }
                  }
                  
                  
                  // we have just passed the first [] pair, holding the units specifier
                  // Now we look for a second [] pait, holding a description specifier
                  i = idx_endbracket + 1;
                  
                  skipwhite( str, i );

                  if ( dbug > 80 ) {
                      std::cerr << "looking for opening of description at " << i 
                                << ", char=" << str[i] << std::endl;                  
                  }
                  if ( str[i] == '[' ) {
                     // got a description
                     
                     i++;
                     
                     idx_endbracket = scanFor( ']', str, i );
                     // std::cerr << "looked for end bracket from " << i << " and got " 
                     // << idx_endbracket << " w/ char=" << str[idx_endbracket] << std::endl;
                     if ( (idx_endbracket > i) && (idx_endbracket < idx_targetsep)) {
                     
                        dd.clear();
                        dd.insert( 0, str, i, idx_endbracket - i );
                        trim(dd);
                        if ( dbug > 70 ) {
                            std::cerr << "got description '" << dd << "'" << std::endl;         
                        }
                     } else {
                       std::cerr << "Unclosed '[' in quantity  target specifier description in '" << str << "'" << std::endl;
                       throw (badConfigSyntax());
                     }
                  
                  } 
                  
               } else {
                  std::cerr << "Unclosed '[' in quantity  target specifier units in '" << str << "'" << std::endl;
                  throw (badConfigSyntax());
               }
               
               // unpack the units string
               uu.clear();
               uu.insert(0, str, idx_begbracket, idx_enduu - idx_begbracket + 1);
               trim(uu);
               if ( dbug > 70 ) {
                  std::cerr << "units string='" <<  uu << "'" << std::endl;         
               }
            }
            
            
            qt->units = uu;
            qt->scale = scl;
            qt->offset = off;
            qt->description = dd;
            
            (quant->tlist).push_back( *qt );
         
            delete qt;
         
            idx = idx_targetsep;
            if ( str[idx] != 0 ) {
               idx++;
            }
            
            skipwhite( str, idx );
         
         } else {
            done = true;
         }
             
     }
}


void Catalog::parse( const std::string& conf )
{
     const char *str;
     size_t idx;
     size_t k;
     size_t len;
     int state;
     std::string name;
     char varType;
     Variable* varib;
     Target* tgt;
     Quantity* quant;
     Dimension *dim;
     bool mustBeQuantity;
     
     // initialize
     str = conf.c_str();
     len = conf.size();
     idx = 0;
     state = 0;

     // starts w/ ';'? it's the attribute definition line
     // starts w/ '>'? it's the lower time limit line
     // starts w/ '<'? it's the upper time limit line
     if ( str[idx] != ';' && str[idx] != '>' && str[idx] != '<' ) {
     
        // If the line starts with "(", then what follows is
        // a quantity name, possibly with weird characters.
        // Otherwise, it's a regular identifier (which makes life simpler).

        mustBeQuantity = false;
             
        name = getWeirdIdentifier( str, idx );
        if ( name == "" ) {
           // this line starts with some kind of identifier
           name = getIdentifier( str, idx );
           if ( name == ""  ) {
              std::cerr << "No identifier in line " << conf << std::endl;
              throw (badConfigSyntax());
           }
        } else {
           mustBeQuantity = true;
        }
        
        skipwhite( str, idx );
        if ( str[idx] == 0 ) {
           std::cerr << "Identifier is alone in line " << conf << std::endl;
           throw (badConfigSyntax());
        }
        
        
        
        if ( str[idx] == ':' ) {
           
           idx++;
           if ( str[idx] == '=' ) {
              // target definition
              if ( mustBeQuantity ) {
                 std::cerr << "Parenthesized identifier was used for a target definition: "
                 << str << std::endl;
                 throw (badConfigSyntax());
              }
              if ( dbug > 5 ) {
                 std::cerr << "defining target " << name << std::endl;
              }
              idx++;
              skipwhite( str, idx );
              tgt = parseTarget( str, idx );
              if ( tgt != NULLPTR ) {
                 tgt->name = name;
                 targetset.define( tgt );
                 delete tgt;
              }
           } else {
              // quantity definition
              if ( dbug > 5 ) {
                 std::cerr << "defining quantity " << name << std::endl;
              }
              skipwhite( str, idx );
              
              quant = new Quantity(name);
              parseQuantity( quant, str, idx );
            
              quantityset.define( quant );
              
              if ( quant->stdname != "" ) {
                 quantityset.stdnames[ quant->name ] = quant->stdname;
              }
              
              delete quant;
            
           }
        } else if ( str[idx] == '~' ) {
            // dimensional definition
            
            idx++;
            skipwhite( str, idx );
            dim = parseDimDef( str, idx );                  
            if ( dim != NULLPTR ) {
            
               dim->name = name;
            
               //dim->dump();
               
               dimset.define( dim );
               
               delete dim;
            }
            
        } else {
           // probably a variable definiition
           
           varType = 'S';
           if ( str[idx] == '/' ) {
              idx++;
              if ( isVarType( str[idx] ) ) {
                 varType = str[idx];
                 idx++;
                 skipwhite( str, idx );                 
              } else {
                 std::cerr << "Bad config line (expected variable type spec): " << conf << std::endl;
                 throw (badConfigSyntax());              
              }
          }
             
          if ( str[idx] == '=' ) {
             // definitely a variable definition
             if ( dbug > 5 ) {
                std::cerr << "defining variable " << name << std::endl;
             }
             
             // defines if it does not exist
             // otherwise, checks the type
             varset.define( name, varType );
 
             varib = varset.getVariable( name );
             
             idx++; // past the "="
             skipwhite( str, idx );  
             parseVarDef( varib, str, idx );               
             
             
             
          } else {
             std::cerr << "Bad config line: " << conf << std::endl;
             throw (badConfigSyntax());
          }
        
        }
     } else if ( str[idx] == ';' ) {
        // only the attribute definition line starts with a ";"
        try {
           parseAttrNames(  str,  idx );
        } catch ( badConfigSyntax ) {
           std::cerr << "Bad attribute names config line: " << conf << std::endl;
           throw ( badConfigSyntax() );
        }
     } else if ( str[idx] == '>' ) {
         idx++;
         skipwhite( str, idx );
         k = isStringLiteral( str, len, idx );
         if ( k > 0 ) {
            first_date.assign( str, idx, k );
         } else {
           std::cerr << "Bad first_date config line: " << conf << std::endl;
           throw ( badConfigSyntax() );         
         }
     } else if ( str[idx] == '<' ) {
         idx++;
         skipwhite( str, idx );
         k = isStringLiteral( str, len, idx );
         if ( k > 0 ) {
            final_date.assign( str, idx, k );
         } else {
           std::cerr << "Bad final_date config line: " << conf << std::endl;
           throw ( badConfigSyntax() );         
         }
     } else {
           std::cerr << "Bad config line: " << conf << std::endl;
           throw ( badConfigSyntax() );
     }

}




void Catalog::setup_vars( const std::string& quantity, double tyme, const std::string& tag  )
{
    VarVal* test;
    VarVal* value;
    VarExpr* texpr;
    VarExpr* vexpr;
    std::string date;
    int year, month, dom, hour, minute, second, doy;
    float seconds;
    bool doit;
    
    doit = true;
    
    // are the local variables already defined?
    if ( varset.has_locals ) {
       // were the local variables set up with the same
       // quantity and time and tag?
       if ( (varset.quant == quantity) && ( varset.time == tyme ) && (varset.tag == tag ) ) {
          doit = false;
       }
    
    }
  
    if ( doit ) {
      if ( d2String( tyme, date ) ) {
  
          d2parts( tyme, year, month, dom, doy, hour, minute, seconds );
          second = roundf(seconds);
  
          test = new VarVal( true );
   
          takedown_vars();
          
          // QUANTITY
          texpr = new VarExpr();
          texpr->add( test );
          value = new VarVal( quantity, 'S' );
          vexpr = new VarExpr();
          vexpr->add( value );
          varset.addValue( "QUANTITY", 'S', vexpr, texpr );
          delete value;        
          // DATETIME
          texpr = new VarExpr();
          texpr->add( test );
          value = new VarVal( date, 'S' );
          vexpr = new VarExpr();
          vexpr->add( value );
          varset.addValue( "DATETIME", 'S', vexpr, texpr );
          delete value;        
          // DATE
          texpr = new VarExpr();
          texpr->add( test );
          value = new VarVal( date.substr(0,10), 'S' );
          vexpr = new VarExpr();
          vexpr->add( value );
          varset.addValue( "DATE", 'S', vexpr, texpr );
          delete value;        
          // TIME
          texpr = new VarExpr();
          texpr->add( test );
          value = new VarVal( date.substr(11,8), 'S' );
          vexpr = new VarExpr();
          vexpr->add( value );
          varset.addValue( "TIME", 'S', vexpr, texpr );
          delete value;        
          // YEAR
          texpr = new VarExpr();
          texpr->add( test );
          value = new VarVal( date.substr(0,4), 'I' );
          value->fmt1 = 4;
          vexpr = new VarExpr();
          vexpr->add( value );
          varset.addValue( "YEAR", 'I', vexpr, texpr );
          delete value;        
          // CN
          texpr = new VarExpr();
          texpr->add( test );
          value = new VarVal( date.substr(0,2), 'I' );
          value->fmt1 = 2;
          vexpr = new VarExpr();
          vexpr->add( value );
          varset.addValue( "CN", 'I', vexpr, texpr );
          delete value;        
          // YR
          texpr = new VarExpr();
          texpr->add( test );
          value = new VarVal( date.substr(2,2), 'I' );
          value->fmt1 = 2;
          vexpr = new VarExpr();
          vexpr->add( value );
          varset.addValue( "YR", 'I', vexpr, texpr );
          delete value;        
          // MONTH
          texpr = new VarExpr();
          texpr->add( test );
          value = new VarVal( date.substr(5,2), 'I' );
          value->fmt1 = 2;
          vexpr = new VarExpr();
          vexpr->add( value );
          varset.addValue( "MONTH", 'I', vexpr, texpr );
          delete value;        
          // DOM
          texpr = new VarExpr();
          texpr->add( test );
          value = new VarVal( date.substr(8,2), 'I' );
          value->fmt1 = 2;
          vexpr = new VarExpr();
          vexpr->add( value );
          varset.addValue( "DOM", 'I', vexpr, texpr );
          delete value;        
          // DOY
          texpr = new VarExpr();
          texpr->add( test );
          value = new VarVal( doy );
          value->fmt1 = 3;
          vexpr = new VarExpr();
          vexpr->add( value );
          varset.addValue( "DOY", 'I', vexpr, texpr );
          delete value;        
          // HOUR
          texpr = new VarExpr();
          texpr->add( test );
          value = new VarVal( date.substr(11,2), 'I' );
          value->fmt1 = 2;
          vexpr = new VarExpr();
          vexpr->add( value );
          varset.addValue( "HOUR", 'I', vexpr, texpr );
          delete value;        
          // MINUTE
          texpr = new VarExpr();
          texpr->add( test );
          value = new VarVal( date.substr(14,2), 'I' );
          value->fmt1 = 2;
          vexpr = new VarExpr();
          vexpr->add( value );
          varset.addValue( "MINUTE", 'I', vexpr, texpr );
          delete value;        
          // SECOND
          texpr = new VarExpr();
          texpr->add( test );
          value = new VarVal( date.substr(17,2), 'I' );
          value->fmt1 = 2;
          vexpr = new VarExpr();
          vexpr->add( value );
          varset.addValue( "SECOND", 'I', vexpr, texpr );
          delete value;        
          // TAG
          texpr = new VarExpr();
          texpr->add( test );
          value = new VarVal( tag, 'S' );
          vexpr = new VarExpr();
          vexpr->add( value );
          varset.addValue( "TAG", 'S', vexpr, texpr );
          delete value;        


          delete test;
      }
      
      varset.tag = tag;
      varset.time = tyme;
      varset.quant = quantity;
      varset.has_locals = true;
      
    }
} 
void Catalog::takedown_vars()
{
    if ( varset.exists( "QUANTITY" ) ) {
       varset.undefine( "QUANTITY" );
    }
    if ( varset.exists( "DATETIME" ) ) {
       varset.undefine( "DATETIME" );
    }
    if ( varset.exists( "DATE" ) ) {
       varset.undefine( "DATE" );
    }
    if ( varset.exists( "TIME" ) ) {
       varset.undefine( "TIME" );
    }
    if ( varset.exists( "YEAR" ) ) {
       varset.undefine( "YEAR" );
    }
    if ( varset.exists( "CN" ) ) {
       varset.undefine( "CN" );
    }
    if ( varset.exists( "YR" ) ) {
       varset.undefine( "YR" );
    }
    if ( varset.exists( "MONTH" ) ) {
       varset.undefine( "MONTH" );
    }
    if ( varset.exists( "DOM" ) ) {
       varset.undefine( "DOM" );
    }
    if ( varset.exists( "DOY" ) ) {
       varset.undefine( "DOY" );
    }
    if ( varset.exists( "HOUR" ) ) {
       varset.undefine( "HOUR" );
    }
    if ( varset.exists( "MINUTE" ) ) {
       varset.undefine( "MINUTE" );
    }
    if ( varset.exists( "SECOND" ) ) {
       varset.undefine( "SECOND" );
    }
    if ( varset.exists( "TAG" ) ) {
       varset.undefine( "TAG" );
    }
    
    varset.has_locals = false;
    
}


//...........




Catalog::VarVal* Catalog::getValue( const std::string& name )
{
     bool status;
     Variable* var;
     VarExpr* vexpr;
     VarExpr* texpr;
     VarVal*  val;
     std::map< std::string, Variable>::iterator item;
     VarVal* value;
     VarVal* testval;
     bool doThis;
     VarVal* result;
     
     result = NULLPTR;
     status = false;
     
     if ( varset.exists( name ) ) {

        // are we already trying to evaluate this variable?
        if ( ref[name] ) {
          // throw an error for circular definition
          std::cerr << "Recursive definition of variable " << name << std::endl;
          throw (badRecursiveVar());
        }
        // proceed with the evaluation,
        // but mark this variable name so that we
        // don't get into a definition loop
        ref[name] = true;

        var = varset.getVariable( name );
     
        // for each of the variable's possible valies
        for ( int i = 0; i < var->size() ; i++ ) {
        
            // get the test and value expreessions 
            var->getExprs( i, &vexpr, &texpr );
            
            // evaluate the test expression
            // (Note that eval returns is always a new value, deletable)
            testval = eval( texpr );
            
            // if true, then evaluate the corresponding value expression
            if ( testval != NULLPTR ) {
               if ( testval->type != 'B' ) {
                  // convert testval to Boolean
                  testval->convertType( 'B' );
               }
               if ( ! testval->hasEval() ) {
                  // generate eval
                  testval->n2e();
               }
               
               doThis = testval->evalB;
               
               delete testval;
               
               if ( doThis ) {
                  // it's true
               
                  // eval the value expression
                  result = eval( vexpr );
                  
                  if ( result != NULLPTR ) {               
                     // convert the *value* to the *variable's* type
                     result->convertType( var->type );
                     
                     status = true;
                     
                  } else {
                    std::cerr << "Failed to evaluate value expression in variable " << name 
                    << ", value " << i << ":" << std::endl;
                    if (  dbug > 0 ) {
                       vexpr->dump();
                    }
                    throw (badExpression());
                  }

                  // testval was true, so we do not need to evaluate any
                  // more expressions for this variable
            
                  // break out of the loop
                  break;
               }
               
            } else {
               std::cerr << "Failed to evaluate test expression in variable " << name 
               << ", value " << i << ":" << std::endl;
               if (  dbug > 0 ) {
                  texpr->dump();
               }
               throw (badExpression());
            }
        }
     }
    
     // check if we got any kind of status
     // throw an error if none of the tests evaluated as true
    
     // we are no longer referencing this variable
     ref[name] = false;
     
     return result;
}


Catalog::VarVal* Catalog::opTestEQ( Catalog::VarVal* v1, Catalog::VarVal*v2 )
{
   VarVal* result;
   
   result = NULLPTR;
   
   // do any conversions necessary to make the two value amenable to the same operator
   if ( reconcileVals( v1, v2 ) ) {
         
       result = new VarVal( false );;
   
       switch (v1->type) {
       case 'S':
           result->evalB = ( v1->evalS == v2->evalS );
           break;
       case 'B':
           result->evalB = ( v1->evalB == v2->evalB );
           break;
       case 'I':
           result->evalB = ( v1->evalI == v2->evalI );
           break;
       case 'F':
           result->evalB = ( v1->evalF == v2->evalF );
           break;
       case 'D':
           result->evalB = ( v1->evalD == v2->evalD );
           break;
       }
       result->flags = VarVal::IsLiteral | VarVal::ValidEval;
   }
   
   return result;
}

Catalog::VarVal* Catalog::opTestNE( Catalog::VarVal* v1, Catalog::VarVal*v2 )
{
   VarVal* result;
   
   result = NULLPTR;
   
   // do any conversions necessary to make the two value amenable to the same operator
   if ( reconcileVals( v1, v2 ) ) {
         
      result = new VarVal( false );;
   
      switch (v1->type) {
      case 'S':
          result->evalB = ( v1->evalS != v2->evalS );
          break;
      case 'B':
          result->evalB = ( v1->evalB != v2->evalB );
          break;
      case 'I':
          result->evalB = ( v1->evalI != v2->evalI );
          break;
      case 'F':
          result->evalB = ( v1->evalF != v2->evalF );
          break;
      case 'D':
          result->evalB = ( v1->evalD != v2->evalD );
          break;
      }
      result->flags = VarVal::IsLiteral | VarVal::ValidEval;
   }
   
   return result;
}

Catalog::VarVal* Catalog::opTestGT( Catalog::VarVal* v1, Catalog::VarVal*v2 )
{
   VarVal* result;
         
   result = NULLPTR;
   
   // do any conversions necessary to make the two value amenable to the same operator
   if ( reconcileVals( v1, v2 ) ) {
   
       result = new VarVal( false );;
   
       switch (v1->type) {
       case 'S':
           result->evalB = ( v1->evalS > v2->evalS );
           break;
       case 'B':
           result->evalB = ( v1->evalB > v2->evalB );
           break;
       case 'I':
           result->evalB = ( v1->evalI > v2->evalI );
           break;
       case 'F':
           result->evalB = ( v1->evalF > v2->evalF );
           break;
       case 'D':
           result->evalB = ( v1->evalD > v2->evalD );
           break;
       }
       result->flags = VarVal::IsLiteral | VarVal::ValidEval;
   }
   
   return result;
}

Catalog::VarVal* Catalog::opTestGE( Catalog::VarVal* v1, Catalog::VarVal*v2 )
{
   VarVal* result;
         
   result = NULLPTR;
   
   // do any conversions necessary to make the two value amenable to the same operator
   if ( reconcileVals( v1, v2 ) ) {
   
       result = new VarVal( false );;
       
       switch (v1->type) {
       case 'S':
           result->evalB = ( v1->evalS >= v2->evalS );
           break;
       case 'B':
           result->evalB = ( v1->evalB >= v2->evalB );
           break;
       case 'I':
           result->evalB = ( v1->evalI >= v2->evalI );
           break;
       case 'F':
           result->evalB = ( v1->evalF >= v2->evalF );
           break;
       case 'D':
           result->evalB = ( v1->evalD >= v2->evalD );
           break;
       }
       result->flags = VarVal::IsLiteral | VarVal::ValidEval;
   }
   
   return result;
}

Catalog::VarVal* Catalog::opTestLT( Catalog::VarVal* v1, Catalog::VarVal*v2 )
{
   VarVal* result;
         
   result = NULLPTR;
   
   // do any conversions necessary to make the two value amenable to the same operator
   if ( reconcileVals( v1, v2 ) ) {
   
       result = new VarVal( false );;
   
       switch (v1->type) {
       case 'S':
           result->evalB = ( v1->evalS < v2->evalS );
           break;
       case 'B':
           result->evalB = ( v1->evalB < v2->evalB );
           break;
       case 'I':
           result->evalB = ( v1->evalI < v2->evalI );
           break;
       case 'F':
           result->evalB = ( v1->evalF < v2->evalF );
           break;
       case 'D':
           result->evalB = ( v1->evalD < v2->evalD );
           break;
       }
       result->flags = VarVal::IsLiteral | VarVal::ValidEval;
   }
   
   return result;
}

Catalog::VarVal* Catalog::opTestLE( Catalog::VarVal* v1, Catalog::VarVal*v2 )
{
   VarVal* result;
         
   result = NULLPTR;
   
   // do any conversions necessary to make the two value amenable to the same operator
   if ( reconcileVals( v1, v2 ) ) {
   
       result = new VarVal( false );;
   
       switch (v1->type) {
       case 'S':
           result->evalB = ( v1->evalS <= v2->evalS );
           break;
       case 'B':
           result->evalB = ( v1->evalB <= v2->evalB );
           break;
       case 'I':
           result->evalB = ( v1->evalI <= v2->evalI );
           break;
       case 'F':
           result->evalB = ( v1->evalF <= v2->evalF );
           break;
       case 'D':
           result->evalB = ( v1->evalD <= v2->evalD );
           break;
       }
       result->flags = VarVal::IsLiteral | VarVal::ValidEval;
   }
   
   return result;
}

Catalog::VarVal* Catalog::opTestOR( Catalog::VarVal* v1, Catalog::VarVal*v2 )
{
   VarVal* result;
   
   v1->convertType('B');
   v2->convertType('B');
   
   result = new VarVal( false );;
   
   result->evalB = ( v1->evalB || v2->evalB );
   result->flags = VarVal::IsLiteral | VarVal::ValidEval;
   
   return result;
}

Catalog::VarVal* Catalog::opTestAND( Catalog::VarVal* v1, Catalog::VarVal*v2 )
{
   VarVal* result;
   
   v1->convertType('B');
   v2->convertType('B');
   
   result = new VarVal( false );;
   
   result->evalB = ( v1->evalB && v2->evalB );
   result->flags = VarVal::IsLiteral | VarVal::ValidEval;
   
   return result;
}

Catalog::VarVal* Catalog::opAdd( Catalog::VarVal* v1, Catalog::VarVal*v2 )
{
   VarVal* result;
   
   result = NULLPTR;
   
   // do any conversions necessary to make the two value amenable to the same operator
   if ( reconcileVals( v1, v2 ) ) {
   
       result = v1->copy();
   
       switch (v1->type) {
       case 'S':
           result->evalS = v1->evalS + v2->evalS;
           break;
       case 'B':
           result->evalB = v1->evalB || v2->evalB;
           break;
       case 'I':
           result->evalI = v1->evalI + v2->evalI;
           break;
       case 'F':
           result->evalF = v1->evalF + v2->evalF;
           break;
       case 'D':
           result->evalD = v1->evalD + v2->evalD;
           break;
       }
       result->flags = VarVal::IsLiteral | VarVal::ValidEval;
   }
   
   return result;
}

Catalog::VarVal* Catalog::opSubtract( Catalog::VarVal* v1, Catalog::VarVal*v2 )
{
   VarVal* result;
   
   result = NULLPTR;
   
   // do any conversions necessary to make the two value amenable to the same operator
   if ( reconcileVals( v1, v2 ) ) {
   
       result = v1->copy();
   
       switch (v1->type) {
       case 'S':
           delete result;
           result = NULLPTR;
           break;
       case 'B':
           delete result;
           result = NULLPTR;
           break;
       case 'I':
           result->evalI = v1->evalI - v2->evalI;
           break;
       case 'F':
           result->evalF = v1->evalF - v2->evalF;
           break;
       case 'D':
           result->evalD = v1->evalD - v2->evalD;
           break;
       }
      result->flags = VarVal::IsLiteral | VarVal::ValidEval;
   }
   
   return result;
}

Catalog::VarVal* Catalog::opMultiply( Catalog::VarVal* v1, Catalog::VarVal*v2 )
{
   VarVal* result;
   
   result = NULLPTR;
   
   // do any conversions necessary to make the two value amenable to the same operator
   if ( reconcileVals( v1, v2 ) ) {
   
       result = v1->copy();
   
       switch (v1->type) {
       case 'S':
           delete result;
           result = NULLPTR;
           break;
       case 'B':
           result->evalB = v1->evalB && v2->evalB;
           break;
       case 'I':
           result->evalI = v1->evalI * v2->evalI;
           break;
       case 'F':
           result->evalF = v1->evalF * v2->evalF;
           break;
       case 'D':
           result->evalD = v1->evalD * v2->evalD;
           break;
       }
      result->flags = VarVal::IsLiteral | VarVal::ValidEval;
   }
   
   return result;
}

Catalog::VarVal* Catalog::opDivide( Catalog::VarVal* v1, Catalog::VarVal*v2 )
{
   VarVal* result;
   
   result = NULLPTR;
   
   // do any conversions necessary to make the two value amenable to the same operator
   if ( reconcileVals( v1, v2 ) ) {
   
       result = v1->copy();
   
       switch (v1->type) {
       case 'S':
           delete result;
           result = NULLPTR;
           break;
       case 'B':
           delete result;
           result = NULLPTR;
           break;
       case 'I':
           result->evalI = v1->evalI / v2->evalI;
           break;
       case 'F':
           result->evalF = v1->evalF / v2->evalF;
           break;
       case 'D':
           result->evalD = v1->evalD / v2->evalD;
           break;
       }
      result->flags = VarVal::IsLiteral | VarVal::ValidEval;
   }
   
   return result;
}

Catalog::VarVal* Catalog::opRemainder( Catalog::VarVal* v1, Catalog::VarVal*v2 )
{
   VarVal* result;
   
   result = NULLPTR;
   
   // do any conversions necessary to make the two value amenable to the same operator
   if ( reconcileVals( v1, v2 ) ) {
   
       result = v1->copy();
   
       switch (v1->type) {
       case 'S':
           delete result;
           result = NULLPTR;
           break;
       case 'B':
           delete result;
           result = NULLPTR;
           break;
       case 'I':
           result->evalI = v1->evalI % v2->evalI;
           break;
       case 'F':
           delete result;
           result = NULLPTR;
           break;
       case 'D':
           delete result;
           result = NULLPTR;
           break;
       }
      result->flags = VarVal::IsLiteral | VarVal::ValidEval;
   }
   
   return result;
}

Catalog::VarVal* Catalog::opTestNOT( Catalog::VarVal* v1 )
{
   VarVal* result;
   
   v1->convertType('B');
   
   result = new VarVal(false);
   
   result->evalB = ! v1->evalB;
   result->flags = VarVal::IsLiteral | VarVal::ValidEval;
   
   return result;
}

Catalog::VarVal* Catalog::opNegate( Catalog::VarVal* v1)
{
   VarVal* result;
   
   result = v1->copy();;
   
   switch (v1->type) {
   case 'S':
       delete result;
       result = NULLPTR;
       break;
   case 'B':
           result->evalB = ! v1->evalB;
       break;
   case 'I':
           result->evalI = - v1->evalI;
       break;
   case 'F':
           result->evalF = - v1->evalF;
       break;
   case 'D':
           result->evalD = - v1->evalD;
       break;
   }
   result->flags = VarVal::IsLiteral | VarVal::ValidEval;
   
   return result;
}

Catalog::VarVal* Catalog::opNOP( Catalog::VarVal* v1 )
{
   VarVal* result;
   
   result = v1->copy();
      
   return result;
}


Catalog::VarVal* Catalog::operate( VarOp* op, VarVal* v1 )
{
   VarVal* result;
   
   result = NULLPTR;
   
   if ( eval(v1) && v1->hasLiteral() ) {
      switch ( op->id ) {
      case TestEQ:
         case TestNOT:
              result = opTestNOT( v1 );
              break;
         case Negate:
              result = opNegate( v1 );
              break;
         case NOP:
              result = opNOP( v1 );
              break;     
         default:
                // throw an error: illegal binary operator
                std::cerr << "Illegal unary operator" << std::endl;
                op->dump();
                throw (badExpression());
                break;
         }
   }

   return result;
}

Catalog::VarVal* Catalog::operate( VarOp* op, VarVal* v1, VarVal* v2 )
{
   VarVal* result;
   
   result = NULLPTR;
   
   // check where v1 is a literal, and if not then resolve it.
   if ( eval(v1) && v1->hasLiteral() ) {
      if ( eval(v2) && v2->hasLiteral() ) {
         
         switch ( op->id ) {
         case TestEQ:
                result = opTestEQ( v1, v2 );  
                break;
         case TestNE:
                result = opTestNE( v1, v2 );  
                break;
         case TestGT:
                result = opTestGT( v1, v2 );  
                break;
         case TestGE:
                result = opTestGE( v1, v2 );  
                break;
         case TestLT:
                result = opTestLT( v1, v2 );  
                break;
         case TestLE:
                result = opTestLE( v1, v2 );  
                break;
         case TestOR:
                result = opTestOR( v1, v2 );  
                break;
         case TestAND:
                result = opTestAND( v1, v2 );  
                break;
         case Add:
                result = opAdd( v1, v2 );  
                break;
         case Subtract:
                result = opSubtract( v1, v2 );  
                break;
         case Multiply:
                result = opMultiply( v1, v2 );  
                break;
         case Divide:
                result = opDivide( v1, v2 );  
                break;
         case Remainder:
                result = opRemainder( v1, v2 );  
                break;
         default:
                // throw an error: illegal binary operator
                std::cerr << "Illegal binary operator" << std::endl;
                op->dump();
                throw (badExpression());
                break;
         }
         
      } else {
         // throw error: cannot eval v2;
        std::cerr << "Unable to evaluate second operand in an operation: " << op->id << std::endl;
        v2->dump();
        throw (badExpression());
      }
   } else {
       // throw error: cannot eval v1 
        std::cerr << "Unable to evaluate first operand in an operation: " << op->id << std::endl;
        v1->dump();
        throw (badExpression());
   }
   
   
   return result;

}

Catalog::VarVal* Catalog::eval( Catalog::VarExpr* expr )
{
     VarVal* result;
     VarVal* vv;
     VarVal* v1;
     VarVal* v2;
     VarOp* op;
     VarOp* op1;
     int idx_op;
     int idx_val;
     int n_items;
     int n_val;
     int pri;
     int junk;
     std::deque<VarVal*> valStack;
     bool ok;
     int i;
     VarExprItem* item;
     
     
     result = NULLPTR;
     
     if ( dbug > 60 ) {
        std::cerr << "Catalog::eval(expr): starting" << std::endl;
        expr->dump(5);
     }
     
          
     // get the number of items in the expression list     
     n_items = expr->size();
     
     for ( i = 0; i < n_items; i++ ) {
     
         item = expr->getItem( i );
        
         if ( item->isVal ) {
            // value. push it on the stack
            // note: we copy values, since they may change.
            // we do not copy operators, since they will not change/
            v1 = (item->val)->copy();
            if (  v1->hasNominal() && ! v1->hasEval() ) {
               eval( v1 );
            }
            if ( ( ! v1->hasLiteral() ) && v1->hasEval() ) {
                // it's just a value
                vv = lookup( v1->evalS );
                if ( vv != NULLPTR ) {
                   vv->fmt1 = v1->fmt1;
                   vv->fmt2 = v1->fmt2;
                   
                   delete v1;
                   v1 = vv;
                } else {
                   std::cerr << " unable to look up referenced name in value " << std::endl;
                   v1->dump(5);
                   throw (badExpression());
                }
            }    
            valStack.push_back( v1 );

            if ( dbug > 60 ) {
               std::cerr << "Catalog::eval(expr): pushed value onto valstack" << std::endl;
               v1->dump(5);
            }
            
         } else if ( item->isOp ) {
                  
            op1 = item->op;
            if ( dbug > 60 ) {
               std::cerr << "Catalog::eval(expr): encountered operator" << std::endl;
               op1->dump(5);
            }
            
            if ( op1->nops == 1 ) {
               // unary operator
               
               // check that valStack has at least one element


               v1 = valStack.back();
               valStack.pop_back();
                if ( dbug > 60 ) {
                   std::cerr << "Catalog::eval(expr): popped operand off valstack for operator" << std::endl;
                   v1->dump(10);
                }
               
               // apply operator to value
               vv = operate( op1, v1 );
               if ( vv != NULLPTR ) {
                  // and push the result back onto the stack
                  valStack.push_back( vv );
                  if ( dbug > 60 ) {
                     std::cerr << "Catalog::eval(expr): pushed operator result onto valstack" << std::endl;
                     vv->dump(10);
                  }
                  delete v1;
               } else {
                   // throw error: failed to apply operator to value
                   std::cerr << "Failed to apply unary operator " << op1->id << " to value " << std::endl;
                   v1->dump(5);
                   throw (badExpression());
               }         
            } else if ( op1->nops == 2 ) {
            
                // check that valStack has at least two elements
               
                v2 = valStack.back();
                valStack.pop_back();
                v1 = valStack.back();
                valStack.pop_back();
                if ( dbug > 60 ) {
                   std::cerr << "Catalog::eval(expr): popped operands off valstack for operator" << std::endl;
                   v1->dump(10);
                   v2->dump(10);
                }
                
                vv = operate( op1, v1, v2 );
                if ( vv != NULLPTR ) {
                   // and push the result back onto the stack
                   valStack.push_back( vv );
                   if ( dbug > 60 ) {
                      std::cerr << "Catalog::eval(expr): pushed operator result onto valstack" << std::endl;
                      vv->dump(10);
                   }
                   delete v2;
                   delete v1;
                } else {
                    // throw error: failed to apply operator to value
                   std::cerr << "Failed to apply binary operator " << op1->id << " to values " << std::endl;
                   v1->dump(5);
                   v2->dump(5);
                   throw (badExpression());
                }         
                
            } else {
               // throw an arror: unexpected number of operands for operator
              std::cerr << "Operator with an improper number of operands" << std::endl;
              op->dump(5);
              throw (badExpression());
            }
         
         } else {
            // throw error: item is neither a value nor an operator
              std::cerr << "Expression item is neither an operator nor an operancd!?" << std::endl;
              item->dump(5);
              throw (badExpression());
         }
     
     
     }

     // all done with the eval
     
     if ( valStack.size() != 1 ) {
        // throw an error: expressiondoes not result in a single value on the stack: insufficient operators
        std::cerr << "Expression has operands left over after all operators have been applied:" << std::endl;
        expr->dump(5);
        throw (badExpression());
     }     
     
     result = valStack[0];
     valStack.pop_back();

     if ( dbug > 60 ) {
        std::cerr << "Catalog::eval(expr): ending w/" << std::endl;
        result->dump(5);
     }
     
     return result;
}

bool Catalog::eval( VarVal* val )
{
     bool result;
     std::string name;
     int fmt1, fmt2;
     std::string teststring;
     const char* str;
     size_t idx;
     VarVal* found;
     bool ok;
     
     result = false;
     ok = false;
     
     if ( dbug > 10 ) {
        std::cerr << "Catalog::eval(val): starting with VarVal:" << std::endl;
        val->dump(3);
     }
     
     if ( val->type != 'S' && val->type != 'D' ) {
        // it's not a string or a date:
        if ( dbug > 50 ) {
           std::cerr << "Catalog::eval(val): not a string or date" << std::endl;
        }
        
        if ( ! val->hasLiteral() ) {
           // it's some kind of variable reference
           if ( val->hasEval() ) {
              name = val->evalS;
           } else if ( val->hasNominal() ) {
              val->e2n();
              if ( val->hasEval() ) {
                 name = val->evalS;  
              } else {
                 // throw an error 
                 std::cerr << "No valid value to interpolate: " << std::endl;
                 val->dump();
                 throw (badExpression());
              }
           } else {
                 std::cerr << "No value to interpolate: " << std::endl;
                 val->dump();
                 throw (badExpression());
           }
           if ( dbug > 50 ) {
              std::cerr << "Catalog::eval(val): not a literal. Ref is '" << name << "'" << std::endl;
              std::cerr << " now lookup " << name << std::endl;
           }
           
           // we have the name
           /// look up its reference value
           found = lookup( name );

           ok = ( found != NULLPTR );
           if ( ok ) {
              if ( dbug > 50 ) {
                 std::cerr << "Catalog::eval(val): found " << name << std::endl;
                 found->dump();
              }
               // convert it  to our value's type       
               found->convertType( val->type );
               // copy its contents to our value, except keep the format
               int fmt1 = val->fmt1;
               int fmt2 = val->fmt2;
               *val = *found;
               val->fmt1 = fmt1;
               val->fmt2 = fmt2;
               // no more need for this
               delete found;
           }
           
        } else {
           // it's already a literal
           result = true;
        }
        
     } else if ( val->type == 'D' ) {
        // it's a date

        if ( val->hasNominal() ) {
           teststring = val->nominal;
        } else if ( val->hasEval() ) {
           teststring = val->evalS;
        } else {
            teststring = "";
        }
        
        if ( ! val->hasLiteral() ) {
           // it's some kind of variable reference
           if ( val->hasEval() ) {
              // If there is an eval value and it's a reference
              // then the variable name has been stashed away here
              name = val->evalS;
           } else if ( val->hasNominal() ) {
              // might still be a simple variable ref,
              // if there is a nominal value that has not bene evaluated yet
              // but it might also be a literal-like string with embedded var refs
              val->n2e();
              if ( val->hasEval() ) {
                 // ok it was a simple var ref after all
                 name = val->evalS;  
              } else {
                 // not a simple variable ref.
                 // hopefully, it's a literal-like string with variable refs embedded in it
                 teststring = interpVarRefs( val->nominal );
                 if ( isDateLiteral( teststring.c_str(), teststring.size(), 0 ) != 0 ) {
                    // this will be a flag for testing below
                    name = "";
                 } else {
                    // hopeless
                    std::cerr << "No valid value to interpolate: " << std::endl;
                    val->dump();
                    throw (badExpression());
                 }
              }
           } else {
                 std::cerr << "No value to interpolate: " << std::endl;
                 val->dump();
                 throw (badExpression());
           }
           if ( dbug > 50 ) {
              std::cerr << "Catalog::eval(val): not a literal. Ref is '" << name << "'" << std::endl;
           }
           
           if ( name != "" ) {
              // we have the name
              /// look up its reference value
              found = lookup( name );

              ok = ( found != NULLPTR );
              if ( ok ) {
                 if ( dbug > 50 ) {
                    std::cerr << "Catalog::eval(val): found " << name << std::endl;
                    found->dump();
                 }
                  // convert it  to our value's type       
                  found->convertType( val->type );
                  // copy its contents to our value, except keep the format
                  int fmt1 = val->fmt1;
                  int fmt2 = val->fmt2;
                  *val = *found;
                  val->fmt1 = fmt1;
                  val->fmt2 = fmt2;
                  // no more need for this
                  delete found;
              }
           } else {
              // no name means this was a literal-like string
              // with embedded var refs in it
              
              found = new VarVal( teststring, 'D' );
              if ( found != NULLPTR ) {
                 if ( found->hasLiteral() ) {
                    val->evalD = found->evalD;
                    val->flags = val->flags | VarVal::ValidEval | VarVal::IsLiteral;
                    result = true;
                    delete found;
                 } else {
                    std::cerr << "Bad date pseudo-literal: " << teststring << std::endl;
                    val->dump();
                    delete found;
                    throw (badExpression());
                 }
              } else {
                 std::cerr << "invalid date pseudo-literal: " << teststring << std::endl;
                 val->dump();
                 throw (badExpression());              
              }
           }
           
        } else {
           // it's already a literal
           result = true;
        }
      
     } else { 
        // it's a string, which means we will need to  substitute var refs,
        // possibly in the middle of the string
        
        
        if ( val->hasNominal() ) {
           teststring = val->nominal;
        } else if ( val->hasEval() ) {
           teststring = val->evalS;
        } else {
            teststring = "";
        }

        teststring = interpVarRefs( teststring );
                
        result = true;
        val->evalS  = teststring;
        val->flags = VarVal::ValidNominal | VarVal::ValidEval | VarVal::IsLiteral;   
     
     }

     if ( dbug > 10 ) {
        std::cerr << "Catalog::eval(val): ending w/" << std::endl;
        val->dump();
     }
     return result;
}

std::string Catalog::interpVarRefs( const std::string refstr )
{
    std::string teststring;
    size_t idx;
    const char* str;
    int len;
    bool more;
    bool eos;
    bool foundref;
    std::string newstring;
    std::string refname;
    std::string name;
    int fmt1, fmt2;
    VarVal* found;
    std::string tmpstring;
    
    
    teststring = refstr;
    
    str = teststring.c_str();
    len = teststring.size();
    
    if ( dbug > 50 ) {
       std::cerr << "Catalog::eval(val):  entry with: '" << teststring << "." << std::endl;
    }
    
    more = true;
    
    while ( more ) {
       if ( dbug > 100 ) {
          std::cerr << "Catalog::eval(val): teststring ='" << teststring << "'" << std::endl;  
       }
      
       newstring = "";
       idx = 0;
       
       // end-of-string
       eos = false;
       
       foundref = false;
       
       while ( ! eos ) {
          if ( dbug > 100 ) {
             std::cerr << "Catalog::eval(val):    idx =" << idx << std::endl;  
          }
       
          // search for something that might be a var ref
          while ( str[idx] != 0 && str[idx] != '$' ) {
              newstring.push_back( str[idx] );
              idx++;
          }
          if ( dbug > 100 ) {
             std::cerr << "Catalog::eval(val):    idx is now " << idx 
             << ", or char '" << str[idx] << "'" << std::endl;  
          }
          if ( str[idx] == '$' ) {
             if ( dbug > 100 ) {
                std::cerr << "Catalog::eval(val):    found $. Extract var ref." << std::endl;  
             }
             // ok, this might be a var ref
             extractVarRef( str, idx, name, refname, &fmt1, &fmt2 );
             if ( name != "" ) {
                // yes! it is a var ref
                if ( dbug > 100 ) {
                   std::cerr << "Catalog::eval(val): found a var ref for " << name  << std::endl;
                   std::cerr << "Catalog::eval(val): looking up " << refname << std::endl;
                }
                found = lookup ( refname );
                
                if ( found != NULLPTR ) {
                    if ( dbug > 100 ) {
                       std::cerr << "Catalog::eval(val): var ref " << name  
                       << " resolved to: '" << "'" << std::endl;
                       found->dump(5);
                    }
                    if ( fmt1 >= 0 ) {
                       found->fmt1 = fmt1;
                       if ( fmt2 >= 0 ) {
                          found->fmt2 = fmt2;
                       }
                    }
                    if ( found->print( tmpstring ) ) {
                       if ( dbug > 100 ) {
                          std::cerr << "Catalog::eval(val): var ref " << name  
                          << " printed as '" << tmpstring << "'" << std::endl;
                       }
                       newstring = newstring + tmpstring;
                    }  else {
                       std::cerr << "Unable to obtain a print string from value ref " << name << std::endl;
                       found->dump(5);
                       throw (badExpression());
                    }
                    
                    delete found;
                } else {
                    // nothing with the desired name was found
                    std::cerr << "Could not find anything with the referenced name '" << name << "'" << std::endl;                    
                    throw (badExpression());
                }
                
             } else {
                // not a var ref after all.
                // continue
                newstring.push_back( str[idx] );               
                idx++;
                if ( dbug > 100 ) {
                   std::cerr << "Catalog::eval(val): $ not part of a var ref " << std::endl;
                }
             }   
          } else {
             // no '$' found; must be the terminating null
              eos = true;
              if ( dbug > 100 ) {
                 std::cerr << "Catalog::eval(val): at end of test string " << std::endl;
              }
          }
       }
       if ( dbug > 50 ) {
          std::cerr << "Catalog::eval(val): newstring ='" << newstring << "'" << std::endl;  
       }
       
       if ( teststring != newstring ) {
          teststring = newstring;
          str = teststring.c_str();
          len = teststring.size();    
       } else {
          more = false;
       }  
    }

    return teststring;

}

Catalog::VarVal* Catalog::lookup( const std::string& name )
{
     VarVal* result;
     std::map< std::string, std::string>::iterator attr;
     std::string valstr;
     bool ok;
     
     result = NULLPTR;
          
     if ( dbug > 60 ) {
        std::cerr << "Catalog::lookup begins w/ name = " << name << std::endl;
     }

     ok = false;
     if ( varset.exists( name ) ) {
         //  is the name a defined variable? get it from getValue.
         
         result = getValue( name );
         ok = ( result != NULLPTR );

         if ( dbug > 60 ) {
            std::cerr << "Catalog::lookup simple ref: " << std::endl;
            if ( ok ) {
               result->dump(8);
            } else {
               std::cerr << "  null pointer" << std::endl;
            }
         }
         
     } else {
         if ( dbug > 60 ) {
            std::cerr << "Catalog::lookup undefined var! ";
         }
     
         // check the current target's attributes
         if ( currentTarget != NULLPTR ) {
            for (attr = currentTarget->attrs.begin(); attr != currentTarget->attrs.end(); attr++ ) {                   
                if ( attr->first == name ) {
                   valstr = attr->second;
                   ok = true;
                   if ( dbug > 60 ) {
                      std::cerr << "Catalog::lookup found target attribute! ";
                   }
                   break;
                }
            }
         }
         // still nothing? check the environment
         if ( ! ok ) {
            
            valstr = getEnv( name );
            if ( valstr != "" ) {
               ok = true;
               if ( dbug > 60 ) {
                  std::cerr << "Catalog::lookup found env variable! ";
               }
            }
     
         }
         
         // make a new value with the retrived string as its contents
         if ( ok ) {
           
            result = new VarVal( valstr, 'S' );   
            if ( dbug > 60 ) {
               std::cerr << "Catalog::lookup found value: " << valstr;
               result->dump(8);
            }
     
         }
     }
     
     return result;

}


bool Catalog::reconcileVals( Catalog::VarVal* v1, Catalog::VarVal* v2 )
{
    bool result;
    VarVal vtmp;
    
    result = false;
         
    if ( v1->type != v2->type ) {
    
       if ( v1->type == 'S' ) {
          // try converting string v1 to non-string v2's type
          vtmp = *v1;
          vtmp.convertType( v2->type );
          if ( vtmp.hasEval() ) {
             // ok. copy it back
             *v1 = vtmp;
             result = true;
          } else {
             // conversion failed. Try converting the other one to S
             v2->convertType( v1->type );
             result = true;
          }   
       } else if ( v2->type == 's' ) {
          // try converting string v2 to non-string v1's type
          vtmp = *v2;
          vtmp.convertType( v1->type );
          if ( vtmp.hasEval() ) {
             // ok. copy it back
             *v2 = vtmp;
             result = true;
          } else {
             // conversion failed. Try converting the other one to S
             v1->convertType( v2->type );
             result = true;
          }   
       } else if ( v1->type == 'B' ) {
          v1->convertType( v2->type );
          result = v2->hasEval();
       } else if ( v2->type == 'B' ) {
          v2->convertType( v1->type );
          result = v2->hasEval();
       } else if ( v1->type == 'I' ) {
          v1->convertType( v2->type );
          result = v2->hasEval();
       } else if ( v2->type == 'I' ) {
          v2->convertType( v1->type );
          result = v2->hasEval();
       } else if ( v1->type == 'F' ) {
          v1->convertType( v2->type );
          result = v2->hasEval();
       } else if ( v2->type == 'F' ) {
          v2->convertType( v1->type );
          result = v2->hasEval();
       } else if ( v1->type == 'D' ) {
          v1->convertType( v2->type );
          result = v2->hasEval();
       } else if ( v2->type == 'D' ) {
          v2->convertType( v1->type );
          result = v2->hasEval();
       }   
    
       if ( dbug > 60 ) {
          std::cerr << "Catalog::lookup ends w/ result = " << std::endl;
          v2->dump();
          std::cerr << "  w/ result = " << result << std::endl;
       }
    
    } else {
       result = true;
    }

   return result;

}

double Catalog::date2number( const std::string& time )
{
     double result;
     bool ok;
     
     if ( ! s2Date( time, result ) ) {
        result = -1.0;
     }

     return result;
}

void Catalog::dump( int indent ) const
{
   std::string spaces;
   
   for ( int i=0; i < indent; i++ ) {
       spaces.push_back(' ');
   }
   
   std::cerr << spaces << "************ Catalog Dump: ************" << std::endl;
   
   std::cerr << std::endl;
   varset.dump(10);
   std::cerr << std::endl;
   dimset.dump(10);
   std::cerr << std::endl;
   targetset.dump(10);
   std::cerr << std::endl;
   quantityset.dump(10);
   
}

double Catalog::advanceTime( double base, TimeInterval& tinc )
{
     int year;
     int month;
     int dayofmonth;
     int dayofyear;
     int hours;
     int minutes;
     float seconds; 
     double ydays;
     double mdays;
     double add_days;
     double add_time;
     double result;

     if ( tinc.allHere ) {
        result = base;
     }

     // break the base time into year, month, day, etc.
     d2parts( base, year, month, dayofmonth, dayofyear, hours, minutes, seconds ); 
     
     // We start by finding the number of days to advance for the years separation
     ydays = 0.0;
     for ( int i=1; i<=tinc.years; i++ ) {
     
         ydays = ydays + 365.0;
         // If this is a leap year, and a 1-year jump includes the leap day...
         if ( isLeap( year ) && ( month <= 2 ) ) {
            ydays = ydays + 1.0;
         }
         // If we are jumping into a leap year, past its leap day...
         if ( isLeap( year + 1 ) && ( month > 2 ) ) {
            ydays = ydays + 1.0;
         }
     
         // on to the next 1-year jump
         year++;
         
     }
     
     // Now find the number of days to advance for the months separation
     // (note that we start after advancing any years)
     mdays = 0.0;
     for ( int i=1; i<=tinc.months; i++ ) {
         
         int monlen = monthlengths[ month ];
         if ( isLeap(year) && (month == 2) ) {
            monlen++;
         } 
         
         mdays = mdays + static_cast<double>(monlen);
         
         // on to the next month
         month++;
         // but take care of advancing into the next year
         if ( month > 12 ) {
            year++;
            month = 1;
         }
         
     }
     
     // advancing days ahead is easy
     add_days = static_cast<double>( tinc.days );
     
     // advancing the time interval (in seconds), converted to hours for now
     add_time = tinc.secs/3600.0;
     
     // do we need to impose an external desired time spacing?
     if ( des_tinc > add_time ) {
      
        // check that an integral number of urlSep periods fits 
        // within the desired time spacing
        int des_n = round( des_tinc / add_time );
        if ( abs( (des_n*add_time) - des_tinc ) < 1e-12 ) {
           add_time = des_tinc;
        }
     
     
     }
     add_time = add_time/24.0; // convert hours->days
     
     
     // advance the base date by our interval
     result = base + ydays + mdays + add_days + add_time;

     return result;
}


double Catalog::retreatTime( double base, TimeInterval& tinc )
{
     int year;
     int month;
     int dayofmonth;
     int dayofyear;
     int hours;
     int minutes;
     float seconds; 
     double ydays;
     double mdays;
     double add_days;
     double add_time;
     double result;

     if ( tinc.allHere ) {
        result = base;
     }

     // break the base time into year, month, day, etc.
     d2parts( base, year, month, dayofmonth, dayofyear, hours, minutes, seconds ); 
     
     // We start by finding the number of days to advance for the years separation
     ydays = 0.0;
     for ( int i=1; i<=tinc.years; i++ ) {
     
         ydays = ydays + 365.0;
         // If this is a leap year, and a 1-year jump includes the leap day...
         if ( isLeap( year ) && ( month > 2 ) ) {
            ydays = ydays + 1.0;
         }
         // If we are jumping into a leap year, past its leap day...
         if ( isLeap( year - 1 ) && ( month <= 2 ) ) {
            ydays = ydays + 1.0;
         }
     
         // on to the next 1-year jump
         year--;
         
     }
     
     // Now find the number of days to advance for the months separation
     // (note that we start after advancing any years)
     mdays = 0.0;
     for ( int i=1; i<=tinc.months; i++ ) {
         
         int monlen = monthlengths[ month ];
         if ( isLeap(year) && (month == 2) ) {
            monlen++;
         } 
         
         mdays = mdays + static_cast<double>(monlen);
         
         // on to the next month
         month--;
         // but take care of retreatinging into the previous year
         if ( month < 1 ) {
            year--;
            month = 12;
         }
         
     }
     
     // retreating days behind is easy
     add_days = static_cast<double>( tinc.days );
     
     // retreating the time interval (in seconds), converted to hours for now
     add_time = tinc.secs/3600.0;
     
     // do we need to impose an external desired time spacing?
     if ( des_tinc > add_time ) {
      
        // check that an integral number of urlSep periods fits 
        // within the desired time spacing
        int des_n = round( des_tinc / add_time );
        if ( abs( (des_n*add_time) - des_tinc ) < 1e-12 ) {
           add_time = des_tinc;
        }
     
     
     }
     add_time = add_time/24.0; // convert hours->days
     
     
     // advance the base date by our interval
     result = base - ydays - mdays - add_days - add_time;

     return result;
}

void Catalog::bracket( double tyme, double& pre_t, double& pre_url_t, int& pre_n, double& post_t, double& post_url_t, int& post_n, int& ntot )
{
     VarVal* tmpVal;
     double based;
     double prev_based;
     int year;
     int month;
     int dayofmonth;
     int dayofyear;
     int hours;
     int minutes;
     float seconds; 
     double add_time;
     double ydays;
     double mdays;
     double add_days;
     double tinc;
     double intra_delta;
     double spac;
 
     // get the Target's base date VarVal
     tmpVal = currentTarget->dbase.copy();
     // evaluate it, so that we end up with a numeric time
     eval( tmpVal );
     if ( ! tmpVal->hasEval() ) {
        std::cerr << "Bad Target base date: " << currentTarget->dbase.nominal << std::endl;
        throw (badDateString());
     }
     // and here we have it
     based = tmpVal->evalD;
     
     // If the user is specifying a time offset, we apply that here
     if ( des_toff >= 0 ) {
     
        based = floor(based) + des_toff/24.0;
        
        // Take care of the case where the base time is now
        // after the desired time
        if ( based > tyme ) {
           based = based - des_tinc/24.0;
        }
     
     }
     
     
     // Now we advance the base time until we pass the desired time     
     if ( ! currentTarget->urlSep.allHere ) {

        if ( based <= tyme ) {
           while ( based <= tyme ) {
     
               /* The spacing between successive URLs is held in currentTarget.urlSep.
                  This is broken into parts: year, month, day, seconds.
                  This is necessary because years and months have varying durations.
               */
                    
               // save the current base date (it may become our preceeding time)
               prev_based = based;
               
               // advance the base date by our interval
               based = advanceTime( prev_based, currentTarget->urlSep );

           }
        } else {
           
           prev_based = based;
           
           while ( prev_based > tyme ) {
     
               /* The spacing between successive URLs is held in currentTarget.urlSep.
                  This is broken into parts: year, month, day, seconds.
                  This is necessary because years and months have varying durations.
               */
                    
               // retreat the base date by our interval
               prev_based = retreatTime( based, currentTarget->urlSep );

               if ( prev_based > tyme ) {
                  // (note: this should never be necessary, since we get here
                  // only if the desired tyme is earlier than the base time,
                  // which should only happen if the desired time is less
                  // than one time step before the base time)
                  // save the current base date to become our following time, if necessary
                  based = prev_based;
               }         
           }
        }
     } else {
        // all the timestamps are in one URL
        prev_based = based;
     }
     
     // these two are the times of the URLs that bracket the desired time(tyme); 
     pre_url_t  = prev_based;
     post_url_t = based;
     
     
     // how far into the pre URL would our desired time be?
     intra_delta = tyme - pre_url_t;

     // get the internal-to-URL time increment, converted from hours to days 
     tinc = currentTarget->inctime/24.0;

     // determine the number of time snapshots in the pre URL
     // and the offset index into the URL of the data just before our desired time
     if ( tinc > 0 ) {
        ntot = static_cast<int>( floor( (based - prev_based)/tinc + 1.0/24.0/60.0*0.5 ) );
        pre_n = static_cast<int>( floor( intra_delta/tinc + 1.0/24.0/60.0*0.5 ) );
     } else {
        // one snapshot per file, and this is it
        ntot = 1;
        pre_n = 0;
     }
     
     pre_t = pre_url_t + pre_n*tinc;

     // now turn to the time that follows our desired time
     
     // The spacing to the next time tick is by default given by tinc.
     spac = tinc;
     if ( tinc <= 0 ) {
        // only one snapshot in the URL,
        // so the spacing is between URLs
        spac = post_url_t - pre_url_t;
     }
     if ( spac < des_tinc/24.0 ) {
        // impose a desired time increment
        double hrspac = spac*24.0;
        int des_n = round( des_tinc / hrspac );
        if ( abs( (des_n*hrspac) - des_tinc ) < 1e-12 ) {
           spac = des_tinc/24.0;
        }
     }
     
     // advance to the next data time snapshot
     post_t = pre_t + spac;
     
     
     if ( post_t < post_url_t ) {
     
        // our post time is before the next URL time.
        // So we won;t have to go to the next URL
        // We just use the pre URL for post
        post_url_t = pre_url_t;
     }
     
     // find how far into the post URL we need to go
     post_n = 0;
     if ( post_t > post_url_t ) {
     
        intra_delta = post_t - post_url_t;
     
        if ( tinc > 0 ) {
           post_n = static_cast<int>( floor( intra_delta/tinc + 1.0/24.0/60.0*0.5 ) );
        }
     
        // should not be necessary, but there may be roundoff
        post_t = post_url_t + post_n*tinc;
        
     }
     
     

     delete tmpVal;
}


void Catalog::filter_DataSource( std::vector<Catalog::DataSource>& ds )
{
    size_t n;
    std::vector<int> scores;
    int indiv_score;
    size_t na;
    std::string aname;
    std::string tname;
    std::string tval;
    std::string dval;
    int priority;
    std::vector<Catalog::DataSource> dtmp;
    int max_score;
    
    na = attrNames.size();
    
    n = ds.size();
    
    if ( n > 0 && na > 0 ) {
    
       max_score = 0;
       
       for ( int is=0; is < n; is++ ) { 
    
           tname = ds[is].target;
    
           indiv_score = 0;
           for ( int ia=0; ia < na; ia++ ) {
           
               aname = attrNames[ia];
               
               
               dval = desired( aname, &priority );
               
               if ( dval != "" ) {
                  
                  tval = getAttr( tname, aname );
               
                  if ( dval == tval ) {
                  
                     if ( priority != -9999 ) {
                        indiv_score = indiv_score + 1;
                     } else {
                         // was this match forbidden?
                         indiv_score = -1;
                         break;
                     }               
                  } else {
                     // the attributes do not match
                     
                     // was a match required?
                     if ( priority == 9999 ) {
                         indiv_score = -1;
                         break;
                     }
                  }
           
               }
           }
           
           scores.push_back( indiv_score );
           if ( indiv_score > max_score ) {
              max_score = indiv_score;
           }
    
       }
       
       if ( max_score > 0 ) {
          
          // make a copy
          dtmp = ds;
    
          // and clear out the old to make way for the new
          ds.clear();
          
          // put the high-scorers first, and omit any negative-scorers
          for ( indiv_score = max_score; indiv_score >= 0; indiv_score-- ) {
       
              for ( int is=0; is < n; is++ ) {
               
                  if ( scores[is] == indiv_score ) {
                     ds.push_back( dtmp[is] );
                  }
              
              }    
       
          }   
       }
       
    }
}

