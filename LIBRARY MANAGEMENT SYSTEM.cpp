#include <iostream>
#include <fstream>
#include <windows.h>
#include <sstream>
#include <string>
#include <cstdlib>  

using namespace std;  


const string DB_FILE = "library02.csv";
const string HEADER  = "BookID,Title,Author,Genre,TotalCopies,AvailableCopies,IssuedTo,IssueDate,DueDate";


// HELPER: getField(line, index)
// PURPOSE : Extract a specific field from a CSV line by its
//           position (0-based index).
//           e.g. index 0 = BookID, index 5 = AvailableCopies

string getField(string line, int index) {

    int count = 0;         // tracks which field we are currently on
    string field = "";     // stores the characters of the current field

    // Loop through every character in the line
    for (int i = 0; i < line.length(); i++) {

        if (line[i] == ',') {
            // Comma means end of current field
            if (count == index) return field; // found our target field
            count++;       // move to next field
            field = "";    // reset for next field
        } else {
            field += line[i]; // build up the field character by character
        }
    }

    return field; // return last field (no trailing comma)
}



// FUNCTION 1: initializeDatabase()
// PURPOSE : Check if library.csv exists.
//           If it does NOT exist, create it and write the
//           column headers as the first line.
//           If it already exists, do nothing.

void initializeDatabase() {

    // Try to open the file in read mode to check if it exists
    // .c_str() converts string to const char* which older compilers require
    ifstream checkFile(DB_FILE.c_str());

    if (checkFile.is_open()) {
        // File already exists — no action needed
        cout << "Database already exists: " << DB_FILE << endl;
        checkFile.close();
    } else {
        // File does not exist — create it and write the header row
        ofstream newFile(DB_FILE.c_str());

        if (newFile.is_open()) {
            newFile << HEADER << "\n";
            newFile.close();
            cout << "Database created: " << DB_FILE << endl;
        } else {
            cout << "ERROR: Could not create database file." << endl;
        }
    }
}


// FUNCTION 2: isUnique(string id)
// PURPOSE : Before adding a new book, scan every line of the
//           CSV and check if the given BookID already exists.
//           Returns true  → ID is unique (safe to add)
//           Returns false → ID already exists (duplicate!)

bool isUnique(string id) {

    ifstream file(DB_FILE.c_str());
    string line;

    if (!file.is_open()) {
        cout << "ERROR: Could not open database." << endl;
        return false;
    }

    // Skip the header line so we don't match against column names
    getline(file, line);

    // Read each record line by line
    while (getline(file, line)) {

        // Extract only the first field (BookID) before the first comma
        string existingID = line.substr(0, line.find(','));

        // Compare with the ID we are checking
        if (existingID == id) {
            file.close();
            return false; // Duplicate found
        }
    }

    file.close();
    return true; // No match found — ID is unique
}


// FUNCTION 3: appendRecord(string data)
// PURPOSE : Add a new book record at the END of the CSV file.
//           The caller must pass a complete comma-separated
//           string, e.g.:
//           "BK_107,Atomic Habits,James Clear,Self-Help,3,3,NULL,NULL,NULL"
//           This function first calls isUnique() to prevent
//           duplicate BookIDs from being inserted.

void appendRecord(string data) {

    // Extract the BookID from the data string (field before first comma)
    string newID = data.substr(0, data.find(','));

    // Check for duplicate before writing
    if (!isUnique(newID)) {
        cout << "ERROR: BookID " << newID << " already exists. Record not added." << endl;
        return;
    }

    // Open file in append mode so existing data is not overwritten
    ofstream file(DB_FILE.c_str(), ios::app);

    if (!file.is_open()) {
        cout << "ERROR: Could not open database for writing." << endl;
        return;
    }

    // Write the new record as a new line at the end of the file
    file << data << "\n";
    file.close();

    cout << "Record added successfully: " << newID << endl;
}


// FUNCTION 4: searchByID(string id)
// PURPOSE : Perform a sequential search through every line
//           of the CSV to find the record matching the given
//           BookID (sent by the ESP32 via serial).
//           Prints the full record if found, or a not-found
//           message if no match exists.

