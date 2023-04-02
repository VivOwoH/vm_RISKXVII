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
    int result[3]; 
    int count = 0;

    int mode = scan_int();

    int a = scan_int();
    int b = scan_int();
    result[0] = a ^ b; // XOR
    
    int c = scan_int();
    int d = scan_int();
    result[1] = c | d; // OR

    int e = scan_int();
    int f = scan_int();
    result[2] = e & f; // AND

    if (mode == 'X')
        *ConsoleWriteSInt = result[0];
    else if (mode == 'O')
        *ConsoleWriteSInt = result[1];
    else if (mode == 'A')
        *ConsoleWriteSInt = result[2];

    return 0;
}