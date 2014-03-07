/***************************************************************************
 * 
 * Copyright (c) 2012 izptec.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/

/**
 * @file src/ads_agent_factory.cpp
 * @author YAO LU(luyao@izptec.com)
 * @date 2012/11/25 15:56:45
 * @version 1.0 
 * @brief 
 *  
 **/

#include <string>
#include <map>
#include "asn_reflect_factory.h"
#include "log.h"

std::map< std::string, CreateFuntion >& __getMap()
{
	static std::map < std::string, CreateFuntion > __map;
	return __map;
}

void *AgentFactory::GetAgent(const std::string & name)
{
	std::map < std::string, CreateFuntion >::const_iterator it_find =
		__getMap().find(name);
	if (it_find == __getMap().end()) {
		return NULL;
	} else {
		return it_find->second();
	}
}

void AgentFactory::RegistClass(const std::string & name, CreateFuntion method)
{
	__getMap().insert(std::make_pair(name, method));
}

/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
