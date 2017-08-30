#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include <CUnit/Basic.h>
#include <CUnit/Console.h>
#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>

extern CU_SuiteInfo suites[];

/**************************************************************************
函数名称：测试类的调用总接口
功能描述：
输入参数：
返   回：
**************************************************************************/
void AddTests(){

    if(CUE_SUCCESS != CU_register_suites(suites)){
        exit(EXIT_FAILURE);

    }
}
/*************************************************************************
*功能描述：运行测试入口
*参数列表：
*返回类型：
**************************************************************************/

int RunTest(){
        if(CU_initialize_registry()){
                fprintf(stderr, " Initialization of Test Registry failed. ");
                exit(EXIT_FAILURE);
        }else{
                AddTests();
                /**** Automated Mode *****************
                CU_set_output_filename("TestMax");
                CU_list_tests_to_file();
                CU_automated_run_tests();
                ************************************/
                
                /***** Basice Mode *************/
                CU_basic_set_mode(CU_BRM_VERBOSE);
                CU_basic_run_tests();


                /*****Console Mode ****************/
                //CU_console_run_tests();

                CU_cleanup_registry();
                
                return CU_get_error();      
        }
}

int main(int argc, char * argv[])
{
    return  RunTest();   
}

