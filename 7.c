/*
 * Advent of Code 2022 Day 7
 *
 * Solution by Luke L
 *
 * Known limitations:
 * 	Chained together hidden directories (i.e. ../../../) won't be recognized
 *	./ won't be recognized
 *
 * The program still fulfills the challenge's requirements though.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

// Error message printing
#define	ERR_MSG_START	"error: "
#define	ERR_MSG_END	"\n"
#define	PERR(...)	fprintf(stderr, ERR_MSG_START); \
			fprintf(stderr, __VA_ARGS__); \
			fprintf(stderr, ERR_MSG_END)

// Comment to disable info printing
#define	PRINT_INFO

#ifdef	PRINT_INFO
	#define	INF_MSG_START	"info: "
	#define	INF_MSG_END	"\n"
	#define	PINF(...)	printf(INF_MSG_START); \
				printf(__VA_ARGS__); \
				printf(INF_MSG_END)
#else
	#define	PINF(...)	{(void)0;}
#endif

// Prints to stdout with a specified number of whitespace characters before the message
#define	PTAB(tabs, ...)		for (int i = 0; i < tabs; i++) \
					putchar(' '); \
				printf(__VA_ARGS__); \
				putchar('\n') \

// Size of buffer used to read in strings
#define	BUF_SIZ		200

#define	MAX_SMALL_SIZ	100000LL

#define	DISK_SPACE	70000000LL

#define	MIN_FREE_SPACE	30000000LL

typedef struct{
	char *name;
	long long size;
} File;

typedef struct Directory{
	char *name;
	struct Directory *parent;
	struct Directory **dirs;
	File **files;
	long long size;
} Directory;

// Buffer for reading characters from stdin
typedef struct{
	char *dat;
	size_t len;
} Buf;

typedef struct{
	char *start;
	size_t len;
} StringView;

// Creates a new directory, calls abort() on error
Directory *dir_new(const char *name_start, size_t name_len);

// Directory populating functions
void dir_add_dir(Directory *parent, Directory *child);
void dir_add_file(Directory *parent, File *to_add);

// Directory searching functions
Directory *dir_find_dir(Directory *parent, const char *dir_name);
File *dir_find_file(Directory *parent, const char *file_name);

// Returns the size of a directory
long long dir_get_size(Directory *parent);

// Prints the files and subdirectories of a directory
void dir_list(Directory *parent, int tabs);

// Calculates the size of directories with a size <= 100000, must be called after all directory sizes have been calculated
long long dir_calc_small_size(Directory *parent);

// Returns the size of the smallest directory with a size >= min
long long dir_get_min_size(Directory *parent, long long min, long long current_min);

// Reads all characters from stdin into a buffer, starting from the len value in the buffer, and stopping once the stop char is found, calls abort() on errors
void buf_read(Buf *buf, char stop);

// Moves past characters in standard input, calls abort() if characters that are different than the ones supplied are found
void move_past(const char *str);

// Command handling algorithms
void handle_cd(Buf *buf, Directory **working_dir, Directory *root);
void handle_ls(Buf *buf, Directory **working_dir);

int main(void)
{
	// Current character being read
	int c;

	// Buffer used to read in strings
	char buf_data[BUF_SIZ];
	Buf buf = {
		.dat = buf_data,
		.len = 0,
	};

	Directory root = {
		.name = "/",
		.parent = NULL,
		.dirs = NULL,
		.files = NULL,
		.size = 0,
	};

	Directory *working_dir = &root;

	// Process commands
	while (true)
	{
		// Detect command prompt
		c = getchar();
		if (c == '$')
		{
			move_past(" ");
			PINF("found command prompt");
		}
		else if (c == EOF)
		{
			PINF("found end of file");
			break;
		}

		// Get command
		switch (c = getchar())
		{
		case 'c':
			move_past("d ");
			handle_cd(&buf, &working_dir, &root);
			break;
		case 'l':
			move_past("s\n");
			handle_ls(&buf, &working_dir);
			break;
		}
	}

	// Calculate the sizes of all directories
	PINF("calculating dir sizes...");
	dir_get_size(&root);
	
	// List root
	PINF("listing root...");
	dir_list(&root, 0);

	// Part 1 calculation
	PINF("calculating sum...");
	printf("part 1: sum = %lld\n", dir_calc_small_size(&root));

	// Part 2 calculation
	PINF("calculating whatever the part 2 thingy is...");

	long long free_space = DISK_SPACE - root.size;
	long long min_size = MIN_FREE_SPACE - free_space;

	printf("part 2:\nfree space = %lld\nspace to free = %lld\nsize of smallest deletable dir = %lld\n", free_space, min_size, dir_get_min_size(&root, min_size, LLONG_MAX));
	return 0;
}

// Creates a new directory, calls abort() on error
Directory *dir_new(const char *name_start, size_t name_len)
{
	Directory *d;
	if ((d = malloc(sizeof(Directory))) == NULL)
	{
		PERR("bad malloc when creating directory");
		abort();
	}

	// Initializing name
	if ((d->name = calloc(name_len, sizeof(char))) == NULL)
	{
		PERR("bad calloc when creating directory name");
		abort();
	}
	strncpy(d->name, name_start, name_len);

	// Default values
	d->dirs = NULL;
	d->files = NULL;

	// Note: size is calculated later with a separate function dir_get_size()
	d->size = 0;

	PINF("dir %s created", d->name);
	return d;
}

// Directory populating functions
void dir_add_dir(Directory *parent, Directory *child)
{
	PINF("adding dir %s", child->name);
	if (parent->dirs == NULL)
	{
		// Parent has no dirs, create array
		if ((parent->dirs = calloc(2, sizeof(Directory *))) == NULL)
		{
			PERR("bad calloc when creating subdirs");
			abort();
		}
		child->parent = parent;
		parent->dirs[0] = child;
		parent->dirs[1] = NULL;
		return;
	}

	// Find the last space in the dir array
	Directory *d;
	int i = 0;
	while ((d = parent->dirs[i]) != NULL)
		i++;
	
	// Resize dir array
	if ((parent->dirs = reallocarray(parent->dirs, i + 2, sizeof(Directory *))) == NULL)
	{
		PERR("bad reallocarray when adding subdir");
		abort();
	}

	// Link dirs
	child->parent = parent;
	parent->dirs[i] = child;
	parent->dirs[i + 1] = NULL;
}

void dir_add_file(Directory *parent, File *to_add)
{
	PINF("adding file %s", to_add->name);
	if (parent->files == NULL)
	{
		// Parent has no files, create array
		if ((parent->files = calloc(2, sizeof(File *))) == NULL)
		{
			PERR("bad calloc when creating files");
			abort();
		}
		parent->files[0] = to_add;
		parent->files[1] = NULL;
		return;
	}
	
	// Find the last space in file array
	File *f;
	int i = 0;
	while ((f = parent->files[i]) != NULL)
		i++;

	// Resize file array
	if ((parent->files = reallocarray(parent->files, i + 2, sizeof(File *))) == NULL)
	{
		PERR("bad reallocarray when adding file");
		abort();
	}

	// Add file
	parent->files[i] = to_add;
	parent->files[i + 1] = NULL;
}

// Directory searching functions
Directory *dir_find_dir(Directory *parent, const char *dir_name)
{
	if (parent->dirs == NULL)
		return NULL;
	Directory *d;
	size_t name_len = strlen(dir_name);
	for (int i = 0; (d = parent->dirs[i]) != NULL; i++)
	{
		if (strncmp(d->name, dir_name, name_len) == 0)
		{
			PINF("dir %s found", d->name);
			return d;
		}
	}
	return NULL;
}

File *dir_find_file(Directory *parent, const char *file_name)
{
	if (parent->files == NULL)
		return NULL;
	File *f;
	size_t name_len = strlen(file_name);
	for (int i = 0; (f = parent->files[i]) != NULL; i++)
	{
		if (strncmp(f->name, file_name, name_len) == 0)
		{
			PINF("file %s found", f->name);
			return f;
		}
	}
	return NULL;
}

long long dir_get_size(Directory *parent)
{
	PINF("calculating size of dir %s", parent->name);
	long long size = 0;

	// Sum all file sizes
	if (parent->files != NULL)
	{
		File *f;
		for (int i = 0; (f = parent->files[i]) != NULL; i++)
		{
			PINF("adding file %s with size %lld", f->name, f->size);
			size += f->size;
		}
	}

	// sum all dir sizes (recursion time!)
	if (parent->dirs != NULL)
	{
		Directory *d;
		for (int i = 0; (d = parent->dirs[i]) != NULL; i++)
		{
			dir_get_size(d);
			PINF("adding dir %s with size %lld", d->name, d->size);
			size += d->size;
		}
	}

	parent->size = size;
	PINF("size of dir %s is %lld", parent->name, size);
	return size;
}

// calculates the size of directories with a size <= 100000, must be called after all directory sizes have been calculated
long long dir_calc_small_size(Directory *parent)
{
	long long size = 0;
	if (parent->size <= MAX_SMALL_SIZ)
		size += parent->size;

	if (parent->dirs != NULL)
	{
		Directory *d;
		for (int i = 0; (d = parent->dirs[i]) != NULL; i++)
			size += dir_calc_small_size(d);
	}
	return size;
}

// Returns the size of the smallest directory with a size >= min
long long dir_get_min_size(Directory *parent, long long min, long long current_min)
{
	if (parent->size >= min)
		if (parent->size < current_min)
			current_min = parent->size;

	if (parent->dirs != NULL)
	{
		Directory *d;
		for (int i = 0; (d = parent->dirs[i]) != NULL; i++)
			current_min = dir_get_min_size(d, min, current_min);
	}
	
	return current_min;
}

void dir_list(Directory *parent, int tabs)
{
	PTAB(tabs, "dir: %s", parent->name);
	tabs += 4;
	if (parent->dirs != NULL)
	{
		Directory *d;
		for (int i = 0; (d = parent->dirs[i]) != NULL; i++)
			dir_list(d, tabs);
	}
	if (parent->files != NULL)
	{
		File *f;
		for (int i = 0; (f = parent->files[i]) != NULL; i++)
		{
			PTAB(tabs, "%s", f->name);
		}
	}
}

// Reads all characters from stdin into a buffer, starting from the len value in the buffer, and stopping once the stop char is found, calls abort() on errors
void buf_read(Buf *buf, char stop)
{
	int c;
	while ((c = getchar()) != stop)
	{
		buf->dat[buf->len++] = c;

		// Check if there's enough room to fit the next char or null terminator
		if (buf->len + 1 >= BUF_SIZ)
		{
			PERR("buffer overflowed, try increasing BUF_SIZ");
			abort();
		}
	}
	buf->dat[buf->len++] = '\0';
}

// Moves past characters in standard input, calls abort() if characters that are different than the ones supplied are found
void move_past(const char *str)
{
	if (scanf(str) == EOF)
	{
		PERR("expected \"%s\" but didn't find it", str);
		abort();
	}
}

// Command handling algorithms
void handle_cd(Buf *buf, Directory **working_dir, Directory *root)
{
	PINF("cd");

	// Store dir name in buf
	buf->len = 0;
	buf_read(buf, '\n');

	// Handle special dir names
	if (strncmp(buf->dat, "/", 2) == 0)
	{
		PINF("entering root");
		*working_dir = root;
		return;
	}
	else if (strncmp(buf->dat, "..", 3) == 0)
	{
		PINF("entering parent dir");
		if ((*working_dir)->parent != NULL)
			(*working_dir) = (*working_dir)->parent;
		else
			PINF("can't go above root");
		return;
	}

	// Check if the dir exists
	Directory *d;
	if ((d = dir_find_dir(*working_dir, buf->dat)) == NULL)
	{
		// It doesn't, create it
		Directory *d = dir_new(buf->dat, buf->len);
		dir_add_dir(*working_dir, d);
	}

	// Enter the dir
	PINF("entering dir %s", d->name);
	*working_dir = d;
}

void handle_ls(Buf *buf, Directory **working_dir)
{
	PINF("ls");

	// Continually read contents of dir until next command is reached
	int c;
	while ((c = getchar()) != '$')
	{
		if (isdigit(c))
		{
			PINF("file found");

			// Store file size in buf
			buf->len = 0;
			buf->dat[buf->len++] = c;
			buf_read(buf, ' ');

			// String containing file size
			StringView str_size = {
				.start = buf->dat,
				.len = buf->len,
			};

			// Pointer to string containing file name
			StringView str_name = {
				.start = buf->dat + buf->len,
				// length is uninitialized for now, as it isn't known yet
			};

			// store file name after file size in buf
			buf_read(buf, '\n');

			// Set file name string length
			str_name.len = (buf->dat + buf->len) - str_name.start;

			// Check if the file exists
			File *f;
			if ((f = dir_find_file(*working_dir, str_name.start)) == NULL)
			{
				// It doesn't, create it
				if ((f = malloc(sizeof(File))) == NULL)
				{
					PERR("bad malloc when creating file");
					abort();
				}
				if ((f->name = calloc(str_name.len, sizeof(char))) == NULL)
				{
					PERR("bad calloc when creating file name");
					abort();
				}
				strncpy(f->name, str_name.start, str_name.len);
				if ((f->size = atoll(str_size.start)) == 0)
				{
					PERR("size of file was 0");
					abort();
				}

				// Add it to the working directory
				dir_add_file(*working_dir, f);
			}
		}
		else if (c == 'd')
		{
			PINF("dir found");
			move_past("ir ");
			
			// Store dir name in buf
			buf->len = 0;
			buf_read(buf, '\n');

			// Check if the dir exists
			Directory *d;
			if ((d = dir_find_dir(*working_dir, buf->dat)) == NULL)
			{
				// It doesn't, create it
				Directory *d = dir_new(buf->dat, buf->len);
				dir_add_dir(*working_dir, d);
			}
		}
		else if (c == EOF)
		{
			ungetc(c, stdin);
			return;
		}
		else
		{
			PERR("unknown character found");
			abort();
		}
	}

	// A new command was found, unget the $ and return to the parent routine
	ungetc(c, stdin);
}
