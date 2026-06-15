namespace ftxui {

inline Element QueueStage(float p, long long) {
  std::vector<std::string> tasks = {"Index", "Stats", "Cleanup", "Vacuum"};
  int done = (int)(seg(p, 0.f, 0.9f) * tasks.size());
  Elements row;
  for (int i = done; i < (int)tasks.size(); ++i) {
    if (i > done) row.push_back(text(" "));
    row.push_back(staticBox("[" + tasks[i] + "]", C_PROC, false, 8));
  }
  Element belt = row.empty() ? (text("queue drained") | color(C_OK))
                             : hbox(std::move(row));
  std::string cur = done < (int)tasks.size() ? tasks[done] : "—";
  return vbox({text("Worker Pipeline  (FIFO Queue)") | bold | color(C_PROC),
               text(""), belt | hcenter, text(""),
               text("→ processing: " + cur) | color(C_DEC) | hcenter});
}

}
