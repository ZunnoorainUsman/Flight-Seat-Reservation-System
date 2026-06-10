// ================================================================
// FLIGHT SEAT RESERVATION SYSTEM  
// ================================================================

//L1F25BSCS0627 Zunnoorain Usman
//L1F25BSCS0622 Ibrahim Razi
//L1F25BSCS0624 Hammad Ahmad

#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

const int NFLIGHTS = 5;
const int MAXSEATS = 60;
const int BLEN = 120;
const int INIT_CAP = 8;

//(static 1D / 2D arrays)
char flightCode[NFLIGHTS][10];   // static 2D char - flight codes
char fromCity[NFLIGHTS][30];     // static 2D char - departure city
char toCity[NFLIGHTS][30];       // static 2D char - arrival city
int  capacity[NFLIGHTS];         // static 1D int  - seats per flight

// Dynamic 1D int array - ticket prices in cents (no float)
// Allocated in loadFlights(), freed in freeAll()
int* priceCents = nullptr;

// Static 2D int array - seat map, 0=free 1=booked
int seatMap[NFLIGHTS][MAXSEATS];

// char** double pointer - dynamic 1D array of booking C-strings
char** bookings = nullptr;
int    bCount = 0;    // current booking count
int    bCap = 0;    // current allocated capacity


// FUNCTION PROTOTYPES
// M1 - Loader
void loadFlights(const char* file);
void loadBookings(const char* file);
void saveBookings(const char* file);

// M2 - Seat Map
void displaySeatMap(int fi);
int  findFreeSeat(int fi);
int  seatIsFree(int fi, int seatNum);

// M3 - Booking Engine
void growBookings();
void shrinkBookings();
int  bookSeat(int fi, const char* pid, const char* name, const char* cls, int paid);
int  cancelBooking(const char* pid);

// M4 - Search
void searchByName(const char* name);
void searchByID(const char* pid);

// M5 - Manifest
void parseBooking(const char* line, char* code, char* pid, char* name, char* seat, char* cls, char* paid);
void viewManifest(int fi);
void exportManifest(int fi);

// M6 - Menu & Utilities
void printMenu();
void printLine();
void computeBooked(int fi, int& bookedOut);  // pass by reference
void logAudit(const char* entry);
void freeAll();
int  findFlight(const char* code);
void intToStr(int n, char* buf);

