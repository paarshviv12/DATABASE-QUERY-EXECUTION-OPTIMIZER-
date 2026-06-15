namespace ftxui {

inline Element DijkstraStage(float p, long long ms, const std::string& top,
                             const std::string& left, const std::string& right) {
  bool e1 = p > 0.3f, e2 = p > 0.55f, fin = p > 0.8f;
  auto dot = [](Color c) { return text("●") | bold | color(c); };
  Color cT = fin ? C_OK : (p > 0.1f ? C_PROC : C_OFF);
  Color cL = fin ? C_OK : (e1 ? C_PROC : C_OFF);
  Color cR = fin ? C_OK : (e2 ? C_PROC : C_OFF);
  Color ep = fin ? C_OK : (e1 ? C_PROC : C_OFF);
  Elements col;
  col.push_back(text("Dijkstra Retrieval Route") | bold | color(C_DEC));
  col.push_back(text(""));
  col.push_back(hbox({text("        "), dot(cT), text(" " + top) | color(cT)}));
  col.push_back(hbox({text("       "), text("2") | color(ep),
                      text("/ \\") | color(C_OFF), text("5") | color(C_OFF)}));
  col.push_back(hbox({dot(cL), text(" " + left) | color(cL),
                      text(" ─1─ ") | color(fin ? C_OK : C_OFF), dot(cR),
                      text(" " + right) | color(cR)}));
  col.push_back(text(""));
  col.push_back(fin ? (text("Route:  " + top + " → " + left + " → " + right +
                            "  (cost 3)") |
                       bold | color(C_OK))
                    : (text(spinner(ms) + " relaxing edges...") | color(C_PROC)));
  return vbox(std::move(col));
}

}
