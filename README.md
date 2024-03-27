# NJU-Compilers

A Compiler of one kine of C-like language (**CMM**), we can see this language as **C--** with respect to **C++**.

## Environment

[**Ubuntu 20.04 64bit**](https://ubuntu.com)

+ [**GCC 7.5.0**](https://gcc.gnu.org)
+ [**Flex 2.6.4**](https://github.com/westes/flex)
+ [**Bison 3.5.1**](https://www.gnu.org/software/bison/)
+ [**SPIM 8.0**](http://spimsimulator.sourceforge.net/)
+ [**Python 2.7**](https://python.org)
+ [**Python 3.8**](https://python.org)

## Stages

1. **Lexical** Analysis.
   + Using **GNU Flex**, code at `lexical.l`.
2. **Syntax** Analysis.
   + Using **GNU Bison**, code at `syntax.y`.
