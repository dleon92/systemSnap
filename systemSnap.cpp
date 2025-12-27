#ifdef _WIN32
#include <windows.h>
#endif
#include <iostream>
#include <locale>
#include <vector>
#include <filesystem>
#include <cstdio>
#include "json.hpp"
#include <fstream>
#include <iomanip>
#include <unistd.h>
#include <chrono>
#include <ctime>
#include <string>
#include <thread>
#include <cstdio>
#include <algorithm>
#include <clocale>
#define RESET "\033[0m"
#define BOLD "\033[1m"
#define GREEN "\033[32m"
#define CYAN "\033[36m"
#define YELLOW "\033[33m"
#define RED "\033[31m"
using namespace std;
using json = nlohmann::json;

const char *dias[] = {
    "Sunday", "Monday", "Tuesday",
    "wednesday", "Thursday", "Friday", "Saturday"};

const char *meses[] = {
    "january", "february", "march", "april",
    "may", "june", "july", "august",
    "september", "october", "november", "december"};

// o que é isso?
#ifdef _WIN32
typedef LONG(WINAPI *RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
#endif

struct Processor
{
    string model;
    int cores;
    int threads;
};

struct OperatingSystem
{
    string systemName;
    string version;
};

struct Motherboard
{
    string label;
    string model;
};

struct RAM
{
    float capacity;
    float speed;
};

struct GPU
{
    string model;
    int capacity;
};

struct Data
{
    time_t moment;
    OperatingSystem system;
    Processor processor;
    Motherboard motherboard;
    RAM memory;
    GPU gpu;
};

string storageFolder = "PCs";

void encoding()
{
#ifdef _WIN32 // for windows
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#else // for linux
    locale::global(locale(""));
    wcout.imbue(locale());
#endif
}

void clearTerminal()
{
#ifdef _WIN32 // for windows
    system("cls");
#else // for linux
    system("clear");
#endif
}

bool checkFolderExistence()
{
    if (filesystem::is_directory(storageFolder))
    {
        return true;
    }
    else
    {
        filesystem::create_directory(storageFolder);
    }
    return false;
}

void loadingForFilesList()
{
    clearTerminal();
    cout << "Analysing available data..." << endl;
    this_thread::sleep_for(chrono::milliseconds(250));
    clearTerminal();
    return;
}

void askForEnter()
{
    cout << "\nPress ENTER to continue...";
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // clear buffer
    cin.get();                                                     // Awaits for the next ENTER
    clearTerminal();
}

void createJsonList()
{
    loadingForFilesList();

    ////////////////////////////////////////////////////////////// HEADER

    cout << CYAN << "==========================================" << RESET << endl;
    cout << BOLD << "                systemSnap                " << RESET << endl;
    cout << CYAN << "==========================================" << RESET << endl;
    cout << "\nAVAILABLE FILES: " << endl;

    vector<string> jsonFile;
    for (const auto &entry : filesystem::directory_iterator(storageFolder))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".json")
        {
            jsonFile.push_back(entry.path().filename().string());
        }
    }

    int cont = 1;

    for (const auto &name : jsonFile)
    {
        cout << "\n"
             << setw(2) << setfill('0') << YELLOW << cont << RESET << " | " << GREEN << name << RESET;
        cont++;
    }
    int choice;
    cout << "\n\nChoose which configuration you want to view [" << RED << "0 to go back" << RESET "]:  \t";
    cin >> choice;
    if (choice == 0)
    {
        clearTerminal();
        return;
    }
    if (choice >= cont)
    {
        cout << "\nFile not found." << endl;
        askForEnter();
        clearTerminal();
        return;
    }
    int index = choice - 1;

    clearTerminal();

    ifstream file(storageFolder + "/" + jsonFile[index]);

    cout << CYAN << "==========================================" << RESET << endl;
    cout << BOLD << "                systemSnap                " << RESET << endl;
    cout << CYAN << "==========================================" << RESET << endl;
    cout << "\nSYSTEM SNAPSHOT: " << GREEN << jsonFile[index] << RESET << endl;

    if (!file.is_open())
    {
        cerr << RED << "\nFailed to open file." << RESET << endl;
        return;
    }

    json j;
    file >> j;

    // setup date input from json filme to exibit
    time_t date = j["moment"];
    tm *dt = localtime(&date);

    cout
        << CYAN << "\nCOLLECTION TIME\n" RESET << YELLOW << dias[dt->tm_wday] << ", "
        << dt->tm_mday << " de "
        << meses[dt->tm_mon] << " de "
        << (dt->tm_year + 1900)
        << " - "
        << (dt->tm_hour < 10 ? "0" : "") << dt->tm_hour << ":"
        << (dt->tm_min < 10 ? "0" : "") << dt->tm_min << ":"
        << (dt->tm_sec < 10 ? "0" : "") << dt->tm_sec
        << RESET << endl;

    // setup operating system input from json filme to exibit
    string nameOperatingSystem = j["operating_system"];
    string versionSo = j["operating_system_version"];
    string printSystem = nameOperatingSystem + " " + versionSo;
    cout << CYAN "\nOPERATING SYSTEM" << RESET << endl;
    cout << YELLOW << printSystem << RESET << endl;

    // setup processos input from json filme to exibit
    string processorModel = j["processor_model"];
    int cores = j["processor_cores"];
    int threads = j["processor_threads"];

    cout << CYAN << "\nPROCESSOR" << RESET << endl;
    cout << YELLOW << processorModel << " Cores: " << cores << " Threads: " << threads << RESET << endl;

    // setup motherboard input from json filme to exibit
    string moblabel = j["motherboard_label"];
    string mobmodel = j["motherboard_model"];

    cout << CYAN << "\nMOTHERBOARD" << RESET << endl;
    cout << YELLOW << moblabel << " " << mobmodel << RESET << endl;

    // setup RAM input from json filme to exibit
    float memoryCapacity = j["ram_capacity"];
    float memorySpeed = j["ram_speed"];

    cout << CYAN << "\nRAM" << RESET << endl;
    cout << YELLOW << memoryCapacity << " GB " << memorySpeed << " MHz" << RESET << endl;

    // setup GPU input from json filme to exibit
    string gpuModel = j["gpu_model"];
    unsigned long long gpuCapcity = j["gpu_capacity"];

    cout << CYAN << "\nGPU" << RESET << endl;
    cout << YELLOW << gpuModel << " " << gpuCapcity << " GB" << RESET << endl;

    askForEnter();
    return;
}

