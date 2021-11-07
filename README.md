# malloc

## Description

The **malloc** project is about writing a dynamic allocation memory management library. So you can use it with some programs already in use without modifying them or recompiling by the following libc functions _malloc_, _free_, _realloc_, _reallocf_ and _calloc_. 

It creates the shared library `libft_malloc.so` which can be used to replace malloc in any system commands.

It exposes the following methods:
```c
/*
******* ft_free.c **********************
*/
void	free(void *ptr);

/*
******* ft_malloc.c ********************
*/
void	*malloc(size_t size);

/*
******* ft_realloc.c *******************
*/
void	*realloc(void *ptr, size_t size);

/*
******* ft_reallocf.c ******************
*/
void	*reallocf(void *ptr, size_t size);

/*
******* ft_calloc.c ********************
*/
void	*calloc(size_t num, size_t size);

/*
******* ft_mal_show_alloc_mem.c ********
*/
void	show_alloc_mem(void);

/*
******* ft_mal_show_alloc_mem_ex.c *****
*/
void	show_alloc_mem_ex(void);
```

* The **malloc()** function allocates "size" bytes of memory and returns a pointer to the allocated memory.
* The **realloc()** function tries to change the size of the allocation pointed to by "ptr" to "size", and returns "ptr". If there is not enough room to enlarge the memory allocation pointed to "ptr", **realloc()** creates a new allocation, copies as much of the old data pointed by "ptr" as will fit to the new allocation, frees the old allocation, and returns a pointer to the allocated memory.
* The **reallocf()** function is identical to **realloc()** except that it will free the passed pointer when the requested memory cannot be allocated.
* The **calloc()** function allocates memory for an array of "num" elements of "size" bytes each and returns a pointer to the allocated memory. The memory is set to zero. If "num" or "size" is 0, then calloc() returns NULL.
* The **free()** function deallocates the memory allocation pointed to by "ptr". If "ptr" is a NULL pointer, no operation is performed.
* The **show_alloc_mem()** function show visual state of the allocated memory zones.
* The **show_alloc_mem_ex()** function displays more details, for example, an hexa dump of the allocated zones.

## Build

The dynamic library will be named: **libft_malloc_$HOSTTYPE.so** and `$HOSTTYPE` will be set at `'uname -m'_'uname -s'` if the environment variable is empty. Anyway there will be a link **libft_malloc.so** that will link with the previous file.

```bash
make
make debug # compile with debug flags
make clean # Clean temporary built files
make fclean # Clean all built files
make re # remove all files and compile it again
make re_debug # remove all files and compile it again with debug flags
```

## Implementation

To understand in detail this implementation, [**please refer to the wiki**](https://github.com/fica99/malloc/wiki) of this project.

## Testing

You can change your own malloc istead of system and test it with system commands using my scripts

```bash
➜  malloc git:(main) ✗ ./run_scripts/run_linux.sh  ls     
en.subject.pdf  libft_malloc.so               libftprintf  README.md    src
include         libft_malloc_x86_64_Linux.so  Makefile     run_scripts  tmp
```
