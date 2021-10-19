# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: marvin <marvin@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/10/18 17:48:31 by marvin            #+#    #+#              #
#    Updated: 2021/10/18 17:48:31 by marvin           ###   ########.fr        #
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
PRE_FLS			:=		

SRC_FLS			:=		$(PRE_FLS) \


# ------------  LIBFTPRINTF  ------------------------------------------------- #
LFTPRINTF		:=		libftprintf.a
LFTPRINTF_DIR	:=		$(LIB_DIR)/libftprintf
LFTPRINTF_HS_DIR:=		$(LFT_PRINTF_DIR)/includes

# ------------  FILEPATHS  --------------------------------------------------- #
SRCS			:=		$(addprefix $(SRC_DIR)/, $(SRC_FLS))
OBJS			:=		$(patsubst $(SRC_DIR)/%.c,$(TMP_DIR)/%.o, $(SRCS))
DEPS			:=		$(OBJS:.o=.d)

# ------------  FLAGS  ------------------------------------------------------- #
CC				:=		gcc
RM				:=		rm -rf
# -fPIC position independant code
CFLGS			:=		-Wall -Werror -Wextra -fPIC
DFLGS			:=		-O0 -g
EFLGS			:=		
IFLGS			:=		-I $(HDR_DIR) -I $(LFTPRINTF_HS_DIR)
# -MD deps with system headers,
# -MP dummy deps for header files (in case removing .h file)
DEPFLGS			:=		-MD -MP
LFLGS			:=		-L $(LFTPRINTF_DIR) -lftprintf

# ------------  RULES  ------------------------------------------------------- #
.PHONY: all clean fclean re

all: lib $(NAME)

lib:
	$(MAKE) -C $(LFTPRINTF_DIR)

$(LFTPRINTF_DIR)/$(LFTPRINTF):
	$(MAKE) -C $(LFTPRINTF_DIR)

-include $(DEPS)
$(TMP_DIR)/%.o: $(SRC_DIR)/%.c | $(TMP_DIR)
	$(CC) $(DEPFLGS) $(EFLGS) $(CFLGS) $(DFLGS) -c -o $@ $< $(IFLGS)

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