namespace ftxui {

inline Element CacheStage(float p, long long ms, const std::string& title) {
  const int N = 6;
  int scan = (int)(seg(p, 0.f, 0.8f) * N);
  Elements cells;
  for (int i = 0; i < N; ++i) {
    if (i) cells.push_back(text(" "));
    bool on = i < scan;
    cells.push_back(text(on ? "█" : "□") | color(on ? C_PROC : C_OFF));
  }
  Element res = p < 0.82f
                    ? (text(spinner(ms) + " Checking cache...") | color(C_PROC))
                    : (text("CACHE MISS") | bold | color(C_ERR));
  return vbox({text(title) | bold | color(C_PROC), text(""),
               hbox(std::move(cells)) | hcenter, text(""), res});
}

}
