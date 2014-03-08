/** 
 * @file class_define.cpp
 * @Synopsis  
 * @author Yao lu
 * @version 1.0
 * @date 2014-03-08
 */

#include <iostream>

#include "class_define.h"

DEFINE_CLASS(Student);

void Student::eat()const{
    cout<<"Student eat"<<endl;
}

void Student::talk()const{
    using namespace std;
    cout<<"Student talk"<<endl;
}

DEFINE_CLASS(StudentSingleton);

void StudentSingleton::eat()const{
    cout<<"StudentSingleton eat"<<endl;
}

void StudentSingleton::talk()const{
    using namespace std;
    cout<<"StudentSingleton talk"<<endl;
}







/* vim: set expandtab ts=8 sw=4 sts=4 tw=100 noet: */
