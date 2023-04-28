#include "formula/aalta_formula.h"
#include "carchecker.h"
#include "solver.h"
#include "muc.h"
#include "msolver.h"
#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <assert.h>

using namespace std;
using namespace aalta;

clock_t sTime, eTime;

namespace aalta
{

    int satCnt = 0;
    int ucCnt = 0;
    int ucValid = 0;
    int ucInvalid = 0;
    double ucSatTime = 0;
    vector<aalta_formula *> temp_MUC;
    hash_set<aalta_formula *> allFormulas;

    bool sat(vector<aalta_formula *> &S)
    {
        satCnt++;
        aalta_formula *af = constructAnd(S); //construct CNF
        af = af->nnf();
        af = af->add_tail();
        af = af->remove_wnext();
        //af = af->simplify ();
        af = af->split_next();

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

    vector<aalta_formula *> getAndElement(aalta_formula *f)
    {
        vector<aalta_formula *> result;
        aalta_formula::af_prt_set P = f->to_set();
        for (auto it = P.begin(); it != P.end(); ++it)
            result.push_back(*it);
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
            temp.insert(temp.begin(), and_items.begin(), and_items.begin() + l / 2);
            subFormula = constructAnd(temp);
            temp.clear();
            result.push_back(subFormula);
            temp.insert(temp.begin(), and_items.begin() + l / 2, and_items.end());
            subFormula = constructAnd(temp);
            temp.clear();
            result.push_back(subFormula);
        }
        return result;
    }

    vector<aalta_formula *> Partition(aalta_formula *f)
    {
        vector<aalta_formula *> and_items = getAndElement(f);
        vector<aalta_formula *> result;
        vector<aalta_formula *> temp;
        aalta_formula *subFormula;
        int l = and_items.size();
        if (l > 1)
        {
            result.push_back(and_items[0]);
            temp.insert(temp.begin(), and_items.begin() + 1, and_items.end());
            subFormula = constructAnd(temp);
            result.push_back(subFormula);
        }
        return result;
    }

    void basicMuc(vector<aalta_formula *> &S, vector<aalta_formula *> &MUC, bool UCEable, vector<pair<int, int>> &UCLength)
    {
        vector<aalta_formula *> items;
        aalta_formula *cf;

        if (S.empty())
        {
            return;
        }

        assert(!S.empty());
        aalta_formula *newf = S.back();
        S.pop_back();
        if (UCEable)
        { //find an UC from the selected part of P, to check whether UC is enough to unsatisfy the whole formula
            sTime = clock();
            ucCnt++;
            items.clear();
            items.push_back(newf);
            items.insert(items.end(), MUC.begin(), MUC.end());
            items.insert(items.end(), S.begin(), S.end());
            UCSolver s(items, 0);
            if (s.false_detected())
            { //some off becomes false
                cf = newf;
                //ucInvalid++;
            }
            else
            {
                assert(!s.SAT()); //guaranteed by !sat(items)
                vector<aalta_formula *> UC = s.get_uc_formulas();
                if (UC.empty())
                { //does not work
                    cf = newf;
                    //ucInvalid++;
                }
                else
                {
                    //start to check whether UC is enough to unsatisfy the whole formula
                    items.clear();
                    items.insert(items.end(), UC.begin(), UC.end());
                    items.insert(items.end(), S.begin(), S.end());
                    items.insert(items.end(), MUC.begin(), MUC.end());
                    if (!sat(items))
                    {
                        cf = constructAnd(UC);
                        ucValid++;
                        UCLength.push_back(make_pair(items.size(), UC.size()));
                    }
                    else
                    {
                        cf = newf;
                        // bool flag=false;
                        // auto it=set_difference(S.begin(),S.end(),UC.begin(),UC.end(),S.begin());

                        // for(int i=0;i<UC.size();i++){
                        //     items.clear();
                        //     items.insert(items.end(),UC-UC[i].begin(),UC-UC[i].end());
                        //     items.insert(items.end(),S-UC.begin(),S-UC.end());
                        //     items.insert(items.end(), MUC.begin(), MUC.end());
                        //     if(sat(items)){
                        //         MUC.push_back(UC[i]);
                        //     }
                        //     else{
                        //         flag=true;
                        //         S.reserve
                        //     }
                        //     UC[i]
                        // }
                        ucInvalid++;
                    }
                }
            }
            eTime = clock();
            ucSatTime += (double)(eTime - sTime) / (CLOCKS_PER_SEC / 1000.0);
        }
        else
        {
            cf = newf;
        }
        vector<aalta_formula *> P = Partition(cf);
        if (P.empty())
        { //the formula cannot be partitioned, so it has to be an MUC element
            if (MUC.empty())
            {
                MUC.push_back(cf);
            }
            else
            {
                items.clear();
                items.insert(items.end(), MUC.begin(), MUC.end());
                if (sat(items))
                {
                    MUC.push_back(cf);
                }
            }
            basicMuc(S, MUC, UCEable, UCLength);
            return;
        }
        items.clear();
        items.push_back(P[1]);
        items.insert(items.end(), MUC.begin(), MUC.end());
        if (sat(items))
        {
            MUC.push_back(P[0]);
        }
        S.push_back(P[1]);
        basicMuc(S, MUC, UCEable, UCLength);
    }

