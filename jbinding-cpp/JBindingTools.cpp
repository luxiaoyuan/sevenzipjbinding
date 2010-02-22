/*
 * JBindingTools.cpp
 *
 *  Created on: Jan 31, 2010
 *      Author: boris
 */

#include "SevenZipJBinding.h"

#include "JBindingTools.h"

#define JPARAM_STRING(name, param_spec) JPARAM(String, "Ljava/lang/String;", name, param_spec)

BEGIN_JCLASS("net/sf/sevenzipjbinding", SevenZipException)
/*    */JCLASS_VIRTUAL_METHOD(Object, initCause, "(Ljava/lang/Throwable;)Ljava/lang/Throwable;")
/*    */JCLASS_CONSTRUCTOR(JPARAM_STRING(message, JPARAM_STRING(param2, _)))
/*    */JCLASS_CONSTRUCTOR(JPARAM_STRING(message, _))
/*    */JCLASS_CONSTRUCTOR(_)
/*    */JCLASS_FINAL_METHOD(Void, setCauseLastThrown, "(Ljava/lang/Throwable;)") // (Throwable causeLastThrown)
/*    */JCLASS_FINAL_METHOD(Void, setCauseFirstPotentialThrown, "(Ljava/lang/Throwable;)") // (Throwable causeFirstPotentialThrown)
/*    */JCLASS_FINAL_METHOD(Void, setCauseLastPotentialThrown, "(Ljava/lang/Throwable;)") // (Throwable causeLastPotentialThrown)
END_JCLASS

JavaVM * JBindingSession::_vm = NULL;

void JBindingSession::handleThrownException(jthrowable exceptionLocalRef) {
    ThreadId threadId = PlatformGetCurrentThreadId();

    _threadContextMapCriticalSection.Enter();
    ThreadContext & threadContext = _threadContextMap[threadId];
    MY_ASSERT(!threadContext._javaNativeContext.size())

    // All active JNINativeCallContext objects should be notified
    ThreadContextMap::iterator threadContextIterator = _threadContextMap.begin();
    while (threadContextIterator != _threadContextMap.end()) {
        std::list<JNINativeCallContext *> & jniNativeCallContextList =
                threadContextIterator->second._javaNativeContext;
        if (jniNativeCallContextList.size()) {
            (*jniNativeCallContextList.begin())->exceptionThrownInOtherThread(threadContext._env,
                    exceptionLocalRef);
        }
    }
    _threadContextMapCriticalSection.Leave();
}

JNINativeCallContext::~JNINativeCallContext() {
    _jbindingSession.unregisterNativeContext(*this);

    //jthrowable firstThrownException = NULL;
    //jthrowable lastThrownException = NULL;
    //jthrowable firstThrownExceptionInOtherThread = NULL;
    //jthrowable lastThrownExceptionInOtherThread = NULL;

    if (_firstThrownException && !_lastThrownException && !_firstThrownExceptionInOtherThread && !_lastThrownExceptionInOtherThread ) {
        TRACE("Rethrowing exception " << _lastThrownException)
        _jniCallOriginalEnv->Throw(_firstThrownException);
    } else {
        if (_firstThrownException) {
            jthrowable sevenZipException;
            if (jni::SevenZipException::isInstance(_jniCallOriginalEnv, _firstThrownException)) {
                // Last thrown exception is SevenZipException. Reuse it.
                sevenZipException = _lastThrownException;
            } else {
                // Create new SevenZipException to pass more than one caused by.
                sevenZipException = static_cast<jthrowable>(jni::SevenZipException::newInstance(_jniCallOriginalEnv));
                assertNoExceptionOnJniCallOriginalEnv();

                jni::SevenZipException::initCause(_jniCallOriginalEnv, sevenZipException, _firstThrownException);
                assertNoExceptionOnJniCallOriginalEnv();
            }

            if (_lastThrownException) {
                jni::SevenZipException::setCauseLastThrown(_jniCallOriginalEnv, sevenZipException, _lastThrownException);
                assertNoExceptionOnJniCallOriginalEnv();
            }
            if (_firstThrownExceptionInOtherThread) {
                jni::SevenZipException::setCauseFirstPotentialThrown(_jniCallOriginalEnv, sevenZipException, _firstThrownExceptionInOtherThread);
                assertNoExceptionOnJniCallOriginalEnv();
            }
            if (_lastThrownExceptionInOtherThread) {
                jni::SevenZipException::setCauseLastPotentialThrown(_jniCallOriginalEnv, sevenZipException, _lastThrownExceptionInOtherThread);
                assertNoExceptionOnJniCallOriginalEnv();
            }
            _jniCallOriginalEnv->Throw(sevenZipException);
        }
    }

    if (_firstThrownException) {
        //firstThrownException = _jniCallOriginalEnv->NewLocalRef(_firstThrownException);
        _jniCallOriginalEnv->DeleteGlobalRef(_firstThrownException);
    }
    if (_lastThrownException) {
        //lastThrownException = _jniCallOriginalEnv->NewLocalRef(_lastThrownException);
        _jniCallOriginalEnv->DeleteGlobalRef(_lastThrownException);
    }
    if (_firstThrownExceptionInOtherThread) {
        //firstThrownExceptionInOtherThread = _jniCallOriginalEnv->NewLocalRef(_firstThrownExceptionInOtherThread);
        _jniCallOriginalEnv->DeleteGlobalRef(_firstThrownExceptionInOtherThread);
    }
    if (_lastThrownExceptionInOtherThread) {
        //lastThrownExceptionInOtherThread = _jniCallOriginalEnv->NewLocalRef(_lastThrownExceptionInOtherThread);
        _jniCallOriginalEnv->DeleteGlobalRef(_lastThrownExceptionInOtherThread);
    }

    // Throw Exceptions
    // - throw SevenZipException directly
    // - throw all other exceptions wrapped in SevenZipException
    // - set lastThrownException

}

void JNINativeCallContext::throwException(char const * msg, ...) {
    // TODO
}

#ifdef USE_MY_ASSERTS
int JBindingSession::_attachedThreadCount = 0;
extern "C" JNIEXPORT jint JNICALL Java_net_sf_sevenzipjbinding_junit_tools_SevenZipDebug_nativeGetAttachedThreadCount(
                                                                                                                      JNIEnv * env,
                                                                                                                      jclass clazz) {
    return JBindingSession::_attachedThreadCount;
}
#endif
