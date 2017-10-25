#include "token.h"
#include "lex.h"
#include <format.h>
#include <climits>

namespace Miyuki::Proc {

    using namespace fmt;

    Lexer::Lexer(FileReadPtr frptr) {
        M_fr = frptr;
    }

    TokenPtr Lexer::scan() {
        // Eat comments and wite spaces
        for (readch(); ; readch()) {
            if (peak == Tag::EndOfFile) return make_shared<Token>(Tag::EndOfFile);
            else if (peak == ' ' || peak == '\t' || peak == '\r' || peak == '\n') continue;
            else if (peak == '/') {
                readch();
                if (peak == '/') {
                    for (; ; readch()) {
                        if (peak == '\n')  break;
                        if (peak == Tag::EndOfFile)   return make_shared<Token>(Tag::EndOfFile);
                    }
                }
                else if (peak == '*') {
                    readch();
                    for (; ; readch()) {
                        if (peak == Tag::EndOfFile)   throw SyntaxError("Comment not closed.");
                        if (peak == '*') {
                            readch();
                            if (peak == '/') break;
                            else             retract();
                        }
                    }
                }
                else {
                    // if code runs here, it represent that the '/' is as a divide instead of a comment
                    // So break here
                    retract();
                    break;
                }
            }
            else
                break;
        }

        Token::startColumn = M_fr->getColumn();

        // Integer and floating constant
        long double floating;
        uint64_t intValue = 0;
        int ary;
        bool hasUnsignedSuffix = false;
        bool hasLongSuffix = false;
        bool hasLongLongSuffix = false;
        bool hasFloatingSuffix = false;
        bool suffixInvalid = false;
        string suffix = "";

        if (isdigit(peak)) {
            if (peak != '0') {
                // decimal-constant
                ary = 10;
                for (; isdigit(peak); readch())
                    intValue = intValue * ary + peak - '0';
            }
            else {
                readch();
                if (peak == 'x' || peak == 'X') {
                    // hexadecimal-constant
                    ary = 16;
                    for (readch(); isdigit(peak) || (peak >= 'A' && peak <= 'F') || (peak >= 'a' && peak <= 'f'); readch() )
                        intValue = intValue * ary + (isdigit(peak) ? peak - '0' : 10 + (peak >= 'A' && peak <= 'F' ? peak - 'A' : peak - 'a'));
                }
                else {
                    // octal-constant
                    ary = 8;
                    for (; isdigit(peak); readch())
                        intValue = intValue * ary + peak - '0';
                }
            }

            // if next char is dot it is a floting-constant
            if (peak == '.')  ; // do nothing
            // floating (hexadecimal)
            else if (peak == 'e') {
                floating = intValue;
add_exponment:
                int exponment = 0;  //faction * 1/16
                bool positive = true;
                if (readch(); peak == '-' || peak == '+') { positive = peak == '+'; readch(); }
                if (!isdigit(peak)) throw SyntaxError("exponent has no digits");
                for (; isdigit(peak); readch())
                   exponment = exponment * 10 + peak - '0';
                for (int i=0; i<exponment; i++) {
                    if (positive) floating *= ary;
                    else floating /= ary;
                }
                // Add floating-constant suffix below
                goto add_floating_suffix;
            }
            // integer-suffix
            else  {
                for (; isalnum(peak) ; readch()) {
                    suffix += peak;
                    if (suffixInvalid) continue;
                    if ((peak == 'u' || peak == 'U') && !hasUnsignedSuffix)
                        hasUnsignedSuffix = true;
                    else if ((peak == 'l' || peak == 'L') && !(hasLongLongSuffix || hasLongSuffix)) {
                        char save = peak; readch();
                        if (peak == save) { suffix += peak; hasLongLongSuffix = true; }
                        else { retract(); hasLongSuffix = true; }
                    }
                    else if ((peak == 'f' || peak == 'F') && !hasFloatingSuffix)
                        hasFloatingSuffix = true;
                    else suffixInvalid = true;
                }

                if (suffixInvalid) throw SyntaxError("Invalid integer suffix '{0}'."_format(suffix));
                if (hasFloatingSuffix) {
                    if (hasUnsignedSuffix || hasLongLongSuffix || hasLongSuffix) throw SyntaxError("Invalid floating constant suffix '{0}'."_format(suffix));
                    floating = intValue;
                    goto convert_to_floating_here;
                }

                short bit;
                if (hasLongLongSuffix) bit = 64;
                else if (hasLongSuffix) bit = 32;
                else if ((hasUnsignedSuffix && intValue < INT16_MAX) || (!hasUnsignedSuffix && intValue < INT16_MAX / 2) ) bit = 16;
                else bit = 32;

                retract();
                return make_shared<IntToken>(intValue, hasUnsignedSuffix, bit);
            }

        }

        // Floating-constant (decimal)
        if (peak == '.') {
            readch(); long double decimalPart;
            if (intValue == 0 && ary == 8) ary = 10;
            if (ary == 8)  throw SyntaxError("Invalid number.");
            else if (ary == 10) {
                long double fraction = 0.0, factor = 0.1;
                int exponment = 0;
                for (; isdigit(peak); readch()) {
                    fraction = fraction + (peak - '0') * factor;
                    factor = factor / 10;
                }
                if (peak == '.') throw SyntaxError("too many decimal points in number");
                if (peak == 'e' || peak == 'E')  { floating = intValue + fraction;  goto add_exponment; }
                decimalPart = fraction;
            }
            else if (ary == 16) {
                int fraction = 0, exponment = 0;  //faction * 1/16
                bool positive = true;
                for (; isdigit(peak) || (peak >= 'A' && peak <= 'F') || (peak >= 'a' && peak <= 'f'); readch() )
                    fraction = fraction * 16 + (isdigit(peak) ? peak - '0' : 10 + (peak >= 'A' && peak <= 'F' ? peak - 'A' : peak - 'a'));
                if (peak == '.') throw SyntaxError("too many decimal points in number");
                if (peak != 'p' && peak != 'P') throw SyntaxError("hexadecimal floating constants require an exponent");
                if (readch(); peak == '-' || peak == '+') { positive = peak == '+'; readch(); }
                for (; isdigit(peak); readch())
                    exponment = exponment * 10 + peak - '0';
                if (positive) decimalPart = (long double)fraction / 16 * (1 << exponment);
                else          decimalPart = (long double)fraction / 16 / (1 << exponment);
            }
            floating = ((long double)intValue) + decimalPart;
add_floating_suffix:
            // floting-constant suffix
            for (; isalnum(peak) ; readch()) {
                suffix += peak;
                if (suffixInvalid) continue;
                if ((peak == 'f' || peak == 'F') && !(hasFloatingSuffix || hasLongSuffix))
                    hasFloatingSuffix = true;
                else if ((peak == 'l' || peak == 'L') && !(hasFloatingSuffix || hasLongSuffix))
                    hasLongSuffix = true;
                else suffixInvalid = true;
            }
convert_to_floating_here:
            if (suffixInvalid) throw SyntaxError("Invalid floating constant suffix '{0}'."_format(suffix));

            short bit;
            if (hasFloatingSuffix) bit = 32;
            else if (hasLongSuffix) bit = 64;
            else bit = (*((uint64_t *)(&floating)) >> 32) ? 32 : 64;

            if (peak == '.') throw SyntaxError("too many decimal points in number");

            retract();
            return make_shared<FloatToken>(floating, bit);
        }

        // Identifiers and Keywords
        if (isalpha(peak) || peak == '_' || peak == '\\') {
            string word = "";
            if (peak == 'L' || peak == 'u' || peak == 'U') {
                // Character-constant starts with L, u, and U
                if (readch(); peak == '\'') goto scan_character;
                else retract();
            }
            for (; isalnum(peak) || peak == '_' || peak == '\\'; readch()) {
                if (peak == '\\') {
                    throw SyntaxError("My implementation does not support universal character name.");
                }
                word += peak;
            }
            retract();
            if (word == "alignof")  return make_shared<WordToken>(Tag::Alignof, word);
            if (word == "auto")  return make_shared<WordToken>(Tag::Auto, word);
            if (word == "asm")  return make_shared<WordToken>(Tag::Asm, word);
            if (word == "break")  return make_shared<WordToken>(Tag::Break, word);
            if (word == "case")  return make_shared<WordToken>(Tag::Case, word);
            if (word == "char")  return make_shared<WordToken>(Tag::Char, word);
            if (word == "const")  return make_shared<WordToken>(Tag::Const, word);
            if (word == "continue")  return make_shared<WordToken>(Tag::Continue, word);
            if (word == "default")  return make_shared<WordToken>(Tag::Default, word);
            if (word == "do")  return make_shared<WordToken>(Tag::Do, word);
            if (word == "double")  return make_shared<WordToken>(Tag::Double, word);
            if (word == "else")  return make_shared<WordToken>(Tag::Else, word);
            if (word == "enum")  return make_shared<WordToken>(Tag::Enum, word);
            if (word == "extern")  return make_shared<WordToken>(Tag::Extern, word);
            if (word == "float")  return make_shared<WordToken>(Tag::Float, word);
            if (word == "for")  return make_shared<WordToken>(Tag::For, word);
            if (word == "goto")  return make_shared<WordToken>(Tag::Goto, word);
            if (word == "if")  return make_shared<WordToken>(Tag::If, word);
            if (word == "inline")  return make_shared<WordToken>(Tag::Inline, word);
            if (word == "int")  return make_shared<WordToken>(Tag::Int, word);
            if (word == "long")  return make_shared<WordToken>(Tag::Long, word);
            if (word == "register")  return make_shared<WordToken>(Tag::Register, word);
            if (word == "restrict")  return make_shared<WordToken>(Tag::Restrict, word);
            if (word == "return")  return make_shared<WordToken>(Tag::Return, word);
            if (word == "short")  return make_shared<WordToken>(Tag::Short, word);
            if (word == "signed")  return make_shared<WordToken>(Tag::Signed, word);
            if (word == "sizeof")  return make_shared<WordToken>(Tag::Sizeof, word);
            if (word == "static")  return make_shared<WordToken>(Tag::Static, word);
            if (word == "struct")  return make_shared<WordToken>(Tag::Struct, word);
            if (word == "switch")  return make_shared<WordToken>(Tag::Switch, word);
            if (word == "typedef")  return make_shared<WordToken>(Tag::Typedef, word);
            if (word == "union")  return make_shared<WordToken>(Tag::Union, word);
            if (word == "unsigned")  return make_shared<WordToken>(Tag::Unsigned, word);
            if (word == "void")  return make_shared<WordToken>(Tag::Void, word);
            if (word == "volatile")  return make_shared<WordToken>(Tag::Volatile, word);
            if (word == "while")  return make_shared<WordToken>(Tag::While, word);
            return make_shared<WordToken>(word);
        }

        // Character constant
        //  TODO: Encoding
        if (peak == '\'' || peak == 'L' || peak == 'u' || peak == 'U') {
            if (peak == 'L' || peak == 'u' || peak == 'U') readch();
scan_character:
            string chrseq;
            readch();
            for (; ; readch()) {
                if (peak == '\'') break;
                else if (peak == '\r' || peak == '\n') throw SyntaxError("Unexpected new-line");
                else chrseq += _getCharFromSlash();
            }
            uint64_t x = 0;
            for (int i=chrseq.length() - 1; i>=0; i--) {
                x = (x << 8) | chrseq[i];
            }
            return make_shared<CharToken>(x);
        }

        // String Literal
        //  TODO: Encoding
        if (peak == '"' || peak == 'L' || peak == 'u' || peak == 'U') {
            if (peak == 'L' || peak == 'u' || peak == 'U') {
                readch();
                if (peak == '8') readch();  // Skip "
            }
            string chrseq;
            readch();
            for (; ; readch()) {
                if (peak == '"') break;
                else if (peak == '\r' || peak == '\n') throw SyntaxError("Unexpected new-line");
                else chrseq += _getCharFromSlash();
            }
            return make_shared<StringToken>(chrseq);
        }

        return make_shared<Token>(peak);
    }

