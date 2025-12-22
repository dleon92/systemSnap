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
    cout << "AVAILABLE FILES" << endl;
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
             << setw(2) << setfill('0') << cont << " | " << name;
        cont++;
    }
    int choice;
    cout << "\n\nChoose which configuration you want to view [0 to go back]:  \n";
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

    cout << "PC\n"
         << jsonFile[index] << "\n";

    if (!file.is_open())
    {
        cerr << "\nFailed to open file." << endl;
        return;
    }

    json j;
    file >> j;

    // setup date input from json filme to exibit
    time_t date = j["data_coleta"];
    tm *dt = localtime(&date);

    cout
        << "\nCOLLECTION DATE\n"
        << dias[dt->tm_wday] << ", "
        << dt->tm_mday << " de "
        << meses[dt->tm_mon] << " de "
        << (dt->tm_year + 1900)
        << " - "
        << (dt->tm_hour < 10 ? "0" : "") << dt->tm_hour << ":"
        << (dt->tm_min < 10 ? "0" : "") << dt->tm_min << ":"
        << (dt->tm_sec < 10 ? "0" : "") << dt->tm_sec
        << endl;

    // setup operating system input from json filme to exibit
    string nameOperatingSystem = j["sistema_operacional"];
    string versionSo = j["versao_so"];
    string printSystem = nameOperatingSystem + " " + versionSo;
    cout << "\nOPERATING SYSTEM\n"
         << printSystem << endl;

    // setup processos input from json filme to exibit
    string processorModel = j["processador_modelo"];
    int cores = j["processador_cores"];
    int threads = j["processador_threads"];

    cout << "\nPROCESSADOR\n"
         << processorModel << " Cores: " << cores << " Threads: " << threads << endl;

    // setup motherboard input from json filme to exibit
    string moblabel = j["mainboard_label"];
    string mobmodel = j["mainboard_model"];

    cout << "\nMOTHERBOARD\n"
         << moblabel << " " << mobmodel << endl;

    // setup RAM input from json filme to exibit
    float memoryCapacity = j["memoria_capacidade"];
    float memorySpeed = j["memoria_velocidade"];

    cout << "\nRAM\n"
         << memoryCapacity << " GB " << memorySpeed << " MHz" << endl;

    // setup GPU input from json filme to exibit
    string gpuModel = j["gpu_modelo"];
    unsigned long long gpuCapcity = j["gpu_memoria"];

    cout << "\nGPU\n"
         << gpuModel << " " << gpuCapcity << " GB" << endl;

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
        cerr << "\nFailed to open folder." << endl;
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
string obterMainboardLabel()
{
    FILE *pipe = _popen("wmic baseboard get Manufacturer", "r");
    if (!pipe)
    {
        return "\nFalha ao obter informações.";
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

string obterMainboardModel()
{
    FILE *pipe = _popen("wmic baseboard get Product", "r");
    if (!pipe)
        return "Falha ao obter informações.";

    char buffer[256];
    string linha;

    // lê a primeira linha (cabeçalho)
    fgets(buffer, sizeof(buffer), pipe);

    // lê a segunda linha (valor alinhado)
    if (!fgets(buffer, sizeof(buffer), pipe))
    {
        _pclose(pipe);
        return "Desconhecido";
    }

    linha = buffer;

    _pclose(pipe);

    // remove espaços e quebras de linha
    linha.erase(0, linha.find_first_not_of(" \t\r\n"));
    linha.erase(linha.find_last_not_of(" \t\r\n") + 1);

    return linha;
}

vector<unsigned long long> obterCapacidadesRAM()
{
    vector<unsigned long long> capacites;

    FILE *pipe = _popen("wmic MEMORYCHIP get Capacity", "r");
    if (!pipe)
        return capacites;

    char buffer[256];

    // lê o cabeçalho e ignora
    fgets(buffer, sizeof(buffer), pipe);

    while (fgets(buffer, sizeof(buffer), pipe))
    {
        string linha = buffer;

        // trim de espaços e quebras de linha
        linha.erase(0, linha.find_first_not_of(" \t\r\n"));
        linha.erase(linha.find_last_not_of(" \t\r\n") + 1);

        if (linha.empty())
            continue; // ignora linhas vazias

        // converte para número
        try
        {
            unsigned long long valor = std::stoull(linha);
            capacites.push_back(valor);
        }
        catch (...)
        {
            // ignora linhas inválidas
        }
    }

    _pclose(pipe);
    return capacites;
}

int obterVelocidadeRAM()
{
    FILE *pipe = _popen("wmic MEMORYCHIP get Speed", "r");
    if (!pipe)
        return -1; // indica falha

    char buffer[256];

    // lê e ignora o cabeçalho
    if (!fgets(buffer, sizeof(buffer), pipe))
    {
        _pclose(pipe);
        return -1;
    }

    // lê a primeira linha com valor
    if (!fgets(buffer, sizeof(buffer), pipe))
    {
        _pclose(pipe);
        return -1;
    }

    _pclose(pipe);

    std::string linha = buffer;

    // remove espaços e quebras de linha
    linha.erase(0, linha.find_first_not_of(" \t\r\n"));
    linha.erase(linha.find_last_not_of(" \t\r\n") + 1);

    try
    {
        int velocidade = std::stoi(linha); // converte para int
        return velocidade;
    }
    catch (...)
    {
        return -1; // erro na conversão
    }
}

string obterGPU()
{
    FILE *pipe = _popen("wmic path win32_videocontroller get name", "r");
    if (!pipe)
        return "Desconhecido";

    char buffer[256];

    // lê e ignora o cabeçalho (primeira linha)
    if (!fgets(buffer, sizeof(buffer), pipe))
    {
        _pclose(pipe);
        return "Desconhecido";
    }

    // lê a segunda linha (primeira GPU)
    if (!fgets(buffer, sizeof(buffer), pipe))
    {
        _pclose(pipe);
        return "Desconhecido";
    }

    _pclose(pipe);

    string gpu = buffer;

    // trim de espaços e quebras de linha
    gpu.erase(0, gpu.find_first_not_of(" \t\r\n"));
    gpu.erase(gpu.find_last_not_of(" \t\r\n") + 1);

    return gpu;
}

int obterMemoriaGPU()
{
    FILE *pipe = _popen("wmic path win32_videocontroller get adapterram", "r");
    if (!pipe)
        return 0;

    char buffer[256];

    // ignora o cabeçalho
    if (!fgets(buffer, sizeof(buffer), pipe))
    {
        _pclose(pipe);
        return 0;
    }

    // lê a primeira linha com valor
    if (!fgets(buffer, sizeof(buffer), pipe))
    {
        _pclose(pipe);
        return 0;
    }

    _pclose(pipe);

    std::string linha = buffer;

    // trim
    linha.erase(0, linha.find_first_not_of(" \t\r\n"));
    linha.erase(linha.find_last_not_of(" \t\r\n") + 1);

    try
    {
        unsigned long long bytes = std::stoull(linha);

        float gb = bytes / (1024.0f * 1024.0f * 1024.0f);

        // arredonda para inteiro
        int gbInt = static_cast<int>(std::round(gb));

        return gbInt;
    }
    catch (...)
    {
        return 0;
    }
}
#endif

void mensagensloading()
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
    mensagensloading();
    limparTela();
    //////////////////////////////////////////////////////////////////////// COLETA DATA/HORA
    auto agora = chrono::system_clock::now();
    time_t tempo = chrono::system_clock::to_time_t(agora);
    novo->coleta = tempo;

    tm *dt = localtime(&tempo);

    cout
        << "\nDATA DA COLETA\n"
        << dias[dt->tm_wday] << ", "
        << dt->tm_mday << " de "
        << meses[dt->tm_mon] << " de "
        << (dt->tm_year + 1900)
        << " - "
        << (dt->tm_hour < 10 ? "0" : "") << dt->tm_hour << ":"
        << (dt->tm_min < 10 ? "0" : "") << dt->tm_min << ":"
        << (dt->tm_sec < 10 ? "0" : "") << dt->tm_sec
        << endl;

    //////////////////////////////////////////////////////////////////////// COLETA SISTEMA
    string sistemaOperacional = "Windows";
    int windowsVersion = 0;
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
                    windowsVersion = 7;
                else if (v.dwMajorVersion == 6 && v.dwMinorVersion == 2)
                    windowsVersion = 8;
                else if (v.dwMajorVersion == 6 && v.dwMinorVersion == 3)
                    windowsVersion = 8; // 8.1
                else if (v.dwMajorVersion == 10 && v.dwMinorVersion == 0)
                    windowsVersion = (v.dwBuildNumber >= 22000) ? 11 : 10;
            }
        }
    }

    string aaa = to_string(windowsVersion);
    novo->sistema.sistema = sistemaOperacional;
    novo->sistema.version = aaa;

    cout << "\nSISTEMA OPERACIONAL\n"
         << novo->sistema.sistema << " " << novo->sistema.version << endl;

    //////////////////////////////////////////////////////////////////////// COLETA CPU
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
        cerr << "\nFalha ao obter informações.";
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

    novo->processador.modelo = CPUname;
    novo->processador.nucleos = cores;
    novo->processador.threads = sysinfo.dwNumberOfProcessors;
    cout << "\nPROCESSADOR\n";
    cout << novo->processador.modelo << "\nCores: " << novo->processador.nucleos << "\nThreads: " << novo->processador.threads << endl;

    //////////////////////////////////////////////////////////////////////// COLETA MAINBOARD

    string fabricante = obterMainboardLabel();
    string modeloPlacaMae = obterMainboardModel();

    novo->motherboard.fabricante = fabricante;
    novo->motherboard.modelo = modeloPlacaMae;
    cout << "\nPLACA-MÃE\n";
    cout << novo->motherboard.fabricante << " " << novo->motherboard.modelo << endl;

    //////////////////////////////////////////////////////////////////////// COLETA MEMORIA

    auto ramCaps = obterCapacidadesRAM();
    unsigned long long totalRAM = 0;
    for (auto c : ramCaps)
        totalRAM += c;

    int ramVel = obterVelocidadeRAM();
    if (ramVel == -1)
        cout << "Falha ao obter velocidade da RAM\n";

    float capacidadeDeMemoria = totalRAM / (1024 * 1024 * 1024);
    novo->memoria.quantidade = capacidadeDeMemoria;
    novo->memoria.velocidade = ramVel;

    // cout << "Total RAM: " << totalRAM / (1024 * 1024 * 1024) << " GB\n";

    // cout << "Velocidade RAM: " << ramVel << " MHz\n";

    cout << "\nMEMÓRIA RAM\n";
    cout << novo->memoria.quantidade << " GB " << novo->memoria.velocidade << " MHz" << endl;

    //////////////////////////////////////////////////////////////////////// COLETA GPU
    string gpu = obterGPU();
    novo->gpu.modelo = gpu;
    float memoria = obterMemoriaGPU();
    novo->gpu.memoria = memoria;

    cout << "\nGPU\n";
    cout << novo->gpu.modelo << " " << novo->gpu.memoria << " GB" << endl;

    //////////////////////////////////////////////////////////////////////// CRIAÇÃO ARQUIVO

    char opcao;
    cout << "\n\nVocê deseja salvar esses dados? [S/N] \t";
    cin >> opcao;

    if (opcao != 's' && opcao != 'S')
    {
        system("cls");
        return;
    }
    cin.ignore();
    string nomeDoArquivo;
    cout << "\nDigite o nome do arquivo (sem espaços): \t";
    getline(cin, nomeDoArquivo);
    string nomeComExtensao = pasta + "/" + nomeDoArquivo + ".json";
    ofstream arquivo(nomeComExtensao);

    if (!arquivo.is_open())
    {
        cout << "\nErro ao criar arquivo.";
        return;
    }

    json dados;
    dados["data_coleta"] = novo->coleta;
    dados["sistema_operacional"] = novo->sistema.sistema;
    dados["versao_so"] = novo->sistema.version;
    dados["processador_modelo"] = novo->processador.modelo;
    dados["processador_cores"] = novo->processador.nucleos;
    dados["processador_threads"] = novo->processador.threads;
    dados["mainboard_label"] = novo->motherboard.fabricante;
    dados["mainboard_model"] = novo->motherboard.modelo;
    dados["memoria_capacidade"] = novo->memoria.quantidade;
    dados["memoria_velocidade"] = novo->memoria.velocidade;
    dados["gpu_modelo"] = novo->gpu.modelo;
    dados["gpu_memoria"] = novo->gpu.memoria;

    arquivo << dados.dump(10);
    arquivo.close();

    cout << "\nDados arquivados com sucesso." << endl;
    delay(200);
    limparTela();

