# 🧠 Custom Memory Allocator (umalloc)

A lightweight custom implementation of `malloc()` and `free()` in C, designed to manage dynamic memory allocation manually. This project was built to deepen understanding of low-level memory management, pointer arithmetic, and system-level programming.

---

## 📦 Features

- Implements basic `umalloc()` and `ufree()` functionality
- Manages a fixed-size memory buffer using headers and bookkeeping
- Supports best-fit allocation strategy (or customizable)
- Includes safety checks for memory corruption and invalid frees
- Minimal external dependencies — pure C implementation

---

## 🗂️ File Overview

| File | Description |
|------|-------------|
| `main.c` | Driver program to test allocation, deallocation, and edge cases |
| `umalloc` | Core allocator implementation (entry point) |
| `umem.c` | Memory buffer and logic for block management |
| `umem.h` | Header file with memory structure definitions and function prototypes |

---

## 🛠️ Build & Run

```bash
gcc -o test_alloc main.c umem.c
./test_alloc
