namespace ftxui {

static const Color C_PROC = Color::BlueLight;
static const Color C_OK = Color::GreenLight;
static const Color C_DEC = Color::YellowLight;
static const Color C_ERR = Color::RedLight;
static const Color C_OFF = Color::GrayDark;

inline float clampf(float v, float lo, float hi) {
  return v < lo ? lo : (v > hi ? hi : v);
}
inline float seg(float p, float a, float b) {
  if (b <= a) return p >= b ? 1.f : 0.f;
  return clampf((p - a) / (b - a), 0.f, 1.f);
}
inline std::string spinner(long long ms) {
  static const char* f[] = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};
  return f[(ms / 110) % 10];
}
inline std::string lower_(std::string x) {
  for (char& c : x) c = (char)std::tolower((unsigned char)c);
  return x;
}

inline Element QDataBanner() {
  static const std::vector<std::array<std::string, 6>> g = {
      {{" ████ ", "█    █", "█    █", "█  ▄ █", "█   ██", " █████"}},
      {{"█████ ", "█    █", "█    █", "█    █", "█    █", "█████ "}},
      {{" ████ ", "█    █", "█    █", "██████", "█    █", "█    █"}},
      {{"██████", "  ██  ", "  ██  ", "  ██  ", "  ██  ", "  ██  "}},
      {{" ████ ", "█    █", "█    █", "██████", "█    █", "█    █"}},
  };
  Elements rows;
  for (int r = 0; r < 6; ++r) {
    std::string line;
    for (size_t i = 0; i < g.size(); ++i) {
      if (i) line += " ";
      line += g[i][r];
    }
    rows.push_back(text(line));
  }
  return vbox(std::move(rows)) | bold | color(Color::CyanLight) | hcenter;
}

inline Element Bunny(long long ms) {
  const bool blink = (ms % 2600) < 220;
  const bool hand = (ms / 850) % 2 == 0;
  const std::string eyes = blink ? "( -. - )" : "( 0. 0 )";
  const std::string slot = hand ? "o/ " : "   ";
  return vbox({
             text("   (\\  /)"),
             text(slot + eyes),
             text("   (\")  (\")O"),
         }) |
         color(Color::RGB(247, 168, 190)) | bold;
}

inline Element Loading(long long ms, const std::string& what) {
  std::string dots((int)((ms / 400) % 4), '.');
  return vbox({
      filler(),
      text(spinner(ms) + "  Loading" + dots) | bold | color(C_PROC) | hcenter,
      text(""),
      text("fetching  " + what) | color(Color::GrayLight) | hcenter,
      filler(),
  });
}

inline Element DSListActive(int active) {
  static const std::vector<std::string> names = {
      "AVL TREE", "STACK", "QUEUE", "HEAP", "GRAPH", "DIJKSTRA"};
  Elements items;
  for (int i = 0; i < (int)names.size(); ++i) {
    const bool on = (i == active);
    items.push_back(
        staticBox(names[i], on ? C_OK : Color::White, on, 12) | hcenter);
    if (i + 1 < (int)names.size())
      items.push_back(vArrow(active >= 0 && i < active ? C_OK : C_OFF) |
                      hcenter | yflex);
  }
  return vbox(std::move(items));
}

