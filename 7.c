/*
 * Advent of Code 2022 Day 7
 *
 * Solution by Luke L
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#define	ERR_MSG_START	"error: "
#define	ERR_MSG_END	"\n"
#define	PERR(...)	fprintf(stderr, ERR_MSG_START); \
			fprintf(stderr, __VA_ARGS__); \
			fprintf(stderr, ERR_MSG_END)

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
	char *name;
	long long size;
} File;

typedef struct Directory{
	char *name;
	struct Directory *parent;
	struct Directory **dirs;
	struct File **files;
	long long size;
} Directory;

void dir_add_dir(Directory *parent, Directory *child);
void dir_add_file(Directory *parent, File *to_add);
Directory *dir_find_dir(Directory *parent, const char *dir_name);
File *dir_find_file(Directory *parent, const char *file_name);

int main(void)
{
	int c;

	char buf[BUF_SIZ];
	size_t buf_len = 0;

	Directory root = {
		.name = "/",
		.parent = NULL,
		.dirs = NULL,
		.files = NULL,
		.size = 0,
	};

	Directory *working_dir = &root;

	// process command line
	while (true)
	{
		// detect command prompt
		if ((c = getchar()) != '$')
			abort();
		if ((c = getchar()) != ' ')
			abort();
		PINF("found command prompt");

		// get command
	l_get_command:
		switch (c = getchar())
		{
		case 'c':
			{
				PINF("cd");
				// cd

				// move past "d "
				scanf("d ");

				// store dir name in buf
				buf_len = 0;
				while ((c = getchar()) != '\n')
				{
					switch (c)
					{
					case ' ':
						// there shouldn't be a space in the dir name
						abort();
					default:
						// collect chars in buffer
						buf[buf_len++] = c;
						if (buf_len + 1 >= BUF_SIZ)
						{
							PERR("dir name exceeded buf size");
							abort();
						}
					}
				}
				buf[buf_len++] = '\0';
				PINF("dir name = %s", buf);

				// check for special dir names
				if (buf[0] == '/')
				{
					PINF("entering root");
					working_dir = &root;
					break;
				}
				else if (strncmp(buf, "..", 3) == 0)
				{
					PINF("going back 1 dir");
					if (working_dir->parent != NULL)
						working_dir = working_dir->parent;
					break;
				}

				// check if the dir exists
				Directory *d;
				if ((d = dir_find_dir(working_dir, buf)) == NULL)
				{
					// it doesn't, create it
					if ((d = malloc(sizeof(Directory))) == NULL)
					{
						PERR("bad malloc when creating dir");
						abort();
					}
					strncpy(d->name, buf, buf_len);
					d->dirs = NULL;
					d->files = NULL;
					d->size = 0;
				}

				// add d to working dir, then enter it
				dir_add_dir(working_dir, d);
				working_dir = d;
				break;
			}
		case 'l':
			{
				// ls
				PINF("ls");

				// move past "s\n"
				scanf("s\n");

				while (true)
				{
					c = getchar();
					if (isdigit(c))
					{
						// file found
						PINF("found file");

						// store file size in buf
						buf_len = 0;
						buf[buf_len++] = c;
						while ((c = getchar()) != ' ')
						{
							// collect chars in buffer
							buf[buf_len++] = c;
							if (buf_len + 1 >= BUF_SIZ)
							{
								PERR("file size exceeded buf size");
								abort();
							}
						}
						buf[buf_len++] = '\0';

						// pointer to string containing file size
						char *str_size = buf;
						size_t str_size_len = buf_len;

						// pointer to string containing file name
						char *str_name = buf + buf_len;
						size_t str_name_len;

						// store file name after file size in buf
						while ((c = getchar()) != '\n')
						{
							// collect chars in buffer
							buf[buf_len++] = c;
							if (buf_len + 1 >= BUF_SIZ)
							{
								PERR("file name exceeded buf size");
								abort();
							}
						}
						buf[buf_len++] = '\0';
						str_name_len = (buf + buf_len) - str_name;

						PINF("file name = %s", str_name);
						PINF("file size = %s", str_size);
						
						// check if the file exists
						File *f;
						if ((f = dir_find_file(working_dir, str_name)) == NULL)
						{
							// it doesn't, create it
							if ((f = malloc(sizeof(File))) == NULL)
							{
								PERR("bad malloc when creating file");
								abort();
							}
							if ((f->name = calloc(str_name_len, sizeof(char))) == NULL)
							{
								PERR("bad calloc when creating file name");
								abort();
							}
							strncpy(f->name, str_name, (buf + buf_len) - str_name);
							if ((f->size = atoll(str_size)) == 0)
							{
								PERR("size of file was 0");
								abort();
							}

							// add it to the working directory
							dir_add_file(working_dir, f);
						}
					}
					else if (c == 'd')
					{
						// subdirectory found
						PINF("found dir");

						// move past "ir "
						scanf("ir ");
						
						// store dir name in buf
						buf_len = 0;
						while ((c = getchar()) != '\n')
						{
							switch (c)
							{
							case ' ':
								// there shouldn't be a space in the dir name
								abort();
							default:
								// collect chars in buffer
								buf[buf_len++] = c;
								if (buf_len + 1 >= BUF_SIZ)
								{
									PERR("file name exceeded buf size");
									abort();
								}
							}
						}
						buf[buf_len++] = '\0';

						PINF("dir name = %s", buf);

						// check if the dir exists
						Directory *d;
						if ((d = dir_find_dir(working_dir, buf)) == NULL)
						{
							PINF("creating dir");
							// it doesn't, create it
							if ((d = malloc(sizeof(Directory))) == NULL)
							{
								PERR("bad malloc when creating dir");
								abort();
							}
							if ((d->name = calloc(buf_len, sizeof(char))) == NULL)
							{
								PERR("bad calloc when creating dir name");
								abort();
							}
							strncpy(d->name, buf, buf_len);
							d->dirs = NULL;
							d->files = NULL;
							d->size = 0;
							PINF("dir created");
						}

						// add d to the current working directory
						dir_add_dir(working_dir, d);
						PINF("added dir");
					}
					else if (c == '$')
					{
						PINF("get command");

						// new command
						if ((c = getchar()) != ' ')
							abort();
						goto l_get_command;
					}
					else if (c == EOF)
					{
						PINF("end of file found");
						goto l_calc_size;
					}
					else
					{
						PERR("unknown character found");
						abort();
					}
				}
				break;
			}
		}
	}
l_calc_size:
	PINF("finished reading command line log :)");
	return 0;
}

void dir_add_dir(Directory *parent, Directory *child)
{
	if (parent->dirs == NULL)
	{
		// parent has no dirs, create array
		if ((parent->dirs = calloc(2, sizeof(Directory *))) == NULL)
		{
			PERR("bad calloc when creating subdirs");
			abort();
		}
		parent->dirs[0] = child;
		parent->dirs[1] = NULL;
		return;
	}

	// find the last space in the dir array
	Directory *d;
	int i = 0;
	while ((d = parent->dirs[i]) != NULL)
		i++;
	
	// resize dir array
	if ((parent->dirs = reallocarray(parent->dirs, i + 2, sizeof(Directory *))) == NULL)
	{
		PERR("bad reallocarray when adding subdir");
		abort();
	}

	// link dirs
	child->parent = parent;
	parent->dirs[i] = child;
	parent->dirs[i + 1] = NULL;
	PINF("dir added");
}

void dir_add_file(Directory *parent, File *to_add)
{
	if (parent->files == NULL)
	{
		if ((parent->files = calloc(2, sizeof(File *))) == NULL)
		{
			PERR("bad calloc when creating files");
			abort();
		}
		parent->files[0] = to_add;
		parent->files[1] = NULL;
		return;
	}
	File *f;
	int i = 0;
	while ((f = parent->files[i]) != NULL)
		i++;
	if ((parent->files = reallocarray(parent->files, i + 2, sizeof(File *))) == NULL)
	{
		PERR("bad reallocarray when adding file");
		abort();
	}
	parent->files[i] = to_add;
	parent->files[i + 1] = NULL;
	PINF("file added");
}

Directory *dir_find_dir(Directory *parent, const char *dir_name)
{
	if (parent->dirs == NULL)
		return NULL;
	Directory *d;
	size_t name_len = strlen(dir_name);
	for (int i = 0; (d = parent->dirs[i]) != NULL; i++)
	{
		if (strncmp(d->name, dir_name, name_len) == 0)
			return d;
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
			return f;
	}
	return NULL;
}
