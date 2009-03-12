/* ---------------------------------------------------------------------------
 #
 #       L-Py: L-systems in Python
 #
 #       Copyright 2003-2008 UMR Cirad/Inria/Inra Dap - Virtual Plant Team
 #
 #       File author(s): F. Boudon (frederic.boudon@cirad.fr)
 #
 # ---------------------------------------------------------------------------
 #
 #                      GNU General Public Licence
 #
 #       This program is free software; you can redistribute it and/or
 #       modify it under the terms of the GNU General Public License as
 #       published by the Free Software Foundation; either version 2 of
 #       the License, or (at your option) any later version.
 #
 #       This program is distributed in the hope that it will be useful,
 #       but WITHOUT ANY WARRANTY; without even the implied warranty of
 #       MERCHANTABILITY or FITNESS For A PARTICULAR PURPOSE. See the
 #       GNU General Public License for more details.
 #
 #       You should have received a copy of the GNU General Public
 #       License along with this program; see the file COPYING. If not,
 #       write to the Free Software Foundation, Inc., 59
 #       Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 #
 # ---------------------------------------------------------------------------
 */

#include "matching.h"
#include "axialtree_manip.h"
#include <boost/python.hpp>

LPY_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/
MatchingEngine::eModuleMatchingMethod 
MatchingEngine::ModuleMatchingMethod = MatchingEngine::eDefaultModuleMatching;

void MatchingEngine::setModuleMatchingMethod(MatchingEngine::eModuleMatchingMethod method)
{ ModuleMatchingMethod = method; }

MatchingEngine::eModuleMatchingMethod LPY::MatchingEngine::getModuleMatchingMethod()
{ return ModuleMatchingMethod; }

MatchingEngine::eStringMatchingMethod 
MatchingEngine::StringMatchingMethod = MatchingEngine::eDefaultStringMatching;

void MatchingEngine::setStringMatchingMethod(MatchingEngine::eStringMatchingMethod method)
{ 
	StringMatchingMethod = method; 
}

MatchingEngine::eStringMatchingMethod LPY::MatchingEngine::getStringMatchingMethod()
{ return StringMatchingMethod; }

/*---------------------------------------------------------------------------*/


bool MatchingEngine::module_match(const ParamModule& module, 
								  const ParamModule& pattern,
								  boost::python::list& params) 
{
  switch(ModuleMatchingMethod){
   case eMSimple:
	return MatchingEngineImplementation::simple_module_matching(module,pattern,params);
   case eMWithStar:
	return MatchingEngineImplementation::module_matching_with_star(module,pattern,params);
   default:
	return MatchingEngineImplementation::module_matching_with_star_and_valueconstraints(module,pattern,params);
  }
}


bool MatchingEngine::match(AxialTree::const_iterator  matching_start,
						   AxialTree::const_iterator  string_end,
						   AxialTree::const_iterator  pattern_begin,
						   AxialTree::const_iterator  pattern_end,
						   AxialTree::const_iterator& matching_end,
						   boost::python::list& params) 
{
	return MatchingEngineImplementation::string_exact_match(matching_start,string_end,
						                              pattern_begin,pattern_end,
						                              matching_end,params);
}

bool MatchingEngine::reverse_match(AxialTree::const_iterator matching_start,
								   AxialTree::const_iterator  string_begin,
								   AxialTree::const_reverse_iterator  pattern_rbegin,
								   AxialTree::const_reverse_iterator  pattern_rend,
								   AxialTree::const_iterator& matching_end,
								   boost::python::list& params)
{ 
	return MatchingEngineImplementation::string_exact_reverse_match(matching_start,string_begin,
						                               pattern_rbegin,pattern_rend,
						                               matching_end,params);
}

bool MatchingEngine::right_match(AxialTree::const_iterator  matching_start,
								 AxialTree::const_iterator  string_end,
								 AxialTree::const_iterator  pattern_begin,
								 AxialTree::const_iterator  pattern_end,
								 AxialTree::const_iterator& matching_end,
								 boost::python::list& params) 
{
	switch(StringMatchingMethod){
		case eString:
			return MatchingEngineImplementation::string_exact_match(matching_start,string_end,
						                              pattern_begin,pattern_end,
						                              matching_end,params);
		default:
			return MatchingEngineImplementation::tree_right_match(matching_start,string_end,
						                              pattern_begin,pattern_end,
						                              matching_end,params);
	}
}

