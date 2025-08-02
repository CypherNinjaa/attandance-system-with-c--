#include "attendance_system.h"

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string getCurrentDate() {
    time_t now = time(0);
    char* dt = ctime(&now);
    std::string dateStr(dt);
    // Format: YYYY-MM-DD
    tm *ltm = localtime(&now);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", ltm);
    return std::string(buffer);
}

void printCGIHeaders() {
    std::cout << "Content-Type: application/json\r\n";
    std::cout << "Access-Control-Allow-Origin: *\r\n";
    std::cout << "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n";
    std::cout << "Access-Control-Allow-Headers: Content-Type\r\n";
    std::cout << "\r\n";
}

std::string getPostData() {
    char* contentLength = getenv("CONTENT_LENGTH");
    if (contentLength) {
        int length = std::stoi(contentLength);
        std::string data(length, '\0');
        std::cin.read(&data[0], length);
        return data;
    }
    return "";
}

std::vector<User> loadUsers() {
    std::vector<User> users;
    std::ifstream file("data/users.txt");
    std::string line;
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        auto tokens = split(line, ',');
        if (tokens.size() >= 3) {
            User user;
            user.id = tokens[0];
            user.password = tokens[1];
            user.role = tokens[2];
            users.push_back(user);
        }
    }
    return users;
}

std::vector<Student> loadStudents() {
    std::vector<Student> students;
    std::ifstream file("data/students.txt");
    std::string line;
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        auto tokens = split(line, ',');
        if (tokens.size() >= 4) {
            Student student;
            student.id = tokens[0];
            student.name = tokens[1];
            student.department = tokens[2];
            student.year = tokens[3];
            students.push_back(student);
        }
    }
    return students;
}

std::vector<AttendanceRecord> loadAttendance() {
    std::vector<AttendanceRecord> records;
    std::ifstream file("data/attendance.txt");
    std::string line;
    
    // Skip header line
    if (std::getline(file, line)) {
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            auto tokens = split(line, ',');
            if (tokens.size() >= 4) {
                AttendanceRecord record;
                record.date = tokens[0];
                record.student_id = tokens[1];
                record.status = tokens[2];
                record.marked_by = tokens[3];
                records.push_back(record);
            }
        }
    }
    return records;
}

bool saveAttendanceRecord(const AttendanceRecord& record) {
    std::ofstream file("data/attendance.txt", std::ios::app);
    if (file.is_open()) {
        file << record.date << "," << record.student_id << "," 
             << record.status << "," << record.marked_by << std::endl;
        file.close();
        return true;
    }
    return false;
}

User authenticateUser(const std::string& id, const std::string& password) {
    auto users = loadUsers();
    for (const auto& user : users) {
        if (user.id == id && user.password == password) {
            return user;
        }
    }
    return {"", "", ""};
}