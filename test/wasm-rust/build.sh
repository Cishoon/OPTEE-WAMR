cargo build --release
mkdir -p out
cp target/wasm32v1-none/release/contract_rs.wasm ./out
/path/to/wamrc --target=aarch64 --disable-simd -o out/contract_rs.aot out/contract_rs.wasm 