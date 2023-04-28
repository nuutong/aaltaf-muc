/* 
 * File:   msolver.h
 * Author: Jianwen Li
 * Note: An interface for Boolean MUC solver
 * Created on June 1, 2021
 */
 
#ifndef MSOLVER_H
#define	MSOLVER_H

#include "formula/aalta_formula.h"
#include "aaltasolver.h"
#include "debug.h"
#include <iostream>
#include <vector>

#define TOOL "./muser2/src/tools/muser2/muser2"
#define MUCINPUT "./mucInput.cnf"
#define MUCOUTPUT "./mucOutput"

using namespace std;

namespace aalta
{
class MUCSolver {
public:
	//construct the object from an LTLf formula
	MUCSolver (aalta_formula* af);
	std::vector<aalta_formula*> muc ();
	
	~MUCSolver (){}
private:

	void create_dimacs ();//create MUC solver input
	
	
	aalta_formula *formula_;
	
	hash_map<int, aalta_formula*> id_formula_map_;
	aalta_formula *muc_;

	vector<aalta_formula *> extract_muc_from_file();
	hash_set<int> extract_muc_numbers_from_file();
	void create_partial_dimacs(aalta_formula *af, vector<vector<int>> &clauses);

};

class UCSolver: public AaltaSolver 
{
public:
	UCSolver () : false_detected_(false) {};
	UCSolver (std::vector<aalta_formula *> afs, int assumption_idx, bool verbose = false);
	~UCSolver () {}
	
	std::vector<aalta_formula*> get_uc_formulas ();
	inline bool SAT() {return solve_assumption ();}
	
	inline bool false_detected () {return false_detected_;}
	//inline bool false_detected () {return false;}
	
private:
	void create_clauses_for (aalta_formula *af);
	
	hash_map<int, aalta_formula*> id_formula_map_;
	
	bool false_detected_;
 	
};

}

#endif

