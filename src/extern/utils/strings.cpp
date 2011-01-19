/**
 * @file
 * @date 6 juin 2010
 * @todo comment
 */

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

#include "extern/utils/strings.hpp"

#include <logging/logging.hpp>

bool grab_token_string(::std::string& token, ::std::string& source, char separator) {
  return grab_token_string_pos(token, source, 0, separator);
}

bool grab_token_string(::std::string& token, ::std::string& source, int32_t& from, char separator) {
  //  int32_t from_should = from;

  bool result = grab_token_string_pos(token, source, ++from, separator);

  from += token.length();

  return result;
}

bool grab_token_string_pos(::std::string& token, ::std::string& source, int32_t from, char separator) {
  ::std::string tokens = "]";
  tokens.append(1, separator);

  ::std::size_t pos = source.find_first_of(tokens, from);
  if (from < static_cast< int32_t > (source.length())) {
    pos -= from;
    token.assign(source, from, pos);
  }

  return (token.length() > 0);
}

bool grab_token_expression(::std::string& token, ::std::string& source, int32_t& from, char separator) {
  ::std::string list;
  grab_token_list_as_string(list, source, from, separator);

  token.assign("[");
  token.append(list);
  token.append("]");

  return (token.length() > 2);
}

bool grab_token_list_as_string(::std::string& token, ::std::string& source, int32_t& from, char separator) {
  ::std::string tokens = "[";
  tokens.append(1, separator);

  ::std::size_t left = source.find_first_of(tokens, from);
  ::std::size_t right = source.find_first_of(']', left);

  token.erase();
  if (left != ::std::string::npos && left < source.length()) {
    token.assign(source, left + 1, right - left - 1);
  }

  from += token.length() + 1;

  return (token.length() > 0);
}

char grab_variable_token(::std::string& str, ::std::string& token, char sec_comp, int32_t& pos, int32_t i_pos) {
  __info
    << "str: '" << str << "', sec_comp: '" << sec_comp << "', i_pos: '" << i_pos << "'";

  token.erase();

  int32_t len = static_cast< int32_t > (str.length());
  for (pos = i_pos; pos < len; pos++) {
    if ((str[pos] == '[' && pos + 1 < len) || sec_comp) {
      if (str[pos] == '[' && !sec_comp)
        pos++;
      grab_token_string_pos(token, str, pos, ':');
      pos--;

      if (token.length() > 0)
        return 1;
    }
  }

  return 0;
}

void replace_token_string(::std::string& token, ::std::string& source, int32_t pos, char separator_after,
    ::std::string& replace_by, char separator_before) {
  ::std::string replace_from;
  if (separator_before != 0)
    replace_from = separator_before;
  replace_from += token;
  if (separator_after != 0)
    replace_from += separator_after;

  ::std::string::size_type wpos;

  if ((wpos = source.find(replace_from)) != ::std::string::npos) {
    source.replace(wpos, replace_from.size(), replace_by);
  }
}

inline char simplify(char c) {
  switch (c) {
    case (char) 129:
    case (char) 150:
    case (char) 151:
    case (char) 154:
    case (char) 163:
      return 'u';
    case (char) 152:
      return 'y';
    case (char) 164:
    case (char) 165:
      return 'n';
    case (char) 131:
    case (char) 132:
    case (char) 133:
    case (char) 134:
    case (char) 142:
    case (char) 143:
    case (char) 145:
    case (char) 146:
    case (char) 160:
      return 'a';
    case (char) 130:
    case (char) 136:
    case (char) 137:
    case (char) 138:
    case (char) 144:
      return 'e';
    case (char) 139:
    case (char) 140:
    case (char) 141:
    case (char) 161:
      return 'i';
    case (char) 147:
    case (char) 148:
    case (char) 149:
    case (char) 153:
    case (char) 162:
      return 'o';
    case (char) 128:
    case (char) 135:
      return 'c';
  }
  return ::std::tolower(c);
}

inline char lower_case(char c) {
  switch (c) {
    case (char) 154:
      return (char) 129;
    case (char) 165:
      return (char) 164;
    case (char) 142:
      return (char) 132;
    case (char) 143:
      return (char) 134;
    case (char) 144:
      return (char) 130;
    case (char) 153:
      return (char) 148;
    case (char) 128:
      return (char) 135;
    case (char) 146:
      return (char) 145;
  }
  return ::std::tolower(c);
}

inline char upper_case(char c) {
  switch (c) {
    case (char) 129:
      return (char) 154;
    case (char) 164:
      return (char) 165;
    case (char) 132:
      return (char) 142;
    case (char) 134:
      return (char) 143;
    case (char) 130:
      return (char) 144;
    case (char) 148:
      return (char) 153;
    case (char) 135:
      return (char) 128;
    case (char) 145:
      return (char) 146;
  }
  return ::std::toupper(c);
}

