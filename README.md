# QuantumOS — Операционная система на C (x86 32-bit)

Минималистичная учебная операционная система, написанная на Си и ассемблере x86 (NASM), поддерживающая стандарт загрузки Multiboot (GRUB) и запуск в QEMU.

---

## 📁 Структура проекта

```text
quantumos/
├── Makefile                          # Сценарий сборки (kernel ELF & ISO)
├── linker.ld                         # Скрипт компоновщика (линкует ядро с 1MB)
├── isodir/
│   └── boot/
│       └── grub/
│           └── grub.cfg              # Конфигурация меню GRUB
└── src/
    ├── arch/
    │   └── i386/
    │       ├── boot.asm              # Multiboot-заголовок, стек и точка входа _start
    │       ├── gdt_flush.asm         # Перезагрузка сегментных регистров CS/DS
    │       └── interrupts.asm        # Ассемблерные заглушки для прерываний (ISR 0-31, IRQ 0-15)
    ├── drivers/
    │   ├── io.h                      # Порты ввода/вывода (inb, outb, io_wait)
    │   ├── vga.h / vga.c             # VGA Text Mode (80x25), курсор, скроллинг, kprintf
    │   ├── keyboard.h / keyboard.c   # PS/2 драйвер клавиатуры (обработка скан-кодов, Shift, Caps)
    │   └── serial.h / serial.c       # COM1 последовательный порт (для отладки)
    └── kernel/
        ├── gdt.h / gdt.c             # Global Descriptor Table (сегменты ядра и пользователя)
        ├── idt.h / idt.c             # Interrupt Descriptor Table (обработка исключений CPU и IRQ)
        ├── pic.h / pic.c             # Ремаппинг контроллера прерываний 8259 PIC
        ├── shell.h / shell.c         # Интерактивная командная строка (help, clear, echo, reboot, halt)
        └── kernel.c                  # Главная функция kernel_main
```

---

## 🚀 Сборка и запуск

Все команды выполняются в директории проекта (`/mnt/c/Users/neko/Documents/quantumos`):

### 1. Сборка ядра и загрузочного ISO-образа
```bash
make
```
В результате будут созданы:
- `quantumos.bin` / `quantum.bin` — ELF-бинарник ядра.
- `quantumos.iso` / `quantum.iso` — загрузочный ISO-образ с GRUB.

### 2. Запуск в QEMU (через ISO)
```bash
make run
```
или напрямую командой:
```bash
qemu-system-i386 -cdrom quantumos.iso
```

### 3. Быстрый запуск без создания ISO (Direct Kernel Boot)
```bash
make run-kernel
```

### 4. Очистка артефактов сборки
```bash
make clean
```

---

## 💻 Встроенные команды шелла

После загрузки системы доступен терминал с командной строкой:
- `help` — список доступных команд
- `about` — информация о системе и архитектуре
- `clear` — очистить экран VGA
- `echo <текст>` — вывести сообщение на экран
- `color <0-15>` — изменить цвет шрифта консоли
- `reboot` — перезагрузить виртуальную машину
- `halt` — остановить процессор (`cli; hlt`)

---

## 🛠️ Что можно реализовать дальше:
1. **Physical & Virtual Memory Management**: аллокатор физических страниц (Bitmap / Free list) и Paging (каталоги и таблицы страниц x86).
2. **Heap Memory**: функции `kmalloc()` и `kfree()`.
3. **PIT (Programmable Interval Timer)**: прерывание таймера IRQ 0 для отсчета времени (`sleep(ms)`).
4. **Многозадачность (Multitasking)**: кооперативное или вытесняющее переключение контекста процессов (Round-Robin планировщик).
5. **Файловая система**: ramdisk (Initrd/tar) или FAT12/FAT16.