int main()
{
    loadFlights("flights.txt");
    loadBookings("bookings.txt");

    int choice = -1;

    while (choice != 0)
    {
        printMenu();
        cout << "  >>> Option: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1)        // ---- Book a Seat ----
        {
            cout << "\n";
            printLine();
            cout << "  BOOK A SEAT\n";
            printLine();
            cout << "  Available flights:\n";
            for (int fi = 0; fi < NFLIGHTS; fi++)
            {
                cout << "    " << flightCode[fi] << "  " << fromCity[fi] << " -> " << toCity[fi] << "\n";
            }
            printLine();

            char code[10];
            cout << "  Enter Flight Code (e.g. FL01) : ";
            cin.getline(code, 10);

            int fi = findFlight(code);
            if (fi == -1)
            {
                cout << "  [ERR] Flight not found.\n";
            }
            else if (findFreeSeat(fi) == -1)
            {
                cout << "  [INFO] This flight is FULL. No seats available.\n";
            }
            else
            {
                cout << "\n  --- Enter Passenger Details ---\n";
                cout << "  Passenger ID  (e.g. P010)        : ";
                char pid[20];
                cin.getline(pid, 20);

                cout << "  Full Name     (e.g. Ali Hassan)  : ";
                char name[50];
                cin.getline(name, 50);

                cout << "  Seat Class    (Economy/Business) : ";
                char cls[20];
                cin.getline(cls, 20);

                cout << "  Payment       (1 = Paid, 0 = Unpaid) : ";
                int paid = 0;
                cin >> paid;
                cin.ignore();

                int assignedSeat = findFreeSeat(fi) + 1;   // seat that will be assigned

                if (bookSeat(fi, pid, name, cls, paid) == 1)
                {
                    printLine();
                    cout << "  BOOKING CONFIRMED\n";
                    printLine();
                    cout << "  Flight    : " << flightCode[fi] << "  " << fromCity[fi] << " -> " << toCity[fi] << "\n";
                    cout << "  Passenger : " << name << "  [" << pid << "]\n";
                    cout << "  Seat No.  : " << assignedSeat << "\n";
                    cout << "  Class     : " << cls << "\n";
                    cout << "  Payment   : " << (paid ? "Paid" : "Unpaid") << "\n";
                    printLine();
                    saveBookings("bookings.txt");
                }
            }
        }
        else if (choice == 2)   // ---- Cancel Booking ----
        {
            cout << "\n";
            printLine();
            cout << "  CANCEL BOOKING\n";
            printLine();
            cout << "  Enter Passenger ID to cancel (e.g. P001) : ";
            char pid[20];
            cin.getline(pid, 20);

            if (cancelBooking(pid) == 1)
            {
                cout << "  [OK] Booking for " << pid << " has been cancelled.\n";
                saveBookings("bookings.txt");
            }
            else
            {
                cout << "  [ERR] Passenger ID not found.\n";
            }
        }
        else if (choice == 3)   // ---- View Seat Map ----
        {
            cout << "\n";
            printLine();
            cout << "  VIEW SEAT MAP\n";
            printLine();
            for (int fi = 0; fi < NFLIGHTS; fi++)
            {
                cout << "    " << flightCode[fi] << "  " << fromCity[fi] << " -> " << toCity[fi] << "\n";
            }
            printLine();
            cout << "  Enter Flight Code : ";
            char code[10];
            cin.getline(code, 10);

            int fi = findFlight(code);
            if (fi == -1)
            {
                cout << "  [ERR] Flight not found.\n";
            }
            else
            {
                displaySeatMap(fi);
            }
        }
        else if (choice == 4)   // ---- View Manifest ----
        {
            cout << "\n";
            printLine();
            cout << "  VIEW PASSENGER MANIFEST\n";
            printLine();
            for (int fi = 0; fi < NFLIGHTS; fi++)
            {
                cout << "    " << flightCode[fi] << "  " << fromCity[fi] << " -> " << toCity[fi] << "\n";
            }
            printLine();
            cout << "  Enter Flight Code : ";
            char code[10];
            cin.getline(code, 10);

            int fi = findFlight(code);
            if (fi == -1)
            {
                cout << "  [ERR] Flight not found.\n";
            }
            else
            {
                viewManifest(fi);
            }
        }
        else if (choice == 5)   // ---- Search by Name ----
        {
            cout << "\n";
            printLine();
            cout << "  SEARCH BY NAME\n";
            printLine();
            cout << "  Enter full or partial name (e.g. Ali) : ";
            char name[50];
            cin.getline(name, 50);
            searchByName(name);
        }
        else if (choice == 6)   // ---- Search by ID ----
        {
            cout << "\n";
            printLine();
            cout << "  SEARCH BY PASSENGER ID\n";
            printLine();
            cout << "  Enter Passenger ID (e.g. P001) : ";
            char pid[20];
            cin.getline(pid, 20);
            searchByID(pid);
        }
        else if (choice == 7)   // ---- Export Manifest ----
        {
            cout << "\n";
            printLine();
            cout << "  EXPORT MANIFEST TO FILE\n";
            printLine();
            for (int fi = 0; fi < NFLIGHTS; fi++)
            {
                cout << "    " << flightCode[fi] << "  " << fromCity[fi] << " -> " << toCity[fi] << "\n";
            }
            printLine();
            cout << "  Enter Flight Code : ";
            char code[10];
            cin.getline(code, 10);

            int fi = findFlight(code);
            if (fi == -1)
            {
                cout << "  [ERR] Flight not found.\n";
            }
            else
            {
                exportManifest(fi);
            }
        }
        else if (choice == 0)   // ---- Exit ----
        {
            saveBookings("bookings.txt");
            freeAll();
            cout << "  Goodbye!\n";
        }
        else
        {
            cout << "  [ERR] Invalid option. Please enter a number from the menu.\n";
        }

        if (choice != 0)
        {
            cout << "\n  Press Enter to continue...";
            cin.get();
        }
    }

    return 0;
}

// M1 - LOADER
void loadFlights(const char* file)
{
    ifstream in;
    in.open(file);

    if (!in)
    {
        cout << "[ERR] Cannot open " << file << "\n";
        return;
    }

    priceCents = new int[NFLIGHTS];              // dynamic 1D int array allocated on heap

    for (int i = 0; i < NFLIGHTS; i++)
    {
        in >> flightCode[i];
        in >> fromCity[i];
        in >> toCity[i];
        in >> capacity[i];
        in >> priceCents[i];                     // fill dynamic array from file
    }

    in.close();

    for (int fi = 0; fi < NFLIGHTS; fi++)        // initialise all seats to free
    {
        for (int si = 0; si < MAXSEATS; si++)
        {
            seatMap[fi][si] = 0;
        }
    }
}

