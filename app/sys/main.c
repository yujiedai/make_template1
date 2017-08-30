#include <stdio.h>
#include "feature1.h"
#include "feature2.h"

extern const char version_string[];

int main(int argc , char ** argv)
{
    printf("app(%s) start...\n", version_string);

    feature1_work();

    feature2_work();

    return 0;
}