void checksForJsonExistence(const string &folder)
{
    bool found = false;
    checkFolderExistence();

    try
    {
        for (const auto &entry : filesystem::directory_iterator(folder))
        {
            if (entry.is_regular_file() &&
                entry.path().extension() == ".json")
            {
                found = true;
                break; // already found, no need to keep iterating
            }
        }
    }
    catch (const filesystem::filesystem_error &)
    {
        cerr << RED << "\nFailed to open folder." << RESET << endl;
        return;
    }

    if (found)
    {
        createJsonList();
    }
    else
    {
        cout << "\nNO data found." << endl;
        askForEnter();
    }
}

void delay(int delayTime)
{
    this_thread::sleep_for(chrono::milliseconds(delayTime));
    return;
}

#ifdef _WIN32
string obtainMotherboardLabel()
{
    FILE *pipe = _popen("wmic baseboard get Manufacturer", "r");
    if (!pipe)
    {
        return "\nFailed on retrieving info.";
    }

    char buffer[256];
    string result;
    while (fgets(buffer, sizeof(buffer), pipe))
    {
        result += buffer;
    }
    _pclose(pipe);

    result.erase(0, 33);
    result.erase(0, result.find_first_not_of(" \t\r\n"));
    result.erase(result.find_last_not_of(" \t\r\n") + 1);

    return result;
}

