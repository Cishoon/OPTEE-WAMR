[中文版本](README_zh.md) | [English Version](README.md)

# OPTEE WAMR

本项目是一个基于 [WaTZ](https://github.com/JamesMenetrey/unine-watz/tree/main?tab=readme-ov-file) 移植到最新的 OPTEE 中的可信应用程序，支持在 TEE 中运行 AoT 格式的 wasm 程序。

## 项目来源

本项目基于特定版本的 `unine-watz`、`wasm-micro-runtime` 和 `optee_os`，确保不同模块在最新的 OPTEE 环境中兼容。

* 本项目基于 `unine-watz` 的 `a436628` 版本（Apache License 2.0），删除了原项目中远程认证的部分。
* 本项目引用了 `wasm-micro-runtime` 的 `cba4c782` 版本（Apache License 2.0），用于编译得到可用的 `AoT` 文件。
* 本项目基于 `optee_os` 的 `19662e4` 版本，增加了 `mprotect` 系统调用。

项目中的 `./optee_wamr/build/libvmlib.a` 由 `unine-watz` 编译得到。

## 使用方法

1. 用 `./third_party/optee_os` 覆盖原项目中的 `optee_os`。
2. 将 `./optee_wamr` 文件夹移入 `optee_examples` 中。
3. 重新编译并运行。

进入 Normal World 后，使用以下命令运行 wasm 程序：

```
optee_wamr <heap_size> <aot_path>
```

- 使用的 `AoT` 文件必须由 `cba4c782` 版本的 `wasm-micro-runtime` 的 `wamrc` 编译得到，并指定 `--target=aarch64`。编译  `wamrc` 的方式见：[Build wamrc AOT compiler](https://github.com/bytecodealliance/wasm-micro-runtime/blob/main/wamr-compiler/README.md)
- 而用于编译 `AoT` 文件的 `wasm` 文件需要由 `wasi-sdk` 编译得到。
- `heap_size` 取值可能因环境而异，在我的测试环境下可用范围是 `[408340, 12573936]`，建议一般设置为 `1000000` 以保证稳定性。

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
    --target wasm32-wasip1 external_function.rs
```

然后使用 `wamrc` 编译得到 `external_function.aot` 文件：
```bash
wamrc --target=aarch64 --disable-simd external_function.wasm -o external_function.aot
```

在 OPTEE 中运行命令 `optee_wamr 1000000 ./external_function.aot` 将会在 Secure World 中输出 `Hello Rust World!`。
