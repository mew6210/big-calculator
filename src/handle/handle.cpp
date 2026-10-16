#include "handle.hpp"

#include <iostream>

FileHandle::FileHandle(const std::filesystem::path& path, std::string_view mode) {

    m_file.open(path);
    if (!m_file.is_open()) {
        std::cout<<"Failed to open\n";
        return;
    }

    if (mode == "r" || mode == "read") {
        std::cout<<"File opened for reading!\n";
        m_mode = OpenFileMode::Read;
    }

    else if (mode == "w" || mode == "write") {
        std::cout<<"File opened for writing!\n";
        m_mode = OpenFileMode::Write;
    }

    else if (mode == "a" || mode == "any") {
        std::cout<<"File opened for anything!\n";
        m_mode = OpenFileMode::Any;
    }

    else {
        std::cout<<"File opened with an unkown mode!";
    }

    setClose(false);
}

void FileHandle::close() {
    m_file.close();
    setClose(true);
}
