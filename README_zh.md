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

在 `./test` 目录中提供了一个示例 AoT 文件 `hello_aarch64.aot`，用于快速验证配置是否成功。运行命令 `optee_wamr 1000000 ./test/hello_aarch64.aot` 将会输出 `Hello WebAssembly!`。

更多技术细节和移植过程见[毕设 05](https://cishoon.github.io/graduation-project/05/) 和 [毕设 06](https://cishoon.github.io/graduation-project/06/)。
