// ADD, SUB
// modified from example
int volatile *const ConsoleWriteSInt = (int *)0x0804;
int volatile *const ConsoleWriteUInt = (int *)0x0808; // pointer to int

inline int scan_int() {
    int result;
    int addr = 0x0816; // read int
    asm volatile("lw %[res], 0(%[adr])" // load 
                    : [res]"=r"(result) // output
                    : [adr]"r"(addr)); // input
    return result;
}

inline int subtract(int a, int b) {
    return a - b;
}

int main() {
    int a = scan_int();
    int b = scan_int();
    *ConsoleWriteSInt = a + b;

    int c = scan_int();
    int d = scan_int();
    *ConsoleWriteUInt = subtract(c, d);
    return 0;
}