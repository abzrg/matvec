EXES := matvec_ipc matvec_serial matvec_pthread

CC := clang
CFLAGS := -Wall -Wextra -Wconversion -Wshadow -Wfatal-errors -std=c23 -pedantic

all: $(EXES)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

bear:
	command -v bear >/dev/null 2>&1 && bear -- $(MAKE) -B

clean:
	@$(RM) -rv $(EXES) ./compile_commands.json ./.cache/ ./*.dSYM/

.PHONY: clean bear all
