#include "config_io.h"

#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <memory>
#include <utility>

namespace smart_home::app {
namespace {
ConfigError ioError(const std::string &path, const std::string &operation, int error = errno) {
    return {error == ENOENT ? ConfigErrorKind::MISSING : ConfigErrorKind::IO,
            operation + " (" + path + "): " + std::strerror(error)};
}

std::expected<std::string, ConfigError> canonicalPath(const std::string &path) {
    std::error_code error;
    auto resolved = std::filesystem::canonical(path, error);
    if (error) {
        return std::unexpected(ioError(path, "Cannot resolve config", error.value()));
    }
    return resolved.string();
}

class Lock {
  public:
    explicit Lock(int fd) : m_fd(fd) {}
    ~Lock() {
        if (m_fd >= 0)
            ::close(m_fd);
    }
    Lock(const Lock &) = delete;
    Lock &operator=(const Lock &) = delete;
    Lock(Lock &&other) noexcept : m_fd(std::exchange(other.m_fd, -1)) {}

  private:
    int m_fd;
};

std::expected<Lock, ConfigError> lockConfig(const std::string &path, int mode) {
    const auto lockPath = path + ".lock";
    int fd = ::open(lockPath.c_str(), O_CREAT | O_RDWR | O_CLOEXEC, 0600);
    if (fd < 0)
        return std::unexpected(ioError(path, "Cannot open config lock"));
    if (::flock(fd, mode | LOCK_NB) != 0) {
        auto error = ioError(path, "Cannot lock config; retry after the other operation finishes");
        ::close(fd);
        return std::unexpected(error);
    }
    return Lock(fd);
}

std::expected<std::string, ConfigError> readBytes(const std::string &path) {
    const auto closeFile = [](std::FILE *file) { std::fclose(file); };
    std::unique_ptr<std::FILE, decltype(closeFile)> stream(std::fopen(path.c_str(), "rb"),
                                                           closeFile);
    if (!stream)
        return std::unexpected(ioError(path, "Cannot open config"));
    std::string contents;
    char buffer[4096];
    while (const auto count = std::fread(buffer, 1, sizeof(buffer), stream.get())) {
        contents.append(buffer, count);
    }
    if (std::ferror(stream.get()))
        return std::unexpected(ioError(path, "Cannot read config"));
    if (std::fclose(stream.release()) != 0)
        return std::unexpected(ioError(path, "Cannot close config"));
    return contents;
}
} // namespace

std::expected<ConfigSnapshot, ConfigError> loadConfig(const std::string &path) {
    auto resolved = canonicalPath(path);
    if (!resolved)
        return std::unexpected(resolved.error());
    auto lock = lockConfig(*resolved, LOCK_SH);
    if (!lock)
        return std::unexpected(lock.error());
    auto bytes = readBytes(*resolved);
    if (!bytes)
        return std::unexpected(bytes.error());
    try {
        return ConfigSnapshot{nlohmann::json::parse(*bytes), std::move(*bytes)};
    } catch (const nlohmann::json::exception &error) {
        return std::unexpected(ConfigError{ConfigErrorKind::PARSE,
                                           "Cannot parse config (" + path + "): " + error.what()});
    }
}

std::expected<ConfigSnapshot, ConfigError> saveConfig(const std::string &path,
                                                      const nlohmann::json &config,
                                                      const std::string &expectedVersion) {
    std::string serialized;
    try {
        serialized = config.dump(4) + "\n";
    } catch (const nlohmann::json::exception &error) {
        return std::unexpected(
            ConfigError{ConfigErrorKind::VALIDATION,
                        "Cannot serialize config (" + path + "): " + error.what()});
    }
    auto resolved = canonicalPath(path);
    if (!resolved)
        return std::unexpected(resolved.error());
    auto lock = lockConfig(*resolved, LOCK_EX);
    if (!lock)
        return std::unexpected(lock.error());
    auto current = readBytes(*resolved);
    if (!current)
        return std::unexpected(current.error());
    if (*current != expectedVersion) {
        return std::unexpected(
            ConfigError{ConfigErrorKind::CONFLICT,
                        "Config changed externally (" + path +
                            "). Reload before saving; pending edits were retained."});
    }
    struct stat metadata{};
    if (::stat(resolved->c_str(), &metadata) != 0)
        return std::unexpected(ioError(path, "Cannot inspect config"));

    std::string temporaryPath = *resolved + ".tmp.XXXXXX";
    int fd = ::mkstemp(temporaryPath.data());
    if (fd < 0)
        return std::unexpected(ioError(path, "Cannot create temporary config"));
    std::FILE *stream = ::fdopen(fd, "wb");
    if (!stream) {
        auto error = ioError(path, "Cannot open config stream");
        ::close(fd);
        ::unlink(temporaryPath.c_str());
        return std::unexpected(error);
    }
    struct TemporaryFile {
        ~TemporaryFile() {
            if (stream)
                std::fclose(stream);
            if (!committed)
                ::unlink(path.c_str());
        }
        std::FILE *stream;
        const std::string &path;
        bool committed = false;
    } temporary{stream, temporaryPath};
    if (::fchmod(fd, metadata.st_mode & 0777) != 0)
        return std::unexpected(ioError(path, "Cannot preserve config permissions"));
    if (std::fwrite(serialized.data(), 1, serialized.size(), stream) != serialized.size()) {
        return std::unexpected(ioError(path, "Cannot write config"));
    }
    if (std::fclose(std::exchange(temporary.stream, nullptr)) != 0) {
        return std::unexpected(ioError(path, "Cannot close config"));
    }
    if (::rename(temporaryPath.c_str(), resolved->c_str()) != 0) {
        return std::unexpected(ioError(path, "Cannot replace config"));
    }
    temporary.committed = true;
    return ConfigSnapshot{config, std::move(serialized)};
}
} // namespace smart_home::app