void simplify_string(::std::string& str) {
  ::std::transform(str.begin(), str.end(), str.begin(), simplify);
}

void lower_case_string(::std::string& str) {
  ::std::transform(str.begin(), str.end(), str.begin(), lower_case);
}

void upper_case_string(::std::string& str) {
  ::std::transform(str.begin(), str.end(), str.begin(), upper_case);
}

void abbreviate_string(::std::string& str, int32_t len) {
  if (static_cast< int32_t > (str.length()) <= len) {
    return;
  }

  ::std::string lower = str;
  lower_case_string(lower);

  if (lower.find("a ") == 0) {
    str.erase(0, 2);
  } else if (lower.find("an ") == 0) {
    str.erase(0, 3);
  } else if (lower.find("the ") == 0) {
    str.erase(0, 4);
  }

  lower = str;
  lower_case_string(lower);

  for (int32_t l = str.length() - 1; l >= 1; l--) {
    if (lower[l - 1] == ' ') {
      continue;
    }

    switch (lower[l]) {
      case 'a':
      case 'e':
      case 'i':
      case 'o':
      case 'u':
        str.erase(str.begin() + l);
        if (static_cast< int32_t > (str.length()) <= len) {
          return;
        }
    }
  }

  if (static_cast< int32_t > (str.length()) > len) {
    str.resize(len);
  }
}

void capitalize_string_words(::std::string& str) {
  char conf;
  int32_t s;
  for (s = 0; s < static_cast< int32_t > (str.length()); s++) {
    conf = 0;
    if (s > 0) {
      if (str[s - 1] == ' ' || str[s - 1] == '\"')
        conf = 1;
      if (str[s - 1] == '\'') {
        //DISCOUNT SINGLE QUOTE IF IT ISN'T PRECEDED BY SPACE, COMMA OR NOTHING
        if (s <= 0)
          conf = 1;
        else if (s >= 2) {
          if (str[s - 2] == ' ' || str[s - 2] == ',')
            conf = 1;
        }
      }
    }

    if (s == 0 || conf) {
      str[s] = upper_case(str[s]);
    }
  }
}

void capitalize_string_first_word(::std::string& str) {
  char conf;
  int32_t s;
  for (s = 0; s < static_cast< int32_t > (str.length()); s++) {
    conf = 0;
    if (s > 0) {
      if (str[s - 1] == ' ' || str[s - 1] == '\"')
        conf = 1;
      if (str[s - 1] == '\'') {
        //DISCOUNT SINGLE QUOTE IF IT ISN'T PRECEDED BY SPACE, COMMA OR NOTHING
        if (s <= 0)
          conf = 1;
        else if (s >= 2) {
          if (str[s - 2] == ' ' || str[s - 2] == ',')
            conf = 1;
        }
      }
    }
    if (s == 0 || conf) {
      str[s] = upper_case(str[s]);

      if (str[s] != ' ' && str[s] != '\"')
        return;
    }
  }
}

char standardstringentry(char* str, int maxlen, unsigned int flag, ::std::set< interface_key_t >& events) {
  ::std::string str2(str);
  char ret = standardstringentry(str2, maxlen, flag, events);
  strcpy(str, str2.c_str());
  return ret;
}

