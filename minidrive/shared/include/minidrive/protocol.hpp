#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace minidrive {

struct Request {
    std::string cmd;
    nlohmann::json args;

    static Request from_json(const nlohmann::json& j) {
        Request req;
        req.cmd = j.value("cmd", "");
        req.args = j.value("args", nlohmann::json::object());
        return req;
    }

    nlohmann::json to_json() const {
        return nlohmann::json{{"cmd", cmd}, {"args", args}};
    }
};

struct Response {
    std::string status;
    int code = 0;
    std::string message;
    nlohmann::json data;

    nlohmann::json to_json() const {
        nlohmann::json j;
        j["status"] = status;
        j["code"] = code;
        j["message"] = message;
        if (!data.is_null()) {
            j["data"] = data;
        }
        return j;
    }

    static Response ok(const nlohmann::json& data = nlohmann::json::object()) {
        Response r;
        r.status = "OK";
        r.data = data;
        return r;
    }

    static Response error(int code, const std::string& msg, const nlohmann::json& data = nlohmann::json::object()) {
        Response r;
        r.status = "ERROR";
        r.code = code;
        r.message = msg;
        r.data = data;
        return r;
    }
};

std::string read_message(int fd);
void write_message(int fd, const std::string& msg);

}
