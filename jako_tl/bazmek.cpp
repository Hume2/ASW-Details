#include <math.h>

#include "bazmek.h"

const float PI_KOEF = M_PI / 500000;

Bazmek::Bazmek() :
  b(),
  hodnota(),
  druh(),
  zapojeni()
{

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
