# Sharedfile

**Easily let multiple Python processes read from and write to the same file at once.**

A simple, cross-platform Python extension that makes it safe and easy for different processes to open and use the same file at the same time—perfect for logging, sharing data, or coordinating between programs.

## Features

- Super simple API: `sharedfile.open(path, mode="r", share="rw")`
- Works on Windows and POSIX (Linux/macOS)
- Lets you control how files are shared (Windows)
- Uses advisory locking for safe sharing on POSIX systems
- Feels just like Python’s built-in `open`

## Installation

Install directly from PyPI:

```sh
pip install sharedfile
```

[View on PyPI](https://pypi.org/project/sharedfile/)

Or build and install manually:

```sh
python setup.py build
python setup.py install
```

## Usage Example

Let two processes write to and read from the same file:

```python
import multiprocessing
import time
import sharedfile

def writer():
    with sharedfile.open("shared_test.txt", mode="w", share="rw") as f:
        for i in range(5):
            line = f"Writer line {i}\n"
            f.write(line)
            f.flush()
            print(f"[Writer] wrote: {line.strip()}")
            time.sleep(1)

def reader():
    time.sleep(0.5)  # Let writer start first
    with sharedfile.open("shared_test.txt", mode="r", share="rw") as f:
        for _ in range(5):
            f.seek(0)
            content = f.read()
            print(f"[Reader] read:\n{content.strip()}")
            time.sleep(1)

if __name__ == "__main__":
    p1 = multiprocessing.Process(target=writer)
    p2 = multiprocessing.Process(target=reader)
    p1.start()
    p2.start()
    p1.join()
    p2.join()
```

## API

```python
sharedfile.open(path, mode="r", share="rw")
```

- `path`: File path.
- `mode`: File mode (`"r"`, `"w"`, `"a"`, `"r+"`, etc.).
- `share`: (Windows only) Control who else can access the file:
    - `"r"`: Let others read
    - `"w"`: Let others write
    - `"rw"`: Let others read and write
    - (Ignored on POSIX; uses advisory locking)

Returns: A Python file-like object.

## Platform Details

- **Windows:** Uses native Win32 APIs for explicit read/write sharing.
- **POSIX:** Uses advisory `fcntl` locking for cooperative sharing.

## License

Licensed under the Apache License, Version 2.0. See [LICENSE](./LICENSE) for details.

## Author

[Jan Bremec](https://github.com/JanBremec)