constexpr int kInstaStages = 11;
struct Seg {
  const char* label;
  int ms;
};
struct Frame {
  int stage;
  float p;
};
inline const Seg* RunTimeline() {
  static const Seg t[kInstaStages] = {
      {"Request received", 1800}, {"Cache lookup", 2000},
      {"AVL index search", 2400}, {"Plan generation", 2200},
      {"Heap selection", 1900},   {"Relationship graph", 2200},
      {"Dijkstra route", 2300},   {"Resource balancing", 2000},
      {"Worker queue", 1800},     {"Execution stack", 2100},
      {"Result", 2400}};
  return t;
}
inline int RunTotal() {
  int t = 0;
  for (int i = 0; i < kInstaStages; ++i) t += RunTimeline()[i].ms;
  return t;
}
inline Frame RunFrame(long long ms) {
  long long acc = 0;
  for (int i = 0; i < kInstaStages; ++i) {
    int d = RunTimeline()[i].ms;
    if (ms < acc + d || i == kInstaStages - 1)
      return {i, clampf(d > 0 ? float(ms - acc) / d : 1.f, 0.f, 1.f)};
    acc += d;
  }
  return {kInstaStages - 1, 1.f};
}
inline const char* InstaLabel(int s) {
  static const char* L[kInstaStages] = {
      "Request received", "Cache lookup",       "AVL user index",
      "Plan generation",  "Heap selection",     "Relationship graph",
      "Dijkstra route",   "Resource balancing", "Worker queue",
      "Execution stack",  "Profile loaded"};
  return (s >= 0 && s < kInstaStages) ? L[s] : "";
}
inline const char* CollegeLabel(int s) {
  static const char* L[kInstaStages] = {
      "Request received", "Records cache",      "AVL student index",
      "Plan generation",  "Heap selection",     "Relationship graph",
      "Dijkstra route",   "Resource balancing", "Worker queue",
      "Execution stack",  "Student loaded"};
  return (s >= 0 && s < kInstaStages) ? L[s] : "";
}
inline int RunDSActive(int s) {
  switch (s) {
    case 2: return 0;
    case 4: return 3;
    case 5: return 4;
    case 6: return 5;
    case 8: return 2;
    case 9: return 1;
    default: return -1;
  }
}

struct InstaUser {
  std::string name;
  long long followers;
};
inline const std::vector<InstaUser>& InstaUsers() {
  static const std::vector<InstaUser> u = {{"virat", 270000000},
                                           {"rahul", 5000},
                                           {"ananya", 12000},
                                           {"john", 8000}};
  return u;
}
inline int InstaLookup(const std::string& q) {
  std::string s;
  for (char c : q)
    if (c != ' ' && c != '@') s += (char)std::tolower((unsigned char)c);
  const auto& u = InstaUsers();
  for (int i = 0; i < (int)u.size(); ++i)
    if (u[i].name == s) return i;
  return -1;
}
struct InstaCtx {
  std::string query = "virat";
  int matchIdx = 0;
};
inline std::string human(long long n) {
  if (n >= 1000000000) return std::to_string(n / 1000000000) + "B";
  if (n >= 1000000) return std::to_string(n / 1000000) + "M";
  if (n >= 1000) return std::to_string(n / 1000) + "K";
  return std::to_string(n);
}
inline Element SimilarList(const InstaCtx& cx) {
  const std::string& q = cx.query;
  const bool found = cx.matchIdx >= 0;
  struct H {
    std::string name, fol;
    bool top;
  };
  std::vector<H> hs = {
      {"@" + q, found ? human(InstaUsers()[cx.matchIdx].followers) : "—", found},
      {"@" + q + ".official", "2.1M", false},
      {"@" + q + "_", "540K", false},
      {"@the." + q, "88K", false},
      {"@" + q + "_fan", "12K", false},
      {"@" + q + "4u", "3.4K", false},
  };
  Elements rows;
  rows.push_back(text("  Similar accounts") | bold | color(C_PROC));
  rows.push_back(dashedSep(C_OFF));
  for (auto& h : hs) {
    rows.push_back(hbox({
        text(h.top ? " ● " : " ○ ") | color(h.top ? C_OK : C_OFF),
        text(h.name) | color(Color::White) | flex,
        text(h.fol + " ") | color(Color::GrayLight),
        text(h.top && found ? "✓" : " ") | color(C_PROC),
    }));
    rows.push_back(text(""));
  }
  return vbox(std::move(rows));
}

