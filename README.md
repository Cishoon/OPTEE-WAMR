[中文版本](README_zh.md) | [English Version](README.md)

# OPTEE WAMR

This project is a trusted application based on [WaTZ](https://github.com/JamesMenetrey/unine-watz/tree/main?tab=readme-ov-file) and ported to the latest version of OPTEE. It supports running AoT format WebAssembly (Wasm) programs within a Trusted Execution Environment (TEE).

## Project Sources

This project is based on specific versions of `unine-watz`, `wasm-micro-runtime`, and `optee_os`, ensuring compatibility across different modules within the latest OPTEE environment.

- The project is based on version `a436628` of `unine-watz` (Apache License 2.0) and has removed the original project's remote authentication components.
- The project references version `cba4c782` of `wasm-micro-runtime` (Apache License 2.0) for compiling usable `AoT` files.
- The project is based on version `19662e4` of `optee_os`, with the addition of the `mprotect` system call.

The file `./optee_wamr/build/libvmlib.a` in this project is compiled from `unine-watz`.

## Usage

1. Use `./third_party/optee_os` to overwrite the `optee_os` in the original project.
2. Move the `./optee_wamr` folder into `optee_examples`.
3. Recompile and run.

After entering the Normal World, use the following command to run a Wasm program:

```
optee_wamr <heap_size> <aot_path>
```

- The `AoT` file must be compiled with the `cba4c782` version of `wasm-micro-runtime`'s `wamrc`, specifying the `--target=aarch64` option. See [Build wamrc AOT compiler](https://github.com/bytecodealliance/wasm-micro-runtime/blob/main/wamr-compiler/README.md) for details on compiling `wamrc`.
- The `wasm` file used to compile the `AoT` file needs to be compiled by `wasi-sdk`.
- The `heap_size` value may vary depending on the environment; in my test environment, the valid range is `[408340, 12573936]`. Generally, setting it to `1000000` provides stable performance.

### Importing External Functions

Add the external functions you need to import in the `optee_wamr/ta/wasi.c` and `optee_wamr/ta/wasi.h` files. For details, refer to: [Exporting Native API Steps](https://wamr.gitbook.io/document/wamr-in-practice/features/export_native_api#exporting-native-api-steps)

## Test Code Description

### Hello World

Compile the `hello.c` file to get the `hello.wasm` file:
```bash
/opt/wasi-sdk/bin/clang -O3 -o hello.wasm hello.c
```

Then use `wamrc` to compile the `hello.aot` file:
```bash
wamrc --target=aarch64 --disable-simd hello.wasm -o hello.aot
```

Run the command `optee_wamr 1000000 ./hello.aot` in OPTEE to output `Hello WebAssembly!`.

### External Function Test (Rust)

Compile the `external_function.rs` file to get the `external_function.wasm` file:
```bash
rustc -C link-self-contained=no \
    -C link-args=--no-entry \
    -C link-args=-zstack-size=32768 \
    --target wasm32-wasip1 external_function.rs
```

Then use `wamrc` to compile the `external_function.aot` file:
```bash
wamrc --target=aarch64 --disable-simd external_function.wasm -o external_function.aot
```

Run the command `optee_wamr 1000000 ./external_function.aot` in OPTEE to output `Hello Rust World!` in the Secure World.


