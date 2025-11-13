#include "std/print.h"
#include "std/io.h"
#include "std/keyboard.h"
#include "std/ata.h"
#include "std/fs.h"
#include "std/mem.h" 
#include "std/newmem.h" 
#include "wasmcool/wasmcool.h"
#include "std/string.h"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

struct MemGlobal {
    u8 kbd_keycode;
    u32 reserved;
};

struct StateGlobal {
    char symbol;
    bool initialized;
};

void test_print() {
    clear();
    println("First string");
    println("Second string");
    print("Third string ");
    print_colored("Some value", 0x7);
    print("\n");
    print("\n");
    print("\n");
    println("println");
    println("Some string...");
    string mem = malloc(512);
    println(str_concat(mem, "String ", "concatenation"));
    free(mem);
}

void test_numbers() {
    println(num_to_str(666, 10));
    println(num_to_str(-666, 10));
    println(num_to_str(2000000000, 10));
    println(num_to_str(true, 10));
    println(num_to_str(false, 10));
    println(num_to_str((int)NULL, 10));
    println("");    
}

void test_kb() {
    kbd_enable();
    print("Keyboard test: ");
    for (int i = 0; i<99999; i++)
        print_at(SCREEN_WIDTH * 0 + 20, num_to_str(kbd_keycode(), 16), 0x2);
}

// void test_mem() {
//     string test512 = malloc(512);
//     for (int i = 0; i<512; i++) {
//         test512[i] = 'q';
//     }
//     test512[8] = '\0';

//     string test10kb = malloc(10 * 1024);
//     if (test10kb == NULL)
//         println("test10kb is NULL!");

//     for (int i = 0; i<512; i++) {
//         test10kb[i] = 'w';
//     }
//     test10kb[8] = '\0';

//     println(test512);
//     println(test10kb);

//     print(num_to_str(mem_used_size(), 10));
//     print(" / ");
//     println(num_to_str(mem_total_size(), 10));
//     print("  ");

//     print(num_to_str((int)test512, 10));
//     print("  ");

//     print(num_to_str((int)(string)malloc(512), 10));
//     print("  ");
//     print(num_to_str((int)(string)malloc(512), 10));
//     print("  ");

//     print(num_to_str((int)test10kb, 10));
//     print("  ");

//     println(num_to_str(MEM_FIRST_TABLE_ADDR, 10));
    
//     print(num_to_str(free(test512), 10));
//     print("  ");
//     println(num_to_str(free(test10kb), 10));
    
//     print(num_to_str((int)malloc(512), 10));
//     print("  ");
//     print(num_to_str((int)malloc(512), 10));
//     print("  ");
//     println(num_to_str((int)malloc(512), 10));

//     println(num_to_str(mem_used_size(), 10));
// }

void test_disk() {
    string contents = malloc(512);
    print(num_to_str((int)contents, 16));
    print("  ");
    print(num_to_str(ata_has_any_disk(), 10));
    print("  ");
    println(num_to_str(ata_test_rw_bytes(), 10));
    print("  ");
    print("\n");

    ata_read_sectors(contents, 0x3, 1);

    for (int i = 0; i<512; i++) {
        print(char_to_str(contents[i]));
    }
    print("\n");
    
    contents[0] = contents[0] < 'a' ? 'a' : contents[0] + 1;
    contents[1] = '!';
    contents[2] = '\0';

    ata_write_sectors(0x3, 1, contents);    
    
    free(contents);
}

void test_fs() {
    struct FSTableItem* tmpfile;    

    println("Test fs:");

    for (int i=0; true; i++) {
        tmpfile = fs_get_file(1 + i);
        if (tmpfile->name[0] == 0) {
            break;
        }

        char* contents = fs_get_file_contents(tmpfile);
        print("~/");
        println(tmpfile->name);
        println(contents);
        println("------------");

        free(tmpfile);
        free(contents);
    }    

    println("");

    struct FSTableItem* file = malloc(512);
    string fileContent = malloc(10 * 1024);

    // println("Add new file:");

    // str_copy(file->name, "newTestFile.txt");
    // str_copy(fileContent, "This is a test file from strelka testing function.\nVam vsem pizda *&^$#@!");

    // fs_add_file(file, fileContent, 1);

    println("Get file 'newTestFile.txt'");
    fs_get_file_by_name(file, "newTestFile.txt");
    string newContent = fs_get_file_contents(file);
    println(newContent);

    println("Updating file 'newTestFile.txt'");
    newContent[0] = newContent[0] > 'z' ? 'a' : newContent[0] + 1;
    newContent[1] = '!';
    fs_update_file(file, newContent, 10);
}

void test_leb(long initial) {
    char buff[64] = {0, 0, 0, 0, 0, 0, 0, 0};

    println("Test LEB128");
    println(num_to_str(initial, 10));

    u64 uValue = 0;

    print("uleb: ");
    u8 s1 = writeULeb128(buff, initial);
    print(num_to_str(*(long*)buff, 10));
    print(";");
    println(num_to_str(s1, 10));

    u8 size = readULeb128(buff, &uValue);
    
    print(num_to_str((long)uValue, 10));
    print(";");
    println(num_to_str(size, 10));

    char buff2[64] = {0, 0, 0, 0, 0, 0, 0, 0};

    long sValue = 0;
    writeSLeb128(buff2, -initial);
    u8 size2 = readSLeb128(buff2, &sValue);

    print("sleb: ");
    print(num_to_str(sValue, 10));
    print(";");
    println(num_to_str(size2, 10));

    println("End test");
}

// void test_wsvm() {
//     u8* module = malloc(10 * 1024);
//     ata_read_sectors(module, 0x1, 1);

//     print(".");
//     println(module + 1);

//     wsvmStart(module);

//     println("OK");
// }

