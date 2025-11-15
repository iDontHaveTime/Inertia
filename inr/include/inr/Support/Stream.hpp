#ifndef INERTIA_STREAM_HPP
#define INERTIA_STREAM_HPP

/**
 * @file inr/Support/Stream.hpp
 * @brief Inertia's implementation of C++ stream classes.
 *
 * This header contains Inertia's custom implementation of C++ streams.
 * Its aimed at being lightweight and performant while still keeping convenience of streams.
 *
 **/

#include "inr/Defines/FileDef.hpp"
#include "inr/Support/Cexpr.hpp"
#include "inr/Support/File.hpp"

#include <cstdio>
#include <cstring>

#include <concepts>
#include <charconv>
#include <limits>
#include <string>
#include <type_traits>

namespace inr{

    /* Forward declare so that the 'inr_stream_manipulator' can use it. */
    class inr_ostream;
    /* Forward declare. */
    class inr_istream;
    /* Forward declare. */
    class byte;

    /**
     * @brief The stream calls it if put via operator<<.
     * 
     * This is a basic stream manipulator type, if you put it in the stream it calls it.
     * The manipulator can write to stream, modify stream, and do whatever.
     */
    using inr_stream_manipulator = inr_ostream& (*const)(inr_ostream&);

    /**
     * @brief Base class for 'inr::inr_istream' and 'inr::inr_ostream'.
     *
     */
    template<fs::OpeningType dfopt>
    class inr_iostream{
    protected:
        inrfile file;
    public:
        inr_iostream() = delete;

        /**
         * @brief The main constructor of 'inr_iostream' that sets the underlying file.
         *  
         * This constructor sets the underlying FILE handle to the provided handle.
         * Make sure the file is opened to write!
         *
         * @param _file The FILE handle to output to.
         */
        inr_iostream(FILE* _file, bool own = false) noexcept : file(_file, dfopt, own){};

        /**
         * @brief Opens an already existing 'inrfile' class.
         *
         * @param _inrfile The 'inrfile' to open.
         */
        inr_iostream(inrfile&& _inrfile) noexcept : file(std::move(_inrfile)){};

        inr_iostream(const inr_iostream&) noexcept = delete;
        inr_iostream& operator=(const inr_iostream&) noexcept = delete;

        inr_iostream(inr_iostream&&) noexcept = default;
        inr_iostream& operator=(inr_iostream&&) noexcept = default;

        /**
         * @brief Checks if the underlying file is valid.
         * @return True if file is valid, false if it is not.
         */
        bool valid() const noexcept{
            return file.valid();
        }

        /**
         * @brief Returns if the stream is valid or not.
         * @return True if valid, false if not.
         */
        operator bool() const noexcept{
            return valid();
        }

        /**
         * @brief Returns the underlying file (const version).
         * @return The underlying const file handle.
         */
        const inrfile& get_file() const noexcept{
            return file;
        }
        /**
         * @brief Returns the underlying file.
         * @return The underlying file handle.
         */
        inrfile& get_file() noexcept{
            return file;
        }

        /**
         * @brief Flushes the underlying FILE stream.
         * @return EOF if error, non-EOF if no errors.
         */
        int flush() noexcept{
            return file.flush();
        }

        /**
         * @brief Tells the position in the file.
         *
         * @return Position.
         */
        auto tell() noexcept{
            return file.tell();
        }

        /**
         * @brief Seeks in the file.
         *
         * @param i The offset from the seek pointer.
         * @param st The seek type.
         *
         * @return EOF if error.
         */
        auto seek(tell_integer i, fs::SeekType st = fs::SeekType::SeekSet) noexcept{
            return file.seek(i, st);
        }

        /**
         * @brief Makes sure you can read after writing, or vice versa.
         *
         *  @return Whatever seek() returns.
         */
        auto rw_flush() noexcept{
            return seek(tell());
        }

        /**
         * @brief Closes the file if owned.
         */
        ~inr_iostream() noexcept = default;
    };

    /**
     * @brief Output stream to file. 
     * This is a much more lightweight alternative to STL's ostream class.
     * This uses write functions and then wraps them in a universal operator.
     */
    class inr_ostream : public inr_iostream<fs::OpeningType::Write>{
    public:
        using inr_iostream::inr_iostream;

