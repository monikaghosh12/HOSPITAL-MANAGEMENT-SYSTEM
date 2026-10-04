#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <ctime>
#include <fcntl.h>
#include <unistd.h>
#include <limits>

using namespace std;

// ============================================================
// HOSPITAL MANAGEMENT & TELEMETRY SYSTEM
// Standard C++17 for Linux OS
// ============================================================

// ------------------------- STRUCTURES -------------------------

struct Patient {
    long long id;
    string name;
    int age;
    string gender;
    string phone;
    string department;
    string bloodGroup;
    string status;
};

struct Appointment {
    int id;
    long long patientId;
    int doctorId;
    string department;
    string date;
    string time;
    string status; // "Scheduled" or "Cancelled"
};

struct Doctor {
    int id;
    string name;
    string department;
    string specialization;
    string status; // "Available" or "Not Available"
};

struct Bill {
    int id;
    long long patientId;
    string service;
    double amount;
    string date;
    string status; // "Pending" or "Paid"
};

struct Medicine {
    int id;
    string name;
    string category;
    int stock;
    string expiry;
    string status; // "Available" or "Out"
};

// ------------------------- GLOBAL DATA -------------------------

vector<Patient> patients;
vector<Appointment> appointments;
vector<Doctor> doctors;
vector<Bill> bills;
vector<Medicine> medicines;

// ------------------------- FUNCTION DECLARATIONS -------------------------

void savePatients();
void loadPatients();

void saveAppointments();
void loadAppointments();

void saveDoctors();
void loadDoctors();

void saveBills();
void loadBills();

void saveMedicines();
void loadMedicines();

void dashboard();
void patientManagement();
void appointmentManagement();
void doctorManagement();
void billingManagement();
void inventoryManagement();
void searchPatientModule();
void readIcuVitalsSensor();

// Helper & Search prototypes
bool patientExists(long long id);
bool doctorExists(int id);
bool appointmentExists(int id);
bool billExists(int id);
bool medicineExists(int id);

string getPatientName(long long id);
string getDoctorName(int id);
string getCurrentDate();
void pauseScreen();

// Safe Input Helpers (Prevents cin crashing)
int getIntInput(const string& prompt);
long long getLongInput(const string& prompt);
double getDoubleInput(const string& prompt);
string getNonEmptyString(const string& prompt);

// ------------------------- INPUT HELPERS -------------------------

