#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/inotify.h>
#include <sys/time.h>
#include <linux/limits.h>
#include <unistd.h>
#define __USE_MISC
#include <dirent.h>
#include <time.h>

static char *globalJob = NULL;

const char *HELP = "Usage: remake [options] ...\n\
Options:\n\
	-D[PATH] [MAKE_JOB]       execute MAKE_JOB on changes in PATH\n\
	-R[PATH] [MAKE_JOB]       same as -D, but restarts itself";

enum Option
{
	OPT_HELP,
	OPT_WATCH,
	OPT_RESTART,
	OPT_RUN,
};

typedef struct
{
	enum Option opt;
	char *path;
	char *job;
} Argument;

typedef struct
{
	Argument *list;
	size_t len;
	size_t capacity;
} Arguments;

char *absPath(const char *relativePath)
{
	// Check if the path is already absolute
	if (relativePath[0] == '/')
	{
		// Allocate memory for the absolute path and copy the input path
		char *absPath = (char *)malloc(strlen(relativePath) + 1);
		if (absPath == NULL)
		{
			perror("malloc failed");
			return NULL;
		}
		strcpy(absPath, relativePath);
		return absPath;
	}

	// Get the current working directory
	char *cwd = getcwd(NULL, 0);
	if (cwd == NULL)
	{
		perror("getcwd failed");
		return NULL;
	}

	// Calculate the length of the absolute path
	size_t cwdLen = strlen(cwd);
	size_t relativePathLen = strlen(relativePath);
	size_t absPathLen = cwdLen + relativePathLen + 2; // +2 for the slash and null terminator

	// Allocate memory for the absolute path
	char *absPath = (char *)malloc(absPathLen);
	if (absPath == NULL)
	{
		perror("malloc failed");
		free(cwd);
		return NULL;
	}

	// Copy the current working directory to the absolute path
	strcpy(absPath, cwd);

	// Append a slash if the relative path doesn't start with one
	if (relativePath[0] != '.')
	{
		absPath[cwdLen] = '/';
		cwdLen++;
	}

	// Append the relative path
	strcpy(absPath + cwdLen, relativePath);

	// Free the memory allocated by getcwd
	free(cwd);

	return absPath;
}

void addArgument(Arguments *arguments, enum Option opt, char *path, char *job)
{
	Argument arg = {opt, .path = absPath(path), job};

	if (arguments->len >= arguments->capacity)
	{
		arguments->capacity *= 2;
		arguments->list = realloc(arguments->list, arguments->capacity * sizeof(Argument));
	}

	arguments->list[arguments->len++] = arg;
}

void freeArguments(Arguments *arguments)
{
	for (int i = 0; i < arguments->len; i++)
		free(arguments->list[i].path);

	free(arguments->list);
}

void runJob(char *job)
{
	char buf[1024];
	sprintf(buf, "make %s -s", job);
	printf("[INFO]: %s\n", buf);
	system(buf);
}

Arguments parceArgs(int argc, char *argv[])
{
	if (argc <= 1)
	{
		puts(HELP);
		exit(0);
	}

	Arguments arguments = {
			.list = calloc((argc + 1) / 2, sizeof(Argument)),
			.capacity = (argc + 1) / 2,
			.len = 0};

	for (int i = 1; i < argc; i++)
	{
		if (!strncmp(argv[i], "-D", 2))
		{
			runJob(argv[i + 1]);
			addArgument(&arguments, OPT_WATCH, argv[i] + 2, argv[i + 1]);
			i++;
		}
		else if (!strncmp(argv[i], "-R", 2))
		{
			addArgument(&arguments, OPT_RESTART, argv[i] + 2, "");
		}
		else if (!strncmp(argv[i], "-X", 2))
		{
			globalJob = argv[i] + 2;
		}

		else if (!strncmp(argv[i], "-h", 2) || !strncmp(argv[i], "--help", 6))
		{
			puts(HELP);
			freeArguments(&arguments);
			exit(0);
		}
		else
		{
			printf("%s: What the heck is this?\n", argv[i]);
		}
	}
	return arguments;
}

