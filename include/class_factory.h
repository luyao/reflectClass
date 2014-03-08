/** 
 * @file class_factory.h
 * @Synopsis  
 *
 * @author Yao Lu(yaolu1103@gmail.com) 
 * @version 1.0
 * @date 2014-03-08
 */

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
    NamePrinter():name_("Base class"){}  //sometime we need default

    explicit NamePrinter(const char *name):name_(name){}

    virtual const char* GetClassName()const{return name_;}

    virtual ~NamePrinter(){}
protected:
    const char *name_;
};

namespace create_function{
    template<typename T>
    void *DEFAULT(){return new (std::nothrow) T;}
    template<typename T>
    void *SINGLETON(){static T instance; return &instance;}
}

#define DECLEAR_CLASS(class_name) \
    extern char class_name##Argv[];\
    class class_name: public Register<class_name, class_name##Argv>

#define DECLEAR_SINGLETON_CLASS(class_name) \
    extern char class_name##Argv[];\
    class class_name: public Register<class_name, \
                      class_name##Argv, create_function::SINGLETON<class_name> >

#define DECLEAR_INTERFACE(class_name)\
    class class_name: public NamePrinter

//if you define the class in the header file, you must declear it in
//the source file
#define DEFINE_CLASS(class_name) \
    char class_name##Argv[] = #class_name;

#define DECLEAR_CLASS_EX(class_name, father_class)\
    extern char class_name##Argv[];\
    class class_name:public Register<class_name,class_name##Argv>, public father_class

#define DECLEAR_SINGLETON_CLASS_EX(class_name, father_class)\
    extern char class_name##Argv[];\
    class class_name:public Register<class_name,\
                     class_name##Argv, create_function::SINGLETON<class_name> >,\
                     public father_class


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
#if __cplusplus > 199711L //using c++0x
    ClassFactory() = delete;
    ClassFactory(const ClassFactory&) = delete;
    ClassFactory& operator=(const ClassFactory&) = delete;
#else
    ClassFactory();
    ClassFactory(const ClassFactory&);
    ClassFactory& operator=(const ClassFactory&);  
#endif

};

class RegistyClass {
public:
    RegistyClass(const std::string & name, CreateFuntion method) {
        ClassFactory::RegistClass(name, method);
    }
};

static ClassFactoryDict& _getMap(){
    static  ClassFactoryDict _map;
    return  _map;
}

template <typename T, const char *name, CreateFuntion Create=create_function::DEFAULT<T> > 
class Register : public NamePrinter{
public:
    static const RegistyClass regist_;
protected:
    //this class only be used as base class, 
    //and the void make sure the static object is generated
    Register() : NamePrinter(name){(void)regist_;} 
};

template < class T, const char *name, CreateFuntion Create> 
const RegistyClass Register<T, name, Create>::regist_(name, Create);

#endif

/* vim: set expandtab ts=256 sw=4 sts=2 tw=80 noet:*/
