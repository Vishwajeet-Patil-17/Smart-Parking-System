#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// Base class 
class Vehicle 
{
    protected:
        string number;
        int entryTime; // Entry time in minutes since midnight

    public:
        Vehicle(string num, int entry) 
        {
            number = num;
            entryTime = entry;
        }

        virtual ~Vehicle() {}

        string getNumber() 
        {
            return number;
        }

        int getEntryTime() 
        {
            return entryTime;
        }

        // Virtual functions 
        virtual string getType() 
        {
            return "Unknown";
        }

        virtual double getHourlyRate() 
        {
            return 0.0;
        }
};

// Child classes
class Bike : public Vehicle 
{
    public:
        Bike(string num, int entry) : Vehicle(num, entry) {}

        string getType() override 
        {
            return "Bike";
        }

        double getHourlyRate() override 
        {
            return 10.0;
        }
};

class Car : public Vehicle 
{
    public:
        Car(string num, int entry) : Vehicle(num, entry) {}

        string getType() override 
        {
            return "Car";
        }

        double getHourlyRate() override 
        {
            return 20.0;
        }
};

class Truck : public Vehicle 
{
    public:
        Truck(string num, int entry) : Vehicle(num, entry) {}

        string getType() override 
        {
            return "Truck";
        }

        double getHourlyRate() override 
        {
            return 30.0;
        }
};

// Manage files
class ParkingSystem 
{
    private:
        string dbFilename;

        // Convert time to minutes
        int toMinutes(int h, int m, string p) 
        {
            if (p == "PM" && h != 12) 
            {
                h += 12;
            }
            if (p == "AM" && h == 12) 
            {
                h = 0;
            }
            return (h * 60) + m;
        }

    public:
        ParkingSystem(string dbFile = "data.txt") 
        {
            dbFilename = dbFile;
        }

        // Check if vehicle exists in the database
        bool isParked(string targetNum) 
        {
            ifstream file(dbFilename);
            if (!file.is_open()) 
            {
                return false;
            }

            string num, type;
            int entry;
            while (file >> num >> type >> entry) 
            {
                if (num == targetNum) 
                {
                    file.close();
                    return true;
                }
            }
            file.close();
            return false;
        }

        // File operations to save a parked vehicle
        void parkVehicle(Vehicle* v) 
        {
            ofstream file(dbFilename, ios::app);
            if (file.is_open()) 
            {
                file << v->getNumber() << " " << v->getType() << " " << v->getEntryTime() << "\n";
                file.close();
            }
        }

        // Remove vehicle and generate billing
        void removeAndBill(string targetNum, int exitTime) 
        {
            ifstream file(dbFilename);
            ofstream temp("temp.txt");
            if (!file.is_open() || !temp.is_open()) 
            {
                return;
            }

            string num, type;
            int entryTime;
            bool found = false;

            while (file >> num >> type >> entryTime) 
            {
                if (num == targetNum && !found) 
                {
                    found = true;

                    // Create vehicle object based on type string
                    Vehicle* v = nullptr;
                    if (type == "Bike") 
                    {
                        v = new Bike(num, entryTime);
                    }
                    else if (type == "Car") 
                    {
                        v = new Car(num, entryTime);
                    }
                    else if (type == "Truck") 
                    {
                        v = new Truck(num, entryTime);
                    }

                    if (v != nullptr) 
                    {
                        // Handle past midnight exit
                        int duration = exitTime - entryTime;
                        if (duration < 0) 
                        {
                        duration += 1440; 
                        }

                        // Round up to nearest hour
                        int hours = (duration + 59) / 60;
                        if (hours == 0) 
                        {
                            hours = 1; 
                        }
                    
                        double fee = hours * v->getHourlyRate();
                    
                        cout << "\n+---------------------------------------+\n";
                        cout << "|            BILLING DETAILS            |\n";
                        cout << "+---------------------------------------+\n";
                        cout << "  Vehicle No. : " << v->getNumber() << "\n";
                        cout << "  Type        : " << v->getType() << "\n";
                        cout << "  Duration    : " << hours << " hours\n";
                        cout << "  Total Fee   : Rs. " << fee << "\n";
                        cout << "+---------------------------------------+\n";

                        delete v; // Clean up memory
                    }
                } 
                else 
                {
                    // Keep unmatched records
                    temp << num << " " << type << " " << entryTime << "\n";
                }
            }
            file.close();
            temp.close();

            if (found) 
            {
                remove(dbFilename.c_str());
                rename("temp.txt", dbFilename.c_str());
                cout << "\n[+] SUCCESS: Vehicle removed and record updated.\n";
            } 
            else 
            {
                remove("temp.txt");
                cout << "\n[-] ERROR: Vehicle not found in the system.\n";
            }
        }

        // UI operation to park a vehicle
        void handleParking()
        {
            string num, type, period;
            int h, m;
            cout << "\n--- PARK VEHICLE ---\n";
            cout << "Vehicle Number                  : ";
            cin >> num;
            if (isParked(num)) 
            {
                cout << "[!] ERROR: Vehicle " << num << " is already parked.\n";
                return;
            }

            // Validate vehicle type
            while (true) 
            {
                cout << "Type (Bike / Car / Truck)       : ";
                cin >> type;
                if (type == "Bike" || type == "Car" || type == "Truck") 
                {
                    break;
                }
                cout << "[!] Invalid type. Please exactly type Bike, Car, or Truck.\n";
            }
            cout << "Entry Time (HH MM AM/PM)        : ";
            cin >> h >> m >> period;
            int entryMins = toMinutes(h, m, period);

            // Create specialized vehicle object using inheritance & polymorphism
            Vehicle* v = nullptr;
            if (type == "Bike") 
            {
                v = new Bike(num, entryMins);
            }
            else if (type == "Car") 
            {
                v = new Car(num, entryMins);
            }
            else if (type == "Truck") 
            {
                v = new Truck(num, entryMins);
            }

            if (v != nullptr) 
            {
                parkVehicle(v);
                cout << "[+] SUCCESS: Vehicle parked.\n";
                delete v; // Clean up memory
            }
        }

        // UI operation to checkout a vehicle
        void handleCheckout()
        {
            string num, period;
            int h, m;
            cout << "\n--- REMOVE VEHICLE ---\n";
            cout << "Vehicle Number                  : ";
            cin >> num;
            if (!isParked(num)) 
            {
                cout << "[-] ERROR: Vehicle " << num << " is not in the lot.\n";
                return;
            }
            cout << "Exit Time (HH MM AM/PM)         : ";
            cin >> h >> m >> period;
            int exitMins = toMinutes(h, m, period);
            removeAndBill(num, exitMins);
        }
};

// main     
int main() 
{
    ParkingSystem system;
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
            system.handleParking();
        } 
        else if (choice == 2) 
        {
            system.handleCheckout();
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