struct Student {
  std::string name, roll, cohort;
};
inline const std::vector<Student>& CollegeStudents() {
  static const std::vector<Student> s = {
      {"Raheesh Khatri", "159032239099", "JB"},
      {"Ananya Bose", "159032239100", "JB"},
      {"Rahul Verma", "159032239101", "KP"},
      {"Sneha Iyer", "159032239102", "KP"},
      {"Arjun Mehta", "159032239103", "LR"},
      {"Priya Nair", "159032239104", "LR"},
      {"Karan Singh", "159032239105", "MN"},
      {"Diya Sharma", "159032239106", "MN"},
      {"Vivaan Joshi", "159032239107", "OP"},
      {"Isha Reddy", "159032239108", "OP"},
      {"Rohan Gupta", "159032239109", "QR"},
      {"Meera Pillai", "159032239110", "QR"},
      {"Rahul Khatri", "159032239111", "ST"},
      {"Ananya Verma", "159032239112", "ST"},
      {"Karan Bose", "159032239113", "UV"},
      {"Priya Sharma", "159032239114", "UV"},
      {"Arjun Nair", "159032239115", "JB"},
      {"Sneha Singh", "159032239116", "KP"},
      {"Rohan Iyer", "159032239117", "LR"},
      {"Diya Mehta", "159032239118", "MN"},
      {"Vivaan Reddy", "159032239119", "OP"},
      {"Isha Joshi", "159032239120", "QR"},
      {"Meera Gupta", "159032239121", "ST"},
  };
  return s;
}
inline int CollegeLookup(const std::string& q) {
  std::string s = lower_(q);
  while (!s.empty() && s.front() == ' ') s.erase(s.begin());
  while (!s.empty() && s.back() == ' ') s.pop_back();
  if (s.empty()) return -1;
  const auto& st = CollegeStudents();
  for (int i = 0; i < (int)st.size(); ++i) {
    std::string full = lower_(st[i].name);
    if (full == s) return i;
    size_t start = 0;
    while (start <= full.size()) {
      size_t sp = full.find(' ', start);
      std::string tok =
          full.substr(start, sp == std::string::npos ? sp : sp - start);
      if (tok == s) return i;
      if (sp == std::string::npos) break;
      start = sp + 1;
    }
  }
  return -1;
}
struct CollegeCtx {
  std::string query = "Raheesh Khatri";
  int matchIdx = 0;
};
inline Element CollegeList(int activeIdx) {
  const auto& st = CollegeStudents();
  Elements rows;
  rows.push_back(text(" Student records") | bold | color(C_PROC));
  rows.push_back(dashedSep(C_OFF));
  for (int i = 0; i < (int)st.size(); ++i) {
    const bool top = (i == activeIdx);
    rows.push_back(hbox({
        text(top ? " ● " : " ○ ") | color(top ? C_OK : C_OFF),
        text(st[i].name) | color(top ? C_OK : Color::White) | flex,
        text(st[i].roll + " ") | color(Color::GrayLight),
        text(st[i].cohort) | color(C_DEC),
    }));
  }
  return vbox(std::move(rows));
}
inline Element CollegeOne(int idx) {
  Elements rows;
  rows.push_back(text(" Result") | bold | color(C_OK));
  rows.push_back(dashedSep(C_OFF));
  rows.push_back(text(""));
  if (idx < 0) {
    rows.push_back(text(" no matching student") | color(C_ERR));
  } else {
    const Student& s = CollegeStudents()[idx];
    rows.push_back(
        hbox({text(" ● ") | color(C_OK), text(s.name) | bold | color(C_OK)}));
    rows.push_back(text(""));
    rows.push_back(text("   Roll    " + s.roll) | color(Color::White));
    rows.push_back(text("   Cohort  " + s.cohort) | color(Color::White));
  }
  return vbox(std::move(rows));
}

