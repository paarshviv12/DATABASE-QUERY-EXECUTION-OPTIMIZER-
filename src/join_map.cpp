namespace ftxui {

inline Element GraphStage(float p, const std::string& top,
                          const std::string& left, const std::string& right) {
  Elements col;
  col.push_back(text("Relationship Graph") | bold | color(C_PROC));
  col.push_back(text(""));
  col.push_back((text(top) | bold | color(p > 0.1f ? C_PROC : C_OFF)) | hcenter);
  col.push_back((text("/    \\") | color(p > 0.3f ? C_OK : C_OFF)) | hcenter);
  col.push_back(hbox({text(left) | color(p > 0.45f ? C_PROC : C_OFF),
                      text("   "),
                      text(right) | color(p > 0.6f ? C_PROC : C_OFF)}) |
                hcenter);
  col.push_back(text(""));
  if (p > 0.4f) col.push_back(text(top + " → " + left) | color(C_OK));
  if (p > 0.65f) col.push_back(text(top + " → " + right) | color(C_OK));
  return vbox(std::move(col));
}

}
