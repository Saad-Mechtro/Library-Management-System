# 📚 Secure Library Management System Using ESP32 Authentication

## Overview

The Secure Library Management System is a C++-based application that combines library record management with ESP32-based hardware authentication. The system verifies an authorized ESP32 device before granting access to the application, providing an additional layer of security beyond traditional software-only authentication methods.

The application allows users to search books, add new books, issue books to students, return issued books, and delete records. All data is stored in a CSV database, making the system lightweight, portable, and easy to deploy without requiring external database software.

This project demonstrates the practical integration of software engineering, file handling, serial communication, authentication systems, and embedded systems.

---

## Features

### 🔐 Security Features

* ESP32-Based Hardware Authentication
* Secure Access Control
* Authentication Key Verification
* Unauthorized Access Prevention
* Serial Communication Security

### 📚 Library Management Features

* Search Books by Book ID
* Add New Books
* Issue Books to Students
* Return Issued Books
* Delete Book Records
* Track Book Availability
* Manage Due Dates
* Automatic Record Updates

### 🗄️ Database Features

* CSV-Based Database Storage
* Automatic Database Creation
* Persistent Data Storage
* Duplicate Book ID Prevention
* Efficient Record Management

---

## Technologies Used

### Programming Language

* C++

### Hardware

* ESP32 Development Board

### Libraries Used

```cpp
#include <iostream>
#include <fstream>
#include <windows.h>
#include <sstream>
#include <string>
#include <cstdlib>
```

### Concepts Implemented

* File Handling
* Serial Communication
* CSV Data Processing
* Authentication Systems
* Sequential Search
* Modular Programming
* Data Validation
* Database Management

---

## System Architecture

```text
ESP32 Security Device
          │
          ▼
Serial Communication
          │
          ▼
C++ Application
          │
          ▼
CSV Database
```

---

## Program Execution Flow

```text
Start Program
      │
      ▼
Authenticate ESP32
      │
      ▼
Initialize Database
      │
      ▼
Display Main Menu
      │
      ▼
User Selects Operation
      │
      ▼
Execute Function
      │
      ▼
Update Database
      │
      ▼
Return to Menu
```

---

## Database Structure

The system stores records in:

```text
library02.csv
```

### Database Fields

| Field           | Description            |
| --------------- | ---------------------- |
| BookID          | Unique Book Identifier |
| Title           | Book Title             |
| Author          | Author Name            |
| Genre           | Book Category          |
| TotalCopies     | Total Number of Copies |
| AvailableCopies | Available Copies       |
| IssuedTo        | Student ID             |
| IssueDate       | Book Issue Date        |
| DueDate         | Return Due Date        |

---

## Main Functions

### 🔒 Authentication Function

#### authenticateESP32()

Purpose:

* Establish serial communication with ESP32.
* Send authentication request.
* Receive security key.
* Verify authenticity.
* Grant or deny access.

Windows API Functions Used:

```cpp
CreateFile()
GetCommState()
SetCommState()
WriteFile()
ReadFile()
CloseHandle()
```

---

### 🗄️ Database Functions

#### initializeDatabase()

Purpose:

* Check whether database exists.
* Create database automatically if missing.
* Insert column headers.

---

#### getField()

Purpose:

* Extract a specific field from a CSV record.

Example:

```text
BK_001,C++ Programming,Bjarne Stroustrup,Programming
```

---

#### isUnique()

Purpose:

* Verify uniqueness of Book IDs.
* Prevent duplicate records.

---

#### appendRecord()

Purpose:

* Insert new records into the database.

---

#### updateRecord()

Purpose:

* Update existing records.
* Delete records safely using temporary file replacement.

---

### 📖 Library Management Functions

#### searchByID()

Purpose:

* Search books using Book ID.

Algorithm Used:

* Sequential Search

---

#### addBook()

Purpose:

* Add new books to the database.

---

#### issueBook()

Purpose:

* Issue books to students.
* Update availability count.
* Store student and due date information.

---

#### returnBook()

Purpose:

* Return issued books.
* Restore available copies.
* Clear issue information.

---

#### deleteBook()

Purpose:

* Permanently remove book records.

---

## Algorithms Used

### Sequential Search

Used In:

* searchByID()
* isUnique()

Working:

