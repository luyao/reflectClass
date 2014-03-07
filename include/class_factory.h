#ifndef  __CLASS_FACTORY_H_
#define  __CLASS_FACTORY_H_

#include <string>
#include <map>

typedef void *(*CreateFuntion) (void);

typedef std::map < std::string, CreateFuntion > ClassFactoryDict;
typedef ClassFactoryDict::const_iterator ClassFactoryDictCIter;

static ClassFactoryDict& _getMap();

class NamePrinter{
public:
	NamePrinter():name_("Base class"){}

	explicit NamePrinter(const std::string &name):name_(name){}

	virtual std::string GetClassName()const{
		return name_;
	}

	virtual ~NamePrinter(){}
protected:
	std::string name_;
};

#define DEFINE_CLASS(class_name) \
	extern char class_name##Argv[];\
	class class_name: public Register<class_name, class_name##Argv>

#define DEFINE_INTERFACE(class_name)\
	class class_name: public NamePrinter

//if you define the class in the header file, you must declear it in
//the source file
//TODO: find a subsitution of the solution
#define DECLEAR_CLASS(class_name) \
	char class_name##Argv[] = #class_name;

#define DEFINE_CLASS_EX(class_name, father_class)\
	extern char class_name##Argv[];\
	class class_name:public Register<class_name,class_name##Argv>, public father_class

class ClassFactory{
public:
	static void *GetClass(const std::string & name){
		ClassFactoryDictCIter it_find = _getMap().find(name);
		if (it_find == _getMap().end())return NULL;
		else return it_find->second();
	}

	static void RegistClass(const std::string &name, CreateFuntion method){
		_getMap().insert(ClassFactoryDict::value_type(name, method));
	}
private:
	ClassFactory();
	ClassFactory(const ClassFactory&);
	ClassFactory& operator=(const ClassFactory&);
};


class RegistyClass {
public:
	RegistyClass(const std::string & name, CreateFuntion method) {
		ClassFactory::RegistClass(name, method);
	}
	void init()const{}
};

enum CLASS_TYPE{
	DEFAULT = 0,
	SINGLETON,
	SINGLETON_MULTI,
};

static ClassFactoryDict& _getMap(){
	static  ClassFactoryDict _map;
	return  _map;
}

template < class T, const char name[], int ClassType = DEFAULT> 
class Register : public NamePrinter{
public:
	static void *CreateInstance() {
		switch(ClassType){
			case DEFAULT:
				return new (std::nothrow) T;
			case SINGLETON:
				static T instance;
				return &instance;
			case SINGLETON_MULTI:
			default:
				return NULL;
		};
	}

public:
	static const RegistyClass cRegist_;
protected:
	Register() : NamePrinter(name){
		cRegist_.init();
	}
	//this class only be used as base class
};

template < class T, const char name[], int ClassType>
const RegistyClass
Register<T, name, ClassType>::cRegist_(name, Register<T, name, ClassType>::CreateInstance);

#endif

/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
