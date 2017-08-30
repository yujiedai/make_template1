#include <stdio.h>
#include "feature1.h"
#include "feature1_inter.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
//#include <stdio.h>
//#include "strformat.h"


/**************************************************************************
函数名称：字符串相加
功能描述：
输入参数：
返   回：
**************************************************************************/
string add_str(string word1 ,string word2){
    return (strcat(word1, word2));
}

/**************************************************************************
函数名称：将字符串转换成大写格式
功能描述：
输入参数：
返   回：
**************************************************************************/
string to_Upper(string word){
    int i;
    for(i = 0;word[i] !='\0' ;i++){
        if(word[i]<'z' && word[i]>'a'){
            word[i] -= 32;
        }
    }
    return word;
    
}

/**************************************************************************
函数名称：字符串长度
功能描述：
输入参数：
返   回：
**************************************************************************/
int string_lenth(string word){
    int i;
    for(i = 0 ;word[i] != '\0';i++){
        
    }
    return i;
}

/* strformat.c ends here */

void feature1_work()
{
    printf("feature1 work...\n");
}



