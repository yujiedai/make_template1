#include "../feature2_inter.h"
#include <cxxtest/TestSuite.h>
class feature2TestSuite : public CxxTest::TestSuite
{
    public:
    void testWork2(void)
    {
        int i;
        feature2 f2;
        
        i = f2.feature2_work2();

        TS_ASSERT(i);
    }
};
