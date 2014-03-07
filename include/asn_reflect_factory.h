/***************************************************************************
 * 
 * Copyright (c) 2012 izptec.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/

/**
 * @file 
 * @author YAO LU(luyao@izptec.com)
 * @date 2012/11/24 21:43:10
 * @version 1.0 
 * @brief 
 *  
 **/

#ifndef  __ASN_REFLECT_FACTORY_H_
#define  __ASN_REFLECT_FACTORY_H_

#include <string>

typedef void *(*CreateFuntion) (void);

class ClassFactory{
public:
	static void *GetClass(const std::string & name);
	static void RegistClass(const std::string &name, CreateFuntion method);
private:
	ClassFactory() = delete;
	ClassFactory(const ClassFactory&) = delete;
	ClassFactory& operator(const ClassFactory&) = delete;
};

#endif
/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