void searchByID(string id) {

    ifstream file(DB_FILE.c_str());
    string line;
    bool found = false;

    if (!file.is_open()) {
        cout << "ERROR: Could not open database." << endl;
        return;
    }

    // Skip the header line
    getline(file, line);

    // Read each record and compare the first field (BookID)
    while (getline(file, line)) {

        // The BookID is everything before the first comma
        string currentID = line.substr(0, line.find(','));

        if (currentID == id) {
            // Match found — print each field on its own labeled line
            // This is much easier to read than the raw CSV format
            cout << "\n---------------------------" << endl;
            cout << "         BOOK FOUND        "   << endl;
            cout << "---------------------------"   << endl;
            cout << "Book ID      : " << getField(line, 0) << endl;
            cout << "Title        : " << getField(line, 1) << endl;
            cout << "Author       : " << getField(line, 2) << endl;
            cout << "Genre        : " << getField(line, 3) << endl;
            cout << "Total Copies : " << getField(line, 4) << endl;
            cout << "Available    : " << getField(line, 5) << endl;
            cout << "Issued To    : " << getField(line, 6) << endl;
            cout << "Issue Date   : " << getField(line, 7) << endl;
            cout << "Due Date     : " << getField(line, 8) << endl;
            cout << "---------------------------"         << endl;
            found = true;
            break; // Stop searching after first match (IDs are unique)
        }
    }

    if (!found) {
        cout << "No record found for BookID: " << id << endl;
    }

    file.close();
}


// FUNCTION 5: updateRecord(string id, string newData)
// PURPOSE : Modify OR delete a record identified by BookID.
//           To UPDATE: pass the full new comma-separated row
//                      as newData.
//           To DELETE: pass an empty string "" as newData.
//
//           Strategy: read every line into a TEMP file,
//           replace or skip the matched line, then rename
//           the temp file back to library.csv.
//           This protects data integrity — the original file
//           is only replaced after the temp file is complete.

void updateRecord(string id, string newData) {

    ifstream file(DB_FILE.c_str());
    string   tempFile = "library_temp.csv";
    ofstream temp(tempFile.c_str());
    string   line;
    bool          found = false;

    if (!file.is_open() || !temp.is_open()) {
        cout << "ERROR: Could not open database or temp file." << endl;
        return;
    }

    // Always copy the header line first
    getline(file, line);
    temp << line << "\n";

    // Go through every record line by line
    while (getline(file, line)) {

        // Extract BookID from the current line
        string currentID = line.substr(0, line.find(','));

        if (currentID == id) {
            found = true;

            if (newData.empty()) {
                // DELETE: simply skip writing this line to the temp file
                cout << "Record deleted: " << id << endl;
            } else {
                // UPDATE: write the new data instead of the old line
                temp << newData << "\n";
                cout << "Record updated: " << id << endl;
            }
        } else {
            // Not the target record — copy it unchanged to temp file
            temp << line << "\n";
        }
    }

    file.close();
    temp.close();

    if (!found) {
        cout << "No record found for BookID: " << id << endl;
        // Remove temp file since no change was made
        remove(tempFile.c_str());
        return;
    }

    // Replace original file with the updated temp file
    remove(DB_FILE.c_str());
    rename(tempFile.c_str(), DB_FILE.c_str());
}


// FUNCTION 6: issueBook()
// PURPOSE : Issue a book to a student.
//           Steps:
//           1. Ask user for BookID and StudentID and DueDate
//           2. Find the book record in the CSV
//           3. Check if at least 1 copy is available
//           4. Decrease AvailableCopies by 1
//           5. Fill in IssuedTo, IssueDate, DueDate fields
//           6. Save the updated record using updateRecord()
// NOTE    : Uses updateRecord() internally — not a new function

