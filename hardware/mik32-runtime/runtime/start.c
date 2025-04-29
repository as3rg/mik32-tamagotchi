#include <stdint.h>
#include <string.h>

extern const uint8_t __DATA_IMAGE_START__;
extern const uint8_t __DATA_IMAGE_END__;
extern uint8_t __DATA_START__;

extern const uint8_t __RAM_TEXT_IMAGE_START__;
extern const uint8_t __RAM_TEXT_IMAGE_END__;
extern uint8_t __RAM_TEXT_START__;

extern uint8_t __SBSS_START__;
extern uint8_t __BSS_END__;

extern const uint8_t __TRAP_TEXT_START__;

extern uint8_t __global_pointer;
extern uint8_t __C_STACK_TOP__;

void __libc_init_array();
void __libc_fini_array();

extern void _enter(void) __attribute__((naked, section(".start_text")));
extern void trap_handler_raw(void) __attribute__((interrupt("machine"), section(".trap_text")));
void trap_handler(void) __attribute__((weak));

void SmallSystemInit(void) __attribute__((weak));
void SystemInit(void) __attribute__((weak));

extern void _start(void) __attribute__((noreturn));
void _Exit(int exit_code) __attribute__((noreturn, noinline));

extern int main(void);

void trap_handler_raw(){
    trap_handler();
}

void trap_handler(){
    while(1){}
}

void SmallSystemInit(void) {
    return;
}

void SystemInit(void) {
    return;
}


void _enter(void) {
    asm(    
                     ".option push\n\t"
                     ".option norelax\n\t"
                     "lui   gp, %hi(__global_pointer)\n\t"
                     "addi   gp,gp, %lo(__global_pointer)\n\t"
                     "lui   sp, %hi(__C_STACK_TOP__)\n\t"
                     "addi   sp,sp, %lo(__C_STACK_TOP__)\n\t"
                     "lui   t0, %hi(__TRAP_TEXT_START__)\n\t"
                     "addi   t0, t0, %lo(__TRAP_TEXT_START__)\n\t"
                     "csrw mtvec, t0\n\t"
                     "lui ra, %hi(_start)\n\t"
                     "jalr ra, %lo(_start)(ra)\n\t"
                    ".option pop\n\t");
}

static void _small_delay() {
    asm(    
                     "li t0, 128000\n\t"
                     "start_loop_delay:\n\t"
                     "nop\n\t"
                     "addi t0, t0, -1\n\t"
                     "bnez t0, start_loop_delay\n\t"
    );
}

void _start(void) {
    memcpy(&__DATA_START__, &__DATA_IMAGE_START__, (&__DATA_IMAGE_END__ - &__DATA_IMAGE_START__));

    memcpy(&__RAM_TEXT_START__, &__RAM_TEXT_IMAGE_START__, (&__RAM_TEXT_IMAGE_END__ - &__RAM_TEXT_IMAGE_START__));

    memset(&__SBSS_START__, 0, (&__BSS_END__ - &__SBSS_START__));
   
    _small_delay();
    SmallSystemInit();
    SystemInit();
    __libc_init_array();
    int rc = main();
    __libc_fini_array();
    _Exit(rc);
}

void _Exit(int exit_code) {
    (void)exit_code;
    while (1) {
        __asm__ volatile("wfi");
    }
}