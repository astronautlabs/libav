#include "index.h"
#include "codec.h"
#include "codec-context.h"
#include "profile.h"
#include "packet.h"

extern "C" {
    #include <libavutil/avutil.h>
}

void nlavc_init(Napi::Env env, Napi::Object exports) {
    NAVProfile::Register(env, exports);
    NAVCodec::Register(env, exports);
    NAVCodecContext::Register(env, exports);
    NAVPacket::Register(env, exports);
}