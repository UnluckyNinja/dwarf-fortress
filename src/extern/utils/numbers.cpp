/**
 * @file
 * @date 6 juin 2010
 * @todo comment
 */

#include <boost/lexical_cast.hpp>

#include "extern/utils/numbers.hpp"
#include "intern/utils/numbers.hpp"

#include <logging/logging.hpp>

void get_number_internal(int32_t number, ::std::string& str);

void get_number(int32_t number, ::std::string& str) {
  str.clear();
  get_number_internal(number, str);
}

void get_number_internal(int32_t number, ::std::string& str) {
  if (number < 0) {
    number *= -1;
    str.append("negative ");
  }

  // If the number is zero, just print zero...
  if (number == 0) {
    str.append("zero");
  } else {

    // Deal with billions
    int32_t billions = number / 1000000000;
    number %= 1000000000;
    if (billions > 0) {
      get_number_internal(billions, str);
      str.append(" billion");

      if (number > 0) {
        str.append(" ");
      }
    }

    // Deal with millions
    int32_t millions = number / 1000000;
    number %= 1000000;
    if (millions > 0) {
      get_number_internal(millions, str);
      str.append(" million");

      if (number > 0) {
        str.append(" ");
      }
    }

    // Deal with thousands
    int32_t thousands = number / 1000;
    number %= 1000;
    if (thousands > 0) {
      get_number_internal(thousands, str);
      str.append(" thousand");

      if (number > 0) {
        str.append(" ");
      }
    }

    // Deal with hundreds
    int32_t hundreds = number / 100;
    number %= 100;
    if (hundreds > 0) {
      get_number_internal(hundreds, str);
      str.append(" hundred");

      if (number > 0) {
        str.append(" ");
      }
    }

    // Deal with teens.
    int32_t teens = number / 10;

    // Actually, we only deal twenties and more. Teens are handled directly as units.
    if (teens > 1) {
      number %= 10;

      switch (teens) {
        case 2:
          str.append("twenty");
          break;
        case 3:
          str.append("thirty");
          break;
        case 4:
          str.append("forty");
          break;
        case 5:
          str.append("fifty");
          break;
        case 6:
          str.append("sixty");
          break;
        case 7:
          str.append("seventy");
          break;
        case 8:
          str.append("eighty");
          break;
        case 9:
          str.append("ninety");
          break;
      }

      if (number > 0) {
        str.append("-");
      }
    }

    // Deal with units and teens.
    if (number > 0) {
      switch (number) {
        case 1:
          str.append("one");
          break;
        case 2:
          str.append("two");
          break;
        case 3:
          str.append("three");
          break;
        case 4:
          str.append("four");
          break;
        case 5:
          str.append("five");
          break;
        case 6:
          str.append("six");
          break;
        case 7:
          str.append("seven");
          break;
        case 8:
          str.append("eight");
          break;
        case 9:
          str.append("nine");
          break;
        case 10:
          str.append("ten");
          break;
        case 11:
          str.append("eleven");
          break;
        case 12:
          str.append("twelve");
          break;
        case 13:
          str.append("thirteen");
          break;
        case 14:
          str.append("fourteen");
          break;
        case 15:
          str.append("fifteen");
          break;
        case 16:
          str.append("sixteen");
          break;
        case 17:
          str.append("seventeen");
          break;
        case 18:
          str.append("eighteen");
          break;
        case 19:
          str.append("nineteen");
          break;
      }
    }
  }
}

void get_ordinal_internal(int32_t number, ::std::string& str, bool shorten);

void get_ordinal(int32_t number, ::std::string& str, bool shorten) {
  str.clear();
  get_ordinal_internal(number, str, shorten);
}

void get_ordinal_suffix(int32_t number, ::std::string& str);

void get_ordinal_suffix(int32_t number, ::std::string& str) {
  int32_t unit = number % 10;
  int32_t teen = number % 100;

  switch (teen) {
    case 11:
    case 12:
    case 13:
      str.append("th");
      return;
    default:
      switch (unit) {
        case 1:
          str.append("st");
          return;
        case 2:
          str.append("nd");
          return;
        case 3:
          str.append("rd");
          return;
        default:
          str.append("th");
          return;
      }
  }
}

void get_ordinal_internal(int32_t number, ::std::string& str, bool shorten) {
  if (shorten) {
    if (number < 0) {
      number *= -1;
      str.append("-");
    }

    add_long_to_string(number, str);
  } else {
    if (number < 0) {
      number *= -1;
      str.append("Negative ");
    }

    switch (number) {
      case 0:
        str.append("Zero");
        break;
      case 1:
        str.append("Fir");
        break;
      case 2:
        str.append("Seco");
        break;
      case 3:
        str.append("Thi");
        break;
      case 4:
        str.append("Four");
        break;
      case 5:
        str.append("Fif");
        break;
      case 6:
        str.append("Six");
        break;
      case 7:
        str.append("Seven");
        break;
      case 8:
        str.append("Eigh");
        break;
      case 9:
        str.append("Nin");
        break;
      case 10:
        str.append("Ten");
        break;
      case 11:
        str.append("Eleven");
        break;
      case 12:
        str.append("Twelf");
        break;
      case 13:
        str.append("Thirteen");
        break;
      case 14:
        str.append("Fourteen");
        break;
      case 15:
        str.append("Fifteen");
        break;
      case 16:
        str.append("Sixteen");
        break;
      case 17:
        str.append("Seventeen");
        break;
      case 18:
        str.append("Eighteen");
        break;
      case 19:
        str.append("Nineteen");
        break;
      default:
        add_long_to_string(number, str);
        break;
    }
  }

  get_ordinal_suffix(number, str);
}

void add_long_to_string(int32_t number, ::std::string& str) {
  str.append(::boost::lexical_cast< ::std::string >(number));
}

int32_t convert_string_to_long(::std::string& str) {
//  __debug
//    << "External call to convert_string_to_long, string: '" << str << "'";
  return to_int32(str);
}

uint32_t convert_string_to_ulong(::std::string& str) {
//  __debug
//    << "External call to convert_string_to_ulong, string: '" << str << "'";
  return to_uint32(str);
}

void convert_long_to_string(int32_t number, ::std::string& str) {
  str.assign(::boost::lexical_cast< ::std::string >(number));
}

void convert_ulong_to_string(uint32_t number, ::std::string& str) {
  str.assign(::boost::lexical_cast< ::std::string >(number));
}
