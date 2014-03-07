/***************************************************************************
 * 
 * Copyright (c) 2013 izptec.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/

/**
 * @author YAO LU(luyao@izptec.com)
 * @date 2013/02/04 17:27:17
 * @version 1.0 
 * @brief 实现配置处理逻辑
 * NOTICE that If equal to interface, so the class name with If
 * means this class is used as base class
 *  
 **/

#ifndef  __INCLUDE_ASN_AGENT_H_
#define  __INCLUDE_ASN_AGENT_H_

#include <list>
#include <vector>
#include <iostream>
#include <tr1/unordered_map>


#include "asn_reflect_regist.h"
#include "asn_utility.h"
#include "asn_data.h"

typedef std::tr1::unordered_map<int, sequence_index_value *>                 tMapAdAttr;
typedef std::tr1::unordered_map<int, sequence_index_value *>::iterator       tItMapAdAttr;
typedef std::tr1::unordered_map<int, sequence_index_value *>::const_iterator tCItMapAdAttr;

static const int TMP_BUF_LEN = 128;
struct asn_context
{
	//一次检索的环境变量们，包括请求、返回
	//以及一些检索必须的数据集合
	char					s_buf[TMP_BUF_LEN];		//temp region
    class asn_req_type		*p_request;		//request struct
	class asn_res_type		*p_response;	//response struct
	class WebIndex			*p_index;		//index for web
	IdsType					ids_final;		//ad bit set, final
	IdsType					ids;			//this round
	tMapAdAttr				map_ad_attr;	//ads' attribute
	std::string				s_key;			//field key
	inverted_index_value	value1;		//temp value1 buf
    sequence_index_value	*value2_buf;	//temp value2 buf
	uint32_t				value2_size;	//temp value2 size

	int init(){
		this->value2_buf = new (std::nothrow) sequence_index_value[g_conf.adnum_in_process];
		if (this->value2_buf == NULL){
			FATAL("new value2_buf failed");
			return -1;
		}
		return 0;
	}

	void destroy(){
		if (this->value2_buf != NULL){
			delete [] this->value2_buf;
		}
	}

	void clear(){
		this->s_buf[0] = '\0';
		this->ids.reset();
		this->map_ad_attr.clear();
		this->s_key.clear();
		this->value2_size = 0;
	}
};

typedef int (*pFunciton)(asn_context &);

///<this class is used as the base class of agent
class AgentIf 
{
public:
	AgentIf():_bIsSetField(false),_bIsSetFieldKey(false),_bIsSetWeight(false){}
	virtual ~ AgentIf(){}

	void SetField(const std::string &sFieldName){
		_sField = sFieldName;
		_bIsSetField = true;
		SetFieldKey();
	}
	std::string GetField() const{ return _sField; }
	
	void SetFieldKey(){
		if (get_str_prefix(_sField, g_data().ifield_prefix_map, _sFieldKey)){
			WARN("get key by field fail, field: %s", _sField.c_str());
		}
		_bIsSetFieldKey = true;
	}
	std::string GetFieldKey() const{ return _sFieldKey; }

	void SetWeight(double weight){
		_dWeight = weight;
		_bIsSetWeight = true;
	}
	double GetWeight() const { return _dWeight; }
	
	void SetClassName(const std::string &sClassName){
		_sClassName = sClassName;
	}
	std::string GetClassName() const {return _sClassName;}

	virtual int PreProcess(asn_context &context){return 0;}
	virtual int Process(asn_context &context){return 0;}
	virtual int PostProcess(asn_context &context){return 0;}

protected:
	int             _FilterTemplateMethod(const std::string &);
	std::string     _sField;
	std::string     _sFieldKey;
	double          _dWeight;
	bool			_bIsSetField;
	bool			_bIsSetFieldKey;
	bool			_bIsSetWeight;
	std::string		_sClassName;
};

//extern char RegionFilterArgv[]; 
//class RegionFilter : public Register<RegionFilter,RegionFilterArgv> , public AgentIf{
DEFINE_CLASS_EX(RegionFilter, AgentIf){
public:
	RegionFilter(){}
	virtual int Process(asn_context &context);
};

DEFINE_CLASS_EX(AdSizeFilter, AgentIf){
public:
	AdSizeFilter(){}
	virtual int Process(asn_context &context);
};

DEFINE_CLASS_EX(AdTypeFilter, AgentIf){
public:
	AdTypeFilter(){}
	virtual int Process(asn_context &context);
};