    void getAllMuc(aalta_formula *af, vector<aalta_formula *> &S, vector<vector<aalta_formula *>> &AllMUC, bool UCEable, vector<pair<int, int>> &UCLength)
    {
        vector<aalta_formula *> clause = getAndElement(af);
        vector<aalta_formula *> MUC;
        aalta_formula *newf = af;
        bool flag = false;
        S.push_back(newf);
        temp_MUC.clear();
        muc(S, temp_MUC, UCEable, UCLength);
        MUC = temp_MUC;
        //    cout<<af->to_string()<<endl;
        //    cout<<"MUC: "<<endl;
        //    for(auto it=MUC.begin();it!=MUC.end();++it){
        //        cout<<(*it)->to_string()<<endl;
        //    }
        //    cout<<"-------------------"<<endl;
        for (int i = 0; i < AllMUC.size(); i++)
        {
            if (AllMUC[i] == MUC)
            {
                flag = true;
            }
        }
        if (!flag)
        {
            AllMUC.push_back(MUC);
        }
        vector<aalta_formula *> restClause = clause;
        for (auto it = MUC.begin(); it != MUC.end(); ++it)
        {
            restClause = clause;
            for (auto it2 = restClause.begin(); it2 != restClause.end();)
            {
                if (*it2 == *it)
                {
                    it2 = restClause.erase(it2);
                    break;
                }
                else
                {
                    ++it2;
                }
            }
            newf = constructAnd(restClause);
            if (allFormulas.find(newf) != allFormulas.end()){
                continue;
            }
            else {
                allFormulas.insert(newf);
            }
            if (sat(restClause))
            {
                continue;
            }
            else
            {
                //newf=constructAnd(restClause);
                getAllMuc(newf, S, AllMUC, UCEable, UCLength);
            }
        }
    }
    void muc(vector<aalta_formula *> &S, vector<aalta_formula *> &MUC, bool UCEable, vector<pair<int, int>> &UCLength)
    {
        /*
	S: set of formulas not determined, but there must be at least one MUC element in each formula
	MUC: set of MUC elements determined
	UCEable: flag to turn on the UC heursitics
	*/
        vector<aalta_formula *> items;
        aalta_formula *cf;

        if (S.empty())
        {
            return;
        }
        assert(!S.empty());
        aalta_formula *newf = S.back();
        S.pop_back();
        if (UCEable)
        { //find an UC from the selected part of P, to check whether UC is enough to unsatisfy the whole formula
            sTime = clock();
            ucCnt++;
            items.clear();
            items.push_back(newf);
            items.insert(items.end(), MUC.begin(), MUC.end());
            items.insert(items.end(), S.begin(), S.end());
            UCSolver s(items, 0);
            if (s.false_detected())
            { //some off becomes false
                cf = newf;
                // ucInvalid++;
            }
            else
            {
                assert(!s.SAT()); //guaranteed by !sat(items)

                vector<aalta_formula *> UC = s.get_uc_formulas();
                if (UC.empty())
                { //does not work
                    cf = newf;
                    //ucInvalid++;
                }
                else
                {
                    //start to check whether UC is enough to unsatisfy the whole formula
                    items.clear();
                    items.insert(items.end(), UC.begin(), UC.end());
                    items.insert(items.end(), S.begin(), S.end());
                    items.insert(items.end(), MUC.begin(), MUC.end());

                    if (!sat(items))
                    {
                        cf = constructAnd(UC);
                        UCLength.push_back(make_pair(items.size(), UC.size()));
                        ucValid++;
                    }
                    else
                    {
                        cf = newf;
                        ucInvalid++;
                    }
                }
            }
            eTime = clock();
            ucSatTime += (double)(eTime - sTime) / (CLOCKS_PER_SEC / 1000.0);
        }
        else
        {
            cf = newf;
            //cout<<cf->to_string()<<endl;
        }
        vector<aalta_formula *> P = binaryPartition(cf);

        if (P.empty())
        { //the formula cannot be partitioned, so it has to be an MUC element
            MUC.push_back(cf);
            muc(S, MUC, UCEable, UCLength);
            return;
        }

        bool flag = false;
        for (auto it = P.begin(); it != P.end(); ++it) //P has only two elements
        {
            items.clear();
            items.push_back(*it);
            items.insert(items.end(), MUC.begin(), MUC.end());
            items.insert(items.end(), S.begin(), S.end()); //S contains not determined parts, each element of S includes a MUC element
            if (!sat(items))
            { //the selected part of P is sufficient to unsatisfy the whole formula, so another part of P can be ignored
                flag = true;
                // cout<<(*it)->to_string()<<endl;
                //P.clear();
                vector<aalta_formula *> tmpP = binaryPartition(*it);
                if (tmpP.empty())
                { //the formula cannot be partitioned, so it has to be an MUC element
                    MUC.push_back(*it);
                }
                else
                {
                    //     if (UCEable)
                    //     {//find an UC from the selected part of P, to check whether UC is enough to unsatisfy the whole formula

                    //     	items.clear();
                    //         items.push_back(*it);
                    //         items.insert(items.end(),MUC.begin(),MUC.end());
                    //         items.insert(items.end(),S.begin(),S.end());

                    //         UCSolver s (items, 0);
                    //         if (s.false_detected ()){//some off becomes false
                    //         	S.push_back(*it);
                    //         }
                    //         else{
                    //         assert (!s.SAT()); //guaranteed by !sat(items)
                    //         ucCnt++;
                    //         vector<aalta_formula*> UC = s.get_uc_formulas ();
                    //         if (UC.empty ()){//does not work
                    //         	S.push_back (*it);
                    //         }
                    //         else {
                    //         	//start to check whether UC is enough to unsatisfy the whole formula
                    //         	items.clear();
                    //         	items.insert(items.end(),UC.begin(),UC.end());
                    //         	items.insert(items.end(),S.begin(),S.end());
                    //         	items.insert(items.end(),MUC.begin(),MUC.end());

                    //         	if(!sat(items)){
                    //         		S.push_back(constructAnd(UC));
                    //      		}
                    //     		else{
                    //         		S.push_back(*it);
                    //     		}
                    //             ucCnt++;
                    //     	}

                    //     }
                    //     }
                    //    else
                    //     {
                    S.push_back(*it);
                    //     }
                }
                muc(S, MUC, UCEable, UCLength);
                break;
            }
        }

        if (!flag)
        { //if both parts of P contain an MUC element
            for (int i = 0; i < 2; ++i)
            {
                vector<aalta_formula *> tmpP = binaryPartition(P[i]);
                if (tmpP.empty())
                { //the formula cannot be partitioned, so it has to be an MUC element
                    MUC.push_back(P[i]);
                }
                else
                {
                    //     if (UCEable)
                    //     {//find an UC from the selected part of P (below is P[i]), to check whether UC is enough to unsatisfy the whole formula

                    //         items.clear();
                    //         items.push_back(P[i]);
                    //         if (i == 0)
                    //         	items.push_back(P[(i+1)]);
                    //         items.insert(items.end(),MUC.begin(),MUC.end());
                    //         items.insert(items.end(),S.begin(),S.end());
                    //         UCSolver s (items, 0);
                    //         if (s.false_detected ()){
                    //         	S.push_back(P[i]);
                    //         }
                    //         else{
                    //         assert (!s.SAT()); //guaranteed by !sat(items)
                    //         ucCnt++;
                    //         vector<aalta_formula*> UC = s.get_uc_formulas ();
                    //         if (UC.empty ()){//does not work
                    //         	S.push_back (P[i]);
                    //         }
                    //         else{
                    //         	//start to check whether UC is enough to unsatisfy the whole formula
                    //         	items.clear();
                    //         	items.insert(items.end(),UC.begin(),UC.end());
                    //         	items.insert(items.end(),S.begin(),S.end());
                    //         	items.insert(items.end(),MUC.begin(),MUC.end());
                    //         	if (i == 0)
                    //         		items.push_back(P[(i+1)]);
                    //         	if(!sat(items)){
                    //             	S.push_back(constructAnd(UC));
                    //                 ucCnt++;
                    //         	}
                    //         	else{
                    //             	S.push_back(P[i]);
                    //                 ucCnt++;
                    //         	}
                    //         }

                    //     }
                    //     }
                    //    else
                    //    {
                    S.push_back(P[i]);
                    //    }
                }
            }
        }
        muc(S, MUC, UCEable, UCLength);
    }
}
