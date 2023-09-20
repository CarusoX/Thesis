#include "utils.h"

/**
 * Divide un string en un vector de strings usando un delimitador
 *
 * @param str string a dividir
 * @param delimiter delimitador
 * @return std::vector<std::string> con los strings resultantes
 */
std::vector<std::string> split(std::string str, char delimiter)
{
    std::vector<std::string> result;
    std::string current = "";
    for (char c : str)
    {
        if (c == delimiter)
        {
            result.push_back(current);
            current = "";
        }
        else
        {
            current += c;
        }
    }
    result.push_back(current);
    return result;
}

/**
 * Dada una linea de un archivo .lvm, devuelve un LVM_DATUM
 *
 * @param line linea del archivo .lvm
 * @return LVM_DATUM con los datos de la linea
 */
LVM_DATUM parse_lvm_line(std::string line)
{
    std::vector<std::string> parts = split(line, '\t');
    LVM_DATUM datum;

    // remove the "." and the padded zeros from the time
    std::string time = parts[0];
    time.erase(std::remove(time.begin(), time.end(), '.'), time.end());

    datum.time = std::stoull(time);
    for (uint i = 1; i < parts.size(); i++)
    {
        datum.var.push_back(std::stod(parts[i]));
    }
    return datum;
}

/**
 * Lee un archivo y lo separa en un vector de strings por cada linea
 *
 * @param file path al archivo
 * @return std::vector<std::string> con las lineas del archivo
 */
std::vector<std::string> read_file(fs::path file)
{
    std::ifstream f(file);

    if (!f.is_open())
    {
        std::cerr << "Error opening file " << file << '\n';
        exit(1);
    }

    std::string content(std::istreambuf_iterator<char>(f), (std::istreambuf_iterator<char>()));
    f.close();
    std::vector<std::string> lines = split(content, '\n');
    lines.pop_back();
    return lines;
}

/**
 * Lee un archivo .lvm y devuelve un vector de LVM_DATUM
 *
 * @param lvm_file path al archivo .lvm
 * @return std::vector<LVM_DATUM> con los datos del archivo .lvm
 */
std::vector<LVM_DATUM> read_lvm_file(fs::path lvm_file)
{

    std::vector<LVM_DATUM> data;

    std::vector<std::string> lines = read_file(lvm_file);

    for (const std::string &line : lines)
    {
        data.push_back(parse_lvm_line(line));
    }

    return data;
}

/**
 * Lee las primeras k lineas de un archivo .lvm y devuelve un vector de LVM_DATUM
 *un
 * @param lvm_file path al archivo .lvm
 * @param k cantidad de lineas a leer
 * @return std::vector<LVM_DATUM> con los datos del archivo .lvm
 */
std::vector<LVM_DATUM> read_lvm_file_first_k_lines(fs::path lvm_file, uint k)
{

    std::vector<LVM_DATUM> data;

    std::ifstream file(lvm_file);

    if (!file.is_open())
    {
        std::cerr << "Error opening file " << lvm_file << '\n';
        exit(1);
    }

    std::string line;

    for (uint i = 0; i < k; i++)
    {
        std::getline(file, line);
        data.push_back(parse_lvm_line(line));
    }

    file.close();
    return data;
}

/**
 * Lee las ultimas k lineas de un archivo .lvm y devuelve un vector de LVM_DATUM
 *un
 * @param lvm_file path al archivo .lvm
 * @param k cantidad de lineas a leer
 * @return std::vector<LVM_DATUM> con los datos del archivo .lvm
 */
std::vector<LVM_DATUM> read_lvm_file_last_k_lines(fs::path lvm_file, uint k)
{
    std::vector<LVM_DATUM> data;
    std::ifstream file(lvm_file, std::ios::in);
    std::string line;

    if (!file.is_open())
    {
        std::cerr << "Error opening file " << lvm_file << '\n';
        exit(1);
    }
    file.seekg(-2, std::ios::end);

    for (uint i = 0; i < k; i++)
    {
        std::string line;
        while (file.peek() != '\n')
        {
            line.push_back(file.peek());
            file.seekg(-1, std::ios::cur);
        }
        std::reverse(line.begin(), line.end());
        data.push_back(parse_lvm_line(line));
        file.seekg(-1, std::ios::cur);
    }

    file.close();
    return data;
}

/**
 * Escribe un .lvm
 *
 * @param lvm_file path al archivo .lvm
 * @param std::vector<LVM_DATUM> con los datos del archivo .lvm
 */
void write_lvm_file(fs::path lvm_file, std::vector<LVM_DATUM> &data)
{
    std::ofstream file(lvm_file);

    if (!file.is_open())
    {
        std::cerr << "Error opening file " << lvm_file << '\n';
        exit(1);
    }

    file << std::scientific << std::setprecision(8);

    for (uint i = 0; i < data.size(); i++)
    {
        file << data[i].time << "\t";
        for (uint j = 0; j < data[i].var.size(); j++)
        {
            file << data[i].var[j] << "\t\n"[j == data[i].var.size() - 1];
        }
    }

    file.close();
}
