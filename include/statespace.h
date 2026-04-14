#pragma once

#include "matrix.h"

namespace sss
{
    using namespace maths;
    template<size_t N, size_t IN, size_t OUT>
    class StateSpace {
    public:	 
        StateSpace()
            : A(matrix<N, N>::ZERO)
            , B(matrix<N, IN>::ZERO)
            , C(matrix<OUT, N>::ZERO)
            , D(matrix<OUT, IN>::ZERO)
            , x(matrix<N, 1>::ZERO)
            , y(matrix<OUT, 1>::ZERO)
        {
        }

        matrix<N, N> const& getA()const { return this->A; }
        matrix<N, IN> const& getB()const { return this->B; }
        matrix<OUT, N> const& getC()const { return this->C; }
        matrix<OUT, IN> const& getD()const { return this->D; }

        void setA(matrix<N, N> const& value) { this->A = value; }
        void setB(matrix<N, IN> const& value) { this->B = value; }
        void setC(matrix<OUT, N> const& value) { this->C = value; }
        void setD(matrix<OUT, IN> const& value) { this->D = value; }

        matrix<N, 1>& getState() { return this->x; }
        matrix<N, 1> const& getState() const { return this->x; }
        matrix<OUT, 1>& getOutput() { return this->y; }
        matrix<OUT, 1> const& getOutput() const { return this->y; }

        void reset() {
            x = matrix<N, 1>::ZERO;
            y = matrix<OUT, 1>::ZERO;
        }

        void updateEuler(float dt, matrix<IN, 1> const& u) {
            matrix<N, 1> k1 = fs(x, u);
            x = x + k1 * dt;
            y = fo(x, u);
        }
        void updateLeapfrog(float dt, matrix<IN, 1> const& u) {
            matrix<N, 1> k1 = fs(x, u);
            matrix<N, 1> x_half = x + k1 * (dt * 0.5f);
            matrix<N, 1> k2 = fs(x_half, u);
            x = x + k2 * dt;
            y = fo(x, u);
        }
        void updateRK4(float dt, matrix<IN, 1> const& u) {
            matrix<N, 1> k1 = fs(x, u);
            matrix<N, 1> k2 = fs(x + k1 * (dt / 2.0f), u);
            matrix<N, 1> k3 = fs(x + k2 * (dt / 2.0f), u);
            matrix<N, 1> k4 = fs(x + k3 * dt, u);
            x = x + (k1 + 2 * k2 + 2 * k3 + k4) * (dt / 6.0f);
            y = fo(x, u);
        }

    private:
        inline matrix<N, 1> fs(matrix<N, 1> const& x, matrix<IN, 1> const& u) { return A * x + B * u; }
        inline matrix<OUT, 1> fo(matrix<N, 1> const& x, matrix<IN, 1> const& u) { return C * x + D * u; }

    private:
        /*
            x' = A*x + B*u
            y = C*x + D*u
        */
        matrix<N, N> A;
        matrix<N, IN> B;
        matrix<OUT, N> C;
        matrix<OUT, IN> D;
        matrix<N, 1> x; // actual state
        matrix<OUT, 1> y; // actual output
    };
}