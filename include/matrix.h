#pragma once
#include <stdint.h>
#include <math.h>

namespace maths
{
    template<size_t ROW, size_t COL>
    struct matrix {
        enum matrix_type {
            ZERO,
            IDENTITY
        };

        matrix(std::initializer_list<std::initializer_list<float>> init) {
            size_t i = 0;
            for (auto& row : init) {
                size_t j = 0;
                for (auto& value : row) {
                    data[i][j] = value;
                    ++j;
                }
                ++i;
            }
        }

        matrix(float value = 0) {
            for (size_t i = 0; i < ROW; ++i) for (size_t j = 0; j < COL; ++j)
			    data[i][j] = value;
        }
        matrix(matrix_type type) {
            switch (type)
            {
            case IDENTITY:
                for (size_t i = 0; i < ROW; ++i) for (size_t j = 0; j < COL; ++j)
                    data[i][j] = (i == j) ? 1 : 0;
                break;
            default:
                for (size_t i = 0; i < ROW; ++i) for (size_t j = 0; j < COL; ++j)
                    data[i][j] = 0;
            //case ZERO:
                break;
            }    
        }
        matrix(matrix&&) = default;
        matrix(matrix const&) = default;

        matrix& operator = (matrix&&) = default;
        matrix& operator = (matrix const&) = default;

        matrix& operator = (matrix_type type) {
            switch (type)
            {
            case IDENTITY:
                for (size_t i = 0; i < ROW; ++i) for (size_t j = 0; j < COL; ++j)
                    data[i][j] = (i == j) ? 1 : 0;
                break;
            default:
                for (size_t i = 0; i < ROW; ++i) for (size_t j = 0; j < COL; ++j)
                    data[i][j] = 0;
            //case ZERO:
                break;
            }
			return *this;
        }

        template<size_t M, size_t N>
        float & get() {
            static_assert(M < ROW && N < COL, "Index out of bounds");
            return data[M][N];
		}
        template<size_t M, size_t N>
        float const& get()const {
            static_assert(M < ROW && N < COL, "Index out of bounds");
            return data[M][N];
        }
        template<size_t M, size_t N>
        void set(float value) {
            static_assert(M < ROW && N < COL, "Index out of bounds");
            data[M][N] = value;
        }

        float& operator ()(size_t i, size_t j) {
            return data[i][j];
        }
        float const& operator()(size_t i, size_t j) const {
            return data[i][j];
        }

    private:
		float data[ROW][COL];
    };
}

namespace maths
{
    template<size_t M, size_t N>
    inline matrix<M, N>& zero(matrix<M, N>& m) {
		m = matrix<M, N>::ZERO;
        return m;
    }

    template<size_t M>
    inline matrix<M, M>& identity(matrix<M, M>& m) {
        m = matrix<M, M>::IDENTITY;
        return m;
    }

    template<size_t M, size_t N, size_t O>
    inline matrix<M, O> mul(matrix<M, N> const& a, matrix<N, O> const& b) {
        matrix<M, O> out(0);
        for (int i = 0; i < M; i++) {
            for (int k = 0; k < O; k++) {
                for (int j = 0; j < N; j++) {
                    out(i, k) += a(i, j) * b(j, k);
                }
            }
        }
        return out;
    }

    template<size_t M, size_t N>
    inline matrix<M, N> mul(matrix<M, N> const& a, float const& b) {
        matrix<M, N> out(0);
        for (int i = 0; i < M; i++) {
            for (int k = 0; k < N; k++) {
                out(i, k) = a(i, k) * b;
            }
        }
        return out;
    }

    template<size_t M, size_t N>
    inline matrix<M, N> mul(float const& a, matrix<M, N> const& b) {
        matrix<M, N> out(0);
        for (int i = 0; i < M; i++) {
            for (int k = 0; k < N; k++) {
                out(i, k) = a * b(i, k);
            }
        }
        return out;
    }

