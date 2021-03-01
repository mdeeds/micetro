/*
https://blog.esciencecenter.nl/using-c-in-a-web-app-with-webassembly-efd78c08469

 ~/github/emsdk/upstream/emscripten/em++ spring.cc -o spring.wasm -O3 -s WASM=1

wasm2wat spring.wasm -o spring.wat
*/

#include <vector>
#include <stdio.h>

const float kPressureImpact = 0.0001;
const float kEmaConstant = 0.02;

class Spring
{
public:
  Spring(float k) : k(k) {}

  // Add the specified pressure and return the updated value for
  // moving average
  float update(float pressure)
  {
    float acceleration = -x * k + (pressure - x) * kPressureImpact;
    v += acceleration;
    x += v;
    float x2 = x * x;
    ema = kEmaConstant * x2 + (1.0f - kEmaConstant) * ema;
    return ema;
  }

private:
  float x = 0;   // Current position
  float v = 0;   // Current velocity
  float ema = 0; // Exponential Moving Average
  float k;       // Spring constant
};

class SpringSet
{
public:
  SpringSet(int n)
  {
    springs.reserve(n);
    for (int i = 0; i < n; ++i)
    {
      float k = 1.0; // TODO: Get a good k
      springs.emplace_back(k);
    }
  }

  // Applies specified pressure to the spring and returns
  // resulting EMA energy.
  void update(float pressure, float *energy)
  {
    for (int i = 0; i < springs.size(); ++i)
    {
      energy[i] = springs[i].update(pressure);
    }
    energy[2] = 3.0;
    energy[1] = springs.size();
  }

private:
  std::vector<Spring> springs;
};

extern "C" __attribute__((used)) SpringSet *MakeSpringSet(int n)
{
  return new SpringSet(n);
}

extern "C" __attribute__((used)) void UpdateSprings(
    SpringSet *springs, float pressure, float *energy)
{
  springs->update(pressure, energy);
  energy[0] = 1.0;
  energy[3] = 1.0;
}

int main(int argc, char **argv)
{
}
