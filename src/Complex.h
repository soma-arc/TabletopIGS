#ifndef INCLUDED_Complex_h_
#define INCLUDED_Complex_h_

#include <iostream>
#include <math.h>

using namespace std;

class Complex{
 public:
  Complex();
  Complex(double re);
  Complex(double re, double im);
  void set(double re, double im);
  double re();
  double im();

  Complex operator+(Complex c);
  Complex operator-(Complex c);
  Complex operator*(Complex c);
  Complex operator/(Complex c);

  Complex conjugation();
  static double abs(Complex c);
  static Complex sqrt(Complex c);

  void print(ostream *os);

  static Complex ONE, ZERO, I;
 private:
  double real;
  double image;
};

extern ostream &operator<<(ostream&, Complex&);
#endif
