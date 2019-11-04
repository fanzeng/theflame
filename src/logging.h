#ifndef THEFLAME_LOGGING_H
#define THEFLAME_LOGGING_H
#include <iostream>
#include <sstream>

#define END_COLORED_TEXT printf("\033[0m");

#define BEGIN_BLACK_FG printf("\x1B[30m");
#define BEGIN_RED_FG printf("\x1B[31m");
#define BEGIN_GREEN_FG printf("\x1B[32m");
#define BEGIN_YELLOW_FG printf("\x1B[33m");
#define BEGIN_BLUE_FG printf("\x1B[34m");
#define BEGIN_MAGENTA_FG printf("\x1B[35m");
#define BEGIN_CYAN_FG printf("\x1B[36m");
#define BEGIN_WHITE_FG printf("\x1B[37m");

#define BEGIN_BRIGHTBLACK_FG printf("\x1B[90m");
#define BEGIN_BRIGHTRED_FG printf("\x1B[91m");
#define BEGIN_BRIGHTGREEN_FG printf("\x1B[92m");
#define BEGIN_BRIGHTYELLOW_FG printf("\x1B[93m");
#define BEGIN_BRIGHTBLUE_FG printf("\x1B[94m");
#define BEGIN_BRIGHTMAGENTA_FG printf("\x1B[95m");
#define BEGIN_BRIGHTCYAN_FG printf("\x1B[96m");
#define BEGIN_BRIGHTWHITE_FG printf("\x1B[97m");

#define COLOR_FG(color, statement) BEGIN_##color##_FG statement
#define ENDC  END_COLORED_TEXT
#define V(v, statement) if (m_verbosity >= v) { statement }
#define COUT_FATAL(statement) V(0, COLOR_FG(BRIGHTRED, std::cout << "[FATAL] " << ) statement; ENDC)
#define COUT_ERROR(statement) V(1, COLOR_FG(RED, std::cout << "[ERROR] " << ) statement; ENDC)
#define COUT_WARN(statement) V(2, COLOR_FG(BRIGHTYELLOW, std::cout << "[WARN ] " << ) statement; ENDC)
#define COUT_PROG(statement) V(3, COLOR_FG(BRIGHTGREEN, std::cout << "[PROG ] " << ) statement; ENDC)
#define COUT_INFO(statement) V(4, COLOR_FG(BRIGHTWHITE, std::cout << "[INFO ] " << ) statement; ENDC)
#define COUT_DEBUG(statement) V(5, COLOR_FG(WHITE, std::cout << "[DEBUG] " << ) statement; ENDC)

class exception_with_code_trace : public std::exception {
private:
    int line;
    std::string file;
    std::string w;
public:
    exception_with_code_trace(std::string file_name, int line_number) : exception_with_code_trace(file_name, line_number, "") {}
    exception_with_code_trace(std::string file_name, int line_number, std::string what) : file(file_name), line(line_number), w(what) {}
    const char* what() const noexcept {
        std::stringstream ss;
        ss <<"\"" << w << "\"" << " in " << file << " line " << std::to_string(line);
        static std::string what_trace_string = ss.str();
        return what_trace_string.c_str();
    }
};
#define throwcode(w) throw exception_with_code_trace(__FILE__, __LINE__, w)

#endif /* THEFLAME_LOGGING_H */