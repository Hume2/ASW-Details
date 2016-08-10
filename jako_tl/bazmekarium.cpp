#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "bazmekarium.h"

std::vector<Bazmekarium::Zadani> Bazmekarium::zadani;
int Bazmekarium::max_slozitost = 6;

Bazmekarium::Bazmekarium() :
  bazmeky()
{
}

void Bazmekarium::nahodne() {
  Bazmek b;
  b.nahodne();
  bazmeky.push_back(b);
  while (rand() % 3) {
    b.nahodne();
    bazmeky.push_back(b);
  }
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

float Bazmekarium::spocitej_uzitecnost() {
  float result = 1;
  for (std::vector<Zadani>::iterator z = zadani.begin(); z != zadani.end() ; ++z) {
    float swr = spocitej_swr(z->Zlr, z->Zli, z->f);
    result *= (swr > 10) ? 10 : swr;
  }
  return result;
}

float Bazmekarium::ohodnot() {
  float result = 1;
  result *= spocitej_uzitecnost();
  result *= spocitej_slozitost();
  result *= spocitej_proveditelnost();
  return result;
}

void Bazmekarium::odstran_zbytecne() {
  for (std::vector<Bazmek>::iterator bz = bazmeky.begin(); bz != bazmeky.end() ; ++bz) {
    bz->odstran_prebytecne();
  }
}

QString Bazmekarium::to_string() {
  QString result = "ant < ";
  for (std::vector<Bazmek>::iterator bz = bazmeky.begin(); bz != bazmeky.end() ; ++bz) {
    if (bz->umisteni == Bazmek::PARALELNI) {
      result += "napříč ";
    }
    result += bz->to_string();
    result += " < ";
  }
  return result;
}