string obtainMotherboardModel()
{
    FILE *pipe = _popen("wmic baseboard get Product", "r");
    if (!pipe)
        return "Falha ao obter informações.";

    char buffer[256];
    string stringCollected;

    // read the first line (header)
    fgets(buffer, sizeof(buffer), pipe);

    // read the second line
    if (!fgets(buffer, sizeof(buffer), pipe))
    {
        _pclose(pipe);
        return "Unknown";
    }

    stringCollected = buffer;

    _pclose(pipe);

    // remove spaces and new paragraphs
    stringCollected.erase(0, stringCollected.find_first_not_of(" \t\r\n"));
    stringCollected.erase(stringCollected.find_last_not_of(" \t\r\n") + 1);

    return stringCollected;
}

vector<unsigned long long> obtainRamCapacity()
{
    vector<unsigned long long> ramCapacity;

    FILE *pipe = _popen("wmic MEMORYCHIP get Capacity", "r");
    if (!pipe)
        return ramCapacity;

    char buffer[256];

    // read the first line
    fgets(buffer, sizeof(buffer), pipe);

    while (fgets(buffer, sizeof(buffer), pipe))
    {
        string stringCollected = buffer;

        // trim de espaços e quebras de linha
        stringCollected.erase(0, stringCollected.find_first_not_of(" \t\r\n"));
        stringCollected.erase(stringCollected.find_last_not_of(" \t\r\n") + 1);

        if (stringCollected.empty())
            continue; // ignore empty lines

        // converte para número
        try
        {
            unsigned long long valor = std::stoull(stringCollected);
            ramCapacity.push_back(valor);
        }
        catch (...)
        {
            // ignore empty lines
        }
    }

    _pclose(pipe);
    return ramCapacity;
}

int obtainRamSpeed()
{
    FILE *pipe = _popen("wmic MEMORYCHIP get Speed", "r");
    if (!pipe)
        return -1; // fail

    char buffer[256];

    // reads first line
    if (!fgets(buffer, sizeof(buffer), pipe))
    {
        _pclose(pipe);
        return -1;
    }

    // reads second line
    if (!fgets(buffer, sizeof(buffer), pipe))
    {
        _pclose(pipe);
        return -1;
    }

    _pclose(pipe);

    string stringCollected = buffer;

    // remove spaces and new paragraphs
    stringCollected.erase(0, stringCollected.find_first_not_of(" \t\r\n"));
    stringCollected.erase(stringCollected.find_last_not_of(" \t\r\n") + 1);

    try
    {
        int ramSpeed = stoi(stringCollected); // convert string into int
        return ramSpeed;
    }
    catch (...)
    {
        return -1; // fail
    }
}

string obtainGPU()
{
    FILE *pipe = _popen("wmic path win32_videocontroller get name", "r");
    if (!pipe)
        return "Desconhecido";

    char buffer[256];

    // read first line
    if (!fgets(buffer, sizeof(buffer), pipe))
    {
        _pclose(pipe);
        return "Desconhecido";
    }

    // read second line
    if (!fgets(buffer, sizeof(buffer), pipe))
    {
        _pclose(pipe);
        return "Desconhecido";
    }

    _pclose(pipe);

    string stringCollected = buffer;

    // trim de espaços e quebras de linha
    stringCollected.erase(0, stringCollected.find_first_not_of(" \t\r\n"));
    stringCollected.erase(stringCollected.find_last_not_of(" \t\r\n") + 1);

    return stringCollected;
}

int obtainGPUCapacity()
{
    FILE *pipe = _popen("wmic path win32_videocontroller get adapterram", "r");
    if (!pipe)
        return 0;

    char buffer[256];

    // read first line
    if (!fgets(buffer, sizeof(buffer), pipe))
    {
        _pclose(pipe);
        return 0;
    }

    // read second line
    if (!fgets(buffer, sizeof(buffer), pipe))
    {
        _pclose(pipe);
        return 0;
    }

    _pclose(pipe);

    string stringCollected = buffer;

    // trim
    stringCollected.erase(0, stringCollected.find_first_not_of(" \t\r\n"));
    stringCollected.erase(stringCollected.find_last_not_of(" \t\r\n") + 1);

    try
    {
        unsigned long long bytes = stoull(stringCollected);

        float gb = bytes / (1024.0f * 1024.0f * 1024.0f);

        // rounds to int
        int gbInt = static_cast<int>(std::round(gb));

        return gbInt;
    }
    catch (...)
    {
        return 0;
    }
}
#endif