DEFINE_CLASS_EX(AdStatus, AgentIf){
public:
	AdStatus(){}
	virtual int Process(asn_context &context);
};

DEFINE_CLASS_EX(AdEnabled, AgentIf){
public:
	AdEnabled(){}
	virtual int Process(asn_context &context);
};

DEFINE_CLASS_EX(AdBusinesstype, AgentIf){
public:
	AdBusinesstype(){}
	virtual int Process(asn_context &context);
};

DEFINE_CLASS_EX(AdDsplist, AgentIf){
public:
	AdDsplist(){}
	virtual int Process(asn_context &context);
};

DEFINE_CLASS_EX(AdPosType, AgentIf){
public:
	AdPosType(){}
	virtual int Process(asn_context &context);
};

DEFINE_CLASS_EX(AdTimeRange, AgentIf){
public:
	AdTimeRange(){}
	virtual int Process(asn_context &context);
};

DEFINE_CLASS_EX(AdCurrentDate, AgentIf){
public:
	AdCurrentDate(){}
	virtual int Process(asn_context &context);
};

DEFINE_CLASS_EX(AdWhiteAd, AgentIf){
public:
	AdWhiteAd(){}
	virtual int Process(asn_context &context);
};

DEFINE_CLASS_EX(AdWhiteUrl, AgentIf){
public:
	AdWhiteUrl(){}
	virtual int Process(asn_context &context);
};

DEFINE_CLASS_EX(AdBlackTargetUrl, AgentIf){
public:
	AdBlackTargetUrl(){}
	virtual int Process(asn_context &context);
};

DEFINE_CLASS_EX(AdBlackVisitUrl, AgentIf){
public:
	AdBlackVisitUrl(){}
	virtual int Process(asn_context &context);
};

DEFINE_CLASS_EX(AdBlackCate, AgentIf){
public:
	AdBlackCate(){}
	virtual int Process(asn_context &context);
};


DEFINE_CLASS_EX(Policy, AgentIf){
public:
	Policy();
	virtual int Process(asn_context &context);
};

DEFINE_CLASS_EX(Term, AgentIf){
public:
	Term();
	virtual int Process(asn_context &context);
};

DEFINE_CLASS_EX(UserCate, AgentIf){
public:
	UserCate();
	virtual int Process(asn_context &context);
};

DEFINE_CLASS_EX(PageCate, AgentIf){
public:
	PageCate();
	virtual int Process(asn_context &context);
};

DEFINE_CLASS_EX(InterestWords, AgentIf){
public:
	InterestWords();
	virtual int Process(asn_context &context);
};

DEFINE_CLASS_EX(Sorter, AgentIf){
public:
	Sorter(){}
	virtual int Process(asn_context &context);
};

class AgentCtrl {
public:
	AgentCtrl();
	~AgentCtrl();
	
	int InitAgentFlow(const std::string &flowKey);
	int Process(asn_context &context);
private:
	class AgentCtrlImpl *_pHandle;     /**< 实现句柄       */
};

DEFINE_CLASS_EX(SimpleFilter, AgentCtrl){
	//Using the declaration to access reflect
	//the default funtions satisfy the needs
public:
	SimpleFilter(){}
};

DEFINE_CLASS_EX(ComplexFilter, AgentCtrl){
	//Using the declaration to access reflect
	//the default funtions satisfy the needs
public:
	ComplexFilter(){}
};


DEFINE_CLASS_EX(TargetFlow, AgentCtrl){
	//Using the declaration to access reflect
	//the default funtions satisfy the needs
public:
	TargetFlow(){}
};

DEFINE_CLASS_EX(SorterFlow, AgentCtrl){
	//Using the declaration to access reflect
	//the default funtions satisfy the needs
public:
	SorterFlow(){}
};


class WorkFlowCtrl{
public:
	WorkFlowCtrl();

	~WorkFlowCtrl();

	int InitWorkFlow();

	/**
	 * @brief 总体处理整个调度过程
	 *
	 * @param [in/out] context   : asn_context&
	 * @return  int 
	 * @retval  0 means sucess 
	 * @author luyao
	 * @date 2013/06/21 09:33:22
	**/
	int Process(asn_context &context);
private:
	class WorkFlowCtrlImpl *_pHandle;		  /**< 实现句柄 */
};


#endif							

/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
