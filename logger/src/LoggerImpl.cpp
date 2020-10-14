#include "../include/ILogger.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>

namespace {
    class LoggerImpl : public ILogger {
    public:
        explicit LoggerImpl(void* pClient);
        void destroyLogger(void* pClient) override;
        void log(char const* pMsg, RESULT_CODE err) override;
        RESULT_CODE setLogFile(char const* pLogFile) override;
        ~LoggerImpl() override;
    private:
        static std::vector<void*> _clients;
        static std::ofstream _log;
    };

    std::vector<void*> LoggerImpl::_clients = std::vector<void*>();
    std::ofstream LoggerImpl::_log = std::ofstream();

    LoggerImpl::LoggerImpl(void* pClient) {
        if(std::find(_clients.begin(), _clients.end(), pClient) == _clients.end()) {
            _clients.push_back(pClient);
        }
    }
    void LoggerImpl::destroyLogger(void* pClient) {
        _clients.erase(std::remove(_clients.begin(), _clients.end(), pClient), _clients.end());
    }
    void LoggerImpl::log(char const* pMsg, RESULT_CODE err) {
        if(pMsg != nullptr) {
            if (_log.is_open()) {
                _log << (int)err << ": " << pMsg << std::endl;
            } else {
                std::cout << (int)err << ": " << pMsg << std::endl;
            }
        }
        else {
            _log << (int)RESULT_CODE::BAD_REFERENCE << ": Null error-message reference" << std::endl;
        }
    }
    RESULT_CODE LoggerImpl::setLogFile(char const* pLogFile) {
        if (_log.is_open()) {
            _log.close();
        }
        try {
            _log.open(pLogFile);
        } catch(std::exception &err) {
            return RESULT_CODE::FILE_ERROR;
        }
        return RESULT_CODE::SUCCESS;
    }
    LoggerImpl::~LoggerImpl() {
        if (_log.is_open()) {
            _log.close();
        }
    }
}

ILogger* ILogger::createLogger(void* pClient) {
    if (pClient == nullptr) {
        return nullptr;
    }
    ILogger* log = new (std::nothrow) LoggerImpl(pClient);
    return log;
}
ILogger::~ILogger(){}
