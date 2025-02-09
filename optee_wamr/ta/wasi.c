#include "logging.h"
#include "wasi.h"
#include "wasm.h"
#include <stdio.h>


int foo(wasm_exec_env_t exec_env, int a, int b)
{
    DMSG("foo has been called");
    return a + b;
}

int puts_native(wasm_exec_env_t exec_env, const char *s)
{
    DMSG("puts has been called");
    return puts(s);
}

NativeSymbol wasi_native_symbols[] = {
    EXPORT_WASM_API_WITH_SIG(foo, "(ii)i"),
    {
        "puts",
        puts_native,
        "($)i"
    },
};

const size_t wasi_native_symbols_size = sizeof(wasi_native_symbols);
