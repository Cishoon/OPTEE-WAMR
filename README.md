[中文版本](README_zh.md) | [English Version](README.md)

# OPTEE WAMR

This project is a trusted application based on [WaTZ](https://github.com/JamesMenetrey/unine-watz/tree/main?tab=readme-ov-file) and ported to the latest version of OPTEE. It supports running AoT format WebAssembly (Wasm) programs within a Trusted Execution Environment (TEE).

## Changelog
- **2025-3-1**: Updated to the latest version of `wasm-micro-runtime`, supporting new features such as the multi-memory proposal.

## Project Sources

This project is based on specific versions of `unine-watz`, `wasm-micro-runtime`, and `optee_os`, ensuring compatibility across different modules within the latest OPTEE environment.

- The project is based on version `a436628` of `unine-watz` (Apache License 2.0) and has removed the original project's remote authentication components.
- The project references the latest version of `wasm-micro-runtime` (Apache License 2.0), supporting more WebAssembly features.
- The project is based on version `19662e4` of `optee_os`, with the addition of the `mprotect` system call.

## VM Library Compilation

```bash
cd third_party/wasm-micro-runtime/product-mini/platforms/linux-trustzone
mkdir -p build 
cd build
cmake ..
make
```

## Usage

1. Use `./third_party/optee_os` to overwrite the `optee_os` in the original project.
2. Move the `./optee_wamr` folder into `optee_examples`.
3. Recompile OP-TEE.

After entering the Normal World, use the following command to run a Wasm program:

```
optee_wamr <heap_size> <aot_path>
```

- The `AoT` file is compiled using `wamrc` from `third_party/wasm-micro-runtime`, specifying the `--target=aarch64` option. See [Build wamrc AOT compiler](https://github.com/bytecodealliance/wasm-micro-runtime/blob/main/wamr-compiler/README.md) for details on compiling `wamrc`.
- The `heap_size` is the amount of heap space allocated to the Wasm program, measured in bytes. It should be at least larger than the AoT file.

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
    --target wasm32v1-none external_function.rs
```

Then use `wamrc` to compile the `external_function.aot` file:
```bash
wamrc --target=aarch64 --disable-simd external_function.wasm -o external_function.aot
```

Run the command `optee_wamr 1000000 ./external_function.aot` in OPTEE to output `Hello Rust World!` in the Secure World.

### Rust & Dynamic Memory Allocation

Compile the `wasm-rust` project to get the `wasm-rust.aot` file:
```bash
cd test/wasm-rust
./build.sh
```

Import the external library `wee_alloc` as the memory allocator, so that dynamic memory allocation can be used in the Rust-compiled wasm.