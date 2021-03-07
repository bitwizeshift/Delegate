#include <delegate.hpp>

#include <memory>
#include <catch2/catch.hpp>

#if defined(_MSC_VER)
# pragma warning(push)
// MSVC warns on implicit casts for the covariant functions. Since this is a
// test, this diagnostic is useless
# pragma warning(disable:4267)
#endif

namespace DELEGATE_NAMESPACE_INTERNAL {
inline namespace bitwizeshift {
namespace test {
namespace {

auto square(int x) -> int {
  return x * x;
}
auto square_out(int& x) -> int {
  return (x = (x * x));
}

struct adder
{
  int x;

  auto set(int y) -> int {
    return (x = y);
  }

  auto add(int y) const -> int
  {
    return x + y;
  }
  auto out_add(int* out, int y) const -> int
  {
    return ((*out) = (x + y));
  }
  auto operator()(int y) const -> int {
    return x + y;
  }
};

struct loader
{
  template <typename T>
  auto operator()(T* out, const T& in) const -> T&
  {
    return ((*out) = in);
  }
};

struct square_out_functor
{
  auto operator()(int* out, int x) -> int
  {
    return ((*out) = (x * x));
  }
};

} // namespace

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

TEST_CASE("delegate::delegate()") {
  const auto sut = delegate<void()>{};

  SECTION("Creates unbound delegate") {
    REQUIRE(!static_cast<bool>(sut));
  }
  SECTION("Throws when invoked") {
    REQUIRE_THROWS_AS(sut(), bad_delegate_call);
  }
}

TEST_CASE("delegate::delegate(function_bind_target<Fn>)") {
  SECTION("Function being bound has same signature") {
    const delegate sut = bind<&square>();

    SECTION("Calls bound function") {
      REQUIRE(sut(2) == 4);
    }
    SECTION("Has bound function") {
      REQUIRE(sut.has_target<&square>());
    }
  }

  SECTION("Function being bound has similar signature") {
    const delegate<long(long)> sut = bind<&square>();

    SECTION("Calls bound function") {
      REQUIRE(sut(2) == 4);
    }
    SECTION("Has bound function") {
      REQUIRE(sut.has_target<&square>());
    }
  }

  SECTION("Delegate returns void") {
    delegate<void(int&)> sut = bind<&square_out>();

    SECTION("Calls bound function") {
      auto output = 2;
      sut(output);
      REQUIRE(output == 4);
    }
    SECTION("Has bound function") {
      REQUIRE(sut.has_target<&square_out>());
    }
  }
}

//------------------------------------------------------------------------------

TEST_CASE("delegate::delegate(member_bind_target<MemberFunction,T>)") {
  SECTION("Function being bound has same signature") {
    auto a = adder{42};

    const delegate sut = bind<&adder::set>(&a);

    SECTION("Calls bound function") {
      REQUIRE(sut(2) == 2);
    }
    SECTION("Has bound function") {
      REQUIRE(sut.has_target<&adder::set>(&a));
    }
  }

  SECTION("Function being bound has similar signature") {
    auto a = adder{42};

    const delegate<long(long)> sut = bind<&adder::set>(&a);

    SECTION("Calls bound function") {
      REQUIRE(sut(2) == 2);
    }
    SECTION("Has bound function") {
      REQUIRE(sut.has_target<&adder::set>(&a));
    }
  }

  SECTION("Delegate returns void") {
    auto a = adder{42};

    const delegate<void(int)> sut = bind<&adder::set>(&a);

    SECTION("Calls bound function") {
      const auto input = 2;
      sut(input);
      REQUIRE(a.x == input);
    }
    SECTION("Has bound function") {
      REQUIRE(sut.has_target<&adder::set>(&a));
    }
  }
}

//------------------------------------------------------------------------------

TEST_CASE("delegate::delegate(member_bind_target<MemberFunction, const T>)") {
  SECTION("Function being bound has same signature") {
    const auto x = 42;
    const auto a = adder{x};

    const delegate sut = bind<&adder::add>(&a);

    SECTION("Calls bound function") {
      REQUIRE(sut(2) == 44);
    }
    SECTION("Has bound function") {
      REQUIRE(sut.has_target<&adder::add>(&a));
    }
  }

  SECTION("Function being bound has similar signature") {
    const auto x = 42;
    const auto a = adder{x};

    const delegate<long(long)> sut = bind<&adder::add>(&a);

    SECTION("Calls bound function") {
      REQUIRE(sut(2) == 44);
    }
    SECTION("Has bound function") {
      REQUIRE(sut.has_target<&adder::add>(&a));
    }
  }

  SECTION("Delegate returns void") {
    const auto x = 42;
    const auto a = adder{x};

    const delegate<void(int*, int)> sut = bind<&adder::out_add>(&a);

    SECTION("Calls bound function") {
      auto out = 0;
      sut(&out, 2);
      REQUIRE(out == 44);
    }
    SECTION("Has bound function") {
      REQUIRE(sut.has_target<&adder::out_add>(&a));
    }
  }
}

//------------------------------------------------------------------------------

TEST_CASE("delegate::delegate(callable_ref_bind_target<Callable>)") {
  SECTION("Function being bound has same signature") {
    const auto x = 42;
    auto a = adder{x};

    const delegate<int(int)> sut = bind(&a);

    SECTION("Calls bound function") {
      REQUIRE(sut(2) == 44);
    }
    SECTION("Has bound function") {
      REQUIRE(sut.has_target(&a));
    }
  }

  SECTION("Function being bound has similar signature") {
    const auto x = 42;
    auto a = adder{x};

    const delegate<long(long)> sut = bind(&a);

    SECTION("Calls bound function") {
      REQUIRE(sut(2) == 44);
    }
    SECTION("Has bound function") {
      REQUIRE(sut.has_target(&a));
    }
  }

  SECTION("Delegate returns void") {
    auto a = loader{};

    const delegate<void(int*, int)> sut = bind(&a);

    SECTION("Calls bound function") {
      auto out = 0;
      const auto expected = 2;

      sut(&out, expected);

      REQUIRE(out == expected);
    }
    SECTION("Has bound function") {
      REQUIRE(sut.has_target(&a));
    }
  }
}

//------------------------------------------------------------------------------

TEST_CASE("delegate::delegate(callable_ref_bind_target<const Callable>)") {
  SECTION("Function being bound has same signature") {
    const auto x = 42;
    const auto a = adder{x};

    const delegate<int(int)> sut = bind(&a);

    SECTION("Calls bound function") {
      REQUIRE(sut(2) == 44);
    }
    SECTION("Has bound function") {
      REQUIRE(sut.has_target(&a));
    }
  }

  SECTION("Function being bound has similar signature") {
    const auto x = 42;
    const auto a = adder{x};

    const delegate<long(long)> sut = bind(&a);

    SECTION("Calls bound function") {
      REQUIRE(sut(2) == 44);
    }
    SECTION("Has bound function") {
      REQUIRE(sut.has_target(&a));
    }
  }

  SECTION("Delegate returns void") {
    const auto a = loader{};

    const delegate<void(int*, int)> sut = bind(&a);

    SECTION("Calls bound function") {
      auto out = 0;
      const auto expected = 2;

      sut(&out, expected);

      REQUIRE(out == expected);
    }
    SECTION("Has bound function") {
      REQUIRE(sut.has_target(&a));
    }
  }
}

//------------------------------------------------------------------------------

TEST_CASE("delegate::delegate(empty_callable_target<Fn>)") {
  SECTION("Target is bound statically") {
    SECTION("Function being bound has same signature") {
      const delegate<std::size_t(int)> sut = bind<std::hash<int>>();

      SECTION("Calls bound function") {
        REQUIRE(sut(42) == std::hash<int>{}(42));
      }
      SECTION("Has bound function") {
        REQUIRE(sut.has_target<std::hash<int>>());
      }
    }

    SECTION("Function being bound has similar signature") {
      const delegate<long long(int)> sut = bind<std::hash<int>>();

      SECTION("Calls bound function") {
        REQUIRE(sut(42) == static_cast<long long>(std::hash<int>{}(42)));
      }
      SECTION("Has bound function") {
        REQUIRE(sut.has_target<std::hash<int>>());
      }
    }

    SECTION("Delegate returns void") {
      const delegate<void(int*, int)> sut = bind<square_out_functor>();

      SECTION("Calls bound function") {
        auto out = 0;
        sut(&out, 2);

        REQUIRE(out == 4);
      }
      SECTION("Has bound function") {
        REQUIRE(sut.has_target<square_out_functor>());
      }
    }
  }

  SECTION("Target is bound at runtime") {
    SECTION("Function being bound has same signature") {
      const delegate<std::size_t(int)> sut = bind(std::hash<int>{});

      SECTION("Calls bound function") {
        REQUIRE(sut(42) == std::hash<int>{}(42));
      }
      SECTION("Has bound function") {
        REQUIRE(sut.has_target(std::hash<int>{}));
      }
    }

    SECTION("Function being bound has similar signature") {
      const delegate<long long(int)> sut = bind(std::hash<int>{});

      SECTION("Calls bound function") {
        REQUIRE(sut(42) == static_cast<long long>(std::hash<int>{}(42)));
      }
      SECTION("Has bound function") {
        REQUIRE(sut.has_target(std::hash<int>{}));
      }
    }

    SECTION("Delegate returns void") {
      const delegate<void(int*, int)> sut = bind(square_out_functor{});

      SECTION("Calls bound function") {
        auto out = 0;
        sut(&out, 2);

        REQUIRE(out == 4);
      }
      SECTION("Has bound function") {
        REQUIRE(sut.has_target(square_out_functor{}));
      }
    }
  }
}

//------------------------------------------------------------------------------

TEST_CASE("delegate::delegate(callable_bind_target<Fn>)") {
  SECTION("Function being bound has same signature") {
    int x = 42;
    const auto target = [x](int y){
      return x + y;
    };
    const delegate<int(int)> sut = bind(target);

    SECTION("Calls bound function") {
      REQUIRE(sut(10) == 52);
    }
    SECTION("Has bound function") {
      REQUIRE(sut.has_target(target));
    }
  }

  SECTION("Function being bound has similar signature") {
    int x = 42;
    const auto target = [x](int y) {
      return x + y;
    };

    const delegate<long(long)> sut = bind(target);

    SECTION("Calls bound function") {
      REQUIRE(sut(10) == 52);
    }
    SECTION("Has bound function") {
      REQUIRE(sut.has_target(target));
    }
  }

  SECTION("Delegate returns void") {
    int x = 42;
    const auto target = [x](int* out, int y) -> int{
      return (*out = (x + y));
    };

    const delegate<void(int*, int)> sut = bind(target);

    SECTION("Calls bound function") {
      auto out = 0;
      sut(&out, 10);

      REQUIRE(out == 52);
    }
    SECTION("Has bound function") {
      REQUIRE(sut.has_target(target));
    }
  }
}

TEST_CASE("delegate::delegate(opaque_function_bind_target<UR(UArgs...)>)") {
  SECTION("Function being bound has same signature") {
    const delegate sut = bind(&square);

    SECTION("Calls bound function") {
      REQUIRE(sut(2) == 4);
    }
    SECTION("Has bound function") {
      REQUIRE(sut.has_target(&square));
    }
  }

  SECTION("Function being bound has similar signature") {
    const delegate<long(long)> sut = bind(&square);

    SECTION("Calls bound function") {
      REQUIRE(sut(2) == 4);
    }
    SECTION("Has bound function") {
      REQUIRE(sut.has_target(&square));
    }
  }

  SECTION("Delegate returns void") {
    delegate<void(int&)> sut = bind(&square_out);

    SECTION("Calls bound function") {
      auto output = 2;
      sut(output);
      REQUIRE(output == 4);
    }
    SECTION("Has bound function") {
      REQUIRE(sut.has_target(&square_out));
    }
  }
}

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------

// Not testing 'bind', since this just delegates to 'make'

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

TEST_CASE("delegate::reset()") {
  SECTION("Delegate has a value") {
    const auto to_bind = []{};
    delegate<void()> sut = bind(&to_bind);
    const auto state_before = static_cast<bool>(sut);

    sut.reset();

    SECTION("State changes") {
      const auto state_after = static_cast<bool>(sut);

      REQUIRE(state_before != state_after);
    }
    SECTION("Delegate no longer contains state") {
      REQUIRE(!static_cast<bool>(sut));
    }
    SECTION("Invoking delegate throws bad_delegate_call") {
      REQUIRE_THROWS_AS(sut(), bad_delegate_call);
    }
  }

  SECTION("Delegate does not have a value") {
    auto sut = delegate<void()>{};
    const auto state_before = static_cast<bool>(sut);

    sut.reset();

    SECTION("Delegate does not contain state") {
      REQUIRE(!static_cast<bool>(sut));
    }
    SECTION("State remains unchanged") {
      const auto state_after = static_cast<bool>(sut);

      REQUIRE(state_before == state_after);
    }
    SECTION("Invoking delegate throws bad_delegate_call") {
      REQUIRE_THROWS_AS(sut(), bad_delegate_call);
    }
  }
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

TEST_CASE("delegate::operator bool") {
  SECTION("Delegate is unbound") {
    SECTION("Convertible to false") {
      auto sut = delegate<void()>{};

      REQUIRE_FALSE(static_cast<bool>(sut));
    }
  }
  SECTION("Delegate is bound") {
    SECTION("Convertible to true") {
      delegate<unsigned(int)> sut = bind<std::hash<int>>();

      REQUIRE(static_cast<bool>(sut));
    }
  }
}

TEST_CASE("delegate::operator(Args...)") {
  SECTION("Delegate is unbound") {
    const auto sut = delegate<void()>{};

    SECTION("Throws bad_delegate_call exception") {
      REQUIRE_THROWS_AS(sut(), bad_delegate_call);
    }
  }

  SECTION("Delegate is bound") {
    const auto to_bind = [](auto ptr) {
      return ptr != nullptr;
    };

    delegate<bool(std::unique_ptr<int>)> sut = bind(&to_bind);

    SECTION("Delegate calls bound function with prvalue") {
      REQUIRE(sut(std::make_unique<int>(5)));
    }

    SECTION("Delegate calls bound function with rvlaue") {
      auto input = std::make_unique<int>(5);
      REQUIRE(sut(std::move(input)));
    }

  }
}

//------------------------------------------------------------------------------

TEST_CASE("delegate::has_target()") {
  SECTION("Delegate is unbound") {
    SECTION("has_target returns false") {
      auto sut = delegate<void()>{};

      REQUIRE_FALSE(sut.has_target());
    }
  }
  SECTION("Delegate is bound") {
    SECTION("has_target returns true") {
      delegate<unsigned(int)> sut = bind<std::hash<int>>();

      REQUIRE(sut.has_target());
    }
  }
}

TEST_CASE("delegate::has_target<Fn>()") {
  auto sut = delegate{bind<&square>()};

  SECTION("Delegate has bound target") {
    SECTION("Returns true") {
      REQUIRE(sut.has_target<&square>());
    }
  }
  SECTION("Delegate has late-bound function of target") {
    SECTION("Returns false") {
      REQUIRE_FALSE(sut.has_target(&square));
    }
  }
}

} // namespace test
} // inline namespace bitwizeshift
} // namespace DELEGATE_NAMESPACE_INTERNAL

#if defined(_MSC_VER)
# pragma warning(pop)
#endif
