#![no_std]
#![no_main]

use core::panic::PanicInfo;

mod vga_buffer;

// Entry point of the kernel
#[no_mangle]
pub extern "C" fn _start() -> ! {
    println!("Hello world!");
    println!("Hello world!");
    println!("Hello world!");

    let mut test_list: Vec<i32> = Vec::new();
    test_list.push(11);
    println!("Vector: {:?}", test_list);
    
    loop {
        // Infinite loop to keep the kernel running
    }
}

/// This function is called on panic.
#[panic_handler]
fn panic(info: &PanicInfo) -> ! {
    println!("{}", info);
    loop {}
}
