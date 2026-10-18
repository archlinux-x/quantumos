#include "gdt.h"

#define GDT_ENTRIES 5

static struct gdt_entry gdt[GDT_ENTRIES];
static struct gdt_ptr gp;

extern void gdt_flush(uint32_t);

static void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    gdt[num].access = access;
}

void gdt_init(void) {
    gp.limit = (sizeof(struct gdt_entry) * GDT_ENTRIES) - 1;
    gp.base = (uint32_t)&gdt;

    // 0x00: Null segment
    gdt_set_gate(0, 0, 0, 0, 0);

    // 0x08: Kernel Code segment (base 0, limit 4GB, ring 0, executable, readable)
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    // 0x10: Kernel Data segment (base 0, limit 4GB, ring 0, writable)
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    // 0x18: User Code segment (base 0, limit 4GB, ring 3, executable, readable)
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);

    // 0x20: User Data segment (base 0, limit 4GB, ring 3, writable)
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    gdt_flush((uint32_t)&gp);
}
