/***************************************************************************
 * 
 * Copyright (c) 2013 izptec.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/

/**
 * @file src/asn_reflect.cpp
 * @author YAO LU(luyao@izptec.com)
 * @date 2013/02/18 14:03:22
 * @version 1.0 
 * @brief  this file is used to impliment asn workflow
 *  
 **/

#include <gflags/gflags.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>
#include <map>
#include <algorithm> //for sort
#include <tr1/unordered_set>
#include <tr1/unordered_map>
#include <boost/unordered_set.hpp>
#include <utility>

#include "asn_reflect.h"
#include "asn_define.h"
#include "asn_index.h" 

#define _ASN_DEBUG
#define _REGULAR_HOST

using std::vector;
using std::string;

extern const char *WorkFlowSectionName;
extern const char *FieldSectionName;
extern const char *WeightSectionName;
extern const char *WorkFlowFirstKeyName;

#ifdef _ASN_TIMER
class dis_timer{
	timeval s, e;
	std::string _info;
	public:
	dis_timer(const std::string &info):_info(info){start();}

	~dis_timer(){
		end();
		std::cout<<_info<<" PERFORMANCE_TIMER:"<<costms()<<"us"
				 <<std::endl;
	}

	void start() {gettimeofday(&s, NULL);}   
	void end() {gettimeofday(&e, NULL);}   

	int costms () { return (int)((e.tv_usec-s.tv_usec));}   
};
#else
class dis_timer{
public:
	dis_timer(const std::string &){}
};
#endif

#ifdef _ASN_DEBUG
#else
	void do_nothing(const char *sFormat, ...){}
	#ifdef TRACE
	#undef TRACE
	#endif
	#define TRACE(fmt, arg...)do{\
		do_nothing(fmt, ##arg);\
	}while(0);
#endif

static const char *SPLIT = "\t";

#define DECLEAR_CLASSNAME(class_name)\
	char class_name##Argv[] = #class_name