    template<size_t M, size_t N>
    inline matrix<M, N> add(matrix<M, N> const& a, matrix<M, N> const& b) {
        matrix<M, N> out;
        for (int i = 0; i < M; i++) {
            for (int k = 0; k < N; k++) {
                out(i, k) = a(i, k) + b(i, k);
            }
        }
        return out;
	}

    template<size_t M, size_t N>
    inline matrix<M, N> sub(matrix<M, N> const& a, matrix<M, N> const& b) {
        matrix<M, N> out;
        for (int i = 0; i < M; i++) {
            for (int k = 0; k < N; k++) {
                out(i, k) = a(i, k) - b(i, k);
            }
        }
        return out;
    }

    // Transpuesta
    template<size_t M>
    inline matrix<M, M>& transpose(matrix<M, M>& m) {
        for (int i = 0; i < M; i++) {
            for (int j = i + 1; j < M; j++) {
                float tmp = m(i, j);
                m(i, j) = m(j, i);
                m(j, i) = tmp;
            }
        }
        return m;
    }

    // Determinante (recursivo por expansión de cofactores, no muy eficiente)
    template<size_t M>
    inline float det(matrix<M, M> const& m) {
        if (M == 1) {
            return m(0, 0);
        }
        else if (M == 2) {
            return m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0);
        }
        else {
            float d = 0;
            for (int j = 0; j < M; j++) {
                // construir submatriz (M-1)x(M-1)
                matrix<M - 1, M - 1> sub;
                for (int r = 1; r < M; r++) {
                    int colIndex = 0;
                    for (int c = 0; c < M; c++) {
                        if (c == j) continue;
                        sub(r - 1, colIndex) = m(r, c);
                        colIndex++;
                    }
                }
                float cofactor = ((j % 2 == 0) ? 1 : -1) * m(0, j) * det(sub);
                d += cofactor;
            }
            return d;
        }
    }

    // Inversa (usando adjunta y determinante)
    template<size_t M>
    inline matrix<M, M> invert(matrix<M, M> const& m) {
        float d = det(m);
        if (d == 0) throw std::runtime_error("Matrix not invertible");

        matrix<M, M> out(0);
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < M; j++) {
                // submatriz
                matrix<M - 1, M - 1> sub;
                int rIndex = 0;
                for (int r = 0; r < M; r++) {
                    if (r == i) continue;
                    int cIndex = 0;
                    for (int c = 0; c < M; c++) {
                        if (c == j) continue;
                        sub(rIndex, cIndex) = m(r, c);
                        cIndex++;
                    }
                    rIndex++;
                }
                float cofactor = (((i + j) % 2 == 0) ? 1 : -1) * det(sub);
                out(j, i) = cofactor / d; // transpuesta de cofactores
            }
        }
        return out;
    }

    template<size_t M>
    inline matrix<M, M> pow(matrix<M, M> const& m, int n) {
        if (n == 0) {
            return matrix<M, M>::IDENTITY;
        }
        else if (n > 0) {
            matrix<M, M> result = m;
            for (int i = 1; i < n; i++) {
                result = result * m;
            }
            return result;
        }
        else {
            return invert(pow(m, -n));
        }
    }
}

namespace maths
{
    template<size_t M, size_t N, size_t O>
    inline matrix<M, O> operator * (matrix<M, N> const& a, matrix<N, O> const& b) {
        return mul(a, b);
	}
    template<size_t M, size_t N>
    inline matrix<M, N> operator * (matrix<M, N> const& a, float const& b) {
        return mul(a, b);
    }
    template<size_t M, size_t N>
    inline matrix<M, N> operator * (float const& a, matrix<M, N> const& b) {
        return mul(a, b);
    }
    template<size_t M, size_t N>
    inline matrix<M, N> operator + (matrix<M, N> const& a, matrix<M, N> const& b) {
        return add(a, b);
    }
    template<size_t M, size_t N>
    inline matrix<M, N> operator - (matrix<M, N> const& a, matrix<M, N> const& b) {
        return sub(a, b);
	}

    template<size_t M>
    inline matrix<M, M> operator ~ (matrix<M, M> const& m) {
        return transpose(matrix<M, M>(m));
	}
}
