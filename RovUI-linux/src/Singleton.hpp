#pragma once

#include <QScopedPointer>
#include <QtGlobal>

#include "CallOnce.hpp"

template <class T>
class Singleton {
private:
    typedef T* (*CreateInstanceFunction)();

public:
    static T* instance(CreateInstanceFunction create);

private:
    static void init();

    Singleton();

    ~Singleton();
    Q_DISABLE_COPY(Singleton)

    static QBasicAtomicPointer<void> create;
    static QBasicAtomicInt flag;
    static QBasicAtomicPointer<void> tptr;
    bool inited;
};

template <class T>
T* Singleton<T>::instance(CreateInstanceFunction create)
{
    Singleton::create.storeRelaxed((void*)create);
    qCallOnce(init, flag);
    return (T*)tptr.loadRelaxed();
}

template <class T>
void Singleton<T>::init()
{
    static Singleton singleton;
    if (singleton.inited) {
        CreateInstanceFunction createFunction = (CreateInstanceFunction)Singleton::create.loadRelaxed();
        tptr.storeRelaxed(createFunction());
    }
}

template <class T>
Singleton<T>::Singleton() { inited = true; };

template <class T>
Singleton<T>::~Singleton()
{
    T* createdTptr = (T*)tptr.fetchAndStoreOrdered(nullptr);
    if (createdTptr) {
        delete createdTptr;
    }
    create.storeRelaxed(nullptr);
}

template <class T>
QBasicAtomicPointer<void>
    Singleton<T>::create = Q_BASIC_ATOMIC_INITIALIZER(nullptr);
template <class T>
QBasicAtomicInt
    Singleton<T>::flag
    = Q_BASIC_ATOMIC_INITIALIZER(CallOnce::CO_Request);
template <class T>
QBasicAtomicPointer<void>
    Singleton<T>::tptr = Q_BASIC_ATOMIC_INITIALIZER(nullptr);
