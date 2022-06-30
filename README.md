# Kefir C compiler
This repository contains implementation of C17 language compiler from scratch. No existing open source compiler
infrastructure is being reused. The main priority is self-sufficiency of the project, compatibility with platform ABI and compliance with
C17 language standard. Some exceptions to the standard were made (see `Exceptions` section below).
At the moment, the initial scope of work is effectively finished, and the main concern is stabilization, bugfixes and UX improvements. 
Kefir supports modern x86-64 Linux and FreeBSD environments.
Compiler is also able to produce JSON streams containing program representation on various stages of compilation (tokens, AST, IR),
as well as outputting source in preprocessed form.
By default, the compiler outputs GNU As-compatible assembly (Intel syntax with prefixes).
At the moment, position-independent code generation is not supported.

### Project name
Kefir compiler is named after [fermented milk drink](https://en.wikipedia.org/wiki/Kefir), no other connotations are meant or intended.

## Motivation & goals
The main motivation of the project is deeper understanding of C programming language, as well as practical experience in
the broader scope of compiler implementation aspects. Based on this, following goals were set for the project:
* Self-sufficiency - project shall use minimal number of external dependencies. Runtime dependencies should include only C standard library
and operating system APIs.
* Compliance with C17 standard - resulting product should be reasonably compliant with language standard. All intentional deviations and exceptions
shall be described and justified. 
* Compatibility with platform ABI - produced code should adhere ABI of target platform. It should be possible to transparently link it with
code produced by commonly used compilers of the target platform.
* Reduced scope of the project - full-fledged implementation of C17 compiler is demanding task. Project scope shall be reduced so that
implementation as a pet-project is feasible. For instance, standard library implementation is currently out-of-scope.
* Portability - compiler code itself should be easily portable across different environments. Currently, the development is concentrated on
a single target platform, however it might be extended in future.

Following things are **NON-goals**:
* Performance - trying to outcompete well-established compiler backends, such as GCC backend or LLVM, is not reasonable, thus performance
was never considered a goal, even though some improvements can be occasionally made. In fact, performance is deliberately sacrificed to facilitate
implementation of other goals. 
* Compatibility with other compiler extensions - C compilers are known to include different extensions that are not described by language standard.
Some of those are implemented, however it is not project goal per se, thus there are no guarantees of extension compatibility.

Note on the language standard support: initially the compiler development was focused on C11 language standard support. The migration to C17 happened
when the original plan was mostly finished. During the migration applicable DRs from those included in C17 were considered and code was updated
accordingly. The compiler itself is still written in compliance with C11 language standard.

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
Following exceptions were made in C17 implementation:
* Absence of `_Complex` floating-point number support. This feature is not being used
particularly frequently, at the same time, it complicates target code generator.
* Absence of atomics. C17 standard defines them as optional feature, which I decided
to omit in initial implementation. Support of atomics would complicate both IR and
target code generation.
* Unicode and wide strings are supported under the assumption that source and target character sets are the same.
No re-encoding is performed.
* No `STDC` pragmas are implemented in preprocessor. Respective standard library parts are out-of-scope, thus implementing
these pragmas have no value at the moment.

### Built-ins
At the moment, Kefir supports following builtins for compatibility with GCC:
`__builtin_va_list`, `__builtin_va_start`, `__builtin_va_end`, `__builtin_va_copy`, `__builtin_va_arg`,
`__builtin_alloca`, `__builtin_alloca_with_align`, `__builtin_alloca_with_align_and_max`.

Kefir supports `__attribute__(...)` syntax on parsing level, however attributes are ignored on further
compilation stages. Presence of attribute in source code can be made a syntax error by CLI option.

### Language extensions
Several C language extensions are implemented for better compatibility with GCC. All of them are disabled by default and can
be enabled via command-line options. No specific compability guarantees are provided. Among them:
* Implicit function declarations -- if no function declaration is present at call-site, `int function_name()` is automatically
  defined. The feature was part of previous C standards, however it's absent from C11 onwards.
* `int` as implicit function return type -- function definition may omit return type, `int` will be used instead.
* Designated initializers in `fieldname:` form -- old, deprecated form which is still supported by GCC.
* Labels-as-values -- labels can be addressed with `&&` operator, gotos support arbitratry addresses in
  `goto *`  form.

Kefir also defines a few non-standard macros by default. Specifically, macros indicating data model (`__LP64__`),
endianess (`__BYTE_ORDER__` and `__ORDER_LITTLE_ENDIAN__`), as well as `__KEFIRCC__` which can be used to identify the compiler.

### Standard library
Kefir can be used along with [musl libc](https://musl.libc.org) standard library, with the exception for
`<complex.h>` and `<tgmath.h>` headers which are not available due to lacking support of `_Complex` types.

## Build & Usage
**Usage is strongly discouraged. This is experimental project which is not meant for production purposes.**

Kefir depends on following third-party components: existing C11-compatible compiler (tested with `gcc` and `clang`), `gas`, `valgrind`,
`xsltproc`, `clang-format`. After installing these components, Kefir can be built with a single command: `make ./bin/kefir -j$(nproc)`.
It is also strongly advised to run basic test suite: `make test all -j$(nproc)`.

Optionally, Kefir can be installed via: `make install DESTDIR=/opt/kefir`. Short reference on compiler options can be obtained by
running `kefir --help`.

At the moment, Kefir is automatically tested in Ubuntu 20.04 and FreeBSD 13.0 environments.
Arch Linux is used as primary development environment.

Please note, that assembly modules produced by Kefir shall be linked with `source/runtime/amd64_sysv.s` in order to produce a working
executable.

## Bootstrap
Kefir is capable of bootstraping itself (that is, compiling it's own source code). At the moment, the feature is under testing, however
stage 2 bootstrap is working well. It can be performed as follows:
```bash
make bootstrap LIBC_HEADERS="$MUSL/include" LIBC_LIBS="$MUSL/lib" -j$(nproc) # Replace $MUSL with actual path to musl installation
```

Alternatively, bootstrap can be performed manually:
```bash
# Stage 0: Build & Test initial Kefir version with system compiler.
#          Produces dynamically-linked binary in bin/kefir and
#          shared library bin/libs/libkefir.so
make test all -j$(nproc)
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)/bin/libs
# Stage 1: Use previously built Kefir to compile itself.
#          Replace $MUSL with actual path to musl installation.
#          Produces statically-linked binary bin/bootstrap1/kefir
make -f bootstrap.mk bootstrap SOURCE=$(pwd)/source HEADERS=$(pwd)/headers BOOTSTRAP=$(pwd)/bootstrap/stage1 KEFIRCC=./bin/kefir \
LIBC_HEADERS="$MUSL/include" LIBC_LIBS="$MUSL/lib" -j$(nproc)
rm -rf bin # Remove kefir version produced by stage 0
# Stage 2: Use bootstrapped Kefir version to compile itself once again.
#          Replace $MUSL with actual path to musl installation.
#          Produces statically-linked binary bin/bootstrap2/kefir
make -f bootstrap.mk bootstrap SOURCE=$(pwd)/source HEADERS=$(pwd)/headers BOOTSTRAP=$(pwd)/bootstrap/stage2 KEFIRCC=./bootstrap/stage1/kefir \
LIBC_HEADERS="$MUSL/include" LIBC_LIBS="$MUSL/lib" -j$(nproc)
# Stage 3: Diff assembly files generated by Stage 1 and Stage 2.
#          They shall be identical
./scripts/bootstrap_compare.sh bootstrap/stage1 bootstrap/stage2
```

## Test suite
Kefir relies on following tests, most of which are executed as part of CI:
* Own test suite that includes:
    - Unit tests
    - Integration tests -- each test is a self-contained program that executes some part of compilation process, produces
      a text output which is then compared to the expected.
    - System tests -- each test is a self-contained program that performs compilation process, starting from some stage
      (e.g. compiling a program defined as AST structure or IR bytecode) and produces an assembly output, which is then
      combined with the remaining part of test case containing asserts (compiled with system compiler) and executed.
    - End-to-end tests -- each test consists of multiple `*.c` files which are compiled either using system compiler
      or kefir depending on file extension. Everything is then linked together and executed.
    The test suite is executed on Linux with gcc and clang compilers and on FreeBSD with clang.
* Bootstrapping test -- kefir is used to compile itself using 2-stage bootstrap technique as described above.
* GCC Torture Suite -- `compile` & `execute` parts of GCC torture test suite are executed with kefir compiler, with some permissive options
  enabled. At the moment, out of 3481 tests, 1059 fail and 22 are skipped due to being irrelevant (e.g. SIMD or profiling test cases; there is no
  exhaustive skip list yet). All failures happen on compilation stage, no abortions occur at runtime.
  The work with torture test suite will be continued in order to reduce the number of failures. The tests are executed manually, no CI is configured yet.
* Miscallenous tests:
    - Lua test -- kefir is used to build Lua 5.4 interpreter and then Lua basic test suite is executed on
      the resulting executable 
    - [Test suite](https://github.com/protopopov1122/c-testsuite) which is a fork of [c-testsuite](https://github.com/c-testsuite/c-testsuite) is
      executed. Currently, the test suite reports 4 failures that happen due to C language extensions used in the tests. Failing test cases
      are skipped.
    - SQLite3 -- amalgamated sqlite3 version is compiled with kefir, and a manual smoke test is performed with resulting executable. Integration
      with `sqllogictest` is planned.

Own test suite is deterministic (that is, tests do not fail spuriously), however there might arise problems when executed in unusual environments.
For instance, some tests contain unicode characters and require the environment to have appropriate locale set. Also, issues with local musl 
version might cause test failures.

Currently, extension of the test suite is a major goal. It helps significantly in eliminating bugs, bringing kefir closer to C language standard support,
improving compiler UX in general.

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
* [C17 standard final working draft](https://files.lhmouse.com/standards/ISO%20C%20N2176.pdf)
* [Clarification Request Summary for C11](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2244.htm) - DRs included in C17 language standard as compared to C11.
* [System-V AMD64 ABI](https://gitlab.com/x86-psABIs/x86-64-ABI)
* [Compiler explorer](https://godbolt.org/)
* [C reference](https://en.cppreference.com/w/c)
* [AMD64 instruction set reference](https://www.amd.com/system/files/TechDocs/24594.pdf)

## Further developments
Following things can be focus of further development (in order of feasibility and priority):
* Bugfixes. Extension of test suite.
* Improvements in error reporting.
* Optimization and refactoring of compiler source.
* Implementing missing bits from `Exceptions` section, migration to C17 as target standard.
* Improvements to code generation - support PIE, extend supported platform list.
* Introduction of SSA optimization stage to code generator.
