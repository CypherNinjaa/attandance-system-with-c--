#ifndef ATTENDANCE_SYSTEM_H
#define ATTENDANCE_SYSTEM_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <ctime>
#include <map>

// User structure
struct User {
    std::string id;
    std::string password;
    std::string role;
};

// Student structure
struct Student {
    std::string id;
    std::string name;
    std::string department;
    std::string year;
};

// Attendance record structure
struct AttendanceRecord {
    std::string date;
    std::string student_id;
    std::string status;
    std::string marked_by;
};

// Utility functions
std::vector<std::string> split(const std::string& str, char delimiter);
std::string getCurrentDate();
void printCGIHeaders();
std::string getPostData();
std::vector<User> loadUsers();
std::vector<Student> loadStudents();
std::vector<AttendanceRecord> loadAttendance();
bool saveAttendanceRecord(const AttendanceRecord& record);
User authenticateUser(const std::string& id, const std::string& password);

#endif