//this macro is used to definition the Bonus class
#define RIGIST_CLASS(class_name) \
	class_name::class_name(){\
		FunctionHandleProc::HandleRegist(#class_name, class_name##FunctionHandle);\
	}


DECLEAR_CLASSNAME(SimpleFilter);
DECLEAR_CLASSNAME(ComplexFilter);
DECLEAR_CLASSNAME(TargetFlow);
DECLEAR_CLASSNAME(SorterFlow);

template<typename T>
int __ProcessWhiteListLoop(const std::vector<T> &vec, const char *sFormat, asn_context &context, const std::string &sFieldKey)
{
	int ret=0;
	context.ids.reset();
	bool bSucess = false;
	inverted_index_value &value = context.value1;
	int len = sFieldKey.length();
	strncpy(context.s_buf, sFieldKey.c_str(), len);

	for (size_t i=0; i<vec.size(); ++i){
		snprintf(context.s_buf+len, TMP_BUF_LEN-len, sFormat, vec.at(i));
		ret = context.p_index->find_hash_inverted_index(context.s_buf, value);
		if (ret) {
			DEBUG("find hash_inverted_index failed, key: %s", context.s_buf);
			continue;
		} else {
			bSucess = true;
		}
		context.ids |= value.second;
	}
	if (false == bSucess) {
		return 1;
	}
	context.ids_final &= context.ids;		
	return 0;
}

DECLEAR_CLASSNAME(AdStatus);
int AdStatus::Process(asn_context &context)
{
	vector<uint32_t> vec(1, 0); 
	return __ProcessWhiteListLoop(vec, "%u", context, _sFieldKey);
}

DECLEAR_CLASSNAME(AdEnabled);
int AdEnabled::Process(asn_context &context)
{
	vector<uint32_t> vec(1, 1); 
	return __ProcessWhiteListLoop(vec, "%u", context, _sFieldKey);
}

DECLEAR_CLASSNAME(RegionFilter);
int RegionFilter::Process(asn_context &context)
{
	return __ProcessWhiteListLoop(context.p_request->region_id, "%u", context, _sFieldKey);
}

DECLEAR_CLASSNAME(AdSizeFilter);
int AdSizeFilter::Process(asn_context &context)
{
	return __ProcessWhiteListLoop(context.p_request->ad_size, "%u", context, _sFieldKey);
}

DECLEAR_CLASSNAME(AdPosType);
int AdPosType::Process(asn_context &context)
{
	return __ProcessWhiteListLoop(context.p_request->pos_type, "%u", context, _sFieldKey);
}

DECLEAR_CLASSNAME(AdTypeFilter);
int AdTypeFilter::Process(asn_context &context)
{
	return __ProcessWhiteListLoop(context.p_request->supported_stuff, "%u", context, _sFieldKey);
}

DECLEAR_CLASSNAME(AdBusinesstype);
int AdBusinesstype::Process(asn_context &context)
{
	return __ProcessWhiteListLoop(context.p_request->business_type, "%u", context, _sFieldKey);
}

DECLEAR_CLASSNAME(AdDsplist);
int AdDsplist::Process(asn_context &context)
{
	vector<uint32_t> vec(1, context.p_request->flow_type);
	return __ProcessWhiteListLoop(vec, "%u", context, _sFieldKey);
}

DECLEAR_CLASSNAME(AdTimeRange);
int AdTimeRange::Process(asn_context &context)
{
	time_t t = time(NULL);
	struct tm ptm = {0};
	localtime_r(&t, &ptm);
	vector<uint32_t> vec(1, ptm.tm_hour);
	return __ProcessWhiteListLoop(vec, "%u", context, _sFieldKey);
}


DECLEAR_CLASSNAME(AdWhiteAd);
int AdWhiteAd::Process(asn_context &context)
{
	vector<uint32_t> &rVec = context.p_request->white_ids;
	//设定了白名单才需要做处理
	if (rVec.size()) {	
		IdsType ids(0);
		for (size_t i=0; i<rVec.size(); ++i) {
			if (rVec[i] > ASN_AD_SIZE) {
				continue;
			}
			ids.set(rVec[i]);
		}
		context.ids_final &= ids;
	}

	//搞定白名单之后,构造返回结构,以及广告的精准信息
	int ret;
	size_t nCnt = 0;
	size_t nTotal = context.ids_final.count();
	size_t nSize = context.ids_final.size();
	for (size_t i=0; i<nSize&& nCnt<nTotal; ++i) {
		if (context.ids_final.test(i)) {
			sequence_index_value &value = context.value2_buf[context.value2_size++];
			if (context.value2_size >= g_conf.adnum_in_process){
				FATAL("ad num is too large, more than %u", g_conf.adnum_in_process);
				break;
			}
			ret = context.p_index->find_hash_sequence_index(i, value);
			if (ret) {	// 找不到广告详细信息则做抛弃处理
				// start: add by maohong, 当前buff 未被使用，但buff已经计数，故需要退回计数
				--context.value2_size;
				// end

				continue;
			}
			context.map_ad_attr[i] = &value;
			context.p_response->list_ads.push_back(asn_res_type::_resUnit(i));
			++nCnt;
		}
	}
	log_pushnotice("CutN", "%u", context.value2_size);
	return 0;
}

DECLEAR_CLASSNAME(AdCurrentDate);
int AdCurrentDate::Process(asn_context &context)
{
	//get unix time 
	time_t t;
	time(&t);
	//get the total suitable ads
	asn_res_type::it_asn_res_list it = context.p_response->list_ads.begin();
	for (; it != context.p_response->list_ads.end();) {
		tItMapAdAttr itAttr = context.map_ad_attr.find(it->n_adid);
		if (itAttr == context.map_ad_attr.end()) {//cant find ad attribute
			//remove this ad
			it = context.p_response->list_ads.erase(it);
			continue;
		}

		sequence_index_value &value = *(itAttr->second);
		if (t > value.second->end_date || t < value.second->beg_date) {
			it = context.p_response->list_ads.erase(it);
		} else {
			++it;
		}
	}
	return 0;
}

DECLEAR_CLASSNAME(AdWhiteUrl);
int AdWhiteUrl::Process(asn_context &context)
{
	asn_res_type::it_asn_res_list it = context.p_response->list_ads.begin();
	for (; it != context.p_response->list_ads.end();){
		tItMapAdAttr itAttr = context.map_ad_attr.find(it->n_adid);
		if (itAttr == context.map_ad_attr.end()) {//cant find ad attribute
			//remove this ad
			it = context.p_response->list_ads.erase(it);
			continue;
		}
		sequence_index_value &value = *(itAttr->second);

		string sUrl;
#ifdef _REGULAR_HOST
		sUrl = context.p_request->host;
#else
		sUrl = context.p_request->url;
#endif
		int ret = -1;
		//不设置白名单，就表示可投
		if (!value.second->hwhitelist.size()&&!value.second->vwhitelist.size()){
			++it;
			continue;
		}
		//查找hash白名单
		boost::unordered_set<std::string>::const_iterator hw_iter = value.second->hwhitelist.find(sUrl);
		if (hw_iter!=value.second->hwhitelist.end()){
			++it;
			continue;
		}
		//查找vec白名单
		for (size_t i = 0; i<value.second->vwhitelist.size();++i) {
			ret = one_star_cmp(value.second->vwhitelist.at(i), sUrl);
			if (!ret){
				break;
			}
		}
		
		if (!ret){//match!
			++it;
		} else{
			//TRACE("Remove adid:%u url:[%s] is not in the whitelist",it->n_adid, sUrl.c_str());
			it = context.p_response->list_ads.erase(it);
		}
	}
	return 0;
}

DECLEAR_CLASSNAME(AdBlackTargetUrl);
int AdBlackTargetUrl::Process(asn_context &context)
{
	vector<string> &rVec = context.p_request->black_urls;
	if (rVec.empty()) {
		return 0;
	}

	asn_res_type::it_asn_res_list it = context.p_response->list_ads.begin();
	for (; it != context.p_response->list_ads.end();) {
		tItMapAdAttr itAttr = context.map_ad_attr.find(it->n_adid);
		if (itAttr == context.map_ad_attr.end()) {//cant find ad attribute
			//remove this ad
			it = context.p_response->list_ads.erase(it);
			continue;
		}
		sequence_index_value &value = *(itAttr->second);
/****** start, modified: remove, no use
		string sUrl;
#ifdef _REGULAR_HOST
		sUrl = context.p_request->host;
#else
		sUrl = context.p_request->url;
#endif
******* end
*/
		string sTargetUrl = value.second->target_url;
		if ( (sTargetUrl.size()>7) && (std::string::npos!=sTargetUrl.find("http://")) ){
			sTargetUrl = sTargetUrl.substr(7);
		}
		int ret = -1;
		size_t i = 0;
		for (; i<rVec.size();++i) {
			string tmp = rVec[i];
			if ( (rVec[i].size()>7) && (std::string::npos!=rVec[i].find("http://")) ){
				tmp = rVec[i].substr(7);
			}
			ret = reg_host_cmp(sTargetUrl, tmp, false, false);
			if (!ret) {//target url in the black list
				break;
			}
		}

		if (!ret) {//match!
			//TRACE("Remove AdId:%u target_url:[%s] in blacklist[%s]",it->n_adid, sTargetUrl.c_str(), rVec.at(i).c_str());
			it = context.p_response->list_ads.erase(it);
		}else{
			++it;
		}
	}
	return 0;
}

DECLEAR_CLASSNAME(AdBlackVisitUrl);
int AdBlackVisitUrl::Process(asn_context &context)
{
	asn_res_type::it_asn_res_list it = context.p_response->list_ads.begin();
	for (; it != context.p_response->list_ads.end();) {
		tItMapAdAttr itAttr = context.map_ad_attr.find(it->n_adid);
		if (itAttr == context.map_ad_attr.end()) {//cant find ad attribute
			//remove this ad
			it = context.p_response->list_ads.erase(it);
			continue;
		}
		sequence_index_value &value = *(itAttr->second);

		string sUrl;
#ifdef _REGULAR_HOST
		sUrl = context.p_request->host;
#else
		sUrl = context.p_request->url;
#endif
		int ret = -1;
		//查找hash黑名单
		boost::unordered_set<std::string>::const_iterator hw_iter = value.second->hblacklist.find(sUrl);
		if (hw_iter != value.second->hblacklist.end()){
			it = context.p_response->list_ads.erase(it);
			continue;
		}
		//查找vec黑名单
		vector<string> &rVec = value.second->vblacklist;
		size_t i = 0;
		for (; i<rVec.size(); ++i) {
			ret = one_star_cmp(rVec.at(i), sUrl);
			if (!ret) {
				break;
			}
		}
		if (!ret){ //match!
			it = context.p_response->list_ads.erase(it);
			//TRACE("Remove AdId:%u visit_url:[%s] in ad blacklist[%s]",it->n_adid, sUrl.c_str(), rVec.at(i).c_str());
		} else {
			++it;
		}
	}
	return 0;
}

DECLEAR_CLASSNAME(AdBlackCate);
int AdBlackCate::Process(asn_context &context)
{
	vector<uint32_t> &rVec = context.p_request->black_cates;
	if (rVec.empty()) {
		return 0;
	}

	asn_res_type::it_asn_res_list it = context.p_response->list_ads.begin();
	for (; it != context.p_response->list_ads.end();) {
		tItMapAdAttr itAttr = context.map_ad_attr.find(it->n_adid);
		if (itAttr == context.map_ad_attr.end()) {//cant find ad attribute
			//remove this ad
			it = context.p_response->list_ads.erase(it);
			continue;
		}
		sequence_index_value &value = *(itAttr->second);

		size_t i = 0;
		int ret = -1;
		for (; i<rVec.size();++i) {
			if (value.second->cate_id == rVec.at(i)) {
				ret = 0;
				break;
			}
		}

		if (!ret) {//match the black cate
			//TRACE("Remove AdId:%u cate_id[%u] blackcate[%u]", it->n_adid, attr.cate_id, rVec.at(i));
			it = context.p_response->list_ads.erase(it);
		}else{
			++it;
		}
	}
	return 0;
}

typedef int (__FunctionHandle)(asn_context &context, AdAttr &attr, asn_res_type::it_asn_res_list it, const double dWeight);
class FunctionHandleProc{
public:
	struct _unit{
		__FunctionHandle *pHandle;
		double            dWeight;
	};
	typedef std::map<std::string, _unit> mapHandle;
	typedef std::map<std::string, _unit>::iterator itMapHandle;
	typedef std::map<std::string, _unit>::const_iterator citMapHandle;

	static int HandleRegist(const char *sName, __FunctionHandle* pHandle);
	static int WeightRegist(const char *sName, double dWeight);
	static void DebugMap();
	static __FunctionHandle* GetHandle(const char *sName);
	static double            GetWeight(const char *sName);
	static mapHandle _mapBonus;
};

void FunctionHandleProc::DebugMap()
{
	citMapHandle cit = _mapBonus.begin();
	for (;cit != _mapBonus.end();++cit) {
		std::cout<<cit->first<<":unit[handle:"<<cit->second.pHandle
			<<";weight:"<<cit->second.dWeight<<std::endl;
	}
}

FunctionHandleProc::mapHandle FunctionHandleProc::_mapBonus;

int FunctionHandleProc::WeightRegist(const char *sName, double dWeight)
{
	itMapHandle it = _mapBonus.find(sName);
	if (it != _mapBonus.end()) {
		it->second.dWeight = dWeight;
		return 0;
	}
	_unit unit;
	unit.pHandle = NULL;
	unit.dWeight = dWeight;
	_mapBonus.insert(std::make_pair(sName, unit));
	return 0;
}

int FunctionHandleProc::HandleRegist(const char *sName, __FunctionHandle* pHandle)
{
	itMapHandle it = _mapBonus.find(sName);
	if (it != _mapBonus.end()) {
		it->second.pHandle = pHandle;
		return 0;
	}
	_unit unit;
	unit.pHandle = pHandle;
	unit.dWeight = 1.0;
	_mapBonus.insert( std::make_pair(sName, unit) );
	return 0;
}

__FunctionHandle* FunctionHandleProc::GetHandle(const char *sName)
{
	assert(sName);
	citMapHandle cit = _mapBonus.find(sName);
	if (cit != _mapBonus.end()) {
		return cit->second.pHandle;
	}
	return NULL;
}

double FunctionHandleProc::GetWeight(const char *sName)
{
	assert(sName);
	citMapHandle cit = _mapBonus.find(sName);
	if (cit != _mapBonus.end()) {
		return cit->second.dWeight;
	}
	return 1.0;
}

//policy
//term
//user_cate
//page_cate
static int TermFunctionHandle(asn_context &context, AdAttr &attr,
		asn_res_type::it_asn_res_list it, const double dWeight)
{
	vector<idclick::term_t> &rVec = context.p_request->term;
	size_t i = 0;
	for (; i<rVec.size();++i) {
		std::tr1::unordered_map<std::string, float>::const_iterator word_iter = attr.word.find( rVec.at(i).term );
		if ( word_iter != attr.word.end()) {
			//it's a hit, now record it and update the score
			it->s_trigger += word_iter->first + SPLIT;
			it->d_score   += dWeight * rVec.at(i).weight * word_iter->second;
			//TRACE("Ad[%u]; HIT TERM[%s]; WEIGHT[%f]; SCORE[%f]", 
			//it->n_adid, rVec.at(i).term.c_str(), rVec.at(i).weight, 
			//rVec.at(i).weight*_dWeight);
		}
	}
	return 0;
}

DECLEAR_CLASSNAME(Term);
RIGIST_CLASS(Term)

int Term::Process(asn_context &context)
{
	return 0;
}

static int InterestWordsFunctionHandle(asn_context &context, AdAttr &attr,
		asn_res_type::it_asn_res_list it, const double dWeight)
{
	if (!g_data().p_interest_handle || attr.user_cate.empty() ) {
		return 0;
	}
	vector<idclick::term_t> &rVec = context.p_request->term;
	size_t i = 0;
	char interestId[32];
	std::tr1::unordered_set<uint64_t>::iterator itInterest;
	for (; i<rVec.size();++i) {
		itInterest = attr.user_cate.begin();
		for (;itInterest != attr.user_cate.end(); ++itInterest) {//取出广告中的用户兴趣组 进行匹配
			const std::set<std::string> *pTmp = 
				g_data().p_interest_handle->get_using_buffer()->get_terms(*itInterest);
			if ( NULL == pTmp ) {
				continue;
			}

			std::set<string>::const_iterator cit;
			cit = pTmp->find(rVec.at(i).term);
			if (cit != pTmp->end()) {
				//it's a hit, now record it and update the score
				snprintf( interestId , sizeof( interestId ) , "I%lu", *itInterest );
				it->s_trigger += string( interestId ) + SPLIT;
				it->d_score   += dWeight * rVec.at(i).weight;
			}
		}
	}

	return 0;
}

DECLEAR_CLASSNAME(InterestWords);
RIGIST_CLASS(InterestWords)
int InterestWords::Process(asn_context &context)
{
	return 0;
}


static int PolicyFunctionHandle(asn_context &context,AdAttr &attr,
		asn_res_type::it_asn_res_list it, const double dWeight)
{
	vector<string> &rVec = context.p_request->policy;
	size_t i = 0;
	for (; i<rVec.size();++i) {
		if ( attr.policy_id.end() != attr.policy_id.find( rVec.at(i) ) ) {
			//it's a hit, now record it and update the score
			it->s_trigger += "P" + rVec.at(i) + SPLIT;
			it->d_score   += dWeight;
			//TRACE("Ad[%u]; HIT POLICY[%s]; SCORE[%f]", it->n_adid, rVec.at(i).c_str(), _dWeight);
		}
	}
	return 0;
}

DECLEAR_CLASSNAME(Policy);
RIGIST_CLASS(Policy)
int Policy::Process(asn_context &context)
{
	return 0;
}

static int UserCateFunctionHandle(asn_context &context,AdAttr &attr,
		asn_res_type::it_asn_res_list it, const double dWeight)
{
	vector<idclick::cate_t> &rVec = context.p_request->user_cate;
	char sCate[32];
	for (size_t i = 0; i<rVec.size();++i) {
		if ( attr.user_cate.end() != 
				attr.user_cate.find( (uint32_t)rVec.at(i).cate) ) {
			snprintf(sCate, sizeof(sCate), "I%u", rVec.at(i).cate);
			//it's a hit, now record it and update the score
			it->s_trigger += string(sCate) + SPLIT;
			it->d_score   += dWeight * rVec.at(i).weight;
		}
	}
	return 0;
}

DECLEAR_CLASSNAME(UserCate);
RIGIST_CLASS(UserCate)
int UserCate::Process(asn_context &context)
{
	return 0;
}

static int PageCateFunctionHandle(asn_context &context,AdAttr &attr,
		asn_res_type::it_asn_res_list it, const double dWeight)
{
	vector<idclick::cate_t> &rVec = context.p_request->page_cate;
	char sCate[32];
	for (size_t i = 0; i<rVec.size();++i) {
		if ( attr.page_cate.end() != attr.page_cate.find( (uint32_t)rVec.at(i).cate ) ) {
			//it's a hit, now record it and update the score
			snprintf(sCate, sizeof(sCate), "C%u", rVec.at(i).cate);
			it->s_trigger += string(sCate) + SPLIT;
			it->d_score   += dWeight * rVec.at(i).weight;
		}
	}
	return 0;
}

DECLEAR_CLASSNAME(PageCate);
RIGIST_CLASS(PageCate)
int PageCate::Process(asn_context &context)
{
	return 0;
}

static int __AdCmp(const asn_res_type::_resUnit &a, const asn_res_type::_resUnit &b)
{
	return a.d_score > b.d_score;
}

DECLEAR_CLASSNAME(Sorter);
int Sorter::Process(asn_context &context)
{
	asn_res_type::it_asn_res_list it = context.p_response->list_ads.begin();
	do {
		if (context.p_request->term.empty() &&  context.p_request->policy.empty() &&
				context.p_request->page_cate.empty() && context.p_request->user_cate.empty()) {
			break;//need to sort them
		}

		for (; it != context.p_response->list_ads.end(); ) {
			tItMapAdAttr itAttr = context.map_ad_attr.find(it->n_adid);
			if (itAttr == context.map_ad_attr.end()) { //cant find ad attribute
				//remove this ad
				it = context.p_response->list_ads.erase(it);
				continue;
			}
			sequence_index_value &value = *(itAttr->second);
			//how to use strategy(1:target, 2:rand)
			//do target
			FunctionHandleProc::citMapHandle cit = FunctionHandleProc::_mapBonus.begin();
			for (; cit != FunctionHandleProc::_mapBonus.end(); ++cit) {
				DEBUG("%s PROCESSING", cit->first.c_str());
				cit->second.pHandle(context, *value.second, it, cit->second.dWeight);
			}
			++it;
		}
	} while(0);
	
	it = context.p_response->list_ads.begin();
	while(it != context.p_response->list_ads.end()){
		tItMapAdAttr itAttr = context.map_ad_attr.find(it->n_adid);
		if (itAttr == context.map_ad_attr.end()) { //cant find ad attribute
			//remove this ad
			it = context.p_response->list_ads.erase(it);
			continue;
		}

		sequence_index_value &value = *(itAttr->second);
		if (value.second->strategy_state==tState&&!it->s_trigger.size()){
			it = context.p_response->list_ads.erase(it);
			continue;
		} else if (value.second->strategy_state==rState&&it->s_trigger.size()){
			it = context.p_response->list_ads.erase(it);
			continue;
		}
		++it;
	}

	std::sort(context.p_response->list_ads.begin(),context.p_response->list_ads.end(), __AdCmp);

	char id_buf[128];
	std::string id_list;
	while (context.p_response->list_ads.size() > context.p_request->max_res_num) {
		asn_res_type::cit_asn_res_list citer = context.p_response->list_ads.end() - 1;
		snprintf(id_buf, sizeof(id_buf), "%u, ", citer->n_adid);
		id_list += id_buf;
		context.p_response->list_ads.pop_back();
	}
	log_pushnotice("PopIdList", "%s", id_list.c_str());

	return 0;
}

class AgentCtrlImpl 
{
  public:
	struct __Agent{
		std::string className;
		AgentIf *pAgent;
	};
	AgentCtrlImpl(){}
	~AgentCtrlImpl();
	int InitAgentFlow(const std::string &flowKey);
	int Process(asn_context& context);
  private:
	std::vector<__Agent> _vAgent;//存储每种业务顺序进行的流程
};

AgentCtrlImpl::~AgentCtrlImpl()
{
	if (!_vAgent.size()) {
		return;
	}
	std::vector<__Agent>::iterator it = _vAgent.begin();
	while (it != _vAgent.end()) {
		if (NULL != it->pAgent) {
			delete it->pAgent;
			it->pAgent = NULL;
		}
		++it;
	}
}

int AgentCtrlImpl::InitAgentFlow(const std::string &flowKey)
{
	string v=g_data().p_config_parser->get(WorkFlowSectionName, flowKey);
	if (v.empty()){
		FATAL("the flow: %s no Agent", flowKey.c_str());
		return -1;
	}
	vector<string> vec_v;
	split_str_to_vector(v, vec_v);
	std::string str;
	for (size_t i=0; i<vec_v.size(); ++i){
		AgentIf *pAgent = (AgentIf *)AgentFactory::GetAgent(vec_v[i]);
		str=g_data().p_config_parser->get(FieldSectionName, vec_v[i]);
		if (str.size()){
			INFO("set field: %s", str.c_str());
			pAgent->SetField(str);
		}
		str=g_data().p_config_parser->get(WeightSectionName, vec_v[i]);
		if (str.size()){
			double weight=strtod(str.c_str(), NULL);
			INFO("set weight: %f", weight);
			pAgent->SetWeight(weight);
			FunctionHandleProc::WeightRegist(vec_v[i].c_str(), weight);
		}
		pAgent->SetClassName(vec_v[i]);
		INFO("set class name: %s", vec_v[i].c_str());
		__Agent tmp;
		tmp.className=vec_v[i];
		tmp.pAgent=pAgent;
		_vAgent.push_back(tmp);
	}
	return 0;
}

int AgentCtrlImpl::Process(asn_context& context)
{
	std::vector<__Agent>::iterator it;
	it=_vAgent.begin();
	int ret=0;
	for (; it!=_vAgent.end(); ++it){
		ret=it->pAgent->Process(context);
		if (ret){
			return ret;
		}
	}
	return 0;
}

AgentCtrl::AgentCtrl():_pHandle(NULL)
{
	_pHandle = new(std::nothrow) AgentCtrlImpl;
}

AgentCtrl::~AgentCtrl()
{
	if (_pHandle) {
		delete _pHandle;
		_pHandle = NULL;
	}
}

int AgentCtrl::InitAgentFlow(const string &flowKey)
{
	if (!_pHandle) {
		WARN("handle is invalid");
		return -1;
	}
	if (flowKey.empty()) {
		WARN("NULL flowKey passed in");
		return -1;
	}
	return _pHandle->InitAgentFlow(flowKey);
}

int AgentCtrl::Process(asn_context& context)
{
	if (!_pHandle) {
		WARN("handle is invalid");
		return -1;
	}
	return _pHandle->Process(context);
}

class WorkFlowCtrlImpl
{
public:
	WorkFlowCtrlImpl(){}

	~WorkFlowCtrlImpl(){}

	int InitWorkFlow();

	int Process(asn_context &context);
private:
	std::vector<AgentCtrl*> _vWorkFlow;
};

int WorkFlowCtrlImpl::InitWorkFlow()
{
	string v=g_data().p_config_parser->get(WorkFlowSectionName, WorkFlowFirstKeyName);
	if (v.empty()){
		FATAL("the key: %s no value", WorkFlowFirstKeyName);
		return -1;
	}
	int ret=0;
	vector<string> vec_v;
	split_str_to_vector(v, vec_v);
	for (size_t i=0; i<vec_v.size(); ++i){
		AgentCtrl *pAgent = (AgentCtrl *)AgentFactory::GetAgent(vec_v[i]);  
		ret=pAgent->InitAgentFlow(vec_v[i]);
		if (ret){
			FATAL("Run InitWorkFlow failed");
			return ret;
		}
		_vWorkFlow.push_back(pAgent);
	}
	return 0;
}

int WorkFlowCtrlImpl::Process(asn_context &context)
{
	int ret=0;
	vector<AgentCtrl *>::iterator it;
	it=_vWorkFlow.begin();
	for (; it!=_vWorkFlow.end(); ++it){
		ret=(*it)->Process(context);
		if (ret){
			return ret;
		}
	}
	return 0;
}

///<检索流程控制类的构造函数
WorkFlowCtrl::WorkFlowCtrl():_pHandle(NULL)
{
	_pHandle = new (std::nothrow) WorkFlowCtrlImpl;
}

///<检索流程控制类的析构函数
WorkFlowCtrl::~WorkFlowCtrl()
{
	if (_pHandle) {
		delete _pHandle;
		_pHandle = NULL;
	}
}

///<检索流程控制类的初始化函数
int WorkFlowCtrl::InitWorkFlow()
{
	if (!_pHandle) {
		WARN("handle is invalid");
		return -1;
	}
	return _pHandle->InitWorkFlow();
}

///<检索流程控制类的初始化函数
int WorkFlowCtrl::Process(asn_context &context)
{
	if (!_pHandle) {
		WARN("handle is invalid");
		return -1;
	}
	return _pHandle->Process(context);
}


/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
