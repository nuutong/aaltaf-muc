#include "formula/aalta_formula.h"
#include "ltlfchecker.h"
#include "carchecker.h"
#include "solver.h"
#include "muc.h"
#include <stdio.h>
#include <string.h>


#define MAXN 100000
char in[MAXN];

using namespace aalta;

void get_muc(aalta_formula* af){
  if(af->is_global ()){
	  cout<<"global"<<endl;
	  //af = af->xnf ();
	  //MUCsolver m (af);
	  //vector<aalta_formula*> res = m->muc ();
     //pirnt res
	  /*TO BE DONE
        af->ofg();
        return MUSER2(af);
		*/
    }
	else{
		vector<aalta_formula*> S,MUC;
		S.push_back(af);
		muc(S, MUC, false);
		cout<<"MUC:"<<endl;
		for(int i=0;i<MUC.size();i++){
			cout<<MUC[i]->to_string()<<endl;
		}
	}
}

void 
print_help ()
{
	cout << "usage: ./aalta_f [-e|-v|-blsc|-t|-h] [\"formula\"]" << endl;
	cout << "\t -e\t:\t print example when result is SAT" << endl;
	cout << "\t -v\t:\t print verbose details" << endl;
	cout << "\t -blsc\t:\t use the BLSC checking method; Default is CDLSC" << endl;
	cout << "\t -t\t:\t print weak until formula" << endl;
	cout << "\t -h\t:\t print help information" << endl;
}

void
ltlf_sat (int argc, char** argv)
{
	bool verbose = false;
	bool evidence = false;
	int input_count = 0;
	bool blsc = false;
	bool print_weak_until_free = false;
	
	for (int i = argc; i > 1; i --)
	{
		if (strcmp (argv[i-1], "-v") == 0)
			verbose = true;
		else if (strcmp (argv[i-1], "-e") == 0)
			evidence = true;
		else if (strcmp (argv[i-1], "-blsc") == 0)
			blsc = true;
		else if (strcmp (argv[i-1], "-t") == 0)
			print_weak_until_free = true;
		else if (strcmp (argv[i-1], "-h") == 0)
		{
			print_help ();
			exit (0);
		}
		else //for input
		{
			if (input_count > 1)
			{
				printf ("Error: read more than one input!\n");
        		exit (0);
			}
			strcpy (in, argv[i-1]);
			input_count ++;
		}
	}
	if (input_count == 0)
	{
		puts ("please input the formula:");
    	if (fgets (in, MAXN, stdin) == NULL)
    	{
        	printf ("Error: read input!\n");
        	exit (0);
      	}
	}


  aalta_formula* af;
  //set tail id to be 1
  af = aalta_formula::TAIL ();  
  af = aalta_formula(in, true).unique();
	if (print_weak_until_free)
	{
		cout << af->to_string () << endl;
		return;
	}
  
  aalta_formula *af2 = af->nnf ();
 af2 = af2->add_tail ();
  af2 = af2->remove_wnext ();
  af2 = af2->simplify ();
  af2 = af2->split_next ();
  
 
  
  
  //LTLfChecker checker (af, verbose);
  // A  t (af, verbose);
  /*
  LTLfChecker *checker;
  if (!blsc)
  	checker = new CARChecker (af, verbose, evidence);
  else
  	checker = new LTLfChecker (af, verbose, evidence);

  bool res = checker->check ();
  printf ("%s\n", res ? "sat" : "unsat");
  if (evidence && res)
  	checker->print_evidence ();
  delete checker;
  */

/*
  if (blsc)
  {
	  LTLfChecker checker (af, verbose, evidence);
	  bool res = checker.check ();
	  printf ("%s\n", res ? "sat" : "unsat");
	  if (evidence && res)
		  checker.print_evidence ();
  }
  else
  {
	CARChecker checker (af, verbose, evidence);
	bool res = checker.check ();
	printf ("%s\n", res ? "sat" : "unsat");
	if (evidence && res)
		checker.print_evidence ();
  }
  */

CARChecker checker (af2, verbose, evidence);
bool res = checker.check ();
assert (!res);
get_muc(af);


  aalta_formula::destroy();
  system ("rm -f  cnf.dimacs*");
}


int
main (int argc, char** argv)
{
  ltlf_sat (argc, argv);
  return 0;
  

}
