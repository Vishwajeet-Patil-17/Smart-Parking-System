Smart Parking System (C++)

A command-line based parking management system developed in C++ that simulates real-world parking operations. It allows users to park vehicles, remove them, and generate bills based on parking duration and vehicle type.

Users can add vehicles by entering vehicle number, type (Bike, Car, Truck), and entry time
All parking records are stored in a file (data.txt) for persistence
On exit, the system calculates total parking time, including cases where parking crosses midnight
Billing is done on an hourly basis with rounding and a minimum charge applied
Prevents duplicate vehicle entries and ensures only valid operations are performed
Generates a clear and structured bill with duration and total fee

This project showcases core C++ concepts such as file handling, time conversion, conditional logic, loops.
