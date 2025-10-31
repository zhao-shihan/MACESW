namespace MACE::SmearMACE {

template<Mustard::Data::TupleModelizable... Ts>
auto Smearer::Smear(std::string_view treeName, const muc::flat_hash_map<std::string, std::string>& smearingConfig) const -> void {
    Mustard::Data::Output<Ts...> output{std::string{treeName}};

    std::vector<std::pair<std::string_view, TF1>> smearAction;
    smearAction.reserve(smearingConfig.size());
    for (auto&& [var, smearFormula] : smearingConfig) {
        smearAction.emplace_back(var, TF1{fmt::format("{}Smearer", var).c_str(), smearFormula.c_str()});
    }

    fProcessor->Process<Ts...>(
        ROOT::RDataFrame{treeName, fInputFile}, int{}, "EvtID",
        [&](bool byPass, auto&& event) {
            if (byPass) {
                return;
            }
            for (auto&& entry : event) {
                for (auto&& [var, smear] : smearAction) {
                    entry->Visit(var, [&](muc::arithmetic auto& x) { x = smear(x); });
                }
                output.Fill(*entry);
            }
        });

    output.Write();
}

} // namespace MACE::SmearMACE
