[中文版本](README_zh.md) | [English Version](README.md)

# OPTEE WAMR

本项目是一个基于 [WaTZ](https://github.com/JamesMenetrey/unine-watz/tree/main?tab=readme-ov-file) 移植到最新的 OPTEE 中的可信应用程序，支持在 TEE 中运行 AoT 格式的 wasm 程序。

## 更新日志
- **2025-3-1**: 更新至最新版 `wasm-micro-runtime`，支持多内存提案等新特性。

## 项目来源

本项目基于特定版本的 `unine-watz`、`wasm-micro-runtime` 和 `optee_os`，确保不同模块在最新的 OPTEE 环境中兼容。

* 本项目基于 `unine-watz` 的 `a436628` 版本（Apache License 2.0），删除了原项目中远程认证的部分。
* 本项目引用了最新版本的 `wasm-micro-runtime`（Apache License 2.0），支持更多 WebAssembly 特性。
* 本项目基于 `optee_os` 的 `19662e4` 版本，增加了 `mprotect` 系统调用。

## vm 库编译

```bash
cd third_party/wasm-micro-runtime/product-mini/platforms/linux-trustzone
mkdir -p build 
cd build
cmake ..
make
```

## 使用方法

1. 用 `./third_party/optee_os` 覆盖原项目中的 `optee_os`。
2. 将 `./optee_wamr` 文件夹移入 `optee_examples` 中。
3. 重新编译 OP-TEE 即可。

进入 Normal World 后，使用以下命令运行 wasm 程序：

```
optee_wamr <heap_size> <aot_path>
```

- 使用的 `AoT` 文件由 `third_party/wasm-micro-runtime` 的 `wamrc` 编译得到，并指定 `--target=aarch64`。编译  `wamrc` 的方式见：[Build wamrc AOT compiler](https://github.com/bytecodealliance/wasm-micro-runtime/blob/main/wamr-compiler/README.md)
- `heap_size` 为分配给 wasm 程序的堆空间大小，单位为字节。至少要比 aot 文件大。

### 外部函数导入

在 `optee_wamr/ta/wasi.c` 和 `optee_wamr/ta/wasi.h` 文件中，添加需要导入的外部函数。具体参考：[Exporting Native API Steps](https://wamr.gitbook.io/document/wamr-in-practice/features/export_native_api#exporting-native-api-steps)


## 测试代码说明

### Hello World

编译 `hello.c` 文件得到 `hello.wasm` 文件：
```bash
/opt/wasi-sdk/bin/clang -O3 -o hello.wasm hello.c
```

然后使用 `wamrc` 编译得到 `hello.aot` 文件：
```bash
wamrc --target=aarch64 --disable-simd hello.wasm -o hello.aot
```

在 OPTEE 中运行命令 `optee_wamr 1000000 ./hello.aot` 将会输出 `Hello WebAssembly!`。

### 外部函数测试(rust)

编译 `external_function.rs` 文件得到 `external_function.wasm` 文件：
```bash
rustc -C link-self-contained=no \
    -C link-args=--no-entry \
    -C link-args=-zstack-size=32768 \
    --target wasm32v1-none external_function.rs
```

然后使用 `wamrc` 编译得到 `external_function.aot` 文件：
```bash
wamrc --target=aarch64 --disable-simd external_function.wasm -o external_function.aot
```

在 OPTEE 中运行命令 `optee_wamr 1000000 ./external_function.aot` 将会在 Secure World 中输出 `Hello Rust World!`。

### Rust & 动态内存分配

编译 `wasm-rust` 项目得到 `wasm-rust.aot` 文件：

```bash
cd test/wasm-rust
./build.sh
```

引入外部库 `wee_alloc` 作为内存分配器，可以在 Rust 编译的 wasm 中使用动态内存分配。

