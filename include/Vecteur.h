#ifndef VECTEUR_H
#define VECTEUR_H


class Vecteur
{
    public:
        Vecteur(long double x, long double y);
        virtual ~Vecteur();
        void minimum(Vecteur vec);
        void maximum(Vecteur vec);
        Vecteur midpoint(Vecteur vec);
        Vecteur clone();
        void normalize();
        Vecteur normalized();
        void negate();
        long double sqrLength();
        long double length();
        void scale(long double len);
        void add(Vecteur vec);
        void sub(Vecteur vec);
        Vecteur diff(Vecteur vec);
        long double distance(Vecteur vec);
        long double dot(Vecteur vec);
        bool equals(Vecteur vec);
        Vecteur orthogonal();
        long double distanceToLine(Vecteur v1, Vecteur v2);

        long double x;
        long double y;
    protected:
    private:
};

#endif // VECTEUR_H
