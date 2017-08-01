#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

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
    hodnota = float(rand() % 200 + 1) * float(rand() % 200 + 1) * 1000;
  } else {
    druh = KONDENZATOR;
    hodnota = float(rand() % 200 + 1) * float(rand() % 200 + 1) / 1000;
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
        for (std::list<Bazmek>::iterator bz = b.begin(); bz != b.end() ; ++bz) {
          result += bz->impedance(f);
        }
      } return result;
      case PARALELNI: {
        bool prd = true;
        for (std::list<Bazmek>::iterator bz = b.begin(); bz != b.end() ; ++bz) {
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
    for (std::list<Bazmek>::iterator bz = b.begin(); bz != b.end() ; ++bz) {
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
    for (std::list<Bazmek>::iterator bz = b.begin(); bz != b.end() ; ++bz) {
      result *= bz->proveditelnost();
    }
    return result;
  } else {
    float p;
    switch (druh) {
      case CIVKA:
        switch (umisteni) {
          case PARALELNI:
            p = (1 + 0.0000000001 * hodnota * hodnota) / (hodnota * 0.00002);
            break;
          case SERIOVE:
            p = (1 + 0.00000001 * hodnota * hodnota) / (hodnota * 0.0002);
            break;
          default: p = 1;
        }
        break;
      case KONDENZATOR:
        switch (umisteni) {
          case PARALELNI:
            p = (1 + 0.0001 * hodnota * hodnota) / (hodnota * 0.02);
            break;
          case SERIOVE:
            p = (1 + 0.000001 * hodnota * hodnota) / (hodnota * 0.002);
            break;
          default: p = 1;
        }
        break;
      default: p = 1;
    }
    return (p < 3) ? 1 : p * p * p;
  }
}

void Bazmek::odstran_prebytecne() {
  if (b.size() == 1) {
    hodnota = b.begin()->hodnota;
    zapojeni = b.begin()->zapojeni;
    druh = b.begin()->druh;
  }

  for (std::list<Bazmek>::iterator bz = b.begin(); bz != b.end() ; ++bz) {
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
    for (std::list<Bazmek>::iterator bz = b.begin(); bz != b.end() ; ++bz) {
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

void Bazmek::mutace(float m) {
  if (int(m) == 0 || rand() % int(m) == 0) {
    return;
  }

  switch (rand() % 4) {
    case 0:
      if (zapojeni == SERIOVE) {
        zapojeni = PARALELNI;
      } else {
        zapojeni = SERIOVE;
      }
      break;
    case 1:
      if (umisteni == SERIOVE && druh == KONDENZATOR) {
        umisteni = PARALELNI;
      } else if (umisteni == PARALELNI && druh == CIVKA) {
        umisteni = SERIOVE;
      }
      break;
    case 2:
      if (b.size()) {
        for (std::list<Bazmek>::iterator bz = b.begin(); bz != b.end() ; ++bz) {
          int r = rand() % 100;
          if (r < m) {
            m -= r;
            if (rand() % 7) {
              bz->mutace(r);
            } else {
              if (rand() % 2) {
                std::list<Bazmek>::iterator bz2 = bz;
                bz++;
                b.erase(bz2);
                bz--;
              } else {
                Bazmek bz;
                bz.nahodne();
                b.push_back(bz);
              }
            }
          }
        }
/*        if (b.size() == 1) {
          odstran_prebytecne();
        }*/
      } else {
        if (umisteni == SERIOVE && druh == KONDENZATOR) {
          druh = CIVKA;
          hodnota = 100 / (hodnota + 1);
        } else if (umisteni == PARALELNI && druh == CIVKA) {
          druh = KONDENZATOR;
          hodnota = 1000000 / (hodnota + 1);
        }
      }
      break;
    case 3:
      hodnota *= float(rand() % 100 + 1) / 10;
      if (hodnota < 1) {
        hodnota += 1;
      }
      break;
  }
}
