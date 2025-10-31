namespace MACE::inline Utility {

template<muc::ceta_string P, muc::ceta_string... Ms>
    requires((P == "polarized" or P == "unpolarized") and sizeof...(Ms) >= 1)
InitialStateCLIModule<P, Ms...>::InitialStateCLIModule(gsl::not_null<Mustard::CLI::CLI<>*> cli) :
    ModuleBase{cli} {
    if constexpr (sizeof...(Ms) == 1) {
        TheCLI()
            ->add_argument("-p", "--momentum")
            .help(fmt::format("Initial state {} momentum", Ms.sv()...))
            .default_value(std::vector{0., 0., 0.})
            .required()
            .nargs(3)
            .template scan<'g', double>();
        if constexpr (P == "polarized") {
            TheCLI()
                ->add_argument("-P", "--polarization")
                .help(fmt::format("Initial state {} polarization vector", Ms.sv()...))
                .default_value(std::vector{0., 0., 0.})
                .required()
                .nargs(3)
                .template scan<'g', double>();
        }
    } else {
        const std::array parent{Ms.sv()...};
        for (std::size_t i{1}; i <= sizeof...(Ms); ++i) {
            TheCLI()
                ->add_argument(fmt::format("-p{}", i), fmt::format("--momentum-{}", i))
                .help(fmt::format("Initial state {} momentum", parent[i - 1]))
                .required()
                .nargs(3)
                .template scan<'g', double>();
        }
        if constexpr (P == "polarized") {
            for (std::size_t i{1}; i <= sizeof...(Ms); ++i) {
                TheCLI()
                    ->add_argument(fmt::format("-P{}", i), fmt::format("--polarization-{}", i))
                    .help(fmt::format("Initial state {} polarization vector", parent[i - 1]))
                    .default_value(std::vector{0., 0., 0.})
                    .required()
                    .nargs(3)
                    .template scan<'g', double>();
            }
        }
    }
}

template<muc::ceta_string P, muc::ceta_string... Ms>
    requires((P == "polarized" or P == "unpolarized") and sizeof...(Ms) >= 1)
auto InitialStateCLIModule<P, Ms...>::Momentum() const -> CLHEP::Hep3Vector
    requires(sizeof...(Ms) == 1)
{
    return To3Vector("--momentum");
}

template<muc::ceta_string P, muc::ceta_string... Ms>
    requires((P == "polarized" or P == "unpolarized") and sizeof...(Ms) >= 1)
auto InitialStateCLIModule<P, Ms...>::Momentum() const -> std::array<CLHEP::Hep3Vector, sizeof...(Ms)>
    requires(sizeof...(Ms) >= 2)
{
    std::array<CLHEP::Hep3Vector, sizeof...(Ms)> p;
    for (auto i{1}; i <= sizeof...(Ms); ++i) {
        p[i] = To3Vector(fmt::format("--momentum-{}", i));
    }
    return p;
}

template<muc::ceta_string P, muc::ceta_string... Ms>
    requires((P == "polarized" or P == "unpolarized") and sizeof...(Ms) >= 1)
auto InitialStateCLIModule<P, Ms...>::Polarization() const -> CLHEP::Hep3Vector
    requires(P == "polarized" and sizeof...(Ms) == 1)
{
    return To3Vector("--polarization");
}

template<muc::ceta_string P, muc::ceta_string... Ms>
    requires((P == "polarized" or P == "unpolarized") and sizeof...(Ms) >= 1)
auto InitialStateCLIModule<P, Ms...>::Polarization() const -> std::array<CLHEP::Hep3Vector, sizeof...(Ms)>
    requires(P == "polarized" and sizeof...(Ms) >= 2)
{
    std::array<CLHEP::Hep3Vector, sizeof...(Ms)> p;
    for (auto i{1}; i <= sizeof...(Ms); ++i) {
        p[i] = To3Vector(fmt::format("--polarization-{}", i));
    }
    return p;
}

template<muc::ceta_string P, muc::ceta_string... Ms>
    requires((P == "polarized" or P == "unpolarized") and sizeof...(Ms) >= 1)
auto InitialStateCLIModule<P, Ms...>::To3Vector(std::string_view option) const -> CLHEP::Hep3Vector {
    const auto vector{TheCLI()->template get<std::vector<double>>(option)};
    return {vector[0], vector[1], vector[2]};
}

} // namespace MACE::inline Utility
