#pragma once

typedef struct DECLSPEC_ALIGN(16) _M128A {
    ULONGLONG Low;
    LONGLONG High;
}M128A, *PM128A;

#define nullptr ((void*)0)
