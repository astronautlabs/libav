#include "index.h"
#include "avutil.h"
#include "buffer.h"
#include "dict.h"
#include "frame.h"
#include "class.h"

void nlavu_init(Napi::Env env, Napi::Object exports) {    
    NAVUtil::Register(env, exports);
    NAVBuffer::Register(env, exports);
    NAVBufferPool::Register(env, exports);
    NAVDictionary::Register(env, exports);
    NAVDictionaryEntry::Register(env, exports);
    NAVFrame::Register(env, exports);
    NAVFrameSideData::Register(env, exports);
    NAVClass::Register(env, exports);
}