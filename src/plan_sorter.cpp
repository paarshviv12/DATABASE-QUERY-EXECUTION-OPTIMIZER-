namespace ftxui {

inline Element PlansStage(float p, const std::array<std::string, 3>& lbl) {
  int costs[3] = {120, 40, 10};
  Elements col;
  col.push_back(text("Generating Plans...") | bold | color(C_DEC));
  col.push_back(text(""));
  for (int i = 0; i < 3; ++i) {
    float cp = seg(p, i * 0.25f, i * 0.25f + 0.22f);
    if (cp <= 0.f) continue;
    int sh = (int)(costs[i] * cp);
    bool best = (i == 2) && p > 0.85f;
    col.push_back(dashedBorder(
        hbox({text(" " + std::string(1, char('A' + i)) + "  ") | bold,
              text(lbl[i]) | color(Color::White), filler(),
              text("  Cost " + std::to_string(sh) + " ")}),
        best ? C_OK : C_PROC));
  }
  return vbox(std::move(col));
}

inline Element HeapStage(float p, long long ms, const std::string& bestLabel) {
  bool chosen = p > 0.55f;
  return vbox({
      text("Priority Queue (min-heap)") | bold | color(C_DEC),
      text(""),
      (text("      [10]") | bold | color(chosen ? C_OK : C_DEC)) | hcenter,
      (text("     /    \\") | color(C_OFF)) | hcenter,
      (text("  [40]    [120]") | color(C_OFF)) | hcenter,
      text(""),
      chosen ? (text("Best Plan:  " + bestLabel + "  (cost 10)") | bold |
                color(C_OK))
             : (text(spinner(ms) + " comparing plans...") | color(C_DEC)),
  });
}

}
