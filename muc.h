#ifndef MUC_H
#define	MUC_H

#include "formula/aalta_formula.h"
#include <vector>

//using namespace std;

namespace aalta
{
    /*
    class MUC
    {
        public:
           static void muc( vector<aalta_formula *>& S, vector<aalta_formula *>& MUC);
    };
    */
    extern int  satCnt;
    extern int ucCnt;
    extern int ucValid;
    extern int ucInvalid;
    extern double ucSatTime;
    void muc( std::vector<aalta_formula *>& S, std::vector<aalta_formula *>& MUC, bool UCEable,std::vector<pair<int,int> > &UCLength);
    void basicMuc( std::vector<aalta_formula *>& S, std::vector<aalta_formula *>& MUC, bool UCEable,std::vector<pair<int,int> > &UCLength);
    void getAllMuc( aalta_formula *af,std::vector<aalta_formula *>& S, std::vector<vector<aalta_formula *>>& MUC, bool UCEable,std::vector<pair<int,int> > &UCLength);
    bool sat(std::vector<aalta_formula *>& S);
    aalta_formula *constructAnd(std::vector<aalta_formula *> S);
    std::vector<aalta_formula *> binaryPartition(aalta_formula *f);
    std::vector<aalta_formula *> Partition(aalta_formula *f);
    std::vector<aalta_formula *> getAndElement(aalta_formula* f);
    
}
#endif