int getIntInput(const string& prompt) {
    int val;
    while (true) {
        cout << prompt;
        if (cin >> val) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return val;
        }
        cout << "Invalid input! Please enter a valid number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

long long getLongInput(const string& prompt) {
    long long val;
    while (true) {
        cout << prompt;
        if (cin >> val) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return val;
        }
        cout << "Invalid input! Please enter a numeric ID.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

double getDoubleInput(const string& prompt) {
    double val;
    while (true) {
        cout << prompt;
        if (cin >> val && val >= 0.0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return val;
        }
        cout << "Invalid amount! Please enter a valid non-negative decimal value (e.g. 600.00).\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string getNonEmptyString(const string& prompt) {
    string str;
    while (true) {
        cout << prompt;
        getline(cin, str);
        if (!str.empty()) return str;
        cout << "Field cannot be empty. Please enter text.\n";
    }
}

void pauseScreen() {
    cout << "\nPress Enter to continue...";
    cin.get();
}

// ------------------------- VALIDATION HELPERS -------------------------

bool patientExists(long long id) {
    for (const auto& p : patients) {
        if (p.id == id) return true;
    }
    return false;
}

bool doctorExists(int id) {
    for (const auto& d : doctors) {
        if (d.id == id) return true;
    }
    return false;
}

bool appointmentExists(int id) {
    for (const auto& a : appointments) {
        if (a.id == id) return true;
    }
    return false;
}

bool billExists(int id) {
    for (const auto& b : bills) {
        if (b.id == id) return true;
    }
    return false;
}

bool medicineExists(int id) {
    for (const auto& m : medicines) {
        if (m.id == id) return true;
    }
    return false;
}

string getPatientName(long long id) {
    for (const auto& p : patients) {
        if (p.id == id) return p.name;
    }
    return "Unknown";
}

string getDoctorName(int id) {
    for (const auto& d : doctors) {
        if (d.id == id) return d.name;
    }
    return "Unknown";
}

string getCurrentDate() {
    time_t now = time(nullptr);
    tm* localTime = localtime(&now);
    stringstream ss;
    ss << setfill('0')
       << setw(2) << localTime->tm_mday << "/"
       << setw(2) << localTime->tm_mon + 1 << "/"
       << localTime->tm_year + 1900;
    return ss.str();
}

// ------------------------- LINUX DEVICE DRIVER INTERFACE -------------------------

void readIcuVitalsSensor() {
    cout << "\n================ ICU TELEMETRY (LINUX DEVICE DRIVER) ================\n";
    int fd = open("/dev/vitals_sensor", O_RDONLY);
    if (fd < 0) {
        cout << "[WARNING] /dev/vitals_sensor node not active.\n";
        cout << "Run the background driver: sudo ./vitals_driver &\n";
        return;
    }

    char buffer[128];
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        cout << "Kernel Telemetry Stream : " << buffer;
        cout << "Hardware Driver Device  : /dev/vitals_sensor (POSIX Character Device)\n";
    } else {
        cout << "Awaiting packet from kernel device...\n";
    }
    close(fd);
    cout << "=====================================================================\n";
}

// ============================================================
// FILE PERSISTENCE ENGINE
// ============================================================

void savePatients() {
    ofstream file("patients.txt");
    if (!file) return;
    for (const auto& p : patients) {
        file << p.id << "|" << p.name << "|" << p.age << "|" << p.gender << "|"
             << p.phone << "|" << p.department << "|" << p.bloodGroup << "|" << p.status << "\n";
    }
}

void loadPatients() {
    ifstream file("patients.txt");
    if (!file) return;
    patients.clear();
    Patient p;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string id, age;
        getline(ss, id, '|');
        getline(ss, p.name, '|');
        getline(ss, age, '|');
        getline(ss, p.gender, '|');
        getline(ss, p.phone, '|');
        getline(ss, p.department, '|');
        getline(ss, p.bloodGroup, '|');
        getline(ss, p.status);
        try {
            p.id = stoll(id);
            p.age = stoi(age);
            patients.push_back(p);
        } catch (...) {}
    }
}

void saveAppointments() {
    ofstream file("appointments.txt");
    if (!file) return;
    for (const auto& a : appointments) {
        file << a.id << "|" << a.patientId << "|" << a.doctorId << "|"
             << a.department << "|" << a.date << "|" << a.time << "|" << a.status << "\n";
    }
}

void loadAppointments() {
    ifstream file("appointments.txt");
    if (!file) return;
    appointments.clear();
    Appointment a;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string id, patientId, doctorId;
        getline(ss, id, '|');
        getline(ss, patientId, '|');
        getline(ss, doctorId, '|');
        getline(ss, a.department, '|');
        getline(ss, a.date, '|');
        getline(ss, a.time, '|');
        getline(ss, a.status);
        try {
            a.id = stoi(id);
            a.patientId = stoll(patientId);
            a.doctorId = stoi(doctorId);
            appointments.push_back(a);
        } catch (...) {}
    }
}

void saveDoctors() {
    ofstream file("doctors.txt");
    if (!file) return;
    for (const auto& d : doctors) {
        file << d.id << "|" << d.name << "|" << d.department << "|"
             << d.specialization << "|" << d.status << "\n";
    }
}

void loadDoctors() {
    ifstream file("doctors.txt");
    if (!file) return;
    doctors.clear();
    Doctor d;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string id;
        getline(ss, id, '|');
        getline(ss, d.name, '|');
        getline(ss, d.department, '|');
        getline(ss, d.specialization, '|');
        getline(ss, d.status);
        try {
            d.id = stoi(id);
            doctors.push_back(d);
        } catch (...) {}
    }
}

void saveBills() {
    ofstream file("bills.txt");
    if (!file) return;
    for (const auto& b : bills) {
        file << b.id << "|" << b.patientId << "|" << b.service << "|"
             << fixed << setprecision(2) << b.amount << "|"
             << b.date << "|" << b.status << "\n";
    }
}

void loadBills() {
    ifstream file("bills.txt");
    if (!file) return;
    bills.clear();
    Bill b;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string id, patientId, amount;
        getline(ss, id, '|');
        getline(ss, patientId, '|');
        getline(ss, b.service, '|');
        getline(ss, amount, '|');
        getline(ss, b.date, '|');
        getline(ss, b.status);
        try {
            b.id = stoi(id);
            b.patientId = stoll(patientId);
            b.amount = stod(amount);
            bills.push_back(b);
        } catch (...) {}
    }
}

