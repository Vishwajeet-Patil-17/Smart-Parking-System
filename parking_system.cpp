#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// ==========================================
// 1. VEHICLE CLASS HIERARCHY (OOP)
// ==========================================
class Vehicle {
protected:
    string vehicleNumber;
    int entryTime; // in minutes since midnight

public:
    Vehicle(string num, int entry) : vehicleNumber(num), entryTime(entry) {}
    virtual ~Vehicle() {}

    string getVehicleNumber() const { return vehicleNumber; }
    int getEntryTime() const { return entryTime; }

    // Pure virtual functions (Polymorphism)
    virtual string getType() const = 0;
    virtual double getHourlyRate() const = 0;
};

class Bike : public Vehicle {
public:
    Bike(string num, int entry) : Vehicle(num, entry) {}
    string getType() const override { return "Bike"; }
    double getHourlyRate() const override { return 10.0; }
};

class Car : public Vehicle {
public:
    Car(string num, int entry) : Vehicle(num, entry) {}
    string getType() const override { return "Car"; }
    double getHourlyRate() const override { return 20.0; }
};

class Truck : public Vehicle {
public:
    Truck(string num, int entry) : Vehicle(num, entry) {}
    string getType() const override { return "Truck"; }
    double getHourlyRate() const override { return 30.0; }
};

// ==========================================
// 2. FACTORY PATTERN (Abstraction)
// ==========================================
class VehicleFactory {
public:
    static Vehicle* createVehicle(string type, string num, int entry) {
        if (type == "Bike") {
            return new Bike(num, entry);
        } else if (type == "Car") {
            return new Car(num, entry);
        } else if (type == "Truck") {
            return new Truck(num, entry);
        }
        return nullptr;
    }
};

// ==========================================
// 3. DATABASE MANAGER CLASS (Encapsulation)
// ==========================================
class ParkingDatabase {
private:
    string dbFilename;

public:
    ParkingDatabase(string dbFile = "data.txt") : dbFilename(dbFile) {}

    bool isVehicleParked(string targetNum) {
        ifstream file(dbFilename);
        if (!file.is_open()) return false;

        string num, type;
        int entry;
        while (file >> num >> type >> entry) {
            if (num == targetNum) {
                file.close();
                return true;
            }
        }
        file.close();
        return false;
    }

    void parkVehicle(const Vehicle& vehicle) {
        ofstream file(dbFilename, ios::app);
        if (file.is_open()) {
            file << vehicle.getVehicleNumber() << " " 
                 << vehicle.getType() << " " 
                 << vehicle.getEntryTime() << "\n";
            file.close();
        }
    }

    bool removeAndBill(string targetNum, int exitTime) {
        ifstream file(dbFilename);
        ofstream temp("temp.txt");
        if (!file.is_open() || !temp.is_open()) {
            return false;
        }

        string num, type;
        int entryTime;
        bool found = false;

        while (file >> num >> type >> entryTime) {
            if (num == targetNum && !found) {
                found = true;

                // Create vehicle object dynamically using Factory
                Vehicle* vehicle = VehicleFactory::createVehicle(type, num, entryTime);
                if (vehicle != nullptr) {
                    // Handle past-midnight exit
                    int duration = exitTime - entryTime;
                    if (duration < 0) {
                        duration += 1440; 
                    }

                    // Round up to nearest hour
                    int hours = (duration + 59) / 60;
                    if (hours == 0) hours = 1; 

                    double fee = hours * vehicle->getHourlyRate();

                    // Print Receipt
                    cout << "\n+---------------------------------------+\n";
                    cout << "|            BILLING DETAILS            |\n";
                    cout << "+---------------------------------------+\n";
                    cout << "  Vehicle No. : " << vehicle->getVehicleNumber() << "\n";
                    cout << "  Type        : " << vehicle->getType() << "\n";
                    cout << "  Duration    : " << hours << " hours\n";
                    cout << "  Total Fee   : Rs. " << fee << "\n";
                    cout << "+---------------------------------------+\n";

                    delete vehicle; // Clean up memory
                }
            } else {
                // Keep unmatched records
                temp << num << " " << type << " " << entryTime << "\n";
            }
        }

        file.close();
        temp.close();

        if (found) {
            remove(dbFilename.c_str());
            rename("temp.txt", dbFilename.c_str());
        } else {
            remove("temp.txt");
        }

        return found;
    }
};

// ==========================================
// 4. UTILITIES & MAIN DRIVER
// ==========================================
// Convert time to minutes
int toMinutes(int h, int m, string p) 
{
    if (p == "PM" && h != 12) 
        h += 12;
    if (p == "AM" && h == 12) 
        h = 0;
    return (h * 60) + m;
}

int main() 
{
    ParkingDatabase db;
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
            if (db.isVehicleParked(num)) 
            {
                cout << "[!] ERROR: Vehicle " << num << " is already parked.\n";
                continue;
            }

            // Validate vehicle type
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

            // Create vehicle object dynamically using Factory
            Vehicle* vehicle = VehicleFactory::createVehicle(type, num, entryMins);
            if (vehicle != nullptr) {
                db.parkVehicle(*vehicle);
                cout << "[+] SUCCESS: Vehicle parked.\n";
                delete vehicle; // Clean up memory
            } else {
                cout << "[-] ERROR: Failed to create vehicle object.\n";
            }
        } 
        else if (choice == 2) 
        {
            string num, period;
            int h, m;
            cout << "\n--- REMOVE VEHICLE ---\n";
            cout << "Vehicle Number                  : ";
            cin >> num;
            if (!db.isVehicleParked(num)) 
            {
                cout << "[-] ERROR: Vehicle " << num << " is not in the lot.\n";
                continue;
            }
            cout << "Exit Time (HH MM AM/PM)         : ";
            cin >> h >> m >> period;
            int exitMins = toMinutes(h, m, period);

            if (db.removeAndBill(num, exitMins)) {
                cout << "\n[+] SUCCESS: Vehicle removed and record updated.\n";
            } else {
                cout << "\n[-] ERROR: Vehicle not found in the system.\n";
            }
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