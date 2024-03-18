#include "std/print.h"
#include "std/io.h"
#include "std/keyboard.h"
#include "std/mem.h"
#include "std/ata.h"
#include "jssey/jsmain.h"
#include "std/fs.h"

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

void user_init() {
    struct MemoryTablesList* mem = (struct MemoryTablesList*)MEM_DATA_ADDR;

    clear();

    string code = mem_10kb();
    ata_read_sectors(code, 0x4, 16);

    struct JsseyState** process_list = mem_10kb();

    struct JsseyState* main_process_state = mem_512();
    process_list[0] = main_process_state;
    mem->process_list = process_list;

    // Run main process
    main_process_state->is_initialized = false;
    main_process_state->is_running = false;
    main_process_state->is_finished = false;
    js_run(code, main_process_state, 100, false);
    
}

void user_loop() {
    struct MemoryTablesList* mem = (struct MemoryTablesList*)MEM_DATA_ADDR;
    char keyboard_symbol = kbd_symbol(mem->kbd_keycode);
    struct JsseyState* state = ((struct JsseyState**)(mem->process_list))[0];

    // update state for each process
    js_propset(state, "keycode", state->memory + 2, (double)mem->kbd_keycode, VAR_TYPE_NUMBER);
    double char_address = 0;
    u8 char_type = VAR_TYPE_POINTER;
    js_propget(state, "keyChar", state->memory + 2, &char_address, &char_type);
    *(char*)(state->memory + (int)char_address) = keyboard_symbol;

    // run a piece of code for each process
    if (state->is_finished == false)
        js_run(NULL, state, 20, false);

    if (state->is_running == false && state->is_finished == false) {
        state->is_finished = true;
        print("Process 0 exit: ");
        print(num_to_str((int)state->result, 10));
        print("  Error code: 0x");
        println(num_to_str(state->error, 16));
    }

}

void sys_loop() {
    struct MemoryTablesList* mem = (struct MemoryTablesList*)MEM_DATA_ADDR;

    mem->kbd_keycode = kbd_keycode();
}

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
    string mem = mem_512();
    println(str_concat(mem, "String ", "concatenation"));
    mem_free(mem);
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

void test_mem() {
    string test512 = mem_512();
    for (int i = 0; i<512; i++) {
        test512[i] = 'q';
    }
    test512[8] = '\0';

    string test10kb = mem_10kb();
    if (test10kb == NULL)
        println("test10kb is NULL!");

    for (int i = 0; i<512; i++) {
        test10kb[i] = 'w';
    }
    test10kb[8] = '\0';

    println(test512);
    println(test10kb);

    print(num_to_str(mem_used_size(), 10));
    print(" / ");
    println(num_to_str(mem_total_size(), 10));
    print("  ");

    print(num_to_str((int)test512, 10));
    print("  ");

    print(num_to_str((int)(string)mem_512(), 10));
    print("  ");
    print(num_to_str((int)(string)mem_512(), 10));
    print("  ");

    print(num_to_str((int)test10kb, 10));
    print("  ");

    println(num_to_str(MEM_FIRST_TABLE_ADDR, 10));
    
    print(num_to_str(mem_free(test512), 10));
    print("  ");
    println(num_to_str(mem_free(test10kb), 10));
    
    print(num_to_str((int)mem_512(), 10));
    print("  ");
    print(num_to_str((int)mem_512(), 10));
    print("  ");
    println(num_to_str((int)mem_512(), 10));

    println(num_to_str(mem_used_size(), 10));
}

void test_disk() {
    string contents = mem_512();
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
    
    // contents[0] = contents[0] < 'a' ? 'a' : contents[0] + 1;
    // contents[1] = '!';
    // contents[2] = '\0';

    // ata_write_sectors(0x3, 1, contents);    
    
    mem_free(contents);
}

void test_js() {
    struct MemoryTablesList* mem = (struct MemoryTablesList*)MEM_DATA_ADDR;
    struct JsseyState* state = ((struct JsseyState**)(mem->process_list))[0];

    string code = mem_10kb();
    ata_read_sectors(code, 0x4, 16);

    for (int i = 0; i<512; i++) {
        print(char_to_str(code[i]));
    }
    print("\n");

    for (int i=1; i>0; i++);
    clear();

    double result = js_run(code, state, 1000000000, true);
    print("End of execution. Result = ");
    println(num_to_str((int)result, 10));
    print("Used memory: ");
    println(num_to_str(mem_get_overall_used(), 10));
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

        mem_free(tmpfile);
        mem_free(contents);
    }    

    println("");

    struct FSTableItem* file = mem_512();
    string fileContent = mem_10kb();

    // println("Add new file:");

    // str_copy(file->name, "newTestFile.txt");
    // str_copy(fileContent, "This is a test file from strelka testing function.\nVam vsem pizda *&^$#@!");

    // fs_add_file(file, fileContent, 1);

    println("Get file 'newTestFile.txt'");
    fs_get_file_by_name(file, "newTestFile.txt");
    string newContent = fs_get_file_contents(file);
    println(newContent);
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

    //user_init();

    print_cursor_disable();

    // while (true) {
    //     sys_loop();
    //     user_loop();
    // }

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

    // for (int i=1; i > 0; i++);
    // for (int i=1; i > 0; i++);
    // clear();

    // test_js();

    clear();
    test_fs();

    // println("");
    // println("End of tests");

}