void loadBookings(const char* file)
{
    bCap = INIT_CAP;
    bCount = 0;
    bookings = new char* [bCap];                  // char** heap array of C-string pointers
    for (int i = 0; i < bCap; i++)
    {
        bookings[i] = nullptr;                   // dangling pointer prevention
    }
    ifstream in;
    in.open(file);
    if (!in)
    {
        return;                             // no file yet, start empty
    }
    char line[BLEN];
    while (in.getline(line, BLEN))
    {
        if (line[0] == '\0' || line[0] == '#') 
            continue;

        if (bCount == bCap)
            growBookings();      // auto-grow before adding

        int len = strlen(line);
        bookings[bCount] = new char[len + 1];    // deep copy: allocate exact heap string
        strcpy_s(bookings[bCount], len + 1, line);

        // parse this line to mark seat in seatMap
        // format: CODE|PID|NAME|SEAT|CLASS|PAID
        char tmp[BLEN];
        strcpy_s(tmp, BLEN, line);
        char* p = tmp;

        char code[10];
        int  ci = 0;
        while (*p != '|' && *p != '\0')
        { 
            code[ci++] = *p++;
        }   // extract flight code
        code[ci] = '\0';
        p++;                                     // skip '|'

        while (*p != '|' && *p != '\0') 
            p++;     // skip PID
        p++;
        while (*p != '|' && *p != '\0') 
            p++;     // skip NAME
        p++;

        char sBuf[6];
        int  si = 0;
        while (*p != '|' && *p != '\0') { 
            sBuf[si++] = *p++;
        }   // extract seat number
        sBuf[si] = '\0';

        int seat = 0;
        for (int k = 0; sBuf[k] != '\0'; k++)
        {
            seat = seat * 10 + (sBuf[k] - '0'); // manual string-to-int
        }

        int fi = findFlight(code);
        if (fi != -1 && seat >= 1 && seat <= MAXSEATS)
        {
            seatMap[fi][seat - 1] = 1;           // mark seat as booked
        }
        bCount++;
    }

    in.close();
}

void saveBookings(const char* file)
{
    ofstream out;
    out.open(file);                              // overwrite mode, rewrites all bookings
    for (int i = 0; i < bCount; i++)
        if (bookings[i] != nullptr)
            out << bookings[i] << "\n";
    out.close();
}

// M2 - SEAT MAP

void displaySeatMap(int fi)
{
    cout << "\n";
    printLine();
    cout << "  SEAT MAP : " << flightCode[fi]
        << "   " << fromCity[fi] << " -> " << toCity[fi] << "\n";
    printLine();
    cout << "  [ O ] = Free     [ X ] = Booked\n\n";

    cout << "        1   2   3   4   5   6   7   8   9   10\n";

    for (int row = 0; row < 6; row++)            // outer loop - 6 rows
    {
        cout << "  R" << (row + 1) << "  ";
        for (int col = 0; col < 10; col++)       // inner loop - 10 cols
        {
            int idx = row * 10 + col;            // 2D index formula
            if (seatMap[fi][idx] == 0)
                cout << "[ O ]";
            else
                cout << "[ X ]";
        }
        cout << "\n";
    }

    int booked = 0;
    computeBooked(fi, booked);                   // pass by reference - fills booked

    printLine();
    cout << "  Booked : " << booked << " / " << capacity[fi]
        << "    Free : " << (capacity[fi] - booked) << "\n";
    printLine();
}

int findFreeSeat(int fi)
{
    for (int s = 0; s < capacity[fi]; s++)
    {
        if (seatMap[fi][s] == 0)
        {
            return s;
        }
    }// return 0-based index of first free seat
    return -1;                                   // -1 means flight is full
}

int seatIsFree(int fi, int seatNum)
{
    int idx = seatNum - 1;                       // convert 1-based to 0-based
    if (idx < 0 || idx >= capacity[fi])
    {
        return 0;
    }
    return (seatMap[fi][idx] == 0) ? 1 : 0;
}

// M3 - BOOKING ENGINE

