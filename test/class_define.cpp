/***************************************************************************
 * 
 * Copyright (c) 2014 izptec.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file test/class_define.cpp
 * @author YAO LU(luyao@izptec.com)
 * @date 2014/03/07 22:24:49
 * @version 1.0 
 * @brief 
 *  
 **/

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



















/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
