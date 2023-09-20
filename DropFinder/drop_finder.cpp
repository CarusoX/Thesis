#include "drop_finder.h"

std::vector<double> generate_thresholds(uint ps, double c1, double c2, double c3)
{
    std::vector<double> thresholds;
    for (uint k = 1; k <= ps; k++)
    {
        double umbral = pow((1.0 - 1.0 / double(k)), c3) * (c2 - c1) / pow((1.0 - 1.0 / double(ps)), c3) + c1;
        thresholds.push_back(umbral);
    }
    return thresholds;
}

uint find_pulse_start(std::vector<double> &var, uint i, uint nn, bool is_negative)
{
    // El pulso empieza en la posicion mas proxima a i hacia la izq
    // tal que var[w] < 0 si es positivo y tal que var[w] > 0 si es negativo
    // (en caso de no haber, empieza `nn` posiciones atras)
    uint u = std::max(0, int(i - nn));
    for (int w = i - 1; w >= std::max(0, int(i - nn)); w--)
    {
        if ((!is_negative && var[w] < 0) || (is_negative && var[w] > 0))
        {
            u = w;
            break;
        }
    }
    return u;
}

std::tuple<uint, uint, uint, uint>
process_pulse(std::vector<double> &var_a,
              std::vector<double> &var_b,
              uint i,
              uint j,
              uint nn)
{

    // Nos fijamos si el pulso es negativo
    bool is_negative = var_a[i] < 0;

    // Encontramos el inicio del primer pulso
    uint u1 = find_pulse_start(var_a, i, nn, is_negative);

    // Encontramos el inicio del segundo pulso
    uint u2 = find_pulse_start(var_b, j, nn, is_negative);

    // No puede ocurrir que el inicio del segundo pulso sea anterior al inicio del primero,
    // asi que si eso ocurre, los forzamos a ser iguales
    if (u2 < u1)
    {
        u2 = u1;
    }

    // Buscamos el punto medio del primer pulso, este se da cuando encontramos
    // 2 posiciones consecutivas menores a 0
    uint p1 = i + 3 * nn;
    for (uint p = i; p <= i + 3 * nn; p++)
    {
        bool is_positive_condition = !is_negative && var_a[p] < 0 && var_a[p + 1] < 0;
        bool is_negative_condition = is_negative && var_a[p] > 0 && var_a[p + 1] > 0;
        if (is_positive_condition || is_negative_condition)
        {
            p1 = p;
            break;
        }
    }

    // Buscamos el punto de quiebre en la integral del segundo pulso
    uint p2 = j + 4 * nn;
    double integral = 0;
    for (uint p = 1; p <= 4 * nn; p++)
    {
        integral -= var_b[u2 + p];
        bool is_positive_condition = !is_negative && integral < -0.1;
        bool is_negative_condition = is_negative && integral > 0.1;
        if (is_positive_condition || is_negative_condition)
        {
            double suma = 0;
            for (uint r = p; p <= 4 * nn; r++)
            {
                suma += var_b[u2 + r];
                bool is_positive_condition = !is_negative && var_b[u2 + r + 1] < 0.3 * suma / (r - p + 1);
                bool is_negative_condition = is_negative && var_b[u2 + r + 1] > 0.3 * suma / (r - p + 1);
                if (is_positive_condition || is_negative_condition)
                {
                    p2 = u2 + r;
                    break;
                }
            }
            break;
        }
    }

    std::tuple<uint, uint, uint, uint> tup = {u1, u2, p1, p2};
    return tup;
}

std::pair<uint, uint> create_pulse_cut(
    std::vector<bool> &ind,
    uint u1,
    uint p2,
    uint nn)
{
    uint from = u1, to = uint(-1);

    if (p2 + nn / 2 < u1 + 3 * nn / 2)
    {
        // caso en que el segundo pulso es muy angosto
        // se toma como minimo 3*nn/2 despues del inicio del primer pulso
        to = u1 + 3 * nn / 2;
    }
    else if (p2 + nn / 2 <= u1 + 4 * nn)
    {
        // caso en que el segundo pulso es muy ancho,
        // se toma como maximo nn/2 despues del final del segundo pulso
        to = p2 + nn / 2;
    }
    else
    {
        // caso intermedio,se toma como maximo 4*nn
        // despues del inicio primer pulso
        to = u1 + 4 * nn;
    }

    for (uint i = from; i <= to; i++)
    {
        ind[i] = 0;
    }

    return {from, to};
}