void growBookings()
{
    /*
     * Double capacity: allocate new char*[newCap], copy all existing
     * pointers across, delete old array, point bookings at new array.
     * This is a deep copy of the pointer array (not the strings).
     */
    int    newCap = bCap * 2;
    char** newArr = new char* [newCap];           // new larger heap array

    for (int i = 0; i < bCount; i++)
    {
        newArr[i] = bookings[i];                 // copy existing pointers
    }

    for (int i = bCount; i < newCap; i++)
    {
        newArr[i] = nullptr;                     // initialise new slots
    }

    delete[] bookings;                           // free old pointer array
    bookings = nullptr;                          // dangling pointer prevention
    bookings = newArr;
    bCap = newCap;
}

void shrinkBookings()
{
    /*
     * Shrink to exact bCount: allocate new char*[bCount], copy pointers,
     * delete old array. Saves memory after a cancellation.
     */
    if (bCount == 0)
    {
        return;
    }

    char** newArr = new char* [bCount];           // exact-size new array
    for (int i = 0; i < bCount; i++)
    {
        newArr[i] = bookings[i];                 // deep copy pointers
    }
    delete[] bookings;
    bookings = nullptr;                          // dangling pointer prevention
    bookings = newArr;
    bCap = bCount;
}

int bookSeat(int fi, const char* pid, const char* name, const char* cls, int paid)
{
    int freeIdx = findFreeSeat(fi);
    if (freeIdx == -1)
    {
        return -1;
    }
    seatMap[fi][freeIdx] = 1;                    // mark seat booked in 2D seatMap
    int seatNum = freeIdx + 1;                   // convert 0-based index to 1-based number

    char seatStr[6];
    intToStr(seatNum, seatStr);                  // int to C-string, no sprintf

    char paidStr[3];
    intToStr(paid, paidStr);

    // build booking C-string: CODE|PID|NAME|SEAT|CLASS|PAID
    char entry[BLEN];
    strcpy_s(entry, BLEN, flightCode[fi]);
    strcat_s(entry, BLEN, "|");
    strcat_s(entry, BLEN, pid);
    strcat_s(entry, BLEN, "|");
    strcat_s(entry, BLEN, name);
    strcat_s(entry, BLEN, "|");
    strcat_s(entry, BLEN, seatStr);
    strcat_s(entry, BLEN, "|");
    strcat_s(entry, BLEN, cls);
    strcat_s(entry, BLEN, "|");
    strcat_s(entry, BLEN, paidStr);

    if (bCount == bCap)
    {
        growBookings();          // auto-grow if array is full
    }
    int len = strlen(entry);
    bookings[bCount] = new char[len + 1];        // allocate heap string
    strcpy_s(bookings[bCount], len + 1, entry);
    bCount++;

    char log[150];
    strcpy_s(log, 150, "BOOK ");
    strcat_s(log, 150, pid);
    strcat_s(log, 150, " ");
    strcat_s(log, 150, flightCode[fi]);
    strcat_s(log, 150, " Seat-");
    strcat_s(log, 150, seatStr);
    logAudit(log);                               // append to audit.txt

    return 1;
}

int cancelBooking(const char* pid)
{
    int found = -1;

    for (int i = 0; i < bCount && found == -1; i++)
    {
        if (bookings[i] == nullptr)
        {
            continue;
        }
        char* p = bookings[i];
        while (*p != '|' && *p != '\0')
        {
            p++;     // skip flight code
        }
        p++;                                     // skip '|'

        char extractID[20];
        int  ci = 0;
        while (*p != '|' && *p != '\0') { 
            extractID[ci++] = *p++;
        }  // extract PID
        extractID[ci] = '\0';

        if (strcmp(extractID, pid) == 0)
        {
            found = i;
        }
    }

    if (found == -1)
    {
        return -1;
    }
    char tmp[BLEN];
    strcpy_s(tmp, BLEN, bookings[found]);
    char* p = tmp;

    char code[10];
    int  ci = 0;
    while (*p != '|' && *p != '\0') { 
        code[ci++] = *p++;
    }   // extract flight code
    code[ci] = '\0';
    p++;

    while (*p != '|' && *p != '\0') p++; p++;   // skip PID
    while (*p != '|' && *p != '\0') p++; p++;   // skip NAME

    char sBuf[6];
    ci = 0;
    while (*p != '|' && *p != '\0') { 
        sBuf[ci++] = *p++;
    }   // extract seat number
    sBuf[ci] = '\0';

    int seat = 0;
    for (int k = 0; sBuf[k] != '\0'; k++)
    {
        seat = seat * 10 + (sBuf[k] - '0');     // string-to-int for seat
    }

    int fi = findFlight(code);
    if (fi != -1 && seat >= 1)
    {
        seatMap[fi][seat - 1] = 0;              // free seat in 2D array
    }
    char log[100];
    strcpy_s(log, 100, "CANCEL ");
    strcat_s(log, 100, pid);
    strcat_s(log, 100, " Seat-");
    strcat_s(log, 100, sBuf);
    logAudit(log);                              // append cancel event to audit

    delete[] bookings[found];
    bookings[found] = nullptr;                  // dangling pointer prevention

    for (int i = found; i < bCount - 1; i++)
    {
        bookings[i] = bookings[i + 1];          // shift left to close the gap
    }
    bookings[bCount - 1] = nullptr;
    bCount--;

    shrinkBookings();                           // auto-shrink after removal
    return 1;
}

