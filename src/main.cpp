
#include <algorithm>
#include <array>
#include <atomic>
#include <cctype>
#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/mouse.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include <ftxui/screen/box.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/screen/screen.hpp>

namespace ftxui {

inline Element dashedBorder(Element child, Color color, bool filled = false) {
  Element e = std::move(child) | borderStyled(DASHED, color);
  if (filled) e = e | bgcolor(color);
  return e;
}
inline Element dashedSep(Color c = Color::White) {
  return separatorDashed() | ftxui::color(c);
}
inline Element staticBox(std::string label, Color accent, bool filled,
                         int minw = 10) {
  Element c = text(std::move(label)) | bold | hcenter | vcenter |
              color(Color::White) | size(WIDTH, GREATER_THAN, minw) |
              size(HEIGHT, GREATER_THAN, 1);
  return dashedBorder(c, accent, filled);
}

class VArrowNode : public Node {
 public:
  explicit VArrowNode(Color c) : c_(c) {}
  void ComputeRequirement() override {
    requirement_.min_x = 1;
    requirement_.min_y = 1;
    requirement_.flex_grow_y = 1;
  }
  void Render(Screen& s) override {
    const int x = box_.x_min;
    for (int y = box_.y_min; y <= box_.y_max; ++y) {
      Pixel& p = s.PixelAt(x, y);
      p.character = (y == box_.y_max) ? "↓" : "│";
      p.foreground_color = c_;
    }
  }

 private:
  Color c_;
};
inline Element vArrow(Color c) { return std::make_shared<VArrowNode>(c); }

}

#include "ui.cpp"
#include "result_cache.cpp"
#include "Query_index.cpp"
#include "plan_sorter.cpp"
#include "join_map.cpp"
#include "fastest_path.cpp"
#include "resource_balancer.cpp"
#include "worker_pipeline.cpp"
#include "Execution_log.cpp"

namespace ftxui {

class DashedButtonBase : public ComponentBase {
 public:
  DashedButtonBase(std::string label, Color accent, int min_width,
                   std::function<void()> on_click, std::function<bool()> active)
      : label_(std::move(label)),
        accent_(accent),
        min_width_(min_width),
        on_click_(std::move(on_click)),
        active_(std::move(active)) {}

  Element Render() override {
    Element content = text(label_) | bold | hcenter | vcenter |
                      color(Color::White) |
                      size(WIDTH, GREATER_THAN, min_width_) |
                      size(HEIGHT, GREATER_THAN, 1);
    bool filled = hovered_ || pressed_ || (active_ && active_());
    return dashedBorder(content, accent_, filled) | reflect(box_);
  }
  bool OnEvent(Event event) override {
    if (!event.is_mouse()) return false;
    const Mouse& m = event.mouse();
    hovered_ = box_.Contain(m.x, m.y);
    if (hovered_ && m.button == Mouse::Left) {
      if (m.motion == Mouse::Pressed) {
        pressed_ = true;
        TakeFocus();
      } else if (m.motion == Mouse::Released && pressed_) {
        pressed_ = false;
        if (on_click_) on_click_();
      }
    }
    if (m.motion == Mouse::Released) pressed_ = false;
    return hovered_;
  }
  bool Focusable() const final { return true; }

 private:
  std::string label_;
  Color accent_;
  int min_width_;
  std::function<void()> on_click_;
  std::function<bool()> active_;
  Box box_;
  bool hovered_ = false;
  bool pressed_ = false;
};
inline Component DashedButton(std::string label, Color accent, int min_width,
                              std::function<void()> on_click,
                              std::function<bool()> active = nullptr) {
  return Make<DashedButtonBase>(std::move(label), accent, min_width,
                                std::move(on_click), std::move(active));
}

inline Element InstaSearchView(int s, float p, long long ms,
                               const InstaCtx& cx) {
  switch (s) {
    case 0: return RequestStage(p, ms, "Open Profile  @" + cx.query);
    case 1: return CacheStage(p, ms, "Query Cache");
    case 2: return IndexStageInsta(p, ms, cx);
    case 3:
      return PlansStage(p, {"Scan Entire User Database", "Use User Index",
                            "Use Cache + User Index"});
    case 4: return HeapStage(p, ms, "Use Cache + User Index");
    case 5: return GraphStage(p, "User", "Posts", "Followers");
    case 6: return DijkstraStage(p, ms, "User", "Posts", "Followers");
    case 7: return ResourceStage(p, ms);
    case 8: return QueueStage(p, ms);
    case 9: return StackStage(p, {"Load User", "Load Posts", "Load Followers"});
    default: return InstaResult(p, cx);
  }
}
inline Element CollegeSearchView(int s, float p, long long ms,
                                 const CollegeCtx& cx) {
  switch (s) {
    case 0: return RequestStage(p, ms, "Find Student:  " + cx.query);
    case 1: return CacheStage(p, ms, "Records Cache");
    case 2: return IndexStageCollege(p, ms, cx);
    case 3:
      return PlansStage(p, {"Scan All Student Records", "Use Student Index",
                            "Use Cache + Student Index"});
    case 4: return HeapStage(p, ms, "Use Cache + Student Index");
    case 5: return GraphStage(p, "Student", "Courses", "Results");
    case 6: return DijkstraStage(p, ms, "Student", "Courses", "Results");
    case 7: return ResourceStage(p, ms);
    case 8: return QueueStage(p, ms);
    case 9:
      return StackStage(p, {"Load Student", "Load Courses", "Load Results"});
    default: return CollegeResult(p, cx);
  }
}

}

