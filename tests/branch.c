int volatile *const ConsoleWriteSInt = (int *)0x0804;
int volatile *const ConsoleWriteUInt = (int *)0x0808; // pointer to int

void prints(char *str){
    while(*str) *((char *)0x0800) = *(str++);
}

inline int scan_int() {
    int result;
    int addr = 0x0816; // read int
    asm volatile("lw %[res], 0(%[adr])" // load 
                    : [res]"=r"(result) // output
                    : [adr]"r"(addr)); // input
    return result;
}

int main() {
    prints("Test equal/not equal:\n");
    int a = scan_int();
    int b = scan_int();
    if (a == b) {
        prints("equal!\n");
    } else {
        prints("not equal!\n");
    }

    prints("Test smaller/larger:\n");
    int c = scan_int();
    int d = scan_int();
    if (c < d) {
        prints("c < d\n");
    } else if (c > d) {
        prints("c > d\n");
    }

    prints("Test unsigned smaller/larger:\n");
    unsigned int e = scan_int();
    unsigned int f = scan_int();
    if (e < f) {
        prints("unsigned e < unsigned f\n");
    } else if (e > f) {
        prints("unsigned e > unsigned f\n");
    }

    return 0;
}