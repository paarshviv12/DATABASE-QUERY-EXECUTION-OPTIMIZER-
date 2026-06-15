namespace ftxui {

inline Element ResourceStage(float p, long long ms) {
  float cgrow = 0.10f + seg(p, 0.7f, 1.f) * 0.12f;
  int scan = (int)(seg(p, 0.f, 0.65f) * 3);
  auto wk = [&](const char* n, float v, int idx) {
    bool assigned = (idx == 2 && p > 0.7f);
    Color lc = assigned ? C_OK
                        : (idx == scan && p < 0.65f ? C_DEC : Color::White);
    return hbox({text(std::string(n) + " ") | color(lc),
                 gauge(v) | size(WIDTH, EQUAL, 16) |
                     color(assigned ? C_OK : C_PROC),
                 text(" " + std::to_string((int)(v * 100)) + "%") | color(lc)});
  };
  Elements col;
  col.push_back(text("Resource Balancer") | bold | color(C_PROC));
  col.push_back(text(""));
  col.push_back(wk("Server A", 0.8f, 0));
  col.push_back(wk("Server B", 0.4f, 1));
  col.push_back(wk("Server C", cgrow, 2));
  col.push_back(text(""));
  col.push_back(p > 0.7f ? (text("◉  Routed to Server C (least loaded)") |
                            bold | color(C_OK))
                         : (text(spinner(ms) + " scanning servers...") |
                            color(C_DEC)));
  return vbox(std::move(col));
}

}
