#pragma once

#include "Handler.h"
#include <string>

class WebHandler : public Handler 
{
public:
    // doc_root: 本地文件根目录（不含尾/）
    explicit WebHandler(std::string doc_root);
    ~WebHandler() override = default;

    // 处理单个连接（读取请求、响应、然后返回；不负责 close(connfd) 的上下文可选择）
    void handler(int connfd) override;

private:
    std::string doc_root_;

    // helpers
    std::string url_decode(const std::string& s);
    // 规范化并合成相对于 doc_root_ 的路径；若非法或越界返回 empty string
    std::string make_safe_path(const std::string& url_path);
    std::string get_mime_type(const std::string& path);

    bool read_request(int connfd, std::string& out_request);
    bool send_all(int fd, const char* data, size_t len);

    void send_response_headers(int fd, int status, const std::string& status_text,
                               const std::string& content_type, size_t content_length,
                               bool close_conn = true);
    void send_400(int fd);
    void send_404(int fd);
    void send_501(int fd);
};