void drop_finder_on_lvm_file(fs::path lvm_file)
{

    double c1 = 2, c2 = 0.02, c3 = 5, ps = 1000;
    uint nn = 100;

    std::vector<LVM_DATUM> data = read_lvm_file(lvm_file);

    std::vector<double> var_a, var_b;
    std::vector<bool> ind;
    for (uint i = 0; i < data.size(); i++)
    {
        var_a.push_back(data[i].var[0]);
        var_b.push_back(data[i].var[1]);
        ind.push_back(data[i].var[2]);
    }

    data.clear();

    std::vector<LVM_DATUM> drops;

    std::vector<double> thresholds = generate_thresholds(ps, c1, c2, c3);

    uint data_sz = var_a.size();
    uint drop_cnt = 0;

    std::vector<std::vector<std::pair<uint, uint>>> pulses(thresholds.size());

    // La estrategia para buscar el pulso es la siguiente:
    // - Tenemos una lista de thresholds que iremos recorriendo de manera descendente
    // - Si conseguimos un indice i valido y que supere el threshold, buscamos el indice j
    // - El indice j se encuentra a lo sumo `nn` posicion por delante de i, y debe tambien ser valido y superar el threshold

    // Por esto, un par (i, j) se tiene en cuenta si y solamente si, el minimo entre ambos valores supera el threshold

    // De esta forma, proponemos una forma mas veloz de buscar los pulsos
    // Para cada par (i, j), calculamos en que paso deberiamos analizarlo, y lo hacemos en ese paso.
    // Luego, si el pulso es valido en el paso que nos toque analizarlo, lo agregamos a la lista de pulsos

    for (uint i = 0; i < data_sz - 1; i++)
    {
        // El indice i debe ser valido
        bool valid_a = ind[i] && ind[i + 1];

        if (!valid_a)
        {
            continue;
        }

        bool above_threshold_a = var_a[i] >= c2 && var_a[i + 1] >= c2;
        bool under_threshold_a = var_a[i] <= -c2 && var_a[i + 1] <= -c2;

        if (!above_threshold_a && !under_threshold_a)
        {
            continue;
        }

        uint end = std::min(data_sz - 1, i + nn);

        // Buscamos el segundo pulso hasta un total de nn puntos adelante
        for (uint j = i; j < end; j++)
        {
            // El indice j debe ser valido y superar el threshold minimo
            bool valid_b = ind[j] && ind[j + 1];
            if (!valid_b)
            {
                continue;
            }
            bool above_threshold_b = var_b[j] >= c2 && var_b[j + 1] >= c2;
            bool under_threshold_b = var_b[j] <= -c2 && var_b[j + 1] <= -c2;

            if (!above_threshold_b && !under_threshold_b)
            {
                continue;
            }

            double threshold = 0;

            if (above_threshold_a && above_threshold_b)
            {
                // Threshold minimo entre ambos pulsos
                threshold = std::min({var_a[i], var_a[i + 1], var_b[j], var_b[j + 1]});
            }
            else if (under_threshold_a && under_threshold_b)
            {
                // Threshold minimo entre ambos pulsos (negativos)
                threshold = std::min({-var_a[i], -var_a[i + 1], -var_b[j], -var_b[j + 1]});
            }
            else
            {
                continue;
            }

            // Buscamos el threshold en la lista de thresholds
            uint iteration = std::upper_bound(thresholds.rbegin(), thresholds.rend(), threshold) - thresholds.rbegin();

            // Pusheamos el pulso en la iteracion correspondiente
            pulses[thresholds.size() - iteration].push_back({i, j});
        }
    }

    std::vector<std::tuple<uint, uint, uint, uint>> pulse_ranges;

    for (uint p = 0; p < pulses.size(); p++)
    {
        for (auto &[i, j] : pulses[p])
        {
            // Es necesario que los puntos sigan siendo validos
            bool valid_a = ind[i] && ind[i + 1];
            bool valid_b = ind[j] && ind[j + 1];
            if (!valid_a || !valid_b)
            {
                continue;
            }
            // Encontramos un pulso nuevo. Los picos estan en i y j
            // Lo procesamos para encontrar los inicios y finales de cada pulso
            auto [u1, u2, p1, p2] = process_pulse(var_a, var_b, i, j, nn);

            pulse_ranges.push_back({u1, u2, p1, p2});

            // Creamos el corte del pulso
            auto [from, to] = create_pulse_cut(ind, u1, p2, nn);
            uint drop_time = drop_cnt * 1000;
            for (uint i = from; i <= to; i++)
            {
                drops.push_back({drop_time++, {var_a[i], var_b[i], ind[i]}});
            }
            drop_cnt++;
        }
    }

    for (auto &[u1, u2, p1, p2] : pulse_ranges)
    {
        // Calculamos la integral del primer canal
        // Recordemos que u1 es el inicio, y p1 es el punto medio del pulso

        std::vector<double> i1 = {0}, i2 = {0};

        for (uint j = 0; j < std::min(2 * (p1 - u1), 4 * nn); j++)
        {
            i1.push_back(i1.back() - var_a[u1 + j]);
        }
        uint from = u2 - u1 + 1;
        while (i2.size() != from)
            i2.push_back(0);
        for (uint j = from; j < std::min(p2 - u1 + nn / 2, 4 * nn); j++)
        {
            i2.push_back(i2.back() - var_b[u1 + j]);
        }
    }

    std::cout << "Drops found: " << drop_cnt << std::endl;

    write_lvm_file(lvm_file.parent_path() / "drops.lvm", drops);
}