void saveMedicines() {
    ofstream file("medicines.txt");
    if (!file) return;
    for (const auto& m : medicines) {
        file << m.id << "|" << m.name << "|" << m.category << "|"
             << m.stock << "|" << m.expiry << "|" << m.status << "\n";
    }
}

void loadMedicines() {
    ifstream file("medicines.txt");
    if (!file) return;
    medicines.clear();
    Medicine m;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string id, stock;
        getline(ss, id, '|');
        getline(ss, m.name, '|');
        getline(ss, m.category, '|');
        getline(ss, stock, '|');
        getline(ss, m.expiry, '|');
        getline(ss, m.status);
        try {
            m.id = stoi(id);
            m.stock = stoi(stock);
            medicines.push_back(m);
        } catch (...) {}
    }
}

// ============================================================
// 1. DASHBOARD MODULE
// ============================================================

void dashboard() {
    int availableDoctors = 0;
    int pendingBills = 0;
    int todayAppointments = 0;
    string today = getCurrentDate();

    for (const auto& d : doctors) {
        if (d.status == "Available") availableDoctors++;
    }
    for (const auto& b : bills) {
        if (b.status == "Pending") pendingBills++;
    }
    for (const auto& a : appointments) {
        if (a.date == today && (a.status == "Scheduled" || a.status == "Booked")) todayAppointments++;
    }

    cout << "\n====================================================\n";
    cout << "                HOSPITAL DASHBOARD                  \n";
    cout << "====================================================\n";
    cout << "Total Patients        : " << patients.size() << "\n";
    cout << "Today's Appointments  : " << todayAppointments << "\n";
    cout << "Available Doctors     : " << availableDoctors << "\n";
    cout << "Pending Bills         : " << pendingBills << "\n";

    readIcuVitalsSensor();

    cout << "\n---------------- TODAY'S APPOINTMENTS ----------------\n";
    bool appFound = false;
    for (const auto& a : appointments) {
        if (a.date == today && (a.status == "Scheduled" || a.status == "Booked")) {
            cout << "ID: " << a.id << " | Patient: " << getPatientName(a.patientId)
                 << " | Doctor: " << getDoctorName(a.doctorId)
                 << " | Dept: " << a.department
                 << " | Time: " << a.time << " (" << a.status << ")\n";
            appFound = true;
        }
    }
    if (!appFound) cout << "No appointments scheduled for today (" << today << ").\n";

    cout << "\n---------------- RECENT PATIENTS --------------------\n";
    if (patients.empty()) {
        cout << "No patients registered yet.\n";
    } else {
        int count = 0;
        for (auto it = patients.rbegin(); it != patients.rend() && count < 5; ++it, ++count) {
            cout << "ID: " << it->id << " | Name: " << it->name
                 << " | Dept: " << it->department << " | Status: " << it->status << "\n";
        }
    }

    cout << "\n---------------- DOCTORS ON DUTY --------------------\n";
    bool docFound = false;
    for (const auto& d : doctors) {
        if (d.status == "Available") {
            cout << "ID: " << d.id << " | Dr. " << d.name
                 << " | Dept: " << d.department
                 << " | Spec: " << d.specialization << "\n";
            docFound = true;
        }
    }
    if (!docFound) cout << "No doctors are currently on duty/available.\n";

    cout << "====================================================\n";
    pauseScreen();
}

// ============================================================
// 2. PATIENT MANAGEMENT
// ============================================================

