template<typename Info>
struct ModuloImpl {
  typename Info::ValueType value;

  template<std::integral I>
  ModuloImpl(I const&);

  ModuloImpl<Info>& operator+=(
    ModuloImpl<Info> const& rhs
  ) {
    value += rhs.value;
    if(value >= Info::modulus)
      value -= Info::modulus;

    return *this;
  }
};