void issueBook() {

    string bookID, studentID, dueDate;

    // Get input from user
    cout << "\nEnter Book ID to issue (e.g. BK_005): ";
    cin  >> bookID;
    cout << "Enter Student ID (e.g. STD_012): ";
    cin  >> studentID;
    cout << "Enter Due Date (e.g. 2025-06-01): ";
    cin  >> dueDate;

    // Open the CSV to find the book record
    ifstream file(DB_FILE.c_str());
    string line;
    string foundLine = ""; // will store the matching record line

    if (!file.is_open()) {
        cout << "ERROR: Could not open database." << endl;
        return;
    }

    // Skip header
    getline(file, line);

    // Search for the book line by line
    while (getline(file, line)) {
        string currentID = line.substr(0, line.find(','));
        if (currentID == bookID) {
            foundLine = line; // save the matching line
            break;
        }
    }
    file.close();

    // Book not found in database
    if (foundLine == "") {
        cout << "ERROR: BookID " << bookID << " not found." << endl;
        return;
    }

    // Extract AvailableCopies (field index 5) from the found record
    string availStr = getField(foundLine, 5);
    int availCopies = 0;

    // Convert the string to an integer manually (atoi needs c_str)
    availCopies = atoi(availStr.c_str());

    // Check if any copy is available to issue
    if (availCopies <= 0) {
        cout << "Sorry, no copies of this book are currently available." << endl;
        return;
    }

    // Decrease available copies by 1
    int newAvail = availCopies - 1;

    // Get today's date as issue date (user enters it)
    string issueDate;
    cout << "Enter Today's Date as Issue Date (e.g. 2025-05-09): ";
    cin  >> issueDate;

    // Rebuild the full updated CSV line field by field
    // Fields: BookID,Title,Author,Genre,TotalCopies,AvailableCopies,IssuedTo,IssueDate,DueDate
    string updatedLine = "";
    updatedLine += getField(foundLine, 0) + ","; // BookID
    updatedLine += getField(foundLine, 1) + ","; // Title
    updatedLine += getField(foundLine, 2) + ","; // Author
    updatedLine += getField(foundLine, 3) + ","; // Genre
    updatedLine += getField(foundLine, 4) + ","; // TotalCopies (unchanged)

    // Convert newAvail integer back to string for CSV
    // Using stringstream since older compilers may not have to_string()
    stringstream ss;
    ss << newAvail;
    updatedLine += ss.str()  + ","; // AvailableCopies (decreased by 1)

    updatedLine += studentID + ","; // IssuedTo
    updatedLine += issueDate + ","; // IssueDate
    updatedLine += dueDate;         // DueDate (no trailing comma)

    // Save the updated record back to the CSV
    updateRecord(bookID, updatedLine);

    cout << "Book " << bookID << " successfully issued to " << studentID << "." << endl;
    cout << "Due Date: " << dueDate << endl;
}


// FUNCTION 7: returnBook()
// PURPOSE : Return a previously issued book.
//           Steps:
//           1. Ask user for BookID
//           2. Find the record in CSV
//           3. Check that the book is actually issued (not NULL)
//           4. Increase AvailableCopies by 1
//           5. Reset IssuedTo, IssueDate, DueDate back to NULL
//           6. Save the updated record using updateRecord()

void returnBook() {

    string bookID;

    // Get Book ID from user
    cout << "\nEnter Book ID to return (e.g. BK_005): ";
    cin  >> bookID;

    // Open CSV to find the record
    ifstream file(DB_FILE.c_str());
    string line;
    string foundLine = "";

    if (!file.is_open()) {
        cout << "ERROR: Could not open database." << endl;
        return;
    }

    // Skip header
    getline(file, line);

    // Search for the book
    while (getline(file, line)) {
        string currentID = line.substr(0, line.find(','));
        if (currentID == bookID) {
            foundLine = line;
            break;
        }
    }
    file.close();

    // Book not found
    if (foundLine == "") {
        cout << "ERROR: BookID " << bookID << " not found." << endl;
        return;
    }

    // Check IssuedTo field (index 6) — if NULL, book is not currently issued
    string issuedTo = getField(foundLine, 6);
    if (issuedTo == "NULL") {
        cout << "This book is not currently issued to anyone." << endl;
        return;
    }

    // Increase AvailableCopies by 1
    string availStr  = getField(foundLine, 5);
    int availCopies  = atoi(availStr.c_str());
    int newAvail     = availCopies + 1;

    // Convert newAvail back to string using stringstream
    stringstream ss;
    ss << newAvail;

    // Rebuild the updated CSV line — reset issue fields back to NULL
    string updatedLine = "";
    updatedLine += getField(foundLine, 0) + ","; // BookID
    updatedLine += getField(foundLine, 1) + ","; // Title
    updatedLine += getField(foundLine, 2) + ","; // Author
    updatedLine += getField(foundLine, 3) + ","; // Genre
    updatedLine += getField(foundLine, 4) + ","; // TotalCopies (unchanged)
    updatedLine += ss.str()  + ",";              // AvailableCopies (increased by 1)
    updatedLine += "NULL,";                      // IssuedTo  → reset
    updatedLine += "NULL,";                      // IssueDate → reset
    updatedLine += "NULL";                       // DueDate   → reset

    // Save the updated record back to CSV
    updateRecord(bookID, updatedLine);

    cout << "Book " << bookID << " successfully returned." << endl;
}


