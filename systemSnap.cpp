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
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define GREEN   "\033[32m"
#define CYAN    "\033[36m"
#define YELLOW  "\033[33m"
#define RED     "\033[31m"
using namespace std;
using json = nlohmann::json;

const char *dias[] = {
    "domingo", "segunda-feira", "terça-feira",
    "quarta-feira", "quinta-feira", "sexta-feira", "sábado"};

const char *meses[] = {
    "janeiro", "fevereiro", "março", "abril",
    "maio", "junho", "julho", "agosto",
    "setembro", "outubro", "novembro", "dezembro"};

#ifdef _WIN32
typedef LONG(WINAPI *RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
#endif

struct Processador
{
    string modelo;
    int nucleos;
    int threads;
};

struct Sistema
{
    string sistema;
    string version;
};

struct Motherboard
{
    string fabricante;
    string modelo;
};

struct Memoria
{
    float quantidade;
    float velocidade;
};

struct GPU
{
    string modelo;
    int memoria;
};

struct Dados
{
    time_t coleta;
    Sistema sistema;
    Processador processador;
    Motherboard motherboard;
    Memoria memoria;
    GPU gpu;
};

string pasta = "PCs";

void encoding()
{
#ifdef _WIN32
    // Configuração para Windows
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#else
    // Configuração para Linux/Mac
    locale::global(locale(""));
    wcout.imbue(locale());
#endif
}

void limparTela()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

bool checkfolderexistence()
{
    if (filesystem::is_directory(pasta))
    {
        return true;
    }
    else
    {
        filesystem::create_directory(pasta);
    }
    return false;
}

void loadingListas()
{
    limparTela();
    cout << "Analisando dados disponíveis..." << endl;
    this_thread::sleep_for(chrono::milliseconds(250));
    limparTela();
    return;
}

void pausar()
{
    cout << "\nPressione Enter para continuar...";
    // Limpa o buffer caso haja algo acumulado
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    // Espera o próximo Enter
    cin.get();
    limparTela();
}

void criarListaJson()
{
    loadingListas();
    cout << "ARQUIVOS DISPONÍVEIS" << endl;
    vector<string> arquivosJson;
    for (const auto &entry : filesystem::directory_iterator(pasta))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".json")
        {
            arquivosJson.push_back(entry.path().filename().string());
        }
    }

    int cont = 1;

    for (const auto &nome : arquivosJson)
    {
        cout << "\n"
             << setw(2) << setfill('0') << cont << " | " << nome;
        cont++;
    }
    int escolha;
    cout << "\n\nEscolha qual configuração deseja visualizar [0 para voltar]:  \n";
    cin >> escolha;
    if (escolha == 0)
    {
        limparTela();
        return;
    }
    if (escolha >= cont)
    {
        cout << "\nArquivo inexistente." << endl;
        pausar();
        limparTela();
        return;
    }
    int index = escolha - 1;

    limparTela();

    ifstream arquivo(pasta + "/" + arquivosJson[index]);

    cout << "PC\n"
         << arquivosJson[index] << "\n";

    if (!arquivo.is_open())
    {
        cerr << "\nErro ao abrir o arquivo." << endl;
        return;
    }

    json j;
    arquivo >> j;

    // configurar data para exibição
    time_t data = j["data_coleta"];
    tm *dt = localtime(&data);

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

    // configurar sistema operacional para exibição
    string sistemaOperacional = j["sistema_operacional"];
    string versao_so = j["versao_so"];
    string printSistema = sistemaOperacional + " " + versao_so;
    cout << "\nSISTEMA OPERACIONAL\n"
         << printSistema << endl;

    // configurar processador para exibição
    string modeloProcessador = j["processador_modelo"];
    int cores = j["processador_cores"];
    int threads = j["processador_threads"];

    cout << "\nPROCESSADOR\n"
         << modeloProcessador << " Cores: " << cores << " Threads: " << threads << endl;

    // configurar placa mãe para exibição
    string moblabel = j["mainboard_label"];
    string mobmodel = j["mainboard_model"];

    cout << "\nPLACA-MÃE\n"
         << moblabel << " " << mobmodel << endl;

    // configurar memória ram para exibição
    float memoria = j["memoria_capacidade"];
    float memoriavelocidade = j["memoria_velocidade"];

    cout << "\nMEMÓRIA RAM\n"
         << memoria << " GB " << memoriavelocidade << " MHz" << endl;

    // configurar GPU para exibição
    string modelo = j["gpu_modelo"];
    unsigned long long capacidade = j["gpu_memoria"];

    cout << "\nPLACA DE VÍDEO\n"
         << modelo << " " << capacidade << " GB" << endl;

    pausar();
    return;
}

