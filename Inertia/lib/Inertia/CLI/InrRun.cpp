#include "Inertia/CLI/InrRun.hpp"
#ifdef _WIN32
#include <windows.h>
#include <string>
#include <vector>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

namespace Inertia{

int InrRun::execute_command(int argc, const char* const argv[], const char* const envp[]) noexcept{
    if(argv[argc] != nullptr) return -1;
#ifdef _WIN32
    // NOT TESTED
    size_t totalLen = 0;
    for(int i = 0; i < argc; i++){
        totalLen += 2;
        for(const char* p = argv[i]; *p; p++){
            totalLen += (*p == '"') ? 2 : 1;
        }
        if(i > 0) totalLen += 1;
    }

    std::string commandLine;
    commandLine.reserve(totalLen);

    for(int i = 0; i < argc; i++){
        if(i > 0) commandLine += ' ';
        commandLine += '"';
        for(const char* p = argv[i]; *p; p++){
            if(*p == '"') commandLine += "\\\"";
            else commandLine += *p;
        }
        commandLine += '"';
    }

    LPVOID envBlock = nullptr;
    if(envp){
        size_t envLen = 0;
        for(char** e = envp; *e != nullptr; e++){
            envLen += strlen(*e) + 1;
        }
        envLen += 1;

        std::string envStr;
        envStr.reserve(envLen);
        for(char** e = envp; *e != nullptr; e++){
            envStr += *e;
            envStr += '\0';
        }
        envStr += '\0';
        envBlock = (LPVOID)envStr.c_str();
    }

    STARTUPINFO si{};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi{};

    BOOL success = CreateProcess(
        nullptr,
        commandLine.data(),
        nullptr,
        nullptr,
        FALSE,
        0,
        envBlock,
        nullptr,
        &si,
        &pi
    );

    if(!success) return -1;

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode = 0;
    GetExitCodeProcess(pi.hProcess, &exitCode);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return (int)(exitCode);
#else
    pid_t pid = fork();
    if(pid == 0){
        execve(argv[0], (char* *const)argv, (char* *const)envp);
        _exit(127);
    }
    else if(pid > 0){
        int status;
        if(waitpid(pid, &status, 0) == -1){
            return -1;
        }
        if(WIFEXITED(status)){
            return WEXITSTATUS(status);
        }
        return -1;
    }
    else{
        return -1;
    }
#endif
    return -1;
}

}