typedef Argument Directory;

typedef struct
{
	Directory *list;
	size_t len;
	size_t capacity;
} Directories;

void addDirectory(Directories *dirs, Directory dir)
{
	if (dirs->len >= dirs->capacity)
	{
		dirs->capacity *= 2;
		dirs->list = realloc(dirs->list, dirs->capacity * sizeof(Directory));
	}
	dirs->list[dirs->len++] = dir;
}

void recursivelyAddDirectories(Directories *dirs, Directory directory)
{
	addDirectory(dirs, directory);
	// check for directories in path
	DIR *d;
	struct dirent *dir;
	d = opendir(directory.path);
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if (dir->d_type == DT_DIR)
			{
				Directory newDirectory = {
						.path = malloc(strlen(directory.path) + strlen(dir->d_name) + 2),
						.job = directory.job,
						.opt = directory.opt};
				sprintf(newDirectory.path, "%s/%s", directory.path, dir->d_name);
				if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0)
				{
					recursivelyAddDirectories(dirs, newDirectory);
				}
			}
		}
		closedir(d);
	}
}

Directories gatherDirectories(Arguments arguments)
{
	Directories dirs = {calloc(arguments.len, sizeof(Directory)), 0, arguments.len};
	for (int i = 0; i < arguments.len; i++)
	{
		recursivelyAddDirectories(&dirs, arguments.list[i]);
	}

	return dirs;
}

void freeDirs(Directories *dirs)
{
	for (int i = 0; i < dirs->len; i++)
		free(dirs->list[i].path);
	free(dirs->list);
}

// bad function and should be refactored
void restart()
{
	system("echo 'restarting'");
	system("make dev");
}

long now()
{
	struct timeval tp;
	gettimeofday(&tp, NULL);
	return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}

int main(int argc, char *argv[])
{
	Arguments arguments = parceArgs(argc, argv);

	Directories dirs = gatherDirectories(arguments);

	// for (int i = 0; i < dirs.len; i++)
	// {
	// 	printf("[INFO]: %s\n", dirs.list[i].path);
	// }

	int inotify_fd = inotify_init();
	if (!inotify_fd)
	{
		perror("inotify_init");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < dirs.len; i++)
	{
		if (!inotify_add_watch(inotify_fd, dirs.list[i].path, IN_MODIFY | IN_CREATE | IN_DELETE))
		{
			perror("inotify_add_watch");
			exit(EXIT_FAILURE);
		}
		// printf("opt: %d, job: %s, path: %s\n", dirs.list[i].opt, dirs.list[i].job, dirs.list[i].path);
	}

	char buffer[1024];
	struct inotify_event *event;
	ssize_t length;
	int exiting = 0;
	clock_t last_event = now();
	while (!exiting)
	{
		// printf("\n1. Loop Run\n\n");
		// printf("[1CLOCK]: %ld\n", now() - last_event);
		length = read(inotify_fd, buffer, sizeof(buffer));
		// printf("[2CLOCK]: %ld\n", now() - last_event);
		if (length < 0)
		{
			perror("read");
			exit(EXIT_FAILURE);
		}

		int i = 0;
		// printf("[CLOCK]: %ld\n", last_event);
		// printf("[3CLOCK]: %ld/%d\n", now() - last_event, length > i);
		while (length > i && now() - last_event > 100)
		{
			// printf("[CLOCK]: %ld\n", last_event);
			event = (struct inotify_event *)&buffer;
			i += sizeof(struct inotify_event) + event->len;
			printf("\ntiming: %ld, Event: %s, job: %s, i: %s\n\n", last_event, event->name, dirs.list[event->wd - 1].job, dirs.list[event->wd - 1].path);
			if (dirs.list[event->wd - 1].opt == OPT_RESTART)
			{
				atexit(restart);
				exiting = 1;
				break;
			}
			else
				runJob(dirs.list[event->wd - 1].job);
			runJob(globalJob);
			last_event = now();
		}
	}

	freeDirs(&dirs);
	return EXIT_SUCCESS;
}
