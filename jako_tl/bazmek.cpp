#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "bazmek.h"

const float PI_KOEF = M_PI / 500000;
const char koncovky[13] = {'p', 'n', 'u', 'm', ' ', 'k', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y'};

Bazmek::Bazmek() :
  b(),
  hodnota(),
  druh(),
  zapojeni(),
  umisteni()
{

}

void Bazmek::nahodne() {
  if (rand() % 2) {
    druh = CIVKA;
    hodnota = (rand() % 200 + 1) * (rand() % 200 + 1);
  } else {
    druh = KONDENZATOR;
    hodnota = (rand() % 20 + 1) * (rand() % 20 + 1);
  }

  zapojeni = (Zapojeni)(rand() % 2);
  umisteni = (Zapojeni)(rand() % 2);
  b.clear();

  if (rand() % 6 > 4) {
    Bazmek bz(hodnota, druh, umisteni);
    bz.zapojeni = zapojeni;
    b.push_back(bz);
    while (rand() % 3) {
      bz.nahodne();
      b.push_back(bz);
    }
  }
}

Bazmek::Bazmek(float hodnota_, Druh druh_, Zapojeni umisteni_) :
  b(),
  hodnota(hodnota_),
  druh(druh_),
  zapojeni(),
  umisteni(umisteni_)
{
  zapojeni = (Zapojeni)(rand() % 2);
  umisteni = (Zapojeni)(rand() % 2);
  b.clear();
}

float Bazmek::impedance(float f) {
  if (b.size()) {
    float result = 0;
    switch (zapojeni) {
      case SERIOVE: {
        for (std::vector<Bazmek>::iterator bz = b.begin(); bz != b.end() ; ++bz) {
          result += bz->impedance(f);
        }
      } return result;
      case PARALELNI: {
        bool prd = true;
        for (std::vector<Bazmek>::iterator bz = b.begin(); bz != b.end() ; ++bz) {
          if (prd) {
            prd = false;
            result = bz->impedance(f);
            continue;
          }
          result += bz->impedance(f) * result / (bz->impedance(f) + result);
        }
      } return result;
      default: return 0;
    }
  } else {
    switch (druh) {
      case CIVKA:
        return PI_KOEF * f * hodnota;
      case KONDENZATOR:
        return -1 / (PI_KOEF * f * hodnota);
      default: return 0;
    }
  }
}

int Bazmek::slozitost() {
  if (b.size()) {
    int result = 0;
    for (std::vector<Bazmek>::iterator bz = b.begin(); bz != b.end() ; ++bz) {
      result += bz->slozitost();
    }
    return result;
  } else {
    return 1;
  }
}

float Bazmek::proveditelnost() {
  if (b.size()) {
    float result = 1;
    for (std::vector<Bazmek>::iterator bz = b.begin(); bz != b.end() ; ++bz) {
      result *= bz->proveditelnost();
    }
    return result;
  } else {
    switch (druh) {
      case CIVKA:
        return (1 + 0.00000001 * hodnota * hodnota) / (hodnota * 0.0002);
      case KONDENZATOR:
        return (1 + 0.0001 * hodnota * hodnota) / (hodnota * 0.02);
      default: return 1;
    }
  }
}

void Bazmek::odstran_prebytecne() {
  if (b.size() == 1) {
    hodnota = b[0].hodnota;
    zapojeni = b[0].zapojeni;
    druh = b[0].druh;
  }

  for (std::vector<Bazmek>::iterator bz = b.begin(); bz != b.end() ; ++bz) {
    bz->odstran_prebytecne();
  }
}

QString Bazmek::to_string() {
  QString result = "";
  if (b.size()) {
    switch (zapojeni) {
      case SERIOVE:
        result = "sériově(";
        break;
      case PARALELNI:
        result = "paralelně(";
        break;
    }
    for (std::vector<Bazmek>::iterator bz = b.begin(); bz != b.end() ; ++bz) {
      result += bz->to_string() + ", ";
    }
    result.remove(result.size() - 2, 2);
    result += ")";
  } else {
    int koncovka = 0;
    float h = hodnota;
    while (h > 1000 && koncovka < 12) {
      h /= 1000;
      koncovka++;
    }
    result.setNum(h);
    result += koncovky[koncovka];
    switch (druh) {
      case CIVKA:
        result += "H";
        break;
      case KONDENZATOR:
        result += "F";
        break;
    }
  }
  return result;
}