inline Element RequestStage(float p, long long ms, const std::string& line) {
  (void)ms;
  int gap = (int)((1.f - seg(p, 0.f, 0.6f)) * 3);
  Elements col;
  col.push_back(text("Incoming Request") | bold | color(C_PROC));
  for (int i = 0; i < gap; ++i) col.push_back(text(""));
  col.push_back(dashedBorder(text(line) | hcenter, C_PROC) | hcenter);
  col.push_back(text(""));
  if (p > 0.6f)
    col.push_back(text("◉  request packet released") | color(C_OK) | hcenter);
  return vbox(std::move(col));
}
inline Element InstaResult(float p, const InstaCtx& cx) {
  const bool found = cx.matchIdx >= 0;
  long long fol = found ? InstaUsers()[cx.matchIdx].followers : 0;
  std::vector<std::string> lines = {
      " Username  : " + cx.query,
      " Followers : " + (found ? std::to_string(fol) : std::string("—")),
      " Cache     : MISS", " Best Plan : Plan C", " Exec Cost : 10"};
  int reveal = (int)(seg(p, 0.2f, 0.9f) * lines.size() + 1e-4f);
  Elements rows;
  rows.push_back(text(found ? "PROFILE LOADED" : "PROFILE NOT FOUND") | bold |
                 hcenter | color(found ? C_OK : C_ERR));
  rows.push_back(dashedSep(found ? C_OK : C_ERR));
  for (int i = 0; i < (int)lines.size() && i < reveal; ++i)
    rows.push_back(text(lines[i]) | color(Color::White));
  Element card = dashedBorder(vbox(std::move(rows)) | size(WIDTH, GREATER_THAN, 28),
                              found ? C_OK : C_ERR);
  return vbox({text(found ? "✓  PROFILE LOADED" : "✗  PROFILE NOT FOUND") |
                   bold | color(found ? C_OK : C_ERR) | hcenter,
               text(""), card | hcenter}) |
         vcenter;
}
inline Element CollegeResult(float p, const CollegeCtx& cx) {
  const bool found = cx.matchIdx >= 0;
  Elements rows;
  rows.push_back(text(found ? "STUDENT FOUND" : "STUDENT NOT FOUND") | bold |
                 hcenter | color(found ? C_OK : C_ERR));
  rows.push_back(dashedSep(found ? C_OK : C_ERR));
  if (found) {
    const Student& st = CollegeStudents()[cx.matchIdx];
    std::vector<std::string> lines = {
        " Name   : " + st.name,  " Roll   : " + st.roll,
        " Cohort : " + st.cohort, " Cache  : MISS",
        " Plan   : Plan C",       " Cost   : 10"};
    int reveal = (int)(seg(p, 0.2f, 0.9f) * lines.size() + 1e-4f);
    for (int i = 0; i < (int)lines.size() && i < reveal; ++i)
      rows.push_back(text(lines[i]) | color(Color::White));
  } else {
    rows.push_back(text(" '" + cx.query + "' is not in the records") |
                   color(Color::White));
  }
  Element card = dashedBorder(vbox(std::move(rows)) | size(WIDTH, GREATER_THAN, 28),
                              found ? C_OK : C_ERR);
  return vbox({text(found ? "✓  STUDENT FOUND" : "✗  STUDENT NOT FOUND") | bold |
                   color(found ? C_OK : C_ERR) | hcenter,
               text(""), card | hcenter}) |
         vcenter;
}

inline Element ExplainBox(const char* msg, long long ms) {
  Element guide = hbox({
      vbox({text("WHAT'S HAPPENING") | bold | color(C_DEC),
            text("here's what just happened") | color(Color::GrayLight)}),
      filler(),
      Bunny(ms),
  });
  return vbox({guide, text(""), paragraph(msg) | color(Color::White)});
}
inline Element InstaExplain(int s, const InstaCtx&, long long ms) {
  static const char* M[kInstaStages] = {
      "You searched a profile. Instagram turns this into a query packet and "
      "sends it into the optimizer.",
      "First it checks: was this profile loaded recently? The cache has no "
      "copy — CACHE MISS — so it must fetch fresh.",
      "Usernames are kept in a balanced AVL tree. Instead of scanning every "
      "user, it walks the tree in O(log n) steps.",
      "It drafts ways to fetch the data, each with a cost: scan all (120), use "
      "the index (40), or cache + index (10).",
      "A priority queue (min-heap) instantly surfaces the cheapest plan — Plan "
      "C, cost 10.",
      "Loading a profile needs related data. A graph links User to Posts and "
      "User to Followers.",
      "Edges carry costs. Dijkstra finds the cheapest retrieval route: User -> "
      "Posts -> Followers, total 3.",
      "Servers carry different loads. The resource balancer routes this query "
      "to the least-busy server.",
      "Background jobs (index rebuilds, stats) wait in a FIFO queue so they "
      "never block your request.",
      "Each load is pushed onto an execution stack so the work can roll back "
      "if a step fails. TOP = Load Followers.",
      "The profile is assembled and returned. Every required structure ran: "
      "cache, AVL, heap, graph, Dijkstra, queue, balancer and the stack.",
  };
  return ExplainBox((s >= 0 && s < kInstaStages) ? M[s] : "", ms);
}
inline Element CollegeExplain(int s, const CollegeCtx&, long long ms) {
  static const char* M[kInstaStages] = {
      "You searched a student. CampusPulse turns this into a query and sends "
      "it into the optimizer.",
      "First it checks: was this record loaded recently? The cache has no copy "
      "— CACHE MISS — so it must fetch fresh.",
      "Student names are kept in a balanced AVL tree. Instead of scanning every "
      "record, it walks the tree in O(log n) steps.",
      "It drafts ways to fetch the record, each with a cost: scan all (120), "
      "use the index (40), or cache + index (10).",
      "A priority queue (min-heap) instantly surfaces the cheapest plan — Plan "
      "C, cost 10.",
      "A student links to related tables. A graph connects Student to Courses "
      "and Student to Results.",
      "Edges carry costs. Dijkstra finds the cheapest join route: Student -> "
      "Courses -> Results, total 3.",
      "Servers carry different loads. The resource balancer routes this query "
      "to the least-busy server.",
      "Maintenance jobs (index rebuilds, stats) wait in a FIFO queue so they "
      "never block your request.",
      "Each load is pushed onto an execution stack so the work can roll back "
      "if a step fails. TOP = Load Results.",
      "The record is assembled and returned. Every required structure ran: "
      "cache, AVL, heap, graph, Dijkstra, queue, balancer and the stack.",
  };
  return ExplainBox((s >= 0 && s < kInstaStages) ? M[s] : "", ms);
}

