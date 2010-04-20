// Generated by Flexc++ v0.7.1~pre on Tue, 20 Apr 2010 16:34:24 +0000


#ifndef SCANNERBASE_H_INCLUDED_
#define SCANNERBASE_H_INCLUDED_

#include <cstddef>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <stack>
#include <stdexcept>
#include <memory>
#include <cstring>
#include <map>
// $insert namespace-open


// StreamInfo contract:
// - StreamInfo needs a default constructor, probably connecting to std::cin
// - StreamInfo needs a member connect, expecting a StreamAdapter
// - StreamInfo needs a member resume, expecting a StreamInfo and a size_t
template <typename StreamInfo>
class ScannerTemplate
{
    public:
        typedef StreamInfo StreamInfoType;

        struct StreamAdapter            // modifiable by StreamInfo
        {
            std::istream *in;
            size_t lineno;
        };

        /* StreamBuffer keeps a std::string d_buffer, containing the text
         * as it is read from the istream.
         *
         * Its most important public member is next(), which can be used
         * by a finite state automaton (FA) to request the next letter.
         *
         * StreamBuffer remembers how many times next() is called, and
         * makes this availabe with the getter nSteps(). (Named after
         * the steps that have been taken in the FA).
         *
         * If next() is called, StreamBuffer attempts to obtain the next
         * letter from the istream. If there is still text on the buffer,
         * the next char is converted to a dfa letter as follows:
         *
         *     char ch;
         *     int dfaLetter = static_cast<unsigned char>(ch);
         *
         * StreamBuffer then increments d_endSeen, an iterator that marks
         * the end of the substring of d_buffer that it has 'seen'. If
         * there is no text left on the istream, next() returns EOF.
         * d_endSeen is not incremented, but d_pointingBeyondEOF is set
         * to true, indicating that on the (simulated) buffer of dfa letters,
         * a (simulated) end marker is pointing beyond EOF. (If the end marker
         * is requested to move even further, an exception is thrown).
         *
         * During or after a run of an FA, restore(size_t n_steps) may be
         * called to put StreamBuffer back in the state it was after n_steps
         * steps. d_endSeen is also restored.
         *
         * At any point, the members seen() and lastSeen() may be used to
         * access the textual representation of what StreamBuffer has seen.
         * If only EOF has been returned, seen() will return the empty string.
         *
         * After a run of an FA, forget() may be called. This causes
         * StreamBuffer to forget anything on std::string d_buffer before
         * d_endSeen, and forget all FA letters it has returned. StreamBuffer
         * is then ready for another FA run.
         *
         */
        class StreamBuffer              // fields controlled by lex()
        {
            std::shared_ptr<StreamInfo> d_streamInfo;
            StreamAdapter d_adapter;

            std::string d_buffer;       // textual buffer
            std::string::iterator d_endSeen; // Iterator to beyond the last
                                             // seen char. Not const because it
                                             // is used in forget() to modify
                                             // d_buffer.
            bool d_atBol; // search for next token started at beginning of line
            bool d_eofSensed;
            bool d_pointingBeyondEOF; // in simulated int buffer for dfa

            public:
                StreamBuffer(size_t lineno = 1);
                StreamBuffer(StreamInfo *streamInfo, size_t lineno = 1);
                StreamBuffer(StreamBuffer const &other);

                StreamBuffer &operator=(StreamBuffer rhs);

                StreamInfo &streamInfo() const;

                int next(); // returns next dfa letter (including EOF).
                void restore(size_t n_steps); // use values from nSteps()
                size_t nSteps() const; // number of times next() was called.
                std::string seen() const; // that what's before d_endSeen
                std::string lastSeen() const; // that what's just before d_endSeen

                void forget();  // that what's before d_endSeen,
                                // also sets atBol()

                // remember is used by less(), it puts a string back in front
                // of the buffer and sets d_endSeen to point at the front.
                void remember(std::string::const_iterator abegin,
                              std::string::const_iterator aend);

                bool atBol() const; // started scanning at beginning of line?
                void setBol(bool atBol); // set d_atBol for next token
                size_t lineno() const;
                void updateLineno(); // with #newlines in seen().

            private:
                std::istream &in(); // returns d_adapter.in
                void fill(); // sets d_eofSensed if EOF was sensed.
        };

    protected:
        StreamBuffer d_curBuf;

    private:
        std::stack<StreamBuffer> d_streams;

    protected:
        ScannerTemplate();
        ScannerTemplate(StreamInfo *streamInfo);

