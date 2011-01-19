/**
 * @file
 * @date 6 juin 2010
 * @todo comment
 */

#ifndef EXTERN_UTILS_STRINGS_HPP_
#define EXTERN_UTILS_STRINGS_HPP_

#include <set>

#include <string>
#include "integer_types.hpp"
#include "event_types.hpp"

/**
 * @{
 * @ingroup extern
 */
/**
 * @brief Get the first @a token in @a source, terminated by @a separator, ']' or end of string.
 * @param[out] token the returned token
 * @param[in] source the source string
 * @param[in] separator the separator char
 * @retval true if a token was found
 * @retval false otherwise
 */
bool grab_token_string(::std::string& token, ::std::string& source, char separator = ':');

/**
 * @brief Get the next @a token in @a source, starting at @a position + 1 and terminated by @a separator, ']' or end of string.
 * @param[out] token the returned token
 * @param[in] source the source string
 * @param[in] position the current position in @a source
 * @param[out] position the updated position in @a source, incremented by @a token length.
 * @param[in] separator the separator char
 * @retval true if a token was found
 * @retval false otherwise
 */
bool grab_token_string(::std::string& token, ::std::string& source, int32_t& position, char separator = ':');

/**
 * @brief Get the next @a token in @a source, starting at @a position + 1 and terminated by @a separator, ']' or end of string.
 * @param[out] token the returned token
 * @param[in] source the source string
 * @param[in] position the current position in @a source
 * @param[in] separator the separator char
 * @retval true if a token was found
 * @retval false otherwise
 */
bool grab_token_string_pos(::std::string& token, ::std::string& source, int32_t position, char separator = ':');

/**
 * @brief Get the first token @a expression in @a source, starting at @a position, with each token separated with @a separator, wrapping the expression between '[' and ']'.
 *        Any token before @a position is skipped.
 * @param[out] expression the returned expression
 * @param[in] source the source string
 * @param[in] position the current position in @a source
 * @param[out] position the updated position in @a source, incremented by @a expression length.
 * @param[in] separator the separator char
 * @retval true if an expression was found
 * @retval false otherwise
 */
bool grab_token_expression(::std::string& expression, ::std::string& source, int32_t& position, char separator = ':');

/**
 * @brief Get the first token @a expression in @a source, starting at @a position, with each token separated with @a separator, triming the wrapping '[' and ']'.
 *        Any token before @a position is skipped.
 * @param[out] expression the returned expression
 * @param[in] source the source string
 * @param[in] position the current position in @a source
 * @param[out] position the updated position in @a source, incremented by @a expression length.
 * @param[in] separator the separator char
 * @retval true if an expression was found
 * @retval false otherwise
 */
bool grab_token_list_as_string(::std::string& expression, ::std::string& source, int32_t& position, char separator = ':');

char grab_variable_token(::std::string& expression, ::std::string& source, char separator, int32_t& position, int32_t start_position);

void replace_token_string(::std::string& token, ::std::string& source, int32_t position, char separator_before, ::std::string& replacement, char separator_after);

void simplify_string(::std::string&);
void abbreviate_string(::std::string&, int32_t);

void lower_case_string(::std::string&);
void upper_case_string(::std::string&);

void capitalize_string_words(::std::string&);
void capitalize_string_first_word(::std::string&);

/**
 * @{
 * @brief stringentry functions flags
 */
#define STRINGENTRY_LETTERS (1 << 0)
#define STRINGENTRY_SPACE   (1 << 1)
#define STRINGENTRY_NUMBERS (1 << 2)
#define STRINGENTRY_CAPS    (1 << 3)
#define STRINGENTRY_SYMBOLS (1 << 4)
/** @} */

/**
 * @brief Translates a set of character interface events into a char array.
 */
char standardstringentry(char* str, int maxlen, unsigned int flag, ::std::set< interface_key_t >& events);
/**
 * @brief Translates a set of character interface events into a char array.
 */
char standardstringentry(::std::string& str, int maxlen, unsigned int flag, ::std::set< interface_key_t >& events);
/** @} */

// TODO: comment
// TODO: categorize
enum justification: unsigned char {
  justify_left, justify_center, justify_right,
    justify_cont,
  not_truetype
};

// TODO: comment
// TODO: categorize
extern int charmap[256];

#endif /* EXTERN_UTILS_STRINGS_HPP_ */
