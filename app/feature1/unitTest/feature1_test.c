/* Code: */
//#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>
#include "../feature1_inter.h"

/**************************************************************************
函数名称：测试string_lenth()方法
功能描述：
输入参数：
返   回：
**************************************************************************/
void test_string_lenth(void){
    string test = "Hello";
    int len = string_lenth(test);
    CU_ASSERT_EQUAL(len,5);
}

/**************************************************************************
函数名称：测试方法to_Upper()
功能描述：
输入参数：
返   回：
**************************************************************************/

void test_to_Upper(void){
    char test[] = "Hello";
    CU_ASSERT_STRING_EQUAL(to_Upper(test),"HELLO");
    
}

/**************************************************************************
函数名称：测试方法 add_str()
功能描述：
输入参数：
返   回：
**************************************************************************/
void test_add_str(void){
    char test1[] = "Hello!";
    char test2[] = "MGC";
    CU_ASSERT_STRING_EQUAL(add_str(test1,test2),"Hello!MGC");
    
}


/**************************************************************************
函数名称：suite初始化过程
功能描述：
输入参数：
返   回：
**************************************************************************/
int suite_success_init(void){
    return 0;
    
}

/**************************************************************************
函数名称：suite清理过程，以便恢复原状，使结果不影响到下次运行
功能描述：
输入参数：
返   回：
**************************************************************************/
int suite_success_clean(void){
    return 0;
}