bool MatchingEngine::left_match(AxialTree::const_iterator  matching_start,
								AxialTree::const_iterator  string_begin,
								AxialTree::const_iterator  string_end,
								AxialTree::const_reverse_iterator  pattern_rbegin,
								AxialTree::const_reverse_iterator  pattern_rend,
								AxialTree::const_iterator& matching_end,
								boost::python::list& params) 
{
	switch(StringMatchingMethod){
		case eString:
			return MatchingEngineImplementation::string_exact_reverse_match(matching_start,string_begin,
						                                 pattern_rbegin,pattern_rend,
						                                 matching_end,params);
		default:
			return MatchingEngineImplementation::tree_left_match(matching_start,string_begin,string_end,
						                                 pattern_rbegin,pattern_rend,
						                                 matching_end,params);
	}
}

/*---------------------------------------------------------------------------*/

bool 
MatchingEngineImplementation::simple_module_matching(const ParamModule& module, 
													 const ParamModule& pattern, 
													 boost::python::list& l)
{
  if( module.sameName(pattern) && module.argSize() == pattern.argSize()){
	l = module.getArgs();
	return true;
  }
  return false;
}

bool MatchingEngineImplementation::module_matching_with_star(const ParamModule& module, 
															 const ParamModule& pattern, 
									                         boost::python::list& l)
{
  if (pattern.isStar()){
	size_t s = pattern.argSize();
	if (s == 0) return true;
	if (s == 1) {
	  size_t s2 = module.argSize();
	  boost::python::object of = pattern.getAt(0);
	  LsysVar v = boost::python::extract<LsysVar>(of);
	  if(v.isArgs()) { 
		  boost::python::list largs;
		  largs.append(boost::python::object(module.name()));
		  largs += module.getArgs();
		  l.append(largs); return true; 
	  }
	  else {
		if( s2 == 0){ l.append(boost::python::object(module.name()));return true; }
	    else return false;
	  }
	}
	else {
	  size_t s2 = module.argSize();
	  if (s2 < s-2) return false;
	  boost::python::object of = pattern.getAt(s-1);
	  LsysVar v = boost::python::extract<LsysVar>(of)();
	  if(!v.isArgs()){
  		if (s2 != s-1) return false;
		l.append(boost::python::object(module.name()));
		l += module.getArgs();
		return true;
	  }
	  l.append(boost::python::object(module.name()));
	  if(s != 2){
		for(int i = 0; i < s-2; i++)
		  l.append(module.getAt(i));
		if(s2 > s-2)l.append(module.getslice(s-2,s2));
		else l.append(boost::python::list());
	  }
	  else l.append(module.getArgs());
	  return true;
	}
  }
  else if(module.isStar())return module_matching_with_star(pattern,module,l);
  else {
	if (!module.sameName(pattern)) return false;
	size_t s = pattern.argSize();
    size_t s2 = module.argSize();
	if (s2 < s-1) return false;
	if( s == 0) return s2 == 0;
	else{
	  boost::python::object of = pattern.getAt(s-1);
	  LsysVar v = boost::python::extract<LsysVar>(of)();
	  if(!v.isArgs()) {
		if(s2 == s) { l = module.getArgs(); return true; }
		else return false;
	  }
	  if(s!=1){
		for(int i = 0; i < s-1; i++)
		  l.append(module.getAt(i));
		if(s2 > s-1)l.append(module.getslice(s-1,s2));
		else l.append(boost::python::list());
	  }
	  else l.append(module.getArgs());
	  return true;
	}
	return true;
  }
}