void patientManagement() {
    int choice;
    do {
        cout << "\n========== PATIENT MANAGEMENT ==========\n";
        cout << "1. Add Patient\n";
        cout << "2. View Patients\n";
        cout << "3. Search Patient\n";
        cout << "4. Update Patient\n";
        cout << "5. Delete Patient\n";
        cout << "6. Back to Main Menu\n";
        choice = getIntInput("Enter your choice: ");

        if (choice == 1) {
            Patient p;
            while (true) {
                p.id = getLongInput("Enter Patient ID (Numeric): ");
                if (p.id <= 0) {
                    cout << "Patient ID must be a positive number.\n";
                    continue;
                }
                if (patientExists(p.id)) {
                    cout << "Patient ID already exists! Please enter a unique ID.\n";
                } else {
                    break;
                }
            }

            // Name Validation (letters and spaces only)
            while (true) {
                cout << "Enter Patient Name: ";
                getline(cin, p.name);
                bool valid = !p.name.empty();
                for (char c : p.name) {
                    if (!isalpha(static_cast<unsigned char>(c)) && c != ' ') {
                        valid = false;
                        break;
                    }
                }
                if (valid) break;
                cout << "Invalid name! Use letters and spaces only.\n";
            }

            // Age Validation (1-99)
            while (true) {
                p.age = getIntInput("Enter Age (1-99): ");
                if (p.age >= 1 && p.age <= 99) break;
                cout << "Age must be between 1 and 99.\n";
            }

            // Gender Validation
            while (true) {
                cout << "Enter Gender (Male/Female): ";
                cin >> p.gender;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (p.gender == "Male" || p.gender == "Female") break;
                cout << "Invalid input! Please enter exactly 'Male' or 'Female'.\n";
            }

            // Phone Validation (exactly 10 digits)
            while (true) {
                cout << "Enter Phone Number (10 digits): ";
                cin >> p.phone;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                bool valid = (p.phone.length() == 10);
                for (char c : p.phone) {
                    if (!isdigit(static_cast<unsigned char>(c))) valid = false;
                }
                if (valid) break;
                cout << "Phone number must contain exactly 10 numeric digits.\n";
            }

            p.department = getNonEmptyString("Enter Department: ");

            // Blood Group Validation
            while (true) {
                cout << "Enter Blood Group (A+/A-/B+/B-/AB+/AB-/O+/O-): ";
                cin >> p.bloodGroup;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (p.bloodGroup == "A+" || p.bloodGroup == "A-" ||
                    p.bloodGroup == "B+" || p.bloodGroup == "B-" ||
                    p.bloodGroup == "AB+" || p.bloodGroup == "AB-" ||
                    p.bloodGroup == "O+" || p.bloodGroup == "O-") {
                    break;
                }
                cout << "Invalid blood group!\n";
            }

            p.status = "Active";
            patients.push_back(p);
            savePatients();
            cout << "\nPatient added successfully!\n";
            pauseScreen();

        } else if (choice == 2) {
            cout << "\n================ PATIENT LIST ================\n";
            if (patients.empty()) {
                cout << "No patients registered.\n";
            } else {
                for (const auto& p : patients) {
                    cout << "\nID          : " << p.id;
                    cout << "\nName        : " << p.name;
                    cout << "\nAge         : " << p.age;
                    cout << "\nGender      : " << p.gender;
                    cout << "\nPhone       : " << p.phone;
                    cout << "\nDepartment  : " << p.department;
                    cout << "\nBlood Group : " << p.bloodGroup;
                    cout << "\nStatus      : " << p.status;
                    cout << "\n----------------------------------------------";
                }
                cout << "\n";
            }
            pauseScreen();

        } else if (choice == 3) {
            searchPatientModule();

        } else if (choice == 4) {
            long long id = getLongInput("Enter Patient ID to update: ");
            bool found = false;
            for (auto& p : patients) {
                if (p.id == id) {
                    p.department = getNonEmptyString("Enter New Department: ");
                    
                    while (true) {
                        cout << "Enter New Phone (10 digits): ";
                        cin >> p.phone;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        bool valid = (p.phone.length() == 10);
                        for (char c : p.phone) {
                            if (!isdigit(static_cast<unsigned char>(c))) valid = false;
                        }
                        if (valid) break;
                        cout << "Phone number must be exactly 10 digits.\n";
                    }

                    while (true) {
                        cout << "Enter New Status (Active/Inactive): ";
                        cin >> p.status;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        if (p.status == "Active" || p.status == "Inactive") break;
                        cout << "Please enter 'Active' or 'Inactive'.\n";
                    }

                    savePatients();
                    cout << "Patient details updated successfully!\n";
                    found = true;
                    break;
                }
            }
            if (!found) cout << "Patient not found.\n";
            pauseScreen();

        } else if (choice == 5) {
            long long id = getLongInput("Enter Patient ID to delete: ");
            bool found = false;
            for (auto it = patients.begin(); it != patients.end(); ++it) {
                if (it->id == id) {
                    patients.erase(it);
                    savePatients();
                    cout << "Patient deleted successfully!\n";
                    found = true;
                    break;
                }
            }
            if (!found) cout << "Patient not found.\n";
            pauseScreen();
        }
    } while (choice != 6);
}

// ============================================================
// 3. APPOINTMENT MANAGEMENT
// ============================================================

