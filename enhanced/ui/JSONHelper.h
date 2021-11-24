//
//
//  Created by Omer Gilad on 12/6/15.
//
//

#ifndef JSONHelper_h
#define JSONHelper_h

#include <string>

#include "enhanced/ui/Extras.h"

namespace Enhanced {
    
class JSONHelper {
   
public:
    
    static vector<ExtraItem> parseExtras();
    
    static vector<int> parseCrossPromotionCheck(std::string reponse);
    
private:
    
    static vector<ExtraItem> sExtras;
};
    
} // Enhanced

#endif /* JSONHelper_h */
