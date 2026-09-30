#include "config_io.h"

#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <utility>

namespace smart_home::tui {

std::expected<nlohmann::json, std::string> loadConfig(const std::string &path) {
    const auto closeFile = [](std::FILE *file) { std::fclose(file); };
    std::unique_ptr<std::FILE, decltype(closeFile)> stream(std::fopen(path.c_str(), "rb"),
                                                           closeFile);
    const int openError = stream ? 0 : errno;
    if (!stream) {
        if (openError == ENOENT) {
            return std::unexpected("Missing config: " + path);
        }
        return std::unexpected("Cannot open config (" + path + "): " + std::strerror(openError));
    }
    std::string contents;
    char buffer[4096];
    while (const auto count = std::fread(buffer, 1, sizeof(buffer), stream.get())) {
        contents.append(buffer, count);
    }
    if (std::ferror(stream.get())) {
        const int readError = errno;
        return std::unexpected("Cannot read config (" + path + "): " + std::strerror(readError));
    }
    try {
        return nlohmann::json::parse(contents);
    } catch (const nlohmann::json::exception &error) {
        return std::unexpected("Cannot parse config (" + path + "): " + error.what());
    }
}

std::expected<void, std::string> saveConfig(const std::string &path, const nlohmann::json &config) {
    std::string serialized;
    try {
        serialized = config.dump(4);
    } catch (const nlohmann::json::exception &error) {
        return std::unexpected(std::string("Cannot serialize config: ") + error.what());
    }

    // Create exclusively beside the destination so replacement stays on one filesystem.
    std::string temporaryPath = path + ".tmp.XXXXXX";
    int fd = ::mkstemp(temporaryPath.data());
    if (fd < 0) {
        return std::unexpected("Cannot create temporary config: " +
                               std::string(std::strerror(errno)));
    }
    std::FILE *stream = ::fdopen(fd, "wb");
    if (!stream) {
        int error = errno;
        ::close(fd);
        ::unlink(temporaryPath.c_str());
        return std::unexpected("Cannot open config stream: " + std::string(std::strerror(error)));
    }

    struct TemporaryFile {
        ~TemporaryFile() {
            if (stream) {
                std::fclose(stream);
            }
            if (!committed) {
                ::unlink(path.c_str());
            }
        }

        std::FILE *stream;
        const std::string &path;
        bool committed = false;
    } temporary{stream, temporaryPath};

    if (std::fwrite(serialized.data(), 1, serialized.size(), temporary.stream) !=
        serialized.size()) {
        return std::unexpected("Cannot write config: " + std::string(std::strerror(errno)));
    }
    // Closing also flushes buffered output; only replace the config if it succeeds.
    if (std::fclose(std::exchange(temporary.stream, nullptr)) != 0) {
        return std::unexpected("Cannot close config: " + std::string(std::strerror(errno)));
    }
    if (::rename(temporaryPath.c_str(), path.c_str()) != 0) {
        return std::unexpected("Cannot replace config: " + std::string(std::strerror(errno)));
    }
    temporary.committed = true;
    return {};
}

} // namespace smart_home::tui
