#include "Vecteur.h"
#include <cmath>

long double min(long double x, long double y){
    if(x>y){
        return y;
    }
    else{
        return x;
    }
}

long double max(long double x, long double y){
    if(x<y){
        return y;
    }
    else{
        return x;
    }
}

Vecteur::Vecteur(long double x_, long double y_)
{
    x = x_;
    y = y_;
}

Vecteur::~Vecteur()
{
    //dtor
}


void Vecteur::minimum(Vecteur vec){

    x = min(x, vec.x);
    y = min(y, vec.y);

}

void Vecteur::maximum(Vecteur vec){

    x = max(x, vec.x);
    y = max(y, vec.y);

}

Vecteur Vecteur::midpoint(Vecteur vec){

    return  Vecteur((x+vec.x)*0.5, (y+vec.y)*0.5);

}
Vecteur Vecteur::clone(){

    return Vecteur(x, y);

}

void Vecteur::normalize(){

    long double len = this->length();
    x /= len;
    y /= len;

}

Vecteur Vecteur::normalized(){

    Vecteur vec = Vecteur(x, y);
    vec.normalize();
    return vec;

}


void Vecteur::negate(){

    x = -x;
    y = -y;

}


long double Vecteur::sqrLength(){

    return x * x + y * y;

}


long double Vecteur::length(){

    return std::sqrt(x * x + y * y);

}

void Vecteur::scale(long double len){

    x *= len;
    y *= len;

}

void Vecteur::add(Vecteur vec){

    x += vec.x;
    y += vec.y;

}

void Vecteur::sub(Vecteur vec){

    x -= vec.x;
    y -= vec.y;

}
Vecteur Vecteur::diff(Vecteur vec){

    return Vecteur(x-vec.x, y-vec.y);

}


long double Vecteur::distance(Vecteur vec){

    long double x_ = x-vec.x;
    long double y_ = y-vec.y;
    return std::sqrt(x_*x_+y_*y_);

}

long double Vecteur::dot(Vecteur vec){

    return x*vec.x+y*vec.y;

}

bool Vecteur::equals(Vecteur vec){

    return (x == vec.x && y == vec.y);

}

Vecteur Vecteur::orthogonal(){

    return Vecteur(y, -x);

}

long double Vecteur::distanceToLine(Vecteur v0, Vecteur v1){

    long double sqrLen = v1.diff(v0).sqrLength();
    long double u      = ((x-v0.x)*(v1.x-v0.x)+(y-v0.y)*(v1.y-v0.y))/sqrLen;
    Vecteur v1c        = v1.diff(v0);
    v1c.scale(u);
    Vecteur pl = v0.clone();
    pl.add(v1c);
    return this->distance(pl);

}