* Reads records line by line.
* Compares Book IDs.
* Stops when a match is found.

---

### Authentication Verification

Used In:

* authenticateESP32()

Working:

```text
Send Request
      │
      ▼
Receive Key
      │
      ▼
Compare Key
      │
      ▼
Access Granted / Denied
```

---

### Temporary File Replacement Algorithm

Used In:

* updateRecord()

Working:

```text
Read Original File
        │
        ▼
Create Temp File
        │
        ▼
Modify Required Record
        │
        ▼
Delete Original File
        │
        ▼
Rename Temp File
```

---

## Sample Menu

```text
===============================
  LIBRARY MANAGEMENT SYSTEM
===============================
1. Search a Book by ID
2. Add a New Book
3. Issue a Book to Student
4. Return a Book
5. Delete a Book
0. Exit
===============================
```

---

## Testing Performed

✅ Authentication Testing

✅ Database Initialization Testing

✅ Search Function Testing

✅ Add Book Testing

✅ Issue Book Testing

✅ Return Book Testing

✅ Delete Book Testing

✅ Data Persistence Testing

---

## Future Enhancements

* GUI-Based Interface
* RFID Authentication
* Barcode Scanner Integration
* MySQL Database
* Cloud Storage Support
* Student Login Portal
* Fine Management System
* Mobile Application
* Online Library Access

---

## Learning Outcomes

Through this project, the following concepts were practically implemented:

* C++ Programming
* File Handling
* CSV Database Management
* Serial Communication
* ESP32 Integration
* Authentication Systems
* Software Design Principles
* Modular Programming
* Debugging and Testing

---
## Installation and Usage

### Requirements

* Windows Operating System
* C++ Compiler (Dev-C++, Code::Blocks, Visual Studio, or MinGW)
* ESP32 Development Board
* USB Data Cable

---

### Setup Instructions

1. Clone or download the repository.

```bash
git clone https://github.com/your-repository-link.git
```

2. Open the project in your preferred C++ IDE.

3. Ensure that the CSV database file (`library02.csv`) is present in the same directory as the executable.

4. Connect the ESP32 device to your computer.

5. Verify the correct COM port in the source code:

```cpp
CreateFile("\\\\.\\COM3", ...)
```

If your ESP32 is connected to a different COM port, update the port number accordingly.

6. Compile and run the project.

---

### Authentication

The system uses ESP32-based hardware authentication.

When the application starts:

1. The software sends an authentication request to the ESP32.
2. The ESP32 responds with a predefined security key.
3. The application verifies the received key.
4. Access is granted only if the key matches.

Default Authentication Key:

```text
Fada4fff
```

> Note: This key is included for demonstration and academic purposes. In a production environment, authentication credentials should be stored securely and not exposed in source code.

---

### Using the System

After successful authentication, the following menu will appear:

```text
1. Search a Book by ID
2. Add a New Book
3. Issue a Book to Student
4. Return a Book
5. Delete a Book
0. Exit
```

#### Search a Book

* Select option `1`
* Enter the Book ID
* The system displays complete book information

#### Add a New Book

* Select option `2`
* Enter Book ID, Title, Author, Genre, and Total Copies
* The record will be saved to the database

#### Issue a Book

* Select option `3`
* Enter Book ID
* Enter Student ID
* Enter Due Date and Issue Date
* Available copies will be updated automatically

#### Return a Book

* Select option `4`
* Enter the Book ID
* The system restores available copies and clears issue information

#### Delete a Book

* Select option `5`
* Enter the Book ID
* The selected record will be permanently removed

---

### Sample Login Flow

```text
Checking ESP32 security device...
Authentication Key Verified.
Access Granted. Welcome!
```

The user can then access all library management features.

## Contributors

| Name                      | GitHub Username   |
| ------------------------- | ----------------- |
| Hafiz Muhammad Saad Habib | @Saad-Mechtro     |
| Ahmad Hassan              | @ahmedmechtro-afk |
| Obaid Ur Rehman           | @obaid.mechtro    |

---

## Conclusion

The Secure Library Management System successfully integrates C++ programming, CSV database management, and ESP32-based hardware authentication into a single application. The system provides secure access control, efficient record management, and reliable book tracking while demonstrating the practical integration of software engineering and embedded systems concepts.

---

## License

This project is developed for educational and academic purposes.
