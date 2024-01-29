#include "Log.h"

#ifndef NDEBUG
#   define INF_ASSERT(Expr, Msg) \
    Inferno::Log::Warning(std::format("ASSERT::{}:{} - {}", __FILE__, __LINE__, Msg)); \
    __debugbreak();
#else
#   define INF_ASSERT(Expr, Msg) ;
#endif