#else
    mensagensloading();
    clearTerminal();
    //////////////////////////////////////////////////////////////////////// MOMENT RETRIEVING
    setlocale(LC_TIME, "pt-BR.UTF-8");
    auto now = chrono::system_clock::now();
    time_t newTime = chrono::system_clock::to_time_t(now);
    newData->moment = newTime;
    tm *daytime = localtime(&newData->moment);
    char buffer[100];
    strftime(buffer, sizeof(buffer), "%A, %d de %B de %Y - %H:%M:%S", daytime);

    cout << "\nDATE\n";
    cout << buffer << endl;

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

    cout << "\nOPERATING SYSTEM\n"
         << newData->system.systemName << " " << newData->system.version << endl;

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

    cout << "\nPROCESSOR\n"
         << newData->processor.model << " Cores: " << newData->processor.cores << " Threads: " << newData->processor.threads << endl;

    //////////////////////////////////////////////////////////////////////// MOTHERBOARD DATA RETRIEVING
    ifstream motherboard_brand("/sys/class/dmi/id/board_vendor");
    ifstream motherboard_model("/sys/class/dmi/id/board_name");

    // OBTAIN MOBA LABEL
    getline(motherboard_brand, line);
    newData->motherboard.label = line;

    // OBTAIN MOBA MODEL
    getline(motherboard_model, line);
    newData->motherboard.model = line;

    cout << "\nMOTHERBOARD\n"
         << newData->motherboard.label << " " << newData->motherboard.model << endl;

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

    cout << "\nRAM MEMORY\n"
         << newData->memory.capacity << " GB " << newData->memory.speed << " MHz" << endl;

    newData->gpu.capacity = 0;
    newData->gpu.model = "Unknown";

    //////////////////////////////////////////////////////////////////////// GPU RETRIEVING
    newData->gpu.model = "Unknown";
    newData->gpu.capacity = 0;

    cout << "\nGPU\n"
         << newData->gpu.model << " " << newData->gpu.capacity << " GB" << endl;

    //////////////////////////////////////////////////////////////////////// JSON FILE CREATION

    char opcao;
    cout << "\n\nDo you wanna save this information? [Y/N] \t";
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
        cerr << "\nFailed to create the file.";
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
        
        if (!(cin >> op)) //input validation
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
            askForEnter();
            break;

        case 2:
            clearTerminal();
            cout << BOLD << "Loading data..." << RESET << endl;
            checksForJsonExistence(storageFolder);
            askForEnter(); // Importante para o usuário ler o resultado
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