void appointmentManagement() {
    int choice;
    do {
        cout << "\n========== APPOINTMENT MANAGEMENT ==========\n";
        cout << "1. Book Appointment\n";
        cout << "2. View Appointments\n";
        cout << "3. Cancel Appointment\n";
        cout << "4. Back to Main Menu\n";
        choice = getIntInput("Enter your choice: ");

        if (choice == 1) {
            Appointment a;
            while (true) {
                a.id = getIntInput("Enter Appointment ID (Numeric): ");
                if (appointmentExists(a.id)) {
                    cout << "Appointment ID already exists! Please enter a different ID.\n";
                } else {
                    break;
                }
            }

            while (true) {
                a.patientId = getLongInput("Enter Patient ID: ");
                if (patientExists(a.patientId)) break;
                cout << "Patient ID not found! Please register the patient first.\n";
            }

            cout << "\nAvailable Doctors on Duty:\n";
            bool docFound = false;
            for (const auto& d : doctors) {
                if (d.status == "Available") {
                    cout << "ID: " << d.id << " - Dr. " << d.name << " (" << d.department << ")\n";
                    docFound = true;
                }
            }
            if (!docFound) {
                cout << "No doctors are currently available. Please add or update doctors first.\n";
                pauseScreen();
                continue;
            }

            while (true) {
                a.doctorId = getIntInput("Enter Doctor ID from list: ");
                if (doctorExists(a.doctorId)) break;
                cout << "Doctor not found! Please enter a valid Doctor ID.\n";
            }

            a.department = getNonEmptyString("Enter Department: ");
            a.date = getNonEmptyString("Enter Date (DD/MM/YYYY): ");
            a.time = getNonEmptyString("Enter Time (HH:MM): ");
            a.status = "Scheduled";

            appointments.push_back(a);
            saveAppointments();
            cout << "Appointment booked successfully!\n";
            pauseScreen();

        } else if (choice == 2) {
            cout << "\n=============== APPOINTMENT LIST ===============\n";
            if (appointments.empty()) {
                cout << "No appointments on record.\n";
            } else {
                for (const auto& a : appointments) {
                    cout << "\nAppointment ID : " << a.id;
                    cout << "\nPatient        : " << getPatientName(a.patientId) << " (ID: " << a.patientId << ")";
                    cout << "\nDoctor         : " << getDoctorName(a.doctorId) << " (ID: " << a.doctorId << ")";
                    cout << "\nDepartment     : " << a.department;
                    cout << "\nDate           : " << a.date;
                    cout << "\nTime           : " << a.time;
                    cout << "\nStatus         : " << a.status;
                    cout << "\n------------------------------------------------";
                }
                cout << "\n";
            }
            pauseScreen();

        } else if (choice == 3) {
            int id = getIntInput("Enter Appointment ID to cancel: ");
            bool found = false;
            for (auto& a : appointments) {
                if (a.id == id) {
                    a.status = "Cancelled";
                    saveAppointments();
                    cout << "Appointment marked as Cancelled successfully!\n";
                    found = true;
                    break;
                }
            }
            if (!found) cout << "Appointment ID not found.\n";
            pauseScreen();
        }
    } while (choice != 4);
}

// ============================================================
// 4. DOCTOR MANAGEMENT
// ============================================================

void doctorManagement() {
    int choice;
    do {
        cout << "\n========== DOCTOR MANAGEMENT ==========\n";
        cout << "1. Add Doctor\n";
        cout << "2. View Doctors\n";
        cout << "3. Available Doctors\n";
        cout << "4. Search Doctor\n";
        cout << "5. Back to Main Menu\n";
        choice = getIntInput("Enter your choice: ");

        if (choice == 1) {
            Doctor d;
            while (true) {
                d.id = getIntInput("Enter Doctor ID (Numeric): ");
                if (doctorExists(d.id)) {
                    cout << "Doctor ID already exists! Please enter a different ID.\n";
                } else {
                    break;
                }
            }

            d.name = getNonEmptyString("Enter Doctor Name: ");
            d.department = getNonEmptyString("Enter Department: ");
            d.specialization = getNonEmptyString("Enter Specialization: ");

            while (true) {
                cout << "Enter Status (Available/Not Available): ";
                getline(cin, d.status);
                if (d.status == "Available" || d.status == "Not Available") break;
                cout << "Please enter 'Available' or 'Not Available'.\n";
            }

            doctors.push_back(d);
            saveDoctors();
            cout << "Doctor registered successfully!\n";
            pauseScreen();

        } else if (choice == 2) {
            cout << "\n=============== ALL DOCTORS ===============\n";
            if (doctors.empty()) {
                cout << "No doctors registered.\n";
            } else {
                for (const auto& d : doctors) {
                    cout << "ID: " << d.id << " | Dr. " << d.name
                         << " | Dept: " << d.department
                         << " | Spec: " << d.specialization
                         << " | Status: " << d.status << "\n";
                }
            }
            pauseScreen();

        } else if (choice == 3) {
            cout << "\n========== AVAILABLE DOCTORS (ON DUTY) ==========\n";
            bool found = false;
            for (const auto& d : doctors) {
                if (d.status == "Available") {
                    cout << "ID: " << d.id << " | Dr. " << d.name
                         << " | Dept: " << d.department
                         << " | Spec: " << d.specialization << "\n";
                    found = true;
                }
            }
            if (!found) cout << "No doctors are currently available.\n";
            pauseScreen();

        } else if (choice == 4) {
            int id = getIntInput("Enter Doctor ID to search: ");
            bool found = false;
            for (const auto& d : doctors) {
                if (d.id == id) {
                    cout << "\nDoctor Found:";
                    cout << "\nID            : " << d.id;
                    cout << "\nName          : Dr. " << d.name;
                    cout << "\nDepartment    : " << d.department;
                    cout << "\nSpecialization: " << d.specialization;
                    cout << "\nStatus        : " << d.status << "\n";
                    found = true;
                    break;
                }
            }
            if (!found) cout << "Doctor not found.\n";
            pauseScreen();
        }
    } while (choice != 5);
}

