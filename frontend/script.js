// Global state
let currentUser = null;
let students = [];
let attendanceData = [];

// DOM elements
const loginPage = document.getElementById('loginPage');
const adminDashboard = document.getElementById('adminDashboard');
const studentDashboard = document.getElementById('studentDashboard');
const loginForm = document.getElementById('loginForm');
const loginError = document.getElementById('loginError');

// Initialize application
document.addEventListener('DOMContentLoaded', function() {
    setupEventListeners();
    showLoginPage();
    setTodayDate();
});

function setupEventListeners() {
    // Login form
    loginForm.addEventListener('submit', handleLogin);
    
    // Logout buttons
    document.getElementById('logoutBtn').addEventListener('click', logout);
    document.getElementById('studentLogoutBtn').addEventListener('click', logout);
    
    // Navigation tabs
    document.getElementById('studentsTab').addEventListener('click', () => showTab('students'));
    document.getElementById('markAttendanceTab').addEventListener('click', () => showTab('markAttendance'));
    document.getElementById('viewAttendanceTab').addEventListener('click', () => showTab('viewAttendance'));
    
    // Save attendance button
    document.getElementById('saveAttendance').addEventListener('click', saveAttendance);
    
    // Refresh attendance button
    document.getElementById('refreshAttendance').addEventListener('click', loadAttendanceRecords);
    
    // Student filter
    document.getElementById('studentFilter').addEventListener('change', loadAttendanceRecords);
}

function setTodayDate() {
    const today = new Date().toISOString().split('T')[0];
    document.getElementById('attendanceDate').value = today;
}

async function handleLogin(e) {
    e.preventDefault();
    
    const formData = new FormData(loginForm);
    const params = new URLSearchParams(formData);
    
    try {
        const response = await fetch('/cgi-bin/login.cgi', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/x-www-form-urlencoded',
            },
            body: params
        });
        
        const data = await response.json();
        
        if (data.success) {
            currentUser = data.user;
            hideError();
            
            if (data.user.role === 'STUDENT') {
                showStudentDashboard();
            } else {
                showAdminDashboard();
            }
        } else {
            showError(data.error || 'Login failed');
        }
    } catch (error) {
        console.error('Login error:', error);
        showError('Connection error. Please try again.');
    }
}

function showError(message) {
    loginError.textContent = message;
    loginError.style.display = 'block';
}

function hideError() {
    loginError.style.display = 'none';
}

function showLoginPage() {
    loginPage.classList.remove('hidden');
    adminDashboard.classList.add('hidden');
    studentDashboard.classList.add('hidden');
}

async function showAdminDashboard() {
    loginPage.classList.add('hidden');
    adminDashboard.classList.remove('hidden');
    studentDashboard.classList.add('hidden');
    
    document.getElementById('userWelcome').textContent = `Welcome, ${currentUser.id}`;
    
    await loadStudents();
    showTab('students');
}

async function showStudentDashboard() {
    loginPage.classList.add('hidden');
    adminDashboard.classList.add('hidden');
    studentDashboard.classList.remove('hidden');
    
    document.getElementById('studentWelcome').textContent = `Welcome, ${currentUser.name || currentUser.id}`;
    
    await loadStudentStats();
    await loadStudentAttendance();
}

function logout() {
    currentUser = null;
    students = [];
    attendanceData = [];
    loginForm.reset();
    showLoginPage();
}

function showTab(tabName) {
    // Hide all tab contents
    document.querySelectorAll('.tab-content').forEach(tab => {
        tab.classList.add('hidden');
    });
    
    // Remove active class from all nav buttons
    document.querySelectorAll('.nav-btn').forEach(btn => {
        btn.classList.remove('active');
    });
    
    // Show selected tab and mark button as active
    switch (tabName) {
        case 'students':
            document.getElementById('studentsContent').classList.remove('hidden');
            document.getElementById('studentsTab').classList.add('active');
            displayStudents();
            break;
        case 'markAttendance':
            document.getElementById('markAttendanceContent').classList.remove('hidden');
            document.getElementById('markAttendanceTab').classList.add('active');
            displayAttendanceForm();
            break;
        case 'viewAttendance':
            document.getElementById('viewAttendanceContent').classList.remove('hidden');
            document.getElementById('viewAttendanceTab').classList.add('active');
            loadAttendanceRecords();
            break;
    }
}

