#include "Drop.hpp"

Drop::Drop(bool isPositive, int c1, int c2) : isPositive(isPositive), c1(c1), c2(c2), valid(1) {}

Drop::Drop() : c1(-1), c2(-1), valid(0) {}

int Drop::findSensor1MiddlePoint()
{
  auto isSensorAnulled = [&](double x) -> bool
  {
    return this->isPositive ? x < 0 : x > 0;
  };

  for (int i = 0; i < this->size() - 1; i++)
  {
    if (isSensorAnulled(this->sensor1[i]) && isSensorAnulled(this->sensor1[i + 1]))
    {
      return this->p1 = i;
    }
  }
  return this->p1 = this->size();
}

int Drop::findSensor2TippingPoint()
{

  auto compare = [&](double x, double y) -> bool
  {
    return this->isPositive ? x < y : x > y;
  };

  double integral;
  for (int i = this->u2; i < this->size(); i++)
  {
    integral = integral - this->sensor2[i];
    if (compare(integral, this->isPositive ? -0.1 : 0.1))
    {
      double sum = 0;
      for (int j = i; j < this->size() - 1; j++)
      {
        sum += this->sensor2[j];
        if (compare(this->sensor2[j + 1], 0.3 * sum / (j - i + 1)))
        {
          return this->p2 = j;
        }
      }
    }
  }
  return this->p2 = this->size();
}

void Drop::computeIntegral()
{
  integralSensor1.push_back(0);
  integralSensor2.push_back(0);
  for (int i = 0; i < this->size(); ++i)
  {
    this->integralSensor1.push_back(integralSensor1.back() - this->sensor1[i]);
    this->integralSensor2.push_back(integralSensor2.back() - this->sensor2[i]);
  }
}

void Drop::computeRingCharge()
{
  this->q1 = INTEGRATION_FACTOR / DATA_PER_SECOND * this->integralSensor1[this->p1];
}

void Drop::computeDishCharge()
{
  this->q2 = INTEGRATION_FACTOR / DATA_PER_SECOND * this->integralSensor2[this->p2];
}

void Drop::computeAverageCharge()
{
  this->q = (this->q1 / PROP_CHARGE + this->q2) / 2.;
}

int Drop::satisfiesBasicFilters()
{
  if (std::abs(this->q2) < MINIMUM_CHARGE)
  {
    return this->valid = 0;
  }
  if (std::abs(this->q1) < MINIMUM_CHARGE)
  {
    return this->valid = 0;
  }
  // minima diferencia entre picos (velocidad maxima de 11.4 m/s)
  if (this->p1 > this->p2 - NN / 4)
  {
    return this->valid = 0;
  }

  // no pueden ser de diferente signo
  if (this->q1 > 0 && this->q2 < 0)
  {
    return this->valid = 0;
  }
  // no pueden ser de diferente signo
  if (this->q1 < 0 && this->q2 > 0)
  {
    return this->valid = 0;
  }

  return this->valid = 1;
}

void Drop::computeVelocity()
{
  this->v = RING_DISH_SEP / (this->time[p2] - this->time[p1]);
}

void Drop::computeModels()
{
  for (int i = 0; i < this->size(); i++)
  {
    // std::cout << 30.6 * this->q1 << " " << std::pow(std::pow(30.6, 1 / 1.7) + std::pow((this->v / 50.7), 2) * std::pow(i - this->p1, 2), 1.7) << std::endl;
    this->a1.push_back(30.6 * this->q1 / std::pow(std::pow(30.6, 1 / 1.7) + std::pow((this->v / 50.7), 2) * std::pow(i - this->p1, 2), 1.7));
    this->b1.push_back(this->q1 * std::exp(-std::pow(i - this->p1, 2) / std::pow(-this->p1, 2) * 3.62));
  }

  for (int i = 0; i < this->size(); i++)
  {
    if (i < this->u2)
    {
      this->a2.push_back(0);
    }
    else if (i < this->p2)
    {
      this->a2.push_back(this->q2 * std::pow(i - this->u2, 1.373) / std::pow(this->p2 - this->u2, 1.373));
    }
    else
    {
      this->a2.push_back(this->q2);
    }
  }
}

void Drop::computeSumOfSquaredDiffPenalty()
{
  for (int i = 0; i < std::min(2 * this->p1, this->size()); ++i)
  {
    this->sumOfSquaredDiffPenalty1 += std::pow((this->a1[i] - this->integralSensor1[i] * INTEGRATION_FACTOR / DATA_PER_SECOND) / this->q1, 2);
  }
  for (int i = 0; i < std::min(this->p2 + NN / 2, this->size()); ++i)
  {
    this->sumOfSquaredDiffPenalty2 += std::pow((this->a2[i] - this->integralSensor2[i] * INTEGRATION_FACTOR / DATA_PER_SECOND) / this->q2, 2);
  }
  this->sumOfSquaredDiffPenalty1 = std::log(this->sumOfSquaredDiffPenalty1);
  this->sumOfSquaredDiffPenalty2 = std::log(this->sumOfSquaredDiffPenalty2);
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
  return this->sumOfSquaredDiffPenalty1 + this->sumOfSquaredDiffPenalty2 + this->chargeDiffPenalty + this->widthDiffPenalty + this->noisePropPenalty;
}

void Drop::debug() const
{
  std::cout << "Drop: " << std::endl;
  std::cout << "  u1: " << u1 << std::endl;
  std::cout << "  u2: " << u2 << std::endl;
  std::cout << "  c1: " << c1 << std::endl;
  std::cout << "  c2: " << c2 << std::endl;
  std::cout << "  p1: " << p1 << std::endl;
  std::cout << "  p2: " << p2 << std::endl;
  std::cout << "  q1: " << q1 << std::endl;
  std::cout << "  q2: " << q2 << std::endl;
  std::cout << "  q: " << q << std::endl;
  std::cout << "  v: " << v << std::endl;
  std::cout << "  sumOfSquaredDiffPenalty1: " << sumOfSquaredDiffPenalty1 << std::endl;
  std::cout << "  sumOfSquaredDiffPenalty2: " << sumOfSquaredDiffPenalty2 << std::endl;
  std::cout << "  chargeDiffPenalty: " << chargeDiffPenalty << std::endl;
  std::cout << "  widthDiffPenalty: " << widthDiffPenalty << std::endl;
  std::cout << "  noisePropPenalty: " << noisePropPenalty << std::endl;
  std::cout << "  penalty: " << this->penalty() << std::endl;
}

int Drop::size() const
{
  return sensor1.size();
}

void Drop::computeStats()
{
  this->findSensor1MiddlePoint();         // p1
  this->findSensor2TippingPoint();        // p2
  this->computeIntegral();                // integrals
  this->computeRingCharge();              // q1
  this->computeDishCharge();              // q2
  this->computeAverageCharge();           // q
  this->satisfiesBasicFilters();          // filter 2
  this->computeVelocity();                // v
  this->computeModels();                  // a1, b1, a2
  this->computeSumOfSquaredDiffPenalty(); // sumOfSquaredDiffPenalty1, sumOfSquaredDiffPenalty2
  this->computeChargeDiffPenalty();       // chargeDiffPenalty
  this->computeWidthDiffPenalty();        // widthDiffPenalty
  this->computeNoisePropPenalty();        // noisePropPenalty
}
