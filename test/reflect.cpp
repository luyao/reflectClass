/** 
 * @file reflect.cpp
 * @Synopsis  
 * @author Yao lu
 * @version 1.0
 * @date 2014-03-08
 */

#include <iostream>

#include "class_define.h"
#include "class_factory.h"

using namespace std;

int main()
{
    PersonIf *p = (PersonIf*)ClassFactory::GetClass("Student");
    if (!p) {
        cout<<"Get Nothing"<<endl;
    }else{
        cout<<p->GetClassName()<<endl;
    }

    p = (PersonIf*)ClassFactory::GetClass("StudentSingleton");
    if (!p) {
        cout<<"Get Nothing"<<endl;
    }else{
        cout<<p->GetClassName()<<endl;
    }

    return 0;
}








/* vim: set expandtab ts=256 sw=4 sts=4 tw=100 noet: */
