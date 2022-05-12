/* 
 * File:   msolver.cpp
 * Author: Jianwen Li
 * Note: An interface for MUC solver
 * Created on June 1, 2021
 */

#include "msolver.h"
#include <iostream>
#include <assert.h>
#include <fstream>
#include<sstream>
#include "utility.h"

using namespace std;
clock_t stTime,enTime;
namespace aalta
{
	MUCSolver::MUCSolver(aalta_formula *af)
	{
		formula_ = af;
		muc_ = NULL;
		create_dimacs();
	}

	vector<aalta_formula *> MUCSolver::muc()
	{
		if (muc_ != NULL){
			vector<aalta_formula *> res;
			res.push_back (muc_);
			return res;
		}
		
		int pid = getpid();
		string cmd = string(TOOL) + " -wf " + MUCOUTPUT + to_string(pid) + " " + string(MUCINPUT) + to_string(pid);// + progress id;
		stTime = clock();
		system(cmd.c_str());
		enTime = clock();
		cout << "The run time of muser2 is:" <<(int)((double)(enTime - stTime) /(CLOCKS_PER_SEC /1000.0))<< "ms" << endl;
		vector<aalta_formula *> res = extract_muc_from_file();
		//remove MUCINPUT+process id
		cmd="rm -f "+string(MUCINPUT)+to_string(pid);
		system(cmd.c_str());
		return res;
	}

	vector<aalta_formula *> MUCSolver::extract_muc_from_file()
	{
		//extract MUC from MUC Solver output file
		hash_set<int> muc = extract_muc_numbers_from_file();
		aalta_formula *af = NULL;
		vector<aalta_formula *> res;
		for (auto it = muc.begin(); it != muc.end(); ++it)
		{
			auto it2 = id_formula_map_.find (*it);
			if (it2 == id_formula_map_.end ())
				continue;
			af = it2->second;
			assert(af != NULL);
			res.push_back(af);
		}
		return res;
	}

	hash_set<int> MUCSolver::extract_muc_numbers_from_file()
	{
		//TO BE DONE
		//input: the output file from muser2, here it is fixed to MUCOUTPUT
		//output: a vector of unit clauses in the MUC
		int pid = getpid();
		ifstream file(MUCOUTPUT+to_string(pid)+".cnf");//TO BE DONE
		string line;
		stringstream ss;
		hash_set<int> V;
		int data;
		int cnt, tmp;
		if (!file.is_open())
		{
			cout << "open file failed!" << endl;
		}
		while (getline(file, line))
		{
			cnt = 0;
			if (line[0] == 'c' || line[0] == 'p')
				continue;
			ss.clear();
			ss.str(line);
			while (ss >> data)
			{
				if (data != 0)
				{
					cnt++;
					if (cnt > 1)
						break;
					tmp = data;
				}
			}
			if (cnt == 1)
			{
				V.insert(tmp);
			}
		}
		file.close();
		string cmd="rm -f "+string(MUCOUTPUT) + to_string(pid) + ".cnf";
		//cout<<cmd<<endl;
		system(cmd.c_str());
		//remove MUCOUTPUT + process id 
		return V;
	}

	void MUCSolver::create_dimacs()
	{
		aalta_formula::af_prt_set formula_set = formula_->to_set();
		int num_vars = 0;
		vector<vector<int>> clauses;
		for (auto it = formula_set.begin(); it != formula_set.end(); ++it)
		{
			aalta_formula *af = (*it)->ofg();
			
			if (af == aalta_formula::FALSE ()){
				muc_ = *it;
				return;
			}

			if (af->id() > num_vars)
				num_vars = af->id();

			id_formula_map_.insert(std::pair<int, aalta_formula *>(af->id(), *it));
			
			vector<int> cl;
			cl.push_back (af->id ());
			cl.push_back (0);
			clauses.push_back (cl);
			create_partial_dimacs(af, clauses);
		}
		//create the dimacs file MUCINPUT
		//the first line is p cnf num_vars clauses.size()
		//from the second line display each element in clauses, ending with 0
		int pid = getpid();
		ofstream infile(MUCINPUT+to_string(pid));
		string firstLine="p cnf "+to_string(num_vars)+" "+to_string(clauses.size());
		infile<<firstLine<<endl;
		int i,j;
		for(i=0;i<clauses.size();i++){
			for(j=0;j<clauses[i].size()-1;j++){
				infile<<clauses[i][j]<<' ';
			}
			infile<<clauses[i][j]<<endl;
		}
		infile.close();
	}

