#include "clean.h"
#include "utils.h"

/**
 * Cambia el tiempo de todos los datos para empezar desde 0
 *
 * @param data vector con los datos
 * @return void
 */
void changeStartingTimeToZero(std::vector<LVM_DATUM> &data)
{
    ull start_time = data[0].time;
    for (uint i = 1; i < data.size(); i++)
    {
        data[i].time -= start_time;
    }
    data[0].time = 0;
}

/**
 * Divide el tiempo de todos los datos por 200 (cada dato es cada 0.0002s)
 *
 * @param data vector con los datos
 * @return void
 */
void divideTimesBy200(std::vector<LVM_DATUM> &data)
{
    for (uint i = 0; i < data.size(); i++)
    {
        data[i].time /= 200;
    }
}

/**
 * Completa los huecos en los datos
 * Esto se da cuando el tiempo de un dato no es consecutivo al anterior
 *
 * @param data vector con los datos
 * @return std::vector<LVM_DATUM> con los datos completados
 */
std::vector<LVM_DATUM> complete_gaps(std::vector<LVM_DATUM> &data)
{
    std::vector<LVM_DATUM> result;

    ull current_time = 0;

    for (uint i = 0; i < data.size(); i++)
    {
        if (data[i].time != current_time)
        {
            // Tenemos un hueco => [current_time, data[i].time)

            // Sacamos un promedio de los anteriores 100 puntos (ambas dimensiones)
            // y otro para los siguientes 1000 puntos
            std::pair<double, double> from = {result.back().var[0], result.back().var[1]};
            std::pair<double, double> to = {data[i].var[0], data[i].var[1]};

            // Completamos el hueco con una recta entre ambos promedios
            // avg_1 -> avg_3 // avg_2 -> avg_4
            double slope_1 = (to.first - from.first) / (data[i].time - current_time + 1);
            double slope_2 = (to.second - from.second) / (data[i].time - current_time + 1);

            for (uint j = current_time; j < data[i].time; j++)
            {
                double x = j - current_time + 1;
                double y = from.first + slope_1 * x;
                double z = from.second + slope_2 * x;
                result.push_back({j, {y, z, 0}});
            }
        }
        result.push_back(data[i]);
        result.back().var.push_back(1);
        current_time = data[i].time + 1;
    }

    return result;
}

/**
 * Realiza transformacion en los datos para finalmente completar los huecos
 * Guarda la nueva data en un archivo con el mismo nombre que el original pero con un
 * "_complete.lvm" al final
 *
 * @param directory directorio donde se encuentra el archivo .lvm
 * @param lvm_file nombre del archivo .lvm
 * @return void
 */
void fill_gaps_on_lvm_file(fs::path lvm_file)
{
    std::vector<LVM_DATUM> data = read_lvm_file(lvm_file);

    changeStartingTimeToZero(data);

    divideTimesBy200(data);

    std::vector<LVM_DATUM> lvm_data = complete_gaps(data);

    write_lvm_file(lvm_file.parent_path() / "data_complete.lvm", lvm_data);
}

/**
 * Calcula el promedio de los datos en una ventana de tamaño w
 * y lo guarda en el centro de la ventana
 *changeStartingTimeToZero
 * @param data vector con los datos
 * @param w tamaño de la ventana (debe ser par)
 * @return void
 */
void compute_dynamic_avg_on_lvm_file(fs::path lvm_file, fs::path lvm_file_left, fs::path lvm_file_right, uint w)
{

    if (w % 2 == 1)
    {
        std::cerr << "Window size must be even" << std::endl;
        exit(1);
    }

    std::vector<LVM_DATUM> data = read_lvm_file(lvm_file);

    std::vector<LVM_DATUM> left_data, right_data;
    if (lvm_file_left != "")
    {
        left_data = read_lvm_file_last_k_lines(lvm_file_left, w / 2);
    }

    if (lvm_file_right != "")
    {
        right_data = read_lvm_file_first_k_lines(lvm_file_right, w / 2);
    }

    std::vector<LVM_DATUM> all_data;
    all_data.insert(all_data.end(), left_data.begin(), left_data.end());
    all_data.insert(all_data.end(), data.begin(), data.end());
    all_data.insert(all_data.end(), right_data.begin(), right_data.end());

    std::vector<double> pref_a = {0}, pref_b = {0};

    for (uint i = 0; i < all_data.size(); i++)
    {
        pref_a.push_back(pref_a.back() + all_data[i].var[0]);
        pref_b.push_back(pref_b.back() + all_data[i].var[1]);
    }

    int qt_left = left_data.size();

    for (uint i = 0; i < data.size(); i++)
    {
        int index_left = i - w / 2 + qt_left;
        uint index_right = i + w / 2 + qt_left;

        if (index_left < 0 || index_right >= pref_a.size())
        {
            data[i].var = {0, 0};
            continue;
        }

        data[i].var[0] -= (pref_a[index_right] - pref_a[index_left]) / (w + 1);
        data[i].var[1] -= (pref_b[index_right] - pref_b[index_left]) / (w + 1);
    }

    write_lvm_file(lvm_file.parent_path() / "data_avg.lvm", data);
}
