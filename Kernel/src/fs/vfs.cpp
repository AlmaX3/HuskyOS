#include <vfs.h>
#include <hkStdio.h>
#include <string.h>
#include <heap.h>
#include <list.h>

fs_node_t *fs_root = 0;

uint32_t read_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer){
    if(node->read != 0){
        return node->read(node, offset, size, buffer);
    } else {
        return 0;
    }
}

uint32_t write_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer){
    if(node->write != 0){
        return node->write(node, offset, size, buffer);
    } else {
        return 0;
    }
}

void open_fs(fs_node_t *node, uint8_t read, uint8_t write){
    if(node->open){
        node->open(node, read, write);
    }
}

void close_fs(fs_node_t *node){
    if(node == fs_root)
        HuskyStandardOutput.assertpanic("ATTEMPTED TO CLOSE ROOT FILE SYSTEM.");
    
    if(node->close){
        node->close(node);
    }
}

struct dirent *readdir_fs(fs_node_t *node, uint32_t index) {
	if ((node->flags & FS_DIRECTORY) && node->readdir != 0) {
		struct dirent *ret = node->readdir(node, index);
		return ret;
	} else {
		HuskyStandardOutput.kprint("Found none as '%s'\n", node->name);
		return (struct dirent *)nullptr;
	}
}

fs_node_t *finddir_fs(fs_node_t *node, char *name) {
	if ((node->flags & FS_DIRECTORY) && node->finddir != 0) {
		fs_node_t *ret = node->finddir(node, name);
		return ret;
	} else {
		return (fs_node_t *)nullptr;
	}
}

int create_file_fs(char *name, uint16_t permission) {
	int32_t i = strlen(name);
	char *dir_name = (char *)malloc(i + 1);
	memcpy(dir_name, name, i);
	dir_name[i] = '\0';
	if (dir_name[i - 1] == '/')
		dir_name[i - 1] = '\0';
	if (strlen(dir_name) == 0) {
		free(dir_name);
		return 1;
	}
	for (i = strlen(dir_name) - 1; i >= 0; i--) {
		if (dir_name[i] == '/') {
			dir_name[i] = '\0';
			break;
		}
	}

	// get the parent dir node.
	fs_node_t *node;
	if (i >= 0) {
		node = kopen(dir_name, 0);
	} else {
		/* XXX This is wrong */
		node = kopen(".", 0);
	}

	if (node == nullptr) {
		free(dir_name);
		return 2;
	}

	i++;
	if ((node->flags & FS_DIRECTORY) && node->mkdir) {
		node->create(node, dir_name + i, permission);
	}

	free(node);
	free(dir_name);
	return 0;
}

int mkdir_fs(char *name, uint16_t permission) {
	int32_t i = strlen(name);
	char *dir_name = (char*)malloc(i + 1);
	memcpy(dir_name, name, i);
	dir_name[i] = '\0';
	if (dir_name[i - 1] == '/')
		dir_name[i - 1] = '\0';
	if (strlen(dir_name) == 0) {
		free(dir_name);
		return 1;
	}
	for (i = strlen(dir_name) - 1; i >= 0; i--) {
		if (dir_name[i] == '/') {
			dir_name[i] = '\0';
			break;
		}
	}

	// get the parent dir node.
	fs_node_t *node;
	if (i >= 0) {
		node = kopen(dir_name, 0);
	} else {
		node = kopen(".", 0);
	}

	if (node == nullptr) {
		HuskyStandardOutput.statuslog(0xff0000, "WARNING", "mkdir: Directory does not exist");
		free(dir_name);
		return 1;
	}

	i++;
	if ((node->flags & FS_DIRECTORY) && node->mkdir) {
		node->mkdir(node, dir_name + i, permission);
	}

	free(node);
	free(dir_name);

	return 0;
}