	void MUCSolver::create_partial_dimacs(aalta_formula *af, vector<vector<int>> &clauses)
	{
		//af is in XNF, so only consider atom, !, /\ and \/
		vector<int> cl;
		switch (af->oper())
		{
		case aalta_formula::Not: //literal, then af->id <-> -af->right->id
			cl.clear();
			cl.push_back(af->id());
			cl.push_back(af->r_af()->id());
			cl.push_back(0);
			clauses.push_back(cl);
			cl.clear();
			cl.push_back(-af->id());
			cl.push_back(-af->r_af()->id());
			cl.push_back(0);
			clauses.push_back(cl);
			break;
		case aalta_formula::And: //af->id <-> af->left->id & af->right->id
			cl.clear();
			cl.push_back(-af->id());
			cl.push_back(af->l_af()->id());
			cl.push_back(0);
			clauses.push_back(cl);
			cl.clear();
			cl.push_back(-af->id());
			cl.push_back(af->r_af()->id());
			cl.push_back(0);
			clauses.push_back(cl);
			cl.clear();
			cl.push_back(af->id());
			cl.push_back(-af->l_af()->id());
			cl.push_back(-af->r_af()->id());
			cl.push_back(0);
			clauses.push_back(cl);
			create_partial_dimacs(af->l_af(), clauses);
			create_partial_dimacs(af->r_af(), clauses);
			break;
		case aalta_formula::Or: //af->id <-> af->left->id | af->right->id
			cl.clear ();
			cl.push_back(af->id());
			cl.push_back(-af->l_af()->id());
			cl.push_back(0);
			clauses.push_back(cl);
			cl.clear();
			cl.push_back(af->id());
			cl.push_back(-af->r_af()->id());
			cl.push_back(0);
			clauses.push_back(cl);
			cl.clear();
			cl.push_back(-af->id());
			cl.push_back(af->l_af()->id());
			cl.push_back(af->r_af()->id());
			cl.push_back(0);
			clauses.push_back(cl);
			create_partial_dimacs(af->l_af(), clauses);
			create_partial_dimacs(af->r_af(), clauses);
			break;
		default: //atom af->id
			break;
		/*	cl.clear ();
			cl.push_back(af->id());
			cl.push_back(0);
			clauses.push_back(cl);*/
		}
	}

	//constructor for UC solver
	UCSolver::UCSolver(vector<aalta_formula *> afs, int assumption_idx, bool verbose) : AaltaSolver(verbose)
	{
		false_detected_ = false;
		assert(afs.size() > assumption_idx);

		aalta_formula *af = afs[assumption_idx]; //the exact UC part we need, so go into
		aalta_formula::af_prt_set formula_set = af->to_set();
		for (auto it = formula_set.begin(); it != formula_set.end(); ++it)
		{
			aalta_formula *af2 = (*it)->off();
			if (af2 == aalta_formula::FALSE ())
				false_detected_ = true;
			//set id_formula_map_ for UC extracton later
			id_formula_map_.insert(std::pair<int, aalta_formula *>(af2->id(), *it));

			assumption_.push(SAT_lit(af2->id()));
			//cout << "add assumption " << af2->id() << endl;
			//if (af2->oper () < aalta_formula::Undefined)
				create_clauses_for(af2);
		}

		for (int i = 0; i < afs.size(); ++i)
		{
			if (i != assumption_idx)
			{
				aalta_formula *af2 = afs[i];
				af2 = af2->off();
				if (af2 == aalta_formula::FALSE ())
					false_detected_ = true;
				//assumption_.push (SAT_lit (af2->id()));
				add_clause(af2->id());
				//cout << "add assumption " << af2->id() << endl;
				create_clauses_for(af2);
			}
		}
		// cout << "begin id_formula_map_" << endl;
		// for (auto it = id_formula_map_.begin(); it != id_formula_map_.end (); ++it)
		// 	cout << it->first << "  " << it->second->to_string() << endl;
		// cout << "end id_formula_map_" << endl;
		//print_clauses ();
	}