async function loadStudents() {
    try {
        const response = await fetch('/cgi-bin/get_attendance.cgi?action=students');
        const data = await response.json();
        
        if (data.success) {
            students = data.students;
            populateStudentFilter();
        }
    } catch (error) {
        console.error('Error loading students:', error);
    }
}

function populateStudentFilter() {
    const select = document.getElementById('studentFilter');
    select.innerHTML = '<option value="">All Students</option>';
    
    students.forEach(student => {
        const option = document.createElement('option');
        option.value = student.id;
        option.textContent = `${student.name} (${student.id})`;
        select.appendChild(option);
    });
}

function displayStudents() {
    const container = document.getElementById('studentsList');
    container.innerHTML = '';
    
    students.forEach(student => {
        const studentCard = document.createElement('div');
        studentCard.className = 'student-card';
        studentCard.innerHTML = `
            <h3>${student.name}</h3>
            <div class="student-info">
                <p><strong>ID:</strong> ${student.id}</p>
                <p><strong>Department:</strong> ${student.department}</p>
                <p><strong>Year:</strong> ${student.year}</p>
            </div>
        `;
        container.appendChild(studentCard);
    });
}

function displayAttendanceForm() {
    const container = document.getElementById('attendanceTable');
    container.innerHTML = '';
    
    // Create header
    const header = document.createElement('div');
    header.className = 'attendance-row header';
    header.innerHTML = `
        <div>Student Name</div>
        <div>Student ID</div>
        <div>Status</div>
    `;
    container.appendChild(header);
    
    // Create rows for each student
    students.forEach(student => {
        const row = document.createElement('div');
        row.className = 'attendance-row';
        row.innerHTML = `
            <div>${student.name}</div>
            <div>${student.id}</div>
            <div class="status-controls">
                <button class="status-btn present" data-student="${student.id}" data-status="PRESENT">Present</button>
                <button class="status-btn absent" data-student="${student.id}" data-status="ABSENT">Absent</button>
            </div>
        `;
        container.appendChild(row);
    });
    
    // Add event listeners to status buttons
    container.addEventListener('click', handleStatusClick);
}

function handleStatusClick(e) {
    if (e.target.classList.contains('status-btn')) {
        const studentId = e.target.dataset.student;
        const status = e.target.dataset.status;
        
        // Remove active class from other buttons for this student
        const studentButtons = document.querySelectorAll(`[data-student="${studentId}"]`);
        studentButtons.forEach(btn => btn.classList.remove('active'));
        
        // Add active class to clicked button
        e.target.classList.add('active');
    }
}

async function saveAttendance() {
    const date = document.getElementById('attendanceDate').value;
    if (!date) {
        alert('Please select a date');
        return;
    }
    
    const activeButtons = document.querySelectorAll('.status-btn.active');
    if (activeButtons.length === 0) {
        alert('Please mark attendance for at least one student');
        return;
    }
    
    const promises = [];
    
    activeButtons.forEach(button => {
        const studentId = button.dataset.student;
        const status = button.dataset.status;
        
        const params = new URLSearchParams({
            date: date,
            student_id: studentId,
            status: status,
            marked_by: currentUser.id
        });
        
        promises.push(
            fetch('/cgi-bin/mark_attendance.cgi', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/x-www-form-urlencoded',
                },
                body: params
            })
        );
    });
    
    try {
        const responses = await Promise.all(promises);
        const results = await Promise.all(responses.map(r => r.json()));
        
        const successful = results.filter(r => r.success).length;
        const total = results.length;
        
        if (successful === total) {
            alert('Attendance saved successfully!');
            // Clear selections
            document.querySelectorAll('.status-btn.active').forEach(btn => {
                btn.classList.remove('active');
            });
        } else {
            alert(`Saved ${successful} out of ${total} records. Some failed.`);
        }
    } catch (error) {
        console.error('Error saving attendance:', error);
        alert('Error saving attendance. Please try again.');
    }
}