inline Element KnowMore() {
  auto head = [](std::string t) { return text(t) | bold | color(C_DEC); };
  auto btn = [](std::string name) {
    return text(" • " + name) | bold | color(C_PROC);
  };
  auto para = [](std::string t) {
    return paragraph(t) | color(Color::White);
  };
  auto qa = [](std::string n, std::string question, std::string answer) {
    return vbox({paragraph(" " + n + "  " + question) | bold |
                     color(Color::White),
                 text("      → " + answer) | color(C_OK), text("")});
  };

  Element leftCol = vbox({
      head(" WHAT IS THIS?"),
      para("A terminal visualiser that shows how a database query optimiser "
           "thinks. It turns a simple search into an animated walk through the "
           "core data structures from the case study, narrated in plain "
           "English by a friendly guide."),
      text(""),
      head(" THE BUTTONS"),
      text(""),
      btn("KNOW MORE"),
      para("Opens this page — the project overview, what each button does, and "
           "how every case-study requirement is met."),
      text(""),
      btn("SIMULATE"),
      para("Runs a live, animated query-optimiser. First you pick which "
           "database to model:"),
      text("     · CollegeDB — find a student by name / surname") |
          color(Color::White),
      text("     · InstaDB   — find a social profile") |
          color(Color::White),
      para("The middle panel shows the live data structures; the bunny narrates "
           "each step; the right rail tracks which structure is active."),
      text(""),
      btn("ROLLBACK SIMULATION"),
      para("(coming soon) Replays a query that fails midway and unwinds its "
           "execution stack one operation at a time, restoring the last stable "
           "state."),
  });

  Element rightCol = vbox({
      head(" NOTE:-"),
      text(""),
      qa("1.", "How was information indexed for fast retrieval?",
         "AVL Tree  —  Query Index  (O(log n))"),
      qa("2.", "How was execution history kept for rollback?",
         "Stack  —  Execution Log"),
      qa("3.", "How were requests processed in arrival order?",
         "FIFO Queue  —  Worker Pipeline"),
      qa("4.", "How were table relationships represented?",
         "Graph  —  Join Map"),
      qa("5.", "How was the most efficient path determined?",
         "Dijkstra  —  Fastest Path"),
      para("Also: Result Cache = hash map · Plan Sorter = min-heap · "
           "Resource Balancer = greedy load allocation."),
  });

  Element page = vbox({
      QDataBanner(),
      text("DATA QUERY EXECUTION OPTIMISER LAB") | bold | hcenter |
          color(Color::CyanLight),
      dashedSep(Color::Yellow),
      hbox({leftCol | flex, text("   "), rightCol | flex}),
      dashedSep(Color::Yellow),
      text("Esc = back to home") | dim | hcenter | color(Color::White),
  });
  return dashedBorder(page, Color::Yellow) | size(WIDTH, GREATER_THAN, 104) |
         center;
}

}
