#ifndef INCLUDED_Complex_Cpp_
#define INCLUDED_Complex_Cpp_

#include "Complex.h"

Complex::Complex(){
  real = 0.0;
  image = 0.0;
}

Complex::Complex(double re){
  this->real = re;
  this->image = 0.;
}

Complex::Complex(double re, double im){
  this->real = re;
  this->image = im;
}

void Complex::set(double re, double im){
  this->real = re;
  this->image = im;
}

double Complex::re(){
  return real;
}

double Complex::im(){
  return image;
}

Complex Complex::operator+(Complex c){
  Complex comp;
  double re = real + c.re();
  double im = image + c.im();
  comp.set(re, im);
  return comp;
}

Complex Complex::operator-(Complex c){
  Complex comp;
  double re = real - c.re();
  double im = image - c.im();
  comp.set(re, im);
  return comp;
}

Complex Complex::operator*(Complex c){
  Complex comp;
  double re = real * c.re() - image * c.im();
  double im = real * c.im() + image * c.re();
  comp.set(re, im);
  return comp;
}

Complex Complex::operator/(Complex c){
  double denominator = c.re() * c.re() + c.im() * im();
  Complex comp;
  double re = (real * c.re() + image * c.im()) / denominator;
  double im = (image * c.re() - real * c.im()) / denominator;
  comp.set(re, im);
  return comp;
}

Complex Complex::conjugation(){
  Complex conjugation(real, -1 * image);
  return conjugation;
}

double Complex::abs(Complex c){
  return std::sqrt(c.re() * c.re() + c.im() * c.im());
}

Complex Complex::sqrt(Complex c){
  Complex comp;
  if(c.im() > 0){
    double re = std::sqrt(   c.re() + std::sqrt(c.re() * c.re() + c.im() * c.im())) / std::sqrt(2);
    double im = std::sqrt(-1*c.re() + std::sqrt(c.re() * c.re() + c.im() * c.im())) / std::sqrt(2);
    comp.set(re, im);
    return comp;
  }else if(c.im() < 0){
    return Complex(std::sqrt(c.re() + std::sqrt(c.re() * c.re() + c.im() * c.im())) / std::sqrt(2.0),
		  -std::sqrt(-c.re() + std::sqrt(c.re()*c.re() + c.im() * c.im())) / std::sqrt(2.0));
  }

  if(c.re() < 0){
    return Complex(0.0, std::sqrt(abs(c.re())));
  }

  return Complex(std::sqrt(c.re()), 0.);
}

void Complex::print(ostream *os){
  *os << real << " + " << image << "i";
}

ostream &operator<<(ostream &os,  Complex &c){
  c.print(&os);
  return os;
}

Complex Complex::ONE = Complex(1.);
Complex Complex::ZERO = Complex();
Complex Complex::I = Complex(0., 1.);

#endif
