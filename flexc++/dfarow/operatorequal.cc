#include "dfarow.ih"

bool DFARow::operator==(DFARow const &rhs) const
{
    return d_finalRule == rhs.d_finalRule && sameTransits(d_map, rhs.d_map);

//        &&
//        d_map.size() == rhs.d_map.size()
//        &&
//        equal(d_map.begin(), d_map.end(), rhs.d_map.begin(), 
//                                                        equal_to<Pair>());

}
