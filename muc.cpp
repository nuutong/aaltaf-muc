#include "formula/aalta_formula.h"
#include "carchecker.h"
#include "solver.h"
#include "muc.h"
#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <assert.h>

using namespace std;
using namespace aalta;

namespace aalta{

bool sat(vector<aalta_formula *>& S)
{
    aalta_formula *af = constructAnd(S); //construct CNF

    af = af->nnf ();
    af = af->add_tail ();
    af = af->remove_wnext ();
    //af = af->simplify ();
    af = af->split_next ();

    // cout<<newf->to_string()<<endl;
    bool verbose = false, evidence = false;
    CARChecker checker(af, verbose, evidence);
    bool res = checker.check();
    return res;
}

aalta_formula *constructAnd(vector<aalta_formula *> S)
{
    assert(!S.empty());
    aalta_formula *res = *(S.begin());
    for (auto it = S.begin(); it != S.end(); ++it)
    {
        if (it == S.begin())
            continue;
        else
            res = aalta_formula(aalta::aalta_formula::And, res, *it).unique();
    }
    return res;
}

vector<aalta_formula *> getAndElement(aalta_formula* f)
{
    vector<aalta_formula *> result;
    aalta_formula::af_prt_set P = f->to_set ();
    for (auto it = P.begin (); it != P.end (); ++it)
        result.push_back (*it);
    return result;
}

vector<aalta_formula *> binaryPartition(aalta_formula *f)
{
    vector<aalta_formula *> and_items = getAndElement(f);
    vector<aalta_formula *> result;
    vector<aalta_formula *> temp;
    aalta_formula *subFormula;
    int l = and_items.size();
    if (l > 1)
    {
        temp.insert(temp.begin(),and_items.begin(), and_items.begin() + l / 2 );
        subFormula = constructAnd(temp);
        temp.clear();
        result.push_back(subFormula);
        temp.insert(temp.begin(),and_items.begin()+l/2, and_items.end());
        subFormula = constructAnd(temp);
        temp.clear();
        result.push_back(subFormula);
    }
    return result;
}

vector<aalta_formula *> getUC(aalta_formula *f)
{
}

void muc(vector<aalta_formula *> &S, vector<aalta_formula *> &MUC, bool UCEable)
{
    //vector<aalta_formula *> MUC;
    //vector<aalta_formula *> construct;
    vector<aalta_formula *> items;
    // vector<aalta_formula *>::iterator it,ij;
    if (S.empty())
    {
        return;
    }

    assert(!S.empty());
    aalta_formula *cf = S.back();
    S.pop_back();

    vector<aalta_formula *> P = binaryPartition(cf);
    if (P.empty())
    {
        MUC.push_back(cf);
    }

    bool flag = false;
    for (auto it = P.begin(); it != P.end(); ++it)
    {
        items.clear();
        items.push_back(*it);
        items.insert(items.end(), MUC.begin(), MUC.end());
        items.insert(items.end(), S.begin(), S.end()); //S contains not determined parts, each element of S includes a MUC element
        if (!sat(items))
        {
            flag = true;
            // cout<<(*it)->to_string()<<endl;
            //P.clear();
            vector<aalta_formula *> tmpP = binaryPartition(*it);
            if (tmpP.empty())
            {
                MUC.push_back(*it);
            }
            else
            {
                if (UCEable)
                {
                    /* TO BE DONE
                vector<aalta_formula*> UC;
                UC=get_uc(*it);
                items.clear();
                items.insert(items.end(),UC.begin(),UC.end());
                 items.insert(items.end(),S.begin(),S.end());
                 items.insert(items.end(),MUC.begin(),MUC.end());
               
                 if(!sat(items)){
                    S.push_back(construct_and(UC));
                 }
                else{
                    S.push_back(*it);
                }
                */
                }
                else
                {
                    S.push_back(*it);
                }
            }
            muc(S, MUC, UCEable);
            break;
        }
    }
    //if both parts of P contain an MUC element
    if (!flag)
    {  
        for (auto  it = P.begin(); it != P.end(); ++it)
        {
            vector<aalta_formula *> tmpP = binaryPartition(*it);
            if (tmpP.empty())
            {
                MUC.push_back(*it);
            }
            else
            {
                if (UCEable)
                {
                    /* TO BE DONE
                     UC=get_uc(*((it+1)%2));
                    items.clear();
                    items.insert(items.end(),UC.begin(),UC.end());
                    items.insert(items.end(),S.begin(),S.end());
                    items.push_back(*((it+1)%2));
                    if(!sat(items)){
                        S.push_back(construct_and(UC));
                    }
                    else{
                        S.push_back(*it);
                    }
                    */
                }
                else
                {
                    S.push_back(*it);
                }
            }
        //        cout<<MUC.size()<<endl;
        // cout<<S.size()<<endl;
        }
        muc(S, MUC, UCEable);
    }
}

}