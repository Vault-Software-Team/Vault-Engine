#include <ExternalApps/Links.hpp>

namespace External {
    void OpenURLInBrowser(const std::string &url) {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        system(std::string("start ").append(url).c_str());
#elif __APPLE__
        system(std::string("open ").append(url).c_str());
#elif __linux__
        system(std::string("xdg-open ").append(url).c_str());
#else
    #error "Unknown compiler"
#endif
    }
} // namespace External