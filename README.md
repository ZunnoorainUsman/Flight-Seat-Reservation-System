# ✈️ Flight Seat Reservation System (C++)

A console-based **Flight Seat Reservation System** developed in **C++** that demonstrates the use of:

* Static & Dynamic Arrays
* Dynamic Memory Allocation (`new` / `delete`)
* Pointer Arithmetic
* Double Pointers (`char**`)
* File Handling (`ifstream`, `ofstream`)
* String Manipulation using C-Strings
* Searching Algorithms
* Data Persistence
* Manual Memory Management

This project simulates a small airline reservation system where users can book seats, cancel reservations, view passenger manifests, search passengers, and export flight reports.

---

## 📌 Features

### 🎟️ Booking Management

* Book seats on available flights
* Automatic seat assignment
* Payment status tracking
* Dynamic booking storage
* Booking confirmation summary

### ❌ Cancellation System

* Cancel bookings using Passenger ID
* Automatically frees reserved seats
* Updates booking records instantly
* Audit logging for cancellations

### 💺 Seat Map Visualization

* Real-time seat availability display
* Shows booked and free seats
* Flight occupancy statistics

### 👤 Passenger Search

Search passengers by:

* Passenger Name
* Passenger ID

Supports partial name matching.

### 📋 Passenger Manifest

View complete passenger lists including:

* Passenger ID
* Name
* Seat Number
* Seat Class
* Payment Status

### 📤 Export Reports

Generate flight manifest files automatically:

```txt
FL01_manifest.txt
FL02_manifest.txt
...
```

### 📝 Audit Logging

Every booking and cancellation is logged into:

```txt
audit.txt
```

Example:

```txt
[LOG] BOOK P101 FL01 Seat-5
[LOG] CANCEL P101 Seat-5
```

---

# 🏗️ System Architecture

The project is divided into six major modules:

| Module | Responsibility          |
| ------ | ----------------------- |
| M1     | Flight & Booking Loader |
| M2     | Seat Map Management     |
| M3     | Booking Engine          |
| M4     | Search Functions        |
| M5     | Manifest & Export       |
| M6     | Menu & Utilities        |

---

# 📂 Data Structures Used

## Static 2D Arrays

### Flight Information

```cpp
char flightCode[NFLIGHTS][10];
char fromCity[NFLIGHTS][30];
char toCity[NFLIGHTS][30];
```

Stores flight routes and codes.

---

## Static 1D Arrays

```cpp
int capacity[NFLIGHTS];
```

Stores flight capacities.

---

## Dynamic 1D Array

```cpp
int* priceCents;
```

Allocated at runtime for flight ticket prices.

---

## Static 2D Seat Map

```cpp
int seatMap[NFLIGHTS][MAXSEATS];
```

Where:

```cpp
0 = Free
1 = Booked
```

---

## Dynamic Double Pointer

```cpp
char** bookings;
```

Stores booking records dynamically.

Example record:

```txt
FL01|P001|Ali Hassan|12|Economy|1
```

---

# 🔄 Dynamic Memory Management

### Allocation

```cpp
priceCents = new int[NFLIGHTS];
bookings = new char*[bCap];
```

### Deep Copy Storage

```cpp
bookings[bCount] = new char[len + 1];
```

### Automatic Growth

```cpp
growBookings();
```

Doubles booking capacity when full.

### Automatic Shrinking

```cpp
shrinkBookings();
```

Reduces memory usage after cancellations.

### Memory Cleanup

```cpp
freeAll();
```

Prevents memory leaks and dangling pointers.

---

# 📁 File Structure

```txt
Project/
│
├── main.cpp
├── flights.txt
├── bookings.txt
├── audit.txt
│
├── FL01_manifest.txt
├── FL02_manifest.txt
└── ...
```

---

# 📄 Input Files

## flights.txt

Example:

```txt
FL01 Lahore Karachi 60 250000
FL02 Karachi Islamabad 50 180000
FL03 Lahore Multan 40 120000
FL04 Islamabad Quetta 30 220000
FL05 Karachi Peshawar 45 170000
```

Format:

```txt
FlightCode DepartureCity ArrivalCity Capacity PriceInCents
```

---

## bookings.txt

Example:

```txt
FL01|P001|Ali Hassan|12|Economy|1
FL01|P002|Sara Khan|14|Business|0
```

Format:

```txt
CODE|PID|NAME|SEAT|CLASS|PAID
```

---

# 📊 Menu Options

```txt
[1] Book a Seat
[2] Cancel Booking
[3] View Seat Map
[4] View Manifest
[5] Search by Name
[6] Search by ID
[7] Export Manifest
[0] Exit
```

---

# 🔍 Searching Implementation

### Search by Name

Uses manual substring matching:

```cpp
for (int s = 0; s <= nLen - tLen; s++)
```

Supports partial searches such as:

```txt
Ali
Sara
Hassan
```

---

### Search by ID

Uses:

```cpp
strcmp()
```

for exact Passenger ID matching.

---

# 💰 Revenue Calculation

Revenue is calculated using integer arithmetic:

```cpp
revenue += priceCents[fi];
```

Display:

```cpp
Revenue Collected: Rs.2500.00
```

No floating-point operations are used.

---

# 🧠 Concepts Demonstrated

This project demonstrates:

✅ Static Arrays
✅ Dynamic Arrays
✅ Double Pointers
✅ Dynamic Memory Allocation
✅ Deep Copying
✅ Pointer Arithmetic
✅ File Handling
✅ String Parsing
✅ Data Persistence
✅ Search Algorithms
✅ Menu-Driven Programming
✅ Memory Leak Prevention
✅ Audit Logging

---

# 🚀 How to Compile & Run

### Using g++

```bash
g++ main.cpp -o FlightReservation
```

Run:

```bash
./FlightReservation
```

### Using Visual Studio

1. Create a new C++ Console Project
2. Add `main.cpp`
3. Place `flights.txt` in the project directory
4. Build and Run

---

# 📸 Sample Output

```txt
=======================================================
      FLIGHT SEAT RESERVATION SYSTEM v1.0
=======================================================

Flights : 5
Total Seats : 225
Booked : 32
Free : 193

[1] Book a Seat
[2] Cancel Booking
[3] View Seat Map
[4] View Manifest
[5] Search by Name
[6] Search by ID
[7] Export Manifest
[0] Exit
```

---

# 🎯 Learning Objectives

This project is ideal for students learning:

* Data Structures
* Dynamic Memory Management
* File Handling
* C-Strings
* Pointer Manipulation
* Console-Based System Design
* Software Engineering Fundamentals

---

# 📜 License

This project is open-source and available for educational and academic purposes.
