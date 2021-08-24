# Kefir C compiler
This repository contains results of on-going work on implementation of C11 language compiler from scratch. No existing open source compiler
infrastructure is being reused. The main priority is self-sufficiency of the project, compatibility with platform ABI and compliance with
C11 language standard.

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
|Module                     |Status         |Comments                                                                       |
|---------------------------|---------------|-------------------------------------------------------------------------------|
|Code generator             |Done           |IR translator targetting System-V AMD64 ABI                                    |
|Intermediate representation|Done           |Stack-based bytecode abstracting out calling convention and data layout details|
|AST structure and analysis |Done           |See exceptions section below                                                   |
|AST translator             |Done           |See exceptions section below                                                   |
|Parser                     |Done           |Recursive descent parser with backtracking. See exceptions section below       |
|Lexer                      |Done           |See exceptions ection below                                                    |
|Preprocessor               |Not planned    |Out-of-scope of initial effort                                                 |
|Standard library           |Not planned    |Out-of-scope of initial effort                                                 |
|Command-line interface     |In progress    |Very rudimentary implementation available                                      |


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
* Absence of multibyte and unicode characters and string literals. These features were excluded
to temporarily simplify parsing and analysis. Implementation of these is the priority after finishing
base compiler.

## Design notes
In order to simplify translation and facilitate portability, intermediate representation
(IR) layer was introduced. It defines architecture-agnostic 64-bit stack machine
bytecode, providing generic calling convention and abstracting out type layout
information. Compiler is structured into separate modules with respect to IR: code
generation and AST analysis and translation. IR layer provides several interfaces for
AST analyzer to retrieve necessary target type layout information (for instance, for
constant expression analysis). AST analysis and translation are separate stages to 
improve code structure and reusability. Parser uses recursive descent approach with back-tracking.

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