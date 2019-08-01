#ifndef __FAIRYGUIMACROS_H__
#define __FAIRYGUIMACROS_H__

#define f_assert(...)
#define f_assert2(...)
#define f_assertmsg(...)
#define f_printf(...)

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#define NS_FGUI_BEGIN                     namespace fairygui {
#define NS_FGUI_END                       }
#define USING_NS_FGUI                     using namespace fairygui

#define CALL_LATER_FUNC(__TYPE__,__FUNC__) \
void __selector_##__FUNC__(float dt) \
{\
    Scheduler::getInstance()->unschedule(FUI_SCHEDULE_SELECTOR(__TYPE__::__selector_##__FUNC__), this);\
    __FUNC__(); \
}\
void __FUNC__()

#define CREATE_FUNC(CLASS) \
static CLASS* create() \
{ \
    CLASS* pRet = new CLASS;\
    if (pRet != nullptr && pRet->init())\
        pRet->autorelease();\
    return pRet; \
}

#define FUI_SAFE_DELETE(p)           do { delete (p); (p) = nullptr; } while(0)
#define FUI_SAFE_RELEASE(p)          do { if(p) { (p)->release(); } } while(0)
#define FUI_SAFE_RETAIN(p)           do { if(p) { (p)->retain(); } } while(0)

#define CALL_LATER(__TYPE__,__FUNC__,...) \
Scheduler::getInstance()->schedule(FUI_SCHEDULE_SELECTOR(__TYPE__::__selector_##__FUNC__), this, (__VA_ARGS__+0), false)

#define CALL_LATER_CANCEL(__TYPE__,__FUNC__) \
Scheduler::getInstance()->unschedule(FUI_SCHEDULE_SELECTOR(__TYPE__::__selector_##__FUNC__), this)

#define CALL_PER_FRAME(__TYPE__,__FUNC__) \
Scheduler::getInstance()->schedule(FUI_SCHEDULE_SELECTOR(__TYPE__::__FUNC__), this, 0, false)

#define CALL_PER_FRAME_CANCEL(__TYPE__,__FUNC__) \
Scheduler::getInstance()->unschedule(FUI_SCHEDULE_SELECTOR(__TYPE__::__FUNC__), this)

#define UIRoot GRoot::getInstance()

#define FUI_RTTI_BASE(CLASS) \
protected: \
    template<class T> static void* getTypeId() { static char TYPE; return &TYPE; }; \
    virtual bool isTypeOf(const void* type) const { return type == this->getTypeId<CLASS>(); }; \
public: \
    template<class T> bool IsA() const { return this->isTypeOf(getTypeId<T>()); }; \
    template<class T> T* SafeCast() const { return this->isTypeOf(this->getTypeId<T>()) ? (T*) this : nullptr; };
//    template<class T> T* SafeCast() const { n_assert(this->isTypeOf(this->getTypeId<T>())); return (T*) this; };

#define FUI_RTTI_DERIVED(CLASS, BASECLASS) \
protected: \
    bool isTypeOf(const void* type) const override { return type == this->getTypeId<CLASS>() ? true : BASECLASS::isTypeOf(type); };


#define FUI_Declare_Singleton(type) \
private: \
    static type * Singleton; \
public:\
    static void CreateInstance() { if(nullptr == Singleton) Singleton = new type;}\
    static void DestroyInstance() { if (nullptr != Singleton) delete Singleton; Singleton = nullptr; }\
    static type * GetInstance() { AZ_Assert(nullptr != Singleton, "Singleton is nullptr"); return Singleton; }; \
    static type * getInstance() { AZ_Assert(nullptr != Singleton, "Singleton is nullptr"); return Singleton; }; \
    static bool HasInstance() { return nullptr != Singleton; }; \
private:

#define FUI__Implement_Singleton(type) \
    type * type::Singleton = nullptr;


#include "tinyxml2/tinyxml2.h"

typedef tinyxml2::XMLElement TXMLElement;
typedef tinyxml2::XMLDocument TXMLDocument;

#endif