// FUNCTION 8: addBook()
// PURPOSE : Ask the user to enter all book details and then
//           call appendRecord() to save it to the CSV.
//           New books always start with full copies available
//           and no issue information (NULL fields).
void addBook() {

    string id, title, author, genre;
    int totalCopies;

    cout << "\n--- Add New Book ---" << endl;
    cout << "Enter Book ID (e.g. BK_107): ";
    cin  >> id;

    // Clear input buffer before reading full lines with spaces
    cin.ignore();

    cout << "Enter Title: ";
    getline(cin, title);

    cout << "Enter Author: ";
    getline(cin, author);

    cout << "Enter Genre: ";
    getline(cin, genre);

    cout << "Enter Total Copies: ";
    cin  >> totalCopies;

    // Convert totalCopies to string using stringstream
    stringstream ss;
    ss << totalCopies;
    string copiesStr = ss.str();

    // Build the full CSV record — new book has all copies available, no issue info
    string record = id + "," + title + "," + author + "," + genre + ","
                  + copiesStr + "," + copiesStr + ",NULL,NULL,NULL";

    // appendRecord() will check for duplicate ID before saving
    appendRecord(record);
}


// FUNCTION 9: deleteBook()
// PURPOSE : Ask user for a BookID and delete it from the CSV
//           by calling updateRecord() with empty newData.

void deleteBook() {

    string bookID;
    cout << "\nEnter Book ID to delete (e.g. BK_005): ";
    cin  >> bookID;

    // Passing empty string as second argument triggers delete in updateRecord()
    updateRecord(bookID, "");
}

// FUNCTION 10: ESP32 authentication

bool authenticateESP32() {

    HANDLE hSerial = CreateFile("\\\\.\\COM3", GENERIC_READ | GENERIC_WRITE,
                                0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hSerial == INVALID_HANDLE_VALUE) {
        return false;
    }

    DCB dcb = {0};
    dcb.DCBlength = sizeof(dcb);
    GetCommState(hSerial, &dcb);

    dcb.BaudRate = CBR_115200;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity   = NOPARITY;

    SetCommState(hSerial, &dcb);

    Sleep(3000); // ESP32 reset delay
    PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);

    char request = 'R';
    DWORD written;

    WriteFile(hSerial, &request, 1, &written, NULL);

    Sleep(1000);

    char buffer[20] = {0};
    DWORD read;

    ReadFile(hSerial, buffer, sizeof(buffer)-1, &read, NULL);

    CloseHandle(hSerial);

    string key = buffer;

    return (key.find("Fada4fff") != string::npos);
}
// PURPOSE : Show a menu in a loop. User picks an option, the
//           matching function runs, then menu shows again.
//           Loop exits only when user chooses 0.

int main() {

    cout << "Checking ESP32 security device...\n";

    if (!authenticateESP32()) {
        cout << "ESP32 not connected!\nAccess Denied. Program Closing...\n";
        return 0;
    }

    cout << "Access Granted. Welcome!\n";

    initializeDatabase();

    int choice; // stores the user's menu selection

    // Keep showing the menu until the user chooses to exit
    do {
        // Display menu options
        cout << "\n===============================" << endl;
        cout << "  LIBRARY MANAGEMENT SYSTEM   "   << endl;
        cout << "==============================="   << endl;
        cout << "1. Search a Book by ID"            << endl;
        cout << "2. Add a New Book"                 << endl;
        cout << "3. Issue a Book to Student"        << endl;
        cout << "4. Return a Book"                  << endl;
        cout << "5. Delete a Book"                  << endl;
        cout << "0. Exit"                           << endl;
        cout << "==============================="   << endl;
        cout << "Enter your choice: ";
        cin  >> choice;

        // Call the appropriate function based on user choice
        switch (choice) {

            case 1: {
                // Search: ask for Book ID and display the record
                // Curly braces needed here because we declare a variable
                // inside a case — C++ requires a scope block for this
                string searchID;
                cout << "\nEnter Book ID to search (e.g. BK_003): ";
                cin  >> searchID;
                searchByID(searchID);
                break;
            }

            case 2:
                // Add: collect details and append to CSV
                addBook();
                break;

            case 3:
                // Issue: fill IssuedTo, IssueDate, DueDate fields
                issueBook();
                break;

            case 4:
                // Return: clear issue fields, restore available copy
                returnBook();
                break;

            case 5:
                // Delete: remove the record entirely from CSV
                deleteBook();
                break;

            case 0:
                // Exit the program
                cout << "Goodbye!" << endl;
                break;

            default:
                // Any other number is invalid
                cout << "Invalid choice. Please enter 0-5." << endl;
        }

    } while (choice != 0); // keep looping until user enters 0

    return 0;
}
