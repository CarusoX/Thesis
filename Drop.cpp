/**
 * @file Drop.cpp
 * @brief Implementation of the Drop class for water drop analysis
 * 
 * This file contains the implementation of all methods for analyzing detected
 * water drops, including computation of physical properties, quality assessment,
 * and file I/O operations.
 */

#include "Drop.hpp"

/**
 * @brief Constructor for a detected drop
 * @param isPositive Whether the drop has positive peaks
 * @param c1 Critical point index for sensor1
 * @param c2 Critical point index for sensor2
 */
Drop::Drop(bool isPositive, int c1, int c2)
    : isPositive(isPositive), c1(c1), c2(c2), dataOffset(0), valid(1)
{
}

/**
 * @brief Default constructor for an empty/invalid drop
 */
Drop::Drop() : c1(-1), c2(-1), dataOffset(0), valid(0) {}

/**
 * @brief Finds the middle point of the first sensor signal
 * 
 * This method identifies the point where the first sensor signal crosses
 * the baseline (zero) and returns to its neutral state. This point is
 * used as a reference for calculating the ring sensor charge.
 * 
 * @return Index of the middle point where the signal returns to baseline
 */
int Drop::findSensor1MiddlePoint()
{
    // Lambda to check if sensor signal has returned to baseline
    auto isSensorAnulled = [&](double x) -> bool
    { return this->isPositive ? x < 0 : x > 0; };

    // Search for the point where signal crosses baseline
    for (int i = 0; i < this->size() - 1; i++)
    {
        if (isSensorAnulled(this->sensor1[i]) &&
            isSensorAnulled(this->sensor1[i + 1]))
        {
            return this->p1 = i;
        }
    }
    // If no crossing found, use the last point
    return this->p1 = this->size() - 1;
}

/**
 * @brief Finds the tipping point of the second sensor signal
 * 
 * This method identifies the point where the second sensor signal reaches
 * its maximum integral value and begins to decline. This point is used
 * as a reference for calculating the dish sensor charge and drop velocity.
 * 
 * The algorithm uses a two-stage approach:
 * 1. Find where the integral reaches a threshold
 * 2. Look for the point where the signal starts declining significantly
 * 
 * @return Index of the tipping point where the signal begins to decline
 */
int Drop::findSensor2TippingPoint()
{
    // Lambda for comparison based on drop polarity
    auto compare = [&](double x, double y) -> bool
    { return this->isPositive ? x < y : x > y; };

    // Calculate running integral and find threshold crossing
    double integral = 0;
    for (int i = this->u2 - this->u1; i < this->size(); i++)
    {
        integral = integral - this->sensor2[i];
        
        // Check if integral has reached threshold
        if (compare(integral, this->isPositive ? -0.1 : 0.1))
        {
            // Look for the actual tipping point by analyzing signal decline
            double sum = 0;
            for (int j = i; j < this->size() - 1; j++)
            {
                sum += this->sensor2[j];
                // Check if signal has declined significantly
                if (compare(this->sensor2[j + 1], 0.3 * sum / (j - i + 1)))
                {
                    return this->p2 = j;
                }
            }
        }
    }
    // If no tipping point found, use the last point
    return this->p2 = this->size() - 1;
}

/**
 * @brief Computes the integral of sensor data
 * 
 * This method calculates the cumulative integral of both sensor signals,
 * which is used for charge calculations. The integral represents the
 * total electrical charge accumulated over time.
 */
void Drop::computeIntegral()
{
    // Initialize integrals with zero
    integralSensor1.push_back(0);
    integralSensor2.push_back(0);
    
    // Calculate cumulative integral for each sensor
    for (int i = 0; i < this->size(); ++i)
    {
        this->integralSensor1.push_back(integralSensor1.back() -
                                        this->sensor1[i]);
        this->integralSensor2.push_back(integralSensor2.back() -
                                        this->sensor2[i]);
    }
}

/**
 * @brief Computes the ring sensor charge (q1)
 * 
 * Calculates the electrical charge measured by the ring sensor at the
 * middle point of the signal. This represents the charge induced on
 * the ring as the drop passes through.
 */
