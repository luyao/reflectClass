/***************************************************************************
 * 
 * Copyright (c) 2014 izptec.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file test/reflect.cpp
 * @author YAO LU(luyao@izptec.com)
 * @date 2014/03/07 20:41:36
 * @version 1.0 
 * @brief 
 *  
 **/

#include <iostream>

#include "class_define.h"
#include "class_factory.h"

using namespace std;

int main()
{
	PersonIf *p = (PersonIf*)ClassFactory::GetClass("Student");
	if (!p) {
		cout<<"Get Nothing"<<endl;
	}
	cout<<p->GetClassName()<<endl;
	return 0;
}

















/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