async function loadAttendanceRecords() {
    const studentFilter = document.getElementById('studentFilter').value;
    const params = new URLSearchParams({
        action: 'list',
        student_id: studentFilter
    });
    
    try {
        const response = await fetch(`/cgi-bin/get_attendance.cgi?${params}`);
        const data = await response.json();
        
        if (data.success) {
            displayAttendanceRecords(data.data);
        }
    } catch (error) {
        console.error('Error loading attendance records:', error);
    }
}

function displayAttendanceRecords(records) {
    const container = document.getElementById('attendanceRecords');
    
    if (records.length === 0) {
        container.innerHTML = '<p>No attendance records found.</p>';
        return;
    }
    
    let tableHTML = `
        <table>
            <thead>
                <tr>
                    <th>Date</th>
                    <th>Student ID</th>
                    <th>Student Name</th>
                    <th>Status</th>
                    <th>Marked By</th>
                </tr>
            </thead>
            <tbody>
    `;
    
    records.forEach(record => {
        const statusClass = record.status === 'PRESENT' ? 'status-present' : 'status-absent';
        tableHTML += `
            <tr>
                <td>${record.date}</td>
                <td>${record.student_id}</td>
                <td>${record.student_name}</td>
                <td class="${statusClass}">${record.status}</td>
                <td>${record.marked_by}</td>
            </tr>
        `;
    });
    
    tableHTML += '</tbody></table>';
    container.innerHTML = tableHTML;
}

async function loadStudentStats() {
    try {
        const response = await fetch(`/cgi-bin/get_attendance.cgi?action=stats&student_id=${currentUser.id}`);
        const data = await response.json();
        
        if (data.success) {
            const stats = data.stats;
            document.getElementById('totalDays').textContent = stats.total_days;
            document.getElementById('presentDays').textContent = stats.present_days;
            document.getElementById('absentDays').textContent = stats.absent_days;
            document.getElementById('attendancePercentage').textContent = `${stats.percentage.toFixed(1)}%`;
        }
    } catch (error) {
        console.error('Error loading student stats:', error);
    }
}

async function loadStudentAttendance() {
    const params = new URLSearchParams({
        action: 'list',
        student_id: currentUser.id
    });
    
    try {
        const response = await fetch(`/cgi-bin/get_attendance.cgi?${params}`);
        const data = await response.json();
        
        if (data.success) {
            displayStudentAttendanceRecords(data.data);
        }
    } catch (error) {
        console.error('Error loading student attendance:', error);
    }
}

function displayStudentAttendanceRecords(records) {
    const container = document.getElementById('studentAttendanceRecords');
    
    if (records.length === 0) {
        container.innerHTML = '<p>No attendance records found.</p>';
        return;
    }
    
    let tableHTML = `
        <table>
            <thead>
                <tr>
                    <th>Date</th>
                    <th>Status</th>
                    <th>Marked By</th>
                </tr>
            </thead>
            <tbody>
    `;
    
    records.forEach(record => {
        const statusClass = record.status === 'PRESENT' ? 'status-present' : 'status-absent';
        tableHTML += `
            <tr>
                <td>${record.date}</td>
                <td class="${statusClass}">${record.status}</td>
                <td>${record.marked_by}</td>
            </tr>
        `;
    });
    
    tableHTML += '</tbody></table>';
    container.innerHTML = tableHTML;
}