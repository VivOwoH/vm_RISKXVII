int volatile *const ConsoleWriteUInt = (int *)0x0808; // pointer to int

inline int malloc(int value){
    int result;
    int addr = 0x0830; // malloc address
    asm volatile("sw %[res], 0(%[adr]), %[value]"
        : [res]"=r"(result) // output
        : [adr]"r"(addr), [value]"r"(value)); // input
    return result;
}

inline void free(int value) {
    int result;
    int addr = 0x0834 ; // free address
    asm volatile("sw %[res], 0(%[adr]), %[value]" 
        : [res]"=r"(result) // output
        : [adr]"r"(addr), [value]"r"(value)); // input
    return result;
}

int main() {
    prints("Request to allocate 192 bytes");
    *ConsoleWriteUInt = malloc(192); // should give me b700
    prints("\n");

    prints("Request to free first bank");
    free(0xb740);
    prints("\n");

    print("Request to allocate 10 bytes");
    *ConsoleWriteUInt = malloc(10); // should give me b740
    prints("\n");
}