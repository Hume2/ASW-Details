#include <math.h>

#include "bazmekarium.h"

std::vector<float> Bazmekarium::kmitocty(7, 1);
int Bazmekarium::max_slozitost = 6;

Bazmekarium::Bazmekarium() :
  bazmeky()
{
}

int Bazmekarium::spocitej_slozitost() {
  int result = -max_slozitost;
  for (std::vector<Bazmek>::iterator bz = bazmeky.begin(); bz != bazmeky.end() ; ++bz) {
    result += bz->slozitost();
  }
  return (result < 1) ? 1 : result;
}

float Bazmekarium::spocitej_proveditelnost() {
  float result = 1;
  for (std::vector<Bazmek>::iterator bz = bazmeky.begin(); bz != bazmeky.end() ; ++bz) {
    result *= bz->proveditelnost();
  }
  return result;
}

float Bazmekarium::spocitej_swr(float Zlr, float Zli, float f) {
  const float ref = 50;

  for (std::vector<Bazmek>::iterator bz = bazmeky.begin(); bz != bazmeky.end() ; ++bz) {
    if (bz->umisteni == Bazmek::SERIOVE) {
      Zli += bz->impedance(f);
    } else {
      float x = bz->impedance(f);
      float D = Zli + x;
      D = D * D + Zlr * Zlr;
      float Zlr2 = x * (Zlr + Zli);
      float Zli2 = Zlr * Zlr + Zli * Zli;
      Zlr = Zlr2 / D;
      Zli = Zli2 / D;
    }
  }

  float D = Zlr + ref;
  D = D * D + Zli * Zli;
  float Zlr2 = Zlr * Zlr;
  float Zli2 = Zli * Zli;
  float ref2 = ref * ref;
  float Gr = (Zlr2 + Zli2 - ref2) / D;
  float Gi = 2 * Zli * ref / D;
  float G = sqrt(Gr * Gr + Gi * Gi);
  if (G == 1) {
    return 999999999;
  } else {
    return (1 + G) / (1 - G);
  }
}

float Bazmekarium::spocitej_uzitecnost(float Zlr, float Zli) {
  float result = 1;
  for (std::vector<float>::iterator f = kmitocty.begin(); f != kmitocty.end() ; ++f) {
    float swr = spocitej_swr(Zlr, Zli, *f);
    result *= (swr > 10) ? 10 : swr;
  }
  return result;
}

float Bazmekarium::ohodnot(float Zlr, float Zli) {
  float result = 1;
  result *= spocitej_uzitecnost(Zlr, Zli);
  result *= spocitej_slozitost();
  result *= spocitej_proveditelnost();
  return result;
}

void Bazmekarium::odstran_zbytecne() {
  for (std::vector<Bazmek>::iterator bz = bazmeky.begin(); bz != bazmeky.end() ; ++bz) {
    bz->odstran_prebytecne();
  }
}
