#ifndef INERTIA_INRSTREAM_HPP
#define INERTIA_INRSTREAM_HPP

#include "inr/Defines/inrfiledef.hpp"
#include "inr/Support/inrcexpr.hpp"
#include "inr/Support/inrfile.hpp"

#include <cstdio>
#include <cstring>

#include <charconv>
#include <limits>
#include <string>
#include <type_traits>

/**
 * @file inr/Support/inrstream.hpp
 * @brief Inertia's implementation of C++ stream classes.
 *
 * This header contains Inertia's custom implementation of C++ streams.
 * Its aimed at being lightweight and performant while still keeping convenience of streams.
 *
 **/

namespace inr{

    /* Forward declare so that the 'inr_stream_manipulator' can use it. */
    class inr_ostream;
    /* Forward declare. */
    class inr_istream;

    /**
     * @brief The stream calls it if put via operator<<.
     * 
     * This is a basic stream manipulator type, if you put it in the stream it calls it.
     * The manipulator can write to stream, modify stream, and do whatever.
     */
    using inr_stream_manipulator = inr_ostream& (*const)(inr_ostream&);

    /**
     * @brief Output stream to file. 
     * This is a much more lightweight alternative to STL's ostream class.
     * This uses write functions and then wraps them in a universal operator.
     */
    class inr_ostream{
        inrfile file;
    public:
        inr_ostream() = delete;

        /**
         * @brief The main constructor of 'inr_ostream' that sets the underlying file.
         *  
         * This constructor sets the underlying FILE handle to the provided handle.
         * Make sure the file is opened to write!
         *
         * @param _file The FILE handle to output to.
         */
        inr_ostream(FILE* _file, bool own = false) noexcept : file(_file, fs::OpeningType::Write, own){};

        /**
         * @brief Opens an already existing 'inrfile' class.
         *
         * @param _inrfile The 'inrfile' to open.
         */
        inr_ostream(inrfile&& _inrfile) noexcept : file(std::move(_inrfile)){};

        /**
         * @brief Opens a file using fopen and assumes ownership
         *
         * This constructor opens a file using fopen with the provided file name and mode.
         * It assumes ownership of the file, so it closes it for you.
         *
         * @param name The name of the file to open.
         * @param mode The mode to open the file in.
         */
        inr_ostream(const char* name, fs::OpeningType mode = fs::OpeningType::Write) noexcept : file(fopen(name, fs::opening_type_write(mode) ? inr_open_type_to_cfopen(mode) : nullptr), mode, true){};

        inr_ostream(const inr_ostream&) noexcept = delete;
        inr_ostream& operator=(const inr_ostream&) noexcept = delete;

        inr_ostream(inr_ostream&&) noexcept = default;
        inr_ostream& operator=(inr_ostream&&) noexcept = default;

        /**
         * @brief Closes the file if owned.
         */
        ~inr_ostream() noexcept = default;

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
         * @brief Flushes the underlying FILE stream.
         * @return EOF if error, non-EOF if no errors.
         */
        int flush() noexcept{
            return file.flush();
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
                write("err", 1, 3);
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
         * @brief Tells the position in the file.
         *
         * @return Position.
         */
        auto tell() noexcept{
            return file.tell();
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

    class inr_istream{
        inrfile file;
    public:
        inr_istream() = delete;

        /**
         * @brief The main constructor of 'inr_istream' that sets the underlying file.
         *  
         * This constructor sets the underlying FILE handle to the provided handle.
         * Make sure the file was opened to read!
         *
         * @param _file The FILE handle to output to.
         */
        inr_istream(FILE* _file, bool own = false) noexcept : file(_file, fs::OpeningType::Read, own){};

        /**
         * @brief Opens an already existing 'inrfile' class.
         *
         * @param _inrfile The 'inrfile' to open.
         */
        inr_istream(inrfile&& _inrfile) noexcept : file(std::move(_inrfile)){};

        inr_istream(const inr_istream&) noexcept = delete;
        inr_istream& operator=(const inr_istream&) noexcept = delete;

        inr_istream(inr_istream&&) noexcept = default;
        inr_istream& operator=(inr_istream&&) noexcept = default;

        /**
         * @brief Closes the file if owned.
         */
        ~inr_istream() noexcept = default;

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

        /**
         * @brief Tells the position in the file.
         *
         * @return Position.
         */
        auto tell() noexcept{
            return file.tell();
        }
    };

    /**
     * @brief Standard input stream for getting console input.
     *
     * This stream reads from the global stdin FILE* provided by stdio.
     * This is like std::cin but provided by Inertia's API.
     */
    extern inr_istream in;
}

#endif // INERTIA_INRSTREAM_HPP
