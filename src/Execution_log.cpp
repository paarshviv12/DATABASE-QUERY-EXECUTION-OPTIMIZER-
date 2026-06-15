namespace ftxui {

inline Element StackStage(float p, const std::array<std::string, 3>& ops) {
  int pushed = std::min(3, (int)(seg(p, 0.f, 0.85f) * 3) + 1);
  Elements col;
  col.push_back(text("Execution Stack") | bold | color(C_PROC));
  col.push_back(text(""));
  if (pushed >= 1) col.push_back(text("TOP") | color(C_OFF));
  for (int i = pushed - 1; i >= 0; --i)
    col.push_back(staticBox(ops[i], C_PROC, false, 18));
  return vbox(std::move(col));
}

inline const std::vector<std::string>& RollbackOps() {
  static const std::vector<std::string> ops = {"Scan Students", "Filter Marks",
                                                "Join Courses", "Join Results"};
  return ops;
}
constexpr long long kRbCycle = 20000;
inline int RbState(long long ms, int& onStack, int& undoing) {
  const auto& ops = RollbackOps();
  const int n = (int)ops.size();
  float p = (float)(ms % kRbCycle) / kRbCycle;
  undoing = -1;
  if (p < 0.34f) {
    onStack = std::min(n, (int)(p / 0.34f * n) + 1);
    return 0;
  }
  if (p < 0.46f) {
    onStack = n;
    return 1;
  }
  if (p < 0.86f) {
    int popped = std::min(n, (int)((p - 0.46f) / 0.40f * n) + 1);
    onStack = std::max(0, n - popped);
    undoing = (onStack < n) ? onStack : -1;
    return 2;
  }
  onStack = 0;
  return 3;
}

inline Element RollbackView(long long ms) {
  const auto& ops = RollbackOps();
  const int n = (int)ops.size();
  int onStack = 0, undoing = -1;
  int phase = RbState(ms, onStack, undoing);

  Elements col;
  col.push_back(text("EXECUTION STACK") | bold | color(C_PROC) | hcenter);
  col.push_back(text(""));
  if (onStack > 0)
    col.push_back(text("TOP ↑") | color(C_OFF) | hcenter);
  for (int i = onStack - 1; i >= 0; --i) {
    bool failed = (phase == 1 && i == n - 1);
    col.push_back(staticBox(ops[i], failed ? C_ERR : (phase == 2 ? C_DEC : C_PROC),
                            failed, 22) |
                  hcenter);
  }
  if (onStack == 0)
    col.push_back(text("[ stack empty — clean state ]") | color(C_OFF) | hcenter);
  col.push_back(text(""));

  Element status;
  if (phase == 0)
    status = text(spinner(ms) + "  running query — pushing operations...") |
             color(C_PROC);
  else if (phase == 1)
    status = text("✗  ERROR while running:  " + ops[n - 1] +
                  "   (out of memory)") |
             bold | color(C_ERR);
  else if (phase == 2)
    status = text("↩  rolling back — undo:  " +
                  (undoing >= 0 && undoing < n ? ops[undoing] : ops[0])) |
             bold | color(C_DEC);
  else
    status = text("✓  rolled back to last stable state — data consistent") |
             bold | color(C_OK);
  col.push_back(status | hcenter);
  return vbox(std::move(col)) | vcenter;
}

inline Element RollbackExplain(long long ms) {
  int onStack = 0, undoing = -1;
  int phase = RbState(ms, onStack, undoing);
  const char* msg;
  switch (phase) {
    case 0:
      msg = "As the query runs, every operation it performs is PUSHED onto an "
            "execution stack — a running record of the work done so far.";
      break;
    case 1:
      msg = "Uh oh — an operation just failed. If we stopped here, the "
            "half-finished work would leave the database in a broken state.";
      break;
    case 2:
      msg = "So we ROLL BACK: pop each operation off the stack and undo it, "
            "newest first. The stack is LIFO, so undo happens in exact reverse "
            "order.";
      break;
    default:
      msg = "Done — we're back at the last stable state. Nothing half-done is "
            "left behind, and the database stays consistent.";
      break;
  }
  Element guide = hbox({
      vbox({text("HOW ROLLBACK WORKS") | bold | color(C_DEC),
            text("error handling, explained") | color(Color::GrayLight)}),
      filler(),
      Bunny(ms),
  });
  return vbox({guide, text(""), paragraph(msg) | color(Color::White)});
}

inline int RbPhase(long long ms) {
  int a = 0, b = 0;
  return RbState(ms, a, b);
}

inline Element RbBanner(long long ms) {
  bool flash = ((ms / 300) % 2) == 0;
  Color bg = flash ? Color::Red : Color::RedLight;
  auto bar = [&](Element e) { return (e | hcenter | bgcolor(bg)) | xflex; };
  return vbox({
             bar(text(" ")),
             bar(text(" ")),
             bar(text("⚠    OOPS!   AN OPERATION FAILED    ⚠") | bold |
                 color(Color::White)),
             bar(text(" ")),
             bar(text(" ")),
         }) |
         xflex;
}

}
