import sys
from setuptools import setup, Extension

if sys.platform.startswith("win"):
    sources = ['sharedfile.c']
    libraries = ['kernel32']
else:
    sources = ['sharedfile_posix.c']
    libraries = []

sharedfile_module = Extension(
    'sharedfile',
    sources=sources,
    libraries=libraries,
)

setup(
    name='sharedfile',
    version='0.6',
    description='Cross-platform shared file open with share flags',
    long_description=open("README.md").read(),
    long_description_content_type='text/markdown',
    author='Jan Bremec',
    author_email='jan04bremec@gmail.com',
    url='https://github.com/janbremec/sharedfile',
    license='Apache-2.0',
    classifiers=[
        "Programming Language :: Python :: 3",
        "Programming Language :: C",
        "License :: OSI Approved :: Apache Software License"
    ],
    ext_modules=[sharedfile_module],
)