// ============================================================
// 5. BILLING & PAYMENTS MODULE
// ============================================================

void billingManagement() {
    int choice;
    do {
        cout << "\n========== BILLING & PAYMENTS ==========\n";
        cout << "1. Add Bill\n";
        cout << "2. View Bills\n";
        cout << "3. Search Bill\n";
        cout << "4. Edit Bill\n";
        cout << "5. Mark Bill as Paid\n";
        cout << "6. Delete Bill\n";
        cout << "7. Back to Main Menu\n";
        choice = getIntInput("Enter your choice: ");

        if (choice == 1) {
            Bill b;
            while (true) {
                b.id = getIntInput("Enter Bill ID (e.g., 101, 102): ");
                if (billExists(b.id)) {
                    cout << "Bill ID already exists. Please enter a different ID.\n";
                } else {
                    break;
                }
            }

            while (true) {
                b.patientId = getLongInput("Enter Patient ID: ");
                if (patientExists(b.patientId)) break;
                cout << "Patient not found! Please register the patient first.\n";
            }

            b.service = getNonEmptyString("Enter Service Rendered: ");
            b.amount = getDoubleInput("Enter Bill Amount (e.g. 600.00): ");
            b.date = getCurrentDate();
            b.status = "Pending";

            bills.push_back(b);
            saveBills();
            cout << "Bill created successfully with status 'Pending'!\n";
            pauseScreen();

        } else if (choice == 2) {
            cout << "\n================ ALL BILLS ================\n";
            if (bills.empty()) {
                cout << "No bills recorded.\n";
            } else {
                for (const auto& b : bills) {
                    cout << "\nBill ID   : " << b.id;
                    cout << "\nPatient   : " << getPatientName(b.patientId) << " (ID: " << b.patientId << ")";
                    cout << "\nService   : " << b.service;
                    cout << "\nAmount    : Rs. " << fixed << setprecision(2) << b.amount;
                    cout << "\nDate      : " << b.date;
                    cout << "\nStatus    : " << b.status;
                    cout << "\n-------------------------------------------";
                }
                cout << "\n";
            }
            pauseScreen();

        } else if (choice == 3) {
            int id = getIntInput("Enter Bill ID to search: ");
            bool found = false;
            for (const auto& b : bills) {
                if (b.id == id) {
                    cout << "\nBill Found:";
                    cout << "\nBill ID   : " << b.id;
                    cout << "\nPatient   : " << getPatientName(b.patientId) << " (ID: " << b.patientId << ")";
                    cout << "\nService   : " << b.service;
                    cout << "\nAmount    : Rs. " << fixed << setprecision(2) << b.amount;
                    cout << "\nDate      : " << b.date;
                    cout << "\nStatus    : " << b.status << "\n";
                    found = true;
                    break;
                }
            }
            if (!found) cout << "Bill not found.\n";
            pauseScreen();

        } else if (choice == 4) {
            int id = getIntInput("Enter Bill ID to edit: ");
            bool found = false;
            for (auto& b : bills) {
                if (b.id == id) {
                    b.service = getNonEmptyString("Enter New Service Name: ");
                    b.amount = getDoubleInput("Enter New Amount: ");
                    saveBills();
                    cout << "Bill updated successfully!\n";
                    found = true;
                    break;
                }
            }
            if (!found) cout << "Bill not found.\n";
            pauseScreen();

        } else if (choice == 5) {
            int id = getIntInput("Enter Bill ID to mark as Paid: ");
            bool found = false;
            for (auto& b : bills) {
                if (b.id == id) {
                    b.status = "Paid";
                    saveBills();
                    cout << "Bill ID " << id << " marked as Paid successfully!\n";
                    found = true;
                    break;
                }
            }
            if (!found) cout << "Bill not found.\n";
            pauseScreen();

        } else if (choice == 6) {
            int id = getIntInput("Enter Bill ID to delete: ");
            bool found = false;
            for (auto it = bills.begin(); it != bills.end(); ++it) {
                if (it->id == id) {
                    bills.erase(it);
                    saveBills();
                    cout << "Bill deleted successfully!\n";
                    found = true;
                    break;
                }
            }
            if (!found) cout << "Bill not found.\n";
            pauseScreen();
        }
    } while (choice != 7);
}

