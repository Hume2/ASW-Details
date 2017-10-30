#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>

#include <iostream>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "darwin2.h"

const float RYCHLOST_SVETLA = 299.792458;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  on_pushButton_clicked();
  srand(time(NULL));
}

MainWindow::~MainWindow()
{
  delete ui;
}

float spocitej_swr(float r, float x, float ref, bool& inf) {
  float D = r + ref;
  D = D * D + x * x;
  float r2 = r * r;
  float x2 = x * x;
  float ref2 = ref * ref;
  float Gr = (r2 + x2 - ref2) / D;
  float Gi = 2 * x * ref / D;
  float G = sqrt(Gr * Gr + Gi * Gi);
  if (G == 1) {
    inf = true;
    return -1;
  } else {
    inf = false;
    return (1 + G) / (1 - G);
  }
}

void spocitej_transformaci(float& Vr, float& Vi, float vlna, float l, float Z0, float Zlr, float Zli) {
  float t = tan(2 * M_PI * l / vlna);
  float t2 = t * t;
  float Zlr2 = Zlr * Zlr;
  float Zli2 = Zli * Zli;
  float Z02 = Z0 * Z0;
  float D = Z0 - Zli * t;
  D = D * D + Zlr * Zlr * t2;
  Vr = Z0 * Zlr * (1 + t2);
  Vi = -t * (Zlr2 + Zli2 - Z02) + Z0 * Zli * (1 - t);
  Vr = Z0 * Vr / D;
  Vi = Z0 * Vi / D;
}

void MainWindow::on_pushButton_clicked()
{
  //Počítej!!!!
  bool h0, h1, h2, h3, h4, h5;
  float Z0 = ui->E_Z0->text().toFloat(&h0);
  float Zlr = ui->E_Zlr->text().toFloat(&h1);
  float Zli = ui->E_Zli->text().toFloat(&h2);
  float l = ui->E_l->text().toFloat(&h3);
  float f = ui->E_f->text().toFloat(&h4);
  float zkr = ui->E_zkr->text().toFloat(&h5);
  if (!h0 || !h1 || !h2 || !h3 || !h4 || !h5 || zkr == 0) {
    return;
  }

  // Spočítej transformaci
  l /= zkr;
  float vlna = RYCHLOST_SVETLA / f;
  float Vr, Vi;
  spocitej_transformaci(Vr, Vi, vlna, l, Z0, Zlr, Zli);
  QString str_Vr, str_Vi, str;
  str_Vr.setNum(Vr);
  str_Vi.setNum(Vi);
  if (Vi >= 0) {
    str = str_Vr + "+" + str_Vi + "j";
  } else {
    str = str_Vr + str_Vi + "j";
  }
  ui->l_vysledek->setText(str);

  // Spočítej SWR
  float swr = spocitej_swr(Vr, Vi, 50, h0);
  if (h0 || swr > 1000) {
    ui->l_swr->setText("ČSV: ∞");
  } else {
    str.setNum(swr);
    str = "ČSV: " + str;
    ui->l_swr->setText(str);
  }

  // Spočítej délku ve vlnách
  float l_zb = fmod(l * 4, vlna) / 4;
  float l_cl = l/vlna;
  float l_4cl = int(l_cl * 4);
  if (l_zb > vlna / 8) {
    l_zb = l_zb - vlna / 4;
    l_4cl++;
  } else if (l_zb < -vlna / 8) {
    l_zb = l_zb + vlna / 4;
    l_4cl--;
  }
  str_Vi.setNum(l_cl, '.', 2);
  str.setNum(l_4cl / 4);
  if (l_zb >= 0) {
    str_Vr.setNum(l_zb, '.', 2);
    str = str_Vi + "λ (" + str + "λ + " + str_Vr + "m)";
  } else {
    str_Vr.setNum(-l_zb, '.', 2);
    str = str_Vi + "λ (" + str + "λ - " + str_Vr + "m)";
  }
  ui->l_vlna->setText(str);
}

void MainWindow::on_pushButton_2_clicked()
{
  //Přidej řádek
  ui->t_vstup->insertRow(ui->t_vstup->rowCount());
  ui->t_vystup->insertRow(ui->t_vystup->rowCount());
  ui->t_vystup_2->insertRow(ui->t_vystup_2->rowCount());
}

