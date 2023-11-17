# Multi lambda

## Description

A header-only C++20 library implementing a container for multiple lambdas. `multi_lambda` stores a list of lambdas with the same function call operator signature. It doesn't use type erasure and allows to do things like:

```cpp

auto count{ 0 };

multi_lambda ml
{
    [](double a, double b) { return a + b; },
    [](double a, double b) { return a - b; },
    [](double a, double b) { return a * b; },
    [&count](double a, double b) { return a / b; }
};

ml(1.0, 2.0); // -> std::array{ 3.0, -1.0, 2.0, 0.5 }

ml<2>(1.0, 2.0); // -> 2.0

ml[3](1.0, 2.0); // -> 0.5

auto invoker_13 = ml.get_invoker<1, 3>();

invoker_13(1.0, 2.0); // -> std::array{ -1.0, 0.5 }

auto invoker_02 = ml.get_invoker(0, 2); 

invoker_02(1.0, 2.0); // -> std::array{ 3.0, 2.0 }

std::cout << std::format("Division was called {} times", count); // -> Division was called 3 times
```