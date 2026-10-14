#include "handle.hpp"

#include <iostream>

FileHandle::FileHandle(const std::filesystem::path& path, std::string_view mode) {

    m_file.open(path);
    if (!m_file.is_open()) {
        std::cout<<"Failed to open\n";
        return;
    }
    std::cout<<"File opened!\n";
    setClose(false);
}

void FileHandle::close() {
    m_file.close();
    setClose(true);
}