void Drop::computeRingCharge()
{
    this->q1 =
        INTEGRATION_FACTOR / DATA_PER_SECOND * this->integralSensor1[this->p1];
}

/**
 * @brief Computes the dish sensor charge (q2)
 * 
 * Calculates the electrical charge measured by the dish sensor at the
 * tipping point of the signal. This represents the charge induced on
 * the dish as the drop passes through.
 */
void Drop::computeDishCharge()
{
    this->q2 =
        INTEGRATION_FACTOR / DATA_PER_SECOND * this->integralSensor2[this->p2];
}

/**
 * @brief Computes the average charge (q)
 * 
 * Calculates the average charge by combining measurements from both
 * sensors, accounting for the expected charge ratio between them.
 */
void Drop::computeAverageCharge()
{
    this->q = (this->q1 / PROP_CHARGE + this->q2) / 2.;
}

/**
 * @brief Applies basic validation filters to the drop
 * 
 * This method implements fundamental validation criteria to determine
 * if a detected drop is physically reasonable and should be included
 * in the analysis. The filters check:
 * - Minimum charge thresholds
 * - Maximum velocity limits
 * - Charge polarity consistency
 * 
 * @return 1 if valid, 0 if invalid
 */
int Drop::satisfiesBasicFilters()
{
    // Check minimum charge threshold for dish sensor
    if (std::abs(this->q2) < MINIMUM_CHARGE)
    {
        return this->valid = 0;
    }
    
    // Check minimum charge threshold for ring sensor
    if (std::abs(this->q1) < MINIMUM_CHARGE)
    {
        return this->valid = 0;
    }
    
    // Check maximum velocity (minimum time difference between peaks)
    // This corresponds to a maximum velocity of 11.4 m/s
    if (this->p1 > this->p2 - NN / 4)
    {
        return this->valid = 0;
    }

    // Check charge polarity consistency - charges must have same sign
    if (this->q1 > 0 && this->q2 < 0)
    {
        return this->valid = 0;
    }
    if (this->q1 < 0 && this->q2 > 0)
    {
        return this->valid = 0;
    }

    return this->valid = 1;
}

/**
 * @brief Computes drop velocity from sensor timing
 * 
 * Calculates the drop velocity using the time difference between
 * the key analysis points (p1 and p2) and the known separation
 * distance between the ring and dish sensors.
 */
void Drop::computeVelocity()
{
    this->v = RING_DISH_SEP / (this->time[p2] - this->time[p1]);
}

/**
 * @brief Computes drop diameter from velocity using lookup table
 * 
 * Uses a pre-computed lookup table to determine drop diameter
 * based on the calculated velocity. The relationship between
 * velocity and diameter is based on terminal velocity physics
 * for water drops in air.
 */
void Drop::computeDiameter()
{
    // Handle edge cases - velocity below or above table range
    if (this->v < diameters[0].first) {
        this->d = diameters[0].second;
    } else if (this->v > diameters[diameters.size() - 1].first) {
        this->d = diameters[diameters.size() - 1].second;
    } else {
        // Binary search for the correct velocity range
        int left = 0;
        int right = diameters.size() - 1;

        while (left <= right) {
            int mid = (left + right) / 2;
            
            // Check if velocity falls in this range
            if (diameters[mid].first <= this->v && this->v < diameters[mid + 1].first) {
                this->d = diameters[mid].second;
                break;
            }
            
            // Adjust search range
            if (this->v < diameters[mid].first) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }
    }
}

void Drop::computeModels()
{
    for (int i = 0; i < this->size(); i++)
    {
        this->a1.push_back(
            30.6 * this->q1 /
            std::pow(std::pow(30.6, 1 / 1.7) + std::pow((this->v / 50.7), 2) *
                                                   std::pow(i - this->p1, 2),
                     1.7));
        this->b1.push_back(this->q1 * std::exp(-std::pow(i - this->p1, 2) /
                                               std::pow(this->p1, 2) * 3.62));
    }

    for (int i = 0; i < this->size(); i++)
    {
        if (i < this->u2)
        {
            this->a2.push_back(0);
        }
        else if (i < this->p2)
        {
            this->a2.push_back(this->q2 * std::pow(i - this->u2, 1.373) /
                               std::pow(this->p2 - this->u2, 1.373));
        }
        else
        {
            this->a2.push_back(this->q2);
        }
    }
}