// M4 - SEARCH

void searchByName(const char* target)
{
    printLine();
    cout << "  SEARCH RESULTS FOR NAME : \"" << target << "\"\n";
    printLine();
    int hits = 0;

    for (int i = 0; i < bCount; i++)
    {
        if (bookings[i] == nullptr)
        {
            continue;
        }
        char* p = bookings[i];
        while (*p != '|' && *p != '\0') p++;  p++;  // skip flight code
        while (*p != '|' && *p != '\0') p++;  p++;  // skip PID

        char nameBuf[50];
        int  ci = 0;
        while (*p != '|' && *p != '\0') { 
            nameBuf[ci++] = *p++;
        }   // extract name
        nameBuf[ci] = '\0';

        int tLen = strlen(target);
        int nLen = strlen(nameBuf);
        int matched = 0;

        for (int s = 0; s <= nLen - tLen && !matched; s++)   // outer: start positions
        {
            int ok = 1;
            for (int k = 0; k < tLen && ok; k++)             // inner: char-by-char match
                if (nameBuf[s + k] != target[k]) ok = 0;
            if (ok) matched = 1;
        }

        if (matched)
        {
            char code[10], pid[20], name[50], seat[6], cls[20], paid[3];
            parseBooking(bookings[i], code, pid, name, seat, cls, paid);
            cout << "  Flight: " << code << "  ID: " << pid << "  Name: " << name
                << "  Seat: " << seat << "  Class: " << cls
                << "  Paid: " << (paid[0] == '1' ? "Yes" : "No") << "\n";
            hits++;
        }
    }

    if (hits == 0)
    {
        cout << "  No results found for \"" << target << "\".\n";
    }
    else
    {
        cout << "  Total matches found : " << hits << "\n";
    }

    printLine();
}

void searchByID(const char* pid)
{
    printLine();
    cout << "  SEARCH RESULTS FOR ID : \"" << pid << "\"\n";
    printLine();
    int hits = 0;

    for (int i = 0; i < bCount; i++)
    {
        if (bookings[i] == nullptr)
        {
            continue;
        }

        char* p = bookings[i];
        while (*p != '|' && *p != '\0') p++;  p++;  // skip flight code

        char extractID[20];
        int  ci = 0;
        while (*p != '|' && *p != '\0') { 
            extractID[ci++] = *p++;
        }   // extract PID
        extractID[ci] = '\0';

        if (strcmp(extractID, pid) == 0)
        {
            char code[10], xpid[20], name[50], seat[6], cls[20], paid[3];
            parseBooking(bookings[i], code, xpid, name, seat, cls, paid);
            cout << "  Flight: " << code << "  ID: " << xpid << "  Name: " << name
                << "  Seat: " << seat << "  Class: " << cls
                << "  Paid: " << (paid[0] == '1' ? "Yes" : "No") << "\n";
            hits++;
        }
    }

    if (hits == 0)
    {
        cout << "  No passenger found with ID \"" << pid << "\".\n";
    }
    else
    {
        cout << "  Total matches found : " << hits << "\n";
    }

    printLine();
}

// M5 - MANIFEST VIEW & EXPORT

/*
 * parseBooking: walks a char* through the booking C-string field by
 * field, splitting at '|' separators using pointer arithmetic.
 * All 6 fields are written into the caller-provided buffers.
 */