void MainWindow::on_pushButton_3_clicked()
{
  //Vypočítat dávku!
  bool h0, h1, h2;
  float Z0 = ui->E_Z0->text().toFloat(&h0);
  float l = ui->E_l->text().toFloat(&h1);
  float zkr = ui->E_zkr->text().toFloat(&h2);
  if (!h0 || !h1 || !h2 || zkr == 0) {
    return;
  }
  for (int i = 0; i < ui->t_vstup->rowCount(); i++) {
    QAbstractItemModel* model = ui->t_vstup->model();
    QModelIndex idx = model->index(i, 0);
    float f = model->data(idx).toFloat(&h0);
    idx = model->index(i, 1);
    float Zlr = model->data(idx).toFloat(&h1);
    idx = model->index(i, 2);
    float Zli = model->data(idx).toFloat(&h2);
    if (!h0 || !h1 || !h2) {
      ui->t_vstup->removeRow(i);
      ui->t_vystup->removeRow(i);
      ui->t_vystup_2->removeRow(i);
      i--;
      continue;
    }

    l /= zkr;
    float vlna = RYCHLOST_SVETLA / f, Vr, Vi;
    spocitej_transformaci(Vr, Vi, vlna, l, Z0, Zlr, Zli);
    float swr = spocitej_swr(Vr, Vi, 50, h0);

    QString str;
    str.setNum(Vr, '.', 2);
    ui->t_vystup->setItem(i, 0, new QTableWidgetItem(str));
    str.setNum(Vi, '.', 2);
    ui->t_vystup->setItem(i, 1, new QTableWidgetItem(str));
    if (h0 || swr > 10000) {
      str = "∞";
    } else {
      str.setNum(swr, '.', 2);
    }
    ui->t_vystup->setItem(i, 2, new QTableWidgetItem(str));
  }
}

void MainWindow::on_pushButton_4_clicked()
{
  //Přizpůsob!
  bool h0, h1, h2;
  Darwin2::zadani.clear();
  for (int i = 0; i < ui->t_vystup->rowCount(); i++) {
    QAbstractItemModel* model = ui->t_vstup->model();
    QAbstractItemModel* model_ = ui->t_vystup->model();
    QModelIndex idx = model->index(i, 0);
    float f = model->data(idx).toFloat(&h0);
    idx = model_->index(i, 0);
    float Zlr = model_->data(idx).toFloat(&h1);
    idx = model_->index(i, 1);
    float Zli = model_->data(idx).toFloat(&h2);
    if (!h0 || !h1 || !h2) {
      break;
    }
    Darwin2::zadani.push_back(Darwin2::Zadani(Zlr, Zli, f));
  }

  Darwin2 d;
  Darwin2::zpusob_vyberu = false;
  d.vyres(10);
  //d.vypis_populaci();

  Darwin2::Bazmek nej = d.populace[0];
  nej.uzavri();
  std::string clovek = nej.lidsky();
  std::cout << "\n\n" << clovek << std::endl;
  //nej.odstran_zbytecne();

  d.nastav_druhe_kolo();
  Darwin2::zpusob_vyberu = true;
  d.populace.clear();
  d.vyres(10);
  //d.vypis_populaci();

  d.populace[0].uzavri();
  clovek = d.populace[0].lidsky();
  std::cout << clovek << std::endl;

  nej.houby += d.populace[0].houby;
  //unsigned int prdlolo = 0;
  //nej.normalizuj(prdlolo);
  clovek = nej.lidsky();
  std::cout << clovek << std::endl;


  for (int i = 0; i < int(Darwin2::zadani.size()); i++) {
    float Zlr2 = Darwin2::zadani[i].Zlr, Zli2 = Darwin2::zadani[i].Zli, f2 = Darwin2::zadani[i].f;
    nej.transformuj(Zlr2, Zli2, f2);
    float swr = Darwin2::spocitej_swr(Zlr2, Zli2, 50);

    QString str;
    str.setNum(Zlr2, '.', 2);
    ui->t_vystup_2->setItem(i, 0, new QTableWidgetItem(str));
    str.setNum(Zli2, '.', 2);
    ui->t_vystup_2->setItem(i, 1, new QTableWidgetItem(str));
    if (swr!=swr || swr >= 256 || swr < 0) {
      str = "∞";
    } else {
      str.setNum(swr, '.', 2);
    }
    ui->t_vystup_2->setItem(i, 2, new QTableWidgetItem(str));
  }
  QString the_str;

  the_str.fromStdString(clovek);
  ui->t_prizpusobeni->setText(the_str);
}