void Drop::computeSumOfSquaredDiffPenalty()
{
    this->sumOfSquaredDiffPenalty1 = this->sumOfSquaredDiffPenalty2 = 0;
    for (int i = 0; i < std::min(2 * this->p1, this->size()); ++i)
    {
        this->sumOfSquaredDiffPenalty1 +=
            std::pow((this->a1[i] - this->integralSensor1[i] *
                                        INTEGRATION_FACTOR / DATA_PER_SECOND) /
                         this->q1,
                     2);
    }
    for (int i = 0; i < std::min(this->p2 + NN / 2, this->size()); ++i)
    {
        this->sumOfSquaredDiffPenalty2 +=
            std::pow((this->a2[i] - this->integralSensor2[i] *
                                        INTEGRATION_FACTOR / DATA_PER_SECOND) /
                         this->q2,
                     2);
    }
    this->sumOfSquaredDiffPenalty1 =
        std::log(this->sumOfSquaredDiffPenalty1 + 1);
    this->sumOfSquaredDiffPenalty2 =
        std::log(this->sumOfSquaredDiffPenalty2 + 1);
}

void Drop::computeChargeDiffPenalty()
{
    double x1 = this->q1 / this->q;
    double x2 = this->q2 / this->q;
    this->chargeDiffPenalty = 3 * std::abs(x1 - PROP_CHARGE * x2);
}

void Drop::computeWidthDiffPenalty()
{
    double x1 = this->p1;
    double x2 = this->p2;
    double y1 = 2 * x1 / (x1 / PROP_WIDTH + x2);
    double y2 = 2 * x2 / (x1 / PROP_WIDTH + x2);
    this->widthDiffPenalty = 3 * std::abs(y1 - PROP_WIDTH * y2);
}

void Drop::computeNoisePropPenalty()
{
    double avgSensor1 = 0, avgSensor2 = 0;
    for (int i = 0; i < this->size(); i++)
    {
        avgSensor1 += this->sensor1[i];
        avgSensor2 += this->sensor2[i];
    }
    avgSensor1 /= this->size(), avgSensor2 /= this->size();

    int noise = 0;
    for (int i = 0; i < this->size(); i++)
    {
        if (std::abs(sensor1[i] - avgSensor1) < MINIMUM_THRESHOLD)
        {
            noise++;
        }
        if (std::abs(sensor2[i] - avgSensor2) < MINIMUM_THRESHOLD)
        {
            noise++;
        }
    }
    this->noisePropPenalty = std::pow(double(noise) / (this->size() * 2), 2);
}

double Drop::penalty() const
{
    return this->sumOfSquaredDiffPenalty1 + this->sumOfSquaredDiffPenalty2 +
           this->chargeDiffPenalty + this->widthDiffPenalty +
           this->noisePropPenalty;
}

void Drop::debug() const
{
    std::cout << "Drop " << id << ":" << std::endl;
    std::cout << "  u1: " << u1 << std::endl;
    std::cout << "  u2: " << u2 << std::endl;
    std::cout << "  c1: " << c1 << std::endl;
    std::cout << "  c2: " << c2 << std::endl;
    std::cout << "  p1: " << p1 << std::endl;
    std::cout << "  p2: " << p2 << std::endl;
    std::cout << "  q1: " << q1 << std::endl;
    std::cout << "  q2: " << q2 << std::endl;
    std::cout << "  q: " << q << std::endl;
    std::cout << "  d: " << d << std::endl;
    std::cout << "  v: " << v << std::endl;
    std::cout << "  sumOfSquaredDiffPenalty1: " << sumOfSquaredDiffPenalty1
              << std::endl;
    std::cout << "  sumOfSquaredDiffPenalty2: " << sumOfSquaredDiffPenalty2
              << std::endl;
    std::cout << "  chargeDiffPenalty: " << chargeDiffPenalty << std::endl;
    std::cout << "  widthDiffPenalty: " << widthDiffPenalty << std::endl;
    std::cout << "  noisePropPenalty: " << noisePropPenalty << std::endl;
    std::cout << "  penalty: " << this->penalty() << std::endl;
    std::cout << "  valid: " << valid << std::endl;
}