char standardstringentry(::std::string& str, int maxlen, unsigned int flag, ::std::set< interface_key_t >& events) {
  unsigned char entry = 255;
  if (flag & STRINGENTRY_LETTERS) {
    if (events.count(INTERFACEKEY_STRING_A097))
      entry = 'a';
    if (events.count(INTERFACEKEY_STRING_A098))
      entry = 'b';
    if (events.count(INTERFACEKEY_STRING_A099))
      entry = 'c';
    if (events.count(INTERFACEKEY_STRING_A100))
      entry = 'd';
    if (events.count(INTERFACEKEY_STRING_A101))
      entry = 'e';
    if (events.count(INTERFACEKEY_STRING_A102))
      entry = 'f';
    if (events.count(INTERFACEKEY_STRING_A103))
      entry = 'g';
    if (events.count(INTERFACEKEY_STRING_A104))
      entry = 'h';
    if (events.count(INTERFACEKEY_STRING_A105))
      entry = 'i';
    if (events.count(INTERFACEKEY_STRING_A106))
      entry = 'j';
    if (events.count(INTERFACEKEY_STRING_A107))
      entry = 'k';
    if (events.count(INTERFACEKEY_STRING_A108))
      entry = 'l';
    if (events.count(INTERFACEKEY_STRING_A109))
      entry = 'm';
    if (events.count(INTERFACEKEY_STRING_A110))
      entry = 'n';
    if (events.count(INTERFACEKEY_STRING_A111))
      entry = 'o';
    if (events.count(INTERFACEKEY_STRING_A112))
      entry = 'p';
    if (events.count(INTERFACEKEY_STRING_A113))
      entry = 'q';
    if (events.count(INTERFACEKEY_STRING_A114))
      entry = 'r';
    if (events.count(INTERFACEKEY_STRING_A115))
      entry = 's';
    if (events.count(INTERFACEKEY_STRING_A116))
      entry = 't';
    if (events.count(INTERFACEKEY_STRING_A117))
      entry = 'u';
    if (events.count(INTERFACEKEY_STRING_A118))
      entry = 'v';
    if (events.count(INTERFACEKEY_STRING_A119))
      entry = 'w';
    if (events.count(INTERFACEKEY_STRING_A120))
      entry = 'x';
    if (events.count(INTERFACEKEY_STRING_A121))
      entry = 'y';
    if (events.count(INTERFACEKEY_STRING_A122))
      entry = 'z';
    if (events.count(INTERFACEKEY_STRING_A065))
      entry = 'A';
    if (events.count(INTERFACEKEY_STRING_A066))
      entry = 'B';
    if (events.count(INTERFACEKEY_STRING_A067))
      entry = 'C';
    if (events.count(INTERFACEKEY_STRING_A068))
      entry = 'D';
    if (events.count(INTERFACEKEY_STRING_A069))
      entry = 'E';
    if (events.count(INTERFACEKEY_STRING_A070))
      entry = 'F';
    if (events.count(INTERFACEKEY_STRING_A071))
      entry = 'G';
    if (events.count(INTERFACEKEY_STRING_A072))
      entry = 'H';
    if (events.count(INTERFACEKEY_STRING_A073))
      entry = 'I';
    if (events.count(INTERFACEKEY_STRING_A074))
      entry = 'J';
    if (events.count(INTERFACEKEY_STRING_A075))
      entry = 'K';
    if (events.count(INTERFACEKEY_STRING_A076))
      entry = 'L';
    if (events.count(INTERFACEKEY_STRING_A077))
      entry = 'M';
    if (events.count(INTERFACEKEY_STRING_A078))
      entry = 'N';
    if (events.count(INTERFACEKEY_STRING_A079))
      entry = 'O';
    if (events.count(INTERFACEKEY_STRING_A080))
      entry = 'P';
    if (events.count(INTERFACEKEY_STRING_A081))
      entry = 'Q';
    if (events.count(INTERFACEKEY_STRING_A082))
      entry = 'R';
    if (events.count(INTERFACEKEY_STRING_A083))
      entry = 'S';
    if (events.count(INTERFACEKEY_STRING_A084))
      entry = 'T';
    if (events.count(INTERFACEKEY_STRING_A085))
      entry = 'U';
    if (events.count(INTERFACEKEY_STRING_A086))
      entry = 'V';
    if (events.count(INTERFACEKEY_STRING_A087))
      entry = 'W';
    if (events.count(INTERFACEKEY_STRING_A088))
      entry = 'X';
    if (events.count(INTERFACEKEY_STRING_A089))
      entry = 'Y';
    if (events.count(INTERFACEKEY_STRING_A090))
      entry = 'Z';
  }
  if (flag & STRINGENTRY_SPACE) {
    if (events.count(INTERFACEKEY_STRING_A032))
      entry = ' ';
  }
  if (events.count(INTERFACEKEY_STRING_A000))
    entry = '\x0';
  if (flag & STRINGENTRY_NUMBERS) {
    if (events.count(INTERFACEKEY_STRING_A048))
      entry = '0';
    if (events.count(INTERFACEKEY_STRING_A049))
      entry = '1';
    if (events.count(INTERFACEKEY_STRING_A050))
      entry = '2';
    if (events.count(INTERFACEKEY_STRING_A051))
      entry = '3';
    if (events.count(INTERFACEKEY_STRING_A052))
      entry = '4';
    if (events.count(INTERFACEKEY_STRING_A053))
      entry = '5';
    if (events.count(INTERFACEKEY_STRING_A054))
      entry = '6';
    if (events.count(INTERFACEKEY_STRING_A055))
      entry = '7';
    if (events.count(INTERFACEKEY_STRING_A056))
      entry = '8';
    if (events.count(INTERFACEKEY_STRING_A057))
      entry = '9';
  }
  if (flag & STRINGENTRY_SYMBOLS) {
    if (events.count(INTERFACEKEY_STRING_A000))
      entry = 0;
    if (events.count(INTERFACEKEY_STRING_A001))
      entry = 1;
    if (events.count(INTERFACEKEY_STRING_A002))
      entry = 2;
    if (events.count(INTERFACEKEY_STRING_A003))
      entry = 3;
    if (events.count(INTERFACEKEY_STRING_A004))
      entry = 4;
    if (events.count(INTERFACEKEY_STRING_A005))
      entry = 5;
    if (events.count(INTERFACEKEY_STRING_A006))
      entry = 6;
    if (events.count(INTERFACEKEY_STRING_A007))
      entry = 7;
    if (events.count(INTERFACEKEY_STRING_A008))
      entry = 8;
    if (events.count(INTERFACEKEY_STRING_A009))
      entry = 9;
    if (events.count(INTERFACEKEY_STRING_A010))
      entry = 10;
    if (events.count(INTERFACEKEY_STRING_A011))
      entry = 11;
    if (events.count(INTERFACEKEY_STRING_A012))
      entry = 12;
    if (events.count(INTERFACEKEY_STRING_A013))
      entry = 13;
    if (events.count(INTERFACEKEY_STRING_A014))
      entry = 14;
    if (events.count(INTERFACEKEY_STRING_A015))
      entry = 15;
    if (events.count(INTERFACEKEY_STRING_A016))
      entry = 16;
    if (events.count(INTERFACEKEY_STRING_A017))
      entry = 17;
    if (events.count(INTERFACEKEY_STRING_A018))
      entry = 18;
    if (events.count(INTERFACEKEY_STRING_A019))
      entry = 19;
    if (events.count(INTERFACEKEY_STRING_A020))
      entry = 20;
    if (events.count(INTERFACEKEY_STRING_A021))
      entry = 21;
    if (events.count(INTERFACEKEY_STRING_A022))
      entry = 22;
    if (events.count(INTERFACEKEY_STRING_A023))
      entry = 23;
    if (events.count(INTERFACEKEY_STRING_A024))
      entry = 24;
    if (events.count(INTERFACEKEY_STRING_A025))
      entry = 25;
    if (events.count(INTERFACEKEY_STRING_A026))
      entry = 26;
    if (events.count(INTERFACEKEY_STRING_A027))
      entry = 27;
    if (events.count(INTERFACEKEY_STRING_A028))
      entry = 28;
    if (events.count(INTERFACEKEY_STRING_A029))
      entry = 29;
    if (events.count(INTERFACEKEY_STRING_A030))
      entry = 30;
    if (events.count(INTERFACEKEY_STRING_A031))
      entry = 31;
    if (events.count(INTERFACEKEY_STRING_A032))
      entry = 32;
    if (events.count(INTERFACEKEY_STRING_A033))
      entry = 33;
    if (events.count(INTERFACEKEY_STRING_A034))
      entry = 34;
    if (events.count(INTERFACEKEY_STRING_A035))
      entry = 35;
    if (events.count(INTERFACEKEY_STRING_A036))
      entry = 36;
    if (events.count(INTERFACEKEY_STRING_A037))
      entry = 37;
    if (events.count(INTERFACEKEY_STRING_A038))
      entry = 38;
    if (events.count(INTERFACEKEY_STRING_A039))
      entry = 39;
    if (events.count(INTERFACEKEY_STRING_A040))
      entry = 40;
    if (events.count(INTERFACEKEY_STRING_A041))
      entry = 41;
    if (events.count(INTERFACEKEY_STRING_A042))
      entry = 42;
    if (events.count(INTERFACEKEY_STRING_A043))
      entry = 43;
    if (events.count(INTERFACEKEY_STRING_A044))
      entry = 44;
    if (events.count(INTERFACEKEY_STRING_A045))
      entry = 45;
    if (events.count(INTERFACEKEY_STRING_A046))
      entry = 46;
    if (events.count(INTERFACEKEY_STRING_A047))
      entry = 47;
    if (events.count(INTERFACEKEY_STRING_A048))
      entry = 48;
    if (events.count(INTERFACEKEY_STRING_A049))
      entry = 49;
    if (events.count(INTERFACEKEY_STRING_A050))
      entry = 50;
    if (events.count(INTERFACEKEY_STRING_A051))
      entry = 51;
    if (events.count(INTERFACEKEY_STRING_A052))
      entry = 52;
    if (events.count(INTERFACEKEY_STRING_A053))
      entry = 53;
    if (events.count(INTERFACEKEY_STRING_A054))
      entry = 54;
    if (events.count(INTERFACEKEY_STRING_A055))
      entry = 55;
    if (events.count(INTERFACEKEY_STRING_A056))
      entry = 56;
    if (events.count(INTERFACEKEY_STRING_A057))
      entry = 57;
    if (events.count(INTERFACEKEY_STRING_A058))
      entry = 58;
    if (events.count(INTERFACEKEY_STRING_A059))
      entry = 59;
    if (events.count(INTERFACEKEY_STRING_A060))
      entry = 60;
    if (events.count(INTERFACEKEY_STRING_A061))
      entry = 61;
    if (events.count(INTERFACEKEY_STRING_A062))
      entry = 62;
    if (events.count(INTERFACEKEY_STRING_A063))
      entry = 63;
    if (events.count(INTERFACEKEY_STRING_A064))
      entry = 64;
    if (events.count(INTERFACEKEY_STRING_A065))
      entry = 65;
    if (events.count(INTERFACEKEY_STRING_A066))
      entry = 66;
    if (events.count(INTERFACEKEY_STRING_A067))
      entry = 67;
    if (events.count(INTERFACEKEY_STRING_A068))
      entry = 68;
    if (events.count(INTERFACEKEY_STRING_A069))
      entry = 69;
    if (events.count(INTERFACEKEY_STRING_A070))
      entry = 70;
    if (events.count(INTERFACEKEY_STRING_A071))
      entry = 71;
    if (events.count(INTERFACEKEY_STRING_A072))
      entry = 72;
    if (events.count(INTERFACEKEY_STRING_A073))
      entry = 73;
    if (events.count(INTERFACEKEY_STRING_A074))
      entry = 74;
    if (events.count(INTERFACEKEY_STRING_A075))
      entry = 75;
    if (events.count(INTERFACEKEY_STRING_A076))
      entry = 76;
    if (events.count(INTERFACEKEY_STRING_A077))
      entry = 77;
    if (events.count(INTERFACEKEY_STRING_A078))
      entry = 78;
    if (events.count(INTERFACEKEY_STRING_A079))
      entry = 79;
    if (events.count(INTERFACEKEY_STRING_A080))
      entry = 80;
    if (events.count(INTERFACEKEY_STRING_A081))
      entry = 81;
    if (events.count(INTERFACEKEY_STRING_A082))
      entry = 82;
    if (events.count(INTERFACEKEY_STRING_A083))
      entry = 83;
    if (events.count(INTERFACEKEY_STRING_A084))
      entry = 84;
    if (events.count(INTERFACEKEY_STRING_A085))
      entry = 85;
    if (events.count(INTERFACEKEY_STRING_A086))
      entry = 86;
    if (events.count(INTERFACEKEY_STRING_A087))
      entry = 87;
    if (events.count(INTERFACEKEY_STRING_A088))
      entry = 88;
    if (events.count(INTERFACEKEY_STRING_A089))
      entry = 89;
    if (events.count(INTERFACEKEY_STRING_A090))
      entry = 90;
    if (events.count(INTERFACEKEY_STRING_A091))
      entry = 91;
    if (events.count(INTERFACEKEY_STRING_A092))
      entry = 92;
    if (events.count(INTERFACEKEY_STRING_A093))
      entry = 93;
    if (events.count(INTERFACEKEY_STRING_A094))
      entry = 94;
    if (events.count(INTERFACEKEY_STRING_A095))
      entry = 95;
    if (events.count(INTERFACEKEY_STRING_A096))
      entry = 96;
    if (events.count(INTERFACEKEY_STRING_A097))
      entry = 97;
    if (events.count(INTERFACEKEY_STRING_A098))
      entry = 98;
    if (events.count(INTERFACEKEY_STRING_A099))
      entry = 99;
    if (events.count(INTERFACEKEY_STRING_A100))
      entry = 100;
    if (events.count(INTERFACEKEY_STRING_A101))
      entry = 101;
    if (events.count(INTERFACEKEY_STRING_A102))
      entry = 102;
    if (events.count(INTERFACEKEY_STRING_A103))
      entry = 103;
    if (events.count(INTERFACEKEY_STRING_A104))
      entry = 104;
    if (events.count(INTERFACEKEY_STRING_A105))
      entry = 105;
    if (events.count(INTERFACEKEY_STRING_A106))
      entry = 106;
    if (events.count(INTERFACEKEY_STRING_A107))
      entry = 107;
    if (events.count(INTERFACEKEY_STRING_A108))
      entry = 108;
    if (events.count(INTERFACEKEY_STRING_A109))
      entry = 109;
    if (events.count(INTERFACEKEY_STRING_A110))
      entry = 110;
    if (events.count(INTERFACEKEY_STRING_A111))
      entry = 111;
    if (events.count(INTERFACEKEY_STRING_A112))
      entry = 112;
    if (events.count(INTERFACEKEY_STRING_A113))
      entry = 113;
    if (events.count(INTERFACEKEY_STRING_A114))
      entry = 114;
    if (events.count(INTERFACEKEY_STRING_A115))
      entry = 115;
    if (events.count(INTERFACEKEY_STRING_A116))
      entry = 116;
    if (events.count(INTERFACEKEY_STRING_A117))
      entry = 117;
    if (events.count(INTERFACEKEY_STRING_A118))
      entry = 118;
    if (events.count(INTERFACEKEY_STRING_A119))
      entry = 119;
    if (events.count(INTERFACEKEY_STRING_A120))
      entry = 120;
    if (events.count(INTERFACEKEY_STRING_A121))
      entry = 121;
    if (events.count(INTERFACEKEY_STRING_A122))
      entry = 122;
    if (events.count(INTERFACEKEY_STRING_A123))
      entry = 123;
    if (events.count(INTERFACEKEY_STRING_A124))
      entry = 124;
    if (events.count(INTERFACEKEY_STRING_A125))
      entry = 125;
    if (events.count(INTERFACEKEY_STRING_A126))
      entry = 126;
    if (events.count(INTERFACEKEY_STRING_A127))
      entry = 127;
    if (events.count(INTERFACEKEY_STRING_A128))
      entry = 128;
    if (events.count(INTERFACEKEY_STRING_A129))
      entry = 129;
    if (events.count(INTERFACEKEY_STRING_A130))
      entry = 130;
    if (events.count(INTERFACEKEY_STRING_A131))
      entry = 131;
    if (events.count(INTERFACEKEY_STRING_A132))
      entry = 132;
    if (events.count(INTERFACEKEY_STRING_A133))
      entry = 133;
    if (events.count(INTERFACEKEY_STRING_A134))
      entry = 134;
    if (events.count(INTERFACEKEY_STRING_A135))
      entry = 135;
    if (events.count(INTERFACEKEY_STRING_A136))
      entry = 136;
    if (events.count(INTERFACEKEY_STRING_A137))
      entry = 137;
    if (events.count(INTERFACEKEY_STRING_A138))
      entry = 138;
    if (events.count(INTERFACEKEY_STRING_A139))
      entry = 139;
    if (events.count(INTERFACEKEY_STRING_A140))
      entry = 140;
    if (events.count(INTERFACEKEY_STRING_A141))
      entry = 141;
    if (events.count(INTERFACEKEY_STRING_A142))
      entry = 142;
    if (events.count(INTERFACEKEY_STRING_A143))
      entry = 143;
    if (events.count(INTERFACEKEY_STRING_A144))
      entry = 144;
    if (events.count(INTERFACEKEY_STRING_A145))
      entry = 145;
    if (events.count(INTERFACEKEY_STRING_A146))
      entry = 146;
    if (events.count(INTERFACEKEY_STRING_A147))
      entry = 147;
    if (events.count(INTERFACEKEY_STRING_A148))
      entry = 148;
    if (events.count(INTERFACEKEY_STRING_A149))
      entry = 149;
    if (events.count(INTERFACEKEY_STRING_A150))
      entry = 150;
    if (events.count(INTERFACEKEY_STRING_A151))
      entry = 151;
    if (events.count(INTERFACEKEY_STRING_A152))
      entry = 152;
    if (events.count(INTERFACEKEY_STRING_A153))
      entry = 153;
    if (events.count(INTERFACEKEY_STRING_A154))
      entry = 154;
    if (events.count(INTERFACEKEY_STRING_A155))
      entry = 155;
    if (events.count(INTERFACEKEY_STRING_A156))
      entry = 156;
    if (events.count(INTERFACEKEY_STRING_A157))
      entry = 157;
    if (events.count(INTERFACEKEY_STRING_A158))
      entry = 158;
    if (events.count(INTERFACEKEY_STRING_A159))
      entry = 159;
    if (events.count(INTERFACEKEY_STRING_A160))
      entry = 160;
    if (events.count(INTERFACEKEY_STRING_A161))
      entry = 161;
    if (events.count(INTERFACEKEY_STRING_A162))
      entry = 162;
    if (events.count(INTERFACEKEY_STRING_A163))
      entry = 163;
    if (events.count(INTERFACEKEY_STRING_A164))
      entry = 164;
    if (events.count(INTERFACEKEY_STRING_A165))
      entry = 165;
    if (events.count(INTERFACEKEY_STRING_A166))
      entry = 166;
    if (events.count(INTERFACEKEY_STRING_A167))
      entry = 167;
    if (events.count(INTERFACEKEY_STRING_A168))
      entry = 168;
    if (events.count(INTERFACEKEY_STRING_A169))
      entry = 169;
    if (events.count(INTERFACEKEY_STRING_A170))
      entry = 170;
    if (events.count(INTERFACEKEY_STRING_A171))
      entry = 171;
    if (events.count(INTERFACEKEY_STRING_A172))
      entry = 172;
    if (events.count(INTERFACEKEY_STRING_A173))
      entry = 173;
    if (events.count(INTERFACEKEY_STRING_A174))
      entry = 174;
    if (events.count(INTERFACEKEY_STRING_A175))
      entry = 175;
    if (events.count(INTERFACEKEY_STRING_A176))
      entry = 176;
    if (events.count(INTERFACEKEY_STRING_A177))
      entry = 177;
    if (events.count(INTERFACEKEY_STRING_A178))
      entry = 178;
    if (events.count(INTERFACEKEY_STRING_A179))
      entry = 179;
    if (events.count(INTERFACEKEY_STRING_A180))
      entry = 180;
    if (events.count(INTERFACEKEY_STRING_A181))
      entry = 181;
    if (events.count(INTERFACEKEY_STRING_A182))
      entry = 182;
    if (events.count(INTERFACEKEY_STRING_A183))
      entry = 183;
    if (events.count(INTERFACEKEY_STRING_A184))
      entry = 184;
    if (events.count(INTERFACEKEY_STRING_A185))
      entry = 185;
    if (events.count(INTERFACEKEY_STRING_A186))
      entry = 186;
    if (events.count(INTERFACEKEY_STRING_A187))
      entry = 187;
    if (events.count(INTERFACEKEY_STRING_A188))
      entry = 188;
    if (events.count(INTERFACEKEY_STRING_A189))
      entry = 189;
    if (events.count(INTERFACEKEY_STRING_A190))
      entry = 190;
    if (events.count(INTERFACEKEY_STRING_A191))
      entry = 191;
    if (events.count(INTERFACEKEY_STRING_A192))
      entry = 192;
    if (events.count(INTERFACEKEY_STRING_A193))
      entry = 193;
    if (events.count(INTERFACEKEY_STRING_A194))
      entry = 194;
    if (events.count(INTERFACEKEY_STRING_A195))
      entry = 195;
    if (events.count(INTERFACEKEY_STRING_A196))
      entry = 196;
    if (events.count(INTERFACEKEY_STRING_A197))
      entry = 197;
    if (events.count(INTERFACEKEY_STRING_A198))
      entry = 198;
    if (events.count(INTERFACEKEY_STRING_A199))
      entry = 199;
    if (events.count(INTERFACEKEY_STRING_A200))
      entry = 200;
    if (events.count(INTERFACEKEY_STRING_A201))
      entry = 201;
    if (events.count(INTERFACEKEY_STRING_A202))
      entry = 202;
    if (events.count(INTERFACEKEY_STRING_A203))
      entry = 203;
    if (events.count(INTERFACEKEY_STRING_A204))
      entry = 204;
    if (events.count(INTERFACEKEY_STRING_A205))
      entry = 205;
    if (events.count(INTERFACEKEY_STRING_A206))
      entry = 206;
    if (events.count(INTERFACEKEY_STRING_A207))
      entry = 207;
    if (events.count(INTERFACEKEY_STRING_A208))
      entry = 208;
    if (events.count(INTERFACEKEY_STRING_A209))
      entry = 209;
    if (events.count(INTERFACEKEY_STRING_A210))
      entry = 210;
    if (events.count(INTERFACEKEY_STRING_A211))
      entry = 211;
    if (events.count(INTERFACEKEY_STRING_A212))
      entry = 212;
    if (events.count(INTERFACEKEY_STRING_A213))
      entry = 213;
    if (events.count(INTERFACEKEY_STRING_A214))
      entry = 214;
    if (events.count(INTERFACEKEY_STRING_A215))
      entry = 215;
    if (events.count(INTERFACEKEY_STRING_A216))
      entry = 216;
    if (events.count(INTERFACEKEY_STRING_A217))
      entry = 217;
    if (events.count(INTERFACEKEY_STRING_A218))
      entry = 218;
    if (events.count(INTERFACEKEY_STRING_A219))
      entry = 219;
    if (events.count(INTERFACEKEY_STRING_A220))
      entry = 220;
    if (events.count(INTERFACEKEY_STRING_A221))
      entry = 221;
    if (events.count(INTERFACEKEY_STRING_A222))
      entry = 222;
    if (events.count(INTERFACEKEY_STRING_A223))
      entry = 223;
    if (events.count(INTERFACEKEY_STRING_A224))
      entry = 224;
    if (events.count(INTERFACEKEY_STRING_A225))
      entry = 225;
    if (events.count(INTERFACEKEY_STRING_A226))
      entry = 226;
    if (events.count(INTERFACEKEY_STRING_A227))
      entry = 227;
    if (events.count(INTERFACEKEY_STRING_A228))
      entry = 228;
    if (events.count(INTERFACEKEY_STRING_A229))
      entry = 229;
    if (events.count(INTERFACEKEY_STRING_A230))
      entry = 230;
    if (events.count(INTERFACEKEY_STRING_A231))
      entry = 231;
    if (events.count(INTERFACEKEY_STRING_A232))
      entry = 232;
    if (events.count(INTERFACEKEY_STRING_A233))
      entry = 233;
    if (events.count(INTERFACEKEY_STRING_A234))
      entry = 234;
    if (events.count(INTERFACEKEY_STRING_A235))
      entry = 235;
    if (events.count(INTERFACEKEY_STRING_A236))
      entry = 236;
    if (events.count(INTERFACEKEY_STRING_A237))
      entry = 237;
    if (events.count(INTERFACEKEY_STRING_A238))
      entry = 238;
    if (events.count(INTERFACEKEY_STRING_A239))
      entry = 239;
    if (events.count(INTERFACEKEY_STRING_A240))
      entry = 240;
    if (events.count(INTERFACEKEY_STRING_A241))
      entry = 241;
    if (events.count(INTERFACEKEY_STRING_A242))
      entry = 242;
    if (events.count(INTERFACEKEY_STRING_A243))
      entry = 243;
    if (events.count(INTERFACEKEY_STRING_A244))
      entry = 244;
    if (events.count(INTERFACEKEY_STRING_A245))
      entry = 245;
    if (events.count(INTERFACEKEY_STRING_A246))
      entry = 246;
    if (events.count(INTERFACEKEY_STRING_A247))
      entry = 247;
    if (events.count(INTERFACEKEY_STRING_A248))
      entry = 248;
    if (events.count(INTERFACEKEY_STRING_A249))
      entry = 249;
    if (events.count(INTERFACEKEY_STRING_A250))
      entry = 250;
    if (events.count(INTERFACEKEY_STRING_A251))
      entry = 251;
    if (events.count(INTERFACEKEY_STRING_A252))
      entry = 252;
    if (events.count(INTERFACEKEY_STRING_A253))
      entry = 253;
    if (events.count(INTERFACEKEY_STRING_A254))
      entry = 254;
    if (events.count(INTERFACEKEY_STRING_A255))
      entry = 255;
  }

  if (entry != 255) {
    if (entry == '\x0') {
      if (str.length() > 0)
        str.resize(str.length() - 1);
    } else {
      int cursor = str.length();
      if (cursor >= maxlen)
        cursor = maxlen - 1;
      if (cursor < 0)
        cursor = 0;

      if (str.length() < cursor + 1)
        str.resize(cursor + 1);

      if (entry >= 'a' && entry <= 'z' && (flag & STRINGENTRY_CAPS))
        str[cursor] = entry + 'A' - 'a';
      else
        str[cursor] = entry;
    }

    return 1;
  }

  return 0;
}