void parseBooking(const char* line, char* code, char* pid, char* name,
    char* seat, char* cls, char* paid)
{
    char tmp[BLEN];
    strcpy_s(tmp, BLEN, line);
    char* p = tmp;
    int   ci;

    ci = 0; while (*p != '|' && *p != '\0') { code[ci++] = *p++; } code[ci] = '\0'; p++;
    ci = 0; while (*p != '|' && *p != '\0') { pid[ci++] = *p++; } pid[ci] = '\0'; p++;
    ci = 0; while (*p != '|' && *p != '\0') { name[ci++] = *p++; } name[ci] = '\0'; p++;
    ci = 0; while (*p != '|' && *p != '\0') { seat[ci++] = *p++; } seat[ci] = '\0'; p++;
    ci = 0; while (*p != '|' && *p != '\0') { cls[ci++] = *p++; } cls[ci] = '\0'; p++;
    ci = 0; while (*p != '\0') { paid[ci++] = *p++; } paid[ci] = '\0';
}

void viewManifest(int fi)
{
    printLine();
    cout << "  PASSENGER MANIFEST : " << flightCode[fi]
        << "   " << fromCity[fi] << " -> " << toCity[fi] << "\n";
    cout << "  Economy Price : Rs." << priceCents[fi] / 100 << "\n";   // dynamic int* used here
    printLine();
    cout << "  ID        NAME                     SEAT   CLASS       PAID\n";
    printLine();

    int total = 0;
    int revenue = 0;

    for (int i = 0; i < bCount; i++)
    {
        if (bookings[i] == nullptr)
        {
            continue;
        }
        char code[10], pid[20], name[50], seat[6], cls[20], paid[3];
        parseBooking(bookings[i], code, pid, name, seat, cls, paid);

        if (strcmp(code, flightCode[fi]) != 0)
        {
            continue;
        }

        // print row with manual padding for aligned columns
        cout << "  ";
        cout << pid;
        for (int sp = strlen(pid); sp < 10; sp++) cout << " ";   // pad ID
        cout << name;
        for (int sp = strlen(name); sp < 25; sp++) cout << " ";   // pad name
        cout << seat;
        for (int sp = strlen(seat); sp < 7; sp++) cout << " ";   // pad seat
        cout << cls;
        for (int sp = strlen(cls); sp < 12; sp++) cout << " ";   // pad class
        cout << (paid[0] == '1' ? "Yes" : "No") << "\n";

        total++;
        if (paid[0] == '1')
        {
            revenue += priceCents[fi];                             // int arithmetic - add cents
        }
    }

    printLine();
    cout << "  Total Passengers : " << total << "\n";
    cout << "  Revenue Collected: Rs." << (revenue / 100)         // divide cents -> rupees
        << "." << (revenue % 100) << "\n";                       // remainder -> paisa
    printLine();
}

void exportManifest(int fi)
{
    char fname[30];
    strcpy_s(fname, 30, flightCode[fi]);
    strcat_s(fname, 30, "_manifest.txt");        // build filename string

    ofstream out;
    out.open(fname);                             // ofstream - creates or overwrites
    if (!out) { 
        cout << "  [ERR] Cannot create file.\n";
        return;
    }

    out << "================================================\n";
    out << "  MANIFEST : " << flightCode[fi]
        << "   " << fromCity[fi] << " -> " << toCity[fi] << "\n";
    out << "  Economy Price : Rs." << priceCents[fi] / 100 << "\n";
    out << "================================================\n";
    out << "  ID        NAME                     SEAT   CLASS       PAID\n";
    out << "------------------------------------------------\n";

    int total = 0;
    int revenue = 0;

    for (int i = 0; i < bCount; i++)
    {
        if (bookings[i] == nullptr)
        {
            continue;
        }
        char code[10], pid[20], name[50], seat[6], cls[20], paid[3];
        parseBooking(bookings[i], code, pid, name, seat, cls, paid);

        if (strcmp(code, flightCode[fi]) != 0)
        {
            continue;
        }
        out << "  ";
        out << pid;
        for (int sp = strlen(pid); sp < 10; sp++) out << " ";
        out << name;
        for (int sp = strlen(name); sp < 25; sp++) out << " ";
        out << seat;
        for (int sp = strlen(seat); sp < 7; sp++) out << " ";
        out << cls;
        for (int sp = strlen(cls); sp < 12; sp++) out << " ";
        out << (paid[0] == '1' ? "Paid" : "Unpaid") << "\n";

        total++;
        if (paid[0] == '1') revenue += priceCents[fi];
    }

    out << "------------------------------------------------\n";
    out << "  Total Passengers : " << total << "\n";
    out << "  Revenue Collected: Rs." << (revenue / 100) << "." << (revenue % 100) << "\n";
    out << "================================================\n";
    out.close();

    cout << "  [OK] Manifest exported to \"" << fname << "\"\n";
}

