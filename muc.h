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
   
    void muc( std::vector<aalta_formula *>& S, std::vector<aalta_formula *>& MUC, bool UCEable);

    bool sat(std::vector<aalta_formula *>& S);
    aalta_formula *constructAnd(std::vector<aalta_formula *> S);
    std::vector<aalta_formula *> binaryPartition(aalta_formula *f);
    std::vector<aalta_formula *> getAndElement(aalta_formula* f);
    
}
#endif