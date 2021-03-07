# Unbelievably Fast Delegate

[![Ubuntu Build Status](https://github.com/bitwizeshift/Delegate/workflows/Ubuntu/badge.svg?branch=master)](https://github.com/bitwizeshift/Delegate/actions?query=workflow%3AUbuntu)
[![macOS Build Status](https://github.com/bitwizeshift/Delegate/workflows/macOS/badge.svg?branch=master)](https://github.com/bitwizeshift/Delegate/actions?query=workflow%3AmacOS)
[![Windows Build Status](https://github.com/bitwizeshift/Delegate/workflows/Windows/badge.svg?branch=master)](https://github.com/bitwizeshift/Delegate/actions?query=workflow%3AWindows)
[![Coverage Status](https://coveralls.io/repos/github/bitwizeshift/Delegate/badge.svg?branch=master)](https://coveralls.io/github/bitwizeshift/Delegate?branch=master)
[![Github Issues](https://img.shields.io/github/issues/bitwizeshift/Delegate.svg)](http://github.com/bitwizeshift/Delegate/issues)
<br>
[![Github Releases](https://img.shields.io/github/v/release/bitwizeshift/Delegate.svg?include_prereleases)](https://github.com/bitwizeshift/Delegate/releases)

**Delegate** is an unbelievably fast, lightweight, and 0-overhead function
container.

## Teaser

```cpp
cpp::delegate d = cpp::bind<&std::strlen>();

assert(d("hello world") == 11u);
assert(d.has_target<&std::strlen>());
```

See the [quick start guide](#quick-start) below for how to use this type.

## Features

* [x] Uses absolutely no heap memory
* [x] Invocations of bound functions execute as fast as raw function pointers
* [x] Statically binds functions, member functions, and small callable objects
* [x] `delegate` objects are trivially copyable and trivially destructible
* [x] Written in modern <kbd>C++17</kbd>
* [x] Single-header, **header-only** solution -- easily drops into any project

## Background

The C++ standard library provides a few type-erased function containers, such as
`std::function` and `std::packaged_task`, but neither of them provide any
**guaranteed performance characteristics**. In particular, there is no telling
how large an object is, what the cost is to relocating an object, and -- more
importantly -- where that object *lives* either on the heap or on the stack.

Although type-erasure usually has a cost, the reality of most programs is that
bound functions are almost always known **at compile time**. The `delegate`
type leverages this fact to provide a fast, 0-overhead, lightweight solution.

The `delegate` object is tiny -- only 2 pointers in size, and trivially cheap
to copy and destroy. There are only two costs to this design:

1. Functions must be either statically known, or small callable objects such as
   an empty lambda
2. To create a `delegate`, you must call a `bind` function.

## Quick-Start

### Binding to delegates

To create a `cpp::delegate`, you need to pass it the result of a `cpp::bind`
call.

`cpp::bind` has overloads for the following bindable targets:

* Statically-specified functions: `cpp::bind<&std::strlen>()`
* Statically-specified members: `cpp::bind<&std::string::length>(&str)`
* Local (viewed) functors: `cpp::bind(&func)`
* Empty trivial functors: `cpp::bind<std::hash<int>>()`
* Small functors: `cpp::bind([x}{/* some lambda */})`
* Opaque function pointers: `cpp::bind( (void(*)) ::dlsym(...) )`

For example:

```cpp
std::string str{"hello world"};

cpp::delegate<long()> d = cpp::bind<&std::string::length>(&str);

assert(d() == 11);
```

Bound functions don't need to be exactly the same as the signature, as implicit
conversions are valid (such as the above example with `long` instead of
`std::string::size_type`).

Additionally, you can also leverage CTAD to deduce the function type for any
function pointers or member function pointer inputs:

```cpp
std::string str{"hello world"};

// deduces cpp::delegate<std::string::size_type()>
cpp::delegate d = cpp::bind<&std::string::length>(&str);
```

### Binding Opaque Functions

`delegate` supports binding both statically specified and opaque specified
function pointers. In general, most uses will be static -- where a user is
already aware of what the function pointer is explicitly named, such as:

```cpp
cpp::delegate d = cpp::bind<&std::strlen>();
```

However there may be cases when interacting with other APIs, such as `::dlsym`,
where the actual symbol for the function is not expressable. In such cases, the
function may be bound opaquely:

```cpp
auto func = reinterpret_cast<void(*)()>(::dlsym(...));

cpp::delegate d = cpp::bind(func);
```

Although this overload also works with statically specified functions as well,
it should be avoided for general use. Statically specifying functions helps the
compiler for the purposes of inlining. Opaque pointers, on the other hand, may
experience two separate levels of indirections for the invocation.

### Querying Bound Targets

Like `std::function`, the underlying target of the bound `delegate` may be
queried. This may be done using one of the available `cpp::delegate::has_target`
functions. Each overload of `has_target` corresponds to the same inputs as the
respective `cpp::bind` call.

```cpp
cpp::delegate d = cpp::bind<&std::strlen>();

assert( d.has_target<&std::strlen>() );
```

--------------------------------------------------------------------------------

**Note:** `has_target` will only return `true` if a target was bound using the
equivalent `cpp::bind` function. This means that a statically-specified function
bound with `cpp::bind` cannot be queried using the opaque `has_target` overload:

```cpp
cpp::delegate d = cpp::bind<&std::strlen>();

assert( not d.has_target(&std::strlen) );
```

Always make sure not to mix these up if there is a need to query targets!

## Optional Features

Although not required, **Delegate** supports custom namespaces

### Using a Custom Namespace

The `namespace` that `delegate` is defined in is configurable. By default,
it is defined in `namespace cpp`; however this can be toggled by defining
the preprocessor symbol `DELEGATE_NAMESPACE` to be the name of the desired
namespace.

This could be done either through a `#define` preprocessor directive:

```cpp
#define DELEGATE_NAMESPACE example
#include <delegate.hpp>

example::delegate d = example::bind<&std::strlen>();
```

Or it could also be defined using the compile-time definition with `-D`, such
as:

`g++ -std=c++11 -DDELEGATE_NAMESPACE=example test.cpp`

```cpp
#include <delegate.hpp>

example::delegate d = example::bind<&std::strlen>();
```

## License

This project is licensed under the extremely permissive
[Boost License](https://opensource.org/licenses/BSL-1.0):

> Boost Software License - Version 1.0 - August 17th, 2003
>
> Copyright (c) 2017, 2018, 2020-2021 Matthew Rodusek
>
> Permission is hereby granted, free of charge, to any person or organization
> obtaining a copy of the software and accompanying documentation covered by
> this license (the "Software") to use, reproduce, display, distribute,
> execute, and transmit the Software, and to prepare derivative works of the
> Software, and to permit third-parties to whom the Software is furnished to
> do so, all subject to the following:
>
> The copyright notices in the Software and this entire statement, including
> the above license grant, this restriction and the following disclaimer,
> must be included in all copies of the Software, in whole or in part, and
> all derivative works of the Software, unless such copies or derivative
> works are solely in the form of machine-executable object code generated by
> a source language processor.
>
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
> IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
> FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
> SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
> FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
> ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
> DEALINGS IN THE SOFTWARE.