// ============================================================
// 6. MEDICINE & INVENTORY MODULE
// ============================================================

void inventoryManagement() {
    int choice;
    do {
        cout << "\n========== MEDICINE & INVENTORY ==========\n";
        cout << "1. Add Medicine\n";
        cout << "2. View Medicines\n";
        cout << "3. Update Stock\n";
        cout << "4. Search Medicine\n";
        cout << "5. Back to Main Menu\n";
        choice = getIntInput("Enter your choice: ");

        if (choice == 1) {
            Medicine m;
            while (true) {
                m.id = getIntInput("Enter Medicine ID (Numeric): ");
                if (medicineExists(m.id)) {
                    cout << "Medicine ID already exists! Please enter a unique ID.\n";
                } else {
                    break;
                }
            }

            m.name = getNonEmptyString("Enter Medicine Name: ");
            m.category = getNonEmptyString("Enter Category: ");

            while (true) {
                m.stock = getIntInput("Enter Initial Stock Quantity: ");
                if (m.stock >= 0) break;
                cout << "Stock cannot be negative.\n";
            }

            m.expiry = getNonEmptyString("Enter Expiry Date (MM/YYYY or DD/MM/YYYY): ");
            m.status = (m.stock > 0) ? "Available" : "Out";

            medicines.push_back(m);
            saveMedicines();
            cout << "Medicine added successfully!\n";
            pauseScreen();

        } else if (choice == 2) {
            cout << "\n=============== PHARMACY INVENTORY ===============\n";
            if (medicines.empty()) {
                cout << "No medicines recorded.\n";
            } else {
                for (const auto& m : medicines) {
                    cout << "ID: " << m.id << " | Name: " << m.name
                         << " | Cat: " << m.category
                         << " | Stock: " << m.stock
                         << " | Expiry: " << m.expiry
                         << " | Status: " << m.status << "\n";
                }
            }
            pauseScreen();

        } else if (choice == 3) {
            int id = getIntInput("Enter Medicine ID to update stock: ");
            bool found = false;
            for (auto& m : medicines) {
                if (m.id == id) {
                    cout << "\n1. Add Stock\n2. Replace Stock\n3. Back\n";
                    int opt = getIntInput("Select option: ");
                    if (opt == 1) {
                        int addQty = getIntInput("Enter quantity to add: ");
                        if (addQty >= 0) {
                            m.stock += addQty;
                            m.status = (m.stock > 0) ? "Available" : "Out";
                            saveMedicines();
                            cout << "Stock updated! New Stock: " << m.stock << "\n";
                        } else {
                            cout << "Quantity cannot be negative.\n";
                        }
                    } else if (opt == 2) {
                        int repQty = getIntInput("Enter replacement quantity: ");
                        if (repQty >= 0) {
                            m.stock = repQty;
                            m.status = (m.stock > 0) ? "Available" : "Out";
                            saveMedicines();
                            cout << "Stock replaced! New Stock: " << m.stock << "\n";
                        } else {
                            cout << "Stock cannot be negative.\n";
                        }
                    }
                    found = true;
                    break;
                }
            }
            if (!found) cout << "Medicine ID not found.\n";
            pauseScreen();

        } else if (choice == 4) {
            string name = getNonEmptyString("Enter Medicine Name to search: ");
            bool found = false;
            for (const auto& m : medicines) {
                if (m.name == name) {
                    cout << "\nMedicine Found:";
                    cout << "\nID       : " << m.id;
                    cout << "\nName     : " << m.name;
                    cout << "\nCategory : " << m.category;
                    cout << "\nStock    : " << m.stock;
                    cout << "\nExpiry   : " << m.expiry;
                    cout << "\nStatus   : " << m.status << "\n";
                    found = true;
                }
            }
            if (!found) cout << "Medicine not found.\n";
            pauseScreen();
        }
    } while (choice != 5);
}

