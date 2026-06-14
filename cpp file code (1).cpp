#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <sstream>
#include <ctime>
#include <cmath>    // for ceil
using namespace std;

const int MAX_PARK = 50;

struct Vehicle {
    string number;
    string type;
    int entryHour, entryMinute;
    int exitHour, exitMinute;

    Vehicle() {
        exitHour = -1;
        exitMinute = -1;
    }
};

Vehicle groundFloor[MAX_PARK];
Vehicle firstFloor[MAX_PARK];
Vehicle secondFloor[MAX_PARK];

int busCount = 0, rickshawCount = 0, carCount = 0;

string toLower(string str) {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

void parseTime(const string& timeStr, int& hour, int& minute) {
    sscanf(timeStr.c_str(), "%d:%d", &hour, &minute);
}

void checkIn() {
    Vehicle* v = new Vehicle;

    cout << "Enter vehicle number to check in : ";
    cin >> v->number;

    cout << "Enter vehicle type (Rickshaw/Car/Bus): ";
    cin >> v->type;
    string type = toLower(v->type);

    string timeStr;
    bool validTime = false;

    while (!validTime) {
        cout << "Enter entry time (HH:MM): ";
        cin >> timeStr;
        parseTime(timeStr, v->entryHour, v->entryMinute);

        if (v->entryHour >= 0 && v->entryHour < 24 &&
            v->entryMinute >= 0 && v->entryMinute < 60) {
            validTime = true;
        } else {
            cout << "Invalid time entered. Please enter time in HH:MM (24-hour) format.\n";
        }
    }

    if (type == "bus") {
        if (busCount >= MAX_PARK) {
            cout << "Bus parking full on Ground Floor!\n";
            delete v;
            return;
        }
        groundFloor[busCount++] = *v;
    }
    else if (type == "rickshaw") {
        if (rickshawCount >= MAX_PARK) {
            cout << "Rickshaw parking full on First Floor!\n";
            delete v;
            return;
        }
        firstFloor[rickshawCount++] = *v;
    }
    else if (type == "car") {
        if (carCount >= MAX_PARK) {
            cout << "Car parking full on Second Floor!\n";
            delete v;
            return;
        }
        secondFloor[carCount++] = *v;
    }
    else {
        cout << "Invalid vehicle type!\n";
        delete v;
        return;
    }

    ofstream fout("parking_log.txt", ios::app);
    fout << v->number << " " << v->type << " "
         << v->entryHour << ":" << (v->entryMinute < 10 ? "0" : "") << v->entryMinute << " -1:-1\n";
    fout.close();

    delete v;
    cout << "Vehicle checked in (parked) successfully.\n";
}

void checkOut(Vehicle floor[], int& count, int rate, string number, int exitHour, int exitMinute, bool& found) 
{
    for (int i = 0; i < count; i++) {
        if (floor[i].number == number && floor[i].exitHour == -1) {
            floor[i].exitHour = exitHour;
            floor[i].exitMinute = exitMinute;

            int entryTotal = floor[i].entryHour * 60 + floor[i].entryMinute;
            int exitTotal = exitHour * 60 + exitMinute;
            int duration = exitTotal - entryTotal;

            if (duration < 1) duration = 1;

            float hours = duration / 60.0;
            int fee = ceil(hours * rate);

            cout << "Vehicle " << number << " exited at "
                 << exitHour << ":" << (exitMinute < 10 ? "0" : "") << exitMinute
                 << ". Fee: Rs. " << fee << endl;

            // Log to file
            ofstream fout("parking_log.txt", ios::app);
            fout << number << " " << floor[i].type << " "
                 << floor[i].entryHour << ":" << (floor[i].entryMinute < 10 ? "0" : "") << floor[i].entryMinute
                 << " " << exitHour << ":" << (exitMinute < 10 ? "0" : "") << exitMinute << "\n";
            fout.close();

            found = true;
            break;
        }
    }
}

void exitVehicle() {
    string number;

    cout << "Enter vehicle number to checkout: ";
    cin >> number;

    // Get current system time
    time_t now = time(0);
    tm* localTime = localtime(&now);
    int exitHour = localTime->tm_hour;
    int exitMinute = localTime->tm_min;

    bool found = false;
    checkOut(groundFloor, busCount, 60, number, exitHour, exitMinute, found);
    checkOut(firstFloor, rickshawCount, 20, number, exitHour, exitMinute, found);
    checkOut(secondFloor, carCount, 40, number, exitHour, exitMinute, found);

    if (!found) {
        cout << "Vehicle not found or already checked out.\n";
    }
}

void showStatus() {
    cout << "\n===== Parking Status =====\n";

    cout << "\n-- Ground Floor (Bus) --\n";
    for (int i = 0; i < busCount; i++) {
        cout << groundFloor[i].number << " at "
             << groundFloor[i].entryHour << ":"
             << (groundFloor[i].entryMinute < 10 ? "0" : "")
             << groundFloor[i].entryMinute << "\n";
    }

    cout << "\n-- First Floor (Rickshaw) --\n";
    for (int i = 0; i < rickshawCount; i++) {
        cout << firstFloor[i].number << " at "
             << firstFloor[i].entryHour << ":"
             << (firstFloor[i].entryMinute < 10 ? "0" : "")
             << firstFloor[i].entryMinute << "\n";
    }

    cout << "\n-- Second Floor (Car) --\n";
    for (int i = 0; i < carCount; i++) {
        cout << secondFloor[i].number << " at "
             << secondFloor[i].entryHour << ":"
             << (secondFloor[i].entryMinute < 10 ? "0" : "")
             << secondFloor[i].entryMinute << "\n";
    }
}

int main() {
    int choice;

    do {
        cout << "\n===== Car Parking System =====\n";
        cout << "1. Park Vehicle\n";
        cout << "2. Exit Vehicle\n";
        cout << "3. Show Parking Status\n";
        cout << "4. Exit Program\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                checkIn();
                break;
            case 2:
                exitVehicle();
                break;
            case 3:
                showStatus();
                break;
            case 4:
                cout << "Thank you! Exiting...\n";
                exit(0);
            default:
                cout << "Invalid choice!\n";
        }
    } while (true);

    return 0;
}
