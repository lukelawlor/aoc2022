/*
 * Advent of Code 2022 Day 7
 *
 * Solution by Luke L
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define	ERR_MSG_START	"error: "
#define	ERR_MSG_END	"\n"
#define	PERR(...)	fprintf(stderr, ERR_MSG_START); \
			fprintf(stderr, __VA_ARGS__); \
			fprintf(stderr, ERR_MSG_ENG)

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

// size of buffer used to read in strings
#define	BUF_SIZ	200

typedef struct File{
	const char *name;
	long long size;
} File;

typedef struct Directory{
	const char *name;
	struct Directory *parent;
	struct Directory **dirs;
	struct File **files;
	long long size;
} Directory;

void dir_add_dir(Directory *parent, Directory *child)
{
	if (parent->dirs == NULL)
	{
		if ((parent->dirs = calloc(2, sizeof(Directory *))) == NULL)
		{
			PERR("bad calloc when creating subdirs");
			abort();
		}
		parent->dirs[0] = child;
		parent->dirs[1] = NULL;
		return;
	}
	Directory *d;
	int i = 0;
	while ((d = parent->dirs[i]) != NULL)
		i++;
	if ((parent->dirs = reallocarray(parents->dirs, i + 2, sizeof(Directory *))) == NULL)
	{
		PERR("bad reallocarray when adding subdir");
		abort();
	}
	parent->dirs[i] = child;
	parent->dirs[i + 1] = NULL;
	PINF("dir added");
}

void dir_add_file(Directory *parent, File *to_add)
{
	if (d->files == NULL)
	{
		if ((parents->files = calloc(2, sizeof(Files *))) == NULL)
		{
			PERR("bad calloc when creating files");
			abort();
		}
		parent->files[0] = f;
		parent->files[1] = NULL;
		return;
	}
	File *f;
	int i = 0;
	while ((f = parent->files[i]) != NULL)
		i++;
	if ((parent->files = reallocarray(parents->files, i + 2, sizeof(File *))) == NULL)
	{
		PERR("bad reallocarray when adding file");
		abort();
	}
	parent->files[i] = to_add;
	parent->files[i + 1] = NULL;
	PINF("file added");
}

int main(void)
{
	int c;

	char buf[BUF_SIZ];
	size_t buf_len = 0;

	Directory root = {
		.name = {"/", 1},
		.dirs = NULL,
		.files = NULL,
	};

	Directory *working_dir = &root;

	while (true)
	{
		// detect command prompt
		if ((c = getchar()) != '$')
			abort();
		if ((c = getchar()) != ' ')
			abort();

		// get command
		switch (c = getchar())
		{
		case 'c':
			{
				// cd

				// move past "d "
				getchar();
				getchar();

				// get dir name
				buf_len = 0;
				while ((c = getchar()) != '\n')
				{
					switch (c)
					{
					case ' ':
						abort();
					default:
						buf[buf_len++] = c;
						if (buf_len + 1 >= BUF_SIZ)
						{
							PERR("dir name exceeded buf size");
							abort();
						}
					}
				}

				// add null terminator
				buf[buf_len++] = '\0';

				// create dir
				Directory *d;
				if ((d = malloc(sizeof(Directory))) == NULL)
				{
					PERR("bad malloc when creating dir");
					abort();
				}
				strncpy(d->name, buf, buf_len);
				d->parent = working_dir;
				d->dirs = NULL;
				d->files = NULL;
				d->size = 0;

				// set this dir to the new working dir
				working_dir = d;
				break;
			}
		case 'l':
			{
				// ls

				// move past "s\n"
				getchar();
				getchar();

				while (true)
				{
					c = getchar();
					if (isdigit(c))
					{
						// file found
					}
					else
					{
						// subdirectory found

						// move past "dir) "
						getchar();
						getchar();
						getchar();
						getchar();
						getchar();
						
						// get dir name
						buf_len = 0;
						while ((c = getchar()) != '\n')
						{
							buf[buf_len++] = c;
							if (buf_len + 1 >= BUF_SIZ)
							{
								PERR("buf overflow reading dir name");
								abort();
							}
						}
						buf[buf_len++] = '\0';

						// create dir
						Directory *d;
						if ((d = malloc(sizeof(Directory))) == NULL)
						{
							PERR("bad malloc when creating dir");
							abort();
						}
						strncpy(d->name, buf, buf_len);
						d->parent = working_dir;
						d->dirs = NULL;
						d->files = NULL
						d->size = 0;
					}
				}
				break;
			}
		}
	}
}
