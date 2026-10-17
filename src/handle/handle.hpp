#pragma once

#include <fstream>
#include <filesystem>

enum class HandleType {
    File,
    Null
};


class Handle {

    HandleType m_type = HandleType::Null;
    bool m_isClosed = true;

protected:
    void setClose(const bool close) noexcept {m_isClosed = close;}

public:
    virtual ~Handle() = default;
    virtual void close() = 0;
    [[nodiscard]] HandleType getHandleType() const noexcept {return m_type;}
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
public:
    [[nodiscard]]std::string getContent();
    FileHandle(const std::filesystem::path& path, std::string_view mode);
    void close() override;
};