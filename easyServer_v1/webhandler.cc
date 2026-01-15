#include "webhandler.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>

#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>

namespace {

constexpr size_t MAX_REQUEST_SIZE = 8 * 1024; // 最大请求头 8KB
constexpr size_t SEND_BUF_SIZE = 64 * 1024;  // 64KB 文件发送缓冲

// trim helpers
inline std::string ltrim(const std::string &s) {
    size_t i = 0;
    while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
    return s.substr(i);
}
inline std::string rtrim(const std::string &s) {
    if (s.empty()) return s;
    size_t i = s.size();
    while (i > 0 && std::isspace(static_cast<unsigned char>(s[i-1]))) --i;
    return s.substr(0, i);
}
inline std::string trim(const std::string &s) {
    return rtrim(ltrim(s));
}

} // namespace

WebHandler::WebHandler(std::string doc_root)
    : doc_root_(std::move(doc_root))
{
    // 移除尾部可能的 '/'
    if (!doc_root_.empty() && doc_root_.back() == '/')
        doc_root_.pop_back();
}

bool WebHandler::read_request(int connfd, std::string& out_request)
{
    out_request.clear();
    std::string buf;
    buf.reserve(1024);

    char tmp[1024];
    ssize_t n;
    // 读取直到遇到空行 CRLF CRLF 或达到最大限制
    while (out_request.find("\r\n\r\n") == std::string::npos &&
           out_request.size() < MAX_REQUEST_SIZE) {
        n = recv(connfd, tmp, sizeof(tmp), 0);
        if (n == 0) {
            // 对端关闭连接
            break;
        } else if (n < 0) {
            if (errno == EINTR) continue;
            return false;
        } else {
            out_request.append(tmp, static_cast<size_t>(n));
        }
    }
    if (out_request.size() > MAX_REQUEST_SIZE) return false;
    return !out_request.empty();
}

std::string WebHandler::url_decode(const std::string& s)
{
    std::string out;
    out.reserve(s.size());
    for (size_t i = 0; i < s.size(); ++i) {
        char c = s[i];
        if (c == '%') {
            if (i + 2 < s.size()) {
                auto hex = s.substr(i+1, 2);
                char decoded = static_cast<char>(std::strtol(hex.c_str(), nullptr, 16));
                out.push_back(decoded);
                i += 2;
            } else {
                // 不完整的编码，直接忽略
            }
        } else if (c == '+') {
            out.push_back(' ');
        } else {
            out.push_back(c);
        }
    }
    return out;
}

std::string WebHandler::make_safe_path(const std::string& url_path)
{
    // 只处理 PATH 部分（例如: /index.html 或 /a/b/../c）
    // 去掉查询字符串
    std::string path = url_path;
    auto qpos = path.find('?');
    if (qpos != std::string::npos) path.resize(qpos);

    // 解码
    path = url_decode(path);

    // 必须以 '/' 开头
    if (path.empty() || path[0] != '/') return {};

    // 分割并规范化，防止 ../
    std::vector<std::string> parts;
    size_t i = 1; // 跳过开头 '/'
    while (i <= path.size()) {
        size_t j = path.find('/', i);
        if (j == std::string::npos) j = path.size();
        std::string seg = path.substr(i, j - i);
        if (!seg.empty() && seg != ".") {
            if (seg == "..") {
                if (parts.empty()) {
                    // 试图脱离根目录，非法
                    return {};
                }
                parts.pop_back();
            } else {
                parts.push_back(seg);
            }
        }
        i = j + 1;
    }

    // 如果 path 指向网页目录, 则映射到 index.html
    if (parts.empty()) {
        parts.push_back("index.html");
    }

    // 组合到 doc_root_
    std::string full = doc_root_;
    for (const auto& p : parts) {
        full.push_back('/');
        full.append(p);
    }
    return full;
}

