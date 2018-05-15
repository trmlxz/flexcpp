// Generated by Bisonc++ V2.09.04 on Mon, 16 Jan 2012 21:40:37 +0100

#ifndef Parser_h_included
#define Parser_h_included


// $insert baseclass
#include "parserbase.h"
// $insert scanner.h
#include "../scanner/scanner.h"


#undef Parser
class Parser: public ParserBase
{
    // $insert scannerobject
    Scanner d_scanner;
    bool d_print;
    std::string d_msg;
    size_t d_nErrors = 0;
        
    public:
        Parser(bool doPrint);
        int parse();
        void setScannerDebug(bool doDebug)
        {
            d_scanner.setDebug(doDebug);
        }
        size_t status() const;

    private:
        void block();
        void error(char const *msg);    // called on (syntax) errors
        int lex();                      // returns the next token from the
                                        // lexical scanner. 
        void print();                   // use, e.g., d_token, d_loc

    // support functions for parse():
        void executeAction(int ruleNr);
        void errorRecovery();
        int lookup(bool recovery);
        void nextToken();
};

inline size_t Parser::status() const
{
    return d_nErrors;
}

#include <stdexcept>

inline void Parser::error(char const *msg)
{
    ++d_nErrors;
    throw std::runtime_error(msg);
    std::cerr << msg << '\n';
}

// $insert lex
inline int Parser::lex()
{
    print();
    return d_scanner.lex();
}

// $insert print
inline void Parser::print()
{
    enum { _UNDETERMINED_ = -2 };

    if (d_print && d_token_ != _UNDETERMINED_ )
    {
        std::cout << "Token: " << symbol_(d_token_) <<
                    ", text: `" <<  d_scanner.matched() << "'\n";
    }
}

#endif