using namespace ftxui;
using namespace std::chrono_literals;

int main() {
  auto screen = ScreenInteractive::Fullscreen();

  int screen_idx = 0;
  int sim_theme = 0;
  std::chrono::steady_clock::time_point run_start;
  const auto app_start = std::chrono::steady_clock::now();
  std::atomic<bool> app_alive{true};
  std::thread ticker;

  auto now_ms = [](std::chrono::steady_clock::time_point t) {
    return (long long)std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - t)
        .count();
  };

  std::string search_text;
  InstaCtx ictx;
  bool searched = false;
  auto run_search = [&] {
    if (search_text.empty()) return;
    ictx.query = search_text;
    ictx.matchIdx = InstaLookup(ictx.query);
    searched = true;
    run_start = std::chrono::steady_clock::now();
  };
  InputOption iopt;
  iopt.placeholder = "type a username e.g. virat";
  iopt.multiline = false;
  iopt.on_enter = [&] { run_search(); };
  auto search_input = Input(&search_text, iopt);

  std::string college_text;
  CollegeCtx cctx;
  bool csearched = false;
  auto run_college = [&] {
    if (college_text.empty()) return;
    cctx.query = college_text;
    cctx.matchIdx = CollegeLookup(college_text);
    csearched = true;
    run_start = std::chrono::steady_clock::now();
  };
  InputOption copt;
  copt.placeholder = "type a student name e.g. Raheesh";
  copt.multiline = false;
  copt.on_enter = [&] { run_college(); };
  auto college_input = Input(&college_text, copt);

  std::string status = "Hover a button. Press q to quit.";
  auto know_more =
      DashedButton(" KNOW MORE ", Color::Yellow, 18, [&] { screen_idx = 4; });
  auto simulate =
      DashedButton(" SIMULATE ", Color::Green, 18, [&] { screen_idx = 1; });
  auto rollback = DashedButton(" ROLLBACK SIMULATION ", Color::Red, 28, [&] {
    run_start = std::chrono::steady_clock::now();
    screen_idx = 5;
  });
  auto home_buttons = Container::Vertical({know_more, simulate, rollback});
  auto home = Renderer(home_buttons, [&] {
    Element page = vbox({
        QDataBanner(),
        text(""),
        text("WELCOME TO THE DATA QUERY OPTIMISER LAB") | bold | hcenter |
            color(Color::White),
        text(""),
        know_more->Render() | hcenter,
        dashedSep(),
        simulate->Render() | hcenter,
        dashedSep(),
        text("CHECK FOR FAILED QUERIES ROLLBACK") | bold | hcenter |
            color(Color::White),
        dashedSep(),
        rollback->Render() | hcenter,
        dashedSep(),
        text(status) | dim | hcenter | color(Color::White),
    });
    return dashedBorder(page, Color::White) | size(WIDTH, GREATER_THAN, 64) |
           center;
  });

  auto college = DashedButton(" CollegeDB ", Color::Magenta, 16,
                              [&] { sim_theme = 0; }, [&] { return sim_theme == 0; });
  auto insta = DashedButton(" InstaDB ", Color::Magenta, 16,
                            [&] { sim_theme = 1; }, [&] { return sim_theme == 1; });
  auto yes2 = DashedButton(" YES ", Color::Green, 16, [&] {
    if (sim_theme == 1) {
      searched = false;
      screen_idx = 2;
      search_input->TakeFocus();
    } else {
      csearched = false;
      screen_idx = 3;
      college_input->TakeFocus();
    }
  });
  auto no2 = DashedButton(" NO ", Color::Red, 16, [&] { screen_idx = 0; });
  auto sim_buttons = Container::Vertical({Container::Horizontal({college, insta}),
                                          Container::Horizontal({yes2, no2})});
  auto sim_confirm = Renderer(sim_buttons, [&] {
    Element page = vbox({
        hbox({text("  "), staticBox(" SIMULATE ", Color::Green, false, 16)}),
        dashedSep(),
        vbox({
            text(""),
            text("THIS SIMULATION SHOWS HOW A REAL DATABASE") | hcenter |
                color(Color::White),
            text("OPTIMISER HANDLES AN EVERYDAY REQUEST.") | hcenter |
                color(Color::White),
            text("FOR EDUCATIONAL UNDERSTANDING PURPOSE ONLY.") | hcenter |
                color(Color::White),
            text(""),
            text("CHOOSE A DATABASE TO SIMULATE:") | bold | hcenter |
                color(Color::White),
            text(""),
            hbox({college->Render(), text("    "), insta->Render()}) | hcenter,
            text(""),
            (sim_theme == 0
                 ? text("CollegeDB — CampusPulse student search") |
                       color(Color::GrayLight)
                 : text("InstaDB — Instagram profile search") |
                       color(Color::GrayLight)) |
                hcenter,
            text(""),
        }),
        dashedSep(),
        vbox({
            text(""),
            text("WOULD YOU LIKE TO CONTINUE ?") | bold | hcenter |
                color(Color::White),
            text(""),
            hbox({yes2->Render(), text("      "), no2->Render()}) | hcenter,
            text(""),
        }),
    });
    return dashedBorder(page, Color::White) | size(WIDTH, GREATER_THAN, 66) |
           center;
  });

  const int kLeftW = 40, kColLeftW = 46, kRightW = 18, kExplainH = 9;

  auto insta_page = Renderer(search_input, [&] {
    long long ms = searched ? now_ms(run_start) : 0;
    long long bms = now_ms(app_start);
    int stage = searched ? RunFrame(ms).stage : -1;
    float p = searched ? RunFrame(ms).p : 0.f;
    bool done = searched && stage >= kInstaStages - 1;

    Element leftInner = vbox({
        text("INSTADB OPTIMIZER") | bold | hcenter | color(Color::White),
        text(""),
        hbox({text(" Search: ") | color(C_PROC),
              search_input->Render() | color(C_OK) | underlined | flex}),
        text("try: virat · rahul · ananya · john") | hcenter | dim |
            color(Color::GrayLight),
        dashedSep(Color::GrayDark),
        (!searched ? vbox({filler(),
                           text("results will appear here") | hcenter |
                               color(Color::GrayLight),
                           filler()})
         : done    ? SimilarList(ictx)
                   : Loading(bms, "@" + ictx.query)) |
            flex,
    });
    Element left = dashedBorder(leftInner | flex, Color::White) |
                   size(WIDTH, EQUAL, kLeftW) | yflex;

    Element dsaInner =
        searched ? vbox({text("Step " + std::to_string(stage + 1) + "/" +
                              std::to_string(kInstaStages) + "  ·  " +
                              InstaLabel(stage)) |
                             color(Color::YellowLight),
                         dashedSep(Color::GrayDark),
                         InstaSearchView(stage, p, ms, ictx) | yflex})
                 : vbox({filler(),
                         text("DSA LIVE STRUCTURE SHOWING") | hcenter |
                             color(Color::White),
                         text("(animates as you search)") | hcenter |
                             color(Color::GrayLight),
                         filler()});
    Element dsaLive = dashedBorder(dsaInner | flex, Color::White) | flex;

    Element explInner =
        searched ? InstaExplain(stage, ictx, bms)
                 : vbox({filler(), hbox({filler(), Bunny(bms), filler()}),
                         text(""),
                         text("Enter name from the list to simulate") |
                             hcenter | color(Color::White),
                         text("the search query optimizer") | hcenter |
                             color(Color::GrayLight),
                         filler()});
    Element explain = dashedBorder(explInner | flex, Color::White) |
                      size(HEIGHT, EQUAL, kExplainH);
    Element middle = vbox({dsaLive, explain}) | flex | yflex;
    Element right =
        dashedBorder(DSListActive(searched ? RunDSActive(stage) : -1),
                     Color::White) |
        size(WIDTH, EQUAL, kRightW) | yflex;
    Element body = hbox({left, text(" "), middle, text(" "), right}) | yflex;
    return vbox({body, text("Esc = back to home   •   Enter = search") | dim |
                           hcenter | color(Color::White)}) |
           flex;
  });

  auto college_page = Renderer(college_input, [&] {
    long long ms = csearched ? now_ms(run_start) : 0;
    long long bms = now_ms(app_start);
    int stage = csearched ? RunFrame(ms).stage : -1;
    float p = csearched ? RunFrame(ms).p : 0.f;
    bool done = csearched && stage >= kInstaStages - 1;

    Element leftInner = vbox({
        text("CAMPUSPULSE OPTIMIZER") | bold | hcenter | color(Color::White),
        text(""),
        hbox({text(" Search: ") | color(C_PROC),
              college_input->Render() | color(C_OK) | underlined | flex}),
        dashedSep(Color::GrayDark),
        (!csearched ? CollegeList(-1)
         : done     ? CollegeOne(cctx.matchIdx)
                    : Loading(bms, cctx.query)) |
            flex,
    });
    Element left = dashedBorder(leftInner | flex, Color::White) |
                   size(WIDTH, EQUAL, kColLeftW) | yflex;

    Element dsaInner =
        csearched ? vbox({text("Step " + std::to_string(stage + 1) + "/" +
                              std::to_string(kInstaStages) + "  ·  " +
                              CollegeLabel(stage)) |
                              color(Color::YellowLight),
                          dashedSep(Color::GrayDark),
                          CollegeSearchView(stage, p, ms, cctx) | yflex})
                  : vbox({filler(),
                          text("DSA LIVE STRUCTURE SHOWING") | hcenter |
                              color(Color::White),
                          text("(animates as you search)") | hcenter |
                              color(Color::GrayLight),
                          filler()});
    Element dsaLive = dashedBorder(dsaInner | flex, Color::White) | flex;

    Element explInner =
        csearched ? CollegeExplain(stage, cctx, bms)
                  : vbox({filler(), hbox({filler(), Bunny(bms), filler()}),
                          text(""),
                          text("Enter name from the list to simulate") |
                              hcenter | color(Color::White),
                          text("the student search query optimizer") |
                              hcenter | color(Color::GrayLight),
                          filler()});
    Element explain = dashedBorder(explInner | flex, Color::White) |
                      size(HEIGHT, EQUAL, kExplainH);
    Element middle = vbox({dsaLive, explain}) | flex | yflex;
    Element right =
        dashedBorder(DSListActive(csearched ? RunDSActive(stage) : -1),
                     Color::White) |
        size(WIDTH, EQUAL, kRightW) | yflex;
    Element body = hbox({left, text(" "), middle, text(" "), right}) | yflex;
    return vbox({body, text("Esc = back to home   •   Enter = search") | dim |
                           hcenter | color(Color::White)}) |
           flex;
  });

  auto know_more_page = Renderer([&] { return KnowMore(); });

  auto rb_yes = DashedButton(" YES ", Color::Green, 14, [&] {
    run_start = std::chrono::steady_clock::now();
  });
  auto rb_no =
      DashedButton(" NO ", Color::Red, 14, [&] { screen_idx = 0; });
  auto rb_buttons = Container::Horizontal({rb_yes, rb_no});

  auto rollback_page = Renderer(rb_buttons, [&] {
    long long ms = now_ms(run_start);
    bool finished = ms >= kRbCycle;
    long long vms = std::min(ms, (long long)kRbCycle - 1);

    Element title = dashedBorder(
        vbox({text("ROLLBACK SIMULATION") | bold | hcenter | color(Color::White),
              text("how a failed query recovers — the execution log unwinds") |
                  hcenter | color(Color::GrayLight)}),
        Color::Red);
    Element left = dashedBorder(RollbackView(vms) | flex, Color::Red) | flex;
    Element right = dashedBorder(RollbackExplain(vms) | flex, Color::White) |
                    size(WIDTH, EQUAL, 46) | yflex;
    Element body = hbox({left, text(" "), right}) | yflex;

    Element foot =
        finished
            ? vbox({text("Watch the rollback again?") | bold | hcenter |
                        color(Color::White),
                    hbox({rb_yes->Render(), text("    "), rb_no->Render()}) |
                        hcenter})
            : (text("Esc = back to home   •   q = quit") | dim | hcenter |
               color(Color::White));

    if (!finished && RbPhase(vms) == 1)
      return vbox({title, RbBanner(vms), body, foot}) | flex;
    return vbox({title, body, foot}) | flex;
  });

  auto tabs = Container::Tab({home, sim_confirm, insta_page, college_page,
                              know_more_page, rollback_page},
                             &screen_idx);
  auto root = CatchEvent(tabs, [&](Event e) {
    if (e == Event::Character('q') && screen_idx != 2 && screen_idx != 3) {
      app_alive = false;
      screen.ExitLoopClosure()();
      return true;
    }
    if (e == Event::Escape) {
      if (screen_idx == 0) {
        app_alive = false;
        screen.ExitLoopClosure()();
      } else {
        screen_idx = 0;
      }
      return true;
    }
    return false;
  });

  ticker = std::thread([&] {
    while (app_alive) {
      screen.PostEvent(Event::Custom);
      std::this_thread::sleep_for(90ms);
    }
  });
  screen.Loop(root);
  app_alive = false;
  if (ticker.joinable()) ticker.join();
  return 0;
}
