#![no_std]
#![no_main]

use core::panic::PanicInfo;

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}

extern "C" {
    fn puts(s: *const i8) -> i32;
}

#[no_mangle]
fn main() {
    let hello = "Hello Rust World!";
    let hello_ptr: *const i8 = hello.as_ptr() as *const i8;

    unsafe {
        puts(hello_ptr);
    }
}