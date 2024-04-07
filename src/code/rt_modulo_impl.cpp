template<typename ModInfo>
struct ModuloImpl {
  typename ModInfo::ValueType value;

  ModuloImpl<ModInfo>& operator+=(
    ModuloImpl<ModInfo> const& rhs
  ) {
    value += rhs.value;
    if(value >= ModInfo::modulus)
      value -= ModInfo::modulus;

    return *this;
  }

  ModuloImpl<ModInfo>& operator*=(
    ModuloImpl<ModInfo> const& rhs
  ) {
    ModInfo::product(value, rhs.value);
    return *this;
  }
};
