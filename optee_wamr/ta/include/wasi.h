#ifndef WASI_H
#define WASI_H

#include "wasm_export.h"
#include "bh_platform.h"

int foo(wasm_exec_env_t exec_env, int a, int b);
int puts_native(wasm_exec_env_t exec_env, const char *s);

extern NativeSymbol wasi_native_symbols[];
extern const size_t wasi_native_symbols_size;

#endif
