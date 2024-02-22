void kmain() {
    char* vid = (char*)0xb8000;

    char* str = "Hello";

    for (int i = 0; i < 5; i++) {
        vid[i * 2] = str[i];
        vid[i * 2 + 1] = 3;
    }

}