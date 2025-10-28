#![no_std]
#![no_main]

use core::panic::PanicInfo;

mod vga_buffer;

// Entry point of the kernel
#[no_mangle]
pub extern "C" fn _start() -> ! {
    vga_buffer::println!("Hello world!");
    
    loop {
        // Infinite loop to keep the kernel running
    }
}

/// This function is called on panic.
#[panic_handler]
fn panic(info: &PanicInfo) -> ! {
    vga_buffer::println!("{}", info);
    loop {}
}
