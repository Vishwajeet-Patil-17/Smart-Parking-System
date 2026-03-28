#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// convert time to minutes
int toMinutes(int h, int m, string p) 
{
    if (p == "PM" && h != 12) 
        h += 12;
    if (p == "AM" && h == 12) 
        h = 0;
    return (h * 60) + m;
}

// calculate parking fee
double getFee(string type, int hours) 
{
    if (type == "Bike") 
        return hours * 10.0;
    if (type == "Car") 
        return hours * 20.0;
    if (type == "Truck") 
        return hours * 30.0;
    return 0.0;
}

// check if vehicle exists
bool isParked(string targetNum) 
{
    ifstream file("data.txt");
    string num, type;
    int entry;
    while (file >> num >> type >> entry) 
    {
        if (num == targetNum) return true;
    }
    return false;
}

// file operations
void parkVehicle(string num, string type, int entry) 
{
    ofstream file("data.txt", ios::app);
    file << num << " " << type << " " << entry << "\n";
}

void removeAndBill(string targetNum, int exitTime) 
{
    ifstream file("data.txt");
    ofstream temp("temp.txt");
    string num, type;
    int entryTime;
    bool found = false;
    while (file >> num >> type >> entryTime) 
    {
        if (num == targetNum && !found) 
        {
            found = true;

            // handle past-midnight exit
            int duration = exitTime - entryTime;
            if (duration < 0) 
                duration += 1440; 

            // round up to nearest hour
            int hours = (duration + 59) / 60;
            if (hours == 0) hours = 1; 
            double fee = getFee(type, hours);
            cout << "\n+---------------------------------------+\n";
            cout << "|            BILLING DETAILS            |\n";
            cout << "+---------------------------------------+\n";
            cout << "  Vehicle No. : " << num << "\n";
            cout << "  Type        : " << type << "\n";
            cout << "  Duration    : " << hours << " hours\n";
            cout << "  Total Fee   : Rs. " << fee << "\n";
            cout << "+---------------------------------------+\n";
        } 
        else 
        {
            // keep unmatched records
            temp << num << " " << type << " " << entryTime << "\n";
        }
    }
    file.close();
    temp.close();
    if (found) 
    {
        remove("data.txt");
        rename("temp.txt", "data.txt");
        cout << "\n[+] SUCCESS: Vehicle removed and record updated.\n";
    } 
    else 
    {
        remove("temp.txt");
        cout << "\n[-] ERROR: Vehicle not found in the system.\n";
    }
}

// main interface
int main() 
{
    int choice;
    while (true) 
    {
        cout << "\n=========================================\n";
        cout << "          SMART PARKING SYSTEM           \n";
        cout << "=========================================\n";
        cout << "  [1] Park a Vehicle\n";
        cout << "  [2] Remove Vehicle & Generate Bill\n";
        cout << "  [3] Exit System\n";
        cout << "-----------------------------------------\n";
        cout << "  Select an option (1-3): ";
        if (!(cin >> choice)) 
        {
            cout << "[!] Invalid input. Numbers only.\n";
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        if (choice == 1) 
        {
            string num, type, period;
            int h, m;
            cout << "\n--- PARK VEHICLE ---\n";
            cout << "Vehicle Number                  : ";
            cin >> num;
            if (isParked(num)) 
            {
                cout << "[!] ERROR: Vehicle " << num << " is already parked.\n";
                continue;
            }

            // validate vehicle type
            while (true) 
            {
                cout << "Type (Bike / Car / Truck)       : ";
                cin >> type;
                if (type == "Bike" || type == "Car" || type == "Truck") break;
                cout << "[!] Invalid type. Please exactly type Bike, Car, or Truck.\n";
            }
            cout << "Entry Time (HH MM AM/PM)        : ";
            cin >> h >> m >> period;
            int entryMins = toMinutes(h, m, period);
            parkVehicle(num, type, entryMins);
            cout << "[+] SUCCESS: Vehicle parked.\n";
        } 
        else if (choice == 2) 
        {
            string num, period;
            int h, m;
            cout << "\n--- REMOVE VEHICLE ---\n";
            cout << "Vehicle Number                  : ";
            cin >> num;
            if (!isParked(num)) 
            {
                cout << "[-] ERROR: Vehicle " << num << " is not in the lot.\n";
                continue;
            }
            cout << "Exit Time (HH MM AM/PM)         : ";
            cin >> h >> m >> period;
            int exitMins = toMinutes(h, m, period);
            removeAndBill(num, exitMins);

        } 
        else if (choice == 3) 
        {
            cout << "\nShutting down...\n";
            break;
        } 
        else 
        {
            cout << "\n[!] ERROR: Please select 1, 2, or 3.\n";
        }
    }
    return 0;
}