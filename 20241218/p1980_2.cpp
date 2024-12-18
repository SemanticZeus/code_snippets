#include <iostream>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/math/constants/constants.hpp>

int main() {
    using namespace boost::multiprecision;
    using cpp_dec_float_100 = number<cpp_dec_float<100>>; // 100 digits precision

    // Define the base numbers
    cpp_dec_float_100 sqrt2 = sqrt(cpp_dec_float_100(2));
    cpp_dec_float_100 sqrt3 = sqrt(cpp_dec_float_100(3));
    cpp_dec_float_100 base = sqrt2 + sqrt3;

    // Compute the power
    cpp_dec_float_100 result = pow(base, 1980);

    // Extract integral part
    cpp_int integral_part = static_cast<cpp_int>(result);

    // Extract fractional part
    cpp_dec_float_100 fractional_part = result - cpp_dec_float_100(integral_part);

    // Get the first decimal digit
    int first_decimal_digit = static_cast<int>(fractional_part * 10);

    // Output results
    std::cout << "Result: " << result << std::endl;
    std::cout << "Integral part: " << integral_part << std::endl;
    std::cout << "First decimal digit: " << first_decimal_digit << std::endl;

    return 0;
}
