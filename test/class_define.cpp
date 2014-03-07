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

DECLEAR_CLASS(Student);

void Student::eat()const{
	cout<<"Student eat"<<endl;
}

void Student::talk()const{
	using namespace std;
	cout<<"Student talk"<<endl;
}



















/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
