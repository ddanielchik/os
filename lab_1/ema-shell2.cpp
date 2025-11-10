#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <windows.h>
#include <direct.h>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <filesystem>

using namespace std;
using namespace std::chrono;

vector<string> split_command(const string& input) {
    vector<string> args;
    stringstream ss(input);
    string token;
    
    while (ss >> token) {
        args.push_back(token);
    }
    
    return args;
}

string get_current_directory() {
    char buffer[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, buffer);
    return string(buffer);
}

void ema_sort_int(const vector<string>& args) {
    if (args.size() < 2) {
        cout << "Использование: ema-sort-int [числа...]" << endl;
        return;
    }
    
    vector<int> numbers;
    for (size_t i = 1; i < args.size(); i++) {
        try {
            numbers.push_back(stoi(args[i]));
        } catch (...) {
            cout << "Ошибка: '" << args[i] << "' не является числом" << endl;
            return;
        }
    }
    
    sort(numbers.begin(), numbers.end());
    
    cout << "Отсортированные числа: ";
    for (int num : numbers) {
        cout << num << " ";
    }
    cout << endl;
}

void short_path_command(const vector<string>& args) {
    char short_path_buf[MAX_PATH];
    
    if (args.size() > 1) {
        // Преобразуем указанный путь
        if (GetShortPathNameA(args[1].c_str(), short_path_buf, MAX_PATH) > 0) {
            cout << short_path_buf << endl;
        } else {
            cout << "Ошибка: не удалось преобразовать путь '" << args[1] << "'" << endl;
        }
    } else {
        // Без аргументов - текущая директория
        char current_dir[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, current_dir);
        if (GetShortPathNameA(current_dir, short_path_buf, MAX_PATH) > 0) {
            cout << short_path_buf << endl;
        } else {
            cout << current_dir << endl;
        }
    }
}

bool execute_exe(const vector<string>& args) {
    if (args.empty()) return false;
    
    // Собираем командную строку
    string command_line;
    for (const auto& arg : args) {
        if (arg.find(' ') != string::npos) {
            command_line += "\"" + arg + "\" ";
        } else {
            command_line += arg + " ";
        }
    }
    
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    
    // Запускаем процесс
    if (CreateProcessA(
        NULL,                   // Имя модуля (используем командную строку)
        (LPSTR)command_line.c_str(), // Командная строка
        NULL,                   // Дескриптор процесса не наследуется
        NULL,                   // Дескриптор потока не наследуется
        FALSE,                  // Не наследовать дескрипторы
        0,                      // Флаги создания
        NULL,                   // Использовать environment родителя
        NULL,                   // Использовать текущую директорию
        &si,                    // STARTUPINFO
        &pi                     // PROCESS_INFORMATION
    )) {
        // Ждем завершения процесса
        WaitForSingleObject(pi.hProcess, INFINITE);
        
        // Закрываем дескрипторы
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return true;
    }
    
    return false;
}

int main() {
    cout << "Добро пожаловать в EMA-Shell для Windows!" << endl;
    cout << "Введите 'exit' для выхода" << endl << endl;
    
    string input;
    
    while (true) {
        cout << "EMA-Shell> ";
        getline(cin, input);
        
        if (input == "exit") break;
        if (input.empty()) continue;
        
        vector<string> args = split_command(input);
        
        if (args.empty()) continue;
        
        // Встроенные команды
        if (args[0] == "cd") {
            if (args.size() > 1) {
                if (_chdir(args[1].c_str()) != 0) {
                    

                    cout << "Ошибка смены директории" << endl;
                }
            }
            continue;
        }
        
        if (args[0] == "pwd") {
            cout << get_current_directory() << endl;
            continue;
        }
        
        if (args[0] == "ema-sort-int") {
            ema_sort_int(args);
            continue;
        }

        if (args[0] == "short-path") {
            short_path_command(args);
            continue;
        }
        
        if (args[0] == "cls") {
            system("cls");
            continue;
        }

        if (args[0] == "ls") {
            std::filesystem::path p = (args.size() > 1) ? std::filesystem::path(args[1]) : std::filesystem::current_path();
            try {
                for (const auto& entry : std::filesystem::directory_iterator(p)) {
                    cout << entry.path().filename().string();
                    if (entry.is_directory()) cout << "/";
                    cout << endl;
                }
            } catch (const std::exception&) {
                cout << "Ошибка: не удалось перечислить " << p.string() << endl;
            }
            continue;
        }

        if (args[0] == "echo") {
            // ищем перенаправление '>' или '>>'
            bool append = false;
            string out_file;
            size_t redir_index = string::npos;

            for (size_t i = 1; i < args.size(); ++i) {
                if (args[i] == ">>") { append = true; redir_index = i; break; }
                if (args[i] == ">")  { append = false; redir_index = i; break; }
                // поддержка формы ">file" или ">>file"
                if (args[i].rfind(">>", 0) == 0 && args[i].size() > 2) { append = true; out_file = args[i].substr(2); redir_index = i; break; }
                if (args[i].rfind(">", 0) == 0  && args[i].size() > 1) { append = false; out_file = args[i].substr(1); redir_index = i; break; }
            }

            // если нашли токен '>' или '>>', возьмём имя файла (если не было в том же токене)
            if (redir_index != string::npos && out_file.empty()) {
                if (redir_index + 1 < args.size()) out_file = args[redir_index + 1];
                else {
                    cout << "Ошибка: не указано имя файла для перенаправления" << endl;
                    continue;
                }
            }

            // сформируем строку вывода (аргументы до redir_index или все аргументы)
            string output;
            size_t end = (redir_index == string::npos) ? args.size() : redir_index;
            for (size_t i = 1; i < end; ++i) {
                if (i > 1) output += " ";
                output += args[i];
            }

            // если файл не указан — вывод в консоль
            if (out_file.empty()) {
                cout << output << endl;
            } else {
                // записываем в файл (перезапись или дописывание)
                #include <fstream> // убедитесь, что этот include есть вверху файла
                std::ofstream ofs;
                if (append) ofs.open(out_file, std::ios::out | std::ios::app);
                else       ofs.open(out_file, std::ios::out | std::ios::trunc);

                if (!ofs) {
                    cout << "Ошибка: не удалось открыть файл '" << out_file << "'" << endl;
                } else {
                    ofs << output << std::endl;
                    ofs.close();
                }
            }
            continue;
        }

        
        bool executed = execute_exe(args);
        
        if (!executed) {
            // Если не удалось через CreateProcess, пробуем через system
            int result = system(input.c_str());
            executed = (result != -1);
        }
        
        if (args[0] == "help") {
            cout << "Встроенные команды:" << endl;
            cout << "  cd [dir]         — сменить директорию" << endl;
            cout << "  pwd              — показать текущую директорию" << endl;
            cout << "  ls [dir]         — перечислить файлы/папки" << endl;
            cout << "  echo [text...]   — вывести текст" << endl;
            cout << "  ema-sort-int     — сортировка целых (ema-sort-int )" << endl;
            cout << "  short-path       — короткий путь (short-path [path])" << endl;
            cout << "  cls              — очистить экран" << endl;
            cout << "  exit             — выход" << endl;
            cout << "  help             — это сообщение" << endl;
            continue;
        }
        
        if (executed) continue;
        // Внешние команды с измерением времени
        auto start = high_resolution_clock::now();
        int result = system(input.c_str());
        auto end = high_resolution_clock::now();
        
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Время выполнения: " 
             << duration.count() / 1000000.0 
             << " секунд" << endl;
    }
    
    cout << "До свидания!" << endl;
    return 0;
}