#include "formula/aalta_formula.h"
#include "ltlfchecker.h"
#include "carchecker.h"
#include "solver.h"
#include "muc.h"
#include "msolver.h"
#include <stdio.h>
#include <string.h>
#include<ctime>


#define MAXN 100000
clock_t startTime,endTime;
char in[MAXN];
string method="binary";
bool UCEable=false;
bool check=false;

using namespace aalta;

void check_muc(vector<aalta_formula*> MUC){
	if(!sat(MUC)){
		int cnt = MUC.size();
		if(cnt == 1){
			cout<<"MUC is right"<<endl;
			return;
		}
		bool flag = false;
		while(cnt--){
			aalta_formula* af=MUC.back();
			//cout<<af->to_string()<<endl;
			MUC.pop_back();
			//cout<<MUC.size()<<endl;
			if(!sat(MUC)){
				flag = true;
				break;
			}
			else{
				MUC.insert(MUC.begin(),af);
			}
		}
		if(flag){
			cout<<"MUC is wrong"<<endl;
		}
		else{
			cout<<"MUC is right"<<endl;
		}
	}
	else{
		cout<<"MUC is wrong"<<endl;
	}
}

void get_muc(aalta_formula* af){
  	if(af->is_global () && method=="global"){
		cout<<"global"<<endl;
		MUCSolver m (af);
	  	vector<aalta_formula*> res = m.muc ();
		if(check){
				check_muc(res);
		  }
		cout<<"MUC:"<<endl;
		for(int i=0;i<res.size();i++){
			cout<<res[i]->to_string()<<endl;
		}
	}
	else{
		vector<aalta_formula*> S,MUC;
		vector<vector<aalta_formula*>> AllMUC;
		vector<pair<int,int> >UCLength;
		S.push_back(af);
		if(method=="naive"){
			basicMuc(S,MUC,UCEable,UCLength);
		}
		else if(method=="binary"||method=="global"){
			//getAllMuc(af,S, AllMUC, UCEable,UCLength);
			muc(S, MUC, UCEable,UCLength);
		}
		if(check){
			check_muc(MUC);
		}
		// cout<<"SAT:"<<satCnt<<endl;
		// cout<<"UC:"<<ucCnt<<endl;
		// cout<<"UCValid:"<<ucValid<<endl;
		// cout<<"UCInvalid:"<<ucInvalid<<endl;
		// cout<<"UCSatTime:"<<ucSatTime<<"ms"<<endl;
		cout<<"MUC:"<<endl;
		for(int i=0;i<MUC.size();i++){
			cout<<MUC[i]->to_string()<<endl;
		}
		// cout<<"LengthComp:"<<endl;
        // for(int i=0;i<UCLength.size();i++){
        //     cout<<"("<<UCLength[i].first<<","<<UCLength[i].second<<")"<<endl;
        // }
		// cout<<"AllMUC:"<<endl;
		// for(int i=0;i<AllMUC.size();i++){
		// 	cout<<i+1<<endl;
		// 	for(int j=0;j<AllMUC[i].size();j++){
		// 		cout<<AllMUC[i][j]->to_string()<<endl;
		// 	}
		// }
	}
    }




void 
print_help ()
{
	cout << "usage: ./aalta_f [-e|-v|-blsc|-t|-h|-c|-naive|-naive-uc|-binary|-binary-uc|-global|] [\"formula\"]" << endl;
	cout << "\t -e\t:\t print example when result is SAT" << endl;
	cout << "\t -v\t:\t print verbose details" << endl;
	cout << "\t -blsc\t:\t use the BLSC checking method; Default is CDLSC" << endl;
	cout << "\t -t\t:\t print weak until formula" << endl;
	cout << "\t -h\t:\t print help information" << endl;
	cout << "\t -c\t:\t open the Correctness Check" << endl;
	cout << "\t -naive\t:\t use the NAIVE approach to obtain MUC" << endl;
	cout << "\t -naive-uc\t:\t use the NAIVE+UC approach to obtain MUC" << endl;
	cout << "\t -binary\t:\t use the BINARY approach to obtain MUC" << endl;
	cout << "\t -binary-uc\t:\t use the BINARY+UC approach to obtain MUC" << endl;
	cout << "\t -global\t:\t use the GLOBAL approach to obtain MUC" << endl;
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
		else if(strcmp (argv[i-1], "-naive") == 0){
			method="naive";
		}
		else if(strcmp (argv[i-1], "-naive-uc") == 0){
			method="naive";
			UCEable=true;
		}
		else if(strcmp (argv[i-1], "-binary") == 0){
			method="binary";
		}
		else if(strcmp (argv[i-1], "-binary-uc") == 0){
			method="binary";
			UCEable=true;
		}
		else if(strcmp (argv[i-1], "-global") == 0){
			method="global";
		}
		else if(strcmp (argv[i-1], "-c") == 0){
			check=true;
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
startTime = clock();
CARChecker checker (af2, verbose, evidence);
bool res = checker.check ();
assert (!res);
get_muc(af);


  aalta_formula::destroy();
  //system ("rm -f  cnf.dimacs*");
	endTime = clock();
//cout << "The run time is:" <<(int)((double)(endTime - startTime) / CLOCKS_PER_SEC *1000)/1000.0 << "s" << endl;
cout << "The run time is:" <<(int)((double)(endTime - startTime) /(CLOCKS_PER_SEC /1000.0))<< "ms" << endl;
}


int
main (int argc, char** argv)
{
  ltlf_sat (argc, argv);
  return 0;
  

}