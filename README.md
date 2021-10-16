# Kefir C compiler
This repository contains implementation of C11 language compiler from scratch. No existing open source compiler
infrastructure is being reused. The main priority is self-sufficiency of the project, compatibility with platform ABI and compliance with
C11 language standard. Some exceptions to the standard were made (see `Exceptions` section below).
At the moment, the initial scope of work is effectively finished, and the main concern is stabilization, bugfixes and UX improvements. 
Kefir supports modern x86-64 Linux and FreeBSD environments.
Compiler is also able to produce JSON streams containing program representation on various stages of compilation (tokens, AST, IR).
By default, the compiler outputs GNU As-compatible assembly (Intel syntax).
At the moment, position-independent code generation is not supported.

## Motivation & goals
The main motivation of the project is deeper understanding of C programming language, as well as practical experience in
the broader scope of compiler implementation aspects. Based on this, following goals were set for the project:
* Self-sufficiency - project shall use minimal number of external dependencies. Runtime dependencies should include only C standard library
and operating system APIs.
* Compliance with C11 standard - resulting product should be reasonably compliant with language standard. All intentional deviations and exceptions
shall be described and justified. 
* Compatibility with platform ABI - produced code should adhere ABI of target platform. It should be possible to transparently link it with
code produced by commonly used compilers of the target platform.
* Reduced scope of the project - full-fledged implementation of C11 compiler is demanding task. Project scope shall be reduced so that
implementation as a pet-project is feasible. For instance, standard library implementation is currently out-of-scope.
* Portability - compiler code itself should be easily portable across different environments. Currently, the development is concentrated on
a single target platform, however it might be extended in future.

Following things are **NON-goals**:
* Performance - trying to outcompete well-established compiler backends, such as GCC backend or LLVM, is not reasonable, thus performance
was never considered a goal, even though some improvements can be occasionally made. In fact, performance is deliberately sacrificed to facilitate
implementation of other goals. 
* Compatibility with other compiler extensions - C compilers are known to include different extensions that are not described by language standard.
Implementing some of those might be necessary to re-use third-party standard library implementations, however it is not project goal per se.

Note on the selection of C11 as language to implement: C programming language is extremely widespread. At the same time, it is relatively simple in
terms of semantics, thus making implementation of the compiler feasible. C11 standard was picked over C17 because the latter does not bring any new
features. Project can be relatively easily extended to comply with C17 once the original plan is finished.

## Progress
Table below lists progress on various compiler components. 'Implemenataion done' in the status field means that the main body of component
is implemented, however refactoring and bug-fixes are still on-going. At the moment, initial development effort is mostly finished.

|Module                     |Status                    |Comments                                                                       |
|---------------------------|--------------------------|-------------------------------------------------------------------------------|
|Code generator             |Implementation done       |IR translator targetting System-V AMD64 ABI                                    |
|Intermediate representation|Implementation done       |Stack-based bytecode abstracting out calling convention and data layout details|
|AST structure and analysis |Implementation done       |See exceptions section below                                                   |
|AST translator             |Implementation done       |See exceptions section below                                                   |
|Parser                     |Implementation done       |Recursive descent parser with backtracking. See exceptions section below       |
|Lexer                      |Implementation done       |See exceptions section below                                                   |
|Preprocessor               |Implementation done       |Basic preprocessor with no support for pragmas                                 |
|Standard library           |Third-party library       |See `Standard library` section below                                           |
|Command-line interface     |Implementation done       |Minimal useful implementation                                                  |

### Exceptions
Following exceptions were made in C11 implementation:
* Absence of `_Complex` floating-point number support. This feature is not being used
particularly frequently, at the same time, it complicates target code generator.
* Absence of thread-local storage. Even though it is implemented on AST structure
and analysis level, code generator lacks support of thread-local data. 
* Absence of atomics. C11 standard defines them as optional feature, which I decided
to omit in initial implementation. Support of atomics would complicate both IR and
target code generation.
* Absence of VLAs. C11 standard defines them as optional feature, which I decided
to omit in initial implementation. VLAs are implemented on AST level, however there is
no translator and code generator support yet.
* Unicode and wide strings are supported under the assumption that source and target character sets are the same.
No re-encoding is performed.
* No `STDC` pragmas are implemented in preprocessor. Respective standard library parts are out-of-scope, thus implementing
these pragmas have no value at the moment.

### Built-ins
At the moment, Kefir supports following builtins for compatibility with GCC:
`__builtin_va_list`, `__builtin_va_start`, `__builtin_va_end`, `__builtin_va_copy`, `__builtin_va_arg`,
`__builtin_alloca`, `__builtin_alloca_with_align`, `__builtin_alloca_with_align_and_max`

### Standard library
Quick and dirty patch for [musl libc](https://musl.libc.org) was prepared in order to make it more compatible with kefir.
The patch is available in [kefir-musl repository](https://github.com/protopopov1122/kefir-musl). Most of musl source was left
intact, introducing a few `ifdef`s disabling features not supported by Kefir. Patched library undergone minimal number of smoke tests,
thus it's not guaranteed to work for any real-world scenario.

## Build & Usage
**Usage is strongly discouraged. This is experimental project which is not meant for production purposes.**

Kefir depends on following third-party components: existing C11-compatible compiler (tested with `gcc` and `clang`), `gas`, `valgrind`,
`xsltproc`, `clang-format`. After installing these components, Kefir can be built with a single command: `make ./bin/kefir -j$(nproc)`.
It is also strongly advised to run test suite: `make test all -j$(nproc)`.

Optionally, Kefir can be installed via: `make install DESTDIR=/opt/kefir`. Short reference on compiler options can be obtained by
running `kefir --help`.

At the moment, Kefir is automatically tested in Ubuntu 20.04 and FreeBSD 13.0 environments.
Arch Linux is used as primary development environment.

Please note, that assembly modules produced by Kefir shall be linked with `source/runtime/amd64_sysv.asm` in order to produce working
executable.

## Design notes
In order to simplify translation and facilitate portability, intermediate representation
(IR) layer was introduced. It defines architecture-agnostic 64-bit stack machine
bytecode, providing generic calling convention and abstracting out type layout
information. Compiler is structured into separate modules with respect to IR: code
generation and AST analysis and translation. IR layer provides several interfaces for
AST analyzer to retrieve necessary target type layout information (for instance, for
constant expression analysis). AST analysis and translation are separate stages to 
improve code structure and reusability. Parser uses recursive descent approach with back-tracking.
Lexer was implemented before preprocessor and can be used independently of it (preprocessing stage
can be completely omitted), thus both lexer and preprocessor modules share the same lexing facilities.

## Author and license
Author: Jevgenijs Protopopovs \
License:
* Main body of the compiler - GNU GPLv3
* Runtime library - BSD 3-clause
  
## Useful links
* [C11 standard final working draft](http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf)
* [System-V AMD64 ABI](https://gitlab.com/x86-psABIs/x86-64-ABI)
* [Compiler explorer](https://godbolt.org/)
* [C reference](https://en.cppreference.com/w/c)
* [AMD64 instruction set reference](https://www.amd.com/system/files/TechDocs/24594.pdf)

## Further developments
Following things can be focus of further development (in order of feasibility and priority):
* Bugfixes.
* Improvements in error reporting.
* Optimization and refactoring of compiler source.
* Implementing missing bits from `Exceptions` section, migration to C17 as target standard.
* Improvements to code generation - support PIE, extend supported platform list.
* Introduction of SSA optimization stage to code generator.