// M6 - MENU DRIVER & UTILITIES

void printLine()
{
    cout << "  -------------------------------------------------------\n";
}

/*
 * computeBooked: counts booked seats for flight fi by scanning seatMap.
 * Result is written into bookedOut via pass by reference (&).
 * Called by printMenu() and displaySeatMap() to avoid code duplication.
 */
void computeBooked(int fi, int& bookedOut)
{
    bookedOut = 0;
    for (int s = 0; s < capacity[fi]; s++)
    {
        if (seatMap[fi][s] == 1)
        {
            bookedOut++;    // increment ref variable directly
        }
    }
}

void printMenu()
{
    int totalSeats = 0;
    int totalBooked = 0;

    for (int fi = 0; fi < NFLIGHTS; fi++)
    {
        totalSeats += capacity[fi];              // sum all flight capacities
    }

    for (int fi = 0; fi < NFLIGHTS; fi++)
    {
        int booked = 0;
        computeBooked(fi, booked);               // pass by reference to get count
        totalBooked += booked;
    }

    cout << "\n  =======================================================\n";
    cout << "          FLIGHT SEAT RESERVATION SYSTEM v1.0\n";
    cout << "  =======================================================\n";
    cout << "  Flights : " << NFLIGHTS
        << "  | Total Seats : " << totalSeats
        << "  | Booked : " << totalBooked
        << "  | Free : " << (totalSeats - totalBooked) << "\n";
    printLine();

    for (int fi = 0; fi < NFLIGHTS; fi++)        // per-flight live status table
    {
        int booked = 0;
        computeBooked(fi, booked);               // pass by reference
        int free = capacity[fi] - booked;

        cout << "  " << flightCode[fi]
            << "   " << fromCity[fi];
        for (int sp = strlen(fromCity[fi]); sp < 12; sp++) cout << " ";   // pad city name
        cout << "-> " << toCity[fi];
        for (int sp = strlen(toCity[fi]); sp < 12; sp++) cout << " ";   // pad city name
        cout << "  Cap:" << capacity[fi]
            << "  Bkd:" << booked
            << "  Free:" << free;
        if (free == 0) cout << "  [FULL]";
        cout << "\n";
    }

    printLine();
    cout << "  [1] Book a Seat\n";
    cout << "  [2] Cancel Booking\n";
    cout << "  [3] View Seat Map\n";
    cout << "  [4] View Manifest\n";
    cout << "  [5] Search by Name\n";
    cout << "  [6] Search by ID\n";
    cout << "  [7] Export Manifest to File\n";
    cout << "  [0] Exit\n";
    cout << "  =======================================================\n";
}

int findFlight(const char* code)
{
    for (int fi = 0; fi < NFLIGHTS; fi++)
        if (strcmp(flightCode[fi], code) == 0) return fi;   // return index if match found
    return -1;                                              // -1 = not found
}

void logAudit(const char* entry)
{
    ofstream log;
    log.open("audit.txt", ios::app);             // append mode - never overwrites
    if (log) log << "[LOG] " << entry << "\n";
    log.close();
}

void freeAll()
{
    for (int i = 0; i < bCap; i++)
    {
        if (bookings[i] != nullptr)
        {
            delete[] bookings[i];
            bookings[i] = nullptr;               // dangling pointer prevention
        }
    }
    delete[] bookings;
    bookings = nullptr;                          // dangling pointer prevention
    bCount = 0;
    bCap = 0;

    delete[] priceCents;                         // free dynamic int array
    priceCents = nullptr;                        // dangling pointer prevention
}

void intToStr(int n, char* buf)
{
    if (n == 0) { buf[0] = '0'; buf[1] = '\0'; return; }

    char tmp[12];
    int  i = 0;

    while (n > 0)
    {
        tmp[i++] = '0' + (n % 10);              // extract digits right to left
        n /= 10;
    }

    tmp[i] = '\0';

    int len = i;
    for (int k = 0; k < len; k++)
        buf[k] = tmp[len - 1 - k];              // reverse into output buffer
    buf[len] = '\0';
}