void existejson(const string &pasta)
{
    bool encontrado = false;
    checkfolderexistence();

    try
    {
        for (const auto &entrada : filesystem::directory_iterator(pasta))
        {
            if (entrada.is_regular_file() &&
                entrada.path().extension() == ".json")
            {
                encontrado = true;
                break; // já encontrou, não precisa continuar
            }
        }
    }
    catch (const filesystem::filesystem_error &)
    {
        cerr << "\nErro ao acessar a pasta. Verifique se ela existe." << endl;
        return;
    }

    if (encontrado)
    {
        criarListaJson();
    }
    else
    {
        cout << "\nNão há nenhum registro salvo." << endl;
        pausar();
    }
}

void intervalo()
{
    this_thread::sleep_for(chrono::milliseconds(50));
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
    cout << "Obtendo informações do sistema operacional..." << endl;
    intervalo();
    cout << "Obtendo informações do processador..." << endl;
    intervalo();
    cout << "Obtendo informações da placa-mãe..." << endl;
    intervalo();
    cout << "Obtendo informações de memória-ram..." << endl;
    intervalo();
    cout << "Obtendo informações da placa de vídeo..." << endl;
    intervalo();
    cout << "Compilando informações..." << endl;
    intervalo();
    cout << "Pronto!" << endl;
    intervalo();

    return;
}

