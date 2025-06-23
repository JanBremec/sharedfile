/*
 * sharedfile.c
 * Author: Your Name (c) 2025
 * Licensed under the Apache License, Version 2.0
 * https://www.apache.org/licenses/LICENSE-2.0
 */


#include <Python.h>
#include <Windows.h>
#include <fcntl.h>
#include <io.h>

static PyObject* shared_open(PyObject* self, PyObject* args, PyObject* kwargs) {
    const char* path;
    const char* mode = "r";
    const char* share = "r";
    static char* kwlist[] = {"path", "mode", "share", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|ss", kwlist, &path, &mode, &share))
        return NULL;

    DWORD access = 0;
    DWORD creation = OPEN_EXISTING;
    DWORD share_mode = 0;

    if (strchr(mode, 'r')) access |= GENERIC_READ;
    if (strchr(mode, 'w') || strchr(mode, 'a')) access |= GENERIC_WRITE;

    if (strchr(share, 'r')) share_mode |= FILE_SHARE_READ;
    if (strchr(share, 'w')) share_mode |= FILE_SHARE_WRITE;

    if (strchr(mode, 'w')) creation = CREATE_ALWAYS;
    else if (strchr(mode, 'a')) creation = OPEN_ALWAYS;

    HANDLE h = CreateFileA(
        path,
        access,
        share_mode,
        NULL,
        creation,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (h == INVALID_HANDLE_VALUE) {
        PyErr_SetFromWindowsErrWithFilename(GetLastError(), path);
        return NULL;
    }

    int flags = 0;
    if (access & GENERIC_READ && access & GENERIC_WRITE) flags = _O_RDWR;
    else if (access & GENERIC_WRITE) flags = _O_WRONLY;
    else flags = _O_RDONLY;

    int fd = _open_osfhandle((intptr_t)h, flags);
    if (fd == -1) {
        CloseHandle(h);
        PyErr_SetString(PyExc_OSError, "Failed to convert handle to fd");
        return NULL;
    }

    FILE* fp = _fdopen(fd, mode);
    if (!fp) {
        _close(fd);
        PyErr_SetString(PyExc_OSError, "Failed to open FILE* stream");
        return NULL;
    }

    int py_fd = _fileno(fp);
    PyObject* py_file = PyFile_FromFd(py_fd, path, mode, -1, NULL, NULL, NULL, 1);
    return py_file;
}

static PyMethodDef SharedFileMethods[] = {
    {"open", (PyCFunction)shared_open, METH_VARARGS | METH_KEYWORDS, "Open file with shared access"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef sharedfilemodule = {
    PyModuleDef_HEAD_INIT,
    "sharedfile",
    "Windows shared file access module",
    -1,
    SharedFileMethods
};

PyMODINIT_FUNC PyInit_sharedfile(void) {
    return PyModule_Create(&sharedfilemodule);
}