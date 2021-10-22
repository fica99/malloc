/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mal_chunk.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/19 17:00:31 by marvin            #+#    #+#             */
/*   Updated: 2021/10/19 17:00:31 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MAL_CHUNK_H
# define FT_MAL_CHUNK_H

/*
*************************** Structures ****************************************
*/

/*
******** malloc_chunk (Chunk representations) ********
*/
/*
	This struct declaration is misleading (but accurate and necessary).
	It declares a "view" into memory allowing access to necessary
	fields at known offsets from a given base. See explanation below.
*/
typedef struct				s_ft_mal_chunk
{
	size_t					prev_size;		/* Size of previous chunk (if free).  */
	size_t					size;			/* Size in bytes, including overhead. */

	struct s_ft_mal_chunk*	fd;				/* double links -- used only if free. */
	struct s_ft_mal_chunk*	bk;

	/* Only used for large blocks: pointer to next larger size.  */
	struct s_ft_mal_chunk*	fd_nextsize;	/* double links -- used only if free. */
	struct s_ft_mal_chunk*	bk_nextsize;
}							t_s_ft_mal_chunk;
/*
	s_ft_mal_chunk details:

	(The following includes lightly edited explanations by Colin Plumb.)

	Chunks of memory are maintained using a `boundary tag' method as
	described in e.g., Knuth or Standish.  (See the paper by Paul
	Wilson ftp://ftp.cs.utexas.edu/pub/garbage/allocsrv.ps for a
	survey of such techniques.)  Sizes of free chunks are stored both
	in the front of each chunk and at the end.  This makes
	consolidating fragmented chunks into bigger chunks very fast.  The
	size fields also hold bits representing whether chunks are free or
	in use.

	An allocated chunk looks like this:
	
	chunk-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		    |             Size of previous chunk, if allocated            | |
		    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		    |             Size of chunk, in bytes                       |M|P|
	  mem-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		    |             User data starts here...                          .
		    .                                                               .
		    .                                                               .
		    .                                                               |
nextchunk-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		    |             Size of chunk                                     |
		    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	Where "chunk" is the front of the chunk for the purpose of most of
	the malloc code, but "mem" is the pointer that is returned to the
	user.  "Nextchunk" is the beginning of the next contiguous chunk.

	Chunks always begin on even word boundaries, so the mem portion
	(which is returned to the user) is also on an even word boundary, and
	thus at least double-word aligned.

	Free chunks are stored in circular doubly-linked lists, and look like this:

	chunk-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		    |             Size of previous chunk                            |
		    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	`head:' |             Size of chunk, in bytes                         |P|
	      mem-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		    |             Forward pointer to next chunk in list             |
		    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		    |             Back pointer to previous chunk in list            |
		    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		    |             Unused space (may be 0 bytes long)                .
		    .                                                               .
		    .                                                               |
nextchunk-> +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    `foot:' |             Size of chunk, in bytes                           |
	    	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

    The P (PREV_INUSE) bit, stored in the unused low-order bit of the
    chunk size (which is always a multiple of two words), is an in-use
    bit for the *previous* chunk.  If that bit is *clear*, then the
    word before the current chunk size contains the previous chunk
    size, and can be used to find the front of the previous chunk.
    The very first chunk allocated always has this bit set,
    preventing access to non-existent (or non-owned) memory. If
    prev_inuse is set for any given chunk, then you CANNOT determine
    the size of the previous chunk, and might even get a memory
    addressing fault when trying to do so.

    Note that the `foot' of the current chunk is actually represented
    as the prev_size of the NEXT chunk. This makes it easier to
    deal with alignments etc but can be very confusing when trying
    to extend or adapt this code.

    The two exceptions to all this are

     1. The special chunk `top' doesn't bother using the
	trailing size field since there is no next contiguous chunk
	that would have to index off it. After initialization, `top'
	is forced to always exist.  If it would become less than
	MINSIZE bytes long, it is replenished.

     2. Chunks allocated via mmap, which have the second-lowest-order
	bit M (IS_MMAPPED) set in their size fields.  Because they are
	allocated one-by-one, each must contain its own trailing size field.
*/
#endif
