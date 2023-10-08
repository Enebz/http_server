CC = gcc
CFLAGS = -Wall -Werror
LIBS = -lwsock32

TARGET_DIR = bin
SRCDIR = .
OBJDIR = obj

TARGET = EnebzHTTP

SOURCES = $(SRCDIR)/main.c \
		$(SRCDIR)/logger/logger.c \
		$(SRCDIR)/tree/tree.c \
		$(SRCDIR)/hashtable/hashtable.c \
		$(SRCDIR)/http/client.c \
		$(SRCDIR)/http/method.c \
		$(SRCDIR)/http/request.c \
		$(SRCDIR)/http/response.c \
		$(SRCDIR)/http/route.c \
		$(SRCDIR)/http/server.c \
		$(SRCDIR)/http/status.c \
		$(SRCDIR)/http/version.c \
		$(SRCDIR)/http/tokenbucket.c \

# This will take all source files and replace the .c with .o and put them in the object directory
# So the object files will remain inside the object directory
# Example:
# SOURCES = ./main.c ./tree/tree.c ./hashtable/hashtable.c
# OBJECTS = ./obj/main.o ./obj/tree/tree.o ./obj/hashtable/hashtable.o
# Make sure to avoid redundant paths in the object files so that the object file paths dont look like this:
# ./obj/./tree/tree.o
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))

# This will create the target directory if it does not exist
$(shell if not exist ${TARGET_DIR} mkdir $(TARGET_DIR))

# This will create the object directory if it does not exist
$(shell if not exist ${OBJDIR} mkdir $(OBJDIR))
$(shell if not exist $(OBJDIR)\logger mkdir $(OBJDIR)\logger)
$(shell if not exist $(OBJDIR)\tree mkdir $(OBJDIR)\tree)
$(shell if not exist $(OBJDIR)\hashtable mkdir $(OBJDIR)\hashtable)
$(shell if not exist $(OBJDIR)\http mkdir $(OBJDIR)\http)

INCLUDES = -I$(SRCDIR) \
		-I$(SRCDIR)/tree \
		-I$(SRCDIR)/hashtable \
		-I$(SRCDIR)/http


# The first rule is the default rule
# This rule will only run if any of the prequisites are newer than the target
# or if the target does not exist
# Here i am using wildcards, which will expand to all files that match the pattern in the source directory
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $^ -o $@ $(LIBS)

# The prequisite for the target is the object files
# This means, if any of the object files are newer than the target, the target will be rebuilt
# The object files will be built because the rule above is used to build them and runs first
# This will also run if the target does not exist
$(TARGET_DIR)/$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

clean:
	rmdir /s /q $(OBJDIR)

.PHONY: all
all: $(TARGET)