    public:
        StreamBuffer const switchStream(StreamInfo *streamInfo);
        void switchStream(StreamBuffer const &buffer);
        void pushStreamInfo(StreamInfo *streamInfo);
        void popStreamInfo();
        size_t streamStackSize() const;

        StreamInfo &streamInfo() const;
        size_t lineno() const;
};

// StreamBuffer implementations

template <typename StreamInfo>
ScannerTemplate<StreamInfo>::StreamBuffer::StreamBuffer(size_t lineno)
:
    d_streamInfo(new StreamInfo()),
    d_endSeen(d_buffer.begin()),
    d_atBol(true),
    d_eofSensed(false),
    d_pointingBeyondEOF(false)
{
    d_adapter.lineno = lineno;
    d_streamInfo->connect(&d_adapter);
}

template <typename StreamInfo>
ScannerTemplate<StreamInfo>::StreamBuffer::StreamBuffer(
        StreamInfo *streamInfo, size_t lineno)
:
    d_streamInfo(streamInfo),
    d_endSeen(d_buffer.begin()),
    d_atBol(true),
    d_eofSensed(false),
    d_pointingBeyondEOF(false)
{
    d_adapter.lineno = lineno;
    d_streamInfo->connect(&d_adapter);
}

template <typename StreamInfo>
ScannerTemplate<StreamInfo>::StreamBuffer::StreamBuffer(
        typename ScannerTemplate<StreamInfo>::StreamBuffer::StreamBuffer
        const &other)
:
    d_streamInfo(other.d_streamInfo),
    d_adapter(other.d_adapter),
    d_buffer(other.d_buffer),
    d_atBol(other.d_atBol),
    d_eofSensed(other.d_eofSensed)
{
    restore(other.nSteps());
}

template <typename StreamInfo>
typename ScannerTemplate<StreamInfo>::StreamBuffer &
        ScannerTemplate<StreamInfo>::StreamBuffer::operator=(
                ScannerTemplate<StreamInfo>::StreamBuffer rhs)
{
    enum
    {
        size = sizeof(ScannerTemplate<StreamInfo>::StreamBuffer),
        infosize = sizeof(std::shared_ptr<StreamInfo>),
        itersize = sizeof(std::string::iterator)
    };

    size_t idx =        d_endSeen -     d_buffer.begin();
    size_t rhsidx = rhs.d_endSeen - rhs.d_buffer.begin();

    char buffer[size];
    memcpy(buffer,  this,   size);
    memcpy(this,    &rhs,   size);
    memcpy(&rhs,    buffer, size);


    memcpy(buffer,  &d_streamInfo,   infosize);
    memcpy(this,    &rhs.d_streamInfo,   infosize);
    memcpy(&rhs.d_streamInfo,    buffer, infosize);

    d_streamInfo.swap(rhs.d_streamInfo);
    d_endSeen = d_buffer.begin() + rhsidx;
    rhs.d_endSeen = rhs.d_buffer.begin() + idx;

    return *this;
}

template <typename StreamInfo>
inline StreamInfo &
        ScannerTemplate<StreamInfo>::StreamBuffer::streamInfo() const
{
    return *d_streamInfo;
}

template <typename StreamInfo>
inline bool ScannerTemplate<StreamInfo>::StreamBuffer::atBol() const
{
    return d_atBol;
}

template <typename StreamInfo>
inline void ScannerTemplate<StreamInfo>::StreamBuffer::setBol(bool atBol)
{
    d_atBol = atBol;
}

template <typename StreamInfo>
inline void ScannerTemplate<StreamInfo>::StreamBuffer::updateLineno()
{
    d_adapter.lineno += count(d_buffer.begin(), d_endSeen, '\n');
}

template <typename StreamInfo>
void ScannerTemplate<StreamInfo>::StreamBuffer::fill()
{
    // remember position of d_endSeen
    int fwdOffset = d_endSeen - d_buffer.begin();

    // fill the buffer
    std::string nextLine;
    if (std::getline(in(), nextLine).eof())
        d_eofSensed = true;
    else
        nextLine += "\n";
    d_buffer += nextLine;
// $insert debug "Tried to refill buffer. Istream states: fail = " << in().fail() << ", eof = " << in().eof() << ".\n"
// $insert debug "Buffer contents: `" << d_buffer << (in().eof() ? "'\n" : "' (+\\n)\n")


    // restore d_endSeen
    d_endSeen = d_buffer.begin() + fwdOffset;
}