bool MatchingEngineImplementation::module_matching_with_star_and_valueconstraints(
										  const ParamModule& module, 
									      const ParamModule& pattern, 
									      boost::python::list& l)
{ 
  if (pattern.isStar()){
	size_t s = pattern.argSize();
	if (s == 0) return true;
	if (s == 1) {
	  size_t s2 = module.argSize();
	  boost::python::object of = pattern.getAt(0);
	  boost::python::extract<LsysVar> e(of);
	  if(!e.check()){
		  if (s2 != 1) return false;
		  if (of != module.getAt(0))return false;
		  return true;
	  }
	  else {
		LsysVar v = e();
		if(v.isArgs()) { 
			boost::python::list largs;
			largs.append(module.name());
			largs += module.getArgs();
			l.append(largs); return true; 
		}
		else {
		  if(s2 == 0){ l.append(module.name()); return true; }
		  else return false;
		}
	  }
	}
	else {
	  size_t s2 = module.argSize();
	  int beg = 0;
	  boost::python::object o1 = pattern.getAt(0);
	  boost::python::extract<LsysVar> e1(o1);
	  if(!e1.check()){
		  if (s2 < s-1) return false;
		  if (o1 != module.getAt(0))return false;
		  beg = 1;
	  }
	  else if (s2 < s-2) return false;
	  else l.append(module.name());
	  boost::python::object of = pattern.getAt(s-1);
	  bool lastarg = false;
	  boost::python::object lastargval;
	  boost::python::extract<LsysVar> ef(of);
	  if(!ef.check()){
		if(beg == 1){
		  if (s2 != s) return false;
		  if (of != module.getAt(s-1))return false;
		}
		else {
		  if (s2 != s-1) return false;
		  if (of != module.getAt(s-2))return false;
		}
	  }
	  else {
		lastarg = true;
		LsysVar v = ef();
		if(beg == 1){
		  if(!v.isArgs()){
			if (s2 != s) return false;
			lastargval = module.getAt(s-1);
		  }
		  else {
			if (s2 < s-1) return false;
			else if (s2 == s-1) lastargval = boost::python::list();
			lastargval = module.getslice(s-1,s2);
		  }
		}
		else {
		  if(!v.isArgs()){
			if (s2 != s-1) return false;
			lastargval = module.getAt(s-2);
		  }
		  else {
			if (s2 < s-2) return false;
			else {
			  if(s == 2){ l.append(module.getArgs()); return true; }
			  else if (s2 == s-2) lastargval = boost::python::list();
			  else lastargval = module.getslice(s-2,s2);
			}
		  }
		}
	  }
	  for(size_t i = 1; i < s-1; i++){
		boost::python::object oi = pattern.getAt(i);
		boost::python::extract<LsysVar> e(oi);
		if(!e.check()){
		  if (oi != module.getAt(i-1+beg))return false;
		}
		else l.append(module.getAt(i-1+beg));
	  }
	  if(lastarg)l.append(lastargval);
	  return true;
	}
  }
  else if(module.isStar())return module_matching_with_star_and_valueconstraints(pattern,module,l);
  else {
	if (!module.sameName(pattern)) return false;
	int s = pattern.argSize();
    int s2 = module.argSize();
	if( s == 0) return s2 == 0;
	else {
	  bool lastarg = false;
	  boost::python::object lastargval;
	  boost::python::object of = pattern.getAt(s-1);
	  boost::python::extract<LsysVar> ef(of);
	  if(!ef.check()){
		if (s2 != s) return false;
		if (of != module.getAt(s-1))return false;
	  }
	  else {
		lastarg = true;
		LsysVar v = ef();
		if(!v.isArgs()){
		  if (s2 != s) return false; 
		  else lastargval = module.getAt(s-1);
		}
		else {
		  if (s2 < s - 1) return false;
		  if(s == 1){ l.append(module.getArgs());return true; }
		  else if (s2 == s - 1) lastargval = boost::python::list();
		  else lastargval = module.getslice(s-1,s2);
		}
	  }
	  for(size_t i = 0; i < s-1; i++){
		boost::python::object oi = pattern.getAt(i);
		boost::python::extract<LsysVar> e(oi);
		if(!e.check()){
		  if (oi != module.getAt(i))return false;
		}
		else l.append(module.getAt(i));
	  }
	  if(lastarg)l.append(lastargval);
	}
	return true;
  }
}

/*---------------------------------------------------------------------------*/


bool MatchingEngineImplementation::string_exact_match(AxialTree::const_iterator  matching_start,
						   AxialTree::const_iterator  string_end,
						   AxialTree::const_iterator  pattern_begin,
						   AxialTree::const_iterator  pattern_end,
						   AxialTree::const_iterator& matching_end,
						   boost::python::list& params) 
{
	AxialTree::const_iterator it = matching_start;
	boost::python::list lp;
	for (AxialTree::const_iterator it2 = pattern_begin; it2 != pattern_end; ++it2){
		if( it == string_end || !it->match(*it2,lp))return false;
		else { ++it; }
	}
	params += lp;
	matching_end = it;
	return true; 	
}