// TODO: comment
// TODO: categorize
// Map DF's CP437 to Unicode
// see: http://dwarffortresswiki.net/index.php/Character_table
int charmap[256] = {
  ' ', 0x263A, 0x263B, 0x2665, 0x2666, 0x2663, 0x2660, 0x2022,
  0x25D8, 0x25CB, 0x25D9, 0x2642, 0x2640, 0x266A, 0x266B, 0x263C,
  0x25BA, 0x25C4, 0x2195, 0x203C, 0x00B6, 0x00A7, 0x25AC, 0x21A8,
  0x2191, 0x2193, 0x2192, 0x2190, 0x221F, 0x2194, 0x25B2, 0x25BC,
  /* 0x20 */
  0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
  0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
  0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
  0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
  0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
  0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x2302,
  /* 0x80 */
  0xC7, 0xFC, 0xE9, 0xE2, 0xE4, 0xE0, 0xE5, 0xE7,
  0xEA, 0xEB, 0xE8, 0xEF, 0xEE, 0xEC, 0xC4, 0xC5,
  0xC9, 0xE6, 0xC6, 0xF4, 0xF6, 0xF2, 0xFB, 0xF9,
  0xFF, 0xD6, 0xDC, 0xA2, 0xA3, 0xA5, 0x20A7, 0x192,
  0xE1, 0xED, 0xF3, 0xFA, 0xF1, 0xD1, 0xAA, 0xBA,
  0xBF, 0x2310, 0xAC, 0xBD, 0xBC, 0xA1, 0xAB, 0xBB,
  0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556,
  0x2555, 0x2563, 0x2551, 0x2557, 0x255D, 0x255C, 0x255B, 0x2510,
  0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x255E, 0x255F,
  0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x2567,
  0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256B,
  0x256A, 0x2518, 0x250C, 0x2588, 0x2584, 0x258C, 0x2590, 0x2580,
  0x3B1, 0xDF/*yay*/, 0x393, 0x3C0, 0x3A3, 0x3C3, 0xB5, 0x3C4,
  0x3A6, 0x398, 0x3A9, 0x3B4, 0x221E, 0x3C6, 0x3B5, 0x2229,
  0x2261, 0xB1, 0x2265, 0x2264, 0x2320, 0x2321, 0xF7, 0x2248,
  0xB0, 0x2219, 0xB7, 0x221A, 0x207F, 0xB2, 0x25A0, 0xA0
};
