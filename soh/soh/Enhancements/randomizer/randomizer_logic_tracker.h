#pragma once

#include <libultraship/libultraship.h>

class LogicTrackerWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;
    void DrawElement() override;

    static void ShowRandomizerCheck(RandomizerCheck check);

  protected:
    void InitElement() override;
    void UpdateElement() override;
};