void loadingMessage()
{
    cout << "Retrieving operating system data..." << endl;
    delay(50);
    cout << "Retrieving processor data..." << endl;
    delay(50);
    cout << "Retrieving motherboard data..." << endl;
    delay(50);
    cout << "Retrieving RAM data..." << endl;
    delay(50);
    cout << "Retrieving GPU data..." << endl;
    delay(50);
    cout << "Compiling data..." << endl;
    delay(50);
    cout << "Done!" << endl;
    delay(50);

    return;
}

void collectData()
{
    Data *newData = new Data;

#ifdef _WIN32
    loadingMessage();
    clearTerminal();

    //////////////////////////////////////////////////////////////////////// HEADER

    cout << CYAN << "==========================================" << RESET << endl;
    cout << BOLD << "                systemSnap                " << RESET << endl;
    cout << CYAN << "==========================================" << RESET << endl;

    //////////////////////////////////////////////////////////////////////// OBTAIN MOMENT DATA
    auto moment_now = chrono::system_clock::now();
    time_t timestamp = chrono::system_clock::to_time_t(moment_now);
    newData->moment = timestamp;

    tm *dt = localtime(&timestamp);

    cout << CYAN
         << "\nDATE\n"
         << RESET << YELLOW
         << dias[dt->tm_wday] << ", "
         << dt->tm_mday << " de "
         << meses[dt->tm_mon] << " de "
         << (dt->tm_year + 1900)
         << " - "
         << (dt->tm_hour < 10 ? "0" : "") << dt->tm_hour << ":"
         << (dt->tm_min < 10 ? "0" : "") << dt->tm_min << ":"
         << (dt->tm_sec < 10 ? "0" : "") << dt->tm_sec
         << RESET << endl;

    //////////////////////////////////////////////////////////////////////// OBTAIN SYSTEM DATA
    string operatingSystem_name = "Windows";
    int operatingSystemVersion = 0;
    HMODULE hMod = GetModuleHandleW(L"ntdll.dll");
    if (hMod)
    {
        auto fx = (RtlGetVersionPtr)GetProcAddress(hMod, "RtlGetVersion");
        if (fx)
        {
            RTL_OSVERSIONINFOW v = {};
            v.dwOSVersionInfoSize = sizeof(v);

            if (fx(&v) == 0)
            {
                if (v.dwMajorVersion == 6 && v.dwMinorVersion == 1)
                    operatingSystemVersion = 7;
                else if (v.dwMajorVersion == 6 && v.dwMinorVersion == 2)
                    operatingSystemVersion = 8;
                else if (v.dwMajorVersion == 6 && v.dwMinorVersion == 3)
                    operatingSystemVersion = 8; // 8.1
                else if (v.dwMajorVersion == 10 && v.dwMinorVersion == 0)
                    operatingSystemVersion = (v.dwBuildNumber >= 22000) ? 11 : 10;
            }
        }
    }

    string aaa = to_string(operatingSystemVersion);
    newData->system.systemName = operatingSystem_name;
    newData->system.version = aaa;

    cout << CYAN << "\nOPERATING SYSTEM" << RESET << endl;
    cout << YELLOW << newData->system.systemName << " " << newData->system.version << RESET << endl;

    //////////////////////////////////////////////////////////////////////// OBTAIN CPU DATA
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);

    HKEY hKey;
    char CPUname[256];
    DWORD size = sizeof(CPUname);
    RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey);
    RegQueryValueExA(hKey, "ProcessorNameString", nullptr, nullptr, (LPBYTE)CPUname, &size);
    RegCloseKey(hKey);

    FILE *pipe = _popen("wmic cpu get NumberOfCores", "r");
    if (!pipe)
    {
        cerr << RED << "\nFailed on retrieving info." << RESET << endl;
        return;
    }

    char buffer[256];
    string result;
    while (fgets(buffer, sizeof(buffer), pipe))
    {
        result += buffer;
    }
    _pclose(pipe);

    istringstream iss(result);
    string word;
    int cores = 0;

    while (iss >> word)
    {
        try
        {
            cores = stoi(word);
            break;
        }
        catch (...)
        {
        }
    }

    newData->processor.model = CPUname;
    newData->processor.cores = cores;
    newData->processor.threads = sysinfo.dwNumberOfProcessors;
    cout << CYAN << "\nPROCESSOR" << YELLOW << endl;
    cout << newData->processor.model << " Cores: " << newData->processor.cores << " Threads: " << newData->processor.threads << RESET << endl;

    //////////////////////////////////////////////////////////////////////// OBTAIN MOTHERBOARD DATA

    string motherboard_label = obtainMotherboardLabel();
    string motherboard_model = obtainMotherboardModel();

    newData->motherboard.label = motherboard_label;
    newData->motherboard.model = motherboard_model;
    cout << CYAN << "\nMOTHERBOARD" << YELLOW << endl;
    cout << newData->motherboard.label << " " << newData->motherboard.model << RESET << endl;

    //////////////////////////////////////////////////////////////////////// OBTAIN RAM DATA

    auto ramCapacity = obtainRamCapacity();
    unsigned long long totalRAM = 0;
    for (auto c : ramCapacity)
        totalRAM += c;

    int ramVel = obtainRamSpeed();
    if (ramVel == -1)
        cout << "Falha ao obter velocidade da RAM\n";

    float totalRamCapacityIntoGB = totalRAM / (1024 * 1024 * 1024);
    newData->memory.capacity = totalRamCapacityIntoGB;
    newData->memory.speed = ramVel;

    // cout << "Total RAM: " << totalRAM / (1024 * 1024 * 1024) << " GB\n";

    // cout << "Velocidade RAM: " << ramVel << " MHz\n";

    cout << CYAN << "\nRAM MEMORY" << YELLOW << endl;
    cout << newData->memory.capacity << " GB " << newData->memory.speed << " MHz" << RESET << endl;

    //////////////////////////////////////////////////////////////////////// OBTAIN GPU DATA
    string gpu = obtainGPU();
    newData->gpu.model = gpu;
    float gpuCapacity = obtainGPUCapacity();
    newData->gpu.capacity = gpuCapacity;

    cout << CYAN << "\nGPU" << YELLOW << endl;
    cout << newData->gpu.model << " " << newData->gpu.capacity << " GB" << RESET << endl;

    //////////////////////////////////////////////////////////////////////// FILE CREATION

    char option;
    cout << "\n\nDo you wanna save this information? [" << GREEN << "Y" << RESET << "/" << RED << "N" << RESET << "] \t";
    cin >> option;

    if (option != 'y' && option != 'Y')
    {
        system("cls");
        return;
    }
    cin.ignore();
    string fileName;
    cout << "\nType the desired file name (no spaces): \t";
    getline(cin, fileName);
    string fileNameWithExtention = storageFolder + "/" + fileName + ".json";
    ofstream file(fileNameWithExtention);

    if (!file.is_open())
    {
        cerr << RED << "\nFailed to create the file." << RESET << endl;
        return;
    }

    json data;
    data["moment"] = newData->moment;
    data["operating_system"] = newData->system.systemName;
    data["operating_system_version"] = newData->system.version;
    data["processor_model"] = newData->processor.model;
    data["processor_cores"] = newData->processor.cores;
    data["processor_threads"] = newData->processor.threads;
    data["motherboard_label"] = newData->motherboard.label;
    data["motherboard_model"] = newData->motherboard.model;
    data["ram_capacity"] = newData->memory.capacity;
    data["ram_speed"] = newData->memory.speed;
    data["gpu_model"] = newData->gpu.model;
    data["gpu_capacity"] = newData->gpu.capacity;

    file << data.dump(10);
    file.close();

    cout << "\nFile created sucessfully." << endl;
    delay(200);
    clearTerminal();