    Lexer::~Lexer() {

    }

    char Lexer::_getCharFromSlash() {
        short chr;
        if (peak == '\\') {
            readch();
            if (peak == '\'' || peak == '\"' || peak == '?' || peak == '\\') chr = peak;
            else if (peak == 'a') chr = '\a';
            else if (peak == 'b') chr = '\b';
            else if (peak == 'f') chr = '\f';
            else if (peak == 'n') chr = '\n';
            else if (peak == 'r') chr = '\r';
            else if (peak == 't') chr = '\t';
            else if (peak == 'v') chr = '\v';
            else if (peak >= '0' && peak <= '7') {
                // octal-digit
                short x = peak - '0'; readch();
                if (peak >= '0' && peak <= '7') {
                    x = x * 8 + peak - '0'; readch();
                    if (peak >= '0' && peak <= '7') {
                        x = x * 8 + peak - '0';
                    }
                }
                chr = x;
            }
            else if (peak == 'x') {
                // hexadecimal-escape-sequence
                short hex = 0;
                readch();
                if (!(isdigit(peak) || (peak >= 'a' && peak <= 'f') || (peak >= 'A' && peak <= 'D')))
                    throw SyntaxError("Invalid hexadecimal-escape-sequence");
                for (; isdigit(peak) || (peak >= 'a' && peak <= 'f') || (peak >= 'A' && peak <= 'D'); readch()) {
                    hex = hex * 16 + peak - '0';
                }
                retract();
                chr = hex;
            }
            else throw SyntaxError("Invalid escape sequence");
        }
        else chr = peak;
        return chr;
    }
}
