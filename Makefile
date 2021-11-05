# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: aashara- <aashara-@student.21-school.ru    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/10/18 17:48:31 by marvin            #+#    #+#              #
#    Updated: 2021/10/30 15:11:07 by aashara-         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ------------  PROJECT  ----------------------------------------------------- #

# uname -m - architecture
# uname -s - OS
ifeq ($(HOSTTYPE),)
	HOSTTYPE 	:= 		$(shell uname -m)_$(shell uname -s)
endif

NAME			:=	 	libft_malloc_$(HOSTTYPE).so
LN_NAME			:=		libft_malloc.so

# ------------  DIRECTORIES  ------------------------------------------------- #
SRC_DIR			:=		src
HDR_DIR			:=		include
TMP_DIR			:=		tmp
LIB_DIR			:=		.

# ------------  SOURCE FILES  ------------------------------------------------ #
PRE_FLS			:=		ft_mal_precomp.c

SRC_FLS			:=		$(PRE_FLS)					\
						ft_mal_arena.c				\
						ft_mal_heap.c				\
						ft_mal_chunk.c				\
						ft_free.c					\
						ft_malloc.c					\
						ft_realloc.c				\
						ft_mal_show_alloc_mem.c		\
						ft_calloc.c					\
						ft_reallocf.c				\
						ft_mal_memory.c				\
						ft_mal_print.c				\
						ft_mal_show_alloc_mem_ex.c

# ------------  LIBFTPRINTF  ------------------------------------------------- #
LFTPRINTF		:=		libftprintf.a
LFTPRINTF_DIR	:=		$(LIB_DIR)/libftprintf
LFTPRINTF_HS_DIR:=		$(LFTPRINTF_DIR)/include

# ------------  FILEPATHS  --------------------------------------------------- #
SRCS			:=		$(addprefix $(SRC_DIR)/, $(SRC_FLS))
OBJS			:=		$(patsubst $(SRC_DIR)/%.c,$(TMP_DIR)/%.o, $(SRCS))
DEPS			:=		$(OBJS:.o=.d)

# ------------  FLAGS  ------------------------------------------------------- #
CC				:=		gcc
RM				:=		rm -rf
WFLGS			:=		-Wall -Werror -Wextra
# -fPIC position independant code
CFLGS			:=		-fPIC
DFLGS			:=		-O0 -g
OFLGS			:=		-O2
EFLGS			:=		
IFLGS			:=		-I $(HDR_DIR) -I $(LFTPRINTF_HS_DIR)
# -MD deps with system headers,
# -MP dummy deps for header files (in case removing .h file)
DEPFLGS			:=		-MD -MP
LFLGS			:=		-L $(LFTPRINTF_DIR) -lftprintf -lpthread -lm

ifeq ($(findstring debug,$(MAKECMDGOALS)),debug)
	CFLGS += $(DFLGS)
else
	CFLGS += $(WFLGS) $(OFLGS)
endif

# ------------  RULES  ------------------------------------------------------- #
.PHONY: all lib debug lib_debug clean fclean re re_debug

all: lib $(NAME)

lib:
	$(MAKE) -C $(LFTPRINTF_DIR)

debug: lib_debug $(NAME)

lib_debug:
	$(MAKE) -C $(LFTPRINTF_DIR) debug

$(LFTPRINTF_DIR)/$(LFTPRINTF):
	$(MAKE) -C $(LFTPRINTF_DIR)

-include $(DEPS)
$(TMP_DIR)/%.o: $(SRC_DIR)/%.c | $(TMP_DIR)
	$(CC) $(DEPFLGS) $(EFLGS) $(CFLGS) -c -o $@ $< $(IFLGS)

$(TMP_DIR):
	mkdir -p $(TMP_DIR)

$(NAME): $(LFTPRINTF_DIR)/$(LFTPRINTF) $(OBJS)
	$(CC) -shared -o $(NAME) $(OBJS) $(LFLGS)
	$(RM) $(LN_NAME)
	ln -s $(NAME) $(LN_NAME)

clean:
	$(MAKE) -C $(LFTPRINTF_DIR) clean
	$(RM) $(TMP_DIR)

fclean:
	$(MAKE) -C $(LFTPRINTF_DIR) fclean
	$(RM) $(TMP_DIR)
	$(RM) $(LN_NAME) $(NAME)

re: fclean all

re_debug: fclean debug
