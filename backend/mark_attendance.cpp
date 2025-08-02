#include "attendance_system.h"

// Simple JSON-like response builder for attendance marking
std::string buildMarkAttendanceResponse(bool success, const std::string& message = "") {
    std::string response = "{";
    response += "\"success\": " + std::string(success ? "true" : "false");
    if (!message.empty()) {
        response += ", \"message\": \"" + message + "\"";
    }
    response += "}";
    return response;
}

// Parse form data
std::map<std::string, std::string> parseFormData(const std::string& data) {
    std::map<std::string, std::string> params;
    auto pairs = split(data, '&');
    for (const auto& pair : pairs) {
        auto keyValue = split(pair, '=');
        if (keyValue.size() == 2) {
            params[keyValue[0]] = keyValue[1];
        }
    }
    return params;
}

int main() {
    printCGIHeaders();
    
    try {
        std::string postData = getPostData();
        auto params = parseFormData(postData);
        
        std::string date = params["date"];
        std::string student_id = params["student_id"];
        std::string status = params["status"];
        std::string marked_by = params["marked_by"];
        
        if (date.empty() || student_id.empty() || status.empty() || marked_by.empty()) {
            std::cout << buildMarkAttendanceResponse(false, "Missing required fields") << std::endl;
            return 1;
        }
        
        // Validate status
        if (status != "PRESENT" && status != "ABSENT") {
            std::cout << buildMarkAttendanceResponse(false, "Invalid status") << std::endl;
            return 1;
        }
        
        // Check if student exists
        auto students = loadStudents();
        bool studentExists = false;
        for (const auto& student : students) {
            if (student.id == student_id) {
                studentExists = true;
                break;
            }
        }
        
        if (!studentExists) {
            std::cout << buildMarkAttendanceResponse(false, "Student not found") << std::endl;
            return 1;
        }
        
        // Create attendance record
        AttendanceRecord record;
        record.date = date;
        record.student_id = student_id;
        record.status = status;
        record.marked_by = marked_by;
        
        if (saveAttendanceRecord(record)) {
            std::cout << buildMarkAttendanceResponse(true, "Attendance marked successfully") << std::endl;
        } else {
            std::cout << buildMarkAttendanceResponse(false, "Failed to save attendance") << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << buildMarkAttendanceResponse(false, e.what()) << std::endl;
    }
    
    return 0;
}