	void UCSolver::create_clauses_for(aalta_formula *af)
	{
		//af is in OFF, so only consider atom, !, /\ and \/
		switch (af->oper())
		{
		/*
		case aalta_formula::True:
			add_clause (af->id(), -af->id());
			break;
		case aalta_formula::False:
			add_clause (af->id());
			add_clause (-af->id());
			break;
			*/
		case aalta_formula::Not: //literal, then af->id <-> -af->right->id
			add_clause(af->id(), af->r_af()->id());
			add_clause(-af->id(), -af->r_af()->id());
			// cout << af->to_string () << "  ->  " << af->id () << endl;
			// cout << af->r_af()->to_string () << "  ->  " << af->r_af()->id () << endl;
			// cout << "add clause " << af->id() << " " << af->r_af()->id() << endl;
			// cout << "add clause " << -af->id() << " " << -af->r_af()->id() << endl;
			break;
		case aalta_formula::And: //af->id <-> af->left->id & af->right->id
			add_clause(-af->id(), af->l_af()->id());
			add_clause(-af->id(), af->r_af()->id());
			add_clause(af->id(), -af->l_af()->id(), -af->r_af()->id());
			// cout << af->to_string () << "  ->  " << af->id () << endl;
			// cout << af->l_af()->to_string () << "  ->  " << af->l_af()->id () << endl;
			// cout << af->r_af()->to_string () << "  ->  " << af->r_af()->id () << endl;
			// cout << "add clause " << -af->id() << " " << af->l_af()->id() <<endl;
			// cout << "add clause " << -af->id() << " " << af->r_af()->id() <<endl;
			// cout << "add clause " << af->id() << " " << -af->l_af()->id() << " " << -af->r_af()->id() <<endl;
			create_clauses_for(af->l_af());
			create_clauses_for(af->r_af());
			break;
		case aalta_formula::Or: //af->id <-> af->left->id | af->right->id
			add_clause(af->id(), -af->l_af()->id());
			add_clause(af->id(), -af->r_af()->id());
			add_clause(-af->id(), af->l_af()->id(), af->r_af()->id());
			// cout << af->to_string () << "  ->  " << af->id () << endl;
			// cout << af->l_af()->to_string () << "  ->  " << af->l_af()->id () << endl;
			// cout << af->r_af()->to_string () << "  ->  " << af->r_af()->id () << endl;
			// cout << "add clause " << af->id() << " " << -af->l_af()->id() <<endl;
			// cout << "add clause " << af->id() << " " << -af->r_af()->id() <<endl;
			// cout << "add clause " << -af->id() << " " << af->l_af()->id() << " " << af->r_af()->id() <<endl;
			create_clauses_for(af->l_af());
			create_clauses_for(af->r_af());
			break;
		default: //atom af->id
			;//add_clause(af->id());
		}
	}

	vector<aalta_formula *> UCSolver::get_uc_formulas()
	{
		vector<aalta_formula *> res;
		vector<int> uc = get_uc();
		for (auto it = uc.begin(); it != uc.end(); ++it)
		{
			auto it2 = id_formula_map_.find(*it);
			assert (it2 != id_formula_map_.end());
			res.push_back(it2->second);
		}
		return res;
	}

}