bool MatchingEngineImplementation::string_exact_reverse_match(AxialTree::const_iterator matching_start,
								   AxialTree::const_iterator  string_begin,
								   AxialTree::const_reverse_iterator  pattern_rbegin,
								   AxialTree::const_reverse_iterator  pattern_rend,
								   AxialTree::const_iterator& matching_end,
								   boost::python::list& params)
{ 
	AxialTree::const_iterator it = matching_start;
	boost::python::list lp;
	for (AxialTree::const_reverse_iterator it2 = pattern_rbegin; it2 != pattern_rend; ){
		boost::python::list lmp;
		if(!it->match(*it2,lmp)) return false; 
		else { 
			++it2;
			if (it == string_begin){
				if (it2 != pattern_rend) return false;
			}
			else --it;
			lmp += lp; lp = lmp; 
		}
	}
	params += lp;
	matching_end = it;
	return true; 
}


bool MatchingEngineImplementation::tree_right_match(AxialTree::const_iterator  matching_start,
								 AxialTree::const_iterator  string_end,
								 AxialTree::const_iterator  pattern_begin,
								 AxialTree::const_iterator  pattern_end,
								 AxialTree::const_iterator& matching_end,
								 boost::python::list& params) 
{
	AxialTree::const_iterator it = matching_start;
	AxialTree::const_iterator it2 = pattern_begin;
	while(it != string_end && it2 != pattern_end){
		if(it->isIgnored()){ ++it; }
		if(it2->isStar()){
			boost::python::list lp;
			if(it->match(*it2,lp)){ params += lp; ++it; ++it2; }
			else return false;
		}
		else if(!it2->isBracket()){
			if(!it->isBracket()) {
				boost::python::list lp;
				if(it->match(*it2,lp)){ params += lp; ++it; ++it2; }
				else return false;
			}
			else { // it->isBracket()
				if(it->isRightBracket())return false;
				else if(it->isLeftBracket()) {
					it = endBracket(it,string_end);
					if(it != string_end)++it;
				}
			}
		}
		else { // it2->isBracket()
			if(it2->isRightBracket()){
				if(it->isRightBracket()) { ++it; ++it2; }
				else { 
					if(it2->isExactRightBracket())return false;
					else {
						// search start before it to avoid matching A[B]C with A[B[]C]C
						it = endBracket(it,string_end,true);
						if(it!=string_end)++it; 
						++it2; 
					}
				}
			}
			else { // it2->isLeftBracket()
				if(!it->isLeftBracket())return false;
				else { ++it; ++it2;	}
			}
		}
	}
	if(it2 == pattern_end){
		matching_end = it;
		return true;
	}
	else return false;
}

bool MatchingEngineImplementation::tree_left_match(AxialTree::const_iterator  matching_start,
								AxialTree::const_iterator  string_begin,
								AxialTree::const_iterator  string_end,
								AxialTree::const_reverse_iterator  pattern_rbegin,
								AxialTree::const_reverse_iterator  pattern_rend,
								AxialTree::const_iterator& matching_end,
								boost::python::list& params) 
{
	AxialTree::const_iterator it = matching_start;
	if(it == string_begin)return false;
	AxialTree::const_reverse_iterator it2 = pattern_rbegin;
	it = father(it,string_begin,string_end);
	boost::python::list lparams;
	while(it2 != pattern_rend && it != string_end){
		boost::python::list lp;
		if (! it->match(*it2,lp) ) return false;
		lparams += lp; 
		++it2; 
		if (it2 == pattern_rend) break;
		if(it2->isStar() || it2->isBracket()){ 
			++it2; --it; 
			while(it->isIgnored()) {
				if (it != string_begin) --it;
				else return false;
			}
		}
		else it = father(it,string_begin,string_end);
	}
	if((it2 == pattern_rend)){
		matching_end = it;
		params += lparams;
		return true;
	}
	else return false;
}


LPY_END_NAMESPACE