#else
    loadingMessage();
    clearTerminal();

    //////////////////////////////////////////////////////////////////////// HEADER

    cout << CYAN << "==========================================" << RESET << endl;
    cout << BOLD << "                systemSnap                " << RESET << endl;
    cout << CYAN << "==========================================" << RESET << endl;

    //////////////////////////////////////////////////////////////////////// MOMENT RETRIEVING
    setlocale(LC_TIME, "pt-BR.UTF-8");
    auto now = chrono::system_clock::now();
    time_t newTime = chrono::system_clock::to_time_t(now);
    newData->moment = newTime;
    tm *daytime = localtime(&newData->moment);
    char buffer[100];
    strftime(buffer, sizeof(buffer), "%A, %d de %B de %Y - %H:%M:%S", daytime);

    cout << CYAN << "\nDATE" << RESET << endl;
    cout << YELLOW << buffer << RESET << endl;

    //////////////////////////////////////////////////////////////////////// OPERATING SYSTEM RETRIEVING
    ifstream systemOp("/etc/os-release");
    // tinha uma string systemOp_name aqui
    // tinha uma string systemOp_version aqui
    string line;

    while (getline(systemOp, line))
    {
        if (line.rfind("NAME=", 0) == 0)
        {
            string systemOp_name = line.substr(line.find('=') + 1);

            if (!systemOp_name.empty() && systemOp_name.front() == '"')
                systemOp_name = systemOp_name.substr(1, systemOp_name.size() - 2);
            newData->system.systemName = systemOp_name;
            break;
        }
    }

    while (getline(systemOp, line))
    {
        if (line.rfind("VERSION=", 0) == 0)
        {
            string systemOp_version = line.substr(line.find('=') + 1);

            if (!systemOp_version.empty() && systemOp_version.front() == '"')
                systemOp_version = systemOp_version.substr(1, systemOp_version.size() - 2);

            newData->system.version = systemOp_version;
            break;
        }
    }

    cout << CYAN << "\nOPERATING SYSTEM" << RESET << endl;
    cout << YELLOW << newData->system.systemName << " " << newData->system.version << RESET << endl;

    //////////////////////////////////////////////////////////////////////// CPU DATA RETRIEVING
    ifstream cpu("/proc/cpuinfo");

    // OBTAIN CPU LABEL AND MODEL
    while (getline(cpu, line))
    {
        if (line.find("model name") != string::npos)
        {
            string dado = line.substr(line.find(":") + 2);
            newData->processor.model = dado;
            break;
        }
    }

    // OBTAIN CORES NUMBER
    while (getline(cpu, line))
    {
        if (line.find("cpu cores") != string::npos)
        {
            int cores = std::stoi(line.substr(line.find(":") + 2));
            newData->processor.cores = cores;
            break;
        }
    }

    // OBTAIN CPU THREADS
    int threads = sysconf(_SC_NPROCESSORS_ONLN);
    newData->processor.threads = threads;

    cout << CYAN << "\nPROCESSOR" << RESET << endl;
    cout << YELLOW << newData->processor.model << " Cores: " << newData->processor.cores << " Threads: " << newData->processor.threads << RESET << endl;

    //////////////////////////////////////////////////////////////////////// MOTHERBOARD DATA RETRIEVING
    ifstream motherboard_brand("/sys/class/dmi/id/board_vendor");
    ifstream motherboard_model("/sys/class/dmi/id/board_name");

    // OBTAIN MOBA LABEL
    getline(motherboard_brand, line);
    newData->motherboard.label = line;

    // OBTAIN MOBA MODEL
    getline(motherboard_model, line);
    newData->motherboard.model = line;

    cout << CYAN << "\nMOTHERBOARD" << RESET << endl;
    cout << YELLOW << newData->motherboard.label << " " << newData->motherboard.model << RESET << endl;

    //////////////////////////////////////////////////////////////////////// RAM RETRIEVING
    ifstream RAMmemory("/proc/meminfo");
    while (getline(RAMmemory, line))
    {
        if (line.find("MemTotal") != string::npos)
        {
            long kb = stol(line.substr(line.find(":") + 1));
            int gb = (kb / (1024 * 1024)) + 1;
            newData->memory.capacity = gb;
        }
    }

    newData->memory.speed = 0;

    cout << CYAN << "\nRAM MEMORY" << RESET << endl;
    cout << YELLOW << newData->memory.capacity << " GB " << newData->memory.speed << " MHz" << RESET << endl;

    //////////////////////////////////////////////////////////////////////// GPU RETRIEVING
    newData->gpu.model = "Unknown";
    newData->gpu.capacity = 0;

    cout << CYAN << "\nGPU" << RESET << endl;
    cout << YELLOW << newData->gpu.model << " " << newData->gpu.capacity << " GB" << RESET << endl;

    //////////////////////////////////////////////////////////////////////// JSON FILE CREATION

    char opcao;
    cout << "\n\nDo you wanna save this information? [" << GREEN << "Y" << RESET << "/" << RED << "N" << RESET << "]\t";
    cin >> opcao;

    if (opcao != 'y' && opcao != 'Y')
    {
        system("clear");
        return;
    }
    cin.ignore();
    string fileName;
    cout << "\nType the desired file name (no spaces): \t";
    getline(cin, fileName);
    string fileNameWithExtention = storageFolder + "/" + fileName + ".json";
    ofstream file(fileNameWithExtention);

    if (!file.is_open())
    {
        cerr << RED << "\nFailed to create the file." << RESET << endl;
        return;
    }

    json data;
    data["moment"] = newData->moment;
    data["operating_system"] = newData->system.systemName;
    data["operating_system_version"] = newData->system.version;
    data["processor_model"] = newData->processor.model;
    data["processor_cores"] = newData->processor.cores;
    data["processor_threads"] = newData->processor.threads;
    data["motherboard_label"] = newData->motherboard.label;
    data["motherboard_model"] = newData->motherboard.model;
    data["ram_capacity"] = newData->memory.capacity;
    data["ram_speed"] = newData->memory.speed;
    data["gpu_model"] = newData->gpu.model;
    data["gpu_capacity"] = newData->gpu.capacity;

    file << data.dump(10);
    file.close();

    cout << "\nFile created sucessfully." << endl;
    delay(200);
    clearTerminal();
