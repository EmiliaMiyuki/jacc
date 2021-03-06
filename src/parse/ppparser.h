#ifndef _MIYUKI_PPPARSER_H
#define _MIYUKI_PPPARSER_H

#include "ifparser.h"
#include "lex/pplex.h"
#include "lex/imlex.h"
#include "ppcond.h"
#include <unordered_map>

namespace Miyuki::Parse {

    using namespace Lex;

    DEFINE_SHARED_PTR(MacroDefine)
    DEFINE_SHARED_PTR(MacroPack)
    DEFINE_SHARED_PTR(Macro)
    DEFINE_SHARED_PTR(FunctionLikeMacro)
    DEFINE_SHARED_PTR(GroupPart)

    // macro defination.
    class MacroDefine {
    public:
        string macroName;
        bool   isFunctionLike;
        // for function-like macros
        bool   isParamVarible;
        deque<WordTokenPtr> lparlen;
        TokenSequence replacement;

        int getParamIndex(string& name) {
            for (int i=0; i<lparlen.size(); i++)
                if (lparlen[i]->name == name)
                    return i;
            return -1;
        }
    };

    // store a pack of macro
    class MacroPack {
        static MacroPack* instance;
    public:
        unordered_map<string, MacroDefinePtr> macros;

        MacroPack() { instance = this; }

        static MacroPack* &getInstance() { return instance; }
        MacroDefinePtr getMacroDef(string name, bool isFunctionLike) { // FIXME: remove? maybe useless
            auto it = macros.find(name);
            if (it != macros.end() && it->second->isFunctionLike == isFunctionLike) return it->second;
            return nullptr;
        }
        MacroDefinePtr getMacroDef(string name) {
            auto it = macros.find(name);
            if (it != macros.end()) return it->second;
            return nullptr;
        }
        bool removeMacroDef(string name) {
            auto it = macros.find(name);
            if (it != macros.end()) {
                macros.erase(it);
                return true;
            }
            return false;
        }
        bool addMacro(const string& name, MacroDefinePtr def) {
            // if macro is exist
            if ( getMacroDef(name) )  return false;
            macros.insert(make_pair<string, MacroDefinePtr>( string(name) , std::move(def) ));
            return true;
        }
    };

    // use macro (directly use or call)
    class Macro {
    public:
        TokenPtr macroName;
        MacroDefinePtr defination;
        bool isFunctionLike;

        explicit Macro(MacroDefinePtr def) {
            defination = def;
        }
        // Incremental add tokens to toksResult
        // return values: number of token replaced.
        virtual int replace(TokenSequence& toksResult) {
            // copy replacement to token sequence, same as replacement
            size_t replen = defination->replacement.size();
            for (int i=0; i<replen; i++) {
                toksResult.push_back(defination->replacement[i]);
            }
            return 1;
        }
    };

    // group-part (ref A.2.4)
    class GroupPart {
    public:
        TokenPtr directiveTok = nullptr;
        uint32_t kind;

        enum : uint32_t {
            Include = 0, If, Ifndef, Ifdef, Elif, Else, Endif, Define, Undef, Line, Error, Pragma, Empty, TextLine
        };

        GroupPart(uint32_t _kind) : kind(_kind) {  }

        bool is(uint32_t _tag) { return kind == _tag; }
    };

    class FunctionLikeMacro : public Macro {
    public:
        deque<TokenSequencePtr> params;

        FunctionLikeMacro(MacroDefinePtr def) : Macro(def) {

        }

        bool isParameter(string name) {
            if (name == "__VA_ARGS__")
                return true;
            for (WordTokenPtr tok : defination->lparlen) {
                if (tok->name == name)
                    return true;
            }
            return false;
        }

        // get parameter name from value
        //   value structure is unnamed, so we should get names structure and map them
        //   to find the value we want,
        //   In addition, we do special process on __VA_ARGS__ macro, if we know it is
        //   __VA_ARGS__, we marge all item in max-named-param size
        //   If there is a ... in the identifier-list in the macro definition, then the trailing arguments,
        //   including any separating comma preprocessing tokens, are merged to form a single item:
        //   the variable arguments. The number of arguments so combined is such that, following
        //   merger, the number of arguments is one more than the number of parameters in the macro
        //   definition (excluding the ...).
        TokenSequencePtr getParameterValue(string& name);

        // note: when add replacement list, add an additional 0 for convience
        int replace(TokenSequence& toksResult) override;
    };

    // This file defined preprocessor parser
    class PreprocessorParser : public IParser {
        // Preprocessor lexer: get preprocessor token from it
        PreprocessorLexerPtr M_pplex;

        // Intermediate lexer: replace token from pptoken to normal token to parse
        IntermediateLexerPtr M_imlex;

        // Save a line of Token, read token until new-line or EOF (*), and try replace on
        // this line of tokens, until no replacement being taken, write to file and start
        // next cache.
        // for command line: skip # and token with special meaning,
        // for text line: Recursively descend to read the structure, util encount a new-line
        // note2; structure for text-line has only one situation. function call.
        //    no matter it is or function or a function-like macro.
        TokenSequencePtr cachedLine;

        // we store calculated value (calulated by evalCache from cacheLine)
        // and the value will be return to parser
        TokenSequencePtr evaledToks;

        // iterator to evaledToks,
        // used to point to the next token that should be returned in evaledToks
        TokenSequenceIter evaledToksIter;

        // macro defined.
        MacroPack macros;

        // group part type info
        GroupPartPtr groupPart;

        // condition stack for #if #ifdef and #ifndef hierarchy
        ConditionHierarchy condHierarchy;

        // condition operations
        bool getCondition();
        bool setCondition(bool c);
        bool negateCondition();
        bool endCurrentCondition();
        void addNewCondition(bool c);

        // Process group-parts
        void processIf();
        void processIfdef();
        void processIfndef();
        void processElif();
        void processElse();
        void processEndif();
        void processDefine();
        void processUndef();
        void processLine();
        void processError();
        void processPragma();
        void processTextline();
        void processInclude();

    private:
        // get group-part kind from opTok
        // return value: true if is a valid group part,
        //               false if invalid
        bool setGroupPart(TokenPtr op);

        // cache a line with full syntax meaning from lexer
        // return value: true: there're lines did not read
        //               false: there's no line did not read
        // dide effect:  set cachedLine, if the value is nullptr,
        //               means the value is invalid, and no more line to read
        bool getCache();

        // calculate values
        TokenSequencePtr eval(TokenSequencePtr seq);

        // from cache, eval value or execute, replace any macros
        void evalCachedLine();

        // convert preprocessor tokens to parser token (from cachedLine)
        //   convert PPLiterial to corresponding token
        void convertToken();

    public:
        explicit PreprocessorParser(const char * path) : M_pplex(make_shared<PreprocessorLexer>()), M_imlex(make_shared<IntermediateLexer>()) {
            M_pplex->openFile(path);
            M_lex = M_pplex;
            registerObserver();
        }

        void testLexer();
        void parse() final;

        // return tokens to parser
        TokenPtr nextToken();

        // initial parser in order to get token.
        void prepareGetToken();

        // finishing work
        void finish();

        // get preprocessor lexer
        LexerPtr getPreprocessorLexer() { return M_lex; }

    };

	DEFINE_SHARED_PTR(PreprocessorParser);

	typedef PreprocessorParser Preprocessor;
	typedef PreprocessorParserPtr PreprocessorPtr;

}

#endif