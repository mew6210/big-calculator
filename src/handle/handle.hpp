#pragma once

#include <fstream>
#include <filesystem>

enum class HandleType {
    File,
    Null
};


class Handle {

    bool m_isClosed = true;

protected:
    void setClose(const bool close) noexcept {m_isClosed = close;}

public:
    virtual ~Handle() = default;
    virtual void close() = 0;
    [[nodiscard]] virtual HandleType getHandleType() const noexcept = 0;
    [[nodiscard]] bool isOpen() const noexcept {return !m_isClosed;}
};

enum class OpenFileMode {
    Read,
    Write,
    Any
};

class FileHandle : public Handle {

    std::fstream m_file;
    OpenFileMode m_mode;
    int64_t m_curReadChar = 0;
public:
    [[nodiscard]]std::string getContent();
    FileHandle(const std::filesystem::path& path, std::string_view mode);
    std::string getLine();
    bool hasNextLine();
    char getChar();
    bool hasNextChar();
    void close() override;
    [[nodiscard]] HandleType getHandleType() const noexcept override;
};