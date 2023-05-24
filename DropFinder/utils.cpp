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
 * @param data_columns cantidad de columnas de datos (la primera es el tiempo) en el archivo .lvm
 * @return std::vector<LVM_DATUM> con los datos del archivo .lvm
 */
std::vector<LVM_DATUM> read_lvm_file(fs::path lvm_file, uint data_columns)
{

    std::vector<LVM_DATUM> data;

    std::vector<std::string> lines = read_file(lvm_file);

    for (const std::string &line : lines)
    {
        std::vector<std::string> values = split(line, '\t');

        std::string time = values[0];
        // remove the "." and the padded zeros from the time
        time.erase(std::remove(time.begin(), time.end(), '.'), time.end());

        std::vector<double> data_values(data_columns);
        for (uint i = 1; i < values.size(); i++)
        {

            data_values[i - 1] = std::stod(values[i]);
        }
        data.push_back({std::stoull(time), data_values});
    }

    return data;
}

/**
 * Lee un archivo .dat y devuelve un vector de DAT_DATUM
 *
 * @param dat_file path al archivo .dat
 * @param data_columns cantidad de columnas de datos en el archivo .dat
 * @return std::vector<DAT_DATUM> con los datos del archivo .dat
 */
std::vector<DAT_DATUM> read_dat_file(fs::path dat_file, uint data_columns)
{

    std::vector<DAT_DATUM> data;

    std::vector<std::string> lines = read_file(dat_file);

    for (const std::string &line : lines)
    {
        std::vector<std::string> values = split(line, '\t');

        std::vector<double> data_values(data_columns);
        for (uint i = 0; i < values.size(); i++)
        {

            data_values[i] = std::stod(values[i]);
        }
        data.push_back({data_values});
    }

    return data;
}

/**
 * Lee las primeras k lineas de un archivo .dat y devuelve un vector de DAT_DATUM
 *un
 * @param dat_file path al archivo .dat
 * @param data_columns cantidad de columnas de datos en el archivo .dat
 * @param k cantidad de lineas a leer
 * @return std::vector<DAT_DATUM> con los datos del archivo .dat
 */
std::vector<DAT_DATUM> read_dat_file_first_k_lines(fs::path dat_file, uint data_columns, uint k)
{

    std::vector<DAT_DATUM> data;

    std::ifstream file(dat_file);

    if (!file.is_open())
    {
        std::cerr << "Error opening file " << dat_file << '\n';
        exit(1);
    }

    std::string line;

    for (uint i = 0; i < k; i++)
    {
        std::getline(file, line);
        std::vector<std::string> values = split(line, '\t');

        std::vector<double> data_values(data_columns);
        for (uint i = 0; i < values.size(); i++)
        {

            data_values[i] = std::stod(values[i]);
        }
        data.push_back({data_values});
    }

    file.close();
    return data;
}

/**
 * Lee las ultimas k lineas de un archivo .dat y devuelve un vector de DAT_DATUM
 *un
 * @param dat_file path al archivo .dat
 * @param data_columns cantidad de columnas de datos en el archivo .dat
 * @param k cantidad de lineas a leer
 * @return std::vector<DAT_DATUM> con los datos del archivo .dat
 */
std::vector<DAT_DATUM> read_dat_file_last_k_lines(fs::path dat_file, uint data_columns, uint k)
{
    std::vector<DAT_DATUM> data;
    std::ifstream file(dat_file, std::ios::in);
    std::string line;

    if (!file.is_open())
    {
        std::cerr << "Error opening file " << dat_file << '\n';
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
        std::vector<std::string> values = split(line, '\t');

        std::vector<double> data_values(data_columns);
        for (uint i = 0; i < values.size(); i++)
        {

            data_values[i] = std::stod(values[i]);
        }
        data.push_back({data_values});
        file.seekg(-1, std::ios::cur);
    }

    file.close();
    return data;
}

/**
 * Escribe un .dat
 *
 * @param dat_file path al archivo .dat
 * @param std::vector<DAT_DATUM> con los datos del archivo .dat
 */
void write_dat_file(fs::path dat_file, std::vector<DAT_DATUM> &data)
{

    std::ofstream file(dat_file);

    if (!file.is_open())
    {
        std::cerr << "Error opening file " << dat_file << '\n';
        exit(1);
    }

    file << std::scientific << std::setprecision(8);

    for (DAT_DATUM datum : data)
    {
        for (uint i = 0; i < datum.var.size(); i++)
        {
            file << datum.var[i] << "\t\n"[i == datum.var.size() - 1];
        }
    }

    file.close();
}
