namespace MACE::inline Utility {

template<int D>
auto WriteAutocorrelationFunction(const std::vector<std::pair<unsigned, Eigen::Array<double, D, 1>>>& autocorrelationFunction) -> void {
    std::array<TGraph, D> graph;
    for (int i{}; i < D; ++i) {
        graph[i].SetName(fmt::format("ACF_u{}", i).c_str());
        graph[i].SetTitle(fmt::format("Autocorrelation function of u{}", i).c_str());
        graph[i].GetXaxis()->SetTitle("Lag");
        graph[i].GetYaxis()->SetTitle("Autocorrelation");
    }
    for (auto&& [lag, autocorrelation] : autocorrelationFunction) {
        for (int i{}; i < D; ++i) {
            graph[i].AddPoint(lag, autocorrelation[i]);
        }
    }
    for (int i{}; i < D; ++i) {
        graph[i].SetOption("P");
        graph[i].Write();
    }
}

} // namespace MACE::inline Utility
