int volatile *const ConsoleWriteUInt = (int *)0x0808; // pointer to int

void prints(char *str){
    while(*str) *((char *)0x0800) = *(str++);
}

inline int malloc_mem(int value){
    int addr = 0x0830; // malloc address
    asm volatile("sw %[adr], 0(%[value])"
        : [adr]"=r"(addr) // output
        : [value]"r"(value)); // input
}

inline void free_mem(int value) {
    int addr = 0x0834 ; // free address
    asm volatile("sw %[adr], 0(%[value])" 
        : [adr]"=r"(addr) // output
        : [value]"r"(value)); // input
}

int main() {
    prints("Request to allocate 192 bytes\n");
    malloc_mem(192); // should give me b700
    prints("\n");

    prints("Request to free first bank\n");
    free_mem(0xb740);
    prints("\n");

    prints("Request to allocate 10 bytes\n");
    malloc_mem(10); // should give me b740
    prints("\n");
}