std::string WebHandler::get_mime_type(const std::string& path)
{
    auto pos = path.rfind('.');
    std::string ext;
    if (pos != std::string::npos) ext = path.substr(pos + 1);

    // 简单映射，按需扩展
    if (ext == "html" || ext == "htm") return "text/html; charset=utf-8";
    if (ext == "css") return "text/css; charset=utf-8";
    if (ext == "js") return "application/javascript";
    if (ext == "png") return "image/png";
    if (ext == "jpg" || ext == "jpeg") return "image/jpeg";
    if (ext == "gif") return "image/gif";
    if (ext == "svg") return "image/svg+xml";
    if (ext == "json") return "application/json";
    if (ext == "txt") return "text/plain; charset=utf-8";
    if (ext == "pdf") return "application/pdf";
    return "application/octet-stream";
}

bool WebHandler::send_all(int fd, const char* data, size_t len)
{
    size_t sent = 0;
    while (sent < len) {
        //循环写块，std::min设置了上限
        ssize_t n = send(fd, data + sent, static_cast<size_t>(std::min<size_t>(len - sent, 64*1024)), 0);
        if (n < 0) {
            if (errno == EINTR) continue;
            return false;
        }
        sent += static_cast<size_t>(n);
    }
    return true;
}

void WebHandler::send_response_headers(int fd, int status, const std::string& status_text,
                                       const std::string& content_type, size_t content_length,
                                       bool close_conn)
{
    std::ostringstream ss;
    ss << "HTTP/1.1 " << status << " " << status_text << "\r\n";
    ss << "Content-Length: " << content_length << "\r\n";
    if (!content_type.empty())
        ss << "Content-Type: " << content_type << "\r\n";
    ss << "Connection: " << (close_conn ? "close" : "keep-alive") << "\r\n";
    ss << "\r\n";
    std::string hdr = ss.str();
    send_all(fd, hdr.data(), hdr.size());
}

void WebHandler::send_400(int fd)
{
    const char body[] = "<html><body><h1>400 Bad Request</h1></body></html>\n";
    send_response_headers(fd, 400, "Bad Request", "text/html; charset=utf-8", sizeof(body)-1);
    send_all(fd, body, sizeof(body)-1);
}

void WebHandler::send_404(int fd)
{
    const char body[] = "<html><body><h1>404 Not Found</h1></body></html>\n";
    send_response_headers(fd, 404, "Not Found", "text/html; charset=utf-8", sizeof(body)-1);
    send_all(fd, body, sizeof(body)-1);
}

void WebHandler::send_501(int fd)
{
    const char body[] = "<html><body><h1>501 Not Implemented</h1></body></html>\n";
    send_response_headers(fd, 501, "Not Implemented", "text/html; charset=utf-8", sizeof(body)-1);
    send_all(fd, body, sizeof(body)-1);
}

void WebHandler::handler(int connfd)
{
    std::string request;
    if (!read_request(connfd, request)) {
        // Bad or empty request
        send_400(connfd);
        return;
    }

    // 简单解析请求行
    std::istringstream reqs(request);
    std::string request_line;
    if (!std::getline(reqs, request_line)) {
        send_400(connfd);
        return;
    }
    // 删除末尾 \r
    if (!request_line.empty() && request_line.back() == '\r')
        request_line.pop_back();

    std::istringstream rl(request_line);
    std::string method, url, version;
    rl >> method >> url >> version;

    if (method != "GET") {
        send_501(connfd);
        return;
    }

    // 规范路径
    std::string safe_path = make_safe_path(url);
    if (safe_path.empty()) {
        send_400(connfd);
        return;
    }

    // 打开文件
    std::ifstream ifs(safe_path, std::ios::in | std::ios::binary);
    if (!ifs) {
        send_404(connfd);
        return;
    }

    // 获取文件长度
    ifs.seekg(0, std::ios::end);
    std::streamoff fsize = ifs.tellg();
    if (fsize < 0) fsize = 0;
    ifs.seekg(0, std::ios::beg);

    std::string mime = get_mime_type(safe_path);
    send_response_headers(connfd, 200, "OK", mime, static_cast<size_t>(fsize));

    // 逐块读取并发送
    std::vector<char> buf(SEND_BUF_SIZE);
    while (ifs) {
        ifs.read(buf.data(), static_cast<std::streamsize>(buf.size()));
        std::streamsize r = ifs.gcount();
        if (r > 0) {
            if (!send_all(connfd, buf.data(), static_cast<size_t>(r))) {
                // 发送失败，停止
                break;
            }
        }
    }
    // 处理完成，调用者负责 close(connfd)
}