#![no_std]

// 当发生 panic 时的处理函数
#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! {
    core::arch::wasm32::unreachable()
}

// 使用 wee_alloc 作为全局分配器
#[global_allocator]
static ALLOC: wee_alloc::WeeAlloc = wee_alloc::WeeAlloc::INIT;

extern crate alloc;

use alloc::string::String;

#[no_mangle]
fn test() -> *const u8 {
    let mut s: String = String::from("Hello, World!");
    s.push_str("!");
    s.as_ptr()
}

#[no_mangle]
pub fn main() {
    let _ptr = test();
    ()
}

#[no_mangle]
pub extern "C" fn memcpy(dest: *mut u8, src: *const u8, len: usize) -> *mut u8 {
    unsafe {
        let mut i = 0;
        while i < len {
            *dest.add(i) = *src.add(i);
            i += 1;
        }
    }
    dest
}