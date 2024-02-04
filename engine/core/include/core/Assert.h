#include "Log.h"

#ifndef NDEBUG
#   define INF_ASSERT(Expr, Msg) \
    if(!(Expr)) {\
    INF::Log::Warning(std::format("ASSERT::{}:{} - {}", __FILE__, __LINE__, Msg)); \
    __debugbreak();}
#else
#   define INF_ASSERT(Expr, Msg) ;
#endif

#ifndef NDEBUG
#   define INF_VERIFY(Expr, Msg) \
    if(!(Expr)) {\
    INF::Log::Warning(std::format("VERIFY::{}:{} - {}", __FILE__, __LINE__, Msg)); \
    __debugbreak();}
#else
#   define INF_VERIFY(Expr, Msg) (Expr);
#endif