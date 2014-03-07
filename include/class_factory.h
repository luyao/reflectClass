#ifndef  __CLASS_FACTORY_H_
#define  __CLASS_FACTORY_H_

#include <string>
#include <map>

typedef void *(*CreateFuntion) (void);

class ClassFactory{
public:
	typedef std::map < std::string, CreateFuntion > ClassFactoryDict;
	typedef std::map < std::string, CreateFuntion >::const_iterator ClassFactoryDictCIter;
	static void *GetClass(const std::string & name);
	static void RegistClass(const std::string &name, CreateFuntion method){
		_getMap().insert(ClassFactoryDict::value_type(name, method));
	}
private:
	ClassFactory();
	ClassFactory(const ClassFactory&);
	ClassFactory& operator(const ClassFactory&);

	std::map< std::string, CreateFuntion >& _getMap(){
		static  ClassFactoryDict _map;
		return  _map;
	}
};

void* ClassFactory::GetClass(const std::string & name)
{
	ClassFactoryDictCIter it_find = _getMap().find(name);
	if (it_find == _getMap().end()) {
		return NULL;
	} else {
		return it_find->second();
	}
}

class RegistyClass {
using std::string;
public:
	RegistyClass(const string & name, CreateFuntion method) {
		ClassFactory::RegistClass(name, method);
	}
};

template < class T, const char name[] > 
class Register{
public:
	static void *CreateInstance() {
		return new T;
	}

	static void *CreateSingletonInstance(){
		static T instance;
		return &T;
	}
public:
	static const RegistyClass cRegist_;
	static const RegistyClass cRegistSingleton_;
protected:
	Register() {} //this class only be used as base class
};

template < class T, const char name[] >
const RegistyClass
Register < T, name >::cRegist_(name, Register < T, name >::CreateInstance);

template < class T, const char name[] >
const RegistyClass
Register < T, name >::cRegist_(name, Register < T, name >::CreateSingletonInstance);

#define DEFINE_CLASS(class_name) \
	extern char class_name##Argv[];\
	class class_name:public Register<class_name,class_name##Argv>

#define DEFINE_CLASS_EX(class_name,father_class)\
	extern char class_name##Argv[];\
	class class_name:public Register<class_name,class_name##Argv>,public father_class


#endif

/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