void test_wsvm() {
    print("Testing WASM Cool VM...");
    println("");

    u8* module = mem_512();
    ata_read_sectors(module, 0x0, 1);

    print(".");
    println(module + 1);
    
    wasm_test(module);

    println("WASM Cool VM test completed");
}

void test_newmem() {
    println("Testing new memory allocation system...");
    
    // // Print start address
    // print("Heap start address: 0x");
    // println(num_to_str((int)0x2000000, 16));
    // print("Heap size: ");
    // println(num_to_str(0x10000000, 10));
    // println("");

    print_blocks();
    
    // Allocate different sized memory blocks
    void* ptr1 = malloc(512);  // 512 bytes
    print("Allocated 512 bytes at: 0x");
    println(num_to_str((int)ptr1, 16));
    
    void* ptr2 = malloc(1024);  // 1KB
    print("Allocated 1KB at: 0x");
    println(num_to_str((int)ptr2, 16));
    
    void* ptr3 = malloc(16384);  // 16KB
    print("Allocated 16KB at: 0x");
    println(num_to_str((int)ptr3, 16));
    println("");
    
    // Fill blocks with test data
    // if (ptr1) {
    //     char* data1 = (char*)ptr1;
    //     for (int i = 0; i < 10; i++) {
    //         data1[i] = 'A' + i;
    //     }
    //     data1[10] = '\0';
    //     print("Data in first block: ");
    //     println(data1);
    // }
    
    // if (ptr2) {
    //     char* data2 = (char*)ptr2;
    //     for (int i = 0; i < 10; i++) {
    //         data2[i] = 'a' + i;
    //     }
    //     data2[10] = '\0';
    //     print("Data in second block: ");
    //     println(data2);
    // }
    
    // if (ptr3) {
    //     char* data3 = (char*)ptr3;
    //     for (int i = 0; i < 10; i++) {
    //         data3[i] = '0' + i;
    //     }
    //     data3[10] = '\0';
    //     print("Data in third block: ");
    //     println(data3);
    // }
    // println("");
    
    // Print heap information
    print("Heap used: ");
    print(num_to_str(get_heap_used(), 10));
    print(" bytes, Heap free: ");
    print(num_to_str(get_heap_free(), 10));
    println(" bytes");
    println("");
    print_blocks();
    
    // Free the first block
    print("Freeing first block at: 0x");
    println(num_to_str((int)ptr1, 16));
    free(ptr1);
    ptr1 = NULL;
    println("");
    
    // Print heap information again after freeing
    print("After freeing first block:");
    println("");
    print("Heap used: ");
    print(num_to_str(get_heap_used(), 10));
    print(" bytes, Heap free: ");
    print(num_to_str(get_heap_free(), 10));
    println(" bytes");
    println("");
    print_blocks();

    // Free the second block
    print("Freeing second block at: 0x");
    println(num_to_str((int)ptr2, 16));
    free(ptr2);
    ptr2 = NULL;
    println("");
    
    // Print heap information again after freeing
    print("After freeing second block:");
    println("");
    print("Heap used: ");
    print(num_to_str(get_heap_used(), 10));
    print(" bytes, Heap free: ");
    print(num_to_str(get_heap_free(), 10));
    println(" bytes");
    println("");
    print_blocks();
    
    print("New memory allocation test completed");
    println("");
}

void kmain() {
    println("Strelka System");
    print_cursor_enable(0, 0);
    print_cursor_set(0, 1);

    //kbd_leds(0x3);

    for (int i=0; i<999999999; i++) {}

    //kbd_leds(0x0);

    // int memsize = mem_get_size();
    // print(SCREEN_WIDTH * 2 + 0, "Memory(Mb):     / ");
    // print(SCREEN_WIDTH * 2 + 12, num_to_str(mem_get_overall_used() / 1024 / 1024, 10));
    // print(SCREEN_WIDTH * 2 + 18, memsize > 0 ? num_to_str(memsize / 1024 / 1024, 10) : ">=4Gb");

    mem_init();
    newmem_init((void*)0x2000000, 0x10000000);   // from 32mb, 256mb size

    //user_init();

    print_cursor_disable();    

    ////////////////////////////////////////////////////////////////
    // TESTS
    ////////////////////////////////////////////////////////////////

    // test_print();

    // for (int i=1; i > 0; i++);
    // for (int i=1; i > 0; i++);
    // for (int i=1; i > 0; i++);
    // clear();

    // test_numbers();

    // for (int i=1; i > 0; i++);
    // for (int i=1; i > 0; i++);
    // //for (int i=1; i > 0; i++);
    // clear();

    // test_kb();

    // //for (int i=1; i > 0; i++);
    // clear();

    // test_mem();

    // for (int i=1; i > 0; i++);
    // for (int i=1; i > 0; i++);
    // for (int i=1; i > 0; i++);
    // //for (int i=1; i > 0; i++);
    // clear();

    // test_disk();

    for (int i=1; i > 0; i++);
    for (int i=1; i > 0; i++);
    for (int i=1; i > 0; i++);
    // clear();

    // test_fs();

    clear();
    // test_leb(0);
    // test_leb(15);
    // test_leb(45600666);
    
    test_newmem();  // Testing new memory allocation system

    // for (int i=1; i > 0; i++);
    // for (int i=1; i > 0; i++);
    // for (int i=1; i > 0; i++);
    // for (int i=1; i > 0; i++);
    // for (int i=1; i > 0; i++);
    // for (int i=1; i > 0; i++);

    // clear();

    // test_wsvm();

    // print("Heap used/free: ");
    // print(num_to_str(get_heap_used(), 10));
    // print("/");
    // print(num_to_str(get_heap_free(), 10));
    // println("");

    // println("");
    // println("End of tests");

}
