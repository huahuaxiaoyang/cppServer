#include"./log/Log4cpp.hpp"

Logger* Logger::_pInstance = nullptr;

 Logger* Logger::getInstance() 
 {
    if (_pInstance == nullptr) {
            string initFileName = "./log/log4cpp.conf";
            log4cpp::PropertyConfigurator::configure(initFileName);
            log4cpp::Category& _root = log4cpp::Category::getRoot();
            log4cpp::Category& sub1 =
                log4cpp::Category::getInstance(std::string("sub1"));
            _pInstance = new Logger(_root, sub1);
        }
        return _pInstance;
 }

 void Logger::destroy() 
 {
     if (_pInstance) {
            delete _pInstance;
            _pInstance = nullptr;
        }
 }