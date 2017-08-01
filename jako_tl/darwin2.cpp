#include "darwin2.h"

#include <qstring.h>
#include <cmath>
#include <tuple>

#include <iostream>

std::vector<Darwin2::Zadani> Darwin2::zadani;

const float E12[12] = {1.0, 1.2, 1.5, 1.8, 2.2, 2.7, 3.3, 3.9, 4.7, 5.6, 6.8, 8.2};
const float rady[8] = {1E-12, 1E-11, 1E-10, 1E-9, 1E-8, 1E-7, 1E-6, 1E-5};
const char koncovky[17] = {'y', 'z', 'a', 'f', 'p', 'n', 'u', 'm' ,' ', 'k', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y'};

Darwin2::Darwin2() :
  populace(),
  generace(0),
  max_vyber(100),
  mutacni_index(20)
{
}

std::string Darwin2::Bazmek::lidsky() {
  bool co = true;
  std::string result = "ANT";
  int hloubka = 0;
  bool je_civka;
  for (unsigned int i = 0; i < houby.size(); i++) {
    if (co) {
      result += "> ";
      if (hloubka == 0 && (houby[i] & 0b00001000)) {
        result += "napříč ";
      }
      switch ((houby[i] & 0b00110000)>>4) {
        case 0b00:
          //result += "kondik ";
          je_civka = false;
          co = false;
          break;
        case 0b01:
          //result += "cívka ";
          je_civka = true;
          co = false;
          break;
        case 0b10:
          result += "sériově(";
          hloubka++;
          break;
        case 0b11:
          result += "paralelně(";
          hloubka++;
          break;
      }
    } else {
      co = true;
      char hodnota = (houby[i] & 0b11110000)>>4;
      char rad = houby[i] & 0b00000111;
      if (hodnota >= 12 || (!je_civka && rad >= 5) || (je_civka && rad <= 2)) {
        if (hloubka > 0) {
          hloubka--;
          result += ") ";
        }
        continue;
      }

      float h = E12[int(hodnota)] * rady[int(rad)];

      QString str1;
      int koncovka = 8;
      while (h < 1 && koncovka >= 0) {
        h *= 1000;
        koncovka--;
      }
      str1.setNum(h);
      result += str1.toStdString() + koncovky[koncovka] + (je_civka ? "H" : "F");
    }
  }
  while (hloubka > 0) {
    hloubka--;
    result += ")";
  }
  return result;
}

void Darwin2::Bazmek::ohodnot() {
  swr = 1;
  for (std::vector<Zadani>::iterator it = zadani.begin(); it != zadani.end(); ++it) {
    float Zr = it->Zlr;
    float Zi = it->Zli;
    transformuj(Zr, Zi, it->f);
    //float swr_ = spocitej_swr(Zr, Zi, 50);
    float swr_ = -(Zr - 50)*(50/Zr - 1);
    if (swr_!=swr_ || swr_ >= 256) {
      swr_ = 256;
    }
    swr *= swr_;
  }
  hodnota = swr + houby.size()/10;
}

float Darwin2::Bazmek::dej_cast(unsigned int &i, bool paralel, float f) {
  bool co = true;
  bool je_civka;
  float Zi = paralel ? nan("") : 0;
  float imp;
  bool new_imp = false;
  while (i < houby.size()) {
    if (co) {
      if (houby[i] & 0b00100000) {
        i++;
        imp = dej_cast(i, houby[i] & 0b00010000, f);
        new_imp = imp == imp;
      } else {
        je_civka = houby[i] & 0b00010000;
        new_imp = false;
        co = false;
      }
    } else {
      co = true;
      char hodnota = (houby[i] & 0b11110000)>>4;
      char rad = houby[i] & 0b00000111;
      if (hodnota >= 12 || (!je_civka && rad >= 5) || (je_civka && rad <= 2)) {
        return Zi;
      } else {
        imp = E12[int(hodnota)] * rady[int(rad)];
        if (je_civka) {
          imp = civka(imp, f);
        } else {
          imp = kondik(imp, f);
        }
        new_imp = true;
      }
    }

    if (new_imp) {
      if (paralel) {
        Zi = paralelne(Zi, imp);
      } else {
        Zi = seriove(Zi, imp);
      }
    }
    i++;
  }
  return Zi;
}

void Darwin2::Bazmek::transformuj(float &Zr, float &Zi, float f) {
  bool co = true;
  bool napric = false;
  bool je_civka;
  float imp;
  bool new_imp = false;
  unsigned int i = 0;
  while (i < houby.size()) {
    if (co) {
      if (houby[i] & 0b00001000) {
        napric = true;
      }

      if (houby[i] & 0b00100000) {
        i++;
        imp = dej_cast(i, houby[i] & 0b00010000, f);
        new_imp = imp == imp;
      } else {
        je_civka = houby[i] & 0b00010000;
        new_imp = false;
        co = false;
      }
    } else {
      co = true;
      char hodnota = (houby[i] & 0b11110000)>>4;
      char rad = houby[i] & 0b00000111;
      if (hodnota >= 12 || (!je_civka && rad >= 5) || (je_civka && rad <= 2)) {
        new_imp = false;
      } else {
        imp = E12[int(hodnota)] * rady[int(rad)];
        if (je_civka) {
          imp = civka(imp, f);
        } else {
          imp = kondik(imp, f);
        }
        new_imp = true;
      }
    }

    if (new_imp) {
      if (napric) {
        float E = Zi + imp;
        float D = Zr*Zr + E*E;
        float Zr2 = Zr*imp*imp/D;
        Zi = imp*(Zr*Zr + Zi*Zi + Zi*imp)/D;
        Zr = Zr2;
      } else {
        Zi += imp;
      }
    }
    i++;
  }
}

void Darwin2::pridej_tvory(int pocet) {
  for (int i = pocet; i; i--) {
    Bazmek tvor;
    int l = (rand() % 10) + 4;
    while (l) {
      char c = rand();
      tvor.houby.push_back(c);
      l--;
    }
    populace.push_back(tvor);
  }
}

void Darwin2::vypis_populaci() {
  std::cout << "Výpis " << generace << ". generace:" << std::endl;
  for (std::vector<Bazmek>::iterator tvor = populace.begin(); tvor != populace.end() ; ++tvor) {
    std::cout << tvor->lidsky().c_str() << tvor->hodnota << " " << tvor->swr << " " << std::endl;
  }
}

void Darwin2::mutace() {
  for (std::vector<Bazmek>::iterator tvor = populace.begin(); tvor != populace.end() ; ++tvor) {
    for (unsigned int i = 0; i < tvor->houby.size(); i++) {
      if (rand() % 1875 < 10) {
        char c = 1 << (rand() % 8);
        tvor->houby[i] = tvor->houby[i] ^ c;
      }
    }
  }
}

void Darwin2::serad() {
  std::sort(populace.begin(), populace.end(), je_lepsi);
  populace.resize(std::min(max_vyber, int(populace.size())));
}

void Darwin2::ohodnot() {
  for (std::vector<Bazmek>::iterator tvor = populace.begin(); tvor != populace.end() ; ++tvor) {
    tvor->ohodnot();
  }
}

void Darwin2::krizeni() {
  for (int i = max_vyber / 2; i; i--) {
    int a = rand() % populace.size();
    int b = rand() % populace.size();
    if (a == b) continue;

    //populace[a].krizeni(populace[b]);
    int l1 = populace[a].houby.size();
    int l2 = populace[b].houby.size();
    int p1 = rand() % l1;
    int p2 = rand() % l2;
    std::string s11, s12, s21, s22;
    s11 = populace[a].houby.substr(0, p1);
    s12 = populace[a].houby.substr(p1, l1 - p1);
    s21 = populace[b].houby.substr(0, p2);
    s22 = populace[b].houby.substr(p2, l2 - p2);

    populace[a].houby = s11 + s22;
    populace[b].houby = s21 + s12;
  }
}

void Darwin2::rozmnozuj() {
  for (int i = populace.size(); i; i--) {
    int a = rand() % populace.size();
    populace.push_back(populace[a]);
  }
}

bool Darwin2::je_lepsi(const Bazmek &A, const Bazmek &B) {
  return A.hodnota < B.hodnota;
}

float Darwin2::kondik(float c, float f) {
  f *= 2E6 * M_PI;
  return -1/(c*f);
}

float Darwin2::civka(float l, float f) {
  f *= 2E6 * M_PI;
  return l*f;
}

float Darwin2::paralelne(float a, float b) {
  return 1/(1/a + 1/b);
}

float Darwin2::seriove(float a, float b) {
  return a+b;
}

float Darwin2::spocitej_swr(float Zlr, float Zli, float ref) {
  if (Zlr != Zlr || Zli != Zli) {
    return nan("");
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
    return nan("");
  } else {
    return (1 + G) / (1 - G);
  }
}
