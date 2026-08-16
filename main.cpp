volatile char* uart = (volatile char*)0x10000000;

char toAsci(unsigned long value) {
    if (value < 10) {
        return '0' + value;
    } else {
        return 'A' + (value - 10);
    }
}

extern "C" void kmain(unsigned long hartid, void* dtb) {
    const char* message = "Hello world! from hart ";
    for (const char* p = message; *p != '\0'; p++) {
        *uart = *p;
    }
    *uart = toAsci(hartid);  // Print the hart ID
    *uart = '\n';            // Print a newline

    for (;;) {
        // Infinite loop to prevent the program from exiting
    }
}
