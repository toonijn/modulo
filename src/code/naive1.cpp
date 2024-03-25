template<int modulus>
struct Modulo {
    int value;

    friend Modulo<modulus> operator+(Modulo<modulus> lhs, Modulo<modulus> rhs) {
        return Modulo<modulus>((lhs.value + rhs.value) % modulus);
    }

    friend Modulo<modulus> operator*(Modulo<modulus> lhs, Modulo<modulus> rhs) {
        return Modulo<modulus>(lhs.value * rhs.value % modulus);
    }
};
