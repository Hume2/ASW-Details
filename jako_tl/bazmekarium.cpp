#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "bazmekarium.h"

std::vector<Bazmekarium::Zadani> Bazmekarium::zadani;
int Bazmekarium::max_slozitost = 20;

Bazmekarium::Bazmekarium() :
  bazmeky(),
  hodnota(),
  neprizpusobeno(),
  swr(),
  vzdalenost()
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
  for (std::list<Bazmek>::iterator bz = bazmeky.begin(); bz != bazmeky.end() ; ++bz) {
    result += bz->slozitost();
  }
  return /*abs(result) + 1;//*/(result < 1) ? 1 : result;
}

float Bazmekarium::spocitej_proveditelnost() {
  float result = 1;
  for (std::list<Bazmek>::iterator bz = bazmeky.begin(); bz != bazmeky.end() ; ++bz) {
    result *= bz->proveditelnost();
  }
  return result;
}

void Bazmekarium::spocitej_impedanci(float &Zlr, float &Zli, float f) {
  for (std::list<Bazmek>::iterator bz = bazmeky.begin(); bz != bazmeky.end() ; ++bz) {
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
}

float Bazmekarium::spocitej_swr(float Zlr, float Zli, float f) {
  const float ref = 50;

  //float Zlr_ = Zlr;
  spocitej_impedanci(Zlr, Zli, f);

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
    float swr = (1 + G) / (1 - G);
    /*if (swr > 2 && (fabs(Zlr_ - Zlr) < 10)) {
      return 100 + swr;
    } else if (swr < 2) {
      return 0;
    } else {*/
      return swr;
    //}
  }
}

double Bazmekarium::spocitej_uzitecnost() {
  double result = 1;
  for (std::vector<Zadani>::iterator z = zadani.begin(); z != zadani.end() ; ++z) {
    float swr = spocitej_swr(z->Zlr, z->Zli, z->f);
    /*if (swr > 10000 || swr < 1) {
      neudelatelno++;
    } else {
      if (swr > 2) {
        neprizpusobeno++;
      }*/
      result *= swr;
    //}
  }
  return result;
}

double Bazmekarium::ohodnot() {
  double result = 1;
  neprizpusobeno = 0;
  neudelatelno = 0;
  result *= spocitej_uzitecnost();
  swr = result;
  result *= spocitej_slozitost();
  result *= spocitej_proveditelnost();
  hodnota = result;
  return result;
}

void Bazmekarium::odstran_zbytecne() {
  for (std::list<Bazmek>::iterator bz = bazmeky.begin(); bz != bazmeky.end() ; ++bz) {
    bz->odstran_prebytecne();
  }
}

QString Bazmekarium::to_string() {
  QString result = "ant < ";
  for (std::list<Bazmek>::iterator bz = bazmeky.begin(); bz != bazmeky.end() ; ++bz) {
    if (bz->umisteni == Bazmek::PARALELNI) {
      result += "napříč ";
    }
    result += bz->to_string();
    result += " < ";
  }
  return result;
}

void Bazmekarium::mutace(float m) {
  m /= bazmeky.size();
  for (std::list<Bazmek>::iterator bz = bazmeky.begin(); bz != bazmeky.end() ; ++bz) {
    int r = rand() % 100;
    if (r < m) {
      m -= r;
      if (rand() % 7 || bazmeky.size() == 1) {
        bz->mutace(r);
      } else {
        if (rand() % 2) {
          std::list<Bazmek>::iterator bz2 = bz;
          bz++;
          bazmeky.erase(bz2);
          bz--;
        } else {
          Bazmek bz;
          bz.nahodne();
          bazmeky.push_back(bz);
        }
      }
    }
  }
}

void Bazmekarium::krizeni(Bazmekarium &B) {
  if (bazmeky.size() == 1) {
    Bazmek bz;
    bz.nahodne();
    bazmeky.push_back(bz);
  }
  if (B.bazmeky.size() == 1) {
    Bazmek bz;
    bz.nahodne();
    B.bazmeky.push_back(bz);
  }

  int c1 = rand() % (bazmeky.size() - 1) + 1;
  int c2 = rand() % (B.bazmeky.size() - 1) + 1;

  std::list<Bazmek> b11, b12, b21, b22;
  int i = 0;
  for (std::list<Bazmek>::iterator bz = bazmeky.begin(); bz != bazmeky.end() ; ++bz) {
    if (i < c1) {
      b11.push_back(*bz);
    } else {
      b22.push_back(*bz);
    }
    i++;
  }
  i = 0;
  for (std::list<Bazmek>::iterator bz = B.bazmeky.begin(); bz != B.bazmeky.end() ; ++bz) {
    if (i < c2) {
      b21.push_back(*bz);
    } else {
      b12.push_back(*bz);
    }
    i++;
  }

  bazmeky.clear();
  bazmeky.insert(bazmeky.end(), b11.begin(), b11.end());
  bazmeky.insert(bazmeky.end(), b12.begin(), b12.end());

  B.bazmeky.clear();
  B.bazmeky.insert(B.bazmeky.end(), b21.begin(), b21.end());
  B.bazmeky.insert(B.bazmeky.end(), b22.begin(), b22.end());
}