int Drop::size() const { return sensor1.size(); }

/**
 * @brief Computes all drop statistics and properties
 * 
 * This is the main method that orchestrates the calculation of all
 * derived properties and quality metrics for the drop. It must be
 * called after the drop data has been extracted and key analysis
 * points (p1, p2) have been identified.
 * 
 * The computation order is important as later calculations depend
 * on earlier ones.
 */
void Drop::computeStats()
{
    this->computeIntegral();                // Calculate sensor integrals
    this->computeRingCharge();              // Calculate ring sensor charge (q1)
    this->computeDishCharge();              // Calculate dish sensor charge (q2)
    this->computeAverageCharge();           // Calculate average charge (q)
    this->satisfiesBasicFilters();          // Apply basic validation filters
    this->computeVelocity();                // Calculate drop velocity (v)
    this->computeDiameter();                // Calculate drop diameter (d)
    this->computeModels();                  // Calculate theoretical model fits (a1, b1, a2)
    this->computeSumOfSquaredDiffPenalty(); // Calculate model fit quality penalties
    this->computeChargeDiffPenalty();       // Calculate charge ratio penalty
    this->computeWidthDiffPenalty();        // Calculate width ratio penalty
    this->computeNoisePropPenalty();        // Calculate noise proportion penalty
}

std::vector<Drop> Drop::readFromFile(std::ifstream &file)
{
    if (!file.is_open())
    {
        throw std::runtime_error("Error: Unable to open file for reading.");
    }
    std::vector<Drop> drops;
    std::string line;
    Drop currentDrop;
    int currentId = -1;

    // Nos salteamos los headers
    std::getline(file, line);

    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        double time, sensor1, sensor2, integralSensor1, integralSensor2;
        double a1, a2, b1, q1, q2, v, d, sumOfSquaredDiffPenalty1,
            sumOfSquaredDiffPenalty2, chargeDiffPenalty, widthDiffPenalty,
            noisePropPenalty, penalty;
        int step, id;

        // Leer los datos en el mismo orden en el que fueron escritos
        if (!(iss >> time >> step >> sensor1 >> sensor2 >> integralSensor1 >>
              integralSensor2 >> a1 >> a2 >> b1 >> q1 >> q2 >> v >> d >>
              sumOfSquaredDiffPenalty1 >> sumOfSquaredDiffPenalty2 >>
              chargeDiffPenalty >> widthDiffPenalty >> noisePropPenalty >>
              penalty >> id))
        {
            throw std::runtime_error("Error: Invalid data format in file.");
        }
        // Si el ID cambia, armar la nueva gota y comenzar una nueva
        if (currentId != id && !currentDrop.time.empty())
        {
            currentDrop.valid = 1;
            drops.push_back(currentDrop);
            currentDrop = Drop();
            currentDrop.dataOffset = step;
        }
        // Update current ID and append data to the current drop
        currentId = id;
        currentDrop.id = id;
        currentDrop.time.push_back(time);
        currentDrop.sensor1.push_back(sensor1);
        currentDrop.sensor2.push_back(sensor2);
        currentDrop.integralSensor1.push_back(integralSensor1);
        currentDrop.integralSensor2.push_back(integralSensor2);
        currentDrop.a1.push_back(a1);
        currentDrop.a2.push_back(a2);
        currentDrop.b1.push_back(b1);
        currentDrop.q1 = q1;
        currentDrop.q2 = q2;
        currentDrop.v = v;
        currentDrop.d = d;
        currentDrop.sumOfSquaredDiffPenalty1 = sumOfSquaredDiffPenalty1;
        currentDrop.sumOfSquaredDiffPenalty2 = sumOfSquaredDiffPenalty2;
        currentDrop.chargeDiffPenalty = chargeDiffPenalty;
        currentDrop.widthDiffPenalty = widthDiffPenalty;
        currentDrop.noisePropPenalty = noisePropPenalty;
        currentDrop.computeAverageCharge();
        currentDrop.valid = 1;
    }
    if (currentDrop.valid) {
        drops.push_back(currentDrop);
    }
    return drops;
}