template <typename StreamInfo>
int ScannerTemplate<StreamInfo>::StreamBuffer::next()
{
    if (d_pointingBeyondEOF)
        throw std::runtime_error("StreamBuffer::next(): pointing beyond EOF");

    // fill if necessary
    if (d_endSeen == d_buffer.end() && not d_eofSensed)
        fill();

    // return appropriate letter
    if (d_endSeen == d_buffer.end())
    {// $insert debug "next dfa letter: EOF" << '\n'

        return EOF;
    }

    // do the conversion to dfa letter and return; increment d_endSeen.// $insert debug "next dfa letter: " << *d_endSeen << '\n'

    return static_cast<unsigned char>(*d_endSeen++);
}

template <typename StreamInfo>
void ScannerTemplate<StreamInfo>::StreamBuffer::restore(size_t n_steps)
{
    if (n_steps > d_buffer.length() + 1)
        throw std::runtime_error(
                "StreamBuffer::restore(): n_steps out of range");

    d_pointingBeyondEOF = n_steps == d_buffer.length() + 1 ? true : false;

    d_endSeen = d_buffer.begin() + n_steps;
}

template <typename StreamInfo>
inline size_t ScannerTemplate<StreamInfo>::StreamBuffer::nSteps() const
{
    // right now each call to next just increases d_endSeen with one step,
    // except when EOF was returned, in which case d_pointingBeyondEOF
    // was set to true.
    return d_endSeen - d_buffer.begin() + (d_pointingBeyondEOF ? 1 : 0);
}

template <typename StreamInfo>
inline std::string ScannerTemplate<StreamInfo>::StreamBuffer::seen() const
{
    return std::string(d_buffer.begin(),
                       std::string::const_iterator(d_endSeen));
}

template <typename StreamInfo>
std::string ScannerTemplate<StreamInfo>::StreamBuffer::lastSeen() const
{
    // if EOF was last seen, just return the empty string.
    if (d_endSeen == d_buffer.begin() || d_pointingBeyondEOF)
        return "";
    return std::string(d_endSeen - 1, d_endSeen);
}

template <typename StreamInfo>
void ScannerTemplate<StreamInfo>::StreamBuffer::forget()
{
    d_atBol = lastSeen() == "\n";
    d_endSeen = d_buffer.erase(d_buffer.begin(), d_endSeen);
}

template <typename StreamInfo>
void ScannerTemplate<StreamInfo>::StreamBuffer::remember(
        std::string::const_iterator abegin,
        std::string::const_iterator aend)
{
    d_buffer.insert(d_buffer.begin(), abegin, aend);
    d_endSeen = d_buffer.begin(); // can't contract this with statement above!?
    d_pointingBeyondEOF = false;
}

template <typename StreamInfo>
inline size_t ScannerTemplate<StreamInfo>::StreamBuffer::lineno() const
{
    return d_adapter.lineno;
}

template <typename StreamInfo>
inline std::istream &ScannerTemplate<StreamInfo>::StreamBuffer::in()
{
    return *d_adapter.in;
}

// ScannerTemplate implementations

template <typename StreamInfo>
inline ScannerTemplate<StreamInfo>::ScannerTemplate()
{}

template <typename StreamInfo>
inline ScannerTemplate<StreamInfo>::ScannerTemplate(
        StreamInfo *streamInfo)
:
    d_curBuf(streamInfo)
{}

template <typename StreamInfo>
typename ScannerTemplate<StreamInfo>::StreamBuffer const
        ScannerTemplate<StreamInfo>::switchStream(
                StreamInfo *streamInfo)
{
    StreamBuffer old = d_curBuf;
    d_curBuf = StreamBuffer(streamInfo, lineno());
    return old;
}

template <typename StreamInfo>
void ScannerTemplate<StreamInfo>::switchStream(
        ScannerTemplate<StreamInfo>::StreamBuffer const &buffer)
{
    StreamBuffer oldBuf = d_curBuf; // store for resume below
    size_t oldLineno = lineno();

    d_curBuf = buffer;
    d_curBuf.streamInfo().resume(oldBuf.streamInfo(), oldLineno);
}

template <typename StreamInfo>
void ScannerTemplate<StreamInfo>::pushStreamInfo(
        StreamInfo *streamInfo)
{// $insert debug "pushStreamInfo(): pushing stream" << '\n';

    d_streams.push(d_curBuf);
    d_curBuf = StreamBuffer(streamInfo, lineno());
}

template <typename StreamInfo>
void ScannerTemplate<StreamInfo>::popStreamInfo()
{// $insert debug "popStreamInfo(): popping Stream" << '\n';

    StreamBuffer oldBuf = d_curBuf; // store for resume below
    size_t oldLineno = lineno();

    d_curBuf = d_streams.top();
    d_curBuf.streamInfo().resume(oldBuf.streamInfo(), oldLineno);
            // Resume needed after assignment, since assignment overwrites the
            // adapter-values.
    d_streams.pop();
}