#endif
}

void mainMenu()
{
    cout << CYAN << "==========================================" << RESET << endl;
    cout << BOLD << "                systemSnap                " << RESET << endl;
    cout << CYAN << "==========================================" << RESET << endl;
    cout << " [" << GREEN << "1" << RESET << "] System Snapshot" << endl;
    cout << " [" << GREEN << "2" << RESET << "] Show Data" << endl;
    cout << " [" << RED << "0" << RESET << "] Exit" << endl;
    cout << CYAN << "------------------------------------------" << RESET << endl;
    cout << YELLOW << " >> Choose an option: " << RESET;
}

int main()
{
    encoding();
    int op;

    while (true)
    {
        clearTerminal();
        mainMenu();

        if (!(cin >> op)) // input validation
        {
            cin.clear();
            cin.ignore(1000, '\n');
            op = -1;
        }

        switch (op)
        {
        case 1:
            clearTerminal();
            cout << BOLD << "Initializing deep data collection..." << RESET << endl;
            collectData();
            break;

        case 2:
            clearTerminal();
            cout << BOLD << "Loading data..." << RESET << endl;
            checksForJsonExistence(storageFolder);
            break;

        case 0:
            cout << "\nEnding program";
            for (int i = 0; i < 3; i++)
            { // Um feedback visual de carregamento mais limpo
                delay(50);
                cout << ".";
            }
            clearTerminal();
            return 0;
        default:
            cout << RED << "\n[!] Invalid input." << RESET << endl;
            askForEnter();
            break;
        }
    }
}