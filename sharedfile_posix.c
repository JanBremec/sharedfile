/*
 * sharedfile.c
 * Author: Your Name (c) 2025
 * Licensed under the Apache License, Version 2.0
 * https://www.apache.org/licenses/LICENSE-2.0
 */


#include <Python.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

static PyObject* shared_open(PyObject* self, PyObject* args, PyObject* kwargs) {
    const char* path;
    const char* mode = "r";
    const char* share = "r";  // Ignored on POSIX
    static char* kwlist[] = {"path", "mode", "share", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|ss", kwlist, &path, &mode, &share))
        return NULL;

    int flags = 0;
    if (strcmp(mode, "r") == 0) flags = O_RDONLY;
    else if (strcmp(mode, "r+") == 0) flags = O_RDWR;
    else if (strcmp(mode, "w") == 0) flags = O_WRONLY | O_CREAT | O_TRUNC;
    else if (strcmp(mode, "w+") == 0) flags = O_RDWR | O_CREAT | O_TRUNC;
    else if (strcmp(mode, "a") == 0) flags = O_WRONLY | O_CREAT | O_APPEND;
    else if (strcmp(mode, "a+") == 0) flags = O_RDWR | O_CREAT | O_APPEND;
    else {
        PyErr_SetString(PyExc_ValueError, "Unsupported file mode");
        return NULL;
    }

    int fd = open(path, flags, 0666);
    if (fd < 0) {
        PyErr_SetFromErrnoWithFilename(PyExc_OSError, path);
        return NULL;
    }

    // Optional: Apply advisory lock
    struct flock fl = {0};
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0; // Whole file

    // Try advisory locking (won't prevent access unless other processes cooperate)
    if (fcntl(fd, F_SETLK, &fl) == -1) {
        // Non-blocking lock failed
        // Ignore or raise warning?
    }

    FILE* fp = fdopen(fd, mode);
    if (!fp) {
        close(fd);
        PyErr_SetString(PyExc_OSError, "fdopen failed");
        return NULL;
    }

    int py_fd = fileno(fp);
    PyObject* py_file = PyFile_FromFd(py_fd, path, mode, -1, NULL, NULL, NULL, 1);
    return py_file;
}

static PyMethodDef SharedFileMethods[] = {
    {"open", (PyCFunction)shared_open, METH_VARARGS | METH_KEYWORDS, "Open file with shared access (POSIX-compatible)"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef sharedfilemodule = {
    PyModuleDef_HEAD_INIT,
    "sharedfile",
    "Cross-platform shared file access module",
    -1,
    SharedFileMethods
};

PyMODINIT_FUNC PyInit_sharedfile(void) {
    return PyModule_Create(&sharedfilemodule);
}