// SLL, SLT, SLTU, SRL, SRA
// modified from example
int volatile *const ConsoleWriteSInt = (int *)0x0804;

inline int scan_int() {
    int result;
    int addr = 0x0816; // read int
    asm volatile("lw %[res], 0(%[adr])" // load 
                    : [res]"=r"(result) // output
                    : [adr]"r"(addr)); // input
    return result;
}

inline int scan_char() {
    int result;
    int addr = 0x0812; // read char
    asm volatile("lw %[res], 0(%[adr])" // load 
                    : [res]"=r"(result) // output
                    : [adr]"r"(addr)); // input
    return result;
}

inline int lshift(int num, int cnt){
    int result;
    asm volatile("sll %[res], %[num], %[nbits]"
        : [res]"=r"(result)
        : [nbits]"r"(cnt), [num]"r"(num));
    return result;
}

inline int rshift(int num, int cnt){
    int result;
    asm volatile("srl %[res], %[num], %[nbits]"
        : [res]"=r"(result)
        : [nbits]"r"(cnt), [num]"r"(num));
    return result;
}

inline int rrotate(int num, int cnt){
    int result;
    asm volatile("sra %[res], %[num], %[nbits]"
        : [res]"=r"(result)
        : [nbits]"r"(cnt), [num]"r"(num));
    return result;
}

int main() {
    int result[5]; 
    int count = 0;

    char mode = scan_char();

    int a = scan_int();
    int b = scan_int();
    result[0] = lshift(a,b); // SLL = L
    
    int c = scan_int();
    int d = scan_int();
    result[1] = (c < d) ? 1 : 0; // SLT = T
    
    unsigned int e = scan_int();
    unsigned int f = scan_int();
    result[2] = (e < f) ? 1 : 0; // SLTU = U

    int g = scan_int();
    int h = scan_int();
    result[3] = rshift(g,h); // SRL = R

    int i = scan_int();
    int j = scan_int();
    result[4] = rrotate(i,j); // SRA = A

    if (mode == 'L')
        *ConsoleWriteSInt = result[0];
    else if (mode == 'T')
        *ConsoleWriteSInt = result[1];
    else if (mode == 'U')
        *ConsoleWriteSInt = result[2];
    else if (mode == 'R')
        *ConsoleWriteSInt = result[3];
    else if (mode == 'A')
        *ConsoleWriteSInt = result[4];

    return 0;
}