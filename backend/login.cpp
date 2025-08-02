#include "attendance_system.h"

// Simple JSON-like response builder
std::string buildLoginResponse(bool success, const User& user = {"", "", ""}, const std::string& error = "") {
    std::string response = "{";
    response += "\"success\": " + std::string(success ? "true" : "false");
    
    if (success && !user.id.empty()) {
        response += ", \"user\": {";
        response += "\"id\": \"" + user.id + "\",";
        response += "\"role\": \"" + user.role + "\"";
        
        // If it's a student, add student details
        if (user.role == "STUDENT") {
            auto students = loadStudents();
            for (const auto& student : students) {
                if (student.id == user.id) {
                    response += ", \"name\": \"" + student.name + "\",";
                    response += "\"department\": \"" + student.department + "\",";
                    response += "\"year\": \"" + student.year + "\"";
                    break;
                }
            }
        }
        response += "}";
    } else if (!error.empty()) {
        response += ", \"error\": \"" + error + "\"";
    }
    
    response += "}";
    return response;
}

// Simple URL decode function
std::string urlDecode(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '+') {
            result += ' ';
        } else if (str[i] == '%' && i + 2 < str.length()) {
            int hex = std::stoi(str.substr(i + 1, 2), nullptr, 16);
            result += static_cast<char>(hex);
            i += 2;
        } else {
            result += str[i];
        }
    }
    return result;
}

// Parse form data
std::map<std::string, std::string> parseFormData(const std::string& data) {
    std::map<std::string, std::string> params;
    auto pairs = split(data, '&');
    for (const auto& pair : pairs) {
        auto keyValue = split(pair, '=');
        if (keyValue.size() == 2) {
            params[urlDecode(keyValue[0])] = urlDecode(keyValue[1]);
        }
    }
    return params;
}

int main() {
    printCGIHeaders();
    
    try {
        std::string postData = getPostData();
        auto params = parseFormData(postData);
        
        std::string id = params["id"];
        std::string password = params["password"];
        
        if (id.empty() || password.empty()) {
            std::cout << buildLoginResponse(false, {"", "", ""}, "Missing credentials") << std::endl;
            return 1;
        }
        
        User user = authenticateUser(id, password);
        
        if (!user.id.empty()) {
            std::cout << buildLoginResponse(true, user) << std::endl;
        } else {
            std::cout << buildLoginResponse(false, {"", "", ""}, "Invalid credentials") << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << buildLoginResponse(false, {"", "", ""}, e.what()) << std::endl;
    }
    
    return 0;
}