// ============================================================
// 7. SEARCH PATIENT MODULE (ID, NAME, PHONE)
// ============================================================

void searchPatientModule() {
    int opt;
    cout << "\n========== SEARCH PATIENT ==========\n";
    cout << "1. Search by Patient ID\n";
    cout << "2. Search by Patient Name\n";
    cout << "3. Search by Phone Number\n";
    opt = getIntInput("Enter choice: ");

    bool found = false;
    if (opt == 1) {
        long long id = getLongInput("Enter Patient ID: ");
        for (const auto& p : patients) {
            if (p.id == id) {
                cout << "\n[Patient Found]\n";
                cout << "ID          : " << p.id << "\n";
                cout << "Name        : " << p.name << "\n";
                cout << "Age         : " << p.age << "\n";
                cout << "Gender      : " << p.gender << "\n";
                cout << "Phone       : " << p.phone << "\n";
                cout << "Department  : " << p.department << "\n";
                cout << "Blood Group : " << p.bloodGroup << "\n";
                cout << "Status      : " << p.status << "\n";
                found = true;
                break;
            }
        }
    } else if (opt == 2) {
        string name = getNonEmptyString("Enter Patient Name: ");
        for (const auto& p : patients) {
            if (p.name == name) {
                cout << "\n[Patient Found]\n";
                cout << "ID          : " << p.id << "\n";
                cout << "Name        : " << p.name << "\n";
                cout << "Age         : " << p.age << "\n";
                cout << "Gender      : " << p.gender << "\n";
                cout << "Phone       : " << p.phone << "\n";
                cout << "Department  : " << p.department << "\n";
                cout << "Blood Group : " << p.bloodGroup << "\n";
                cout << "Status      : " << p.status << "\n";
                found = true;
            }
        }
    } else if (opt == 3) {
        string phone;
        cout << "Enter 10-digit Phone Number: ";
        cin >> phone;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        for (const auto& p : patients) {
            if (p.phone == phone) {
                cout << "\n[Patient Found]\n";
                cout << "ID          : " << p.id << "\n";
                cout << "Name        : " << p.name << "\n";
                cout << "Age         : " << p.age << "\n";
                cout << "Gender      : " << p.gender << "\n";
                cout << "Phone       : " << p.phone << "\n";
                cout << "Department  : " << p.department << "\n";
                cout << "Blood Group : " << p.bloodGroup << "\n";
                cout << "Status      : " << p.status << "\n";
                found = true;
            }
        }
    } else {
        cout << "Invalid search option.\n";
    }

    if (!found) cout << "No matching patient records found.\n";
    pauseScreen();
}

// ============================================================
// 8. MAIN ENTRY POINT
// ============================================================

int main() {
    // Automatic Loading on Startup
    loadPatients();
    loadAppointments();
    loadDoctors();
    loadBills();
    loadMedicines();

    int choice;
    do {
        cout << "\n====================================================\n";
        cout << "        HOSPITAL MANAGEMENT SYSTEM (LINUX)          \n";
        cout << "====================================================\n";
        cout << "1. Dashboard\n";
        cout << "2. Patient Management\n";
        cout << "3. Appointment Management\n";
        cout << "4. Doctor Management\n";
        cout << "5. Billing & Payments\n";
        cout << "6. Medicine & Inventory\n";
        cout << "7. Search Patient\n";
        cout << "8. Exit\n";
        cout << "====================================================\n";
        choice = getIntInput("Enter your choice: ");

        switch (choice) {
            case 1: dashboard(); break;
            case 2: patientManagement(); break;
            case 3: appointmentManagement(); break;
            case 4: doctorManagement(); break;
            case 5: billingManagement(); break;
            case 6: inventoryManagement(); break;
            case 7: searchPatientModule(); break;
            case 8:
                cout << "\nThank you for using the Hospital Management System.\n";
                cout << "Saving and shutting down gracefully...\n";
                break;
            default:
                cout << "Invalid choice! Please select an option between 1 and 8.\n";
                pauseScreen();
                break;
        }
    } while (choice != 8);

    return 0;
}