// #include <ftxui/dom/elements.hpp>
// #include <ftxui/screen/screen.hpp>
// #include <iostream>
 
// int main(void) {
//   using namespace ftxui;
 
//   // Define the document
//   Element document =
//     hbox({
//       text("left")   | border,
//       text("middle") | border | flex,
//       text("right")  | border,
//     });
//   Element separate = 
//     border(
//       hbox({
//         text("Left"), 
//         separator(),
//         text("Right")
//         })
//       );
//     Element final = vbox({
//       document,
//       separate,
//       gauge(0.25) | color(Color::Red),
//       gauge(0.50) | color(Color::White),
//       gauge(0.75) | color(Color::Blue)
//     });
 
//   auto screen = Screen::Create(
//     Dimension::Full(),       // Width
//     Dimension::Fit(final) // Height
//   );
//   Render(screen, final);
//   screen.Print();
 
//   return EXIT_SUCCESS;
// }
#include <string>  // for string, allocator, basic_string
#include <vector>  // for vector
#include <iostream>
 
#include "ftxui/component/captured_mouse.hpp"      // for ftxui
#include "ftxui/component/component.hpp"           // for Radiobox
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
 
using namespace ftxui;
 
int main() {
  std::vector<std::string> radiobox_list = {
      "Use gcc",
      "Use clang",
      "Use emscripten",
      "Use tcc",
  };
  int selected = 0;

  auto radiobox = Radiobox(&radiobox_list, &selected);

  auto component = Renderer(radiobox, [&] {
    return vbox({
      // Ваш документ сверху
      hbox({
        text("left")   | border,
        text("middle") | border | flex,
        text("right")  | border,
      }),
      
      // Разделитель
      separator(),
      
      // Radiobox посередине  
      text("Select compiler:") | bold,
      radiobox->Render(),
      
      // Информация снизу
      separator(),
      text("You selected: " + radiobox_list[selected]) | color(Color::Red)
    });
  });

  auto screen = ScreenInteractive::TerminalOutput();
  screen.Loop(component);

  return 0;
}