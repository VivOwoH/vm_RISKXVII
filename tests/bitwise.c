// XOR, OR, AND
int volatile *const ConsoleWriteSInt = (int *)0x0804;
char volatile *const ConsoleWriteChar = (char *)0x0800;

inline int scan_int() {
    int result;
    int addr = 0x0816; // read int
    asm volatile("lw %[res], 0(%[adr])" // load 
                    : [res]"=r"(result) // output
                    : [adr]"r"(addr)); // input
    return result;
}

int main() {
    int a = scan_int();
    int b = scan_int();
    *ConsoleWriteSInt = a ^ b; // XOR
    
    int c = scan_int();
    int d = scan_int();
    *ConsoleWriteSInt = c | d; // OR

    int e = scan_int();
    int f = scan_int();
    *ConsoleWriteSInt = e & f; // AND

    return 0;
}