char *canonicalize_path(char *cwd, char *input) {
	/* This is a stack-based canonicalizer; we use a list as a stack */
	list_t *out = list_create();

	/*
	 * If we have a relative path, we need to canonicalize
	 * the working directory and insert it into the stack.
	 */
	if (strlen(input) && input[0] != PATH_SEPARATOR) {
		/* Make a copy of the working directory */
		char *path = (char*)malloc((strlen(cwd) + 1) * sizeof(char));
		memcpy(path, cwd, strlen(cwd) + 1);

		/* Setup tokenizer */
		char *pch;
		char *save;
		pch = strtok_r(path,PATH_SEPARATOR_STRING,&save);

		/* Start tokenizing */
		while (pch != nullptr) {
			/* Make copies of the path elements */
			char *s = (char*)malloc(sizeof(char) * (strlen(pch) + 1));
			memcpy(s, pch, strlen(pch) + 1);
			/* And push them */
			list_insert(out, s);
			pch = strtok_r(nullptr,PATH_SEPARATOR_STRING,&save);
		}
		free(path);
	}

	/* Similarly, we need to push the elements from the new path */
	char *path = (char*)malloc((strlen(input) + 1) * sizeof(char));
	memcpy(path, input, strlen(input) + 1);

	/* Initialize the tokenizer... */
	char *pch;
	char *save;
	pch = strtok_r(path,PATH_SEPARATOR_STRING,&save);

	/*
	 * Tokenize the path, this time, taking care to properly
	 * handle .. and . to represent up (stack pop) and current
	 * (do nothing)
	 */
	while (pch != nullptr) {
		if (!strcmp(pch,PATH_UP)) {
			/*
			 * Path = ..
			 * Pop the stack to move up a directory
			 */
			node_t * n = list_pop(out);
			if (n) {
				free(n->value);
				free(n);
			}
		} else if (!strcmp(pch,PATH_DOT)) {
			/*
			 * Path = .
			 * Do nothing
			 */
		} else {
			/*
			 * Regular path, push it
			 * XXX: Path elements should be checked for existence!
			 */
			char * s = (char*)malloc(sizeof(char) * (strlen(pch) + 1));
			memcpy(s, pch, strlen(pch) + 1);
			list_insert(out, s);
		}
		pch = strtok_r(nullptr, PATH_SEPARATOR_STRING, &save);
	}
	free(path);

	/* Calculate the size of the path string */
	size_t size = 0;
	foreach(item, out) {
		/* Helpful use of our foreach macro. */
		size += strlen((const char*)item->value) + 1;
	}

	/* join() the list */
	char *output = (char*)malloc(sizeof(char) * (size + 1));
	char *output_offset = output;
	if (size == 0) {
		/*
		 * If the path is empty, we take this to mean the root
		 * thus we synthesize a path of "/" to return.
		 */
		output = (char*)realloc(output, sizeof(char) * 2);
		output[0] = PATH_SEPARATOR;
		output[1] = '\0';
	} else {
		/* Otherwise, append each element together */
		foreach(item, out) {
			output_offset[0] = PATH_SEPARATOR;
			output_offset++;
			memcpy(output_offset, item->value, strlen((const char*)item->value) + 1);
			output_offset += strlen((const char*)item->value);
		}
	}

	/* Clean up the various things we used to get here */
	list_destroy(out);
	list_free(out);
	free(out);

	/* And return a working, absolute path */
	return output;
}

fs_node_t *get_mount_point(char * path, size_t path_depth) {
	return fs_root;
}


fs_node_t *kopen(char *filename, uint32_t flags) {
	/* Simple sanity checks that we actually have a file system */
	if (!fs_root || !filename) {
		return nullptr;
	}

	/* Reference the current working directory */
	char *cwd = "/";
	/* Canonicalize the (potentially relative) path... */
	char *path = canonicalize_path(cwd, filename);
	/* And store the length once to save recalculations */
	size_t path_len = strlen(path);

	/* If strlen(path) == 1, then path = "/"; return root */
	if (path_len == 1) {
		/* Clone the root file system node */
		fs_node_t *root_clone = (fs_node_t*)malloc(sizeof(fs_node_t));
		memcpy(root_clone, fs_root, sizeof(fs_node_t));

		/* Free the path */
		free(path);

		/* And return the clone */
		return root_clone;
	}

	/* Otherwise, we need to break the path up and start searching */
	char *path_offset = path;
	uint32_t path_depth = 0;
	while (path_offset < path + path_len) {
		/* Find each PATH_SEPARATOR */
		if (*path_offset == PATH_SEPARATOR) {
			*path_offset = '\0';
			path_depth++;
		}
		path_offset++;
	}
	/* Clean up */
	path[path_len] = '\0';
	path_offset = path + 1;

	/*
	 * At this point, the path is tokenized and path_offset points
	 * to the first token (directory) and path_depth is the number
	 * of directories in the path
	 */

	/*
	 * Dig through the (real) tree to find the file
	 */
	uint32_t depth;
	fs_node_t *node_ptr =  (fs_node_t*)malloc(sizeof(fs_node_t));
	/* Find the mountpoint for this file */
	fs_node_t *mount_point = get_mount_point(path, path_depth);
	/* Set the active directory to the mountpoint */
	memcpy(node_ptr, mount_point, sizeof(fs_node_t));
	fs_node_t *node_next = nullptr;
	for (depth = 0; depth < path_depth; ++depth) {
		/* Search the active directory for the requested directory */
		node_next = finddir_fs(node_ptr, path_offset);
		free(node_ptr);
		node_ptr = node_next;
		if (!node_ptr) {
			/* We failed to find the requested directory */
			free((void *)path);
			return nullptr;
		} else if (depth == path_depth - 1) {
			/* We found the file and are done, open the node */
			open_fs(node_ptr, 1, 0);
			free((void *)path);
			return node_ptr;
		}
		/* We are still searching... */
		path_offset += strlen(path_offset) + 1;
	}
	/* We failed to find the requested file, but our loop terminated. */
	free((void *)path);
	return nullptr;
}

