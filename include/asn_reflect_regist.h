#ifndef  __INCLUDE_ASN_REFLECT_REGIST_H_
#define  __INCLUDE_ASN_REFLECT_REGIST_H_

#include <string>

class RegistyClass {
	using std::string;
	public:
		RegistyClass(const string & name, CreateFuntion method) {
			ClassFactory::RegistClass(name, method);
		}
};

template < class T, const char name[] > 
class Register {
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
	Register() {} 
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