        /**
         * @brief Opens a file using fopen and assumes ownership
         *
         * This constructor opens a file using fopen with the provided file name and mode.
         * It assumes ownership of the file, so it closes it for you.
         *
         * @param name The name of the file to open.
         * @param mode The mode to open the file in.
         */
        inr_ostream(const char* name, fs::OpeningType mode = fs::OpeningType::Write) noexcept : inr_iostream(fopen(name, fs::opening_type_write(mode) ? inr_open_type_to_cfopen(mode) : nullptr), true){};

        /**
         * @brief Writes the provided data to the stream.
         *
         * This writes to the stream inside the class.
         * It writes the size*n amount of bytes read from the data to the stream.
         *
         * @param data This parameter is the data to write to stream.
         * @param size Size of the data to write to the stream.
         * @param n The amount of times to write the size to the stream.
         *
         * @return The amount of objects written with the provided size.
         */
        auto write(const void* data, size_t size, size_t n) noexcept{
            return file.write(data, size, n);
        }

        /**
         * @brief Write function to fit std::ostream's write signature.
         *
         * @param data The data to write.
         * @param n Amount of bytes to write from data.
         *
         * @return *this
         */
        inr_ostream& write(const char* data, size_t n) noexcept{
            write(data, 1, n);
            return *this;
        }

        /**
         * @brief Allows 'inr::byte' to be used for data param.
         *
         * @param data The byte array.
         * @param n Amount of bytes to write from data.
         *
         * @return *this
         */
        inr_ostream& write(const byte* data, size_t n) noexcept{
            return write((const char*)data, n);
        }

        /**
         * @brief Writes the provided character to the stream.
         */
        inr_ostream& operator<<(char c) noexcept{
            write(&c, 1, 1);
            return *this;
        }

        /**
         * @brief Writes the provided string to the stream.
         */
        inr_ostream& operator<<(const char* _str) noexcept{
            write(_str, 1, strlen(_str));
            return *this;
        }

        /**
         * @brief Writes the provided STL string to the stream.
         */
        inr_ostream& operator<<(const std::string& _str) noexcept{
            write(_str.data(), 1, _str.size());
            return *this;
        }

        /**
         * @brief Writes the provided STL string view to the stream.
         */
        inr_ostream& operator<<(const std::string_view& _str) noexcept{
            write(_str.data(), 1, _str.size());
            return *this;
        }

        /**
         * @brief Calls the provided stream manipulator function.
         *
         * The stream manipulator functions are functions that write to the stream when called.
         * When this operator is called, function provided is called (for example inr::endl).
         */
        inr_ostream& operator<<(inr_stream_manipulator func){
            return func(*this);
        }

        /**
         * @brief Writes true or false to the stream.
         */
        inr_ostream& write_bool(bool b){
            if(b){
                write("true", 1, cexpr_strlen("true"));
            }
            else{
                write("false", 1, cexpr_strlen("false"));
            }
            return *this;
        }

        /**
         * @brief Templated function for integer values.
         */
        template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
        inr_ostream& write_integer(T n){
            char buff[std::numeric_limits<T>::digits10 + 2];
            
            std::to_chars_result result = std::to_chars(buff, buff + sizeof(buff), n);

            if(result.ec == std::errc()){
                write(buff, 1, result.ptr - buff);
            }
            else{
                write("err", 1, cexpr_strlen("err"));
            }

            return *this;
        }

        /**
         * @brief Templated stream for integer values.
         */
        template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
        inr_ostream& operator<<(T n){
            return write_integer<T>(n);
        }

        /**
         * @brief Puts a char into the stream.
         *
         * @param c The char to put.
         *
         * @return *this.
         */
        inr_ostream& put(char c) noexcept{
            return write(&c, sizeof(c));
        }

        /**
         * @brief Puts a wchar into the stream.
         *
         * @param c The wchar to put.
         *
         * @return *this.
         */
        inr_ostream& put(wchar_t c) noexcept{
            if constexpr(sizeof(wchar_t) == sizeof(char32_t)){
                return put((char32_t)c);
            }
            else{
                return *this;
            }
        }

