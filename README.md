# malloc

Custom C implementation of the malloc library functions. It creates the shared library `libft_malloc.so` which can be used to replace malloc in any system commands.

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

## Build

```bash
make
make debug # compile with debug flags
make clean # Clean temporary built files
make fclean # Clean all built files
make re # remove all files and compile it again
make re_debug # remove all files and compile it again with debug flags
```

> gcc uses -fPIC to generate position independent code (PIC) for shared libraries.

## Implementation

To understand in detail this implementation, [**please refer to the wiki**](https://github.com/fica99/malloc/wiki) of this project.

## Testing

You can change your own malloc istead of system and test it with system commands

```bash
➜  malloc git:(main) ✗ LD_PRELOAD=$PWD/libft_malloc.so ls
Makefile  README.md  en.subject.pdf  include  libft_malloc.so  libft_malloc_x86_64.so  libftprintf  src  tmp
```

## Bonus

- `calloc()`
- `reallocf()`
- Multi-thread safe with pthread
- Show hex dump of allocated zones with `show_alloc_mem_ex()`
- Optimized algorithm for cache memory in different threads

### Constants

For better performance, we preallocate heaps for small malloc calls. We define 3 heap types: TINY, SMALL, LARGE.

```c

/*
********* Chunk sizes ************
*/
static const size_t FT_MAL_TINY_CHUNK_MIN_ALLOC_SIZE = 1;
static const size_t FT_MAL_TINY_CHUNK_MAX_ALLOC_SIZE = 256;
static const size_t FT_MAL_SMALL_CHUNK_MIN_ALLOC_SIZE = FT_MAL_TINY_CHUNK_MAX_ALLOC_SIZE + 1;
static const size_t FT_MAL_SMALL_CHUNK_MAX_ALLOC_SIZE = 2048;
static const size_t FT_MAL_LARGE_CHUNK_MIN_ALLOC_SIZE = FT_MAL_SMALL_CHUNK_MAX_ALLOC_SIZE + 1;
```
