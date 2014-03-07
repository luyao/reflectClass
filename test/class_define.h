/***************************************************************************
 * 
 * Copyright (c) 2014 izptec.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file test/class_define.h
 * @author YAO LU(luyao@izptec.com)
 * @date 2014/03/07 22:23:57
 * @version 1.0 
 * @brief 
 *  
 **/


#ifndef  __TEST_CLASS_DEFINE_H_
#define  __TEST_CLASS_DEFINE_H_

#include "class_factory.h"
#include <iostream>
using namespace std;

//if we want to declear an interface class, we'd better define it derictly.
//But if we do that, we can using method GetClassName through the base class
//pointer anymore.
DEFINE_INTERFACE(PersonIf){
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

DEFINE_CLASS_EX(Student, PersonIf){
public:
    Student(){
		cout<<"Init Student"<<endl;
	}
	virtual void eat() const;
	virtual void talk()const;
};


















#endif  //__TEST/CLASS_DEFINE_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
