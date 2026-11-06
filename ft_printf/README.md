*This project has been created as part of the 42 curriculum by nide-wil.*

# ft_printf

## Description

The **ft_printf** project is a custom re-implementation of the standard C library function `printf`. Its primary goal is to gain a deep understanding of variadic functions (`stdarg.h`), complex string parsing, and raw output formatting in C.

This implementation supports basic format specifiers (`c`, `s`, `p`, `d`, `i`, `u`, `x`, `X`, `%`), as well as the first set of bonus features: full management of field minimum width, left-justification (`-`), zero-padding (`0`), and precision (`.`) across supported conversions. The codebase strictly adheres to the 42 School Norm.

## Instructions

### Compilation

The project builds the static library `libftprintf.a` using the provided `Makefile`.

* Compile the library:
  ```bash
  make
  ```

* Clean object files:
  ```bash
  make clean
  ```

* Clean object files and static library:
  ```bash
  make fclean
  ```

* Rebuild the library:
  ```bash
  make re
  ```

### Usage

To use `ft_printf` in your own C project, include the header and link the compiled library:

1. Include the header in your C source file:
   ```c
   #include "ft_printf.h"
   ```

2. Compile your program with the library:
   ```bash
   cc main.c libftprintf.a -o my_program
   ```

## Algorithm and Data Structure Justification

To maintain strict compliance with 42 School Norm constraints (such as the 5-function-per-file limit and 25-line-per-function limit) while correctly handling flag combinations, the following architecture was chosen:

### 1. Data Structure: `t_flags` & `t_number`

* **`t_flags`**: Encapsulates the parsed formatting state (`minus`, `zero`, `dot`, `width`, `precision`, `specifier`). Passing a single structure pointer across sub-routines avoids long parameter lists and keeps functions concise.
* **`t_number`**: Stores numeric conversion metadata (`value`, `sign`, `isneg`, `printsign`, `digit_len`, `width`, `base`) for integer and hexadecimal formatting. It normalizes padding and precision calculations across alignment helpers.

### 2. Algorithmic Architecture: Function Pointer Dispatch Table

Instead of a monolithic `switch` statement or an extensive `if/else` chain, the renderer relies on a **Function Pointer Array** (`t_func funcs[128]`):

* **Direct Dispatch**: The specifier character (e.g., `'d'`, `'x'`, `'s'`) indexes directly into the array (`funcs[(unsigned char)specifier]`).
* **Decoupled Logic**: Flag parsing (`parse.c`) is completely isolated from output rendering (`print_nbr.c`, `print_hex.c`, etc.).
* **Modularity**: Formatting logic for padding and precision is split into small alignment functions (`ft_print_tnum_left`, `ft_print_tnum_zero`, `ft_print_tnum_right`), ensuring every function remains under the 25-line limit.

## Testing

For quick evaluation or peer testing, create a main.c file at the root of the repository with the following code:
```c

#include "ft_printf.h"
#include <stdio.h>

int	main(void)
{
	int	ret_ft;
	int	ret_sys;

	printf("=========================================\n");
	printf("          TESTS FT_PRINTF                \n");
	printf("=========================================\n\n");

	// 1. Strings and Characters with width and '-' flag
	printf("--- [ %%c / %%s ] ---\n");
	ret_ft = ft_printf("ft_printf  : [%-10c] [%15s] [%.4s]\n", 'A', "42 School", "Hello World");
	ret_sys = printf("sys_printf : [%-10c] [%15s] [%.4s]\n", 'A', "42 School", "Hello World");
	printf("Return -> ft: %d | sys: %d\n\n", ret_ft, ret_sys);

	// 2. Integers with Precision (.), Flag 0 and Flag -
	printf("--- [ %%d / %%i ] ---\n");
	ret_ft = ft_printf("ft_printf  : [%010d] [%-10d] [%.5d] [%10.5d]\n", 42, -42, 42, 42);
	ret_sys = printf("sys_printf : [%010d] [%-10d] [%.5d] [%10.5d]\n", 42, -42, 42, 42);
	printf("Return -> ft: %d | sys: %d\n\n", ret_ft, ret_sys);

	// 3. Unsigned and Hexadecimal
	printf("--- [ %%u / %%x / %%X ] ---\n");
	ret_ft = ft_printf("ft_printf  : [%10u] [%08x] [%-8X]\n", 3000000000u, 255, 255);
	ret_sys = printf("sys_printf : [%10u] [%08x] [%-8X]\n", 3000000000u, 255, 255);
	printf("Return -> ft: %d | sys: %d\n\n", ret_ft, ret_sys);

	// 4. Pointer and NULL
	printf("--- [ %%p ] ---\n");
	ret_ft = ft_printf("ft_printf  : [%20p] [%-20p]\n", &ret_ft, NULL);
	ret_sys = printf("sys_printf : [%20p] [%-20p]\n", &ret_ft, NULL);
	printf("Return -> ft: %d | sys: %d\n\n", ret_ft, ret_sys);

	return (0);
}
```

Then compile and run it:

```bash
make
cc -Wall -Wextra -Werror main.c libftprintf.a -o test_printf
./test_printf
```

## Resources

### References & Documentation

* [Man page: printf(3)](https://man7.org/linux/man-pages/man3/printf.3.html) - Standard C printf specification.
* [stdarg.h documentation](https://en.cppreference.com/w/c/variadic) - Details on C variadic macros (`va_start`, `va_arg`, `va_end`).
* C99 Language Specification (ISO/IEC 9899:1999) - Formal standard for format specifiers and flags.

### Artificial Intelligence Usage

AI was used solely to assist in writing and formatting this `README.md` file. The code itself was written entirely by hand without AI assistance.
