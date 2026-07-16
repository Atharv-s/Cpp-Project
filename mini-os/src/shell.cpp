#include "vga.h"
#include "keyboard.h"
#include "string.h"
#include "timer.h"
#include "kernel.h"

namespace {
    const size_t LINE_MAX = 128;

    void print_prompt() {
        terminal::setcolor(VGA_LIGHT_GREEN);
        terminal::write("miniOS> ");
        terminal::setcolor(VGA_LIGHT_GREY);
    }

    // Very small integer parser for the calc command
    bool parse_int(const char* s, int32_t* out) {
        bool neg = false;
        if (*s == '-') { neg = true; s++; }
        if (!*s) return false;
        int32_t val = 0;
        while (*s) {
            if (*s < '0' || *s > '9') return false;
            val = val * 10 + (*s - '0');
            s++;
        }
        *out = neg ? -val : val;
        return true;
    }

    void cmd_help() {
        terminal::write("Available commands:\n");
        terminal::write("  help          - show this list\n");
        terminal::write("  about         - about this OS\n");
        terminal::write("  clear         - clear the screen\n");
        terminal::write("  echo <text>   - print text back\n");
        terminal::write("  uptime        - show timer ticks since boot\n");
        terminal::write("  calc <a> <op> <b> - integer calculator (+ - * /)\n");
        terminal::write("  colors        - display VGA color palette\n");
        terminal::write("  reboot        - reboot the machine\n");
    }

    void cmd_about() {
        terminal::write("MiniOS - a small educational bare-metal x86 kernel\n");
        terminal::write("Written in C++, boots via GRUB Multiboot.\n");
        terminal::write("Features: GDT, IDT/ISRs, PIC, PIT timer, PS/2 keyboard, VGA text driver.\n");
    }

    void cmd_colors() {
        for (int i = 0; i < 16; i++) {
            terminal::setcolor((uint8_t)(VGA_BLACK << 4 | i));
            terminal::write_dec(i);
            terminal::write(" ");
        }
        terminal::setcolor(VGA_LIGHT_GREY);
        terminal::write("\n");
    }

    void cmd_reboot() {
        // 8042 keyboard controller reset trick
        uint8_t good = 0x02;
        while (good & 0x02) good = inb(0x64);
        outb(0x64, 0xFE);
    }

    // Splits input into up to 4 whitespace-separated tokens (in place).
    int tokenize(char* line, char* tokens[], int max_tokens) {
        int count = 0;
        char* p = line;
        while (*p && count < max_tokens) {
            while (*p == ' ') *p++ = '\0';
            if (!*p) break;
            tokens[count++] = p;
            while (*p && *p != ' ') p++;
        }
        return count;
    }

    void cmd_calc(char* tokens[], int n) {
        if (n != 3) {
            terminal::write("usage: calc <a> <op> <b>   e.g. calc 4 + 5\n");
            return;
        }
        int32_t a, b;
        if (!parse_int(tokens[0], &a) || !parse_int(tokens[2], &b)) {
            terminal::write("calc: invalid number\n");
            return;
        }
        char op = tokens[1][0];
        int32_t result = 0;
        bool ok = true;
        switch (op) {
            case '+': result = a + b; break;
            case '-': result = a - b; break;
            case '*': result = a * b; break;
            case '/':
                if (b == 0) { terminal::write("calc: division by zero\n"); ok = false; }
                else result = a / b;
                break;
            default:
                terminal::write("calc: unknown operator (use + - * /)\n");
                ok = false;
        }
        if (ok) {
            terminal::write("= ");
            if (result < 0) { terminal::putchar('-'); result = -result; }
            terminal::write_dec((uint32_t)result);
            terminal::write("\n");
        }
    }

    void run_command(char* line) {
        char* tokens[8];
        // Keep a separate copy for commands that need the raw remainder (echo)
        if (strncmp(line, "echo ", 5) == 0) {
            terminal::write(line + 5);
            terminal::write("\n");
            return;
        }
        int n = tokenize(line, tokens, 8);
        if (n == 0) return;

        if (strcmp(tokens[0], "help") == 0) cmd_help();
        else if (strcmp(tokens[0], "about") == 0) cmd_about();
        else if (strcmp(tokens[0], "clear") == 0) terminal::clear();
        else if (strcmp(tokens[0], "colors") == 0) cmd_colors();
        else if (strcmp(tokens[0], "reboot") == 0) cmd_reboot();
        else if (strcmp(tokens[0], "uptime") == 0) {
            terminal::write("ticks: ");
            terminal::write_dec(timer_ticks());
            terminal::write("\n");
        }
        else if (strcmp(tokens[0], "calc") == 0) cmd_calc(tokens + 1, n - 1);
        else {
            terminal::write("Unknown command: ");
            terminal::write(tokens[0]);
            terminal::write("  (type 'help' for a list)\n");
        }
    }
}

void shell_run() {
    terminal::setcolor(VGA_LIGHT_CYAN);
    terminal::write("\nWelcome to MiniOS!\n");
    terminal::write("Type 'help' to see available commands.\n\n");
    terminal::setcolor(VGA_LIGHT_GREY);

    char line[LINE_MAX];
    size_t len = 0;

    print_prompt();
    while (true) {
        char c = keyboard_getchar();
        if (c == '\n') {
            terminal::putchar('\n');
            line[len] = '\0';
            run_command(line);
            len = 0;
            print_prompt();
        } else if (c == '\b') {
            if (len > 0) {
                len--;
                terminal::backspace();
            }
        } else if (len < LINE_MAX - 1) {
            line[len++] = c;
            terminal::putchar(c);
        }
    }
}
