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
    int a = scan_int();
    int b = scan_int();
    *ConsoleWriteSInt = lshift(a,b); // SLL = L
    
    int c = scan_int();
    int d = scan_int();
    *ConsoleWriteSInt = (c < d) ? 1 : 0; // SLT = T
    
    unsigned int e = scan_int();
    unsigned int f = scan_int();
    *ConsoleWriteSInt = (e < f) ? 1 : 0; // SLTU = U

    int g = scan_int();
    int h = scan_int();
    *ConsoleWriteSInt = rshift(g,h); // SRL = R

    int i = scan_int();
    int j = scan_int();
    *ConsoleWriteSInt = rrotate(i,j); // SRA = A

    return 0;
}