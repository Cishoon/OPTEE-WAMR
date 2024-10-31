[中文版本](#README_zh.md) | [English Version](#README.md)

# OPTEE WAMR

This project is a trusted application based on [WaTZ](https://github.com/JamesMenetrey/unine-watz/tree/main?tab=readme-ov-file) and ported to the latest version of OPTEE. It supports running AoT format WebAssembly (Wasm) programs within a Trusted Execution Environment (TEE).

## Project Sources

This project is based on specific versions of `unine-watz`, `wasm-micro-runtime`, and `optee_os`, ensuring compatibility across different modules within the latest OPTEE environment.

- The project is based on version `a436628` of `unine-watz` (Apache License 2.0) and has removed the original project’s remote authentication components.
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
- The `heap_size` value may vary depending on the environment; in my test environment, the valid range is `[408340, 12573936]`. Generally, setting it to `1000000` provides stable performance.

A sample AoT file `hello_aarch64.aot` is provided in the `./test` directory for quick configuration verification. Run the command `optee_wamr 1000000 ./test/hello_aarch64.aot` to output `Hello WebAssembly!`.

For more technical details and information on the porting process, see [Thesis Project 05](https://cishoon.github.io/graduation-project/05/) and [Thesis Project 06](https://cishoon.github.io/graduation-project/06/).
