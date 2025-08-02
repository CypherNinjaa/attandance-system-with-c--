#include "attendance_system.h"

// Build JSON response for attendance data
std::string buildAttendanceResponse(const std::vector<AttendanceRecord>& records, const std::vector<Student>& students) {
    std::string response = "{\"success\": true, \"data\": [";
    
    bool first = true;
    for (const auto& record : records) {
        if (!first) response += ",";
        first = false;
        
        // Find student name
        std::string studentName = record.student_id;
        for (const auto& student : students) {
            if (student.id == record.student_id) {
                studentName = student.name;
                break;
            }
        }
        
        response += "{";
        response += "\"date\": \"" + record.date + "\",";
        response += "\"student_id\": \"" + record.student_id + "\",";
        response += "\"student_name\": \"" + studentName + "\",";
        response += "\"status\": \"" + record.status + "\",";
        response += "\"marked_by\": \"" + record.marked_by + "\"";
        response += "}";
    }
    
    response += "]}";
    return response;
}

// Build response for student statistics
std::string buildStudentStatsResponse(const std::string& student_id, const std::vector<AttendanceRecord>& records) {
    int totalDays = 0;
    int presentDays = 0;
    
    for (const auto& record : records) {
        if (record.student_id == student_id) {
            totalDays++;
            if (record.status == "PRESENT") {
                presentDays++;
            }
        }
    }
    
    double percentage = totalDays > 0 ? (double)presentDays / totalDays * 100 : 0;
    
    std::string response = "{\"success\": true, ";
    response += "\"stats\": {";
    response += "\"total_days\": " + std::to_string(totalDays) + ",";
    response += "\"present_days\": " + std::to_string(presentDays) + ",";
    response += "\"absent_days\": " + std::to_string(totalDays - presentDays) + ",";
    response += "\"percentage\": " + std::to_string(percentage);
    response += "}}";
    
    return response;
}

// Parse query string
std::map<std::string, std::string> parseQueryString() {
    std::map<std::string, std::string> params;
    char* queryString = getenv("QUERY_STRING");
    if (queryString) {
        std::string query(queryString);
        auto pairs = split(query, '&');
        for (const auto& pair : pairs) {
            auto keyValue = split(pair, '=');
            if (keyValue.size() == 2) {
                params[keyValue[0]] = keyValue[1];
            }
        }
    }
    return params;
}

int main() {
    printCGIHeaders();
    
    try {
        auto params = parseQueryString();
        
        std::string action = params["action"];
        std::string student_id = params["student_id"];
        
        auto records = loadAttendance();
        auto students = loadStudents();
        
        if (action == "stats" && !student_id.empty()) {
            // Return statistics for a specific student
            std::cout << buildStudentStatsResponse(student_id, records) << std::endl;
        } else if (action == "list") {
            // Return all attendance records or filtered by student
            std::vector<AttendanceRecord> filteredRecords;
            
            if (student_id.empty()) {
                filteredRecords = records;
            } else {
                for (const auto& record : records) {
                    if (record.student_id == student_id) {
                        filteredRecords.push_back(record);
                    }
                }
            }
            
            std::cout << buildAttendanceResponse(filteredRecords, students) << std::endl;
        } else if (action == "students") {
            // Return list of all students
            std::string response = "{\"success\": true, \"students\": [";
            bool first = true;
            for (const auto& student : students) {
                if (!first) response += ",";
                first = false;
                response += "{";
                response += "\"id\": \"" + student.id + "\",";
                response += "\"name\": \"" + student.name + "\",";
                response += "\"department\": \"" + student.department + "\",";
                response += "\"year\": \"" + student.year + "\"";
                response += "}";
            }
            response += "]}";
            std::cout << response << std::endl;
        } else {
            std::cout << "{\"success\": false, \"error\": \"Invalid action\"}" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "{\"success\": false, \"error\": \"" << e.what() << "\"}" << std::endl;
    }
    
    return 0;
}