        /**
         * @brief Puts a char8 into the stream.
         *
         * @param c The char8 to put.
         *
         * @return *this.
         */
        inr_ostream& put(char8_t c) noexcept{
            return write((const char*)&c, sizeof(c));
        }

        /**
         * @brief Puts a char32 into the stream.
         *
         * @param c The char32 to put.
         *
         * @return *this.
         */
        inr_ostream& put(char32_t c) noexcept{
            char buf[4];

            return write(buf, char32_to_utf8(buf, c));
        }
    };

    /**
     * @brief Standard output stream for general messages.
     *
     * This stream writes to the global stdout FILE* provided by stdio.
     * This is like std::cout but provided by Inertia's API.
     */
    extern inr_ostream out;
    /**
     * @brief Standard error output stream for error reporting.
     *
     * This stream writes to the global stderr FILE* provided by stdio.
     * This is like std::cerr but provided by Inertia's API.
     */
    extern inr_ostream err;
    
    /**
     * @brief Ends the line and then flushes the stream provided.
     */
    extern inr_stream_manipulator endl;
    /**
     * @brief Flushes the stream provided. 
     */
    extern inr_stream_manipulator flush;

    class inr_istream : public inr_iostream<fs::OpeningType::Read>{
    public:
        using inr_iostream::inr_iostream;

        /**
         * @brief Reads from the stream to the provided buffer.
         *
         * This reads the stream to the provided data pointer.
         * It reads the size*n amount of bytes from the stream.
         *
         * @param data This parameter is the destination to write to from the stream.
         * @param size Size of the data to read from the stream.
         * @param n The amount of times to read the size from the stream.
         *
         * @return The amount of objects read with the provided size.
         */
        auto read(void* data, size_t size, size_t n) noexcept{
            return file.read(data, size, n);
        }

        /**
         * @brief Read function to fit std::istream's read signature.
         *
         * @param data The destination to read to.
         * @param n Amount of bytes to read from the stream.
         *
         * @return *this
         */
        inr_istream& read(char* data, size_t n) noexcept{
            read(data, 1, n);
            return *this;
        }

        /**
         * @brief Read to data with 'inr::byte' type.
         *
         * @param data The destination to read to.
         * @param n Amount of bytes to read from the stream.
         *
         * @return *this
         */
        inr_istream& read(byte* data, size_t n) noexcept{
            return read((char*)data, n);
        }

        /**
         * @brief Reads N amount of chars.
         *
         * @param b Buffer to read the chars to.
         * @param n Amount of chars to read.
         * @return *this
         */
        inr_istream& get(char* b, size_t n = 1) noexcept{
            return read(b, n);
        }

        /**
         * @brief Reads characters until a newline, null terminates. Set limit to 0 for no limit.
         *
         *
         * @param b Buffer to read the chars to.
         * @param limit Max amount of characters to read.
         *
         * @return *this
         */
        inr_istream& getline(char* b, size_t limit) noexcept{
            bool limit_c = limit > 0;
            
            while(true){
                if(limit_c){
                    if(!limit){
                        *--b = '\0';
                        break;
                    }
                    limit--;
                }

                char c;
                get(&c);

                if(c == '\n'){
                    break;
                }
                *b++ = c;
            }

            *b = '\0';

            return *this;
        }

        /**
         * @brief Reads characters until a newline, std::string version.
         *
         *
         * @param str String to read the chars to.
         *
         * @return *this
         */
        inr_istream& getline(std::string& str){

            while(true){
                char c;
                get(&c);

                if(c == '\n'){
                    break;
                }
                str += c;
            }

            return *this;
        }
    };

    /**
     * @brief Standard input stream for getting console input.
     *
     * This stream reads from the global stdin FILE* provided by stdio.
     * This is like std::cin but provided by Inertia's API.
     */
    extern inr_istream in;
    
    template<class T>
    concept ostream_t = requires(T t, const char* p, size_t n){
        {t.write(p, n)} -> std::same_as<T&>;
    };

    template<class T>
    concept istream_t = requires(T t, char* p, size_t n){
        {t.read(p, n)} -> std::same_as<T&>;
    };

    template<class T>
    concept stream_t = ostream_t<T> || istream_t<T>;
}

#endif // INERTIA_STREAM_HPP
