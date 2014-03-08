/** 
 * @file class_define.h
 * @Synopsis  
 * @author Yao lu
 * @version 1.0
 * @date 2014-03-08
 */
#ifndef  __TEST_CLASS_DEFINE_H_
#define  __TEST_CLASS_DEFINE_H_

#include "class_factory.h"
#include <iostream>
using namespace std;

//if we want to declear an interface class, we'd better define it derictly.
//But if we do that, we can using method GetClassName through the base class
//pointer anymore.
DECLEAR_INTERFACE(PersonIf){
public:
    virtual void eat()const = 0;
    virtual void talk()const = 0;
    virtual ~PersonIf(){}
protected:
    PersonIf(){
        cout<<"Init Person"<<endl;
    }
    PersonIf& operator=(const PersonIf&);
    PersonIf(const PersonIf&);
};

DECLEAR_CLASS_EX(Student, PersonIf){
public:
    Student(){
        cout<<"Init Student"<<endl;
    }
    virtual void eat() const;
    virtual void talk()const;
};

DECLEAR_SINGLETON_CLASS_EX(StudentSingleton, PersonIf){
public:
    StudentSingleton(){
        cout<<"Init StudentSingleton"<<endl;
    }
    virtual void eat() const;
    virtual void talk()const;
};







#endif  //__TEST_CLASS_DEFINE_H_

/* vim: set expandtab ts=8 sw=4 sts=4 tw=100 noet: */
