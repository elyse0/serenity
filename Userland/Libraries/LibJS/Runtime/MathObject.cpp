/*
 * Copyright (c) 2020, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2020, Linus Groh <linusg@serenityos.org>
 * Copyright (c) 2021, Idan Horowitz <idan.horowitz@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/BuiltinWrappers.h>
#include <AK/Function.h>
#include <AK/Random.h>
#include <LibJS/Runtime/GlobalObject.h>
#include <LibJS/Runtime/MathObject.h>
#include <math.h>

namespace JS {

MathObject::MathObject(GlobalObject& global_object)
    : Object(*global_object.object_prototype())
{
}

void MathObject::initialize(GlobalObject& global_object)
{
    auto& vm = this->vm();
    Object::initialize(global_object);
    u8 attr = Attribute::Writable | Attribute::Configurable;
    define_native_function(vm.names.abs, abs, 1, attr);
    define_native_function(vm.names.random, random, 0, attr);
    define_native_function(vm.names.sqrt, sqrt, 1, attr);
    define_native_function(vm.names.floor, floor, 1, attr);
    define_native_function(vm.names.ceil, ceil, 1, attr);
    define_native_function(vm.names.round, round, 1, attr);
    define_native_function(vm.names.max, max, 2, attr);
    define_native_function(vm.names.min, min, 2, attr);
    define_native_function(vm.names.trunc, trunc, 1, attr);
    define_native_function(vm.names.sin, sin, 1, attr);
    define_native_function(vm.names.cos, cos, 1, attr);
    define_native_function(vm.names.tan, tan, 1, attr);
    define_native_function(vm.names.pow, pow, 2, attr);
    define_native_function(vm.names.exp, exp, 1, attr);
    define_native_function(vm.names.expm1, expm1, 1, attr);
    define_native_function(vm.names.sign, sign, 1, attr);
    define_native_function(vm.names.clz32, clz32, 1, attr);
    define_native_function(vm.names.acos, acos, 1, attr);
    define_native_function(vm.names.acosh, acosh, 1, attr);
    define_native_function(vm.names.asin, asin, 1, attr);
    define_native_function(vm.names.asinh, asinh, 1, attr);
    define_native_function(vm.names.atan, atan, 1, attr);
    define_native_function(vm.names.atanh, atanh, 1, attr);
    define_native_function(vm.names.log1p, log1p, 1, attr);
    define_native_function(vm.names.cbrt, cbrt, 1, attr);
    define_native_function(vm.names.atan2, atan2, 2, attr);
    define_native_function(vm.names.fround, fround, 1, attr);
    define_native_function(vm.names.hypot, hypot, 2, attr);
    define_native_function(vm.names.imul, imul, 2, attr);
    define_native_function(vm.names.log, log, 1, attr);
    define_native_function(vm.names.log2, log2, 1, attr);
    define_native_function(vm.names.log10, log10, 1, attr);
    define_native_function(vm.names.sinh, sinh, 1, attr);
    define_native_function(vm.names.cosh, cosh, 1, attr);
    define_native_function(vm.names.tanh, tanh, 1, attr);

    // 21.3.1 Value Properties of the Math Object, https://tc39.es/ecma262/#sec-value-properties-of-the-math-object
    define_direct_property(vm.names.E, Value(M_E), 0);
    define_direct_property(vm.names.LN2, Value(M_LN2), 0);
    define_direct_property(vm.names.LN10, Value(M_LN10), 0);
    define_direct_property(vm.names.LOG2E, Value(::log2(M_E)), 0);
    define_direct_property(vm.names.LOG10E, Value(::log10(M_E)), 0);
    define_direct_property(vm.names.PI, Value(M_PI), 0);
    define_direct_property(vm.names.SQRT1_2, Value(M_SQRT1_2), 0);
    define_direct_property(vm.names.SQRT2, Value(M_SQRT2), 0);

    // 21.3.1.9 Math [ @@toStringTag ], https://tc39.es/ecma262/#sec-math-@@tostringtag
    define_direct_property(*vm.well_known_symbol_to_string_tag(), js_string(vm, vm.names.Math.as_string()), Attribute::Configurable);
}

MathObject::~MathObject()
{
}

// 21.3.2.1 Math.abs ( x ), https://tc39.es/ecma262/#sec-math.abs
JS_DEFINE_NATIVE_FUNCTION(MathObject::abs)
{
    // Let n be ? ToNumber(x).
    auto number = TRY(vm.argument(0).to_number(global_object));

    // 2. If n is NaN, return NaN.
    if (number.is_nan())
        return js_nan();

    // 3. If n is -0𝔽, return +0𝔽.
    if (number.is_negative_zero())
        return Value(0);

    // 4. If n is -∞𝔽, return +∞𝔽.
    if (number.is_negative_infinity())
        return js_infinity();

    auto number_double = number.as_double();

    // 5. If n < +0𝔽, return -n.
    if (number_double < 0)
        return -number_double;

    // 6. Return n.
    return Value(number_double);
}

// 21.3.2.27 Math.random ( ), https://tc39.es/ecma262/#sec-math.random
JS_DEFINE_NATIVE_FUNCTION(MathObject::random)
{
    // Returns a Number value with positive sign, greater than or equal to +0𝔽 but strictly less than 1𝔽,
    // chosen randomly or pseudo randomly with approximately uniform distribution over that range, using
    // an implementation-defined algorithm or strategy. This function takes no arguments.

    double r = (double)get_random<u32>() / (double)UINT32_MAX;
    return Value(r);
}

// 21.3.2.32 Math.sqrt ( x ), https://tc39.es/ecma262/#sec-math.sqrt
JS_DEFINE_NATIVE_FUNCTION(MathObject::sqrt)
{
    // 1. Let n be ? ToNumber(x).
    auto number = TRY(vm.argument(0).to_number(global_object));

    // 2. If n is NaN, n is +0𝔽, n is -0𝔽, or n is +∞𝔽, return n.
    if (number.is_nan() || number.is_positive_zero() || number.is_negative_zero() || number.is_positive_infinity())
        return number;

    auto number_double = number.as_double();

    // 3. If n < +0𝔽, return NaN.
    if (number_double < 0)
        return js_nan();

    // 4. Return an implementation-approximated Number value representing the result of the square root of ℝ(n).
    return Value(::sqrt(number_double));
}

// 21.3.2.16 Math.floor ( x ), https://tc39.es/ecma262/#sec-math.floor
JS_DEFINE_NATIVE_FUNCTION(MathObject::floor)
{
    // 1. Let n be ? ToNumber(x).
    auto number = TRY(vm.argument(0).to_number(global_object));

    // 2. If n is NaN, n is +0𝔽, n is -0𝔽, n is +∞𝔽, or n is -∞𝔽, return n.
    if (number.is_nan() || number.is_positive_zero() || number.is_negative_zero() || number.is_positive_infinity() || number.is_negative_infinity())
        return number;

    auto number_double = number.as_double();

    // 3. If n < 1𝔽 and n > +0𝔽, return +0𝔽.
    if (number_double < 1 && number_double > 0)
        return Value(0);

    // 4. If n is an integral Number, return n.
    if (number.is_integral_number())
        return number;

    // 5. Return the greatest (closest to +∞) integral Number value that is not greater than n.
    return Value(::floor(number_double));
}

// 21.3.2.10 Math.ceil ( x ), https://tc39.es/ecma262/#sec-math.ceil
JS_DEFINE_NATIVE_FUNCTION(MathObject::ceil)
{
    // 1. Let n be ? ToNumber(x).
    auto number = TRY(vm.argument(0).to_number(global_object));

    // 2. If n is NaN, n is +0𝔽, n is -0𝔽, n is +∞𝔽, or n is -∞𝔽, return n.
    if (number.is_nan() || number.is_positive_zero() || number.is_negative_zero() || number.is_positive_infinity() || number.is_negative_infinity())
        return number;

    auto number_double = number.as_double();

    // 3. If n < +0𝔽 and n > -1𝔽, return -0𝔽.
    if (number_double < 0 && number_double > -1)
        return Value(-0.0);

    // 4. If n is an integral Number, return n.
    if (number.is_integral_number())
        return number;

    // 5. Return the smallest (closest to -∞) integral Number value that is not less than n.
    return Value(::ceil(number.as_double()));
}

// 21.3.2.28 Math.round ( x ), https://tc39.es/ecma262/#sec-math.round
JS_DEFINE_NATIVE_FUNCTION(MathObject::round)
{
    // Let n be ? ToNumber(x).
    auto number = TRY(vm.argument(0).to_number(global_object));

    // 2. If n is NaN, +∞𝔽, -∞𝔽, or an integral Number, return n.
    if (number.is_nan() || number.is_positive_infinity() || number.is_negative_infinity() || number.is_integral_number())
        return number;

    auto number_double = number.as_double();

    // 3. If n < 0.5𝔽 and n > +0𝔽, return +0𝔽.
    if (number_double < 0.5 && number_double > 0)
        return Value(0);

    // 4. If n < +0𝔽 and n ≥ -0.5𝔽, return -0𝔽.
    if (number_double < 0 && number_double >= -0.5)
        return Value(-0.0);

    double integer = ::ceil(number_double);
    if (integer - 0.5 > number_double) {
        integer--;
    }

    // 5. Return the integral Number closest to n, preferring the Number closer to +∞ in the case of a tie.
    return Value(integer);
}

// 21.3.2.24 Math.max ( ...args ), https://tc39.es/ecma262/#sec-math.max
JS_DEFINE_NATIVE_FUNCTION(MathObject::max)
{
    // 1. Let coerced be a new empty List.
    Vector<Value> coerced;

    // 2. For each element arg of args, do
    for (size_t i = 0; i < vm.argument_count(); ++i)
        // a. Let n be ? ToNumber(arg).
        // b. Append n to coerced.
        coerced.append(TRY(vm.argument(i).to_number(global_object)));

    // 3. Let highest be -∞𝔽.
    auto highest = js_negative_infinity();

    // 4. For each element number of coerced, do
    for (auto& number : coerced) {
        // a. If number is NaN, return NaN.
        if (number.is_nan())
            return js_nan();

        // b. If number is +0𝔽 and highest is -0𝔽, set highest to +0𝔽.
        // c. If number > highest, set highest to number.
        if ((number.is_positive_zero() && highest.is_negative_zero()) || number.as_double() > highest.as_double())
            highest = number;
    }

    // 5. Return highest.
    return highest;
}

// 21.3.2.25 Math.min ( ...args ), https://tc39.es/ecma262/#sec-math.min
JS_DEFINE_NATIVE_FUNCTION(MathObject::min)
{
    // 1. Let coerced be a new empty List.
    Vector<Value> coerced;

    // 2. For each element arg of args, do
    for (size_t i = 0; i < vm.argument_count(); ++i)
        // a. Let n be ? ToNumber(arg).
        // b. Append n to coerced.
        coerced.append(TRY(vm.argument(i).to_number(global_object)));

    // 3. Let lowest be +∞𝔽.
    auto lowest = js_infinity();

    // 4. For each element number of coerced, do
    for (auto& number : coerced) {
        // a. If number is NaN, return NaN.
        if (number.is_nan())
            return js_nan();

        // b. If number is -0𝔽 and lowest is +0𝔽, set lowest to -0𝔽.
        // c. If number < lowest, set lowest to number.
        if ((number.is_negative_zero() && lowest.is_positive_zero()) || number.as_double() < lowest.as_double())
            lowest = number;
    }

    // 5. Return lowest.
    return lowest;
}

// 21.3.2.35 Math.trunc ( x ), https://tc39.es/ecma262/#sec-math.trunc
JS_DEFINE_NATIVE_FUNCTION(MathObject::trunc)
{
    // 1. Let n be ? ToNumber(x).
    auto number = TRY(vm.argument(0).to_number(global_object));

    // 2. If n is NaN, n is +0𝔽, n is -0𝔽, n is +∞𝔽, or n is -∞𝔽, return n.
    if (number.is_nan() || number.is_positive_zero() || number.is_negative_zero() || number.is_positive_infinity() || number.is_negative_infinity())
        return number;

    auto number_double = number.as_double();

    // 3. If n < 1𝔽 and n > +0𝔽, return +0𝔽.
    if (number_double < 1 && number_double > 0)
        return Value(0);

    // 4. If n < +0𝔽 and n > -1𝔽, return -0𝔽.
    if (number_double < 0 && number_double > -1)
        return Value(-0.0);

    // 5. Return the integral Number nearest n in the direction of +0𝔽.
    return MathObject::floor(vm, global_object);
}

// 21.3.2.30 Math.sin ( x ), https://tc39.es/ecma262/#sec-math.sin
JS_DEFINE_NATIVE_FUNCTION(MathObject::sin)
{
    // 1. Let n be ? ToNumber(x).
    auto number = TRY(vm.argument(0).to_number(global_object));

    // 2. If n is NaN, n is +0𝔽, or n is -0𝔽, return n.
    if (number.is_nan() || number.is_positive_zero() || number.is_negative_zero())
        return number;

    // 3. If n is +∞𝔽 or n is -∞𝔽, return NaN.
    if (number.is_positive_infinity() || number.is_negative_infinity())
        return js_nan();

    // 4. Return an implementation-approximated Number value representing the result of the sine of ℝ(n).
    return Value(::sin(number.as_double()));
}

// 21.3.2.12 Math.cos ( x ), https://tc39.es/ecma262/#sec-math.cos
JS_DEFINE_NATIVE_FUNCTION(MathObject::cos)
{
    // 1. Let n be ? ToNumber(x).
    auto number = TRY(vm.argument(0).to_number(global_object));

    // 2. If n is NaN, n is +∞𝔽, or n is -∞𝔽, return NaN.
    if (number.is_nan() || number.is_positive_infinity() || number.is_negative_infinity())
        return js_nan();

    // 3. If n is +0𝔽 or n is -0𝔽, return 1𝔽.
    if (number.is_positive_zero() || number.is_negative_zero())
        return Value(1);

    // 4. Return an implementation-approximated Number value representing the result of the cosine of ℝ(n).
    return Value(::cos(number.as_double()));
}

// 21.3.2.33 Math.tan ( x ), https://tc39.es/ecma262/#sec-math.tan
JS_DEFINE_NATIVE_FUNCTION(MathObject::tan)
{
    // 1. Let n be ? ToNumber(x).
    auto number = TRY(vm.argument(0).to_number(global_object));

    // 2. If n is NaN, n is +0𝔽, or n is -0𝔽, return n.
    if (number.is_nan() || number.is_positive_zero() || number.is_negative_zero())
        return number;

    // 3. If n is +∞𝔽, or n is -∞𝔽, return NaN.
    if (number.is_positive_infinity() || number.is_negative_infinity())
        return js_nan();

    // 4. Return an implementation-approximated Number value representing the result of the tangent of ℝ(n).
    return Value(::tan(number.as_double()));
}

// 6.1.6.1.3 Number::exponentiate ( base, exponent ), https://tc39.es/ecma262/#sec-numeric-types-number-exponentiate
static Value exponentiate(Value base, Value exponent)
{
    // 1. If exponent is NaN, return NaN.
    if (exponent.is_nan())
        return js_nan();

    // 2. If exponent is +0𝔽 or exponent is -0𝔽, return 1𝔽.
    if (exponent.is_positive_zero() || exponent.is_negative_zero())
        return Value(1);

    // 3. If base is NaN, return NaN.
    if (base.is_nan())
        return js_nan();

    // 4. If base is +∞𝔽, then
    if (base.is_positive_infinity())
        // a. If exponent > +0𝔽, return +∞𝔽. Otherwise, return +0𝔽.
        return exponent.as_double() > 0 ? js_infinity() : Value(0);

    // 5. If base is -∞𝔽, then
    if (base.is_negative_infinity()) {
        auto is_odd_integral_number = exponent.is_integral_number() && (exponent.as_i32() % 2 != 0);

        // a. If exponent > +0𝔽, then
        if (exponent.as_double() > 0)
            // i. If exponent is an odd integral Number, return -∞𝔽. Otherwise, return +∞𝔽.
            return is_odd_integral_number ? js_negative_infinity() : js_infinity();
        // b. Else,
        else
            // i. If exponent is an odd integral Number, return -0𝔽. Otherwise, return +0𝔽.
            return is_odd_integral_number ? Value(-0.0) : Value(0);
    }

    // 6. If base is +0𝔽, then
    if (base.is_positive_zero())
        // a. If exponent > +0𝔽, return +0𝔽. Otherwise, return +∞𝔽.
        return exponent.as_double() > 0 ? Value(0) : js_infinity();

    // 7. If base is -0𝔽, then
    if (base.is_negative_zero()) {
        auto is_odd_integral_number = exponent.is_integral_number() && (exponent.as_i32() % 2 != 0);

        // a. If exponent > +0𝔽, then
        if (exponent.as_double() > 0)
            // i. If exponent is an odd integral Number, return -0𝔽. Otherwise, return +0𝔽.
            return is_odd_integral_number ? Value(-0.0) : Value(0);
        // b. Else,
        else
            // i. If exponent is an odd integral Number, return -∞𝔽. Otherwise, return +∞𝔽.
            return is_odd_integral_number ? js_negative_infinity() : js_infinity();
    }

    // 8. Assert: base is finite and is neither +0𝔽 nor -0𝔽.
    VERIFY(base.is_finite_number() && !base.is_positive_zero() && !base.is_negative_zero());

    // 9. If exponent is +∞𝔽, then
    if (exponent.is_positive_infinity()) {
        auto absolute_base = fabs(base.as_double());

        // a. If abs(ℝ(base)) > 1, return +∞𝔽.
        if (absolute_base > 1)
            return js_infinity();

        // b. If abs(ℝ(base)) is 1, return NaN.
        if (absolute_base == 1)
            return js_nan();

        // c. If abs(ℝ(base)) < 1, return +0𝔽.
        if (absolute_base < 1)
            return Value(0);
    }

    // 10. If exponent is -∞𝔽, then
    if (exponent.is_negative_infinity()) {
        auto absolute_base = fabs(base.as_double());

        // a. If abs(ℝ(base)) > 1, return +0𝔽.
        if (absolute_base > 1)
            return Value(0);

        // b. If abs(ℝ(base)) is 1, return NaN.
        if (absolute_base == 1)
            return js_nan();

        // c. If abs(ℝ(base)) < 1, return +∞𝔽.
        if (absolute_base < 1)
            return js_infinity();
    }

    // 11. Assert: exponent is finite and is neither +0𝔽 nor -0𝔽.
    VERIFY(exponent.is_finite_number() && !exponent.is_positive_zero() && !exponent.is_negative_zero());

    // 12. If base < +0𝔽 and exponent is not an integral Number, return NaN.
    if (base.as_double() < 0 && !exponent.is_integral_number())
        return js_nan();

    // 13. Return an implementation-approximated Number value representing the result of raising ℝ(base) to the ℝ(exponent) power.
    return Value(::pow(base.as_double(), exponent.as_double()));
}

// 21.3.2.26 Math.pow ( base, exponent ), https://tc39.es/ecma262/#sec-math.pow
JS_DEFINE_NATIVE_FUNCTION(MathObject::pow)
{
    // 1. Set base to ? ToNumber(base).
    auto base = TRY(vm.argument(0).to_number(global_object));

    // 2. Set exponent to ? ToNumber(exponent).
    auto exponent = TRY(vm.argument(1).to_number(global_object));

    // 3. Return ! Number::exponentiate(base, exponent).
    return exponentiate(base, exponent);
}

// 21.3.2.14 Math.exp ( x ), https://tc39.es/ecma262/#sec-math.exp
JS_DEFINE_NATIVE_FUNCTION(MathObject::exp)
{
    // 1. Let n be ? ToNumber(x).
    auto number = TRY(vm.argument(0).to_number(global_object));

    // 2. If n is NaN or n is +∞𝔽, return n.
    if (number.is_nan() || number.is_positive_infinity())
        return number;

    // 3. If n is +0𝔽 or n is -0𝔽, return 1𝔽.
    if (number.is_positive_zero() || number.is_negative_zero())
        return Value(1);

    // 4. If n is -∞𝔽, return +0𝔽.
    if (number.is_negative_infinity())
        return Value(0);

    // 5. Return an implementation-approximated Number value representing the result of the exponential function of ℝ(n).
    return Value(::exp(number.as_double()));
}

// 21.3.2.15 Math.expm1 ( x ), https://tc39.es/ecma262/#sec-math.expm1
JS_DEFINE_NATIVE_FUNCTION(MathObject::expm1)
{
    // 1. Let n be ? ToNumber(x).
    auto number = TRY(vm.argument(0).to_number(global_object));

    // 2. If n is NaN, n is +0𝔽, n is -0𝔽, or n is +∞𝔽, return n.
    if (number.is_nan() || number.is_positive_zero() || number.is_positive_infinity())
        return number;

    // 3. If n is -∞𝔽, return -1𝔽.
    if (number.is_negative_infinity())
        return Value(-1);

    // 4. Return an implementation-approximated Number value representing the result of subtracting 1 from the exponential function of ℝ(n).
    return Value(::expm1(number.as_double()));
}

// 21.3.2.29 Math.sign ( x ), https://tc39.es/ecma262/#sec-math.sign
JS_DEFINE_NATIVE_FUNCTION(MathObject::sign)
{
    // Let n be ? ToNumber(x).
    auto number = TRY(vm.argument(0).to_number(global_object));

    // 2. If n is NaN, n is +0𝔽, or n is -0𝔽, return n.
    if (number.is_nan() || number.is_positive_zero() || number.is_negative_zero())
        return number;

    auto number_double = number.as_double();

    // 3. If n < +0𝔽, return -1𝔽.
    if (number_double < 0)
        return Value(-1);

    // 4. Return 1𝔽.
    return Value(1);
}

// 21.3.2.11 Math.clz32 ( x ), https://tc39.es/ecma262/#sec-math.clz32
JS_DEFINE_NATIVE_FUNCTION(MathObject::clz32)
{
    // Let n be ? ToUint32(x).
    auto number = TRY(vm.argument(0).to_u32(global_object));

    // 2. Let p be the number of leading zero bits in the unsigned 32-bit binary representation of n.
    // 3. Return 𝔽(p).
    if (number == 0)
        return Value(32);

    return Value(count_leading_zeroes(number));
}

// 21.3.2.2 Math.acos ( x ), https://tc39.es/ecma262/#sec-math.acos
JS_DEFINE_NATIVE_FUNCTION(MathObject::acos)
{
    // Let n be ? ToNumber(x).
    auto number = TRY(vm.argument(0).to_number(global_object));

    // 2. If n is NaN, n > 1𝔽, or n < -1𝔽, return NaN.
    if (number.is_nan() || number.as_double() > 1 || number.as_double() < -1)
        return js_nan();

    auto number_double = number.as_double();

    // 3. If n is 1𝔽, return +0𝔽.
    if (number_double == 1)
        return Value(0);

    // 4. Return an implementation-approximated Number value representing the result of the inverse cosine of ℝ(n).
    return Value(::acos(number_double));
}

// 21.3.2.3 Math.acosh ( x ), https://tc39.es/ecma262/#sec-math.acosh
JS_DEFINE_NATIVE_FUNCTION(MathObject::acosh)
{
    // 1. Let n be ? ToNumber(x).
    auto number = TRY(vm.argument(0).to_number(global_object));

    // 2. If n is NaN or n is +∞𝔽, return n.
    if (number.is_nan() || number.is_positive_infinity())
        return number;

    auto number_double = number.as_double();

    // 3. If n is 1𝔽, return +0𝔽.
    if (number_double == 1)
        return 0;

    // 4. If n < 1𝔽, return NaN.
    if (number_double < 1)
        return js_nan();

    // 5. Return an implementation-approximated Number value representing the result of the inverse hyperbolic cosine of ℝ(n).
    return Value(::acosh(number_double));
}

// 21.3.2.4 Math.asin ( x ), https://tc39.es/ecma262/#sec-math.asin
JS_DEFINE_NATIVE_FUNCTION(MathObject::asin)
{
    // 1. Let n be ? ToNumber(x).
    auto number = TRY(vm.argument(0).to_number(global_object));

    // 2. If n is NaN, n is +0𝔽, or n is -0𝔽, return n.
    if (number.is_nan() || number.is_positive_zero() || number.is_negative_zero())
        return number;

    auto number_double = number.as_double();

    // 3. If n > 1𝔽 or n < -1𝔽, return NaN.
    if (number_double > 1 || number_double < -1)
        return js_nan();

    // 4. Return an implementation-approximated Number value representing the result of the inverse sine of ℝ(n).
    return Value(::asin(number.as_double()));
}

// 21.3.2.5 Math.asinh ( x ), https://tc39.es/ecma262/#sec-math.asinh
JS_DEFINE_NATIVE_FUNCTION(MathObject::asinh)
{
    // 1. Let n be ? ToNumber(x).
    auto number = TRY(vm.argument(0).to_number(global_object));

    // 2. If n is NaN, n is +0𝔽, n is -0𝔽, n is +∞𝔽, or n is -∞𝔽, return n.
    if (number.is_nan() || number.is_positive_zero() || number.is_negative_zero() || number.is_positive_infinity() || number.is_negative_infinity()) {
        return number;
    }

    // 3. Return an implementation-approximated Number value representing the result of the inverse hyperbolic sine of ℝ(n).
    return Value(::asinh(number.as_double()));
}

// 21.3.2.6 Math.atan ( x ), https://tc39.es/ecma262/#sec-math.atan
JS_DEFINE_NATIVE_FUNCTION(MathObject::atan)
{
    // Let n be ? ToNumber(x).
    auto number = TRY(vm.argument(0).to_number(global_object));

    // 2. If n is NaN, n is +0𝔽, or n is -0𝔽, return n.
    if (number.is_nan() || number.is_positive_zero() || number.is_negative_zero())
        return number;

    // 3. If n is +∞𝔽, return an implementation-approximated Number value representing π / 2.
    if (number.is_positive_infinity())
        return Value(M_PI_2);

    // 4. If n is -∞𝔽, return an implementation-approximated Number value representing -π / 2.
    if (number.is_negative_infinity())
        return Value(-M_PI_2);

    // 5. Return an implementation-approximated Number value representing the result of the inverse tangent of ℝ(n).
    return Value(::atan(number.as_double()));
}

// 21.3.2.7 Math.atanh ( x ), https://tc39.es/ecma262/#sec-math.atanh
JS_DEFINE_NATIVE_FUNCTION(MathObject::atanh)
{
    auto value = TRY(vm.argument(0).to_number(global_object)).as_double();
    if (value > 1 || value < -1)
        return js_nan();
    return Value(::atanh(value));
}

// 21.3.2.21 Math.log1p ( x ), https://tc39.es/ecma262/#sec-math.log1p
JS_DEFINE_NATIVE_FUNCTION(MathObject::log1p)
{
    auto value = TRY(vm.argument(0).to_number(global_object)).as_double();
    if (value < -1)
        return js_nan();
    return Value(::log1p(value));
}

// 21.3.2.9 Math.cbrt ( x ), https://tc39.es/ecma262/#sec-math.cbrt
JS_DEFINE_NATIVE_FUNCTION(MathObject::cbrt)
{
    return Value(::cbrt(TRY(vm.argument(0).to_number(global_object)).as_double()));
}

// 21.3.2.8 Math.atan2 ( y, x ), https://tc39.es/ecma262/#sec-math.atan2
JS_DEFINE_NATIVE_FUNCTION(MathObject::atan2)
{
    auto constexpr three_quarters_pi = M_PI_4 + M_PI_2;

    auto y = TRY(vm.argument(0).to_number(global_object));
    auto x = TRY(vm.argument(1).to_number(global_object));

    if (y.is_nan() || x.is_nan())
        return js_nan();
    if (y.is_positive_infinity()) {
        if (x.is_positive_infinity())
            return Value(M_PI_4);
        else if (x.is_negative_infinity())
            return Value(three_quarters_pi);
        else
            return Value(M_PI_2);
    }
    if (y.is_negative_infinity()) {
        if (x.is_positive_infinity())
            return Value(-M_PI_4);
        else if (x.is_negative_infinity())
            return Value(-three_quarters_pi);
        else
            return Value(-M_PI_2);
    }
    if (y.is_positive_zero()) {
        if (x.as_double() > 0 || x.is_positive_zero())
            return Value(0.0);
        else
            return Value(M_PI);
    }
    if (y.is_negative_zero()) {
        if (x.as_double() > 0 || x.is_positive_zero())
            return Value(-0.0);
        else
            return Value(-M_PI);
    }
    VERIFY(y.is_finite_number() && !y.is_positive_zero() && !y.is_negative_zero());
    if (y.as_double() > 0) {
        if (x.is_positive_infinity())
            return Value(0);
        else if (x.is_negative_infinity())
            return Value(M_PI);
        else if (x.is_positive_zero() || x.is_negative_zero())
            return Value(M_PI_2);
    }
    if (y.as_double() < 0) {
        if (x.is_positive_infinity())
            return Value(-0.0);
        else if (x.is_negative_infinity())
            return Value(-M_PI);
        else if (x.is_positive_zero() || x.is_negative_zero())
            return Value(-M_PI_2);
    }
    VERIFY(x.is_finite_number() && !x.is_positive_zero() && !x.is_negative_zero());
    return Value(::atan2(y.as_double(), x.as_double()));
}

// 21.3.2.17 Math.fround ( x ), https://tc39.es/ecma262/#sec-math.fround
JS_DEFINE_NATIVE_FUNCTION(MathObject::fround)
{
    auto number = TRY(vm.argument(0).to_number(global_object));
    if (number.is_nan())
        return js_nan();
    return Value((float)number.as_double());
}

// 21.3.2.18 Math.hypot ( ...args ), https://tc39.es/ecma262/#sec-math.hypot
JS_DEFINE_NATIVE_FUNCTION(MathObject::hypot)
{
    Vector<Value> coerced;
    for (size_t i = 0; i < vm.argument_count(); ++i)
        coerced.append(TRY(vm.argument(i).to_number(global_object)));

    for (auto& number : coerced) {
        if (number.is_positive_infinity() || number.is_negative_infinity())
            return js_infinity();
    }

    auto only_zero = true;
    double sum_of_squares = 0;
    for (auto& number : coerced) {
        if (number.is_nan() || number.is_positive_infinity())
            return number;
        if (number.is_negative_infinity())
            return js_infinity();
        if (!number.is_positive_zero() && !number.is_negative_zero())
            only_zero = false;
        sum_of_squares += number.as_double() * number.as_double();
    }
    if (only_zero)
        return Value(0);
    return Value(::sqrt(sum_of_squares));
}

// 21.3.2.19 Math.imul ( x, y ), https://tc39.es/ecma262/#sec-math.imul
JS_DEFINE_NATIVE_FUNCTION(MathObject::imul)
{
    auto a = TRY(vm.argument(0).to_u32(global_object));
    auto b = TRY(vm.argument(1).to_u32(global_object));
    return Value(static_cast<i32>(a * b));
}

// 21.3.2.20 Math.log ( x ), https://tc39.es/ecma262/#sec-math.log
JS_DEFINE_NATIVE_FUNCTION(MathObject::log)
{
    auto value = TRY(vm.argument(0).to_number(global_object)).as_double();
    if (value < 0)
        return js_nan();
    return Value(::log(value));
}

// 21.3.2.23 Math.log2 ( x ), https://tc39.es/ecma262/#sec-math.log2
JS_DEFINE_NATIVE_FUNCTION(MathObject::log2)
{
    auto value = TRY(vm.argument(0).to_number(global_object)).as_double();
    if (value < 0)
        return js_nan();
    return Value(::log2(value));
}

// 21.3.2.22 Math.log10 ( x ), https://tc39.es/ecma262/#sec-math.log10
JS_DEFINE_NATIVE_FUNCTION(MathObject::log10)
{
    auto value = TRY(vm.argument(0).to_number(global_object)).as_double();
    if (value < 0)
        return js_nan();
    return Value(::log10(value));
}

// 21.3.2.31 Math.sinh ( x ), https://tc39.es/ecma262/#sec-math.sinh
JS_DEFINE_NATIVE_FUNCTION(MathObject::sinh)
{
    auto number = TRY(vm.argument(0).to_number(global_object));
    if (number.is_nan())
        return js_nan();
    return Value(::sinh(number.as_double()));
}

// 21.3.2.13 Math.cosh ( x ), https://tc39.es/ecma262/#sec-math.cosh
JS_DEFINE_NATIVE_FUNCTION(MathObject::cosh)
{
    auto number = TRY(vm.argument(0).to_number(global_object));
    if (number.is_nan())
        return js_nan();
    return Value(::cosh(number.as_double()));
}

// 21.3.2.34 Math.tanh ( x ), https://tc39.es/ecma262/#sec-math.tanh
JS_DEFINE_NATIVE_FUNCTION(MathObject::tanh)
{
    auto number = TRY(vm.argument(0).to_number(global_object));
    if (number.is_nan())
        return js_nan();
    if (number.is_positive_infinity())
        return Value(1);
    if (number.is_negative_infinity())
        return Value(-1);
    return Value(::tanh(number.as_double()));
}

}
