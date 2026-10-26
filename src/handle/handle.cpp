#include "handle.hpp"

#include <iostream>

#include "../eval/evalException.hpp"

std::string FileHandle::getContent() {
    if (m_mode != OpenFileMode::Read && m_mode != OpenFileMode::Any) {
        throw EvalException("Tried file reading in not a reading mode",
            "If you want to read from a file, only use files created like that: 'file = fopen(...,\"r\")'");
    }

    return {
        std::istreambuf_iterator<char>(m_file),
        std::istreambuf_iterator<char>()
    };
}

//TODO: Those modes work more like a suggestion than actual setting XD
FileHandle::FileHandle(const std::filesystem::path& path, std::string_view mode) {

    if (mode == "r" || mode == "read") {
        m_file.open(path,std::ios::in);
        std::cout<<"File opened for reading!\n";
        m_mode = OpenFileMode::Read;
    }

    else if (mode == "w" || mode == "write") {
        m_file.open(path,std::ios::out);
        std::cout<<"File opened for writing!\n";
        m_mode = OpenFileMode::Write;
    }

    else if (mode == "app" || mode == "append") {
        m_file.open(path,std::ios::app);
        std::cout<<"File opened for appending!\n";
        m_mode = OpenFileMode::WriteAppend;
    }

    else if (mode == "a" || mode == "any") {
        m_file.open(path);
        std::cout<<"File opened for anything!\n";
        m_mode = OpenFileMode::Any;
    }

    else {
        std::cout<<"File tried to open with an unknown mode!";
    }

    if (!m_file.is_open()) {
        std::cout<<"Failed to open\n";
        return;
    }

    setClose(false);
}

bool FileHandle::hasNextLine() {
    std::string line;
    auto pos = m_file.tellg();
    bool res = static_cast<bool>(std::getline(m_file, line));
    m_file.clear();
    m_file.seekg(pos);
    return res;
}

char FileHandle::getChar() {
    m_file.seekg(m_curReadChar);
    int c = m_file.get();
    m_curReadChar = m_file.tellg();

    if (c == std::char_traits<char>::eof()) {
        throw EvalException("Read end of file","Check for ending of a file with fhaschar(file)");
        return 0;
    }
    else {
        return static_cast<char>(c);
    }
}

bool FileHandle::hasNextChar() {
    auto pos = m_file.tellg();
    bool res = m_file.peek() != std::char_traits<char>::eof();
    m_file.clear();
    m_file.seekg(pos);
    return res;
}

void FileHandle::resetCursor() {
    m_curReadChar = 0;
}

void FileHandle::writeContent(std::string_view s) {
    m_file<<s;
    m_file.flush();
}

std::string FileHandle::getLine() {
    std::string line;
    m_file.seekg(m_curReadChar);
    std::getline(m_file, line);
    m_curReadChar = m_file.tellg();
    return line;
}

void FileHandle::close() {
    m_file.close();
    setClose(true);
}

HandleType FileHandle::getHandleType() const noexcept {
    return HandleType::File;
}
