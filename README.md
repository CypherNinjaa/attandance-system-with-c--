# Attendance Management System

A complete attendance management system built with HTML/CSS/JavaScript frontend and C++ backend with CGI interface.

## Features

### Admin/Faculty Features
- Login with admin/faculty credentials
- View list of all students
- Mark attendance for students (Present/Absent)
- View attendance records for all students or filter by specific student
- Real-time attendance tracking

### Student Features
- Login with student ID
- View personal attendance statistics (total days, present days, absent days, percentage)
- View detailed attendance records

## System Architecture

- **Frontend**: HTML, CSS, JavaScript (responsive design)
- **Backend**: C++ with CGI interface
- **Data Storage**: Text files (CSV format)
- **Communication**: HTTP requests via CGI

## File Structure

```
/
├── frontend/
│   ├── index.html          # Main HTML file with all pages
│   ├── style.css           # Responsive CSS styling
│   └── script.js           # JavaScript functionality
├── backend/
│   ├── attendance_system.h # Header file with structures and functions
│   ├── attendance_system.cpp # Utility functions implementation
│   ├── login.cpp           # Authentication handler
│   ├── mark_attendance.cpp # Attendance marking handler
│   └── get_attendance.cpp  # Attendance retrieval handler
├── cgi-bin/
│   ├── login.cgi           # Compiled login executable
│   ├── mark_attendance.cgi # Compiled attendance marking executable
│   └── get_attendance.cgi  # Compiled attendance retrieval executable
├── data/
│   ├── users.txt           # User credentials (format: id,password,role)
│   ├── students.txt        # Student information (format: id,name,department,year)
│   └── attendance.txt      # Attendance records (format: date,student_id,status,marked_by)
├── Makefile               # Build configuration
├── roadmap.md            # Project requirements
└── README.md             # This file
```

## Setup Instructions

### Prerequisites
- C++ compiler (g++)
- Web server with CGI support (Apache, Nginx, or simple HTTP server)
- Modern web browser

### Building the System

1. **Clone/Navigate to the repository**
   ```bash
   cd /path/to/attendance-system-with-c--
   ```

2. **Build the C++ backend**
   ```bash
   make
   ```
   This will compile all C++ files and create CGI executables in the `cgi-bin/` directory.

3. **Set executable permissions**
   ```bash
   make install
   ```

### Running the System

#### Option 1: Using Apache with CGI
1. Configure Apache to serve the `frontend/` directory
2. Configure CGI to execute files from `cgi-bin/` directory
3. Ensure proper permissions for CGI execution

#### Option 2: Using Python Simple Server (for testing)
1. Start a simple HTTP server from the frontend directory:
   ```bash
   cd frontend
   python3 -m http.server 8000
   ```
2. Configure CGI support or use a reverse proxy for CGI execution

### Default Login Credentials

#### Admin/Faculty
- **Username**: admin
- **Password**: admin123
- **Role**: ADMIN

- **Username**: faculty
- **Password**: faculty123
- **Role**: FACULTY

#### Students
- **Username**: S001
- **Password**: student123
- **Name**: John Doe

- **Username**: S002
- **Password**: student123
- **Name**: Jane Smith

- **Username**: S003
- **Password**: student123
- **Name**: Mike Johnson

## Usage

1. **Login**: Navigate to the main page and login with appropriate credentials
2. **Admin/Faculty Dashboard**:
   - **Students Tab**: View list of all students
   - **Mark Attendance Tab**: Select date and mark attendance for students
   - **View Attendance Tab**: View attendance records with optional filtering
3. **Student Dashboard**:
   - View attendance statistics and detailed records

## Data Files Format

### users.txt
```
id,password,role
admin,admin123,ADMIN
faculty,faculty123,FACULTY
S001,student123,STUDENT
```

### students.txt
```
id,name,department,year
S001,John Doe,Computer Science,2024
S002,Jane Smith,Information Technology,2024
```

### attendance.txt
```
DATE,STUDENT_ID,STATUS,MARKED_BY
2024-01-15,S001,PRESENT,admin
2024-01-15,S002,ABSENT,faculty
```

## API Endpoints

### POST /cgi-bin/login.cgi
Authenticate user with credentials
- **Body**: `id=username&password=password`
- **Response**: JSON with user information

### POST /cgi-bin/mark_attendance.cgi
Mark attendance for a student
- **Body**: `date=YYYY-MM-DD&student_id=ID&status=PRESENT|ABSENT&marked_by=USER`
- **Response**: JSON with success status

### GET /cgi-bin/get_attendance.cgi
Retrieve attendance data
- **Query Parameters**:
  - `action=list` - Get attendance records
  - `action=stats&student_id=ID` - Get student statistics
  - `action=students` - Get list of students
- **Response**: JSON with requested data

## Development

### Adding New Students
Add entries to `data/students.txt` and `data/users.txt`

### Modifying UI
Edit `frontend/style.css` for styling changes
Edit `frontend/script.js` for functionality changes

### Extending Backend
Add new C++ files in `backend/` directory
Update Makefile to include new targets

## Browser Compatibility
- Chrome/Chromium (recommended)
- Firefox
- Safari
- Edge

## Security Notes
- This is a demonstration system with basic authentication
- In production, implement proper password hashing
- Use HTTPS for secure communication
- Validate and sanitize all inputs
- Implement proper session management

## Troubleshooting

### Build Issues
- Ensure g++ is installed and accessible
- Check file permissions in backend directory

### CGI Issues
- Verify web server CGI configuration
- Check executable permissions on CGI files
- Ensure correct file paths in C++ code

### Data Issues
- Verify data file format and permissions
- Check file paths in backend code relative to CGI execution directory