void Drop::writeHeader(std::ofstream &file, bool sortedDrops)
{
    if (sortedDrops)
    {
        // Column order for sorted drops:
        // time, step, sensor1, sensor2, integral_sensor1, integral_sensor2,
        // a1, a2, b1, q1, q2, v, d, penalty
        file <<
            "time\tstep\tsensor1\tsensor2\tintegral_sensor1\tintegral_sensor2"
            "\ta1\ta2\tb1\tq1\tq2\tv\tdiameter\tsum_of_penalties\n";
    }
    else
    {
        // Column order for drops written by drop_finder:
        // time, step, sensor1, sensor2, integral_sensor1, integral_sensor2,
        // a1, a2, b1, q1, q2, v, d,
        // sum_sq_diff_penalty1, sum_sq_diff_penalty2, charge_diff_penalty,
        // width_diff_penalty, noise_prop_penalty, penalty, id
        file <<
            "time\tstep\tsensor1\tsensor2\tintegral_sensor1\tintegral_sensor2"
            "\ta1\ta2\tb1\tq1\tq2\tv\tdiameter\tsum_sq_diff_penalty1"
            "\tsum_sq_diff_penalty2\tcharge_diff_penalty\twidth_diff_penalty"
            "\tnoise_prop_penalty\tsum_of_penalties\tid\n";
    }
}

void Drop::writeToFile(std::ofstream &file, bool sortedDrops)
{
    file << std::fixed
         << std::setprecision(6); // Format numbers to 6 decimal places
    for (int i = 0; i < this->size(); ++i)
    {
        int step = this->dataOffset + i;
        
        // Write data in the correct order based on sortedDrops flag
        if (sortedDrops) {
            // Column order for sorted drops: time, step, sensor1, sensor2, 
            // integral_sensor1, integral_sensor2, a1, a2, b1, q1, q2, v, d, penalty
            file << this->time[i] << "\t" << step << "\t"
                 << this->sensor1[i] << "\t" << this->sensor2[i] << "\t"
                 << this->integralSensor1[i] * INTEGRATION_FACTOR / DATA_PER_SECOND << "\t"
                 << this->integralSensor2[i] * INTEGRATION_FACTOR / DATA_PER_SECOND << "\t"
                 << this->a1[i] << "\t" << this->a2[i] << "\t" << this->b1[i] << "\t"
                 << this->q1 << "\t" << this->q2 << "\t" << this->v << "\t" << this->d << "\t"
                 << this->penalty() << " " << this->id << "\n";
        } else {
            // Column order for drops written by drop_finder: time, step, sensor1, sensor2,
            // integral_sensor1, integral_sensor2, a1, a2, b1, q1, q2, v, d,
            // sum_sq_diff_penalty1, sum_sq_diff_penalty2, charge_diff_penalty,
            // width_diff_penalty, noise_prop_penalty, penalty, id
            file << this->time[i] << "\t" << step << "\t"
                 << this->sensor1[i] << "\t" << this->sensor2[i] << "\t"
                 << this->integralSensor1[i] * INTEGRATION_FACTOR / DATA_PER_SECOND << "\t"
                 << this->integralSensor2[i] * INTEGRATION_FACTOR / DATA_PER_SECOND << "\t"
                 << this->a1[i] << "\t" << this->a2[i] << "\t" << this->b1[i] << "\t"
                 << this->q1 << "\t" << this->q2 << "\t" << this->v << "\t" << this->d << "\t"
                 << this->sumOfSquaredDiffPenalty1 << "\t" << this->sumOfSquaredDiffPenalty2 << "\t"
                 << this->chargeDiffPenalty << "\t" << this->widthDiffPenalty << "\t"
                 << this->noisePropPenalty << "\t" << this->penalty() << "\t" << this->id << "\n";
        }
    }
}