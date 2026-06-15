namespace ftxui {

inline Element IndexStageInsta(float p, long long, const InstaCtx& cx) {
  const bool found = cx.matchIdx >= 0;
  const std::string& q = cx.query;
  auto onPath = [&](const std::string& n) {
    if (!found) return false;
    if (n == "rahul") return true;
    if (n == "ananya") return q == "ananya";
    if (n == "virat") return q == "virat" || q == "john";
    if (n == "john") return q == "john";
    return false;
  };
  auto nd = [&](const std::string& n) {
    bool path = onPath(n), target = found && n == q;
    float reveal = (n == "rahul") ? 0.15f
                   : (n == "ananya" || n == "virat") ? 0.45f
                                                     : 0.72f;
    bool lit = p > reveal && path;
    return text(n) | bold |
           color(target && p > 0.8f ? C_OK : lit ? C_PROC : C_OFF);
  };
  Elements col;
  col.push_back(text("AVL User Index") | bold | color(C_PROC));
  col.push_back(text(""));
  col.push_back(hbox({text("        "), nd("rahul")}));
  col.push_back(text("       /     \\") | color(C_OFF));
  col.push_back(hbox({text("   "), nd("ananya"), text("   "), nd("virat")}));
  col.push_back(text("              /") | color(C_OFF));
  col.push_back(hbox({text("           "), nd("john")}));
  col.push_back(text(""));
  if (p > 0.82f)
    col.push_back(text(found ? "User Found" : "User Not Found") | bold |
                  color(found ? C_OK : C_ERR));
  return vbox(std::move(col));
}

inline Element IndexStageCollege(float p, long long, const CollegeCtx& cx) {
  const bool found = cx.matchIdx >= 0;
  std::string first = cx.query.substr(0, cx.query.find(' '));
  auto nd = [&](const std::string& n, float reveal, bool target) {
    bool lit = p > reveal;
    return text(n) | bold |
           color(target && p > 0.8f ? C_OK : (lit ? C_PROC : C_OFF));
  };
  Elements col;
  col.push_back(text("AVL Student Index") | bold | color(C_PROC));
  col.push_back(text(""));
  col.push_back(hbox({text("        "), nd("Karan", 0.15f, false)}));
  col.push_back(text("       /     \\") | color(C_OFF));
  col.push_back(hbox({text("   "), nd("Ananya", 0.45f, false), text("   "),
                      nd("Rahul", 0.45f, false)}));
  col.push_back(text("            /") | color(C_OFF));
  col.push_back(
      hbox({text("        "), nd(first.empty() ? "?" : first, 0.72f, true)}));
  col.push_back(text(""));
  if (p > 0.82f)
    col.push_back(text(found ? "Student Found" : "Student Not Found") | bold |
                  color(found ? C_OK : C_ERR));
  return vbox(std::move(col));
}

}