void coletarDados()
{
    Dados *novo = new Dados;

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
    intervalo();
    intervalo();
    intervalo();
    intervalo();
    limparTela();

#else
    mensagensloading();
    limparTela();
    //////////////////////////////////////////////////////////////////////// COLETA DATA/HORA
    setlocale(LC_TIME, "pt-BR.UTF-8");
    auto agora = chrono::system_clock::now();
    time_t tempo = chrono::system_clock::to_time_t(agora);
    novo->coleta = tempo;
    tm *dataHora = localtime(&novo->coleta);
    char buffer[100];
    strftime(buffer, sizeof(buffer), "%A, %d de %B de %Y - %H:%M:%S", dataHora);

    cout << "\nDATA DA COLETA\n";
    cout << buffer << endl;

    //////////////////////////////////////////////////////////////////////// COLETA SISTEMA
    ifstream sistema("/etc/os-release");
    string nomesistema;
    string versao;
    string line;

    while (getline(sistema, line))
    {
        if (line.rfind("NAME=", 0) == 0)
        {
            string nomesistema = line.substr(line.find('=') + 1);

            if (!nomesistema.empty() && nomesistema.front() == '"')
                nomesistema = nomesistema.substr(1, nomesistema.size() - 2);
            novo->sistema.sistema = nomesistema;
            break;
        }
    }

    while (getline(sistema, line))
    {
        if (line.rfind("VERSION=", 0) == 0)
        {
            string versao = line.substr(line.find('=') + 1);

            if (!versao.empty() && versao.front() == '"')
                versao = versao.substr(1, versao.size() - 2);

            novo->sistema.version = versao;
            break;
        }
    }

    cout << "\nSISTEMA OPERACIONAL\n"
         << novo->sistema.sistema << " " << novo->sistema.version << endl;

    //////////////////////////////////////////////////////////////////////// COLETA CPU
    ifstream cpu("/proc/cpuinfo");

    // obtem modelo do cpu
    while (getline(cpu, line))
    {
        if (line.find("model name") != string::npos)
        {
            string dado = line.substr(line.find(":") + 2);
            novo->processador.modelo = dado;
            break;
        }
    }

    // obtem numero de cores
    while (getline(cpu, line))
    {
        if (line.find("cpu cores") != string::npos)
        {
            int cores = std::stoi(line.substr(line.find(":") + 2));
            novo->processador.nucleos = cores;
            break;
        }
    }

    // obtem numero de threads
    int threads = sysconf(_SC_NPROCESSORS_ONLN);
    novo->processador.threads = threads;

    cout << "\nPROCESSADOR\n"
         << novo->processador.modelo << " Cores: " << novo->processador.nucleos << " Threads: " << novo->processador.threads << endl;

    //////////////////////////////////////////////////////////////////////// COLETA MAINBOARD
    ifstream motherboard_brand("/sys/class/dmi/id/board_vendor");
    ifstream motherboard_model("/sys/class/dmi/id/board_name");

    // obtem marca da moba
    getline(motherboard_brand, line);
    novo->motherboard.fabricante = line;

    // obtem modelo da moba
    getline(motherboard_model, line);
    novo->motherboard.modelo = line;

    cout << "\nPLACA-MÃE\n"
         << novo->motherboard.fabricante << " " << novo->motherboard.modelo << endl;

    //////////////////////////////////////////////////////////////////////// COLETA MEMORIA
    ifstream memoria("/proc/meminfo");
    while (getline(memoria, line))
    {
        if (line.find("MemTotal") != string::npos)
        {
            long kb = stol(line.substr(line.find(":") + 1));
            int gb = (kb / (1024 * 1024)) + 1;
            novo->memoria.quantidade = gb;
        }
    }

    novo->memoria.velocidade = 0;

    cout << "\nMEMÓRIA RAM\n"
         << novo->memoria.quantidade << " GB " << novo->memoria.velocidade << " MHz" << endl;

    novo->gpu.memoria = 0;
    novo->gpu.modelo = "Desconhecido";

    //////////////////////////////////////////////////////////////////////// COLETA GPU
    novo->gpu.modelo = "Informações indisponíveis";
    novo->gpu.memoria = 0;

    cout << "\nGPU\n"
         << novo->gpu.modelo << " " << novo->gpu.memoria << " GB" << endl;

    //////////////////////////////////////////////////////////////////////// CRIAÇÃO ARQUIVO

    char opcao;
    cout << "\n\nVocê deseja salvar esses dados? [S/N] \t";
    cin >> opcao;

    if (opcao != 's' && opcao != 'S')
    {
        system("clear");
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
    intervalo();
    intervalo();
    intervalo();
    intervalo();
    limparTela();
#endif
}

void exibirMenu() {
    cout << CYAN << "==========================================" << RESET << endl;
    cout << BOLD << "                systemSnap                " << RESET << endl;
    cout << CYAN << "==========================================" << RESET << endl;
    cout << " [" << GREEN << "1" << RESET << "] Exibir Dados Disponíveis" << endl;
    cout << " [" << GREEN << "2" << RESET << "] Analisar Hardware" << endl;
    cout << " [" << RED << "0" << RESET << "] Sair do Programa" << endl;
    cout << CYAN << "------------------------------------------" << RESET << endl;
    cout << YELLOW << " >> Escolha uma opção: " << RESET;
}


int main() {
    encoding();
    int op;

    while (true) {
        limparTela();
        exibirMenu();

        // Validação simples para evitar que o programa quebre se o usuário digitar uma letra
        if (!(cin >> op)) {
            cin.clear(); 
            cin.ignore(1000, '\n');
            op = -1; 
        }

        switch (op) {
            case 1:
                limparTela();
                cout << BOLD << "Carregando dados..." << RESET << endl;
                existejson(pasta);
                pausar(); // Importante para o usuário ler o resultado
                break;
            case 2:
                limparTela();
                cout << BOLD << "Iniciando análise profunda..." << RESET << endl;
                coletarDados();
                pausar();
                break;
            case 0:
                cout << "\nEncerrando programa";
                for(int i=0; i<3; i++) { // Um feedback visual de carregamento mais limpo
                    intervalo(); cout << ".";
                }
                limparTela();
                return 0;
            default:
                cout << RED << "\n[!] Opção inválida. Tente novamente." << RESET << endl;
                pausar();
                break;
        }
    }
}