template <typename StreamInfo>
inline size_t ScannerTemplate<StreamInfo>::streamStackSize() const
{
    return d_streams.size();
}

template <typename StreamInfo>
inline StreamInfo &ScannerTemplate<StreamInfo>::streamInfo() const
{
    return d_curBuf.streamInfo();
}

template <typename StreamInfo>
inline size_t ScannerTemplate<StreamInfo>::lineno() const
{
    return d_curBuf.lineno();
}

// StreamInfo implementations
namespace StreamSwitching
{

    // `regular' StreamInfo: performs most common scenario: opening files from
    // filenames, setting lineno to 1 upon switching streams
class StreamInfo
{
    protected:
        std::istream *d_in;
        std::string d_name;

    public:
        StreamInfo(); // required for default construction of Scanner
        StreamInfo(char const *name);
        StreamInfo(std::istream *in); // StreamInfo will own in!
        ~StreamInfo();

            // Required members
        void connect(ScannerTemplate<StreamInfo>::StreamAdapter *ptr);
        void resume(StreamInfo const &streamInfo, size_t lineno);

        std::string const &name() const;
};


    // Default resume does nothing.
inline void StreamInfo::resume(StreamInfo const &streamInfo, size_t lineno)
{}

inline std::string const &StreamInfo::name() const
{
    return d_name;
}


    // LineStreamInfo implements a second scenario: linenumbering continues
    // after switching streams. For the rest it's just a StreamInfo.
class LineStreamInfo: public StreamInfo
{
    ScannerTemplate<LineStreamInfo>::StreamAdapter *d_ptr;

    public:
        LineStreamInfo() = default;
        LineStreamInfo(char const *name);
        LineStreamInfo(std::istream *in);

        void connect(ScannerTemplate<LineStreamInfo>::StreamAdapter *ptr);
        void resume(LineStreamInfo const &streamInfo, size_t lineno);
};

inline LineStreamInfo::LineStreamInfo(char const *name)
:
    StreamInfo(name)
{}

inline LineStreamInfo::LineStreamInfo(std::istream *in)
:
    StreamInfo(in)
{}

inline void LineStreamInfo::resume(
        LineStreamInfo const &streamInfo, size_t lineno)
{
    d_ptr->lineno = lineno;
}

} // end namespace StreamSwitching


// ScannerBase// $insert streaminfoinclude
#include "../fstreaminfo/fstreaminfo.h"

class ScannerBase: public ScannerTemplate<FStreamInfo>
{
    public:
        enum StartCondition {
            INITIAL// $insert inclusiveStartConditionNames
// $insert exclusiveStartConditionNames
            ,
            charclass,
            comment,
            mblock,
            option,
            pattern,
            patterntxt,
            quote,
            str,
vbar
        };

    private:
        StartCondition d_currentStartCondition;
        std::ostream *d_out;

    protected:
        std::string d_match;
        bool d_append_match;

        bool d_nonReturningAction; // true if an action does not return a value

        ScannerBase();
        ScannerBase(StreamInfoType *streamInfo);

        void begin(StartCondition startCondition);
        StartCondition startCondition() const;
        void ECHO() const;
        void less(size_t n = 0);
        void more();

    public:
        void setOutputStream(std::ostream &out);
        std::ostream &outputStream() const;

        std::string const &match() const;
        std::string const &text() const; // alias for match
        size_t leng() const;
        bool atBol() const;
        void setBol(bool atBol);
};

inline ScannerBase::StartCondition ScannerBase::startCondition() const
{
    return d_currentStartCondition;
}

inline void ScannerBase::begin(StartCondition startCondition)
{
    d_currentStartCondition = startCondition;
}

inline void ScannerBase::setOutputStream(std::ostream &out)
{
    d_out = &out;
}

inline std::ostream &ScannerBase::outputStream() const
{
    return *d_out;
}

inline std::string const &ScannerBase::match() const
{
    return d_match;
}

inline std::string const &ScannerBase::text() const
{
    return d_match;
}

inline void ScannerBase::ECHO() const
{
    *d_out << d_match;
}

inline void ScannerBase::more()
{
    d_append_match = true;
}

inline size_t ScannerBase::leng() const
{
    return d_match.length();
}

inline bool ScannerBase::atBol() const
{
    return d_curBuf.atBol();
}

inline void ScannerBase::setBol(bool atBol)
{
    d_curBuf.setBol(atBol);
}
// $insert namespace-close


#endif // SCANNERBASE_H_INCLUDED_
